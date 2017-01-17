#include "SchottkyEmissionBC.h"

template<>
InputParameters validParams<SchottkyEmissionBC>()
{
	InputParameters params = validParams<IntegratedBC>();
	params.addRequiredParam<Real>("r", "The reflection coefficient");
	params.addRequiredCoupledVar("potential","The electric potential");
	params.addRequiredCoupledVar("mean_en", "The mean energy.");
	params.addRequiredCoupledVar("ip", "The ion density.");
	params.addParam<Real>("position_units", 1, "The units of position.");
	params.addParam<Real>("time_units", 1, "The units of time.");
	params.addRequiredParam<std::string>("potential_units", "The potential units.");
	params.addRequiredParam<bool>("use_moles", "Whether to use units of moles as opposed to # of molecules.");
	params.addParam<Real>("tau", 1e-9, "The time constant for ramping the boundary condition.");
	params.addParam<bool>("relax", false, "Use relaxation for emission.");
	return params;
}

SchottkyEmissionBC::SchottkyEmissionBC(const InputParameters & parameters) :
	IntegratedBC(parameters),

	_use_moles(getParam<bool>("use_moles")),
	_r_units(1. / getParam<Real>("position_units")),
	_t_units(1. / getParam<Real>("time_units")),
	_r(getParam<Real>("r")),

// Coupled Variables
	_grad_potential(coupledGradient("potential")),
	_potential_id(coupled("potential")),
	
	_mean_en(coupledValue("mean_en")),
	_mean_en_id(coupled("mean_en")),
	_ip_var(*getVar("ip",0)),
	_ip(coupledValue("ip")),
	_grad_ip(coupledGradient("ip")),
	_ip_id(coupled("ip")),

	_muem(getMaterialProperty<Real>("muem")),
	_d_muem_d_actual_mean_en(getMaterialProperty<Real>("d_muem_d_actual_mean_en")),
	_massem(getMaterialProperty<Real>("massem")),

	_e(getMaterialProperty<Real>("e")),
	_eps(getMaterialProperty<Real>("eps")),
	_N_A(getMaterialProperty<Real>("N_A")),

	_sgnip(getMaterialProperty<Real>("sgn" + _ip_var.name())),
	_muip(getMaterialProperty<Real>("mu" + _ip_var.name())),
	_Dip(getMaterialProperty<Real>("diff" + _ip_var.name())),
	_se_coeff(getMaterialProperty<Real>("se_coeff")),
	_se_energy(getMaterialProperty<Real>("se_energy")),
	
	_work_function(getMaterialProperty<Real>("work_function")),
	_field_enhancement(getMaterialProperty<Real>("field_enhancement")),
	_Richardson_coefficient(getMaterialProperty<Real>("Richardson_coefficient")),
	_cathode_temperature(getMaterialProperty<Real>("cathode_temperature")),

	_a(0.5),
	_v_thermal(0),
	_ion_flux(0, 0, 0),
	_n_emitted(0),
	_d_v_thermal_d_u(0),
	_d_v_thermal_d_mean_en(0),
	_d_ion_flux_d_potential(0, 0, 0),
	_d_ion_flux_d_ip(0, 0, 0),
	_d_n_emitted_d_potential(0),
	_d_n_emitted_d_ip(0),
	_d_n_emitted_d_u(0),
	_d_n_emitted_d_mean_en(0),
	_actual_mean_en(0),

	_tau(getParam<Real>("tau")),
	_relax(getParam<bool>("relax")),
	_potential_units(getParam<std::string>("potential_units"))

{
	if (_potential_units.compare("V") == 0) {
		_voltage_scaling = 1.;
	} else if (_potential_units.compare("kV") == 0) {
		_voltage_scaling = 1000;
	}

	_dPhi_over_F = 0.00003794686 ; // sqrt(q / (4*pi*E_0) [eV / ( V / m )]
}

Real
SchottkyEmissionBC::computeQpResidual()
{
	Real _EmissionFlux;
	Real _relaxation_expr;

	_EmissionFlux = SchottkyEmissionBC::EmissionFlux() ;
	_relaxation_expr = _relax ? std::tanh(_t / _tau) : 1.0 ;

	return -_relaxation_expr * _test[_i][_qp] * 2. / (1. + _r) * (1 - _a) * _EmissionFlux ; 

}

Real
SchottkyEmissionBC::computeQpJacobian()
{
	return 0.;
}

Real
SchottkyEmissionBC::computeQpOffDiagJacobian(unsigned int jvar)
{
	Real _d_EmissionFlux_d_offDiag(0);
	Real _relaxation_expr;
	
	_relaxation_expr = _relax ? std::tanh(_t / _tau) : 1.0 ;

	if (jvar == _potential_id) {
		_d_EmissionFlux_d_offDiag = SchottkyEmissionBC::d_EmissionFlux_d_potential() ;		
	} else if (jvar == _mean_en_id) {
		_d_EmissionFlux_d_offDiag = SchottkyEmissionBC::d_EmissionFlux_d_mean_en() ;		
	} else if (jvar == _ip_id) {
		_d_EmissionFlux_d_offDiag = SchottkyEmissionBC::d_EmissionFlux_d_ip() ;	
	}
	
	return -_relaxation_expr * _test[_i][_qp] * 2. / (1. + _r) * (1 - _a) * _d_EmissionFlux_d_offDiag ; 
}

Real 
SchottkyEmissionBC::EmissionFlux()
{
	Real dPhi;
	Real kB = 8.617385E-5; // eV/K;
	Real _j_TE;
	Real _j_SE;
	Real F;
	
	if ( _normals[_qp] * -1.0 * -_grad_potential[_qp] > 0.0) {
			_a = 1.0;
			return 0;
	} else {
		_a = 0.0;
	}
	
	_ion_flux = (_sgnip[_qp] * _muip[_qp] * -_grad_potential[_qp] * std::exp(_ip[_qp]) - _Dip[_qp] * std::exp(_ip[_qp]) * _grad_ip[_qp]) * ( _use_moles ? _N_A[_qp] : 1 );

	// Schottky emission
		// je = AR * T^2 * exp(-(wf - dPhi) / (kB * T))
		// dPhi = _dPhi_over_F * sqrt(F) // eV

	F = -(1 - _a) * _field_enhancement[_qp] * _normals[_qp] * _grad_potential[_qp] * _r_units * _voltage_scaling;

	dPhi = _dPhi_over_F * sqrt(F);

	_j_TE = _Richardson_coefficient[_qp] * std::pow(_cathode_temperature[_qp], 2) * std::exp(-(_work_function[_qp] - dPhi) / (kB * _cathode_temperature[_qp]));
	_j_SE = _e[_qp] * _se_coeff[_qp] * _ion_flux * _normals[_qp] ;
	
	return ( _j_TE + _j_SE ) / (_e[_qp] * ( _use_moles ? _N_A[_qp] : 1 ) ) ;
}

Real 
SchottkyEmissionBC::d_EmissionFlux_d_potential()
{
	Real dPhi;
	Real kB = 8.617385E-5; // eV/K;
	Real _j_TE;
	Real _d_j_TE_d_potential;
	Real _d_j_SE_d_potential;
	Real F;
	
	if ( _normals[_qp] * -1.0 * -_grad_potential[_qp] > 0.0) {
		_a = 1.0;
		return 0.0 ;
	} else {
		_a = 0.0;
	}

	_ion_flux = (_sgnip[_qp] * _muip[_qp] * -_grad_potential[_qp] * std::exp(_ip[_qp]) - _Dip[_qp] * std::exp(_ip[_qp]) * _grad_ip[_qp]) * ( _use_moles ? _N_A[_qp] : 1 );
	_d_ion_flux_d_potential = _sgnip[_qp] * _muip[_qp] * -_grad_phi[_j][_qp] * std::exp(_ip[_qp]) ;
	_d_j_SE_d_potential = _e[_qp] * _se_coeff[_qp] * _d_ion_flux_d_potential * _normals[_qp] ;
	
	// Schottky emission
	// je = AR * T^2 * exp(-(wf - dPhi) / (kB * T))
	// dPhi = _dPhi_over_F * sqrt(F) // eV

	F = -(1 - _a) * _field_enhancement[_qp] * _normals[_qp] * _grad_potential[_qp];

	dPhi = _dPhi_over_F * sqrt(F);

	_j_TE = _Richardson_coefficient[_qp] * std::pow(_cathode_temperature[_qp], 2) * exp(-(_work_function[_qp] - dPhi) / (kB * _cathode_temperature[_qp]));

	_d_j_TE_d_potential = _j_TE * (dPhi / (2 * kB * _cathode_temperature[_qp])) * 
						(_grad_phi[_j][_qp] * _normals[_qp] ) / (_grad_potential[_qp] * _normals[_qp] );
	
	return ( _d_j_TE_d_potential + _d_j_SE_d_potential ) / (_e[_qp] * ( _use_moles ? _N_A[_qp] : 1 ) ) ;
}

Real 
SchottkyEmissionBC::d_EmissionFlux_d_em()
{
	return 0.0;
}

Real 
SchottkyEmissionBC::d_EmissionFlux_d_mean_en()
{
	return 0.0;
}

Real 
SchottkyEmissionBC::d_EmissionFlux_d_ip()
{
	return 0.0;
}
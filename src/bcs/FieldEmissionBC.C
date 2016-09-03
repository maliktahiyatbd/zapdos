#include "FieldEmissionBC.h"

template<>
InputParameters validParams<FieldEmissionBC>()
{
  InputParameters params = validParams<IntegratedBC>();
  params.addRequiredParam<Real>("r", "The reflection coefficient");
  params.addRequiredCoupledVar("potential","The electric potential");
  params.addRequiredParam<Real>("position_units", "Units of position.");
  return params;
}

FieldEmissionBC::FieldEmissionBC(const InputParameters & parameters) :
    IntegratedBC(parameters),

    _r_units(1. / getParam<Real>("position_units")),
    _r(getParam<Real>("r")),

// Coupled Variables
    _grad_potential(coupledGradient("potential")),
    _potential_id(coupled("potential")),

    _e(getMaterialProperty<Real>("e")),
    _work_function(getMaterialProperty<Real>("work_function")),
    _field_enhancement(getMaterialProperty<Real>("field_enhancement"))
{}

Real
FieldEmissionBC::computeQpResidual()
{
  Real a;
  Real b;
  Real v;
  Real f;
  Real je;
  Real F;

  // Fowler-Nordheim
  // je = (a / wf) * F^2 * exp(-v(f) * b * wf^1.5 / F)
  // a = 1.541434E-6 A eV/V^2
  // b = 6.830890E9 V/m-eV^1.5
  // v(f) = 1 - f + (f/6)*ln(f)
  // f = (1.439964E9 eV^2 m/V)*(F/wf^2)

  F = _field_enhancement[_qp] * _normals[_qp] * -_grad_potential[_qp] * _r_units;

  a = 1.541434E-6; // A eV/V^2
  b = 6.830890E9; // V/m-eV^1.5

  f = (1.439964E9)*(F / pow(_work_function[_qp], 2) );
  v = 1 - f + (f/6)*std::log(f);

  je = (a / (_work_function[_qp])) * pow( F , 2) * std::exp(v * b * pow(_work_function[_qp], 1.5) / F);

  return _test[_i][_qp] * (je / _e[_qp]);
}

Real
FieldEmissionBC::computeQpJacobian()
{
  return 0.;
}

Real
FieldEmissionBC::computeQpOffDiagJacobian(unsigned int jvar)
{
  Real a;
  Real b;
  Real v;
  Real f;
  Real je;
  Real F;
  Real _E_Flux;
  Real _dv_dF;


  // Fowler-Nordheim
  // je = (a / wf) * F^2 * exp(-v(f) * b * wf^1.5 / F)
  // a = 1.541434E-6 A eV/V^2
  // b = 6.830890E9 V/m-eV^1.5
  // v(f) = 1 - f + (f/6)*ln(f)
  // f = (1.439964E9 eV^2 m/V)*(F/wf^2)

  F = _field_enhancement[_qp] * _normals[_qp] * -_grad_potential[_qp] * _r_units;

  a = 1.541434E-6; // A eV/V^2
  b = 6.830890E9; // V/m-eV^1.5

  f = (1.439964E9)*(F / pow(_work_function[_qp], 2) );
  v = 1 - f + (f/6)*std::log(f);

  je = (a / (_work_function[_qp])) * pow( F , 2) * std::exp(v * b * pow(_work_function[_qp], 1.5) / F);
  _E_Flux = je / _e[_qp];

  if (jvar == _potential_id)
  {
    _dv_dF = (-(5.0/6.0) + (1.0/6.0) * std::log(f)) * (1.439964E9 / pow(_work_function[_qp], 2) ) ;

    return - _test[_i][_qp] * ( (2.0 / F) - ( b * pow(_work_function[_qp], 1.5) * v / pow(F,2) ) + ( b * pow(_work_function[_qp], 1.5) * _dv_dF / F ) ) * ( _E_Flux ) * (-_grad_phi[_j][_qp] * _normals[_qp]) * _r_units ;
  }

  else
    return 0.0;
}

#ifndef SCHOTTKYEMISSIONENERGYBC_H
#define SCHOTTKYEMISSIONENERGYBC_H

#include "SchottkyEmissionBC.h"

class SchottkyEmissionEnergyBC;

template<>
InputParameters validParams<SchottkyEmissionEnergyBC>();

class SchottkyEmissionEnergyBC : public SchottkyEmissionBC
{
public:
	SchottkyEmissionEnergyBC(const InputParameters & parameters);

protected:
	virtual Real computeQpResidual();
	virtual Real computeQpJacobian();
	virtual Real computeQpOffDiagJacobian(unsigned int jvar);
	
	//Variables
	unsigned int _em_id;
	const VariableValue & _em;

	//System variables
	Real _v_drift;
	Real _d_v_drift_d_u;
	
	Real _emission_flux;
	Real _d_emission_flux_d_u;
	
	Real _n_emitted;
	Real _d_n_emitted_d_u;
	
	Real _n_e_transported;
	Real _d_n_e_transported_d_u;
};


#endif /* SCHOTTKYEMISSIONENERGYBC_H */

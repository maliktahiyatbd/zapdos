/****************************************************************/
/*               DO NOT MODIFY THIS HEADER                      */
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*           (c) 2010 Battelle Energy Alliance, LLC             */
/*                   ALL RIGHTS RESERVED                        */
/*                                                              */
/*          Prepared by Battelle Energy Alliance, LLC           */
/*            Under Contract No. DE-AC07-05ID14517              */
/*            With the U. S. Department of Energy               */
/*                                                              */
/*            See COPYRIGHT for full restrictions               */
/****************************************************************/

#ifndef SMOOTHSTEPFUNCTION_H
#define SMOOTHSTEPFUNCTION_H

#include "Function.h"

class SmoothedStepFunction;

template <>
InputParameters validParams<SmoothedStepFunction>();

class SmoothedStepFunction : public Function
{
public:
  SmoothedStepFunction(const InputParameters & parameters);

  virtual Real value(Real t, const Point & x) override;

protected:
  Real _vLow;
  Real _vHigh;
  Real _period;
  Real _duty;
  Real _rise;

  Real DoubleTanh(Real t);
  Real RoundedStep(Real t);
};

#endif // SMOOTHSTEPFUNCTION_H

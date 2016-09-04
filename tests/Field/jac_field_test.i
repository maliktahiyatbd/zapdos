dom0Scale=1

[GlobalParams]
[]

[Mesh]
  type = GeneratedMesh
  dim = 1
  nx = 1
  xmax = 1.1
[]


[Problem]
	type = FEProblem
	kernel_coverage_check = false
[]

[Preconditioning]
	[./smp]
		type = SMP
		full = true
	[../]
[]

[Executioner]
	type = Steady
	petsc_options = '-snes_test_display'
	solve_type = NEWTON
	petsc_options_iname = '-snes_type'
	petsc_options_value = 'test'
[]

[Outputs]
	print_perf_log = true
	print_linear_residuals = false
	[./out]
		type = Exodus
		#	execute_on = 'final'
	[../]
[]

[Debug]
	show_var_residual_norms = true
[]

# [Kernels]
#   [./em_diffusion]
#     type = Diffusion
#     variable = em
#   [../]
#   [./pot_diffusion]
#     type = Diffusion
#     variable = potential
#   [../]
# []

[Variables]
	[./potential]
	[../]
	[./em]
	[../]
[]


[BCs]
	[./field_emission_left]
		type = FieldEmissionBC
		variable = em
		boundary = 'left'
		potential = potential
		r = 0
		position_units = ${dom0Scale}
	[../]
[]

[ICs]
	[./potential_ic]
		type = FunctionIC
		variable = potential
		function = potential_ic_func
	[../]
	[./em_ic]
			type = RandomIC
			variable = em
	[../]
[]

[Functions]
	[./potential_ic_func]
		type = ParsedFunction
		value = '-1.1 * (1.1 - x)'
	[../]
[]

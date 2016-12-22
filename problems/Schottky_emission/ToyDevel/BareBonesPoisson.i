position_units = 1E-6
time_units = 1

dom0Size = ${/ 2E-6 ${position_units}} #m

vhigh = 210E-3 #kV
resistance = 1 #Ohms
area = 5.02e-7 # Formerly 3.14e-6

relaxTime = ${/ 50e-9 ${time_units}} #s if time_units = 1
nCycles = 1
steadyStateTime = ${* ${nCycles} ${relaxTime}}

[GlobalParams]
	time_units = ${time_units}
	position_units = ${position_units}
	potential_units = kV
	use_moles = true
[]

[Mesh]
	type = GeneratedMesh	# Can generate simple lines, rectangles and rectangular prisms
	dim = 1								# Dimension of the mesh
	nx = 1000							# Number of elements in the x direction
	xmax = ${dom0Size}		# Length of test chamber
[]

[Problem]
	type = FEProblem
[]

[Preconditioning]
	[./smp]
		type = SMP
		full = true
	[../]
[]

[Executioner]
	type = Transient
	end_time = ${/ 10E3 ${time_units}}

	[./TimeIntegrator]
		type = ImplicitEuler #AStableDirk4 #CrankNicolson #ImplicitMidpoint #AStableDirk4 #CrankNicolson #ImplicitEuler
	[../]

	trans_ss_check = 1
	ss_check_tol = 1E-15
	ss_tmin = ${steadyStateTime}

	petsc_options = '-snes_converged_reason -snes_linesearch_monitor -snes_ksp_ew'
	solve_type = NEWTON	#PJFNK JFNK NEWTON

	petsc_options_iname = '-pc_type -pc_factor_mat_solver_package -pc_factor_shift_type -pc_factor_shift_amount -ksp_type -snes_linesearch_minlambda -ksp_gmres_restart'
	petsc_options_value = 'lu superlu_dist NONZERO 1.e-10 preonly 1e-3 100'

	nl_rel_tol = 1E-8
	nl_abs_tol = 1E-12

	dtmin = ${/ 1e-25 ${time_units}}
	nl_max_its = 200
	[./TimeStepper]
		type = IterationAdaptiveDT
		cutback_factor = 0.4
		dt = ${/ 1e-12 ${time_units}}
		growth_factor = 1.2
		optimal_iterations = 100
	[../]
[]

[Outputs]
	[./out]
		type = Exodus
	[../]
[]

[Debug]
	show_var_residual_norms = true
[]

[Kernels]
	[./LHS]
		type = CoeffDiffusionLin
		variable = potential
		block = 0
	[../]

	[./RHS]
		type = SetValue
		variable = potential
		value = 0
		block = 0
	[../]
[]

[Variables]
	[./potential]
	[../]
[]

[AuxVariables]
	[./x]
		order = CONSTANT
		family = MONOMIAL
	[../]
	[./Voltage]
		order = CONSTANT
		family = MONOMIAL
	[../]
[]

[AuxKernels]
## Cell AuxKernels
	[./x_g]
		type = Position
		variable = x
		block = 0
	[../]
	[./Voltage]
		type = Potential
		variable = Voltage
		potential = potential
		block = 0
	[../]
[]

[BCs]
	[./potential_dirichlet_left]
		type = DirichletBC
		variable = potential
		boundary = left
		value = -${vhigh}
	[../]

	[./potential_dirichlet_right]
		type = DirichletBC
		variable = potential
		boundary = right
		value = 0
	[../]
[]

[ICs]
	[./potential_ic]
		type = FunctionIC
		variable = potential
		function = potential_ic_func
	[../]
[]

[Functions]
	[./potential_ic_func]
		type = ParsedFunction
		value = '-${vhigh} * (${dom0Size} - x) / ${dom0Size}'
	[../]
[]

[Materials]
	[./gas_block]
		type = GenericConstantMaterial
		block = 0
		prop_names = 'diffpotential'
		prop_values = '8.85E-12'
	[../]
[]

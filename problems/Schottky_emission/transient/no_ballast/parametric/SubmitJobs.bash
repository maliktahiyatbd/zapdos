#!/bin/bash

oldFolder="base"

for d in $( seq 6.0 -1.0 3.0 )
do
	for phi in $( seq 4.40 -0.1 3.80 )
	do
		folder="V_200_V_d_${d}_um_phi_${phi}_eV_tOn_0.5_ns"
		cp -r ${oldFolder} ${folder}
		cd ${folder}
			sed -i 's?user_work_function = 4.00?user_work_function = '${phi}'?g' "Initial.i" ;
			sed -i 's?user_work_function = 4.00?user_work_function = '${phi}'?g' "SteadyState.i" ;
			
			sed -i 's?gap = 2E-6?gap = '${d}'E-6?g' "Initial.i" ;
			sed -i 's?gap = 2E-6?gap = '${d}'E-6?g' "SteadyState.i" ;
			
			sed -i "s?${oldFolder}?${folder}?g" "Submission.job" ;
			
			qsub Submission.job
		cd ..
	done
done

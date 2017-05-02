#!/bin/bash

oldFolder="base"

for V in $( seq 100 -5 40 )
do
	for d in $( seq 2 1 10 )
	do
		for tOff in $( seq 20 10 20 )
		do
			for phi in $( seq 3.00 0.1 3.00 )
			do
				folder="V_${V}_V_d_${d}_um_phi_${phi}_eV_tOn_0.5_ns_tOff_${tOff}_ns"
				cp -rT ${oldFolder} ${folder}
				cd ${folder}
					sed -i 's?vhigh = 200E-3?vhigh = '${V}'E-3?g' "Initial.i" ;
					sed -i 's?vhigh = 200E-3?vhigh = '${V}'E-3?g' "SteadyState.i" ;
					
					sed -i 's?gap = 4E-6?gap = '${d}'E-6?g' "Initial.i" ;
					sed -i 's?gap = 4E-6?gap = '${d}'E-6?g' "SteadyState.i" ;
					
					sed -i 's?tOff = 21E-9?tOff = '${tOff}'E-9?g' "Initial.i" ;
					sed -i 's?tOff = 21E-9?tOff = '${tOff}'E-9?g' "SteadyState.i" ;
					
					sed -i 's?work_function = 4.00?work_function = '${phi}'?g' "Initial.i" ;
					sed -i 's?work_function = 4.00?work_function = '${phi}'?g' "SteadyState.i" ;
					
					sed -i "s?-N ${oldFolder}?-N ${folder}?g" "Submission.job" ;
					
					qsub Submission.job
					cd ..
			done
		done
	done
done

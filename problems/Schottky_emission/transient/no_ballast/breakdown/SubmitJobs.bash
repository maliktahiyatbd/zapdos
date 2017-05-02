#!/bin/bash

oldFolder="base"

for V in $( seq 179 -1 171 )
do
	for d in $( seq 4 1 4 )
	do
		for phi in $( seq 4.00 0.1 4.00 )
		do
			folder="V_${V}_V_d_${d}_um_phi_${phi}_eV"
			cp -rT ${oldFolder} ${folder}
			cd ${folder}
				sed -i 's?vhigh = 200E-3?vhigh = '${V}'E-3?g' "Initial.i" ;
				sed -i 's?gap = 4E-6?gap = '${d}'E-6?g' "Initial.i" ;
				sed -i 's?work_function = 4.00?work_function = '${phi}'?g' "Initial.i" ;
				
				sed -i "s?-N ${oldFolder}?-N ${folder}?g" "Submission.job" ;
				
				qsub Submission.job
			cd ..
		done
	done
done

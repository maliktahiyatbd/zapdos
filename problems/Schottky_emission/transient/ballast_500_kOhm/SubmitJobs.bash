#!/bin/bash

#foreach x ( 2.1 2.2 2.3 2.4 2.5 2.6 2.7 2.8 2.9 3.1 3.2 3.3 3.4 3.5 3.6 3.7 3.8 3.9 4.1 4.2 4.3 4.4 4.5 4.6 4.7 4.8 4.9 5.1 5.2 5.3 5.4 5.5 5.6 5.7 5.8 5.9 )

oldFolder="base"

for x in {190..210..2}
do
	folder="V_${x}_V_d_2_um_phi_4.60_eV_tOn_0.5_ns"
	cp -r ${oldFolder} ${folder}
	cd ${folder}
	sed -i "s@198E-3@${x}E-3@g" "Transient.i" ;
	sed -i "s@198E-3@${x}E-3@g" "SteadyState.i" ;
	sed -i "s@${oldFolder}@${folder}@g" "Submission.job" ;
	qsub Submission.job
	cd ..
done

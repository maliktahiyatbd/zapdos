#!/bin/bash -i

module purge
module load paraview

OutputFile="SteadyState_out"
StartFile="Initial"
EndFile="Final"

pvpython toCSV2.py "${OutputFile}" "${StartFile}" 0

for file in $( ls ${StartFile}*0.csv ) ; do
	TempOutputFile=${file//"0.csv"/".txt"}
	mv ${file} ${TempOutputFile}
done

rm ${StartFile}*.csv

for file in $( ls ${StartFile}*.txt ) ; do
	NewOutputFile=${file//"_out"/""}
	NewOutputFile=${NewOutputFile//"Input"/"Output"}
	NewOutputFile=${NewOutputFile//".txt"/".csv"}

	mv ${file} ${NewOutputFile}
done

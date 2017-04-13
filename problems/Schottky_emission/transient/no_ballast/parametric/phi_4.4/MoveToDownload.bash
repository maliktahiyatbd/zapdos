#!/bin/bash

fileName1="SteadyState_out.e"
fileName2="SteadyState_checkpoint_cp"

newFolder1="download"
newFolder2="checkpoint"

minimumsize=90000

for pathName in `ls -d */${fileName1}` ; do
	folderName="`echo ${pathName} | cut -d "/" -f1`"

	actualsize=$(wc -c <"$pathName")

	if [ $actualsize -ge $minimumsize ]; then
		mv "${folderName}/${fileName1}" "${newFolder1}/${folderName}.e"
		mv "${folderName}/${fileName2}" "${newFolder2}/${folderName}"
		rm -rf "${folderName}"
	else
		echo Did not move $pathName
	fi
done

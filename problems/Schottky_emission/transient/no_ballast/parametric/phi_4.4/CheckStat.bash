#!/bin/bash

fileName1="SteadyState_out.e"
fileName2="SteadyState_checkpoint_cp"

dFolder="download"
cFolder="checkpoint"
bFolder="base"

for pathName in `ls -d ${dFolder}/*` ; do
	folderName="`echo ${pathName} | cut -d "/" -f2 `"
	folderName=${folderName%.e}

	echo "cp -r ${bFolder} ${folderName}"
	echo "cp -r ${dFolder} ${folderName}"
#	if [ $actualsize -ge $minimumsize ]; then
#		mv "${folderName}/${fileName1}" "${newFolder1}/${folderName}.e"
#		mv "${folderName}/${fileName2}" "${newFolder2}/${folderName}"
#		rm -rf "${folderName}"
#	else
#		echo Did not move $pathName
#	fi
done

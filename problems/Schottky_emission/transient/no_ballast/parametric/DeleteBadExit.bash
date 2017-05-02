#!/bin/bash

var=$(cat BadExit.txt | md5sum)

file="CurrentCycle.log"

for f in `ls */${file}` ; do
	var2=$(tail -n 6 ${f} | md5sum)
	
	if [[ $var == $var2 ]] ; then
		echo "deleting $(dirname ${f})"
		rm -rf $(dirname ${f});
#	else
#		echo "not deleting $(dirname ${f})";
	fi

done

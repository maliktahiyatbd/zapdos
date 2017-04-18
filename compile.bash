#!/bin/bash
#$ -V
#-pe mpi-8 8
#$ -N ZapdosInstall
#$ -M jhaase1@nd.edu
#$ -m ae
#$ -r y

if [ -r /opt/crc/Modules/current/init/bash ]
	then
	    source /opt/crc/Modules/current/init/bash
fi

module purge
module load cmake git ompi/2.0.1-gcc-6.2.0 boost/1.63

source /afs/crc.nd.edu/user/j/jhaase1/ZapdosDir/moose.bash

export CC=mpicc
export CXX=mpicxx
export F90=mpif90
export F77=mpif77
export FC=mpif90

make -j8

make METHOD=dbg -j8

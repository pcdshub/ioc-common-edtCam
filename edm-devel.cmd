#! /bin/bash

# Setup edm environment
# export EPICS_HOST_ARCH=linux-x86
# source /reg/g/pcds/setup/epicsenv-3.14.9.sh
source /reg/g/pcds/setup/epicsenv-3.14.12.sh
export EPICS_CA_MAX_ARRAY_BYTES=10000000

export IOC_PV=TST:IOC:EDT:ORCA1
export CAM=TST:EDT:ORCA1
export HUTCH=TST
export PVLIST=tst.lst
edm -x -eolc	\
	-m "IOC=${IOC_PV}"	\
	-m "CAM=${CAM}"		\
	-m "P=${CAM},R=:"	\
	-m "HUTCH=${HUTCH}"	\
	-m "PVLIST=${PVLIST}"	\
	edtPdvScreens/edtTop.edl  \
	edtPdvScreens/hamaOrcaFlash4_0.edl  &


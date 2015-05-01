#! /bin/bash

# Setup edm environment
source /reg/g/pcds/setup/epicsenv-3.14.12.sh
export EPICS_CA_MAX_ARRAY_BYTES=10000000

export EVR_PV=TST:EVR:EDT:PTM1
export IOC_PV=TST:IOC:EDT:PTM1
export CAM=TST:EDT:PTM1
export HUTCH=tst
edm -x -eolc	\
	-m "IOC=${IOC_PV}"	\
	-m "EVR=${EVR_PV}"	\
	-m "CAM=${CAM}"		\
	-m "P=${CAM},R=:"	\
	-m "HUTCH=${HUTCH}"	\
	edtCamScreens/edtCamTop.edl  &


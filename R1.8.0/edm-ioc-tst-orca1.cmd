#! /bin/bash

# Setup edm environment
source /reg/g/pcds/setup/epicsenv-3.14.12.sh
pushd $EPICS-dev/screens/edm/tst/current

export EVR_PV=TST:EVR:EDT:ORCA1
export IOC_PV=TST:IOC:EDT:ORCA1
export CAM=TST:EDT:ORCA1
export TRIG_CH=1
export HUTCH=tst
edm -x -eolc	\
	-m "IOC=${IOC_PV}"	\
	-m "EVR=${EVR_PV}"	\
	-m "CAM=${CAM}"		\
	-m "CH=${TRIG_CH}"	\
	-m "P=${CAM},R=:"	\
	-m "HUTCH=${HUTCH}"	\
	edtCamScreens/edtCamTop.edl  &


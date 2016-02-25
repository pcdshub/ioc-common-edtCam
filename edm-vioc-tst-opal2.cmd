#! /bin/bash

# Setup edm environment
# source $SETUP_SITE_TOP/epicsenv-3.14.12.sh

# Change to a directory w/ appropriate screen links
cd ${EPICS_SITE_TOP}-dev/screens/edm/tst/current

export EVR_PV=TST:EVR:EDT:OPAL2
export IOC_PV=TST:IOC:EDT:OPAL2
export CAM=TST:EDT:OPAL2
export CH=3
export HUTCH=tst
edm -x -eolc	\
	-m "IOC=${IOC_PV}"	\
	-m "EVR=${EVR_PV}"	\
	-m "CAM=${CAM}"		\
	-m "CH=${CH}"		\
	-m "P=${CAM},R=:"	\
	-m "HUTCH=${HUTCH}"	\
	edtCamScreens/edtCamTop.edl  &


#! /bin/bash

# Setup edm environment
source $SETUP_SITE_TOP/epicsenv-3.14.12.sh

# Change to a directory w/ appropriate screen links
cd ${EPICS_SITE_TOP}-dev/screens/edm/tst/current

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


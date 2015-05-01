#! /bin/bash

# Setup edm environment
source /reg/g/pcds/setup/epicsenv-3.14.12.sh
export EPICS_CA_MAX_ARRAY_BYTES=10000000

export EVR_PV=XPP:R30:EVR:27
export IOC_PV=XPP:R30:IOC:27
export CAM=XPP:OPAL1K:1
export HUTCH=xpp

edm -x -eolc	\
	-m "IOC=${IOC_PV}"	\
	-m "EVR=${EVR_PV}"	\
	-m "CAM=${CAM}"		\
	-m "P=${CAM},R=:"	\
	-m "HUTCH=${HUTCH}"	\
	edtCamScreens/edtCamTop.edl  &


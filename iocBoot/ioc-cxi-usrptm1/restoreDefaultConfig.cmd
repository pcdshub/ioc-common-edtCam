#! /bin/bash

# Setup edm environment
source /reg/g/pcds/setup/epicsenv-3.14.12.sh
export EPICS_CA_MAX_ARRAY_BYTES=10000000
edmpathmunge edtPdvScreens

export EVR_PV=CXI:USR:EVR:PTM1
export IOC_PV=CXI:USR:IOC:PTM1
export CAM=CXI:USR:CVV:PTM1
export HUTCH=cxi

caput ${CAM}:CamEventCode 40
caput ${CAM}:BeamEventCode 140
caput ${CAM}:MinX 0
caput ${CAM}:MinY 0
caput ${CAM}:SizeX 640
caput ${CAM}:SizeY 480
caput ${CAM}:AcquireTime 0.001
caput ${CAM}:TriggerMode External

#caput ${EVR_PV}:


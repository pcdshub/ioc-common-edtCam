#!/bin/bash

# Setup the IOC user environment
export IOC="ioc-xpp-opal1"
source /reg/d/iocCommon/All/xpp_env.sh

# Make sure the IOC's data directories are ready for use
$RUNUSER "mkdir -p      $IOC_DATA/$IOC/autosave"
$RUNUSER "mkdir -p      $IOC_DATA/$IOC/archive"
$RUNUSER "mkdir -p      $IOC_DATA/$IOC/iocInfo"
$RUNUSER "chmod ug+w -R $IOC_DATA/$IOC"

# For release

# For development
cd /reg/neh/home1/bhill/wa2/epics/ioc/common/edtCam/current/iocBoot/$IOC

# Copy the archive file to iocData
$RUNUSER "cp -f -p ../../archive/$IOC.archive $IOC_DATA/$IOC/archive"

# Launch the IOC
export CREATE_TIME=`date '+%m%d%Y_%H%M%S'`
$RUNUSER "$PROCSERV --logfile $IOC_DATA/$IOC/iocInfo/ioc.log --name $IOC 30100 ./st.cmd" &
$RUNUSER "ln -s $IOC_DATA/$IOC/iocInfo/ioc.log $IOC_DATA/$IOC/iocInfo/ioc.log_$CREATE_TIME"

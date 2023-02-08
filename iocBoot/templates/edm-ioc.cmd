#! /bin/bash

# Setup the common directory env variables
if [ -e /reg/g/pcds/pyps/config/common_dirs.sh ]; then
	source /reg/g/pcds/pyps/config/common_dirs.sh
else
	source /afs/slac/g/pcds/config/common_dirs.sh
fi

# Setup edm environment
source $SETUP_SITE_TOP/epicsenv-cur.sh

$$IF(APP,edtTpr)
export TPR_PV=$$IF(TPR_PV,$$TPR_PV,$$CAM_PV:NoTpr)
export TPE_PV=$$IF(TPE_PV,$$TPE_PV,$$TPR_PV)
export TPR_TR=$$IF(TPR_TR,$$TPR_TR,0)
export TPR_CH=$$IF(TPR_CH,$$TPR_CH,${TPR_TR})
export TPR_SE=$$IF(TPR_SE,$$TPR_SE,${TPR_CH})
$$ELSE(APP)
export EVR_PV=$$IF(EVR_PV,$$EVR_PV,$$CAM_PV:NoEvr)
export TRIG_CH=$$IF(EVR_TRIG,$$EVR_TRIG,0)
$$ENDIF(APP)
export IOC_PV=$$IOC_PV
export CAM=$$CAM_PV
export CAM_NAME="$$IF(CAM_NAME,$$CAM_NAME,Unnamed)"
export HUTCH=$$IF(HUTCH,$$HUTCH,tst)


EDM_TOP=edtCamScreens/$$IF(APP,$$APP,edt)CamTop.edl
$$IF(SCREENS_TOP)
SCREENS_TOP=$$SCREENS_TOP
$$ELSE(SCREENS_TOP)
SCREENS_TOP=${EPICS_SITE_TOP}-dev/screens/edm/${HUTCH}/current
$$ENDIF(SCREENS_TOP)

#pushd ${SCREENS_TOP}
# Now launching edm from new screenLinks directory under each IOCTOP release
# so each ioc can have it's own custom set of screens that matches which
# set of module depedencies that ioc was built with. 
pushd $$IOCTOP/screenLinks
edm -x -eolc	\
	-m "IOC=${IOC_PV}"			\
$$IF(APP,edtTpr)
	-m "TPR=${TPR_PV}"		\
	-m "TPR_PV=${TPR_PV}"	\
	-m "TPE_PV=${TPE_PV}"	\
	-m "TPR_TR=${TPR_TR}"	\
	-m "TPR_CH=${TPR_CH}"	\
	-m "TPR_SE=${TPR_SE}"	\
$$ELSE(APP)
	-m "EVR=${EVR_PV}"		\
	-m "CH=${TRIG_CH}"		\
$$ENDIF(APP)
	-m "CAM=${CAM}"				\
	-m "CAM_NAME=${CAM_NAME}"	\
	-m "P=${CAM},R=:"			\
	-m "EDM_TOP=${EDM_TOP}"		\
	-m "HUTCH=${HUTCH}"			\
	${EDM_TOP} &


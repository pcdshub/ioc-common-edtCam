TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

#=============================
# Build an ioc application

PROD_IOC = edt

# edt.dbd will be created and installed
DBD += edt.dbd

# edt.dbd will be made up from these files:
# base and std modules
edt_DBD += base.dbd
edt_DBD += iocAdmin.dbd
edt_DBD += autosaveSupport.dbd
edt_DBD += diagTimer.dbd

# EVR
edt_DBD += evrSupport.dbd
edt_DBD += devMrfEr.dbd

# areaDetector
edt_DBD += busyRecord.dbd
edt_DBD += genSubRecord.dbd
edt_DBD += sscan.dbd
edt_DBD += commonDriverSupport.dbd
edt_DBD += edtPdvSupport.dbd
edt_DBD += stream.dbd

edt_DBD += timeStampSource.dbd
edt_DBD += PluginBldSpectrometer.dbd

# ffmpegServer
#edt_DBD += ffmpegServer.dbd

# <name>_registerRecordDeviceDriver.cpp will be created from <name>.dbd
edt_SRCS += edt_registerRecordDeviceDriver.cpp
edt_SRCS += PluginBldSpectrometer.cpp
edt_SRCS_DEFAULT += edtMain.cpp

# Add locally compiled object code
#edt_SRCS += dbSubExample.c

# Add required libraries
# base and std modules
edt_LIBS += autosave
edt_LIBS += devIocStats
edt_LIBS += genSub
edt_LIBS += diagTimer

# EVR
edt_LIBS += evrSupport
edt_LIBS += devMrfEr
edt_LIBS += drvMrf
edt_LIBS_Linux += mrfApi

# areaDetector
edt_LIBS += calc
edt_LIBS += busy
edt_LIBS += NDPlugin
# GraphicsMagick not supported in areaDetector 1.9 for RHEL due to compiler issues
#edt_LIBS += NDFileMagick
edt_LIBS += ADBase
edt_LIBS += netCDF
edt_LIBS += NeXus
edt_LIBS += hdf5
edt_LIBS += simDetector
edt_LIBS += edtPdv
edt_LIBS += pdv
edt_LIBS += stream
edt_LIBS += sscan
edt_LIBS += asyn
edt_LIBS += recIoc
edt_LIBS += softDevIoc
edt_LIBS += miscIoc
edt_LIBS += rsrvIoc
edt_LIBS += dbtoolsIoc
edt_LIBS += asIoc
edt_LIBS += dbIoc
edt_LIBS += registryIoc
edt_LIBS += dbStaticIoc
edt_LIBS += ca
edt_LIBS += Com
#edt_LIBS += PvAPI
edt_LIBS += sz
edt_SYS_LIBS += X11
edt_SYS_LIBS += xml2
edt_SYS_LIBS += tiff
edt_SYS_LIBS += jpeg
edt_SYS_LIBS += z

# ffmpegServer
#edt_LIBS += avcodec
#edt_LIBS += avdevice
#edt_LIBS += avformat
#edt_LIBS += avutil
#edt_LIBS += ffmpegServer
#edt_LIBS += swscale

edt_LIBS += timeStampSource

edt_LIBS += $(EPICS_BASE_IOC_LIBS)

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE


TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

#=============================
# Build an ioc application

PROD_IOC = edt
PROD_IOC += serial_cmd

# edt.dbd will be created and installed
DBD += edt.dbd

# edt.dbd will be made up from these files:
# base and std modules
edt_DBD += base.dbd
edt_DBD += iocAdmin.dbd
edt_DBD += autosaveSupport.dbd
edt_DBD += diagTimer.dbd
edt_DBD += history.dbd

# EVR
edt_DBD += evrSupport.dbd
edt_DBD += devMrfEr.dbd

# areaDetector
edt_DBD += busyRecord.dbd
#edt_DBD += genSubRecord.dbd
edt_DBD += sscan.dbd
edt_DBD += commonDriverSupport.dbd
edt_DBD += edtPdvSupport.dbd
edt_DBD += stream.dbd
edt_DBD += bldClient.dbd

#edt_DBD += genSubRecord.dbd
edt_DBD += timeStampFifo.dbd
edt_DBD += pluginBldSpectrometer.dbd

# ffmpegServer
#edt_DBD += ffmpegServer.dbd

# <name>_registerRecordDeviceDriver.cpp will be created from <name>.dbd
edt_SRCS += edt_registerRecordDeviceDriver.cpp
edt_SRCS += pluginBldSpectrometer.cpp
edt_SRCS += libedt.c
edt_SRCS_DEFAULT += edtMain.cpp

serial_cmd_SRCS += serial_cmd.c
serial_cmd_LIBS += pdv

# Add locally compiled object code
#edt_SRCS += dbSubExample.c

# Add required libraries
# base and std modules
edt_LIBS += autosave
edt_LIBS += devIocStats
edt_LIBS += diagTimer
edt_LIBS += bldClient
edt_LIBS += history

# EVR
edt_LIBS += evrSupport
edt_LIBS += devMrfEr
edt_LIBS += drvMrf
edt_LIBS_Linux += mrfApi

# EDT PDV libs
edt_LIBS += edtPdv
edt_LIBS += pdv

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
#edt_SYS_LIBS += X11
#edt_SYS_LIBS += xml2
edt_LIBS += tiff
edt_LIBS += jpeg
edt_LIBS += pcre
edt_SYS_LIBS += z
edt_SYS_LIBS += pthread
edt_SYS_LIBS += dl

#PROD_DEPLIB_DIRS += $(PCRE_TOP)/lib
#PROD_DEPLIB_DIRS += /usr/lib64

# ffmpegServer
#edt_LIBS += avcodec
#edt_LIBS += avdevice
#edt_LIBS += avformat
#edt_LIBS += avutil
#edt_LIBS += ffmpegServer
#edt_LIBS += swscale

edt_LIBS += timeStampFifo
#edt_LIBS += genSub


edt_LIBS += $(EPICS_BASE_IOC_LIBS)

# install: showme

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

showme:
	@echo T_A=$(T_A)
	@echo PKG_ARCH=$(PKG_ARCH)
	@echo PCRE_TOP=$(PCRE_TOP)


TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

#=============================
# Build an ioc application
PROD_NAME = edt

PROD_IOC = $(PROD_NAME) 
PROD_IOC += serial_cmd

# edt.dbd will be created and installed
DBD += edt.dbd

include $(ADCORE)/ADApp/commonDriverMakefile

# edt.dbd will be made up from these files:
# base and std modules
edt_DBD += iocAdmin.dbd
edt_DBD += caPutLog.dbd
edt_DBD += diagTimer.dbd
edt_DBD += history.dbd

# pvaSrv
edt_DBD += dbPv.dbd

# EVR
edt_DBD += evrSupport.dbd
edt_DBD += devMrfEr.dbd

# areaDetector
#edt_DBD += genSubRecord.dbd
#edt_DBD += sscan.dbd

edt_DBD += edtPdvSupport.dbd
edt_DBD += stream.dbd
edt_DBD += bldClient.dbd

# Genicam support
edt_DBD += asynGenicam.dbd

#edt_DBD += genSubRecord.dbd
edt_DBD += timeStampFifo.dbd
edt_DBD += pluginBldSpectrometer.dbd

# ffmpegServer
edt_DBD += ffmpegServer.dbd

# <name>_registerRecordDeviceDriver.cpp will be created from <name>.dbd
edt_SRCS += edt_registerRecordDeviceDriver.cpp
edt_SRCS += pluginBldSpectrometer.cpp
# Disabling libedt.c as I only brought it in to add some error msgs for debugging an mmap problem
#edt_SRCS += libedt.c
edt_SRCS_DEFAULT += edtMain.cpp

serial_cmd_SRCS += serial_cmd.c
serial_cmd_LIBS += pdv

# Add locally compiled object code
#edt_SRCS += dbSubExample.c

# Add required libraries
# base and std modules
#edt_LIBS += autosave
edt_LIBS += devIocStats
edt_LIBS += caPutLog
edt_LIBS += diagTimer
edt_LIBS += bldClient
edt_LIBS += history

# pvaSrv
edt_LIBS += pvaSrv

# EVR
edt_LIBS += evrSupport
edt_LIBS += devMrfEr
edt_LIBS += drvMrf
edt_LIBS_Linux += mrfApi

# EDT PDV libs
edt_LIBS += edtPdv
edt_LIBS += pdv

# Genicam support
edt_LIBS += asynGenicam

#edt_LIBS += PvAPI
#edt_SYS_LIBS += X11
#edt_SYS_LIBS += xml2
ifdef PCRE_LIB
edt_LIBS += pcre
endif
edt_SYS_LIBS += z
edt_SYS_LIBS += pthread
edt_SYS_LIBS += dl

#PROD_DEPLIB_DIRS += $(PCRE_TOP)/lib
#PROD_DEPLIB_DIRS += /usr/lib64

# ffmpegServer
edt_LIBS += avdevice
edt_LIBS += ffmpegServer
edt_LIBS += swscale
edt_LIBS += avformat
edt_LIBS += avcodec
edt_LIBS += avutil
edt_LIBS += swresample
edt_LIBS += bz2
edt_LIBS += lzma

# areaDetector
# GraphicsMagick not supported in areaDetector 1.9 for RHEL due to compiler issues
#edt_LIBS += NDFileMagick
edt_LIBS += simDetector
edt_LIBS += stream

edt_LIBS += timeStampFifo
#edt_LIBS += genSub


# install: showme


#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

showme:
	@echo T_A=$(T_A)
	@echo PKG_ARCH=$(PKG_ARCH)
	@echo PCRE_LIB=$(PCRE_LIB)


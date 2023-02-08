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

# $(PROD_NAME).dbd will be created and installed
DBD += $(PROD_NAME).dbd

# $(PROD_NAME).dbd will be made up from these files:
include $(ADCORE)/ADApp/commonDriverMakefile

# std ioc modules
$(PROD_NAME)_DBD += iocAdmin.dbd
$(PROD_NAME)_DBD += caPutLog.dbd
$(PROD_NAME)_DBD += history.dbd
$(PROD_NAME)_DBD += system.dbd


ifdef EVENT2
# Timing and EVR
$(PROD_NAME)_DBD += diagTimer.dbd
$(PROD_NAME)_DBD += event2Support.dbd
$(PROD_NAME)_DBD += devEvent2Er.dbd

# Multicore utilities
$(PROD_NAME)_DBD += mcoreutils.dbd

# ADCore timeStamp provider
$(PROD_NAME)_DBD += timeStampFifo.dbd
endif

edt_DBD += edtPdvSupport.dbd
edt_DBD += stream.dbd
edt_DBD += bldClient.dbd

# Genicam support
edt_DBD += asynGenicam.dbd

#edt_DBD += genSubRecord.dbd
edt_DBD += pluginBldSpectrometer.dbd

# <name>_registerRecordDeviceDriver.cpp will be created from <name>.dbd
edt_SRCS += edt_registerRecordDeviceDriver.cpp
edt_SRCS += pluginBldSpectrometer.cpp
# Disabling libedt.c as I only brought it in to add some error msgs for debugging an mmap problem
#edt_SRCS += libedt.c
edt_SRCS_DEFAULT += edtMain.cpp

serial_cmd_SRCS += serial_cmd.c
serial_cmd_LIBS += pdv

# Add required libraries
# base and std modules
PROD_LIBS += $(EPICS_BASE_PVA_CORE_LIBS)

$(PROD_NAME)_LIBS += devIocStats
$(PROD_NAME)_LIBS += caPutLog
$(PROD_NAME)_LIBS += bldClient
$(PROD_NAME)_LIBS += history

# pvaSrv
#$(PROD_NAME)_LIBS += pvaSrv

ifdef EVENT2
# ADCore Timestamp provider
$(PROD_NAME)_LIBS += timeStampFifo

# Timing and EVR
$(PROD_NAME)_LIBS += event2Support
$(PROD_NAME)_LIBS += devEvent2Er
$(PROD_NAME)_LIBS += drvEvent2

$(PROD_NAME)_LIBS += mcoreutils

# Diagnostics
$(PROD_NAME)_LIBS += diagTimer
endif

# EDT PDV libs
edt_LIBS += edtPdv
edt_LIBS += pdv

# streamdevice lib
edt_LIBS += stream

# Genicam support
edt_LIBS += asynGenicam

#edt_LIBS += PvAPI
#edt_SYS_LIBS += X11
#edt_SYS_LIBS += xml2
ifneq ($(wildcard $(STREAMDEVICE)/lib/$(T_A)/libpcre.a),)
edt_LIBS += pcre
endif
edt_SYS_LIBS += z
edt_SYS_LIBS += pthread
edt_SYS_LIBS += dl

ifdef FFMPEGSERVER
# ffmpegServer
$(PROD_NAME)_DBD += ffmpegServer.dbd
DB_INSTALLS += $(wildcard $(FFMPEGSERVER)/db/plugin*.cmd)
DB_INSTALLS += $(wildcard $(FFMPEGSERVER)/db/plugin*.db)
$(PROD_NAME)_LIBS += avdevice
$(PROD_NAME)_LIBS += ffmpegServer
$(PROD_NAME)_LIBS += swscale
$(PROD_NAME)_LIBS += avformat
$(PROD_NAME)_LIBS += avcodec
$(PROD_NAME)_LIBS += avutil
$(PROD_NAME)_LIBS += swresample
$(PROD_NAME)_LIBS += bz2
$(PROD_NAME)_LIBS += lzma
endif

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE


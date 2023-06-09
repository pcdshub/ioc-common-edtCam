TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

#=============================
# Build an ioc application
PROD_NAME = edtTpr

PROD_IOC = $(PROD_NAME)

# $(PROD_NAME).dbd will be created and installed
DBD += $(PROD_NAME).dbd

# $(PROD_NAME).dbd will be made up from these files:
include $(ADCORE)/ADApp/commonDriverMakefile

# std ioc modules
$(PROD_NAME)_DBD += iocAdmin.dbd
$(PROD_NAME)_DBD += caPutLog.dbd
$(PROD_NAME)_DBD += history.dbd
$(PROD_NAME)_DBD += system.dbd


ifdef TPRTRIGGER
# Timing w/ TPR
$(PROD_NAME)_DBD += diagTimer.dbd
$(PROD_NAME)_DBD += ATCACommon.dbd
$(PROD_NAME)_DBD += crossbarControl.dbd
$(PROD_NAME)_DBD += tprTriggerAsynDriver.dbd
$(PROD_NAME)_DBD += yamlLoader.dbd

$(PROD_NAME)_DBD += mcoreutils.dbd

# ADCore timeStamp provider
$(PROD_NAME)_DBD += timeStampFifo.dbd
endif

$(PROD_NAME)_DBD += edtPdvSupport.dbd
$(PROD_NAME)_DBD += stream.dbd
$(PROD_NAME)_DBD += bldClient.dbd

# Genicam support
$(PROD_NAME)_DBD += asynGenicam.dbd

#$(PROD_NAME)_DBD += genSubRecord.dbd
$(PROD_NAME)_DBD += pluginBldSpectrometer.dbd

# <name>_registerRecordDeviceDriver.cpp will be created from <name>.dbd
$(PROD_NAME)_SRCS += $(PROD_NAME)_registerRecordDeviceDriver.cpp
$(PROD_NAME)_SRCS_DEFAULT += $(PROD_NAME)Main.cpp

# Add required libraries
# base and std modules
PROD_LIBS += $(EPICS_BASE_PVA_CORE_LIBS)

$(PROD_NAME)_LIBS += devIocStats
$(PROD_NAME)_LIBS += caPutLog
$(PROD_NAME)_LIBS += bldClient
$(PROD_NAME)_LIBS += history

# pvaSrv
#$(PROD_NAME)_LIBS += pvaSrv

ifdef TPRTRIGGER
# ADCore Timestamp provider
$(PROD_NAME)_LIBS += timeStampFifo

# Timing and TPR
$(PROD_NAME)_LIBS += tprTrigger
$(PROD_NAME)_LIBS += ATCACommon
$(PROD_NAME)_LIBS += yamlLoader

$(PROD_NAME)_LIBS += mcoreutils

# Diagnostics
$(PROD_NAME)_LIBS += diagTimer
endif

# EDT PDV libs
$(PROD_NAME)_LIBS += edtPdv
$(PROD_NAME)_LIBS += pdv

# streamdevice lib
$(PROD_NAME)_LIBS += stream

# Genicam support
$(PROD_NAME)_LIBS += asynGenicam

$(PROD_NAME)_LIBS += pbs

#$(PROD_NAME)_LIBS += PvAPI
#$(PROD_NAME)_SYS_LIBS += X11
#$(PROD_NAME)_SYS_LIBS += xml2
ifneq ($(wildcard $(STREAMDEVICE)/lib/$(T_A)/libpcre.a),)
$(PROD_NAME)_LIBS += pcre
endif
$(PROD_NAME)_SYS_LIBS += z
$(PROD_NAME)_SYS_LIBS += pthread
$(PROD_NAME)_SYS_LIBS += dl

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

#======================================================
# PATH TO "NON EPICS" EXTERNAL PACKAGES: USER LIBRARIES
#======================================================
cpsw_DIR = $(CPSW_FRAMEWORK_LIB)
boost_system_DIR = $(BOOST_LIB)
yaml-cpp_DIR = $(YAML_LIB)
hpstpr_DIR = $(HPSTPR_LIB)
commonATCA_DIR = $(COMMONATCA_LIB)

# ======================================================
# LINK "NON EPICS" EXTERNAL PACKAGE LIBRARIES STATICALLY
#=======================================================
USR_LIBS_Linux += cpsw
USR_LIBS_Linux += boost_system yaml-cpp hpstpr
USR_LIBS_Linux += commonATCA

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE


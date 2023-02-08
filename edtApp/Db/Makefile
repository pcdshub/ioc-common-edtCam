TOP=../..
include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE

DB_INSTALLS += $(EVENT2)/db/devEvrInfo.db

#----------------------------------------------------
# Create and install (or just install)
# databases, templates, substitutions like this

DB_INSTALLS += $(TOP)/edtApp/Db/pluginBldSpectrometer.cmd
DB_INSTALLS += $(ASYN)/db/asynRecord.db
DB_INSTALLS += $(AUTOSAVE)/db/save_restoreStatus.db
DB_INSTALLS += $(IOCADMIN)/db/iocSoft.db
DB_INSTALLS += $(IOCADMIN)/db/devIocInfo.db

# Create iocRelease.db
IOCRELEASE_DB	+=	iocRelease.db

DB += timeStampTprEventCode.db

ifdef TPRTRIGGER
# TODO: Move devTprInfo.db to TPRTRIGGER
#DB += devTprInfo.db

# Install Database for tprTrigger module
DB_INSTALLS += $(TPRTRIGGER)/db/devTprInfo.db
DB_INSTALLS += $(TPRTRIGGER)/db/pcie_tprTrig.db
DB_INSTALLS += $(TPRTRIGGER)/db/pcieSlave_tprTrig.db
ifdef TIMESTAMPFIFO
DB_INSTALLS += $(TIMESTAMPFIFO)/db/timeStampFifo.template
endif
endif

ifdef EVENT2
DB_INSTALLS += $(EVENT2)/db/evrPmc230.db
DB_INSTALLS += $(EVENT2)/db/evrSLAC.db
DB_INSTALLS += $(EVENT2)/db/eventCodeDelays.db
ifdef TIMESTAMPFIFO
DB_INSTALLS += $(TIMESTAMPFIFO)/db/timeStampFifo.template
endif
endif

# Install ADStream db files
DB_INSTALLS += $(wildcard $(ADSTREAM)/db/*.db)

# Install ADStream cmd scripts
DB_INSTALLS += $(wildcard $(ADSTREAM)/db/*.cmd)

DB += ffmpegUsed.db
ifdef FFMPEGSERVER
# Install ffmpegServer files
DB_INSTALLS += $(wildcard $(FFMPEGSERVER)/db/plugin*.db)
DB_INSTALLS += $(wildcard $(FFMPEGSERVER)/db/plugin*.cmd)
endif

# History DB Files
DB += bld_hist.db
DB += edtCam_hist.db

#
# Create and install Camera DB Files
DB += edtPdvCamera.db
DB += pluginBldSpectrometer.db
DB += cannedSequences.db

# The EDT driver will expect that each camera model will
# have 3 versions of the PDV configuration file for
# free run, external triggered, and external pulse mode.
#
# Free run mode:
#	The camera is allowed to acquire images as fast as it can
# External triggered:
#	The camera shutter is triggered by an external trigger,
#	but the shutter duration is set via serial commands
# External pulse:
#	The camera shutter is opened by an external trigger and
#	closed when the trigger pulse ends.
#
# Example for model hamaOrcaFlash4_0
#	hamaOrcaFlash4_0f.cfg		Free Run  Config File
#	hamaOrcaFlash4_0t.cfg		Ext. Trig Config File
#	hamaOrcaFlash4_0p.cfg		Ext Pulse Config File
# 

# Hamamatsu Orca Flash 4.0
#MODELS = hamaOrcaFlash4_0
# Adimec Opal 1000 12 bit
#MODELS += opal1000m_12
# Pulnix 6740 10 bit
#MODELS += ptm6740_10
# Uniq UP685 10 bit
#MODELS += up685
# Uniq UP900 10 bit
#MODELS += up900
#MODELS := $(addprefix $(ADEDTPDV)/db/, $(MODELS))

# Install all ADEDTPDV models
MODELS := $(wildcard $(ADEDTPDV)/db/*.env)
MODELS := $(subst .env,,$(MODELS))

DB_INSTALLS += $(addsuffix .db,		$(MODELS))
DB_INSTALLS += $(addsuffix .env,	$(MODELS))
DB_INSTALLS += $(addsuffix f.cfg,	$(MODELS))
DB_INSTALLS += $(addsuffix t.cfg,	$(MODELS))
DB_INSTALLS += $(addsuffix p.cfg,	$(MODELS))
DB_INSTALLS += $(addsuffix .proto,	$(MODELS))
#DB_INSTALLS += $(ADEDTPDV)/db/ptm4200_12.cfg

#----------------------------------------------------
# Create and install (or just install)
# Archive files, templates, substitutions like this
#
ARCHIVE += bld_hist.archive
ARCHIVE += edtCam_hist.archive
ARCHIVE += edt-ioc.archive
ARCHIVE += pluginBldSpectrometer.archive
ifdef EVENT2
ARCHIVE += $(EVENT2)/archive/archive_evr_PMC.archive
ARCHIVE += $(EVENT2)/archive/archive_evr_SLAC.archive
endif

#----------------------------------------------------
# Create and install (or just install)
# Autosave request files, templates, substitutions like this
#
REQ += bld_hist.req
REQ += edtCam_hist.req
REQ += edt-ioc.req
REQ += pluginBldSpectrometer.req

ifdef EVENT2
# Create and install autosave Files
REQ += $(EVENT2)/autosave/eventCodeDelays.req
REQ += $(EVENT2)/autosave/autosave_evr_PMC.req
REQ += $(EVENT2)/autosave/autosave_evr_SLAC.req
endif

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE


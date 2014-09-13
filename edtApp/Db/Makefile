TOP=../..
include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE

# Create and install EDT Camera DB Files
DB += edtPdvCamera.db

#----------------------------------------------------
# Create and install (or just install)
# databases, templates, substitutions like this

DB_INSTALLS += $(ASYN)/db/asynRecord.db
DB_INSTALLS += $(AUTOSAVE)/db/save_restoreStatus.db
DB_INSTALLS += $(EVENT2)/db/evrPmc230.db
DB_INSTALLS += $(EVENT2)/db/evrSLAC.db
DB_INSTALLS += $(IOCADMIN)/db/iocSoft.db

#
# Install Camera db, configuration, and protocol files
# The driver will expect that each camera model will
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
MODEL = hamaOrcaFlash4_0
DB_INSTALLS += $(ADEDTPDV)/db/$(MODEL).db
DB_INSTALLS += $(ADEDTPDV)/db/$(MODEL).env
DB_INSTALLS += $(ADEDTPDV)/db/$(MODEL)f.cfg
DB_INSTALLS += $(ADEDTPDV)/db/$(MODEL)t.cfg
DB_INSTALLS += $(ADEDTPDV)/db/$(MODEL)p.cfg
DB_INSTALLS += $(ADEDTPDV)/db/$(MODEL).proto

#----------------------------------------------------
# Create and install (or just install)
# Archive files, templates, substitutions like this
#
ARCHIVE += edt-ioc.archive

#----------------------------------------------------
# Create and install (or just install)
# Autosave request files, templates, substitutions like this
#
REQ += edt-ioc.req

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE


TOP=../..
include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#
#----------------------------------------------------
# Optimization of db files using dbst (DEFAULT: NO)
#DB_OPT = YES

#----------------------------------------------------
# Create and install (or just install)
# databases, templates, substitutions like this
DB += timeStampTprEventCode.db

DB_INSTALLS += $(ASYN)/db/asynRecord.db
DB_INSTALLS += $(AUTOSAVE)/db/save_restoreStatus.db
DB_INSTALLS += $(IOCADMIN)/db/iocSoft.db
DB_INSTALLS += $(IOCADMIN)/db/devIocInfo.db

# Create iocRelease.db
IOCRELEASE_DB	+=	iocRelease.db

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
#----------------------------------------------------
# Create and install (or just install)
# Archive files, templates, substitutions like this
#

ifdef TPRTRIGGER
#ARCHIVE += $(TPRTRIGGER)/archive/pcie_tprTrig.archive
#ARCHIVE += $(TPRTRIGGER)/archive/pcieSlave_tprTrig.archive
endif

#----------------------------------------------------
# Create and install (or just install)
# Autosave request files, templates, substitutions like this
#

ifdef TPRTRIGGER
#REQ += $(TPRTRIGGER)/autosave/pcie_tprTrig.req
#REQ += $(TPRTRIGGER)/autosave/pcieSlave_tprTrig.req
endif

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE


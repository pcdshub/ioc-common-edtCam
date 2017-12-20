# Makefile for ioc instance
TOP = ../..
-include ./IOC_APPL_TOP
ifneq (,$(IOC_APPL_TOP))
TOP = $(IOC_APPL_TOP)
endif

include $(TOP)/configure/CONFIG
ARCH = $$IF(TARGET_ARCH,$$TARGET_ARCH,linux-x86_64)

INSTALL_LOCATION = ../..
INSTALL_LOCATION_BIN = $(TOP)/bin

TARGETS = envPaths

# Make sure these archive files are fully expanded
USR_ARCHIVEFLAGS += -V

#----------------------------------------------------
# Create and install IOC archive file
#
ARCHIVE += $$IOCNAME.archive

#----------------------------------------------------
# Create and install IOC autosave file
#
REQ += $$IOCNAME.req

include $(TOP)/configure/RULES.ioc
-include $(TOP)/configure/RULES.archive
-include $(TOP)/configure/RULES.autosave

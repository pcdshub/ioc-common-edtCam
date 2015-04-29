TOP = ../..
include $(TOP)/configure/CONFIG
ARCH = linux-x86_64
TARGETS = envPaths

# Make sure these archive files are fully expanded
USR_ARCHIVEFLAGS += -V

#----------------------------------------------------
# Create and install IOC archive file
#
ARCHIVE += ioc-cxi-usrptm1.archive

#----------------------------------------------------
# Create and install IOC autosave file
#
REQ += ioc-cxi-usrptm1.req

include $(TOP)/configure/RULES
include $(TOP)/configure/RULES.ioc-targets

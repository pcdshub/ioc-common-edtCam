TOP = ../..
include $(TOP)/configure/CONFIG
ARCH = linuxRT_glibc-x86_64
TARGETS = envPaths

# Make sure these archive files are fully expanded
USR_ARCHIVEFLAGS += -V

#----------------------------------------------------
# Create and install IOC archive file
#
ARCHIVE += ioc-tst-opal1.archive

#----------------------------------------------------
# Create and install IOC autosave file
#
REQ += ioc-tst-opal1.req

include $(TOP)/configure/RULES
include $(TOP)/configure/RULES.ioc-targets

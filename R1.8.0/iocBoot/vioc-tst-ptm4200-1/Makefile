TOP = ../..
include $(TOP)/configure/CONFIG
ARCH = linuxRT_glibc-x86_64
TARGETS = envPaths

# Make sure these archive files are fully expanded
USR_ARCHIVEFLAGS += -V

#----------------------------------------------------
# Create and install IOC archive file
#
ARCHIVE += vioc-tst-ptm4200-1.archive

#----------------------------------------------------
# Create and install IOC autosave file
#
REQ += vioc-tst-ptm4200-1.req

include $(TOP)/configure/RULES
include $(TOP)/configure/RULES.ioc-targets

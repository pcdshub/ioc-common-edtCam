# Makefile
TOP = ../..
include $(TOP)/configure/CONFIG
#----------------------------------------
# ADD MACRO DEFINITIONS AFTER THIS LINE

DBD          += pluginBldSpectrometer.dbd

LIBRARY_IOC  += pbs

# The following are required for all ims record configurations.
# Disabling libedt.c as I (Bruce?) only brought it in to add some error msgs for debugging an mmap problem
#pbs_SRCS += libedt.c
pbs_SRCS     += pluginBldSpectrometer.cpp

include $(TOP)/configure/RULES
#----------------------------------------
# ADD RULES AFTER THIS LINE

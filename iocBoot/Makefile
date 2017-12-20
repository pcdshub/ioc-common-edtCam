TOP = ..
-include $(TOP)/IOC_APPL_TOP
ifneq (,$(IOC_APPL_TOP))
TOP = $(IOC_APPL_TOP)
endif

# SLAC PCDS Makefile for building templated IOC instances
IOC_CFG  += $(wildcard *.cfg)
# Don't enable this yet as it conflicts w/ EPICS build
#include /reg/g/pcds/controls/macro/exp/RULES_EXPAND

include $(TOP)/configure/CONFIG

# No need to make these every time
# If you want to test one of these older ioc's,
# run make from it's directory
#DIRS += $(wildcard *ioc*)

include $(EPICS_BASE)/configure/RULES_DIRS


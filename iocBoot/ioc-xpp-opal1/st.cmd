#!../../bin/linux-x86_64/edt

< envPaths
# IOC macro should be the ioc name, used as
# part of the path to the autosave directory
# and in the autosave restoreSet name.
epicsEnvSet( "ENGINEER",	"Bruce Hill (bhill)" )
epicsEnvSet( "LOCATION",	"XPP:R30:IOC:27" )
epicsEnvSet( "IOCSH_PS1",	"$(IOC)> " )

#
# Installation specific ioc instance configuration
#

# PV Prefixes
epicsEnvSet( "IOC_PV",	"XPP:OPAL1K:IOC:01" )
epicsEnvSet( "EVR_PV",	"XPP:OPAL1K:EVR:01" )
epicsEnvSet( "TRIG_PV",	"$(EVR_PV):TRIG0" )
epicsEnvSet( "CAM_PV",	"XPP:OPAL1K:01" )

# Configure EVR
epicsEnvSet( "EVR_CARD",	"0" )
# EVR Type: 0=VME, 1=PMC, 15=SLAC
#epicsEnvSet( "EVR_TYPE",	"1" )
epicsEnvSet( "EVR_TYPE",	"15" )

# Specify camera model, asyn CAM_PORT, Mpeg HTTP_PORT,
# and additional plugins, if desired
epicsEnvSet( "MODEL",		"opal1000m_12" )
epicsEnvSet( "EDT_UNIT",	"0" )
epicsEnvSet( "EDT_CH",		"0" )
epicsEnvSet( "EPICS_CA_MAX_ARRAY_BYTES", "20000000" )
epicsEnvSet( "HTTP_PORT",	"7800" )
epicsEnvSet( "MJPG_PORT",	"8080"	)
epicsEnvSet( "PLUGINS",		"pcdsPlugins" )

# Comment/uncomment/change diagnostic settings as desired
epicsEnvSet( "CAM_TRACE_MASK",    "9" )
epicsEnvSet( "CAM_TRACE_IO_MASK", "1" )
epicsEnvSet( "SER_TRACE_MASK",    "9" )
epicsEnvSet( "SER_TRACE_IO_MASK", "1" )
epicsEnvSet( "ST_CMD_DELAYS", 	  "1" )
epicsEnvSet( "EVR_DEBUG",         "0" )

cd( "../.." )

# Run common startup commands for linux soft IOC's
< /reg/d/iocCommon/All/pre_linux.cmd

# Register all support components
dbLoadDatabase("dbd/edt.dbd")
edt_registerRecordDeviceDriver(pdbbase)

# Set debug variables
var DEBUG_EDT_PDV 2
var DEBUG_TS_FIFO  2

# Load standard soft ioc database
dbLoadRecords( "db/iocSoft.db",				"IOC=$(IOC_PV)" )

# Setup autosave
dbLoadRecords( "db/save_restoreStatus.db",	"IOC=$(IOC_PV)" )
set_savefile_path( "$(IOC_DATA)/$(IOC)/autosave" )
set_requestfile_path( "$(TOP)/autosave" )
# Also look in the iocData autosave folder for auto generated req files
set_requestfile_path( "$(IOC_DATA)/$(IOC)/autosave" )
save_restoreSet_status_prefix( "$(IOC_PV):" )
save_restoreSet_IncompleteSetsOk( 1 )
save_restoreSet_DatedBackupFiles( 1 )
var save_restoreLogMissingRecords 0
set_pass0_restoreFile( "autoSettings.sav" )
set_pass0_restoreFile( "$(IOC).sav" )
set_pass1_restoreFile( "autoSettings.sav" )
set_pass1_restoreFile( "$(IOC).sav" )

# $$ commands for when we go to templated st.cmd
# $$LOOP(EVR)
#
#
#
# Configuring EVR card $(EVR_CARD)
ErDebugLevel( $(EVR_DEBUG) )
ErConfigure( $(EVR_CARD), 0, 0, 0, $(EVR_TYPE) )
#dbLoadRecords( "db/evrPmc230.db",		"EVR=$(EVR_PV),CARD=$(EVR_CARD),IP0E=Enabled" )
dbLoadRecords( "db/evrSLAC.db",			"EVR=$(EVR_PV),CARD=$(EVR_CARD),IP0E=Enabled" )

#
#
#
# Comment/uncomment/change delay as desired so you can see EVR messages during boot
#epicsThreadSleep $(ST_CMD_DELAYS)
# $$LOOP(EVR)

# Default edtPdvDriver settings
epicsEnvSet(	"CAM_PORT",					"CAM" )
epicsEnvSet(	"CAM_MODE",					"Base" )
epicsEnvSet(	"STREAM_PROTOCOL_PATH",		"db" )

# Read model specific environment variables
< db/$(MODEL).env

#
#
#
# =========================================================
# Configure an edtPdv driver for the specified camera model
# =========================================================
edtPdvConfig( "$(CAM_PORT)", $(EDT_UNIT), $(EDT_CH), "$(MODEL)", "$(CAM_MODE)" )

# Set asyn trace flags
asynSetTraceMask(   "$(CAM_PORT)",		1, $(CAM_TRACE_MASK) )
asynSetTraceIOMask( "$(CAM_PORT)",		1, $(CAM_TRACE_IO_MASK) )
asynSetTraceMask(   "$(CAM_PORT).SER",	1, $(SER_TRACE_MASK) )
asynSetTraceIOMask( "$(CAM_PORT).SER",	1, $(SER_TRACE_IO_MASK) )

#
#
#
# Comment/uncomment/change delay as desired so you can see edtPdv messages during boot
epicsThreadSleep $(ST_CMD_DELAYS)

# Configure and load standard edtPdv camera database
dbLoadRecords(	"db/edtPdvCamera.db",		"CAM=$(CAM_PV),CAM_PORT=$(CAM_PORT),CAM_TRIG=$(TRIG_PV),EVR=$(EVR_PV)" )
dbLoadRecords(	"db/timeStampFifo.template","DEV=$(CAM_PV):TSS,PORT_PV=$(CAM_PV):PortName_RBV,EC_PV=$(EVR_PV):EVENT1CTRL.ENM,DLY=1" )

# For camera serial asyn diagnostics
# (AreaDetector plugins each have their own AsynIO record)
dbLoadRecords(	"db/asynRecord.db",			"P=$(CAM_PV):SER,R=:AsynIO,PORT=$(CAM_PORT).SER,ADDR=0,IMAX=0,OMAX=0" )

# Load camera model specific db
dbLoadRecords(	"db/$(MODEL).db",			"P=$(CAM_PV),R=:,PORT=$(CAM_PORT),PWIDTH=$(TRIG_PV):TWID,PW_RBV=$(TRIG_PV):BW_TWIDCALC" )

# Load history records
#dbLoadRecords(	"db/bld_hist.db",			"P=$(CAM_PV),R=:" )
dbLoadRecords(	"db/edtCam_hist.db",		"P=$(CAM_PV),R=:" )

#
#
#
# Comment/uncomment/change delay as desired so you can see remaining camera dbLoad msgs
epicsThreadSleep $(ST_CMD_DELAYS)

# Configure and load any desired viewers
< db/MonoFullViewer.cmd
< db/MonoBin2Viewer.cmd
#< db/MonoBin4Viewer.cmd

# Configure and load any additional plugins, if any
epicsEnvSet(	"N",					"1" )
epicsEnvSet(	"PLUGIN_SRC",			"CAM" )
< db/$(PLUGINS).cmd
#< db/pluginStats.cmd

# 
# Initialize the IOC and start processing records
# 
iocInit()

# Create archive files from info directives
# makeArchiveFileFromDbInfo( "$(IOC_DATA)/$(IOC)/archive/$(IOC).archive", "archiveFields" )

# Create autosave files from info directives
makeAutosaveFileFromDbInfo( "$(IOC_DATA)/$(IOC)/autosave/autoSettings.req", "autosaveFields" )

# Start autosave backups
create_monitor_set( "autoSettings.req", 5, "" )
create_monitor_set( "$(IOC).req", 5, "" )

# All IOCs should dump some common info after initial startup.
< /reg/d/iocCommon/All/post_linux.cmd

# Final delay before auto-start image acquisition
epicsThreadSleep $(ST_CMD_DELAYS)
epicsThreadSleep $(ST_CMD_DELAYS)
dbpf $(CAM_PV):Acquire 1

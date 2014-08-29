#!../../bin/linux-x86_64/edt

< envPaths
# IOC macro should be the ioc name, used as
# part of the path to the autosave directory
# and in the autosave restoreSet name.
epicsEnvSet( "ENGINEER",	"Bruce Hill (bhill)" )
epicsEnvSet( "LOCATION",	"Test Lab" )
epicsEnvSet( "IOCSH_PS1",	"$(IOC)> " )

#
# Installation specific ioc instance configuration
#

# PV Prefixes
epicsEnvSet( "IOC_PV",	"TST:IOC:EDT:ORCA1" )
epicsEnvSet( "EVR_PV",	"TST:EVR:EDT:ORCA1" )
epicsEnvSet( "CAM_PV",	"TST:EDT:ORCA1" )

# Configure EVR
epicsEnvSet( "EVR_CARD",	"0" )
# EVR Type: 0=VME, 1=PMC, 15=SLAC
#epicsEnvSet( "EVR_TYPE",	"1" )
epicsEnvSet( "EVR_TYPE",	"15" )

# Specify camera model, asyn CAM_PORT, Mpeg HTTP_PORT,
# and additional plugins, if desired
epicsEnvSet( "MODEL",		"hamaOrcaFlash4_0" )
epicsEnvSet( "EPICS_CA_MAX_ARRAY_BYTES", "20000000" )
epicsEnvSet( "HTTP_PORT",	"7800" )
epicsEnvSet( "MJPG_PORT",	"8081"	)
#epicsEnvSet( "PLUGINS",		"pcdsPlugins" )

# Pixel resolution and offsets
epicsEnvSet( "RESOLUTION",	"1.0"	)
epicsEnvSet( "EGU",			"?"	)
epicsEnvSet( "X_OFF",		"0"	)
epicsEnvSet( "Y_OFF",		"0"	)

# Comment/uncomment/change diagnostic settings as desired
epicsEnvSet( "CAM_TRACE_MASK",    "1" )
epicsEnvSet( "CAM_TRACE_IO_MASK", "1" )
epicsEnvSet( "SER_TRACE_MASK",    "9" )
epicsEnvSet( "SER_TRACE_IO_MASK", "1" )
epicsEnvSet( "ST_CMD_DELAYS", "8" )

# Do we need these?
# epicsEnvSet("PREFIX", "TST:EDT")
# epicsEnvSet("CAM",  "ORCA")
# epicsEnvSet( "PORT", "CAM")
# epicsEnvSet("ID",   "1")
# epicsEnvSet("TYPE",  "Int16")
# epicsEnvSet("FTVL",  "SHORT")
# epicsEnvSet("XSIZE",  "2048")
# epicsEnvSet("YSIZE",  "2048")
# epicsEnvSet("NELEMENTS", "4198400")
# epicsEnvSet("QSIZE",  "10")
# epicsEnvSet("BLOCKING",  "1")
# epicsEnvSet("NCHANS", "2048")
cd( "../.." )

# Run common startup commands for linux soft IOC's
< /reg/d/iocCommon/All/pre_linux.cmd

# Register all support components
dbLoadDatabase("dbd/edt.dbd")
edt_registerRecordDeviceDriver(pdbbase)

# Set iocsh debug variables
var EDT_PDV_DEBUG 2

# Load standard soft ioc database
dbLoadRecords( "db/iocSoft.db",				"IOC=$(IOC_PV)" )

# Setup autosave
dbLoadRecords( "db/save_restoreStatus.db",	"IOC=$(IOC_PV)" )
set_savefile_path( "$(IOC_DATA)/$(IOC)/autosave" )
set_requestfile_path( "$(TOP)/autosave" )
save_restoreSet_status_prefix( "$(IOC_PV):" )
save_restoreSet_IncompleteSetsOk( 1 )
save_restoreSet_DatedBackupFiles( 1 )
set_pass0_restoreFile( "$(IOC).sav" )
set_pass1_restoreFile( "$(IOC).sav" )

# $$ commands for when we go to templated st.cmd
# $$LOOP(EVR)
#
#
#
# Configure a PMC EVR
ErConfigure( $(EVR_CARD), 0, 0, 0, $(EVR_TYPE) )
dbLoadRecords( "db/evrPmc230.db",			"EVR=TST:EVR:EDT:ORCA1,CARD=$(EVR_CARD)" )

#
#
#
# Comment/uncomment/change delay as desired so you can see EVR messages during boot
#epicsThreadSleep $(ST_CMD_DELAYS)
# $$LOOP(EVR)

< db/$(MODEL).env

# Default edtPdvDriver settings
epicsEnvSet(	"CAM_PORT",					"CAM" )
epicsEnvSet(	"STREAM_PROTOCOL_PATH",		"db" )

#
#
#
# =========================================================
# Configure an edtPdv driver for the specified camera model
# =========================================================
edtPdvConfig( "$(CAM_PORT)", 0, 0, "$(MODEL)" )
# registerUserTimeStampSource( "$(CAM_PORT)", "TimeStampSource" )

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
dbLoadRecords(	"db/edtPdvCamera.db",		"CAM=$(CAM_PV),CAM_PORT=$(CAM_PORT)" )
#dbLoadRecords(	"db/timeStampSource.db",	"DEV=$(PREFIX):$(CAM_PORT),PORT=$(CAM_PORT)" )

# For camera serial asyn diagnostics
# (AreaDetector plugins each have their own AsynIO record)
dbLoadRecords(	"db/asynRecord.db",			"P=$(CAM_PV):SER,R=:AsynIO,PORT=$(CAM_PORT).SER,ADDR=0,IMAX=0,OMAX=0" )

# Load camera model specific db
dbLoadRecords(	"db/$(MODEL).db",			"P=$(CAM_PV),R=:,PORT=$(CAM_PORT)" )
#
#
#
# Comment/uncomment/change delay as desired so you can see remaining camera dbLoad msgs
epicsThreadSleep $(ST_CMD_DELAYS)

#
# NOTE: Using comment enabled scripts is great for configurable db, but
# 	for autosave, look into automatic autosave generation using info directives.
# TODO: Create a tool to do the same for archive files
#	Should just append to existing archive file if PV not already in there
#

# Configure and load any desired datastreams
#< db/FileDataStream.cmd
#< db/MPEGDataStream.cmd
#< db/FFMPGDataStream.cmd

# Configure and load any desired viewers
< db/MonoFullViewer.cmd
#< db/MonoBin2Viewer.cmd
#< db/MonoBin3Viewer.cmd
< db/MonoBin4Viewer.cmd
#< db/FalseColorViewer.cmd
#< db/ColorFullViewer.cmd
#< db/ColorBin2Viewer.cmd
#< db/ColorBin3Viewer.cmd
#< db/ColorBin4Viewer.cmd

# Configure and load the selected plugins, if any
#< db/$(PLUGINS).cmd

#
#
#
# Comment/uncomment/change delay as desired so you can see remaining messages before iocInit
epicsThreadSleep $(ST_CMD_DELAYS)
# 
# Initialize the IOC and start processing records
# 
iocInit()

# Start autosave backups
create_monitor_set( "$(IOC).req", 5, "" )

# All IOCs should dump some common info after initial startup.
< /reg/d/iocCommon/All/post_linux.cmd

dbpf TST:EDT:ORCA1:IMAGE1:EnableCallbacks 1
dbpf TST:EDT:ORCA1:ROI5:EnableCallbacks 1
dbpf TST:EDT:ORCA1:THUMBNAIL:EnableCallbacks 1
dbpf TST:EDT:ORCA1:ROI7:EnableCallbacks 1
dbpf TST:EDT:ORCA1:ROI7:AcquireMode 1


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
epicsEnvSet( "TRIG_PV",	"$(EVR_PV):TRIG0" )
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
#epicsEnvSet( "PLUGINS",     "pcdsPlugins" )
epicsEnvSet( "PLUGINS",		"commonPlugins" )

# Comment/uncomment/change diagnostic settings as desired
epicsEnvSet( "CAM_TRACE_MASK",    "9" )
epicsEnvSet( "CAM_TRACE_IO_MASK", "1" )
epicsEnvSet( "SER_TRACE_MASK",    "9" )
epicsEnvSet( "SER_TRACE_IO_MASK", "1" )
epicsEnvSet( "ST_CMD_DELAYS", 	  "1" )

cd( "../.." )

# Run common startup commands for linux soft IOC's
< /reg/d/iocCommon/All/pre_linux.cmd

# Register all support components
dbLoadDatabase("dbd/edt.dbd")
edt_registerRecordDeviceDriver(pdbbase)

# Set iocsh debug variables
var DEBUG_EDT_PDV 3
var DEBUG_TS_FIFO 1

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
ErConfigure( $(EVR_CARD), 0, 0, 0, $(EVR_TYPE) )
#dbLoadRecords( "db/evrPmc230.db",			"EVR=TST:EVR:EDT:ORCA1,CARD=$(EVR_CARD),IP0E=Enabled" )
dbLoadRecords( "db/evrSLAC.db",			"EVR=TST:EVR:EDT:ORCA1,CARD=$(EVR_CARD),IP0E=Enabled" )

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
edtPdvConfig( "$(CAM_PORT)", 0, 0, "$(MODEL)", "$(CAM_MODE)" )
#registerUserTimeStampSource( "$(CAM_PORT)", "TimeStampSource" )

# Set asyn trace flags
asynSetTraceMask(   "$(CAM_PORT)",		0, $(CAM_TRACE_MASK) )
asynSetTraceIOMask( "$(CAM_PORT)",		0, $(CAM_TRACE_IO_MASK) )
asynSetTraceFile(	"$(CAM_PORT)",		0, "$(IOC_DATA)/$(IOC)/$(CAM_PORT).log" )
asynSetTraceMask(   "$(CAM_PORT).SER",	0, $(SER_TRACE_MASK) )
asynSetTraceIOMask( "$(CAM_PORT).SER",	0, $(SER_TRACE_IO_MASK) )
#asynSetTraceFile(	"$(CAM_PORT).SER",	0, "$(IOC_DATA)/$(IOC)/$(CAM_PORT).SER.log" )

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
dbLoadRecords(	"db/bld_hist.db",			"P=$(CAM_PV),R=:" )
dbLoadRecords(	"db/edtCam_hist.db",		"P=$(CAM_PV),R=:" )

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
< db/MonoBin2Viewer.cmd
#< db/MonoBin3Viewer.cmd
< db/MonoBin4Viewer.cmd
#< db/FalseColorViewer.cmd
#< db/ColorFullViewer.cmd
#< db/ColorBin2Viewer.cmd
#< db/ColorBin3Viewer.cmd
#< db/ColorBin4Viewer.cmd

asynSetTraceMask(   "ROI6",		1, $(CAM_TRACE_MASK) )
asynSetTraceIOMask( "ROI6",		1, $(CAM_TRACE_IO_MASK) )

# Configure and load the BLD plugin
epicsEnvSet(	"N",					"1" )
epicsEnvSet(	"PLUGIN_SRC",			"CAM" )
< setupScripts/pluginBldSpectrometer.cmd

# Configure and load any additional plugins, if any
epicsEnvSet(	"N",					"1" )
epicsEnvSet(	"PLUGIN_SRC",			"CAM" )
< db/$(PLUGINS).cmd
#< setupScripts/pluginStats.cmd

# Create a TIFF plugin, set it to get data from the camera
epicsEnvSet( "PLUGIN_SRC", "$(CAM_PORT)" )
epicsEnvSet( "N", "1" )
#< setupScripts/pluginTIFF.cmd

#
#
#
# Comment/uncomment/change delay as desired so you can see remaining messages before iocInit
epicsThreadSleep $(ST_CMD_DELAYS)
# 
# Initialize the IOC and start processing records
# 
iocInit()
epicsThreadSleep $(ST_CMD_DELAYS)
epicsThreadSleep $(ST_CMD_DELAYS)
epicsThreadSleep $(ST_CMD_DELAYS)


# Create autosave files from info directives
makeAutosaveFileFromDbInfo( "$(IOC_DATA)/$(IOC)/autosave/autoSettings.req", "autosaveFields" )

# Start autosave backups
create_monitor_set( "autoSettings.req", 5, "" )
create_monitor_set( "$(IOC).req", 5, "" )

# All IOCs should dump some common info after initial startup.
< /reg/d/iocCommon/All/post_linux.cmd
epicsThreadSleep $(ST_CMD_DELAYS)
epicsThreadSleep $(ST_CMD_DELAYS)
epicsThreadSleep $(ST_CMD_DELAYS)

# Configure the BLD client
epicsEnvSet( "BLD_XTC",		"0x10048" )	# XTC Type, Id_Spectrometer
epicsEnvSet( "BLD_SRC",		"46" )		# Src Id, FeeSpec0
epicsEnvSet( "BLD_IP",		"239.255.24.$(BLD_SRC)" )
epicsEnvSet( "BLD_PORT",	"10148" )
epicsEnvSet( "BLD_MAX",		"8980" )	# 9000 MTU - 20 byte header
BldConfigSend( "$(BLD_IP)", $(BLD_PORT), $(BLD_SRC), $(BLD_XTC), $(BLD_MAX) )
BldStart()
BldIsStarted()
epicsThreadSleep $(ST_CMD_DELAYS)
epicsThreadSleep $(ST_CMD_DELAYS)

# Hit Ctrl-C to defeat auto-start
#
# 3 seconds to auto-start of image acquisition
#
epicsThreadSleep $(ST_CMD_DELAYS)
#
# 2 seconds to auto-start of image acquisition
#
epicsThreadSleep $(ST_CMD_DELAYS)
#
# 1 seconds to auto-start of image acquisition
#
epicsThreadSleep $(ST_CMD_DELAYS)
dbpf $(CAM_PV):Acquire 1

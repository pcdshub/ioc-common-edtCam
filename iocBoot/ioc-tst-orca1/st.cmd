#!../../bin/linux-x86_64/orca

< envPaths
epicsEnvSet( "ENGINEER",	"Bruce Hill (bhill)" )
epicsEnvSet( "LOCATION",	"TST:IOC:GIGE:ORCA1" )
epicsEnvSet( "EPICS_CA_MAX_ARRAY_BYTES", "20000000" )
epicsEnvSet( "IOCSH_PS1",	"$(IOC)> " )
epicsEnvSet( "IOC_PV",		"TST:IOC:GIGE:ORCA1" )
epicsEnvSet( "EVR_PV",		"TST:EVR:GIGE:ORCA1" )
epicsEnvSet( "EVR_CARD",	"0" )
# EVR Type: 0=VME, 1=PMC, 15=SLAC
epicsEnvSet( "EVR_TYPE",	"1" )
cd( "../.." )

# Run common startup commands for linux soft IOC's
< /reg/d/iocCommon/All/pre_linux.cmd

# Register all support components
dbLoadDatabase("dbd/edt.dbd")
edt_registerRecordDeviceDriver(pdbbase)

# ORCA specific env variables
epicsEnvSet("PREFIX", "TST:GIGE")
epicsEnvSet("PORT", "CAM")
epicsEnvSet("CAM",  "ORCA")
epicsEnvSet("ID",   "1")
epicsEnvSet("TYPE",  "Int16")
epicsEnvSet("FTVL",  "SHORT")
epicsEnvSet("XSIZE",  "2048")
epicsEnvSet("YSIZE",  "2048")
epicsEnvSet("NELEMENTS", "4198400")
epicsEnvSet("QSIZE",  "10")
epicsEnvSet("BLOCKING",  "1")
epicsEnvSet("NCHANS", "2048")
epicsEnvSet("MJPG_PORT", "8081")
epicsEnvSet("X_OFF", "-3247")
epicsEnvSet("Y_OFF", "-2435")
epicsEnvSet("RESOLUTION", "9.9")
epicsEnvSet("EGU", "um")

epicsEnvSet( "MODEL",	"OrcaFlash4-0" )
epicsEnvSet( "CAM",		"TST:GIGE:ORCA1" )
epicsEnvSet( "PLUGINS", "pcdsPlugins" )
epicsEnvSet( "CAM_PORT","CAM" )
epicsEnvSet( "HTTP_PORT","7800" )

# Setup asyn tracing if specified
epicsEnvSet( "TRACE_MASK",    "9" )
epicsEnvSet( "TRACE_IO_MASK", "2" )

< setupScripts/$(MODEL).env

# TODO: Consider moving the following to setupScripts/orcaFlash4-0.cmd
# ====================================================
# Connect to Camera
# ====================================================
edtPdvConfig( "$(PORT)", "OrcaFlash4.0", 50, -1,0, 1000000)
# registerUserTimeStampSource("$(PORT)", "TimeStampSource")

# Optional delay so we can see camera config messages during boot
epicsThreadSleep 1

# Load orca db files
dbLoadRecords( "db/orcaFlash.db", "CAM=$(CAM),CAM_PORT=$(CAM_PORT)" )
#dbLoadRecords( "db/orca.template", "P=$(PREFIX):,R=$(PORT):,PORT=$(PORT),ADDR=0,TIMEOUT=1" )
dbLoadRecords( "db/timeStampSource.db", "DEV=$(PREFIX):$(PORT),PORT=$(PORT)" )
# < setupScripts/orcaFlash4-0.cmd

# Set asyn trace flags
asynSetTraceMask(   "$(CAM_PORT)", $(TRACE_MASK) )
asynSetTraceIOMask( "$(CAM_PORT)", $(TRACE_IO_MASK) )

# Configure and load the plugins
< setupScripts/$(PLUGINS).cmd

# Optional delay so we can see EVR messages during boot
epicsThreadSleep 1

# Configure a PMC EVR
ErConfigure( $(EVR_CARD), 0, 0, 0, $(EVR_TYPE) )

# Optional delay so we can see EVR messages during boot
epicsThreadSleep 2

# Load record instances
dbLoadRecords( "db/iocSoft.db",				"IOC=TST:IOC:GIGE:ORCA1" )
dbLoadRecords( "db/save_restoreStatus.db",	"IOC=TST:IOC:GIGE:ORCA1" )
dbLoadRecords( "db/evrPmc230.db",			"EVR=TST:EVR:GIGE:ORCA1,CARD=$(EVR_CARD)" )

# Setup autosave
set_savefile_path( "$(IOC_DATA)/$(IOC)/autosave" )
set_requestfile_path( "$(TOP)/autosave" )
save_restoreSet_status_prefix( "TST:IOC:GIGE:ORCA1:" )
save_restoreSet_IncompleteSetsOk( 1 )
save_restoreSet_DatedBackupFiles( 1 )
set_pass0_restoreFile( "$(IOC).sav" )
set_pass1_restoreFile( "$(IOC).sav" )

# Initialize the IOC and start processing records
iocInit()

# Start autosave backups
create_monitor_set( "$(IOC).req", 5, "" )

# All IOCs should dump some common info after initial startup.
< /reg/d/iocCommon/All/post_linux.cmd

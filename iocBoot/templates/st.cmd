#!$$IOCTOP/bin/linux-x86_64/gige

# Run common startup commands for linux soft IOC's
< /reg/d/iocCommon/All/pre_linux.cmd

< envPaths
epicsEnvSet("IOCNAME", "$$IOCNAME")
epicsEnvSet("ENGINEER", "$$ENGINEER" )
epicsEnvSet("LOCATION", "$$LOCATION" )
epicsEnvSet("IOCSH_PS1", "$(IOCNAME)> " )
epicsEnvSet("EPICS_CA_MAX_ARRAY_BYTES", "$$IF(MAX_ARRAY,$$MAX_ARRAY,8000000)" )
epicsEnvSet("IOC_PV", "$$IOC_PV")
epicsEnvSet("IOCTOP", "$$IOCTOP")
epicsEnvSet("TOP", "$$TOP")

# Network name or IP addr for gigE camera
epicsEnvSet( "CAM_IP",		"$$CAM_IP" )

# PV prefix for gigE camera
epicsEnvSet( "CAM",			"$$CAM" )

# Choose camera model from setupScripts/$(MODEL).env 
epicsEnvSet( "MODEL",		"$$MODEL" )

# Choose which plugin's to use from setupScripts/$(PLUGINS).cmd 
# If you create a new one, please name it like xyzPlugins.cmd
epicsEnvSet( "PLUGINS",		"$$PLUGINS" )

# Choose an HTTP port number for the camera's MPEG stream
# All gigE camera's on the same host must have unique port numbers
epicsEnvSet( "HTTP_PORT",	"$$HTTP_PORT" )

# Setup asyn tracing if specified
epicsEnvSet( "TRACE_MASK",    "$$IF(TRACE_MASK,$$TRACE_MASK,1)" )
epicsEnvSet( "TRACE_IO_MASK", "$$IF(TRACE_IO_MASK,$$TRACE_IO_MASK,0)" )

# EVR not supported yet
epicsEnvSet( "EVR_ENABLED",	"#" )
epicsEnvSet( "EVR_PV",	"" )

cd( "$(IOCTOP)" )

##############################################################
#
# The remainder of the script should be common for all Prosilica gigE cameras
#

# Register all support components
dbLoadDatabase( "dbd/gige.dbd" )
gige_registerRecordDeviceDriver(pdbbase)

# Setup the environment for the specified gigE camera model
< setupScripts/$(MODEL).env

# Configure and load a Prosilica camera
< setupScripts/prosilica.cmd

# Set asyn trace flags
asynSetTraceMask(   "$(CAM_PORT)", 0, $(TRACE_MASK) )
asynSetTraceIOMask( "$(CAM_PORT)", 0, $(TRACE_IO_MASK) )

# Configure and load the plugins
< setupScripts/$(PLUGINS).cmd

# Initialize EVR
$(EVR_ENABLED) ErDebugLevel( 0 )
$(EVR_ENABLED) ErConfigure( 0, 0, 0, 0, 1 )
$(EVR_ENABLED) dbLoadRecords( "db/evrPmc230.db",  "IOC=$(IOC_PV),EVR=$(EVR_PV),EVRFLNK=" )

# Load soft ioc related record instances
dbLoadRecords( "db/iocAdmin.db",			"IOC=$(IOC_PV)" )
dbLoadRecords( "db/save_restoreStatus.db",	"IOC=$(IOC_PV)" )

# Setup autosave
set_savefile_path( "$(IOC_DATA)/$(IOC)/autosave" )
set_requestfile_path( "$(TOP)/autosave" )
save_restoreSet_status_prefix( "$(IOC_PV):" )
save_restoreSet_IncompleteSetsOk( 1 )
save_restoreSet_DatedBackupFiles( 1 )
set_pass0_restoreFile( "$(IOC).sav" )
set_pass1_restoreFile( "$(IOC).sav" )

save_restoreSet_NumSeqFiles(5)
save_restoreSet_SeqPeriodInSeconds(30)

# Initialize the IOC and start processing records
iocInit()

# TODO: Remove these dbpf calls if possible
# Enable callbacks
dbpf $(CAM):ArrayCallbacks 1

# Start acquiring images
dbpf $(CAM):Acquire 1                         # Start the camera

# ----------

# Start autosave backups
create_monitor_set( "$(IOC).req", 5, "CAM=$(CAM)" )

# All IOCs should dump some common info after initial startup.
< /reg/d/iocCommon/All/post_linux.cmd

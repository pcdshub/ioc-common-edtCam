#!$$IOCTOP/bin/$$TARGET_ARCH/edt

# Run common startup commands for linux soft IOC's
< $(IOC_COMMON)/All/pre_linux.cmd

< envPaths
epicsEnvSet( "IOCNAME",      "$$IOCNAME" )
epicsEnvSet( "ENGINEER",     "$$ENGINEER" )
epicsEnvSet( "LOCATION",     "$$LOCATION" )
epicsEnvSet( "IOCSH_PS1",    "$(IOCNAME)> " )
epicsEnvSet( "IOC_PV",       "$$IOC_PV" )
epicsEnvSet( "IOCTOP",       "$$IOCTOP" )
epicsEnvSet( "BUILD_TOP",    "$$TOP" )
cd( "$(IOCTOP)" )

# Set Max array size
epicsEnvSet( "EPICS_CA_MAX_ARRAY_BYTES", "$$IF(MAX_ARRAY,$$MAX_ARRAY,20000000)" )

# Setup EVR env vars
epicsEnvSet( "EVR_PV",       "$$EVR_PV" )
epicsEnvSet( "EVR_CARD",     "$$IF(EVR_CARD,$$EVR_CARD,0)" )
# EVR Type: 0=VME, 1=PMC, 15=SLAC
epicsEnvSet( "EVRID_PMC",    "1" )
epicsEnvSet( "EVRID_SLAC",   "15" )
epicsEnvSet( "EVRDB_PMC",    "db/evrPmc230.db" )
epicsEnvSet( "EVRDB_SLAC",   "db/evrSLAC.db" )
epicsEnvSet( "EVRID",        "$(EVRID_$$EVR_TYPE)" )
epicsEnvSet( "EVRDB",        "$(EVRDB_$$EVR_TYPE)" )
epicsEnvSet( "EVR_DEBUG",    "$$IF(EVR_DEBUG,$$EVR_DEBUG,0)" )

# Specify camera env variables
epicsEnvSet( "CAM_PV",       "$$CAM_PV" )
epicsEnvSet( "CAM_PORT",     "$$IF(PORT,$$PORT,CAM)" )
epicsEnvSet( "TRIG_NUM",     "$$TRIG" )
epicsEnvSet( "TRIG_PV",      "$$(EVR_PV):TRIG$$TRIG" )
epicsEnvSet( "MODEL",        "$$MODEL" )
epicsEnvSet( "HTTP_PORT",    "$$IF(HTTP_PORT,$$HTTP_PORT,7800)" )
epicsEnvSet( "MJPG_PORT",    "$$IF(MJPG_PORT,$$MJPG_PORT,8081)" )

# Default edtPdvDriver settings
epicsEnvSet( "EDT_CARD",     		"$$IF(BOARD,$$BOARD,0)" )
epicsEnvSet( "EDT_CHAN",     		"$$IF(CHAN,$$CHAN,0)" )
epicsEnvSet( "CAM_MODE",             "$$IF(MODE,$$MODE,Base)" )
epicsEnvSet( "STREAM_PROTOCOL_PATH", "$$IF(STREAM_PROTOCOL_PATH,$$STREAM_PROTOCOL_PATH,db)" )

# Diagnostic settings
epicsEnvSet( "ST_CMD_DELAYS",		"$$IF(ST_CMD_DELAYS,$$ST_CMD_DELAYS,1)" )
epicsEnvSet( "CAM_TRACE_MASK",		"$$IF(CAM_TRACE,$$CAM_TRACE,1)" )
epicsEnvSet( "CAM_TRACE_IO_MASK",	"$$IF(CAM_TRACE_IO,$$CAM_TRACE_IO,0)" )
epicsEnvSet( "SER_TRACE_IO_MASK",	"$$IF(SER_TRACE_IO,$$SER_TRACE_IO,1)" )

# Register all support components
dbLoadDatabase( "dbd/edt.dbd" )
edt_registerRecordDeviceDriver(pdbbase)

# Set iocsh debug variables
var DEBUG_TS_FIFO 1
var DEBUG_EDT_PDV 2

# Setup the environment for the specified camera model
< db/$(MODEL).env

# =========================================================
# Configure an edtPdv driver for the specified camera model
# =========================================================
epicsEnvSet( "CAM_TYPE", "edt" )
edtPdvConfig( "$(CAM_PORT)", "$(EDT_CARD)", "$(EDT_CHAN)", "$(MODEL)", "$(CAM_MODE)" )

# Set asyn trace flags
asynSetTraceMask(   "$(CAM_PORT)",      0, $(CAM_TRACE_MASK) )
asynSetTraceIOMask( "$(CAM_PORT)",      0, $(CAM_TRACE_IO_MASK) )
asynSetTraceMask(   "$(CAM_PORT).SER",  0, $(SER_TRACE_MASK) )
asynSetTraceIOMask( "$(CAM_PORT).SER",  0, $(SER_TRACE_IO_MASK) )

$$IF(USE_TRACE_FILES)
asynSetTraceFile(	"$(CAM_PORT)",		0, "$(IOC_DATA)/$(IOC)/$(CAM_PORT).log" )
asynSetTraceFile(	"$(CAM_PORT).SER",	0, "$(IOC_DATA)/$(IOC)/$(CAM_PORT).SER.log" )
$$ENDIF(USE_TRACE_FILES)

$$IF(NO_ST_CMD_DELAY)
$$ELSE(NO_ST_CMD_DELAY)
# Comment/uncomment/change delay as desired so you can see messages during boot
epicsThreadSleep $(ST_CMD_DELAYS)
$$ENDIF(NO_ST_CMD_DELAY)

# Configure and load standard edtPdv camera database
dbLoadRecords("db/edtPdvCamera.db", "CAM=$(CAM_PV),CAM_PORT=$(CAM_PORT),CAM_TRIG=$(TRIG_PV),EVR=$(EVR_PV)" )
dbLoadRecords("db/edtCam_hist.db",  "P=$(CAM_PV),R=:" )

# For camera serial asyn diagnostics
# (AreaDetector plugins each have their own AsynIO record)
dbLoadRecords("db/asynRecord.db",   "P=$(CAM_PV):SER,R=:AsynIO,PORT=$(CAM_PORT).SER,ADDR=0,IMAX=0,OMAX=0" )

# Load camera model specific db
dbLoadRecords("db/$(MODEL).db",     "P=$(CAM_PV),R=:,PORT=$(CAM_PORT),PWIDTH=$(TRIG_PV):TWID,PW_RBV=$(TRIG_PV):BW_TWIDCALC" )

# Load timestamp plugin
dbLoadRecords("db/timeStampFifo.template",  "DEV=$(CAM_PV):TSS,PORT_PV=$(CAM_PV):PortName_RBV,EC_PV=$(EVR_PV):EVENT1CTRL.ENM,DLY_PV=$(CAM_PV):TrigToTS_Calc NMS CPP" )

# Load history records
$$IF(BLD_SRC)
dbLoadRecords("db/bld_hist.db",     "P=$(CAM_PV),R=:" )
$$ENDIF(BLD_SRC)

$$IF(NO_ST_CMD_DELAY)
$$ELSE(NO_ST_CMD_DELAY)
epicsThreadSleep $(ST_CMD_DELAYS)
$$ENDIF(NO_ST_CMD_DELAY)

# Provide some reasonable defaults for plugin macros
# May be overridden by $(PLUGINS).cmd
epicsEnvSet( "PLUGIN_SRC", "$(CAM_PORT)" )
epicsEnvSet( "N", "1" )
epicsEnvSet( "QSIZE", "5" )

# Configure and load any desired datastreams
$$LOOP(DATASTREAM)
< db/$$(NAME)Stream.cmd
$$ENDLOOP(DATASTREAM)

# Configure and load any desired viewers
$$LOOP(VIEWER)
epicsEnvSet( "IMAGE_NAME",   "$$IF(IMAGE_NAME,$$IMAGE_NAME,IMAGE1)" )
< db/$$(NAME)Viewer.cmd
$$ENDLOOP(VIEWER)

# Configure and load the selected plugins, if any
$$LOOP(PLUGIN)
epicsEnvSet( "N",            "$$IF(NUM,$$NUM,1)" )
epicsEnvSet( "PLUGIN_SRC",   "$$IF(SRC,$$SRC,CAM)" )
< db/plugin$$(NAME).cmd
$$ENDLOOP(PLUGIN)

# Configure and load BLD plugin
$$LOOP(BLD)
epicsEnvSet( "N",            "$$CALC{INDEX+1}" )
epicsEnvSet( "PLUGIN_SRC",   "CAM" )
< db/pluginBldSpectrometer.cmd
$$ENDLOOP(BLD)

$$IF(NO_ST_CMD_DELAY)
$$ELSE(NO_ST_CMD_DELAY)
epicsThreadSleep $(ST_CMD_DELAYS)
$$ENDIF(NO_ST_CMD_DELAY)

# Configure the EVR
ErDebugLevel( 0 )
ErConfigure( $(EVR_CARD), 0, 0, 0, $(EVRID_$$TYPE) )
dbLoadRecords( "$(EVRDB)", "IOC=$(IOC_PV),EVR=$(EVR_PV),CARD=$(EVR_CARD),$$IF(TRIG)IP$$(TRIG)E=Enabled,$$ENDIF(TRIG)$$LOOP(EXTRA_TRIG)IP$$(TRIG)E=Enabled,$$ENDLOOP(EXTRA_TRIG)" )

# Load soft ioc related record instances
dbLoadRecords( "db/iocSoft.db",				"IOC=$(IOC_PV)" )

# Setup autosave
dbLoadRecords( "db/save_restoreStatus.db",	"IOC=$(IOC_PV)" )
set_savefile_path( "$(IOC_DATA)/$(IOC)/autosave" )
set_requestfile_path( "$(BUILD_TOP)/autosave" )
# Also look in the iocData autosave folder for auto generated req files
set_requestfile_path( "$(IOC_DATA)/$(IOC)/autosave" )
save_restoreSet_status_prefix( "$(IOC_PV):" )
save_restoreSet_IncompleteSetsOk( 1 )
save_restoreSet_DatedBackupFiles( 1 )
set_pass0_restoreFile( "autoSettings.sav" )
set_pass0_restoreFile( "$(IOC).sav" )
set_pass1_restoreFile( "autoSettings.sav" )
set_pass1_restoreFile( "$(IOC).sav" )

#
# Initialize the IOC and start processing records
#
iocInit()

# Create autosave files from info directives
makeAutosaveFileFromDbInfo( "$(IOC_DATA)/$(IOC)/autosave/autoSettings.req", "autosaveFields" )

# Start autosave backups
create_monitor_set( "autoSettings.req",  5,  "" )
create_monitor_set( "$(IOCNAME).req",    5,  "" )

# All IOCs should dump some common info after initial startup.
< $(IOC_COMMON)/All/post_linux.cmd

$$LOOP(BLD)
# Configure the BLD client
epicsEnvSet( "BLD_XTC",     "$$IF(XTC,$$XTC,0x10048)" ) # XTC Type, Id_Spectrometer
epicsEnvSet( "BLD_SRC",     "$$SRC" ) # Src Id
epicsEnvSet( "BLD_IP",      "239.255.24.$(BLD_SRC)" )
epicsEnvSet( "BLD_PORT",    "$$IF(PORT,$$PORT,10148)" )
epicsEnvSet( "BLD_MAX",     "$$IF(MAX,$$MAX,8980)" )    # 9000 MTU - 20 byte header
BldConfigSend( "$(BLD_IP)", $(BLD_PORT), $(BLD_MAX) )
BldStart()
BldIsStarted()
$$ENDLOOP(BLD)

$$IF(NO_ST_CMD_DELAY)
$$ELSE(NO_ST_CMD_DELAY)
# Final delay before auto-start image acquisition
epicsThreadSleep $(ST_CMD_DELAYS)
epicsThreadSleep $(ST_CMD_DELAYS)
$$ENDIF(NO_ST_CMD_DELAY)

# TODO: Remove these dbpf calls if possible
# Enable callbacks
# dbpf $(CAM):ArrayCallbacks 1

$$IF(AUTO_START)
dbpf $(CAM_PV):Acquire $$AUTO_START
$$ENDIF(AUTO_START)

#!$$IOCTOP/bin/$$IF(ARCH,$$ARCH,linux-x86_64)/edt

< envPaths
epicsEnvSet("IOCNAME",      "$$IOCNAME")
epicsEnvSet("ENGINEER",     "$$ENGINEER")
epicsEnvSet("LOCATION",     "$$LOCATION")
epicsEnvSet("IOCSH_PS1",    "$(IOCNAME)> ")
epicsEnvSet("IOC_PV",       "$$IOC_PV")
epicsEnvSet("IOCTOP",       "$$IOCTOP")
epicsEnvSet("TOP",          "$$TOP")

# Set Max array size
epicsEnvSet("EPICS_CA_MAX_ARRAY_BYTES", "$$IF(MAX_ARRAY,$$MAX_ARRAY,20000000)" )

# Configure EVR
$$LOOP(EVR)
epicsEnvSet("EVR_PV",       "$$NAME")
epicsEnvSet("EVR_CARD",     "$$IF(CARD,$$CARD,0)")
# EVR Type: 0=VME, 1=PMC, 15=SLAC
epicsEnvSet("EVRID_PMC",    "1")
epicsEnvSet("EVRID_SLAC",   "15")
epicsEnvSet("EVRDB_PMC",    "db/evrPmc230.db")
epicsEnvSet("EVRDB_SLAC",   "db/evrSLAC.db")
epicsEnvSet("EVRID",        "$(EVRID_$$TYPE)")
epicsEnvSet("EVRDB",        "$(EVRDB_$$TYPE)")
epicsEnvSet("EVR_DEBUG",    "$$IF(DEBUG,$$DEBUG,0)")
$$ENDLOOP(EVR)

# Specify camera model, asyn CAM_PORT, Mpeg HTTP_PORT,
# and additional plugins, if desired
$$LOOP(CAMERA)
epicsEnvSet("CAM_PV",       "$$NAME")
epicsEnvSet("TRIG_NUM",     "$$TRIG")
epicsEnvSet("TRIG_PV",      "$(EVR_PV):TRIG$$TRIG")
epicsEnvSet("MODEL",        "$$TYPE")
epicsEnvSet("EDT_CARD",     "$$IF(BOARD,$$BOARD,0)")
epicsEnvSet("EDT_CHAN",     "$$CHAN")
epicsEnvSet("HTTP_PORT",    "$$IF(HTTP_PORT,$$HTTP_PORT,7800)")
epicsEnvSet("MJPG_PORT",    "$$IF(MJPG_PORT,$$MJPG_PORT,8081)")

# Default edtPdvDriver settings
epicsEnvSet("CAM_PORT",             "$$IF(PORT,$$PORT,CAM)")
epicsEnvSet("CAM_MODE",             "$$IF(MODE,$$MODE,Base)")
epicsEnvSet("STREAM_PROTOCOL_PATH", "$$IF(STREAM_PROTOCOL_PATH,$$STREAM_PROTOCOL_PATH,db)")

# Diagnostic settings
epicsEnvSet("CAM_TRACE_MASK",       "$$IF(CAMTRACE,$$CAMTRACE,1)")
epicsEnvSet("CAM_TRACE_IO_MASK",    "$$IF(CAMTRACEIO,$$CAMTRACEIO,0)")
epicsEnvSet("SER_TRACE_MASK",       "$$IF(SERTRACE,$$SERTRACE,1)")
epicsEnvSet("SER_TRACE_IO_MASK",    "$$IF(SERTRACEIO,$$SERTRACEIO,1)")
$$ENDLOOP(CAMERA)

# Global diagnostic settings
epicsEnvSet("ST_CMD_DELAYS",        "$$IF(ST_CMD_DELAYS,$$ST_CMD_DELAYS,1)")

cd( "$(IOCTOP)" )

# Run common startup commands for linux soft IOC's
< /reg/d/iocCommon/All/pre_linux.cmd

##############################################################
#
# The remainder of the script should be common for all Prosilica gigE cameras
#

# Register all support components
dbLoadDatabase("dbd/edt.dbd")
edt_registerRecordDeviceDriver(pdbbase)

# Initialize debug variables
var DEBUG_EDT_PDV $$IF(DEBUG_EDT_PDV,$$DEBUG_EDT_PDV,0)
var DEBUG_TS_FIFO  $$IF(DEBUG_TS_FIFO,$$DEBUG_TS_FIFO,0)

# Load standard soft ioc database
dbLoadRecords("db/iocSoft.db",              "IOC=$(IOC_PV)")
dbLoadRecords("db/save_restoreStatus.db",   "IOC=$(IOC_PV)")

# Configure the EVR
ErConfigure($(EVR_CARD), 0, 0, 0, $(EVRID))
dbLoadRecords("$(EVRDB)",   "IOC=$(IOC_PV),EVR=$(EVR_PV),CARD=$(EVR_CARD),IP$(TRIG_NUM)E=Enabled")

# Setup the environment for the specified camera model
< db/$(MODEL).env

# =========================================================
# Configure an edtPdv driver for the specified camera model
# =========================================================
edtPdvConfig( "$(CAM_PORT)", "$(EDT_CARD)", "$(EDT_CHAN)", "$(MODEL)", "$(CAM_MODE)" )

# Set asyn trace flags
asynSetTraceMask(   "$(CAM_PORT)",      1, $(CAM_TRACE_MASK) )
asynSetTraceIOMask( "$(CAM_PORT)",      1, $(CAM_TRACE_IO_MASK) )
asynSetTraceMask(   "$(CAM_PORT).SER",  1, $(SER_TRACE_MASK) )
asynSetTraceIOMask( "$(CAM_PORT).SER",  1, $(SER_TRACE_IO_MASK) )

# Comment/uncomment/change delay as desired so you can see remaining camera dbLoad msgs
$$IF(NO_ST_CMD_DELAY)
#epicsThreadSleep $(ST_CMD_DELAYS)
$$ELSE(NO_ST_CMD_DELAY)
epicsThreadSleep $(ST_CMD_DELAYS)
$$ENDIF(NO_ST_CMD_DELAY)

# Configure and load standard edtPdv camera database
dbLoadRecords("db/edtPdvCamera.db",         "CAM=$(CAM_PV),CAM_PORT=$(CAM_PORT),CAM_TRIG=$(TRIG_PV),EVR=$(EVR_PV)")
dbLoadRecords("db/timeStampFifo.template",  "DEV=$(CAM_PV):TSS,PORT_PV=$(CAM_PV):PortName_RBV,EC_PV=$(EVR_PV):EVENT1CTRL.ENM,DLY=1")

# For camera serial asyn diagnostics
# (AreaDetector plugins each have their own AsynIO record)
dbLoadRecords("db/asynRecord.db",   "P=$(CAM_PV):SER,R=:AsynIO,PORT=$(CAM_PORT).SER,ADDR=0,IMAX=0,OMAX=0")

# Load camera model specific db
dbLoadRecords("db/$(MODEL).db",     "P=$(CAM_PV),R=:,PORT=$(CAM_PORT),PWIDTH=$(TRIG_PV):TWID,PW_RBV=$(TRIG_PV):BW_TWIDCALC" )

# Load history records
$$IF(BLD_SRC)
dbLoadRecords("db/bld_hist.db",     "P=$(CAM_PV),R=:")
$$ENDIF(BLD_SRC)
dbLoadRecords("db/edtCam_hist.db",  "P=$(CAM_PV),R=:")

# Comment/uncomment/change delay as desired so you can see remaining camera dbLoad msgs
$$IF(NO_ST_CMD_DELAY)
#epicsThreadSleep $(ST_CMD_DELAYS)
$$ELSE(NO_ST_CMD_DELAY)
epicsThreadSleep $(ST_CMD_DELAYS)
$$ENDIF(NO_ST_CMD_DELAY)

# Configure and load any desired datastreams
$$LOOP(DATASTREAM)
< $$IF(LOC,$$LOC,db)/$$(NAME)Stream.cmd
$$ENDLOOP(DATASTREAM)

# Configure and load any desired viewers
$$LOOP(VIEWER)
< $$IF(LOC,$$LOC,db)/$$(NAME)Viewer.cmd
$$ENDLOOP(VIEWER)

# Configure and load the selected plugins, if any
$$LOOP(PLUGIN)
$$IF(NUM)
epicsEnvSet("N",            "$$NUM")
$$ENDIF(NUM)
$$IF(SRC)
epicsEnvSet("PLUGIN_SRC",   "$$SRC")
$$ENDIF(SRC)
< $$IF(LOC,$$LOC,db)/plugin$$(NAME).cmd
$$ENDLOOP(PLUGIN)

# Configure and load BLD plugin
$$LOOP(BLD)
epicsEnvSet("N",            "$$CALC{INDEX+1}")
epicsEnvSet("PLUGIN_SRC",   "CAM")
< setupScripts/pluginBldSpectrometer.cmd
$$ENDLOOP(BLD)

# Comment/uncomment/change delay as desired so you can see remaining camera dbLoad msgs
$$IF(NO_ST_CMD_DELAY)
#epicsThreadSleep $(ST_CMD_DELAYS)
$$ELSE(NO_ST_CMD_DELAY)
epicsThreadSleep $(ST_CMD_DELAYS)
$$ENDIF(NO_ST_CMD_DELAY)

# Setup autosave
dbLoadRecords( "db/save_restoreStatus.db",  "IOC=$(IOC_PV)" )
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

#
# Initialize the IOC and start processing records
#
iocInit()

reate autosave files from info directives
makeAutosaveFileFromDbInfo("$(IOC_DATA)/$(IOC)/autosave/autoSettings.req",  "autosaveFields")

# Start autosave backups
create_monitor_set("autoSettings.req",  5,  "")
create_monitor_set("$(IOCNAME).req",    5,  "IOC=$(IOC_PV)")

# All IOCs should dump some common info after initial startup.
< /reg/d/iocCommon/All/post_linux.cmd

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

# Final delay before auto-start image acquisition
$$IF(NO_ST_CMD_DELAY)
#epicsThreadSleep $(ST_CMD_DELAYS)
#epicsThreadSleep $(ST_CMD_DELAYS)
$$ELSE(NO_ST_CMD_DELAY)
epicsThreadSleep $(ST_CMD_DELAYS)
epicsThreadSleep $(ST_CMD_DELAYS)
$$ENDIF(NO_ST_CMD_DELAY)

dbpf $(CAM_PV):Acquire 1


#
# pluginBldSpectrometer.cmd
#
# Required env vars
#	CAM			- PV Prefix for all camera related PV's
#	N			- Plugin number, must be unique if more than one BldSpectrometer plugin
#	PLUGIN_SRC	- Which port should this plugin get its data from

# Configure the plugin
# BldSpectrometerConfigure( portName, queueSize, blockingCallbacks, dataSrcPortName, addr, maxBuffers, maxMemory, priority, stackSize, maxThreads )
# Set maxBuffers to 0 for unlimited buffers
# Set maxMemory  to 0 for unlimited memory allocation
# Set priority   to 0  for default priority
# Set stackSize  to 0  for default stackSize
# Set maxThreads to 0  for default 1 thread max
BldSpectrometerConfigure( "BLD$(N)", $(QSIZE), 0, "$(PLUGIN_SRC)", 0, 0, 0, 90, 0, 4 )

# Load the plugin records
dbLoadRecords( "db/pluginBldSpectrometer.db",  "CAM=$(CAM_PV),CAM_PORT=$(CAM_PORT),PLUGIN_SRC=$(PLUGIN_SRC),N=$(N),XSIZE=$(IMAGE_XSIZE),YSIZE=$(IMAGE_YSIZE),IMAGE_BIT_DEPTH=$(IMAGE_BIT_DEPTH),BLD_IP=$(BLD_IP)" )

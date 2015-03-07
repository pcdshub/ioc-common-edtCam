TOP=../..
include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE

#----------------------------------------------------
# Create and install (or just install)
# databases, templates, substitutions like this

# Create and install plugin DB Files
DB += pluginColorConvert.db
#DB += pluginFileMPEG.db
DB += pluginHDF5.db
DB += pluginImage.db
DB += pluginJPEG.db
DB += pluginMagick.db
#DB += pluginMJPG.db
DB += pluginNetCDF.db
DB += pluginNexus.db
DB += pluginOverlay.db
DB += pluginProcess.db
DB += pluginROI.db
DB += pluginStats.db
DB += pluginTIFF.db
DB += pluginTransform.db

# Install viewer cmd scripts
DB_INSTALLS += $(ADSTREAM)/db/MonoBin2Viewer.cmd
DB_INSTALLS += $(ADSTREAM)/db/MonoBin4Viewer.cmd
DB_INSTALLS += $(ADSTREAM)/db/MonoFullViewer.cmd
DB_INSTALLS += $(ADSTREAM)/db/commonPlugins.cmd
DB_INSTALLS += $(ADSTREAM)/db/pcdsPlugins.cmd
DB_INSTALLS += $(ADSTREAM)/db/pluginColorConvert.cmd
DB_INSTALLS += $(ADSTREAM)/db/pluginFileMPEG.cmd
DB_INSTALLS += $(ADSTREAM)/db/pluginHDF5.cmd
DB_INSTALLS += $(ADSTREAM)/db/pluginImage.cmd
DB_INSTALLS += $(ADSTREAM)/db/pluginJPEG.cmd
DB_INSTALLS += $(ADSTREAM)/db/pluginMJPG.cmd
DB_INSTALLS += $(ADSTREAM)/db/pluginMagick.cmd
DB_INSTALLS += $(ADSTREAM)/db/pluginNetCDF.cmd
DB_INSTALLS += $(ADSTREAM)/db/pluginNexus.cmd
DB_INSTALLS += $(ADSTREAM)/db/pluginOverlay.cmd
DB_INSTALLS += $(ADSTREAM)/db/pluginProcess.cmd
DB_INSTALLS += $(ADSTREAM)/db/pluginROI.cmd
DB_INSTALLS += $(ADSTREAM)/db/pluginStats.cmd
DB_INSTALLS += $(ADSTREAM)/db/pluginTIFF.cmd
DB_INSTALLS += $(ADSTREAM)/db/pluginTransform.cmd

#----------------------------------------------------
# Create and install (or just install)
# Archive files, templates, substitutions like this
#

# Create and install plugin archive Files
ARCHIVE += pluginColorConvert.archive
#ARCHIVE += pluginFileMPEG.archive
ARCHIVE += pluginHDF5.archive
ARCHIVE += pluginImage.archive
ARCHIVE += pluginJPEG.archive
#ARCHIVE += pluginMJPG.archive
ARCHIVE += pluginMagick.archive
ARCHIVE += pluginNetCDF.archive
ARCHIVE += pluginNexus.archive
ARCHIVE += pluginOverlay.archive
ARCHIVE += pluginProcess.archive
ARCHIVE += pluginROI.archive
ARCHIVE += pluginStats.archive
ARCHIVE += pluginTIFF.archive
ARCHIVE += pluginTransform.archive

#----------------------------------------------------
# Create and install (or just install)
# Autosave request files, templates, substitutions like this
#

# Create and install plugin autosave Files
REQ += pluginColorConvert.req
#REQ += pluginFileMPEG.req
REQ += pluginHDF5.req
REQ += pluginImage.req
REQ += pluginJPEG.req
#REQ += pluginMJPG.req
REQ += pluginMagick.req
REQ += pluginNetCDF.req
REQ += pluginNexus.req
REQ += pluginOverlay.req
REQ += pluginProcess.req
REQ += pluginROI.req
REQ += pluginStats.req
REQ += pluginTIFF.req
REQ += pluginTransform.req

# These must be after the individual plugins
#ARCHIVE += pcdsPlugins.archive
#REQ 	+= pcdsPlugins.req

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE


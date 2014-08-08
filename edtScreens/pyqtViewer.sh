#!/bin/bash
# Guard against use of wrong python version
unset PYTHONPATH
unset LD_LIBRARY_PATH
export PATH=/usr/local/bin:/bin:/usr/bin

# Now bring in our EPICS and python environnments
source /reg/g/pcds/setup/epicsenv-3.14.12.sh
source /reg/g/pcds/setup/python25.sh

# Add in the qt bin and lib paths
pathmunge   /reg/common/package/qt/4.6.2/bin
ldpathmunge /reg/common/package/qt/4.6.2/lib/x86_64-linux

export EPICS_CA_MAX_ARRAY_BYTES=20100100
cd /reg/g/pcds/controls/pycaqt/pulnix6740.latest

# Note: Must specify --instr, --pvlist, and either --camera or --camerapv
# Example:
# gigeScreens/gigeViewer.sh --instr SXR --pvlist sxr.lst --camera 2
echo Launching pycaqt viewer with options $* ...
./pulnix6740.pyw $* >& /tmp/pulnix6740.pyw.`date +%y-%m-%d_%T` &

# Older alternative launchers
#./run_pulnix.csh --instr SXR --pvlist sxr.lst
#python ./pulnix6740.pyw --instr SXR --pvlist /reg/neh/operator/sxropr/.yagviewer/sxr-cam.lst >& /tmp/pulnix6740.pyw.`date +%y-%m-%d_%T` &

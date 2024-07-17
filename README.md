# ioc-common-edtCam
This is an EPICS IOC that is used at LCLS. This repo was automatically transferred to github from an internal filesystem repository via the scripts at https://github.com/pcdshub/afs_ioc_migration.

The original filesystem location was /afs/slac.stanford.edu/g/cd/swe/git/repos/package/epics/ioc/common/edtCam.git.


## Original readme files
### README
#
# EDT AreaDetector IOC Setup and Use
#




IOC info:
sioc name ioc-tst-orca1
Working dir: ~bhill/wa2/epics/ioc/common/edtCam/current

Camera PV Prefix:	TST:EDT:ORCA1
Full Res Image:		TST:EDT:ORCA1:IMAGE1

To launch edm viewer:
~bhill/wa2/epics/ioc/common/edtCam/current/edm-devel.cmd

To launch python viewer:
a.
Hit "Python Viewer" button on EDM Top level GUI

b.
cd /reg/g/pcds/controls/pycaqt/pulnix6740.latest
./pulnix6740.pyw --inst TST --pvlist tst.lst --camerapv TST:EDT:ORCA1



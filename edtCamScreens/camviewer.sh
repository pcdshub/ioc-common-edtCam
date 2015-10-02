#!/bin/sh
if [ -z "$CONFIG_SITE_TOP" ]; then
export CONFIG_SITE_TOP=/reg/g/pcds/pyps/config
fi
if [ -d $CONFIG_SITE_TOP/$1 ]; then
	HUTCH=$1
	shift
else
	HUTCH=tst
fi

$CONFIG_SITE_TOP/$HUTCH/camviewer.sh $*

#!/bin/bash
source /reg/g/pcds/setup/epicsenv-3.14.12.sh

# Reset ROI5 for IMAGE1
caput ${CAM}:ROI5:Name				"Std Img ROI"
caput ${CAM}:ROI5:NDArrayPort		CAM
caput ${CAM}:ROI5:MinCallbackTime	0.0
caput ${CAM}:ROI5:Scale				1
caput ${CAM}:ROI5:BinX				1
caput ${CAM}:ROI5:BinY				1
caput ${CAM}:ROI5:EnableX			0
caput ${CAM}:ROI5:EnableY			0
caput ${CAM}:ROI5:EnableScale		0
caput ${CAM}:ROI5:EnableCallbacks	1

# Reset IMAGE1 plugin
caput ${CAM}:IMAGE1:NDArrayPort		ROI5
caput ${CAM}:IMAGE1:MinCallbackTime	0.02
caput ${CAM}:IMAGE1:EnableCallbacks	1

# Reset ROI6 for Binned Image with overlay
caput ${CAM}:ROI6:Name				IMG_OVER2
caput ${CAM}:ROI6:NDArrayPort		CAM
caput ${CAM}:ROI6:MinCallbackTime	0.2
#caput ${CAM}:ROI6:Scale				64.0
caput ${CAM}:ROI6:Scale				4.0
caput ${CAM}:ROI6:BinX				2
caput ${CAM}:ROI6:BinY				2
caput ${CAM}:ROI6:EnableX			1
caput ${CAM}:ROI6:EnableY			1
caput ${CAM}:ROI6:EnableScale		1
caput ${CAM}:ROI6:EnableCallbacks	1

# Reset Over2 for Binned Image with overlay
caput ${CAM}:Over2:Name				IMG_OVER2
caput ${CAM}:Over2:NDArrayPort		ROI6
caput ${CAM}:Over2:EnableCallbacks	1
caput ${CAM}:Over2:2:Use			0
caput ${CAM}:Over2:2:Shape			1
caput ${CAM}:Over2:2:Green			1023

# Reset IMG_OVER2 BinnedImage plugin
caput ${CAM}:IMG_OVER2:NDArrayPort		Over2
caput ${CAM}:IMG_OVER2:MinCallbackTime	0.5
caput ${CAM}:IMG_OVER2:EnableCallbacks	1

# Reset ROI7 for Thumbnail binning
caput ${CAM}:ROI7:Name				Thumbnail
caput ${CAM}:ROI7:NDArrayPort		CAM
caput ${CAM}:ROI7:MinCallbackTime	0.2
caput ${CAM}:ROI7:Scale				256.0
#caput ${CAM}:ROI7:Scale				16.0
caput ${CAM}:ROI7:BinX				4
caput ${CAM}:ROI7:BinY				4
caput ${CAM}:ROI7:EnableX			1
caput ${CAM}:ROI7:EnableY			1
caput ${CAM}:ROI7:EnableScale		1
caput ${CAM}:ROI7:EnableCallbacks	1

# Reset BinnedImage plugin
caput ${CAM}:THUMBNAIL:NDArrayPort		ROI7
caput ${CAM}:THUMBNAIL:MinCallbackTime	0.5
caput ${CAM}:THUMBNAIL:EnableCallbacks	1

/*
 * PluginBldSpectrometer.cpp
 *
 * Image statistics plugin
 * Author: Bruce Hill
 *
 * Created Oct 29, 2014
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include <epicsString.h>
#include <epicsMutex.h>
#include <iocsh.h>
#include <epicsExport.h>

#include "bldPvClient.h"
#include "evrTime.h"
#include "NDArray.h"
#include "PluginBldSpectrometer.h"

#define MAX(A,B) (A)>(B)?(A):(B)
#define MIN(A,B) (A)<(B)?(A):(B)

// static const char *driverName="PluginBldSpectrometer";

template <typename epicsType>
asynStatus PluginBldSpectrometer::doComputeCentroidT(NDArray *pArray)
{
	epicsType *pData = (epicsType *)pArray->pData;
	double value, *pValue, *pThresh, centroidTotal;
	size_t ix, iy;

	if (pArray->ndims > 2) return(asynError);

	getDoubleParam (PluginBldSpectrometerCentroidThreshold,  &this->centroidThreshold);
	this->unlock();
	this->centroidX = 0;
	this->centroidY = 0;
	this->sigmaX = 0;
	this->sigmaY = 0;
	this->sigmaXY = 0;

	// Clear the profile output buffers
	memset(	this->profileX[profAverage],	0,	this->profileSizeX * sizeof(double) );
	memset(	this->profileY[profAverage],	0,	this->profileSizeY * sizeof(double) );
	memset(	this->profileX[profThreshold],	0,	this->profileSizeX * sizeof(double) );
	memset(	this->profileY[profThreshold],	0,	this->profileSizeY * sizeof(double) );

	// Accumulate the X and Y projections using the profile buffers
	for (iy=0; iy<this->profileSizeY; iy++)
	{
		for (ix=0; ix<this->profileSizeX; ix++)
		{
			value = (double)*pData++;
			this->profileX[profAverage][ix] += value;
			this->profileY[profAverage][iy] += value;
			if (value >= this->centroidThreshold)
			{
				this->profileX[profThreshold][ix] += value;
				this->profileY[profThreshold][iy] += value;
				this->sigmaXY += value * ix * iy;
			}
		}
	}

	/* Normalize the average profiles and compute the centroid from them */
	this->centroidX	= 0.0;
	this->sigmaX	= 0.0;
	centroidTotal	= 0.0;
	pValue			= this->profileX[profAverage];
	pThresh			= this->profileX[profThreshold];
	for (ix=0; ix<this->profileSizeX; ix++, pValue++, pThresh++)
	{
		this->centroidX += *pThresh * ix;
		this->sigmaX	+= *pThresh * ix * ix;
		centroidTotal	+= *pThresh;
		*pValue  /= this->profileSizeY;
		*pThresh /= this->profileSizeY;
	}
	this->centroidY = 0;
	this->sigmaY = 0;
	pValue	= this->profileY[profAverage];
	pThresh = this->profileY[profThreshold];
	for (iy=0; iy<this->profileSizeY; iy++, pValue++, pThresh++)
	{
		this->centroidY += *pThresh * iy;
		this->sigmaY	+= *pThresh * iy * iy;
		*pValue  /= this->profileSizeX;
		*pThresh /= this->profileSizeX;
	}
	if (centroidTotal > 0.)
	{
		this->centroidX /= centroidTotal;
		this->centroidY /= centroidTotal;
		this->sigmaX  = sqrt((this->sigmaX	/ centroidTotal) - (this->centroidX * this->centroidX));
		this->sigmaY  = sqrt((this->sigmaY	/ centroidTotal) - (this->centroidY * this->centroidY));
		this->sigmaXY =		 (this->sigmaXY / centroidTotal) - (this->centroidX * this->centroidY);
		if ((this->sigmaX !=0) && (this->sigmaY != 0))
			this->sigmaXY /= (this->sigmaX * this->sigmaY);
	}
	this->lock();
	return(asynSuccess);
}

asynStatus PluginBldSpectrometer::doComputeCentroid(NDArray *pArray)
{
	asynStatus status;

	switch(pArray->dataType)
	{
		case NDInt8:
			status = doComputeCentroidT<epicsInt8>(pArray);
			break;
		case NDUInt8:
			status = doComputeCentroidT<epicsUInt8>(pArray);
			break;
		case NDInt16:
			status = doComputeCentroidT<epicsInt16>(pArray);
			break;
		case NDUInt16:
			status = doComputeCentroidT<epicsUInt16>(pArray);
			break;
		case NDInt32:
			status = doComputeCentroidT<epicsInt32>(pArray);
			break;
		case NDUInt32:
			status = doComputeCentroidT<epicsUInt32>(pArray);
			break;
		case NDFloat32:
			status = doComputeCentroidT<epicsFloat32>(pArray);
			break;
		case NDFloat64:
			status = doComputeCentroidT<epicsFloat64>(pArray);
			break;
		default:
			status = asynError;
		break;
	}
	return(status);
}

template <typename epicsType>
asynStatus PluginBldSpectrometer::doComputeProfilesT(NDArray *pArray)
{
	epicsType *pData = (epicsType *)pArray->pData;
	epicsType *pCentroid, *pCursor;
	size_t ix, iy;
	int itemp;

	if (pArray->ndims > 2) return(asynError);

	/* Compute the X and Y profiles at the centroid and cursor positions */
	getIntegerParam (PluginBldSpectrometerCursorX, &itemp); this->cursorX = itemp;
	getIntegerParam (PluginBldSpectrometerCursorY, &itemp); this->cursorY = itemp;
	this->unlock();
	iy = (size_t) (this->centroidY + 0.5);
	iy = MAX(iy, 0);
	iy = MIN(iy, this->profileSizeY-1);
	pCentroid = pData + iy*this->profileSizeX;
	iy = this->cursorY;
	iy = MAX(iy, 0);
	iy = MIN(iy, this->profileSizeY-1);
	pCursor = pData + iy*this->profileSizeX;
	for (ix=0; ix<this->profileSizeX; ix++)
	{
		this->profileX[profCentroid][ix] = *pCentroid++;
		this->profileX[profCursor][ix]	 = *pCursor++;
	}
	ix = (size_t) (this->centroidX + 0.5);
	ix = MAX(ix, 0);
	ix = MIN(ix, this->profileSizeX-1);
	pCentroid = pData + ix;
	ix = this->cursorX;
	ix = MAX(ix, 0);
	ix = MIN(ix, this->profileSizeX-1);
	pCursor = pData + ix;
	for (iy=0; iy<this->profileSizeY; iy++)
	{
		this->profileY[profCentroid][iy] = *pCentroid;
		this->profileY[profCursor][iy]	 = *pCursor;
		pCentroid += this->profileSizeX;
		pCursor   += this->profileSizeX;
	}
	this->lock();
	doCallbacksFloat64Array(this->profileX[profAverage],   this->profileSizeX, PluginBldSpectrometerProfileAverageX, 0);
	doCallbacksFloat64Array(this->profileY[profAverage],   this->profileSizeY, PluginBldSpectrometerProfileAverageY, 0);
	doCallbacksFloat64Array(this->profileX[profThreshold], this->profileSizeX, PluginBldSpectrometerProfileThresholdX, 0);
	doCallbacksFloat64Array(this->profileY[profThreshold], this->profileSizeY, PluginBldSpectrometerProfileThresholdY, 0);
	doCallbacksFloat64Array(this->profileX[profCentroid],  this->profileSizeX, PluginBldSpectrometerProfileCentroidX, 0);
	doCallbacksFloat64Array(this->profileY[profCentroid],  this->profileSizeY, PluginBldSpectrometerProfileCentroidY, 0);
	doCallbacksFloat64Array(this->profileX[profCursor],    this->profileSizeX, PluginBldSpectrometerProfileCursorX, 0);
	doCallbacksFloat64Array(this->profileY[profCursor],    this->profileSizeY, PluginBldSpectrometerProfileCursorY, 0);

	return(asynSuccess);
}

asynStatus PluginBldSpectrometer::doComputeProfiles(NDArray *pArray)
{
	asynStatus status;

	switch(pArray->dataType)
	{
		case NDInt8:
			status = doComputeProfilesT<epicsInt8>(pArray);
			break;
		case NDUInt8:
			status = doComputeProfilesT<epicsUInt8>(pArray);
			break;
		case NDInt16:
			status = doComputeProfilesT<epicsInt16>(pArray);
			break;
		case NDUInt16:
			status = doComputeProfilesT<epicsUInt16>(pArray);
			break;
		case NDInt32:
			status = doComputeProfilesT<epicsInt32>(pArray);
			break;
		case NDUInt32:
			status = doComputeProfilesT<epicsUInt32>(pArray);
			break;
		case NDFloat32:
			status = doComputeProfilesT<epicsFloat32>(pArray);
			break;
		case NDFloat64:
			status = doComputeProfilesT<epicsFloat64>(pArray);
			break;
		default:
			status = asynError;
		break;
	}
	return(status);
}


template <typename epicsType>
asynStatus PluginBldSpectrometer::doComputeProjectionsT(
	NDArray				*	pArray,
	BldSpectrometer_t	*	pBldData )
{
	const char	*	functionName = "PluginBldSpectrometer::doComputeProjectionsT";
	if ( pArray->ndims > 2 )
		return( asynError );

	// Get the BLD configuration parameters
	size_t		nPeaks					= pBldData->m_Peaks.size();
	pBldData->m_baselineThreshold		= 0.0;
#if 0
	int				itemp;
	getIntegerParam( PluginBldSpectrometerCursorX, &itemp ); this->cursorX = itemp;
	getIntegerParam( PluginBldSpectrometerCursorY, &itemp ); this->cursorY = itemp;
#endif

	// Unlock to prevent deadlocks?
	this->unlock();

	// Clear BLD data
	pBldData->m_adjCenterOfMass			= 0.0;
	pBldData->m_rawCenterOfMass			= 0.0;
	pBldData->m_rawIntegral				= 0.0;
	pBldData->m_horizProjWidth			= this->profileSizeX;
	pBldData->m_horizProjFirstRowUsed	= 0;	// TODO: Need to fetch asynDrv port CAM, addr 0, param MIN_Y
	// Possibly something along these lines
	//	asynUser	*	pasynUser = asynManager->createAsynUser(0,0);
	//	port * pPort = asynManager->locatePort( NDArrayPort, NDArrayAddr );
	//	int	firstRow	= 0;
	//	pPort->readIntegerParam( MIN_Y, &firstRow );
	pBldData->m_horizProjLastRowUsed	= pBldData->m_horizProjFirstRowUsed	+ this->profileSizeY - 1;
	for (	size_t	iPeak = 0; iPeak < nPeaks; iPeak++ )
	{
		pBldData->m_Peaks[iPeak].m_Start		= 0;
		pBldData->m_Peaks[iPeak].m_End			= 0;
		pBldData->m_Peaks[iPeak].m_PeakPos		= NAN;
		pBldData->m_Peaks[iPeak].m_PeakCenter	= NAN;
		pBldData->m_Peaks[iPeak].m_PeakHeight	= NAN;
		pBldData->m_Peaks[iPeak].m_PeakFwhm		= NAN;
	}
	pBldData->m_HorizProj.resize( this->profileSizeX );
	for (	size_t	ix = 0; ix < this->profileSizeX; ix++ )
	{
		pBldData->m_HorizProj[ix]	= 0.0;
	}

	// Compute BLD data
#if 1
	epicsType	*	pData = (epicsType *) pArray->pData;
	for (		size_t	iy = 0; iy < this->profileSizeY; iy++ )
	{
		for (	size_t	ix = 0; ix < this->profileSizeX; ix++ )
		{
			double	value	= static_cast<double>( *pData++ );
			pBldData->m_HorizProj[ix]	+= value;
			pBldData->m_rawIntegral		+= value;
			pBldData->m_rawCenterOfMass	+= value * ix;
			if ( value >= pBldData->m_baselineThreshold )
			{
				pBldData->m_adjIntegral		+= value;
				pBldData->m_adjCenterOfMass	+= value * ix;
			}
		}
	}
#else
	epicsType	&	rData[][] = (epicsType *) pArray->pData;
	for (		size_t	ix = 0; ix < this->profileSizeX; ix++ )
	{
		epicsType	*	pData = (epicsType *) pArray->pData;
		pData += ix;
		for (	size_t	iy = 0; iy < this->profileSizeY; iy++ )
		{
			double	value	= static_cast<double>( *pData );
			pData += this->profileSizeY;
			pBldData->m_HorizProj[ix]	+= value;
			pBldData->m_rawIntegral		+= value;
			pBldData->m_rawCenterOfMass	+= value * ix;
			if ( value >= pBldData->m_baselineThreshold )
			{
				pBldData->m_adjIntegral		+= value;
				pBldData->m_adjCenterOfMass	+= value * ix;
			}
		}
	}

	// Do automatic peak detection
	// Uses an IIR filtered spectrum to suppress noise for determining
	// start and end points for each peak to be analyzed.
	//
	// The algorithm is designed to determine start and end of each peak region in one pass.
	// Then each peak region is analyzed with a single pass that determines
	// peak height, position, center of mass, and full width half max.
	//
	// Each region to be analyzed for peak characteristics starts
	// when the filtered spectrum exceeds the peak threshold
	// and ends when it drops below the peak threshold.
	//
	// To detect divided peaks, a dividedPeaksPercentage is used.
	// Once the filtered spectrum value dips below the current peak's max
	// times the dividedPeaksPercentage, we look for the lowest point
	// of a possible trough between two peaks.
	//
	// A new peak region will be started if the filtered spectrum value
	// rises again using the lowest point between the peaks
	// as the dividing point between their analysis regions.
	// 
	double		filtSpec		= 0.0;
	double		iir				= 0.7;	// IIR filter factor, Time Constant for 0.7 is ~ 2.8 columns
	double		peakThreshold	= 2000.0;
	double		dividedPercent	= 90.0;
	size_t		iPeak			= 0;
	size_t		peakStart		= 0;
	size_t		peakEnd			= 0;
	double		peakMax			= 0.0;
	double		troughMin		= NAN;
	for ( size_t	ix = 0; ix < this->profileSizeX; ix++ )
	{
		double	value = pBldData->m_HorizProj[ix];
		filtSpec = ( value * iir ) + ( filtSpec * ( 1.0 - iir ) );

		// Keep track of the max
		if( peakMax <= value )
			peakMax =  value;

		if ( peakStart != 0 )
		{
			// Look for peak end
			// Either below the peakThreshold or we've risen above the trough minimum
			if ( filtSpec < peakThreshold
				||	(	!isnan( troughMin )
					&&	value > troughMin ) )
			{
				// End this peak and start another
				pBldData[iPeak].m_Start	= peakStart;
				pBldData[iPeak].m_End	= ix;
				troughMin				= NAN;
				peakMax					= 0.0;
				peakStart				= 0;
				if ( ++iPeak >= N_PEAKS_MAX )
					break;
			}
		}

		// Look for a new peak start
		// Could be the same sample if we just rose above a trough minimum
		if( peakStart == 0 && filtSpec >= peakThreshold )
		{
			peakMax		= value;
			peakStart	= ix;
		}

		// If we're in a peak region and the value drops below the divided peaks threshold ...
		if (	peakStart != 0
			&&	value < peakMax * dividedPercent )
		{
			// Keep track of our possible trough low
			if ( isnan(troughMin) || value < troughMin )
				troughMin = value;
		}
	}

	if ( iPeak < N_PEAKS_MAX )
	{
		// See if we have enough data for a final peak
		if ( peakStart != 0 && !isnan( troughMin ) )
		{
			// End this peak
			pBldData[iPeak].m_Start	= peakStart;
			pBldData[iPeak].m_End	= ix;
		}
	}
#endif

	if( pBldData->m_rawIntegral > 0 )
		pBldData->m_rawCenterOfMass /= pBldData->m_rawIntegral;
	else
		pBldData->m_rawCenterOfMass = 0.0;
	if( pBldData->m_adjIntegral > 0 )
		pBldData->m_adjCenterOfMass /= pBldData->m_adjIntegral;
	else
		pBldData->m_adjCenterOfMass = 0.0;

	asynPrint(	this->pasynUserSelf, ASYN_TRACEIO_DRIVER,
				"%s: rawCOM %.1f, adjCOM %.1f, rawSum %.3e, adjSum %.3e\n", functionName,
				pBldData->m_rawCenterOfMass,	pBldData->m_adjCenterOfMass,
				pBldData->m_rawIntegral,		pBldData->m_adjIntegral		);

	this->lock();


	return(asynSuccess);
}

asynStatus PluginBldSpectrometer::doComputeProjections(
	NDArray				*	pArray,
	BldSpectrometer_t	*	pBldData	)
{
	asynStatus status;

	switch(pArray->dataType)
	{
	case NDInt8:
		status = doComputeProjectionsT<epicsInt8>(pArray, pBldData );
		break;
	case NDUInt8:
		status = doComputeProjectionsT<epicsUInt8>(pArray, pBldData );
		break;
	case NDInt16:
		status = doComputeProjectionsT<epicsInt16>(pArray, pBldData );
		break;
	case NDUInt16:
		status = doComputeProjectionsT<epicsUInt16>(pArray, pBldData );
		break;
	case NDInt32:
		status = doComputeProjectionsT<epicsInt32>(pArray, pBldData );
		break;
	case NDUInt32:
		status = doComputeProjectionsT<epicsUInt32>(pArray, pBldData );
		break;
	case NDFloat32:
		status = doComputeProjectionsT<epicsFloat32>(pArray, pBldData );
		break;
	case NDFloat64:
		status = doComputeProjectionsT<epicsFloat64>(pArray, pBldData );
		break;
	default:
		status = asynError;
		break;
	}
	return(status);
}


asynStatus PluginBldSpectrometer::doSendBld(
	NDArray				*	pNDArray,
	BldSpectrometer_t	*	pBldData	)
{
	const char	*	functionName	= "PluginBldSpectrometer::doSendBld";
	asynStatus		status			= asynSuccess;

	size_t		nPeaks	= pBldData->m_Peaks.size();

	//	Compute the output buffer size
	assert( sizeof(uint32_t) == 4 );
	assert( sizeof(double)	 == 8 );
	size_t			sBufferMax	=	(	sizeof(uint32_t)		//	Projection Width
									+	sizeof(uint32_t)		//	First row used
									+	sizeof(uint32_t)		//	Last row used
									+	sizeof(double)			//	rawCenterOfMass
									+	sizeof(double)			//	baselineThreshold
									+	sizeof(double)			//	adjCenterOfMass
									+	sizeof(double)			//	rawIntegral
									+	sizeof(uint32_t)		//	nPeaks
									+	(	sizeof(uint32_t)	//	Horiz projection
										*	pBldData->m_horizProjWidth	)
									+	(	sizeof(double)		//	Peak positions
										*	nPeaks	)
									+	(	sizeof(double)		//	Peak heights
										*	nPeaks	)
									+	(	sizeof(double)		//	Peak FullWidthHalfMax
										*	nPeaks	)
									);

	//	TODO: Pre-allocate storage to avoid malloc/free
	void		*	pBufferOrig		= malloc( sBufferMax );

	// Packing fixed size portion of output buffer
	uint32_t	*	pBufferUint32	= static_cast<uint32_t *>( pBufferOrig );
	*pBufferUint32++	= pBldData->m_horizProjWidth;
	*pBufferUint32++	= pBldData->m_horizProjFirstRowUsed;
	*pBufferUint32++	= pBldData->m_horizProjLastRowUsed;
	double		*	pBufferDouble	= reinterpret_cast<double *>( pBufferUint32 );
	*pBufferDouble++	= pBldData->m_rawCenterOfMass;
	*pBufferDouble++	= pBldData->m_baselineThreshold;
	*pBufferDouble++	= pBldData->m_adjCenterOfMass;
	*pBufferDouble++	= pBldData->m_rawIntegral;
	pBufferUint32		= reinterpret_cast<uint32_t *>( pBufferDouble );
	*pBufferUint32++	= nPeaks;

	// Packing variable size portion of output buffer
	for ( size_t	ix = 0; ix < pBldData->m_horizProjWidth; ix++ )
		*pBufferUint32++	= static_cast<uint32_t>( this->profileX[profAverage][ix] );

	pBufferDouble	= reinterpret_cast<double *>( pBufferUint32 );
	for ( size_t	iPeak = 0; iPeak < nPeaks; iPeak++ )
		*pBufferDouble++	= pBldData->m_Peaks[iPeak].m_PeakPos;
	for ( size_t	iPeak = 0; iPeak < nPeaks; iPeak++ )
		*pBufferDouble++	= pBldData->m_Peaks[iPeak].m_PeakHeight;
	for ( size_t	iPeak = 0; iPeak < nPeaks; iPeak++ )
		*pBufferDouble++	= pBldData->m_Peaks[iPeak].m_PeakFwhm;

	// Make sure we filled correctly
	assert( reinterpret_cast<char *>(pBufferDouble) == reinterpret_cast<char *>( pBufferOrig ) + sBufferMax );
	//size_t			sBuffer			= sBufferMax;

	// Get the packet info
	size_t			sBuffer			= reinterpret_cast<char *>(pBufferDouble) - reinterpret_cast<char *>(pBufferOrig);
	unsigned int	srcPhysicalID	= 46;			//	from $(PDSDATA_REPO)/trunk/xtc/BldInfo.hh	Pds::BldInfo::FeeSpec0
	unsigned int	xtcType			= 0x10000 | 72;	//	from $(PDSDATA_REPO)/trunk/xtc/TypeId.hh	Pds::TypeId::Id_Spectrometer Vers 1

	// Send the packet
	int bldStatus	= 0;
	if ( bldStatus != 3 )
	bldStatus = BldSendPacket( 0, srcPhysicalID, xtcType, &pNDArray->epicsTS, pBufferOrig, sBuffer );
	free( pBufferOrig );

	if ( bldStatus != 0 )
	{
		asynPrint(	this->pasynUserSelf, ASYN_TRACE_ERROR,
					"%s: BldSendPacket Error, status %d, %zd byte BLD output buffer\n",
					functionName, bldStatus, sBuffer );
		status = asynError;
	}
	else
	{
		asynPrint(	this->pasynUserSelf, ASYN_TRACEIO_DRIVER,
					"%s: Sent %zd byte BLD output buffer\n", 
					functionName, sBuffer );
	}

	return(status);
}


/** Callback function that is called by the NDArray driver with new NDArray data.
  * Does image statistics.
  * \param[in] pArray  The NDArray from the callback.
  */
void PluginBldSpectrometer::processCallbacks(NDArray *pArray)
{
//	const char	*	functionName = "PluginBldSpectrometer::processCallbacks";

	/* This function does array statistics.
	 * It is called with the mutex already locked.	It unlocks it during long calculations when private
	 * structures don't need to be protected.
	 */
	BldSpectrometer_t		bldData,		*	pBld		= &bldData;
//	BldSpectrometer_t		bldDataTemp,	*	pBldTemp	= &bldDataTemp;
	int computeCentroid, computeProfiles, computeProjections;
	int sendBld;
	size_t sizeX=0, sizeY=0;
	NDArrayInfo arrayInfo;

	/* Call the base class method */
	NDPluginDriver::processCallbacks(pArray);

	pArray->getInfo(&arrayInfo);
	getIntegerParam( PluginBldSpectrometerComputeCentroid,		&computeCentroid );
	getIntegerParam( PluginBldSpectrometerComputeProfiles,		&computeProfiles );
	getIntegerParam( PluginBldSpectrometerComputeProjections,	&computeProjections );
	getIntegerParam( PluginBldSpectrometerSendBld,				&sendBld );

	if (pArray->ndims > 0) sizeX = pArray->dims[0].size;
	if (pArray->ndims == 1) sizeY = 1;
	if (pArray->ndims > 1)	sizeY = pArray->dims[1].size;

	if (sizeX != this->profileSizeX)
	{
		this->profileSizeX = sizeX;
		setIntegerParam(PluginBldSpectrometerProfileSizeX,	(int)this->profileSizeX);
		for ( size_t i=0; i<MAX_PROFILE_TYPES; i++)
		{
			if (this->profileX[i]) free(this->profileX[i]);
			this->profileX[i] = (double *)malloc(this->profileSizeX * sizeof(double));
		}
	}
	if (sizeY != this->profileSizeY)
	{
		this->profileSizeY = sizeY;
		setIntegerParam(PluginBldSpectrometerProfileSizeY, (int)this->profileSizeY);
		for ( size_t i=0; i<MAX_PROFILE_TYPES; i++)
		{
			if (this->profileY[i]) free(this->profileY[i]);
			this->profileY[i] = (double *)malloc(this->profileSizeY * sizeof(double));
		}
	}

#if 0
	if (computeStatistics)
	{
		NDDimension_t bgdDims[ND_ARRAY_MAX_DIMS], *pDim;
		size_t bgdPixels;
		int bgdWidth;
		int dim;
		double				bgdCounts,	avgBgd;
		NDArray			*	pBgdArray	= NULL;
		getIntegerParam(PluginBldSpectrometerBgdWidth, &bgdWidth);
		doComputeStatistics(pArray, pBld);
		/* If there is a non-zero background width then compute the background counts */
		if (bgdWidth > 0)
		{
			bgdPixels = 0;
			bgdCounts = 0.;
			/* Initialize the dimensions of the background array */
			for (dim=0; dim<pArray->ndims; dim++)
			{
				pArray->initDimension(&bgdDims[dim], pArray->dims[dim].size);
			}
			for (dim=0; dim<pArray->ndims; dim++)
			{
				pDim = &bgdDims[dim];
				pDim->offset = 0;
				pDim->size = MIN((size_t)bgdWidth, pDim->size);
				this->pNDArrayPool->convert(pArray, &pBgdArray, pArray->dataType, bgdDims);
				pDim->size = pArray->dims[dim].size;
				if (!pBgdArray)
				{
					asynPrint(	this->pasynUserSelf, ASYN_TRACE_ERROR,
								"%s: Error allocating array buffer in convert\n",
								functionName );
					continue;
				}
				doComputeStatistics(pBgdArray, pBldTemp);
				pBgdArray->release();
				bgdPixels += pBldTemp->nElements;
				bgdCounts += pBldTemp->total;
				pDim->offset = MAX(0, (int)(pDim->size - 1 - bgdWidth));
				pDim->size = MIN((size_t)bgdWidth, pArray->dims[dim].size - pDim->offset);
				this->pNDArrayPool->convert(pArray, &pBgdArray, pArray->dataType, bgdDims);
				pDim->offset = 0;
				pDim->size = pArray->dims[dim].size;
				if (!pBgdArray)
				{
					asynPrint(	this->pasynUserSelf, ASYN_TRACE_ERROR,
								"%s: Error allocating array buffer in convert\n",
								functionName );
					continue;
				}
				doComputeStatistics(pBgdArray, pBldTemp);
				pBgdArray->release();
				bgdPixels += pBldTemp->nElements;
				bgdCounts += pBldTemp->total;
			}
			if (bgdPixels < 1) bgdPixels = 1;
			avgBgd = bgdCounts / bgdPixels;
			pBld->net = pBld->total - avgBgd*pBld->nElements;
		}
		setDoubleParam(PluginBldSpectrometerMinValue,	 pBld->min);
		setDoubleParam(PluginBldSpectrometerMinX,		 (double)pBld->minX);
		setDoubleParam(PluginBldSpectrometerMinY,		 (double)pBld->minY);
		setDoubleParam(PluginBldSpectrometerMaxValue,	 pBld->max);
		setDoubleParam(PluginBldSpectrometerMaxX,		 (double)pBld->maxX);
		setDoubleParam(PluginBldSpectrometerMaxY,		 (double)pBld->maxY);
		setDoubleParam(PluginBldSpectrometerMeanValue,	 pBld->mean);
		setDoubleParam(PluginBldSpectrometerSigmaValue,  pBld->sigma);
		setDoubleParam(PluginBldSpectrometerTotal,		 pBld->total);
		setDoubleParam(PluginBldSpectrometerNet,		 pBld->net);
		asynPrint(	this->pasynUserSelf, ASYN_TRACEIO_DRIVER,
					(char *)pArray->pData, arrayInfo.totalBytes,
					"%s: min=%f, max=%f, mean=%f, total=%f, net=%f",
					functionName, pBld->min, pBld->max, pBld->mean, pBld->total, pBld->net );
	}
#endif

	if (computeCentroid)
	{
		doComputeCentroid(pArray);
		setDoubleParam(PluginBldSpectrometerCentroidX,	 this->centroidX);
		setDoubleParam(PluginBldSpectrometerCentroidY,	 this->centroidY);
		setDoubleParam(PluginBldSpectrometerSigmaX,		 this->sigmaX);
		setDoubleParam(PluginBldSpectrometerSigmaY,		 this->sigmaY);
		setDoubleParam(PluginBldSpectrometerSigmaXY,	 this->sigmaXY);
	}

	if (computeProfiles)
	{
		doComputeProfiles(pArray);
	}

	if (computeProjections)
	{
		doComputeProjections( pArray, pBld );

		if (sendBld)
		{
			int		pulseId	= pArray->epicsTS.nsec & PULSEID_INVALID;
			if ( pulseId != PULSEID_INVALID )
				doSendBld( pArray, pBld );
		}

		// Update output PV's
#if 0
		doCallbacksFloat64Array( this->profileX[profAverage],	this->profileSizeX,	PluginBldSpectrometerProfileAverageX,	0 );
		doCallbacksFloat64Array( this->profileY[profAverage],	this->profileSizeY,	PluginBldSpectrometerProfileAverageY,	0 );
		doCallbacksFloat64Array( this->profileX[profThreshold],	this->profileSizeX,	PluginBldSpectrometerProfileThresholdX,	0 );
		doCallbacksFloat64Array( this->profileY[profThreshold],	this->profileSizeY,	PluginBldSpectrometerProfileThresholdY,	0 );
		doCallbacksFloat64Array( this->profileX[profCentroid],	this->profileSizeX,	PluginBldSpectrometerProfileCentroidX,	0 );
		doCallbacksFloat64Array( this->profileY[profCentroid],	this->profileSizeY,	PluginBldSpectrometerProfileCentroidY,	0 );
		doCallbacksFloat64Array( this->profileX[profCursor],	this->profileSizeX,	PluginBldSpectrometerProfileCursorX,	0 );
		doCallbacksFloat64Array( this->profileY[profCursor],	this->profileSizeY,	PluginBldSpectrometerProfileCursorY,	0 );
#endif
	}

#if 0
// Only needed if we want to recompute on the fly as they change threshold, horiz start/end, # peaks, etc
	/* Save a copy of this array for calculations when cursor is moved or threshold is changed */
	if (this->pArrays[0]) this->pArrays[0]->release();
	this->pArrays[0] = this->pNDArrayPool->copy(pArray, NULL, 1);
#endif

	callParamCallbacks();
}

/** Called when asyn clients call pasynInt32->write().
  * This function performs actions for some parameters.
  * For all parameters it sets the value in the parameter library and calls any registered callbacks..
  * \param[in] pasynUser pasynUser structure that encodes the reason and address.
  * \param[in] value Value to write. */
asynStatus PluginBldSpectrometer::writeInt32(asynUser *pasynUser, epicsInt32 value)
{
	static const char	*	functionName	= "PluginBldSpectrometer::writeInt32";
	const char			*	reasonName		= "unknownReason";
	int						function		= pasynUser->reason;
	asynStatus				status			= asynSuccess;

	/* Set the parameter in the parameter library. */
	status = (asynStatus) setIntegerParam(function, value);

	if (function == PluginBldSpectrometerCursorX)
	{
		this->cursorX = value;
		if (this->pArrays[0])
		{
			doComputeProfiles(this->pArrays[0]);
		}
	} else if (function == PluginBldSpectrometerCursorY)
	{
		this->cursorY = value;
		if (this->pArrays[0])
		{
			doComputeProfiles(this->pArrays[0]);
		}
	} else
	{
		/* If this parameter belongs to a base class call its method */
		if (function < FIRST_PLUGIN_BLD_SPEC_PARAM)
			status = NDPluginDriver::writeInt32(pasynUser, value);
	}

	/* Do callbacks so higher layers see any changes */
	status = (asynStatus) callParamCallbacks();

	getParamName( 0, pasynUser->reason, &reasonName );
	if (status)
		epicsSnprintf(pasynUser->errorMessage, pasynUser->errorMessageSize,
			  "%s: Error, status %d, function %d %s, value %d\n",
			  functionName, status, function, reasonName, value );
	else
		asynPrint(pasynUser, ASYN_TRACEIO_DRIVER,
			  "%s: Function %d %s, value %d\n",
			  functionName, function, reasonName, value );
	return status;
}

/** Called when asyn clients call pasynFloat64->write().
  * This function performs actions for some parameters.
  * For all parameters it sets the value in the parameter library and calls any registered callbacks..
  * \param[in] pasynUser pasynUser structure that encodes the reason and address.
  * \param[in] value Value to write. */
asynStatus	PluginBldSpectrometer::writeFloat64(asynUser *pasynUser, epicsFloat64 value)
{
	static const char	*	functionName	= "PluginBldSpectrometer::writeFloat64";
	const char			*	reasonName		= "unknownReason";
	int						function		= pasynUser->reason;
	asynStatus				status			= asynSuccess;
	int computeCentroid, computeProfiles;

	/* Set the parameter and readback in the parameter library.  This may be overwritten when we read back the
	 * status at the end, but that's OK */
	status = setDoubleParam(function, value);

	if (function == PluginBldSpectrometerCentroidThreshold)
	{
		getIntegerParam(PluginBldSpectrometerComputeCentroid, &computeCentroid);
		if (computeCentroid && this->pArrays[0])
		{
			doComputeCentroid(this->pArrays[0]);
			getIntegerParam(PluginBldSpectrometerComputeProfiles, &computeProfiles);
			if (computeProfiles) doComputeProfiles(this->pArrays[0]);
		}
	}
	else
	{
		/* If this parameter belongs to a base class call its method */
		if (function < FIRST_PLUGIN_BLD_SPEC_PARAM)
			status = NDPluginDriver::writeFloat64(pasynUser, value);
	}

	/* Do callbacks so higher layers see any changes */
	callParamCallbacks();

	getParamName( 0, pasynUser->reason, &reasonName );
	if (status)
		asynPrint(pasynUser, ASYN_TRACE_ERROR,
			  "%s: Error, status %d, function %d %s, value %f\n",
			  functionName, status, function, reasonName, value );
	else
		asynPrint(pasynUser, ASYN_TRACEIO_DRIVER,
			  "%s: Function %d %s, value %f\n",
			  functionName, function, reasonName, value );
	return status;
}



/** Constructor for PluginBldSpectrometer; most parameters are simply passed to NDPluginDriver::NDPluginDriver.
  * After calling the base class constructor this method sets reasonable default values for all of the
  * parameters.
  * \param[in] portName The name of the asyn port driver to be created.
  * \param[in] queueSize The number of NDArrays that the input queue for this plugin can hold when
  *			   NDPluginDriverBlockingCallbacks=0.  Larger queues can decrease the number of dropped arrays,
  *			   at the expense of more NDArray buffers being allocated from the underlying driver's NDArrayPool.
  * \param[in] blockingCallbacks Initial setting for the NDPluginDriverBlockingCallbacks flag.
  *			   0=callbacks are queued and executed by the callback thread; 1 callbacks execute in the thread
  *			   of the driver doing the callbacks.
  * \param[in] NDArrayPort Name of asyn port driver for initial source of NDArray callbacks.
  * \param[in] NDArrayAddr asyn port driver address for initial source of NDArray callbacks.
  * \param[in] maxBuffers The maximum number of NDArray buffers that the NDArrayPool for this driver is
  *			   allowed to allocate. Set this to -1 to allow an unlimited number of buffers.
  * \param[in] maxMemory The maximum amount of memory that the NDArrayPool for this driver is
  *			   allowed to allocate. Set this to -1 to allow an unlimited amount of memory.
  * \param[in] priority The thread priority for the asyn port driver thread if ASYN_CANBLOCK is set in asynFlags.
  * \param[in] stackSize The stack size for the asyn port driver thread if ASYN_CANBLOCK is set in asynFlags.
  */
PluginBldSpectrometer::PluginBldSpectrometer(const char *portName, int queueSize, int blockingCallbacks,
						 const char *NDArrayPort, int NDArrayAddr,
						 int maxBuffers, size_t maxMemory,
						 int priority, int stackSize)
	/* Invoke the base class constructor */
	: NDPluginDriver(portName, queueSize, blockingCallbacks,
				   NDArrayPort, NDArrayAddr, 1, NUM_PLUGIN_BLD_SPEC_PARAMS, maxBuffers, maxMemory,
				   asynInt32ArrayMask | asynFloat64ArrayMask | asynGenericPointerMask,
				   asynInt32ArrayMask | asynFloat64ArrayMask | asynGenericPointerMask,
				   0, 1, priority, stackSize)
{
	//const char *functionName = "PluginBldSpectrometer::PluginBldSpectrometer";

	/* Statistics */
	createParam(PluginBldSpectrometerComputeStatisticsString, asynParamInt32,	   &PluginBldSpectrometerComputeStatistics);
	createParam(PluginBldSpectrometerBgdWidthString,		  asynParamInt32,	   &PluginBldSpectrometerBgdWidth);
	createParam(PluginBldSpectrometerMinValueString,		  asynParamFloat64,    &PluginBldSpectrometerMinValue);
	createParam(PluginBldSpectrometerMinXString,			  asynParamFloat64,    &PluginBldSpectrometerMinX);
	createParam(PluginBldSpectrometerMinYString,			  asynParamFloat64,    &PluginBldSpectrometerMinY);
	createParam(PluginBldSpectrometerMaxValueString,		  asynParamFloat64,    &PluginBldSpectrometerMaxValue);
	createParam(PluginBldSpectrometerMaxXString,			  asynParamFloat64,    &PluginBldSpectrometerMaxX);
	createParam(PluginBldSpectrometerMaxYString,			  asynParamFloat64,    &PluginBldSpectrometerMaxY);
	createParam(PluginBldSpectrometerMeanValueString,		  asynParamFloat64,    &PluginBldSpectrometerMeanValue);
	createParam(PluginBldSpectrometerSigmaValueString,		  asynParamFloat64,    &PluginBldSpectrometerSigmaValue);
	createParam(PluginBldSpectrometerTotalString,			  asynParamFloat64,    &PluginBldSpectrometerTotal);
	createParam(PluginBldSpectrometerNetString,				  asynParamFloat64,    &PluginBldSpectrometerNet);

	/* Centroid */
	createParam(PluginBldSpectrometerComputeCentroidString,   asynParamInt32,	   &PluginBldSpectrometerComputeCentroid);
	createParam(PluginBldSpectrometerCentroidThresholdString, asynParamFloat64,    &PluginBldSpectrometerCentroidThreshold);
	createParam(PluginBldSpectrometerCentroidXString,		  asynParamFloat64,    &PluginBldSpectrometerCentroidX);
	createParam(PluginBldSpectrometerCentroidYString,		  asynParamFloat64,    &PluginBldSpectrometerCentroidY);
	createParam(PluginBldSpectrometerSigmaXString,			  asynParamFloat64,    &PluginBldSpectrometerSigmaX);
	createParam(PluginBldSpectrometerSigmaYString,			  asynParamFloat64,    &PluginBldSpectrometerSigmaY);
	createParam(PluginBldSpectrometerSigmaXYString,			  asynParamFloat64,    &PluginBldSpectrometerSigmaXY);

	/* Profiles */
	createParam(PluginBldSpectrometerComputeProfilesString,   asynParamInt32,		  &PluginBldSpectrometerComputeProfiles);
	createParam(PluginBldSpectrometerProfileSizeXString,	  asynParamInt32,		  &PluginBldSpectrometerProfileSizeX);
	createParam(PluginBldSpectrometerProfileSizeYString,	  asynParamInt32,		  &PluginBldSpectrometerProfileSizeY);
	createParam(PluginBldSpectrometerCursorXString,			  asynParamInt32,		  &PluginBldSpectrometerCursorX);
	createParam(PluginBldSpectrometerCursorYString,			  asynParamInt32,		  &PluginBldSpectrometerCursorY);
	createParam(PluginBldSpectrometerProfileAverageXString,   asynParamFloat64Array,  &PluginBldSpectrometerProfileAverageX);
	createParam(PluginBldSpectrometerProfileAverageYString,   asynParamFloat64Array,  &PluginBldSpectrometerProfileAverageY);
	createParam(PluginBldSpectrometerProfileThresholdXString, asynParamFloat64Array,  &PluginBldSpectrometerProfileThresholdX);
	createParam(PluginBldSpectrometerProfileThresholdYString, asynParamFloat64Array,  &PluginBldSpectrometerProfileThresholdY);
	createParam(PluginBldSpectrometerProfileCentroidXString,  asynParamFloat64Array,  &PluginBldSpectrometerProfileCentroidX);
	createParam(PluginBldSpectrometerProfileCentroidYString,  asynParamFloat64Array,  &PluginBldSpectrometerProfileCentroidY);
	createParam(PluginBldSpectrometerProfileCursorXString,	  asynParamFloat64Array,  &PluginBldSpectrometerProfileCursorX);
	createParam(PluginBldSpectrometerProfileCursorYString,	  asynParamFloat64Array,  &PluginBldSpectrometerProfileCursorY);

	/* BLD */
	createParam(PluginBldSpectrometerComputeProjectionsString, asynParamInt32,		 &PluginBldSpectrometerComputeProjections);
	createParam(PluginBldSpectrometerSendBldString, asynParamInt32,		 &PluginBldSpectrometerSendBld);

	// Clear the profile pointers
	memset(this->profileX, 0, sizeof(this->profileX));
	memset(this->profileY, 0, sizeof(this->profileY));

	/* Set the plugin type string */
	setStringParam(NDPluginDriverPluginType, "PluginBldSpectrometer");

	/* Try to connect to the array port */
	connectToArrayPort();
}

/** Configuration command */
extern "C" int BldSpectrometerConfigure(const char *portName, int queueSize, int blockingCallbacks,
								 const char *NDArrayPort, int NDArrayAddr,
								 int maxBuffers, size_t maxMemory,
								 int priority, int stackSize)
{
	new PluginBldSpectrometer(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr,
					  maxBuffers, maxMemory, priority, stackSize);
	return(asynSuccess);
}

/* EPICS iocsh shell commands */
static const iocshArg initArg0 = { "portName",iocshArgString};
static const iocshArg initArg1 = { "frame queue size",iocshArgInt};
static const iocshArg initArg2 = { "blocking callbacks",iocshArgInt};
static const iocshArg initArg3 = { "NDArrayPort",iocshArgString};
static const iocshArg initArg4 = { "NDArrayAddr",iocshArgInt};
static const iocshArg initArg5 = { "maxBuffers",iocshArgInt};
static const iocshArg initArg6 = { "maxMemory",iocshArgInt};
static const iocshArg initArg7 = { "priority",iocshArgInt};
static const iocshArg initArg8 = { "stackSize",iocshArgInt};
static const iocshArg * const initArgs[] = {&initArg0,
											&initArg1,
											&initArg2,
											&initArg3,
											&initArg4,
											&initArg5,
											&initArg6,
											&initArg7,
											&initArg8};
static const iocshFuncDef initFuncDef = {"BldSpectrometerConfigure",9,initArgs};
static void initCallFunc(const iocshArgBuf *args)
{
	BldSpectrometerConfigure(args[0].sval, args[1].ival, args[2].ival,
					 args[3].sval, args[4].ival, args[5].ival,
					 args[6].ival, args[7].ival, args[8].ival);
}

extern "C" void BldSpectrometerRegister(void)
{
	iocshRegister(&initFuncDef,initCallFunc);
}

extern "C" {
epicsExportRegistrar(BldSpectrometerRegister);
}

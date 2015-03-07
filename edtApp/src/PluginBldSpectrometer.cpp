/*
 * PluginSpectrometer.cpp
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

// static const char *driverName="PluginSpectrometer";

template <typename epicsType>
asynStatus PluginSpectrometer::doComputeCentroidT(NDArray *pArray)
{
	epicsType *pData = (epicsType *)pArray->pData;
	double value, *pValue, *pThresh, centroidTotal;
	size_t ix, iy;

	if (pArray->ndims > 2) return(asynError);

	getDoubleParam( PluginSpectrometerCentroidThreshold,  &this->centroidThreshold );
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

asynStatus PluginSpectrometer::doComputeCentroid(NDArray *pArray)
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
asynStatus PluginSpectrometer::doComputeProfilesT(NDArray *pArray)
{
	epicsType *pData = (epicsType *)pArray->pData;
	epicsType *pCentroid, *pCursor;
	size_t ix, iy;
	int itemp;

	if (pArray->ndims > 2) return(asynError);

	/* Compute the X and Y profiles at the centroid and cursor positions */
	getIntegerParam (PluginSpectrometerCursorX, &itemp); this->cursorX = itemp;
	getIntegerParam (PluginSpectrometerCursorY, &itemp); this->cursorY = itemp;
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
	doCallbacksFloat64Array(this->profileX[profAverage],   this->profileSizeX, PluginSpectrometerProfileAverageX, 0);
	doCallbacksFloat64Array(this->profileY[profAverage],   this->profileSizeY, PluginSpectrometerProfileAverageY, 0);
	doCallbacksFloat64Array(this->profileX[profThreshold], this->profileSizeX, PluginSpectrometerProfileThresholdX, 0);
	doCallbacksFloat64Array(this->profileY[profThreshold], this->profileSizeY, PluginSpectrometerProfileThresholdY, 0);
	doCallbacksFloat64Array(this->profileX[profCentroid],  this->profileSizeX, PluginSpectrometerProfileCentroidX, 0);
	doCallbacksFloat64Array(this->profileY[profCentroid],  this->profileSizeY, PluginSpectrometerProfileCentroidY, 0);
	doCallbacksFloat64Array(this->profileX[profCursor],    this->profileSizeX, PluginSpectrometerProfileCursorX, 0);
	doCallbacksFloat64Array(this->profileY[profCursor],    this->profileSizeY, PluginSpectrometerProfileCursorY, 0);

	return(asynSuccess);
}

asynStatus PluginSpectrometer::doComputeProfiles(NDArray *pArray)
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
asynStatus PluginSpectrometer::doComputeProjectionsT(
	NDArray				*	pArray,
	BldSpectrometer_t	*	pBldData )
{
	const char	*	functionName = "PluginSpectrometer::doComputeProjectionsT";
	if ( pArray->ndims > 2 )
		return( asynError );

	// Get the BLD configuration parameters
	size_t		nPeaks					= pBldData->m_Peaks.size();
	getDoubleParam( PluginSpectrometerHorizBaseline,  &pBldData->m_HorizBaseline );
#if 0
	int				itemp;
	getIntegerParam( PluginSpectrometerCursorX, &itemp ); this->cursorX = itemp;
	getIntegerParam( PluginSpectrometerCursorY, &itemp ); this->cursorY = itemp;
#endif

	// Unlock to prevent deadlocks?
	this->unlock();

	// Clear BLD data
	pBldData->m_AdjCtrMass			= 0.0;
	pBldData->m_RawCtrMass			= 0.0;
	pBldData->m_RawIntegral				= 0.0;
	pBldData->m_HorizProjWidth			= this->profileSizeX;
	pBldData->m_HorizProjFirstRowUsed	= 0;	// TODO: Need to fetch asynDrv port CAM, addr 0, param MIN_Y
	// Possibly something along these lines
	//	asynUser	*	pasynUser = asynManager->createAsynUser(0,0);
	//	port * pPort = asynManager->locatePort( NDArrayPort, NDArrayAddr );
	//	int	firstRow	= 0;
	//	pPort->readIntegerParam( MIN_Y, &firstRow );
	pBldData->m_HorizProjLastRowUsed	= pBldData->m_HorizProjFirstRowUsed	+ this->profileSizeY - 1;
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
		pBldData->m_HorizProj[ix]	= 0;
	}

	// Compute BLD data
#if 1
	epicsType	*	pData = (epicsType *) pArray->pData;
	for (		size_t	iy = 0; iy < this->profileSizeY; iy++ )
	{
		for (	size_t	ix = 0; ix < this->profileSizeX; ix++ )
		{
			double	value	= static_cast<double>( *pData++ );
			pBldData->m_HorizProj[ix]	+= static_cast<epicsUInt32>(value);
			pBldData->m_RawIntegral		+= value;
			pBldData->m_RawCtrMass	+= value * ix;
			if ( value >= pBldData->m_HorizBaseline )
			{
				pBldData->m_AdjIntegral		+= value;
				pBldData->m_AdjCtrMass	+= value * ix;
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
			pBldData->m_HorizProj[ix]	+= static_cast<epicsUInt32>(value);
			pBldData->m_RawIntegral		+= value;
			pBldData->m_RawCtrMass	+= value * ix;
			if ( value >= pBldData->m_HorizBaseline )
			{
				pBldData->m_AdjIntegral		+= value;
				pBldData->m_AdjCtrMass	+= value * ix;
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
		double	value = static_cast<epicsUInt32>( pBldData->m_HorizProj[ix] );
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

	if( pBldData->m_RawIntegral > 0 )
		pBldData->m_RawCtrMass /= pBldData->m_RawIntegral;
	else
		pBldData->m_RawCtrMass = 0.0;
	if( pBldData->m_AdjIntegral > 0 )
		pBldData->m_AdjCtrMass /= pBldData->m_AdjIntegral;
	else
		pBldData->m_AdjCtrMass = 0.0;

	asynPrint(	this->pasynUserSelf, ASYN_TRACEIO_DRIVER,
				"%s: rawCOM %.1f, adjCOM %.1f, rawSum %.3e, adjSum %.3e\n", functionName,
				pBldData->m_RawCtrMass,	pBldData->m_AdjCtrMass,
				pBldData->m_RawIntegral,		pBldData->m_AdjIntegral		);

	this->lock();

	// Update PV's
	doCallbacksInt32Array(	reinterpret_cast<epicsInt32 *>( pBldData->m_HorizProj.data() ),
							pBldData->m_HorizProjWidth, PluginSpectrometerHorizProj, 0	);
	// setDoubleParam( PluginSpectrometerHorizBaseline,	pBldData->m_HorizBaseline	);
	setDoubleParam( PluginSpectrometerAdjCtrMass,		pBldData->m_AdjCtrMass	);
	setDoubleParam( PluginSpectrometerRawCtrMass,		pBldData->m_RawCtrMass	);
	setDoubleParam( PluginSpectrometerAdjIntegral,		pBldData->m_AdjIntegral	);
	setDoubleParam( PluginSpectrometerRawIntegral,		pBldData->m_RawIntegral	);

	return(asynSuccess);
}

asynStatus PluginSpectrometer::doComputeProjections(
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


asynStatus PluginSpectrometer::doSendBld(
	NDArray				*	pNDArray,
	BldSpectrometer_t	*	pBldData	)
{
	const char	*	functionName	= "PluginSpectrometer::doSendBld";
	asynStatus		status			= asynSuccess;

	size_t		nPeaks	= pBldData->m_Peaks.size();

	//	Compute the output buffer size
	assert( sizeof(uint32_t) == 4 );
	assert( sizeof(double)	 == 8 );
	size_t			sBufferMax	=	(	sizeof(uint32_t)		//	Projection Width
									+	sizeof(uint32_t)		//	First row used
									+	sizeof(uint32_t)		//	Last row used
									+	sizeof(double)			//	RawCtrMass
									+	sizeof(double)			//	HorizBaseline
									+	sizeof(double)			//	AdjCtrMass
									+	sizeof(double)			//	RawIntegral
									+	sizeof(uint32_t)		//	nPeaks
									+	(	sizeof(uint32_t)	//	Horiz projection
										*	pBldData->m_HorizProjWidth	)
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
	*pBufferUint32++	= pBldData->m_HorizProjWidth;
	*pBufferUint32++	= pBldData->m_HorizProjFirstRowUsed;
	*pBufferUint32++	= pBldData->m_HorizProjLastRowUsed;
	double		*	pBufferDouble	= reinterpret_cast<double *>( pBufferUint32 );
	*pBufferDouble++	= pBldData->m_RawCtrMass;
	*pBufferDouble++	= pBldData->m_HorizBaseline;
	*pBufferDouble++	= pBldData->m_AdjCtrMass;
	*pBufferDouble++	= pBldData->m_RawIntegral;
	pBufferUint32		= reinterpret_cast<uint32_t *>( pBufferDouble );
	*pBufferUint32++	= nPeaks;

	// Packing variable size portion of output buffer
	for ( size_t	ix = 0; ix < pBldData->m_HorizProjWidth; ix++ )
		*pBufferUint32++	= static_cast<uint32_t>( pBldData->m_HorizProj[ix] );

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
	bldStatus = BldSendPacket( 0, srcPhysicalID, xtcType, &pNDArray->epicsTS, pBufferOrig, sBuffer );
	//	TODO: Pre-allocate storage to avoid malloc/free
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
void PluginSpectrometer::processCallbacks(NDArray *pArray)
{
//	const char	*	functionName = "PluginSpectrometer::processCallbacks";

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
	getIntegerParam( PluginSpectrometerComputeCentroid,		&computeCentroid );
	getIntegerParam( PluginSpectrometerComputeProfiles,		&computeProfiles );
	getIntegerParam( PluginSpectrometerComputeProjections,	&computeProjections );
	getIntegerParam( PluginSpectrometerSendBld,				&sendBld );

	if (pArray->ndims > 0) sizeX = pArray->dims[0].size;
	if (pArray->ndims == 1) sizeY = 1;
	if (pArray->ndims > 1)	sizeY = pArray->dims[1].size;

	if (sizeX != this->profileSizeX)
	{
		this->profileSizeX = sizeX;
		setIntegerParam(PluginSpectrometerProfileSizeX,	(int)this->profileSizeX);
		for ( size_t i=0; i<MAX_PROFILE_TYPES; i++)
		{
			if (this->profileX[i]) free(this->profileX[i]);
			this->profileX[i] = (double *)malloc(this->profileSizeX * sizeof(double));
		}
	}
	if (sizeY != this->profileSizeY)
	{
		this->profileSizeY = sizeY;
		setIntegerParam(PluginSpectrometerProfileSizeY, (int)this->profileSizeY);
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
		getIntegerParam(PluginSpectrometerBgdWidth, &bgdWidth);
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
		setDoubleParam(PluginSpectrometerMinValue,	 pBld->min);
		setDoubleParam(PluginSpectrometerMinX,		 (double)pBld->minX);
		setDoubleParam(PluginSpectrometerMinY,		 (double)pBld->minY);
		setDoubleParam(PluginSpectrometerMaxValue,	 pBld->max);
		setDoubleParam(PluginSpectrometerMaxX,		 (double)pBld->maxX);
		setDoubleParam(PluginSpectrometerMaxY,		 (double)pBld->maxY);
		setDoubleParam(PluginSpectrometerMeanValue,	 pBld->mean);
		setDoubleParam(PluginSpectrometerSigmaValue,  pBld->sigma);
		setDoubleParam(PluginSpectrometerTotal,		 pBld->total);
		setDoubleParam(PluginSpectrometerNet,		 pBld->net);
		asynPrint(	this->pasynUserSelf, ASYN_TRACEIO_DRIVER,
					(char *)pArray->pData, arrayInfo.totalBytes,
					"%s: min=%f, max=%f, mean=%f, total=%f, net=%f",
					functionName, pBld->min, pBld->max, pBld->mean, pBld->total, pBld->net );
	}
#endif

	if (computeCentroid)
	{
		doComputeCentroid(pArray);
		setDoubleParam(PluginSpectrometerCentroidX,	 this->centroidX);
		setDoubleParam(PluginSpectrometerCentroidY,	 this->centroidY);
		setDoubleParam(PluginSpectrometerSigmaX,		 this->sigmaX);
		setDoubleParam(PluginSpectrometerSigmaY,		 this->sigmaY);
		setDoubleParam(PluginSpectrometerSigmaXY,	 this->sigmaXY);
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
		doCallbacksFloat64Array( this->profileX[profAverage],	this->profileSizeX,	PluginSpectrometerProfileAverageX,	0 );
		doCallbacksFloat64Array( this->profileY[profAverage],	this->profileSizeY,	PluginSpectrometerProfileAverageY,	0 );
		doCallbacksFloat64Array( this->profileX[profThreshold],	this->profileSizeX,	PluginSpectrometerProfileThresholdX,	0 );
		doCallbacksFloat64Array( this->profileY[profThreshold],	this->profileSizeY,	PluginSpectrometerProfileThresholdY,	0 );
		doCallbacksFloat64Array( this->profileX[profCentroid],	this->profileSizeX,	PluginSpectrometerProfileCentroidX,	0 );
		doCallbacksFloat64Array( this->profileY[profCentroid],	this->profileSizeY,	PluginSpectrometerProfileCentroidY,	0 );
		doCallbacksFloat64Array( this->profileX[profCursor],	this->profileSizeX,	PluginSpectrometerProfileCursorX,	0 );
		doCallbacksFloat64Array( this->profileY[profCursor],	this->profileSizeY,	PluginSpectrometerProfileCursorY,	0 );
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
asynStatus PluginSpectrometer::writeInt32(asynUser *pasynUser, epicsInt32 value)
{
	static const char	*	functionName	= "PluginSpectrometer::writeInt32";
	const char			*	reasonName		= "unknownReason";
	int						function		= pasynUser->reason;
	asynStatus				status			= asynSuccess;

	/* Set the parameter in the parameter library. */
	status = (asynStatus) setIntegerParam(function, value);

	if (function == PluginSpectrometerCursorX)
	{
		this->cursorX = value;
		if (this->pArrays[0])
		{
			doComputeProfiles(this->pArrays[0]);
		}
	} else if (function == PluginSpectrometerCursorY)
	{
		this->cursorY = value;
		if (this->pArrays[0])
		{
			doComputeProfiles(this->pArrays[0]);
		}
	} else
	{
		/* If this parameter belongs to a base class call its method */
		if (function < FIRST_PLUGIN_SPEC_PARAM)
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
asynStatus	PluginSpectrometer::writeFloat64(asynUser *pasynUser, epicsFloat64 value)
{
	static const char	*	functionName	= "PluginSpectrometer::writeFloat64";
	const char			*	reasonName		= "unknownReason";
	int						function		= pasynUser->reason;
	asynStatus				status			= asynSuccess;
	int computeCentroid, computeProfiles;

	/* Set the parameter and readback in the parameter library.  This may be overwritten when we read back the
	 * status at the end, but that's OK */
	status = setDoubleParam(function, value);

	if (function == PluginSpectrometerCentroidThreshold)
	{
		getIntegerParam(PluginSpectrometerComputeCentroid, &computeCentroid);
		if (computeCentroid && this->pArrays[0])
		{
			doComputeCentroid(this->pArrays[0]);
			getIntegerParam(PluginSpectrometerComputeProfiles, &computeProfiles);
			if (computeProfiles) doComputeProfiles(this->pArrays[0]);
		}
	}
	else
	{
		/* If this parameter belongs to a base class call its method */
		if (function < FIRST_PLUGIN_SPEC_PARAM)
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



/** Constructor for PluginSpectrometer; most parameters are simply passed to NDPluginDriver::NDPluginDriver.
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
PluginSpectrometer::PluginSpectrometer(const char *portName, int queueSize, int blockingCallbacks,
						 const char *NDArrayPort, int NDArrayAddr,
						 int maxBuffers, size_t maxMemory,
						 int priority, int stackSize)
	/* Invoke the base class constructor */
	: NDPluginDriver(portName, queueSize, blockingCallbacks,
				   NDArrayPort, NDArrayAddr, 1, NUM_PLUGIN_SPEC_PARAMS, maxBuffers, maxMemory,
				   asynInt32ArrayMask | asynFloat64ArrayMask | asynGenericPointerMask,
				   asynInt32ArrayMask | asynFloat64ArrayMask | asynGenericPointerMask,
				   0, 1, priority, stackSize)
{
	//const char *functionName = "PluginSpectrometer::PluginSpectrometer";

	/* Statistics */
	createParam(PluginSpectrometerComputeStatisticsString, asynParamInt32,	   &PluginSpectrometerComputeStatistics);
	createParam(PluginSpectrometerBgdWidthString,		  asynParamInt32,	   &PluginSpectrometerBgdWidth);
	createParam(PluginSpectrometerMinValueString,		  asynParamFloat64,    &PluginSpectrometerMinValue);
	createParam(PluginSpectrometerMinXString,			  asynParamFloat64,    &PluginSpectrometerMinX);
	createParam(PluginSpectrometerMinYString,			  asynParamFloat64,    &PluginSpectrometerMinY);
	createParam(PluginSpectrometerMaxValueString,		  asynParamFloat64,    &PluginSpectrometerMaxValue);
	createParam(PluginSpectrometerMaxXString,			  asynParamFloat64,    &PluginSpectrometerMaxX);
	createParam(PluginSpectrometerMaxYString,			  asynParamFloat64,    &PluginSpectrometerMaxY);
	createParam(PluginSpectrometerMeanValueString,		  asynParamFloat64,    &PluginSpectrometerMeanValue);
	createParam(PluginSpectrometerSigmaValueString,		  asynParamFloat64,    &PluginSpectrometerSigmaValue);
	createParam(PluginSpectrometerTotalString,			  asynParamFloat64,    &PluginSpectrometerTotal);
	createParam(PluginSpectrometerNetString,				  asynParamFloat64,    &PluginSpectrometerNet);

	/* Centroid */
	createParam(PluginSpectrometerComputeCentroidString,   asynParamInt32,	   &PluginSpectrometerComputeCentroid);
	createParam(PluginSpectrometerCentroidThresholdString, asynParamFloat64,    &PluginSpectrometerCentroidThreshold);
	createParam(PluginSpectrometerCentroidXString,		  asynParamFloat64,    &PluginSpectrometerCentroidX);
	createParam(PluginSpectrometerCentroidYString,		  asynParamFloat64,    &PluginSpectrometerCentroidY);
	createParam(PluginSpectrometerSigmaXString,			  asynParamFloat64,    &PluginSpectrometerSigmaX);
	createParam(PluginSpectrometerSigmaYString,			  asynParamFloat64,    &PluginSpectrometerSigmaY);
	createParam(PluginSpectrometerSigmaXYString,			  asynParamFloat64,    &PluginSpectrometerSigmaXY);

	/* Profiles */
	createParam(PluginSpectrometerComputeProfilesString,   asynParamInt32,		  &PluginSpectrometerComputeProfiles);
	createParam(PluginSpectrometerProfileSizeXString,	  asynParamInt32,		  &PluginSpectrometerProfileSizeX);
	createParam(PluginSpectrometerProfileSizeYString,	  asynParamInt32,		  &PluginSpectrometerProfileSizeY);
	createParam(PluginSpectrometerCursorXString,			  asynParamInt32,		  &PluginSpectrometerCursorX);
	createParam(PluginSpectrometerCursorYString,			  asynParamInt32,		  &PluginSpectrometerCursorY);
	createParam(PluginSpectrometerProfileAverageXString,   asynParamFloat64Array,  &PluginSpectrometerProfileAverageX);
	createParam(PluginSpectrometerProfileAverageYString,   asynParamFloat64Array,  &PluginSpectrometerProfileAverageY);
	createParam(PluginSpectrometerProfileThresholdXString, asynParamFloat64Array,  &PluginSpectrometerProfileThresholdX);
	createParam(PluginSpectrometerProfileThresholdYString, asynParamFloat64Array,  &PluginSpectrometerProfileThresholdY);
	createParam(PluginSpectrometerProfileCentroidXString,  asynParamFloat64Array,  &PluginSpectrometerProfileCentroidX);
	createParam(PluginSpectrometerProfileCentroidYString,  asynParamFloat64Array,  &PluginSpectrometerProfileCentroidY);
	createParam(PluginSpectrometerProfileCursorXString,	  asynParamFloat64Array,  &PluginSpectrometerProfileCursorX);
	createParam(PluginSpectrometerProfileCursorYString,	  asynParamFloat64Array,  &PluginSpectrometerProfileCursorY);

	/* Horiz Projections */
	createParam( PluginSpectrometerComputeProjectionsString,	asynParamInt32,			&PluginSpectrometerComputeProjections );
	createParam( PluginSpectrometerHorizBaselineString,  asynParamFloat64,		&PluginSpectrometerHorizBaseline );
	createParam( PluginSpectrometerHorizProjString,	  		asynParamInt32Array,	&PluginSpectrometerHorizProj );
	createParam( PluginSpectrometerAdjCtrMassString,			asynParamFloat64,		&PluginSpectrometerAdjCtrMass );
	createParam( PluginSpectrometerRawCtrMassString,			asynParamFloat64,		&PluginSpectrometerRawCtrMass );
	createParam( PluginSpectrometerAdjIntegralString,		asynParamFloat64,		&PluginSpectrometerAdjIntegral );
	createParam( PluginSpectrometerRawIntegralString,		asynParamFloat64,		&PluginSpectrometerRawIntegral );

	/* BLD Send control */
	createParam( PluginSpectrometerSendBldString, asynParamInt32,		 &PluginSpectrometerSendBld);

	// Clear the profile pointers
	memset(this->profileX, 0, sizeof(this->profileX));
	memset(this->profileY, 0, sizeof(this->profileY));

	/* Set the plugin type string */
	setStringParam(NDPluginDriverPluginType, "PluginSpectrometer");

	/* Try to connect to the array port */
	connectToArrayPort();
}

/** Configuration command */
extern "C" int BldSpectrometerConfigure(const char *portName, int queueSize, int blockingCallbacks,
								 const char *NDArrayPort, int NDArrayAddr,
								 int maxBuffers, size_t maxMemory,
								 int priority, int stackSize)
{
	new PluginSpectrometer(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr,
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

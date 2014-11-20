#ifndef PluginBldSpectrometer_H
#define PluginBldSpectrometer_H

#include <vector>

#include <epicsTypes.h>

#include "asynStandardInterfaces.h"
#include "NDPluginDriver.h"

// NUmber of peaks supported by PluginBldSpectrometer.template
#define	N_PEAKS_MAX		1

typedef struct SpectrometerPeak
{
	size_t					m_Start;		// Starting column of peak
	size_t					m_End;			// End column not included in peak
	double					m_PeakPos;		// Peak index position, units are pixels relative to start of row
	double					m_PeakCenter;	// Peak center of mass, units are pixels relative to start of row
	double					m_PeakHeight;	// Peak height, range MIN_INT<epicsType> .. MAX_INT<epicsType>
	double					m_PeakFwhm;		// Peak FullWidthHalfMax, units are pixels
}	SpectrometerPeak_t;

typedef struct BldSpectrometer
{
	size_t					nElements;
	double					m_rawCenterOfMass;	// Raw center of mass
	double					m_adjCenterOfMass;	// Baseline adjusted center of mass (Only includes pixels at or above baseline)
	double					m_rawIntegral;		// Integral of raw horizontal projection values
	double					m_adjIntegral;		// Baseline adjusted integral (Only includes pixels at or above baseline)
	double					m_baselineThreshold;
	size_t					m_horizProjWidth;
	size_t					m_horizProjFirstRowUsed;
	size_t					m_horizProjLastRowUsed;
	std::vector<double>		m_HorizProj;		// Horizontal projection of sample values by column
	std::vector<SpectrometerPeak>	m_Peaks;

	// These values cloned from PluginStats
	double	total;
	double	net;
	double	mean;
	double	sigma;
	double	min;
	size_t	minX;
	size_t	minY;
	double	max;
	size_t	maxX;
	size_t	maxY;
}	BldSpectrometer_t;

typedef enum
{
	profAverage,
	profThreshold,
	profCentroid,
	profCursor
}	NDStatProfileType;
#define MAX_PROFILE_TYPES profCursor+1

/* Statistics */
#define PluginBldSpectrometerComputeStatisticsString  "BLD_COMPUTE_STATISTICS"  /* (asynInt32,        r/w) Compute statistics? */
#define PluginBldSpectrometerBgdWidthString           "BLD_BGD_WIDTH"           /* (asynInt32,        r/w) Width of background region when computing net */
#define PluginBldSpectrometerMinValueString           "BLD_MIN_VALUE"           /* (asynFloat64,      r/o) Minimum counts in any element */
#define PluginBldSpectrometerMinXString               "BLD_MIN_X"               /* (asynFloat64,      r/o) X position of minimum counts */
#define PluginBldSpectrometerMinYString               "BLD_MIN_Y"               /* (asynFloat64,      r/o) Y position of minimum counts */
#define PluginBldSpectrometerMaxValueString           "BLD_MAX_VALUE"           /* (asynFloat64,      r/o) Maximum counts in any element */
#define PluginBldSpectrometerMaxXString               "BLD_MAX_X"               /* (asynFloat64,      r/o) X position of maximum counts */
#define PluginBldSpectrometerMaxYString               "BLD_MAX_Y"               /* (asynFloat64,      r/o) Y position of maximum counts */
#define PluginBldSpectrometerMeanValueString          "BLD_MEAN_VALUE"          /* (asynFloat64,      r/o) Mean counts of all elements */
#define PluginBldSpectrometerSigmaValueString         "BLD_SIGMA_VALUE"         /* (asynFloat64,      r/o) Sigma of all elements */
#define PluginBldSpectrometerTotalString              "BLD_TOTAL"               /* (asynFloat64,      r/o) Sum of all elements */
#define PluginBldSpectrometerNetString                "BLD_NET"                 /* (asynFloat64,      r/o) Sum of all elements minus background */

/* Centroid */
#define PluginBldSpectrometerComputeCentroidString    "BLD_COMPUTE_CENTROID"    /* (asynInt32,        r/w) Compute centroid? */
#define PluginBldSpectrometerCentroidThresholdString  "BLD_CENTROID_THRESHOLD"  /* (asynFloat64,      r/w) Threshold when computing centroids */
#define PluginBldSpectrometerCentroidXString          "BLD_CENTROIDX_VALUE"     /* (asynFloat64,      r/o) X centroid */
#define PluginBldSpectrometerCentroidYString          "BLD_CENTROIDY_VALUE"     /* (asynFloat64,      r/o) Y centroid */
#define PluginBldSpectrometerSigmaXString             "BLD_SIGMAX_VALUE"        /* (asynFloat64,      r/o) Sigma X */
#define PluginBldSpectrometerSigmaYString             "BLD_SIGMAY_VALUE"        /* (asynFloat64,      r/o) Sigma Y */
#define PluginBldSpectrometerSigmaXYString            "BLD_SIGMAXY_VALUE"       /* (asynFloat64,      r/o) Sigma XY */

/* Profiles*/
#define PluginBldSpectrometerComputeProfilesString    "BLD_COMPUTE_PROFILES"    /* (asynInt32,        r/w) Compute profiles? */
#define PluginBldSpectrometerProfileSizeXString       "BLD_PROFILE_SIZE_X"      /* (asynInt32,        r/o) X profile size */
#define PluginBldSpectrometerProfileSizeYString       "BLD_PROFILE_SIZE_Y"      /* (asynInt32,        r/o) Y profile size */
#define PluginBldSpectrometerCursorXString            "BLD_CURSOR_X"            /* (asynInt32,        r/w) X cursor position */
#define PluginBldSpectrometerCursorYString            "BLD_CURSOR_Y"            /* (asynInt32,        r/w) Y cursor position */
#define PluginBldSpectrometerProfileAverageXString    "BLD_PROFILE_AVERAGE_X"   /* (asynFloat64Array, r/o) X average profile array */
#define PluginBldSpectrometerProfileAverageYString    "BLD_PROFILE_AVERAGE_Y"   /* (asynFloat64Array, r/o) Y average profile array */
#define PluginBldSpectrometerProfileThresholdXString  "BLD_PROFILE_THRESHOLD_X" /* (asynFloat64Array, r/o) X average profile array after threshold */
#define PluginBldSpectrometerProfileThresholdYString  "BLD_PROFILE_THRESHOLD_Y" /* (asynFloat64Array, r/o) Y average profile array after threshold */
#define PluginBldSpectrometerProfileCentroidXString   "BLD_PROFILE_CENTROID_X"  /* (asynFloat64Array, r/o) X centroid profile array */
#define PluginBldSpectrometerProfileCentroidYString   "BLD_PROFILE_CENTROID_Y"  /* (asynFloat64Array, r/o) Y centroid profile array */
#define PluginBldSpectrometerProfileCursorXString     "BLD_PROFILE_CURSOR_X"    /* (asynFloat64Array, r/o) X cursor profile array */
#define PluginBldSpectrometerProfileCursorYString     "BLD_PROFILE_CURSOR_Y"    /* (asynFloat64Array, r/o) Y cursor profile array */

/* Projections */
#define PluginBldSpectrometerComputeProjectionsString    "BLD_COMPUTE_PROJ"		/* (asynInt32,        r/w) Compute projections? */

#define PluginBldSpectrometerSendBldString   "BLD_SEND"   /* (asynInt32,        r/w) Set non-zero to send BLD pkts */


/* Arrays of total and net counts for MCA or waveform record */
#define PluginBldSpectrometerCallbackPeriodString     "CALLBACK_PERIOD"     /* (asynFloat64,      r/w) Callback period */

/** Does image statistics.  These include
  * Min, max, mean, sigma
  * X and Y centroid and sigma
  */
class PluginBldSpectrometer : public NDPluginDriver
{
public:
	PluginBldSpectrometer(const char *portName, int queueSize, int blockingCallbacks,
				 const char *NDArrayPort, int NDArrayAddr,
				 int maxBuffers, size_t maxMemory,
				 int priority, int stackSize);
	/* These methods override the virtual methods in the base class */
	void processCallbacks(NDArray *pArray);
	asynStatus writeInt32(asynUser *pasynUser, epicsInt32 value);
	asynStatus writeFloat64(asynUser *pasynUser, epicsFloat64 value);

	template <typename epicsType> void doComputeStatisticsT(NDArray *pArray, BldSpectrometer_t *pBld);
	int doComputeStatistics(NDArray *pArray, BldSpectrometer_t *pBld);
	template <typename epicsType> asynStatus doComputeCentroidT(NDArray *pArray);
	asynStatus doComputeCentroid(NDArray *pArray);
	template <typename epicsType> asynStatus doComputeProfilesT(NDArray *pArray);
	asynStatus doComputeProfiles(NDArray *pArray);
	template <typename epicsType> asynStatus	doComputeProjectionsT(	NDArray				*	pArray,
																		BldSpectrometer_t	*	pBldData	);
	asynStatus									doComputeProjections(	NDArray				*	pArray,
																		BldSpectrometer_t	*	pBldData	);
	asynStatus									doSendBld(				NDArray				*	pArray,
																		BldSpectrometer_t	*	pBldData	);

protected:
	int PluginBldSpectrometerComputeStatistics;
	#define FIRST_PLUGIN_BLD_SPEC_PARAM PluginBldSpectrometerComputeStatistics
	/* Statistics */
	int PluginBldSpectrometerBgdWidth;
	int PluginBldSpectrometerMinValue;
	int PluginBldSpectrometerMinX;
	int PluginBldSpectrometerMinY;
	int PluginBldSpectrometerMaxValue;
	int PluginBldSpectrometerMaxX;
	int PluginBldSpectrometerMaxY;
	int PluginBldSpectrometerMeanValue;
	int PluginBldSpectrometerSigmaValue;
	int PluginBldSpectrometerTotal;
	int PluginBldSpectrometerNet;

	/* Centroid */
	int PluginBldSpectrometerComputeCentroid;
	int PluginBldSpectrometerCentroidThreshold;
	int PluginBldSpectrometerCentroidX;
	int PluginBldSpectrometerCentroidY;
	int PluginBldSpectrometerSigmaX;
	int PluginBldSpectrometerSigmaY;
	int PluginBldSpectrometerSigmaXY;

	/* Profiles */
	int PluginBldSpectrometerComputeProfiles;
	int PluginBldSpectrometerProfileSizeX;
	int PluginBldSpectrometerProfileSizeY;
	int PluginBldSpectrometerCursorX;
	int PluginBldSpectrometerCursorY;
	int PluginBldSpectrometerProfileAverageX;
	int PluginBldSpectrometerProfileAverageY;
	int PluginBldSpectrometerProfileThresholdX;
	int PluginBldSpectrometerProfileThresholdY;
	int PluginBldSpectrometerProfileCentroidX;
	int PluginBldSpectrometerProfileCentroidY;
	int PluginBldSpectrometerProfileCursorX;
	int PluginBldSpectrometerProfileCursorY;

	int PluginBldSpectrometerComputeProjections;
	int PluginBldSpectrometerSendBld;

	#define LAST_PLUGIN_BLD_SPEC_PARAM PluginBldSpectrometerSendBld

private:
	double			centroidThreshold;
	double			centroidX;
	double			centroidY;
	double			sigmaX;
	double			sigmaY;
	double			sigmaXY;
	double		*	profileX[MAX_PROFILE_TYPES];
	double		*	profileY[MAX_PROFILE_TYPES];
	size_t			profileSizeX;
	size_t			profileSizeY;
	size_t			cursorX;
	size_t			cursorY;
	epicsInt32	*	totalArray;
	epicsInt32	*	netArray;
};
#define NUM_PLUGIN_BLD_SPEC_PARAMS ((int)(&LAST_PLUGIN_BLD_SPEC_PARAM - &FIRST_PLUGIN_BLD_SPEC_PARAM + 1))

#endif

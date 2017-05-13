#ifndef PluginSpectrometer_H
#define PluginSpectrometer_H

#include <vector>

#include <epicsTypes.h>

#include "asynStandardInterfaces.h"
#include "NDPluginDriver.h"

// NUmber of peaks supported by PluginSpectrometer.template
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
	double					m_RawCtrMass;	// Raw center of mass
	double					m_AdjCtrMass;	// Baseline adjusted center of mass (Only includes pixels at or above baseline)
	double					m_RawIntegral;		// Integral of raw horizontal projection values
	double					m_AdjIntegral;		// Baseline adjusted integral (Only includes pixels at or above baseline)
	double					m_DarkLevel;
	double					m_HorizBaseline;
	size_t					m_HorizProjWidth;
	size_t					m_HorizProjFirstRowUsed;
	size_t					m_HorizProjLastRowUsed;
	std::vector<double>		m_HorizProj;	// Horizontal projection of sample values by column
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
#define PluginSpectrometerComputeStatisticsString  "SPEC_COMPUTE_STATISTICS"  /* (asynInt32,        r/w) Compute statistics? */
#define PluginSpectrometerBgdWidthString           "SPEC_BGD_WIDTH"           /* (asynInt32,        r/w) Width of background region when computing net */
#define PluginSpectrometerMinValueString           "SPEC_MIN_VALUE"           /* (asynFloat64,      r/o) Minimum counts in any element */
#define PluginSpectrometerMinXString               "SPEC_MIN_X"               /* (asynFloat64,      r/o) X position of minimum counts */
#define PluginSpectrometerMinYString               "SPEC_MIN_Y"               /* (asynFloat64,      r/o) Y position of minimum counts */
#define PluginSpectrometerMaxValueString           "SPEC_MAX_VALUE"           /* (asynFloat64,      r/o) Maximum counts in any element */
#define PluginSpectrometerMaxXString               "SPEC_MAX_X"               /* (asynFloat64,      r/o) X position of maximum counts */
#define PluginSpectrometerMaxYString               "SPEC_MAX_Y"               /* (asynFloat64,      r/o) Y position of maximum counts */
#define PluginSpectrometerMeanValueString          "SPEC_MEAN_VALUE"          /* (asynFloat64,      r/o) Mean counts of all elements */
#define PluginSpectrometerSigmaValueString         "SPEC_SIGMA_VALUE"         /* (asynFloat64,      r/o) Sigma of all elements */
#define PluginSpectrometerTotalString              "SPEC_TOTAL"               /* (asynFloat64,      r/o) Sum of all elements */
#define PluginSpectrometerNetString                "SPEC_NET"                 /* (asynFloat64,      r/o) Sum of all elements minus background */

/* Centroid */
#define PluginSpectrometerComputeCentroidString    "SPEC_COMPUTE_CENTROID"    /* (asynInt32,        r/w) Compute centroid? */
#define PluginSpectrometerCentroidThresholdString  "SPEC_CENTROID_THRESHOLD"  /* (asynFloat64,      r/w) Threshold when computing centroids */
#define PluginSpectrometerCentroidXString          "SPEC_CENTROIDX_VALUE"     /* (asynFloat64,      r/o) X centroid */
#define PluginSpectrometerCentroidYString          "SPEC_CENTROIDY_VALUE"     /* (asynFloat64,      r/o) Y centroid */
#define PluginSpectrometerSigmaXString             "SPEC_SIGMAX_VALUE"        /* (asynFloat64,      r/o) Sigma X */
#define PluginSpectrometerSigmaYString             "SPEC_SIGMAY_VALUE"        /* (asynFloat64,      r/o) Sigma Y */
#define PluginSpectrometerSigmaXYString            "SPEC_SIGMAXY_VALUE"       /* (asynFloat64,      r/o) Sigma XY */

/* Profiles*/
#define PluginSpectrometerComputeProfilesString    "SPEC_COMPUTE_PROFILES"    /* (asynInt32,        r/w) Compute profiles? */
#define PluginSpectrometerProfileSizeXString       "SPEC_PROFILE_SIZE_X"      /* (asynInt32,        r/o) X profile size */
#define PluginSpectrometerProfileSizeYString       "SPEC_PROFILE_SIZE_Y"      /* (asynInt32,        r/o) Y profile size */
#define PluginSpectrometerCursorXString            "SPEC_CURSOR_X"            /* (asynInt32,        r/w) X cursor position */
#define PluginSpectrometerCursorYString            "SPEC_CURSOR_Y"            /* (asynInt32,        r/w) Y cursor position */
#define PluginSpectrometerProfileAverageXString    "SPEC_PROFILE_AVERAGE_X"   /* (asynFloat64Array, r/o) X average profile array */
#define PluginSpectrometerProfileAverageYString    "SPEC_PROFILE_AVERAGE_Y"   /* (asynFloat64Array, r/o) Y average profile array */
#define PluginSpectrometerProfileThresholdXString  "SPEC_PROFILE_THRESHOLD_X" /* (asynFloat64Array, r/o) X average profile array after threshold */
#define PluginSpectrometerProfileThresholdYString  "SPEC_PROFILE_THRESHOLD_Y" /* (asynFloat64Array, r/o) Y average profile array after threshold */
#define PluginSpectrometerProfileCentroidXString   "SPEC_PROFILE_CENTROID_X"  /* (asynFloat64Array, r/o) X centroid profile array */
#define PluginSpectrometerProfileCentroidYString   "SPEC_PROFILE_CENTROID_Y"  /* (asynFloat64Array, r/o) Y centroid profile array */
#define PluginSpectrometerProfileCursorXString     "SPEC_PROFILE_CURSOR_X"    /* (asynFloat64Array, r/o) X cursor profile array */
#define PluginSpectrometerProfileCursorYString     "SPEC_PROFILE_CURSOR_Y"    /* (asynFloat64Array, r/o) Y cursor profile array */

/* Horiz Projections */
#define PluginSpectrometerComputeProjectionsString	"SPEC_COMPUTE_PROJ"	/* (asynInt32,		r/w) Compute horiz projection? */
#define PluginSpectrometerDarkLevelString			"SPEC_DARK_LEVEL"	/* (asynFloat64,	r/w) Baseline threshold */
#define PluginSpectrometerHorizBaselineString		"SPEC_HORIZ_BASELINE"/* (asynFloat64,	r/w) Baseline threshold */
#define PluginSpectrometerHorizProjString			"SPEC_HORIZ_PROJ"	/* (asynUInt32Array,r/o) Horiz projection array */
#define PluginSpectrometerAdjCtrMassString   		"SPEC_ADJ_CTR_MASS"	/* (asynFloat64,	r/o) Baseline Adj Center of Mass */
#define PluginSpectrometerRawCtrMassString   		"SPEC_RAW_CTR_MASS"	/* (asynFloat64,	r/o) Raw Center of Mass */
#define PluginSpectrometerAdjIntegralString   		"SPEC_ADJ_INTEGRAL"	/* (asynFloat64,	r/o) Baseline Adj integral */
#define PluginSpectrometerRawIntegralString   		"SPEC_RAW_INTEGRAL"	/* (asynFloat64,	r/o) Raw integral under horiz proj */

#define PluginSpectrometerSendBldString   "SPEC_SEND"   /* (asynInt32,        r/w) Set non-zero to send BLD pkts */


/* Arrays of total and net counts for MCA or waveform record */
#define PluginSpectrometerCallbackPeriodString     "CALLBACK_PERIOD"     /* (asynFloat64,      r/w) Callback period */

/** Does image statistics.  These include
  * Min, max, mean, sigma
  * X and Y centroid and sigma
  */
class PluginSpectrometer : public NDPluginDriver
{
public:
	PluginSpectrometer(const char *portName, int queueSize, int blockingCallbacks,
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
	int PluginSpectrometerComputeStatistics;
	#define FIRST_PLUGIN_SPEC_PARAM PluginSpectrometerComputeStatistics
	/* Statistics */
	int PluginSpectrometerBgdWidth;
	int PluginSpectrometerMinValue;
	int PluginSpectrometerMinX;
	int PluginSpectrometerMinY;
	int PluginSpectrometerMaxValue;
	int PluginSpectrometerMaxX;
	int PluginSpectrometerMaxY;
	int PluginSpectrometerMeanValue;
	int PluginSpectrometerSigmaValue;
	int PluginSpectrometerTotal;
	int PluginSpectrometerNet;

	/* Centroid */
	int PluginSpectrometerComputeCentroid;
	int PluginSpectrometerCentroidThreshold;
	int PluginSpectrometerCentroidX;
	int PluginSpectrometerCentroidY;
	int PluginSpectrometerSigmaX;
	int PluginSpectrometerSigmaY;
	int PluginSpectrometerSigmaXY;

	/* Profiles */
	int PluginSpectrometerComputeProfiles;
	int PluginSpectrometerProfileSizeX;
	int PluginSpectrometerProfileSizeY;
	int PluginSpectrometerCursorX;
	int PluginSpectrometerCursorY;
	int PluginSpectrometerProfileAverageX;
	int PluginSpectrometerProfileAverageY;
	int PluginSpectrometerProfileThresholdX;
	int PluginSpectrometerProfileThresholdY;
	int PluginSpectrometerProfileCentroidX;
	int PluginSpectrometerProfileCentroidY;
	int PluginSpectrometerProfileCursorX;
	int PluginSpectrometerProfileCursorY;

	/* Horiz Projection */
	int PluginSpectrometerComputeProjections;
	int PluginSpectrometerDarkLevel;
	int PluginSpectrometerHorizBaseline;
	int PluginSpectrometerHorizProj;
	int PluginSpectrometerAdjCtrMass;
	int PluginSpectrometerRawCtrMass;
	int PluginSpectrometerAdjIntegral;
	int PluginSpectrometerRawIntegral;

	/* BLD Send Control */
	int PluginSpectrometerSendBld;

	#define LAST_PLUGIN_SPEC_PARAM PluginSpectrometerSendBld

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
#define NUM_PLUGIN_SPEC_PARAMS ((int)(&LAST_PLUGIN_SPEC_PARAM - &FIRST_PLUGIN_SPEC_PARAM + 1))

#endif

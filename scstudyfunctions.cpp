#include "sierrachart.h"
#include "scstudyfunctions.h"


#pragma unmanaged 

/*****************************************************************************

	User note: The functions in this file are intermediate level functions you can
	copy and call from your primary scsf_ functions.  These functions are not
	complete study functions, but are used by primary study functions to
	perform calculations.  Where you see _S at the end, this means that the
	function is a single step function, and only fills in the array element
	at Index.  

*****************************************************************************/

/*==========================================================================*/
SCFloatArrayRef CCI_S(SCFloatArrayRef In, SCFloatArrayRef MAOut, SCFloatArrayRef CCIOut, int Index, int Length, float Multiplier, unsigned int MovingAverageType)
{
	if (Length < 1)
		return CCIOut;

	switch (MovingAverageType)
	{
	case MOVAVGTYPE_EXPONENTIAL:
		ExponentialMovingAverage_S(In, MAOut, Index, Length);
		break;

	case MOVAVGTYPE_LINEARREGRESSION:
		LinearRegressionIndicator_S(In, MAOut, Index, Length);
		break;

	default:  // Unknown moving average type
	case MOVAVGTYPE_SIMPLE:
		SimpleMovAvg_S(In, MAOut, Index, Length);
		break;

	case MOVAVGTYPE_WEIGHTED:
		WeightedMovingAverage_S(In, MAOut, Index, Length);
		break;

	case MOVAVGTYPE_WILDERS:
		WildersMovingAverage_S(In, MAOut, Index, Length);
		break;

	case MOVAVGTYPE_SIMPLE_SKIP_ZEROS:
		SimpleMovAvgSkipZeros_S(In, MAOut, Index, Length);
		break;

	case MOVAVGTYPE_SMOOTHED:
		SmoothedMovingAverage_S(In, MAOut, Index, Length, 0);
		break;
	}

	float Num0 = 0;
	for (int j = Index; j > Index - Length && j >= 0; j--)
		Num0 += fabs(MAOut[Index] - In[j]);

	Num0 /= Length;

	CCIOut[Index] = (In[Index] - MAOut[Index]) / (Num0 * Multiplier);

	return CCIOut;
}

/*==========================================================================*/
SCFloatArrayRef CCISMA_S(SCFloatArrayRef In, SCFloatArrayRef SMAOut, SCFloatArrayRef CCIOut, int Index, int Length, float Multiplier)
{
	return CCI_S(In, SMAOut, CCIOut, Index, Length, Multiplier, MOVAVGTYPE_SIMPLE);
}

/*==========================================================================*/
float Highest(SCFloatArrayRef In, int StartIndex, int Length)
{
	float High = -FLT_MAX;
	
	// Get the high from the last Length indexes in the In array
	for (int SrcIndex = StartIndex; SrcIndex > StartIndex - Length; --SrcIndex)
	{
		if (SrcIndex < 0 || SrcIndex >= In.GetArraySize())
			continue;
		
		if (In[SrcIndex] > High)
			High = In[SrcIndex];
	}
	
	return High;
}

/*==========================================================================*/
SCFloatArrayRef Highest_S(SCFloatArrayRef In, SCFloatArrayRef Out, int Index, int Length)
{
	// Get the high from the last Length indexes in the In array
	float High = Highest(In, Index, Length);
	
	// Put the high in the Out array
	if (High == -FLT_MAX)
		Out[Index] = 0.0f;
	else
		Out[Index] = High;
	
	return Out;
}

/*==========================================================================*/
float Lowest(SCFloatArrayRef In, int StartIndex, int Length)
{
	float Low = FLT_MAX;
	
	// Get the low from the last Length indexes in the In array
	for (int SrcIndex = StartIndex; SrcIndex > StartIndex - Length; --SrcIndex)
	{
		if (SrcIndex < 0 || SrcIndex >= In.GetArraySize())
			continue;
		
		if (In[SrcIndex] < Low)
			Low = In[SrcIndex];
	}
	
	return Low;
}

/*==========================================================================*/
SCFloatArrayRef Lowest_S(SCFloatArrayRef In, SCFloatArrayRef Out, int Index, int Length)
{
	// Get the low from the last Length indexes in the In array
	float Low = Lowest(In, Index, Length);
	
	// Put the low in the Out array
	if (Low == FLT_MAX)
		Out[Index] = 0.0f;
	else
		Out[Index] = Low;
	
	return Out;
}

/*==========================================================================*/
float TrueRange(SCBaseDataRef BaseDataIn, int Index)
{
	if (Index == 0)
		return BaseDataIn[SC_HIGH][0] - BaseDataIn[SC_LOW][0];
	
	float HighLowRange = BaseDataIn[SC_HIGH][Index] - BaseDataIn[SC_LOW][Index];
	float HighToPreviousCloseDifference = fabs(BaseDataIn[SC_HIGH][Index] - BaseDataIn[SC_LAST][Index - 1]);
	float LowToPreviousCloseDifference = fabs(BaseDataIn[SC_LOW][Index] - BaseDataIn[SC_LAST][Index - 1]);
	
	return  max(HighLowRange, max(HighToPreviousCloseDifference, LowToPreviousCloseDifference));
	
}

/*==========================================================================*/
SCFloatArrayRef TrueRange_S(SCBaseDataRef BaseDataIn, SCFloatArrayRef Out, int Index)
{
	Out[Index] = TrueRange(BaseDataIn, Index);
	
	return Out;
}

/*==========================================================================*/
SCFloatArrayRef AverageTrueRange_S(SCBaseDataRef BaseDataIn, SCFloatArrayRef TROut, SCFloatArrayRef ATROut, int Index, int Length, unsigned int MovingAverageType)
{
	// True Range
	TrueRange_S(BaseDataIn, TROut, Index);
	
	// Average
	MovingAverage_S(TROut, ATROut, MovingAverageType, Index, Length);
	
	return ATROut;
}

/*==========================================================================*/
SCFloatArrayRef OnBalanceVolume_S(SCBaseDataRef BaseDataIn, SCFloatArrayRef Out, int Index)
{
	// Min index of 1
	if (Index < 1)
		return Out;
	
	if (BaseDataIn[SC_LAST][Index] > BaseDataIn[SC_LAST][Index - 1])
		Out[Index] = Out[Index - 1] + BaseDataIn[SC_VOLUME][Index];
	else if (BaseDataIn[SC_LAST][Index] < BaseDataIn[SC_LAST][Index - 1])
		Out[Index] = Out[Index - 1] - BaseDataIn[SC_VOLUME][Index];
	else  // Equal
		Out[Index] = Out[Index - 1];
	
	return Out;
}

/*==========================================================================*/
SCFloatArrayRef OnBalanceVolumeShortTerm_S(SCBaseDataRef BaseDataIn, SCFloatArrayRef Out, SCFloatArrayRef OBVTemp, int Index, int Length)
{
	// Need at least two bars in chart
	if (Index < 1)
		return Out;
	
	// OBV
	if (BaseDataIn[SC_LAST][Index - 1] > BaseDataIn[SC_LAST][Index])
	{
		// Since current close is less than previous, subtract
		OBVTemp[Index] = -BaseDataIn[SC_VOLUME][Index];
	}
	else if (BaseDataIn[SC_LAST][Index - 1] < BaseDataIn[SC_LAST][Index])
	{
		// Add...
		OBVTemp[Index] = BaseDataIn[SC_VOLUME][Index];
	}
	else  // Equal
	{
		// Do not change
		OBVTemp[Index] = 0;
	}
	
	// Prevent looking back into a negative array index
	if (Index < Length)
	{
		// Regular OBV for first N terms
		Out[Index] = Out[Index] + OBVTemp[Index];
	}
	else
	{
		// Adjust current OBV by subtracting what we did to get the
		// previous OBV.  Example, if at position 11, data was added to OBV, then
		// at position 12, that data would be subtracted to make it a
		// 'zero' start.
		Out[Index] = Out[Index - 1] + OBVTemp[Index] - OBVTemp[Index - Length];
	}
	
	return Out;
}

/*==========================================================================*/
SCFloatArrayRef MovingAverage_S(SCFloatArrayRef In, SCFloatArrayRef Out, unsigned int MovingAverageType, int Index, int Length)
{
	switch (MovingAverageType)
	{
		case MOVAVGTYPE_EXPONENTIAL:
		return ExponentialMovingAverage_S(In, Out, Index, Length);
		
		case MOVAVGTYPE_LINEARREGRESSION:
		return LinearRegressionIndicator_S(In, Out, Index, Length);
		
		default:  // Unknown moving average type
		case MOVAVGTYPE_SIMPLE:
		return SimpleMovAvg_S(In, Out, Index, Length);
		
		case MOVAVGTYPE_WEIGHTED:
		return WeightedMovingAverage_S(In, Out, Index, Length);
		
		case MOVAVGTYPE_WILDERS:
		return WildersMovingAverage_S(In, Out, Index, Length);

		case MOVAVGTYPE_SIMPLE_SKIP_ZEROS:
		return SimpleMovAvgSkipZeros_S(In, Out, Index, Length);

		case MOVAVGTYPE_SMOOTHED:
		return SmoothedMovingAverage_S(In, Out, Index, Length,0);

	}
}

/*==========================================================================*/
void Stochastic_S(SCBaseDataRef BaseDataIn, SCFloatArrayRef FastKOut, SCFloatArrayRef FastDOut, SCFloatArrayRef SlowDOut, int Index, int FastKLength, int FastDLength, int SlowDLength, unsigned int MovingAverageType)
{
	Stochastic2_S(BaseDataIn[SC_HIGH], BaseDataIn[SC_LOW], BaseDataIn[SC_LAST],  FastKOut,  FastDOut,  SlowDOut,  Index,  FastKLength,  FastDLength,  SlowDLength, MovingAverageType);
}

/*==========================================================================*/
void Stochastic2_S(SCFloatArrayRef InputDataHigh, SCFloatArrayRef InputDataLow, SCFloatArrayRef InputDataLast, SCFloatArrayRef FastKOut, SCFloatArrayRef FastDOut, SCFloatArrayRef SlowDOut, int Index, int FastKLength, int FastDLength, int SlowDLength, unsigned int MovingAverageType)
{
	float High = Highest(InputDataHigh, Index, FastKLength);
	float Low = Lowest(InputDataLow, Index, FastKLength);

	float Range = High - Low;
	if (Range == 0)
		FastKOut[Index] = 100.0f;
	else
		FastKOut[Index] = 100.0f * (InputDataLast[Index] - Low) / Range;

	MovingAverage_S(FastKOut, FastDOut, MovingAverageType, Index, FastDLength);
	MovingAverage_S(FastDOut, SlowDOut, MovingAverageType, Index, SlowDLength);
}

/*============================================================================
	This function calculates an exponential moving average of the In array
	and puts the results in the Out array.
----------------------------------------------------------------------------*/
SCFloatArrayRef ExponentialMovingAverage_S(SCFloatArrayRef In, SCFloatArrayRef Out, int Index, int Length)
{
	if (Index >= In.GetArraySize())
		return Out;

	if(Index < 1 || Length < 1)
		return Out;

	if (Index < Length -1)
		Length = Index+1;

	
	float Multiplier1 = 2.0f / (Length + 1);
	float Multiplier2 = 1.0f - Multiplier1;
	float PreviousMovingAverageValue = Out[Index - 1 ];
	//Check for a previous moving average value of 0 so we can properly initialize the previous value, and also check for out of range values.
	if (PreviousMovingAverageValue == 0.0f || !((PreviousMovingAverageValue > -FLT_MAX) && (PreviousMovingAverageValue < FLT_MAX)))
		Out[Index - 1] = In[Index-1];
	
	Out[Index] = (Multiplier1 * In[Index]) + (Multiplier2 * Out[Index - 1]);
	
	
	return Out;
}

/*==========================================================================*/
SCFloatArrayRef LinearRegressionIndicator_S(SCFloatArrayRef In, SCFloatArrayRef Out, int Index, int Length)
{
	if (Index >= In.GetArraySize())
		return Out;
	
	float sum_y=0, sum_x=0, sum_x2=0, sum_y2=0, sum_x_2=0, sum_y_2=0, sum_xy=0;
	float b_numerator, b_denominator, b,a;

	int Pos = Index;
	if( Index < ( Length - 1 ) )
	{
		Index = Length - 1;
    }

	
	sum_x = (float)((Length * (Length+1)) / 2.0);
	sum_x_2 =  sum_x * sum_x;
	Summation(In,sum_y,Index,Length);
	sum_y_2 = sum_y * sum_y;
	for(int nn=0;nn<Length;nn++)
	{	
		
		sum_y2 +=	In[Index-nn] * In[Index-nn];
		
		sum_xy +=	In[Index-nn]  * (Length-nn);				
	}

	sum_x2=(Length+1)*Length*(2*Length+1)/6.0f; 

	b_numerator		= (Length * sum_xy - sum_x * sum_y);
	b_denominator	= Length * sum_x2 - sum_x_2;
	b				= b_numerator / b_denominator;
	a				= (sum_y-b*sum_x) / Length;

	//compute the end point of the linear regression trendline == linear regression indicator.
	Out[Pos] = a + b * Length;

	return Out;
}

/*==========================================================================*/
// Calculates the Linear Regression Indicator. Also computes standard error which is the last value of a linear regression trend line.

SCFloatArrayRef LinearRegressionIndicatorAndStdErr_S(SCFloatArrayRef In, SCFloatArrayRef Out, SCFloatArrayRef StdErr, int Index, int Length)
{
	if (Index >= In.GetArraySize())
		return Out;
	
	double sum_y=0, sum_x=0, sum_x2=0, sum_y2=0, sum_x_2=0, sum_y_2=0, sum_xy=0;
	double b_numerator, b_denominator, b,a;

	int Pos = Index;
	if( Index < ( Length - 1 ) )
	{
		Index = Length - 1;
    }

	
	sum_x = ((Length * (Length+1)) / 2.0);
	
	sum_x_2 =  sum_x * sum_x;
	
	sum_y = GetSummation(In, Index, Length);
	
	sum_y_2 = sum_y * sum_y;
	for(int nn=0;nn<Length;nn++)
	{	
		sum_y2 +=	In[Index-nn] * In[Index-nn];
		sum_xy +=	In[Index-nn]  * (Length-nn);				
	}

	sum_x2=(Length+1)*Length*(2*Length+1)/6.0f; 

	b_numerator		= (Length * sum_xy - sum_x * sum_y);
	b_denominator	= Length * sum_x2 - sum_x_2;
	b				= b_numerator / b_denominator;
	a				= (sum_y-b*sum_x) / Length;

	//compute the end point of the linear regression trendline == linear regression indicator.
	Out[Pos] = (float)(a + b * Length);

	// compute the standard error
	double temp = (1.0 / (Length * (Length-2.0))) * ( Length * sum_y2 - sum_y_2 - b * b_numerator);
	
	// Check to ensure the data is non-negative before performing the square root operation.
	if(temp > 0)
	{
		StdErr[Pos] = (float)sqrt(temp);
	}
	else
	{
		StdErr[Pos]	= 0;
	}

	return Out;
}

/*============================================================================
	This function calculates an adaptive moving average of the In array and
	puts the results in the Out array.
----------------------------------------------------------------------------*/
SCFloatArrayRef AdaptiveMovAvg_S(SCFloatArrayRef In, SCFloatArrayRef Out, int Index, int Length, float FastSmoothConst, float SlowSmoothConst)
{
	if (Index >= In.GetArraySize())
		return Out;
	
	if (Length < 1)
		return Out;
	
	FastSmoothConst = 2 / (FastSmoothConst + 1);
	SlowSmoothConst = 2 / (SlowSmoothConst + 1);
	
	int pos = max(Length, Index);

	float direction = (In[pos] - In[pos - Length]);
	float sum = 0.0f;
	
	for (int pos2 = pos - (Length - 1); pos2 <= pos; pos2++)
	{
		sum += fabs(In[pos2] - In[pos2 - 1]);
	}

	float volatility = sum;
	
	if (volatility == 0.0f)
		volatility = .000001f;
		
	float er = fabs(direction / volatility);
	float SC = er * (FastSmoothConst - SlowSmoothConst) + SlowSmoothConst;
	SC = SC * SC;
	
	if (Out[pos - 1]==0.0f)
		Out[pos] = In[pos - 1] + SC * (In[pos] - In[pos - 1]);
	else
		Out[pos] = Out[pos - 1] + SC * (In[pos] - Out[pos - 1]);

	return Out;
}

/*============================================================================
	This function calculates a simple moving average of the In array and puts
	the results in the Out array.
----------------------------------------------------------------------------*/
SCFloatArrayRef SimpleMovAvg_S(SCFloatArrayRef In, SCFloatArrayRef Out, int Index, int Length)
{
	
	if (Length < 1)
		return Out;
	
	float sum = 0;

	if (Index < Length -1)
		Length = Index+1;
		
	for(int c = Index - Length + 1; c <= Index; c++)
		sum = sum + In[c]; 
	
	Out[Index] = sum / Length;

	return Out;
}

/*==========================================================================*/
SCFloatArrayRef MovingMedian_S(SCFloatArrayRef In, SCFloatArrayRef Out, SCFloatArrayRef Temp, int Index, int Length)
{
	//Adjust the Length
	if (Index +1 < Length)
		Length = Index +1;

	int BaseIndex = Index - Length + 1;

	if (BaseIndex < 0)//Unnecessary but for safety.
		return Out;

	// Make a copy of the elements to be sorted
	for (int a = BaseIndex; a <= Index; ++a)
		Temp[a] = In[a];

	// This section of code sorts the temporary array just enough to figure
	// out what the middle element is, which is what we need to know to get
	// the median.
	{
		int MiddleIndex = BaseIndex + Length / 2;

		int Bottom = BaseIndex, Top = Index;
		while (Bottom < Top)
		{
			float MiddleValue = Temp[MiddleIndex];

			int NextBottom = Bottom;
			int NextTop = Top;
			do
			{
				while (Temp[NextBottom] < MiddleValue)
					++NextBottom;

				while (Temp[NextTop] > MiddleValue)
					--NextTop;

				if (NextBottom <= NextTop)
				{
					float SwapTemp = Temp[NextBottom];
					Temp[NextBottom] = Temp[NextTop];
					Temp[NextTop] = SwapTemp;

					++NextBottom;
					--NextTop;
				}
			}
			while (NextBottom <= NextTop);

			if (NextBottom > MiddleIndex)
				Top = NextTop;

			if (NextTop < MiddleIndex)
				Bottom = NextBottom;
		}
	}

	// Get the middle element
	// Note: this is not completely accurate for even lengths
	Out[Index] = Temp[BaseIndex + Length / 2];

	return Out;
}

/*==========================================================================*/
SCFloatArrayRef SimpleMovAvgSkipZeros_S(SCFloatArrayRef In, SCFloatArrayRef Out, int Index, int Length)
{
    int count = 0;
	float summ = 0.0f;
	for(int i=Index-Length+1; i <= Index; i++)
	{
		float data = In[i];
		if (data !=0.0f)
		{
			summ += data;
			count ++;
		}
	}

	if (count>0)
	{
		Out[Index] = summ / count;
	}
	else
	{
		Out[Index] = 0.0f;
	}

	return Out;
}

/*==========================================================================*/
SCFloatArrayRef WildersMovingAverage_S(SCFloatArrayRef In, SCFloatArrayRef Out, int Index, int Length)
{
	
	if (Index < 1)
		return Out;


	if(Out[Index- 1] == 0.0)
		SimpleMovAvgSkipZeros_S(In, Out, Index - 1, Length);

	
	if (Out[Index- 1] != 0.0)
		Out[Index] = Out[Index-1] 
	+ (
		(1.0f/(float)Length) 
		*(In[Index] - Out[Index-1])
		);
	
	return Out;
}

/*==========================================================================*/
SCFloatArrayRef WeightedMovingAverage_S(SCFloatArrayRef In, SCFloatArrayRef Out, int Index, int Length)
{
	if ( Length < 1)
		return Out;
	
	double sum =0;
	double divider = (Length * (Length+1.0) / 2.0);

	sum =0;
	int n = Length;
	for(int PriorIndex=Index; PriorIndex > Index - Length; PriorIndex--)
	{
		sum += In[PriorIndex] * (float)(n--);
	}

	Out[Index] = (float) (sum /  divider);
	
	return Out;
}

/*==========================================================================*/
SCFloatArrayRef HullMovingAverage_S(SCFloatArrayRef In, SCFloatArrayRef Out, SCFloatArrayRef InternalArray1, SCFloatArrayRef InternalArray2, SCFloatArrayRef InternalArray3, int Index, int Length)
{
 
    int HalfLength = Length/2;                           // integer of Length/2

    WeightedMovingAverage_S(In, InternalArray1, Index, HalfLength);
    WeightedMovingAverage_S(In, InternalArray2, Index, Length);

    float wma1 = InternalArray1[Index];
    float wma2 = InternalArray2[Index];
    InternalArray3[Index] = 2*wma1 - wma2;

    int isrp = int(sqrt((double)Length) + 0.5);          // rounded square root of Length
    WeightedMovingAverage_S(InternalArray3, Out, Index, isrp);        // HMA

    return Out;
}

/*==========================================================================*/
SCFloatArrayRef TriangularMovingAverage_S(SCFloatArrayRef In, SCFloatArrayRef Out, SCFloatArrayRef InternalArray1, int Index, int Length)
{
   

    int n0, n1;

    if(Length % 2)
    {
        n0 = n1 = Length/2 + 1;
    }
    else
    {
        n0 = Length/2;
        n1 = n0 + 1;
    }

    SimpleMovAvg_S(In,  InternalArray1, Index, n0);
    SimpleMovAvg_S( InternalArray1, Out, Index, n1);

    return Out;
}

/*==========================================================================*/
SCFloatArrayRef VolumeWeightedMovingAverage_S(SCFloatArrayRef InPrice, SCFloatArrayRef InVolume, SCFloatArrayRef Out, int Index, int Length)
{
  

    float summPV = 0;
    float summV = 0;

    for(int i = max(0, Index - Length + 1); i<=Index; i++)
    {
        summPV += InPrice[i]*InVolume[i];
        summV  += InVolume[i];
    }

    if (summV!=0.0f)
    {
        Out[Index] = summPV/summV;
    }
    else
    {
        Out[Index] = 0;
    }

    return Out;
}
/*==========================================================================*/
void GetStandardDeviation(SCFloatArrayRef in, float& out, int start_indx, int length)
{
	double var = 0;
	
	//if( start_indx < length - 1 )
	//{
	//	start_indx = length - 1;
	//}

	var = GetVariance(in, start_indx, length);

	if(var < 0)
	{
		out = 0;
	}
	else
	{
		out = (float)sqrt(var);	
	}
}

/*==========================================================================*/
SCFloatArrayRef StandardDeviation_S(SCFloatArrayRef In, SCFloatArrayRef Out, int Index, int Length)
{
	
	//if (Index < Length - 1)
		//return Out;
	
	GetStandardDeviation(In, Out[Index], Index, Length);
	
	return Out;
}

/*==========================================================================*/
SCFloatArrayRef Ergodic_S(SCFloatArrayRef In, SCFloatArrayRef Out, int Index, int LongEMALength, int ShortEMALength, float Multiplier,
	SCFloatArrayRef InternalArray1, SCFloatArrayRef InternalArray2, SCFloatArrayRef InternalArray3, SCFloatArrayRef InternalArray4, SCFloatArrayRef InternalArray5, SCFloatArrayRef InternalArray6)
{
	// Formula:
	// Numerator = EMA( EMA(Price - LastPrice, LongEMALength), ShortEMALength)
	// Denominator = EMA( EMA( Abs(Price - LastPrice), LongEMALength), ShortEMALength)
	// TSI = Multiplier * Numerator / Denominator
	
	if (Index < 1)
		return Out;  // Not enough elements
	
	// Internal array names
	SCFloatArrayRef PriceChangeArray = InternalArray1;
	//InternalArray2
	SCFloatArrayRef NumeratorArray = InternalArray3;
	SCFloatArrayRef AbsPriceChangeArray = InternalArray4;
	//InternalArray5
	SCFloatArrayRef DenominatorArray = InternalArray6;
	
	float PriceChange = In[Index] - In[Index - 1];  // Price - LastPrice
	
	// Numerator
	PriceChangeArray[Index] = PriceChange;
	ExponentialMovingAverage_S(PriceChangeArray, InternalArray2, Index, LongEMALength);
	ExponentialMovingAverage_S(InternalArray2, NumeratorArray, Index, ShortEMALength);
	float Numerator = NumeratorArray[Index];
	
	// Denominator
	AbsPriceChangeArray[Index] = abs(PriceChange);
	ExponentialMovingAverage_S(AbsPriceChangeArray, InternalArray5, Index, LongEMALength);
	ExponentialMovingAverage_S(InternalArray5, DenominatorArray, Index, ShortEMALength);
	float Denominator = DenominatorArray[Index];
	
	Out[Index] = Multiplier * Numerator / Denominator;
	
	return Out;
}


/*==========================================================================*/
SCFloatArrayRef Keltner_S(SCBaseDataRef BaseDataIn, SCFloatArrayRef In, SCFloatArrayRef KeltnerAverageOut, SCFloatArrayRef TopBandOut, SCFloatArrayRef BottomBandOut, int Index, int KeltnerMALength, unsigned int KeltnerMAType, int TrueRangeMALength, unsigned int TrueRangeMAType, float TopBandMultiplier, float BottomBandMultiplier, SCFloatArrayRef InternalArray1, SCFloatArrayRef InternalArray2)
{
	MovingAverage_S(In, KeltnerAverageOut, KeltnerMAType, Index, KeltnerMALength);
	
	AverageTrueRange_S(BaseDataIn, InternalArray1, InternalArray2, Index, TrueRangeMALength, TrueRangeMAType);
	
	TopBandOut[Index] = KeltnerAverageOut[Index] + InternalArray2[Index] * TopBandMultiplier;
	BottomBandOut[Index] = KeltnerAverageOut[Index] - InternalArray2[Index] * BottomBandMultiplier;
	
	return KeltnerAverageOut;
}

/*==========================================================================*/
float WellesSum(float In, int Index, int Length, SCFloatArrayRef Out)
{
	if (Index == 0)
	{
		Out[0] = In;
	}
	else
	{
		if (Index < Length)
			Out[Index] = Out[Index - 1] + In;
		else
			Out[Index] = Out[Index - 1] - (Out[Index - 1] / Length) + In;
	}
	
	return Out[Index];
}

/*==========================================================================*/
SCFloatArrayRef WellesSum_S(SCFloatArrayRef In, SCFloatArrayRef Out, int Index, int Length)
{
	WellesSum(In[Index], Index, Length, Out);
	
	return Out;
}

/*============================================================================
	Local function
	This function is used by DMI, ADX, and ADXR.
----------------------------------------------------------------------------*/
void DirectionalMovementTrueRangeSummation(SCBaseDataRef BaseDataIn, int Index, int Length,
	SCFloatArrayRef InternalTrueRangeSummation, SCFloatArrayRef InternalPosDM, SCFloatArrayRef InternalNegDM)
{
	if (Index < 1)
		return;
	
	float HighChange = BaseDataIn[SC_HIGH][Index] - BaseDataIn[SC_HIGH][Index - 1];
	float LowChange = BaseDataIn[SC_LOW][Index - 1] - BaseDataIn[SC_LOW][Index];
	
	{
		float Pos;
		if (HighChange > LowChange && HighChange > 0)
			Pos = HighChange;
		else
			Pos = 0.0f;
		
		WellesSum(Pos, Index, Length, InternalPosDM);
	}
	
	{
		float Neg;
		if (LowChange > HighChange && LowChange > 0)
			Neg = LowChange;
		else
			Neg = 0.0f;
		
		WellesSum(Neg, Index, Length, InternalNegDM);
	}
	
	float TrueRangeValue = TrueRange(BaseDataIn, Index);
	WellesSum(TrueRangeValue, Index, Length, InternalTrueRangeSummation);
}

/*==========================================================================*/
void DMI_S(SCBaseDataRef BaseDataIn,
	int Index,
	int Length, unsigned int DisableRounding,
	SCFloatArrayRef PosDMIOut, SCFloatArrayRef NegDMIOut, SCFloatArrayRef DiffDMIOut,
	SCFloatArrayRef InternalTrueRangeSummation, SCFloatArrayRef InternalPosDM, SCFloatArrayRef InternalNegDM)
{
	DirectionalMovementTrueRangeSummation(BaseDataIn, Index, Length,
		InternalTrueRangeSummation, InternalPosDM, InternalNegDM);
	
	if (InternalTrueRangeSummation[Index] == 0.0f)  // Simple prevention of dividing by zero
	{
		PosDMIOut[Index] = 100.0f;
		NegDMIOut[Index] = 100.0f;
		DiffDMIOut[Index] = 0.0f;
	}
	else
	{
		PosDMIOut[Index] = 100.0f * InternalPosDM[Index] / InternalTrueRangeSummation[Index];
		NegDMIOut[Index] = 100.0f * InternalNegDM[Index] / InternalTrueRangeSummation[Index];
		DiffDMIOut[Index] = fabs(PosDMIOut[Index] - NegDMIOut[Index]);
	}
}

/*============================================================================
	Note: the difference calculation is done on rounded values of DMI+ and
	DMI-.
----------------------------------------------------------------------------*/
SCFloatArrayRef DMIDiff_S(SCBaseDataRef BaseDataIn,
	int Index,
	int Length,
	SCFloatArrayRef Out,
	SCFloatArrayRef InternalTrueRangeSummation, SCFloatArrayRef InternalPosDM, SCFloatArrayRef InternalNegDM)
{
	DirectionalMovementTrueRangeSummation(BaseDataIn, Index, Length,
		InternalTrueRangeSummation, InternalPosDM, InternalNegDM);
	
	if (Index < Length - 1)
		return Out;
	else if (Index == Length - 1)
	{
		Out[Index] = 0.0f;
		return Out;
	}
	
	float PosDMI, NegDMI;
	float TrueRangeSum = InternalTrueRangeSummation[Index];
	if (TrueRangeSum == 0.0f)
	{
		PosDMI = 100.0f;
		NegDMI = 100.0f;
	}
	else
	{
		PosDMI = 100.0f * InternalPosDM[Index] / TrueRangeSum;
		NegDMI = 100.0f * InternalNegDM[Index] / TrueRangeSum;
	}
	
	Out[Index] = PosDMI - NegDMI;
	
	return Out;
}

/*==========================================================================*/
SCFloatArrayRef ADX_S(SCBaseDataRef BaseDataIn,
	int Index,
	int DXLength, int DXMovAvgLength,
	SCFloatArrayRef Out,
	SCFloatArrayRef InternalTrueRangeSummation, SCFloatArrayRef InternalPosDM, SCFloatArrayRef InternalNegDM, SCFloatArrayRef InternalDX)
{
	DirectionalMovementTrueRangeSummation(BaseDataIn, Index, DXLength,
		InternalTrueRangeSummation, InternalPosDM, InternalNegDM);
	
	// Compute +DMI and -DMI identically to how it's done in DMI without rounding
	float PosDMI = 100.0f, NegDMI = 100.0f;
	float TrueRangeSum = InternalTrueRangeSummation[Index];
	if (TrueRangeSum != 0.0f)
	{
		PosDMI *= InternalPosDM[Index] / TrueRangeSum;
		NegDMI *= InternalNegDM[Index] / TrueRangeSum;
	}
	
	if (PosDMI - NegDMI == 0.0f)  // Quick zero (also preventing potential divide by zero)
		InternalDX[Index] = 0.0f;
	else if (PosDMI + NegDMI == 0.0f)  // Simple prevention of dividing by zero
		InternalDX[Index] = 100.0f;
	else
		InternalDX[Index] = 100.0f * fabs(PosDMI - NegDMI) / (PosDMI + NegDMI);
	
	if (Index < DXLength - 1)
		return Out;
	
	if (Index == DXLength + DXMovAvgLength - 1)
	{
		Out[Index] = 0.0f;
		for (int a = 0; a < DXMovAvgLength; ++a)
			Out[Index] += InternalDX[Index - a];
		
		Out[Index] = Out[Index] / (DXLength + DXMovAvgLength);
	}
	else if (Index > DXLength + DXMovAvgLength - 1)
	{
		// Take the average of DX to get ADX
		WildersMovingAverage_S(InternalDX, Out, Index, DXMovAvgLength);
	}
	
	return Out;
}

/*============================================================================
	Note: The InternalADX array does not contain the same values as the
	output of ADX.
----------------------------------------------------------------------------*/
SCFloatArrayRef ADXR_S(SCBaseDataRef BaseDataIn,
	int Index,
	int DXLength, int DXMovAvgLength, int ADXRInterval,
	SCFloatArrayRef Out,
	SCFloatArrayRef InternalTrueRangeSummation, SCFloatArrayRef InternalPosDM, SCFloatArrayRef InternalNegDM, SCFloatArrayRef InternalDX, SCFloatArrayRef InternalADX)
{
	DirectionalMovementTrueRangeSummation(BaseDataIn, Index, DXLength,
		InternalTrueRangeSummation, InternalPosDM, InternalNegDM);
	
	// Compute +DMI and -DMI identically to how it's done in DMI without rounding
	float PosDMI = 100.0f, NegDMI = 100.0f;
	float TrueRangeSum = InternalTrueRangeSummation[Index];
	if (TrueRangeSum != 0.0f)
	{
		PosDMI *= InternalPosDM[Index] / TrueRangeSum;
		NegDMI *= InternalNegDM[Index] / TrueRangeSum;
	}
	
	if (PosDMI - NegDMI == 0.0f)  // Quick zero (also preventing potential divide by zero)
		InternalDX[Index] = 0.0f;
	else if (PosDMI + NegDMI == 0.0f)  // Simple prevention of dividing by zero
		InternalDX[Index] = 100.0f;
	else
		InternalDX[Index] = 100.0f * fabs(PosDMI - NegDMI) / (PosDMI + NegDMI);
	
	if (Index < DXLength - 1)
		return Out;
	
	if (Index == DXLength + DXMovAvgLength - 1)
	{
		InternalADX[Index] = 0.0f;
		for (int a = 0; a < DXMovAvgLength; ++a)
			InternalADX[Index] += InternalDX[Index - a];
		
		InternalADX[Index] = InternalADX[Index] / DXMovAvgLength;
	}
	else if (Index > DXLength + DXMovAvgLength - 1)
	{
		// Take the average of DX to get ADX
		WildersMovingAverage_S(InternalDX, InternalADX, Index, DXMovAvgLength);
	}
	
	if (Index < DXLength + DXMovAvgLength + ADXRInterval - 2)
		return Out;
	
	Out[Index] = (InternalADX[Index] + InternalADX[Index - ADXRInterval + 1]) * 0.5f;
	
	return Out;
}

/*==========================================================================*/
SCFloatArrayRef RSI_S(SCFloatArrayRef In, 
                                   SCFloatArrayRef RsiOut, 
                                   SCFloatArrayRef UpSumsTemp, 
                                   SCFloatArrayRef DownSumsTemp, 
                                   SCFloatArrayRef SmoothedUpSumsTemp, 
                                   SCFloatArrayRef SmoothedDownSumsTemp,
                                   int Index, unsigned int AveragingType, int Length)
{
    if (Length < 1 || Index < 1)
        return RsiOut;

    // calculate Up/Down sums
	float previousValue = In[Index - 1];
	float currentValue = In[Index];

	if (currentValue>previousValue) 
	{
		// upward change
		UpSumsTemp[Index] = currentValue - previousValue;
		DownSumsTemp[Index] = 0;
	}
	else
	{
		UpSumsTemp[Index] = 0;
		DownSumsTemp[Index] = previousValue - currentValue;
	}

	// smooth the up/down sums
    MovingAverage_S(UpSumsTemp, SmoothedUpSumsTemp, AveragingType, Index, Length);
    MovingAverage_S(DownSumsTemp, SmoothedDownSumsTemp, AveragingType, Index, Length);

	// compute RSI
	if (SmoothedDownSumsTemp[Index]!=0.0f)
	{
		RsiOut[Index] = 100.0f - 100.0f / (1 + SmoothedUpSumsTemp[Index] / (SmoothedDownSumsTemp[Index]) );
	}
	else
	{
		RsiOut[Index] = RsiOut[Index - 1];
	}

    return RsiOut;
}

/*==========================================================================*/
SCFloatArrayRef SmoothedMovingAverage_S(SCFloatArrayRef In, SCFloatArrayRef SmoothedAverageOut, int Index, int Length, int Offset)
{
	if (Length < 1 || Offset < 0)
		return SmoothedAverageOut;
	
	int InputIndex = Index - Offset; 
	
	if (Index == 0)
		return SmoothedAverageOut;
	
	// Initialize the prior Smoothed moving average value if it is not set.
	if (SmoothedAverageOut[Index-1] == 0)
		SmoothedAverageOut[Index-1] = GetSummation(In,InputIndex-1 , Length)/Length;



	float PriorSummation = GetSummation(In,InputIndex-1 , Length);

	float newValue = In[InputIndex];


	SmoothedAverageOut[Index] = (PriorSummation -  SmoothedAverageOut[Index - 1] + newValue) / Length;


	return SmoothedAverageOut;
}

/*==========================================================================*/
SCFloatArrayRef MACD_S(SCFloatArrayRef In, SCFloatArrayRef FastMAOut, SCFloatArrayRef SlowMAOut, SCFloatArrayRef MACDOut, SCFloatArrayRef MACDMAOut, SCFloatArrayRef MACDDiffOut, int Index, int FastMALength, int SlowMALength, int MACDMALength, int MovAvgType)
{

	MovingAverage_S(In,FastMAOut,MovAvgType,Index,FastMALength);
	MovingAverage_S(In,SlowMAOut,MovAvgType,Index,SlowMALength);

		
	MACDOut[Index] = FastMAOut[Index] - SlowMAOut[Index];

	if(Index< max(SlowMALength,FastMALength) + MACDMALength)
		return MACDOut;

	MovingAverage_S(MACDOut,MACDMAOut,MovAvgType,Index,MACDMALength);


	MACDDiffOut[Index] = MACDOut[Index] - MACDMAOut[Index];

	return MACDOut;
}

/*==========================================================================*/
SCFloatArrayRef TEMA_S(SCFloatArrayRef In, SCFloatArrayRef Out, SCFloatArrayRef InternalArray1,SCFloatArrayRef InternalArray2,SCFloatArrayRef InternalArray3, int Index, int  Length)
{

	ExponentialMovingAverage_S(In, InternalArray1, Index, Length);
	ExponentialMovingAverage_S(InternalArray1, InternalArray2, Index, Length);
	ExponentialMovingAverage_S(InternalArray2, InternalArray3, Index, Length);

	Out[Index] = 3.0f*InternalArray1[Index] - 3.0f*InternalArray2[Index] + InternalArray3[Index];
	return Out;
}


/*==========================================================================*/
SCFloatArrayRef BollingerBands_S(SCFloatArrayRef In, SCFloatArrayRef Avg, SCFloatArrayRef TopBand,SCFloatArrayRef BottomBand,SCFloatArrayRef StdDev, int Index, int  Length,float Multiplier,int MovAvgType)
{
	if(Length < 1)
	{

		return Avg;
	}

	StandardDeviation_S( In, StdDev,  Index,  Length);
	
	MovingAverage_S( In,  Avg, MovAvgType,  Index,  Length);

	TopBand[Index] = Avg[Index]+StdDev[Index]*Multiplier;
	BottomBand[Index] = Avg[Index]-StdDev[Index]*Multiplier;
	return Avg;
}

/*==========================================================================*/

void Summation(SCFloatArrayRef in, float& out, int start_indx, int length)
{
	float sum=0;
	out = 0;
	if( start_indx < ( length - 1 ) )
	{
      start_indx = length - 1;
    }


	
	if (start_indx >= in.GetArraySize())
		return;

	for(int nn=0;nn<length;nn++)
	{
		sum += in[start_indx-nn];
	}
	out = sum;
}

/*==========================================================================*/
float GetSummation(SCFloatArrayRef In,int Index,int Length)
{
 
float Sum = 0 ;
 
 
for (int i = 0; i < Length; i++)
	Sum = Sum + In[Index - i];
 
return Sum;
}

/*==========================================================================*/
SCFloatArrayRef  AccumulationDistribution_S(SCBaseDataRef BaseDataIn, SCFloatArrayRef Out, int Index)
{
		
	const float BIAS_DIVISION_FLOAT = (float)10e-10;
	SCFloatArrayRef phigh = BaseDataIn[SC_HIGH]; 
	SCFloatArrayRef plow = BaseDataIn[SC_LOW];
	SCFloatArrayRef pvolume = BaseDataIn[SC_VOLUME];
	SCFloatArrayRef pclose = BaseDataIn[SC_LAST];
	
	if (Index == 0)
	{
		Out[Index] = ( (pclose[Index] - plow[Index]) - (phigh[Index] - pclose[Index]) ) / (phigh[Index] - plow[Index] + BIAS_DIVISION_FLOAT) * pvolume[Index];
	}
	else
	{
		Out[Index] = ( (pclose[Index] - plow[Index]) - (phigh[Index] - pclose[Index]) ) / (phigh[Index] - plow[Index] + BIAS_DIVISION_FLOAT) * pvolume[Index] + Out[Index-1];
	}

	return Out;
}
/************************************************************************/


double GetMovingAverage(SCFloatArrayRef In, int start_indx, int length)
{
	double sum= 0;
	double out = 0;
	
	if( start_indx < length - 1 )
	{
		start_indx = length - 1;
	}
	if (start_indx >= In.GetArraySize())
		return 0;

	for(int nn=0;nn < length;nn++)
	{
		sum += (double)In[start_indx-nn];
	}
	out = sum / (double)length;

	return out;
}

/************************************************************
* Method				- Variance
* Description	- Computes the Variance
* shortcut to computing the variance V(x) = E[x^2] -E[x]^2


************************************************************/
double GetVariance(SCFloatArrayRef in, int start_indx, int length)
{	
	double mean1 =0; // E[X^2]
	double mean2 =0; // E[X]^2
	for(int nn=0;nn<length;nn++)
	{ // compute X^2
		if (start_indx-nn < 0)
			break;
		
		mean1 += in[start_indx-nn] * in[start_indx-nn];
	}
	// compute E[X^2]
	mean1 = mean1 / length;

	// compute E[X]^2
	mean2 = GetMovingAverage(in,start_indx, length);
	mean2 *= mean2;

	//Compute V(x)
	return mean1 - mean2;
}

/*==========================================================================*/
SCFloatArrayRef CumulativeSummation_S(SCFloatArrayRef In, SCFloatArrayRef Out, int Index)
{
	if(Index==0)
		Out[Index] = In[Index];
	else
		Out[Index] = Out[Index-1] + In[Index];

	return Out;
}

/*==========================================================================*/
SCFloatArrayRef ArmsEMV_S(SCBaseDataRef BaseDataIn, SCFloatArrayRef Out, int VolumeDivisor, int Index)
{
	int		pos = Index;
	float   pr = 0;               /* price range    */
	float   br = 0;               /* box ratio      */
	float   mpm = 0;              /* midpoint move  */
	float   vol = 0;              /* vol in units of volinc */

	if(Index == 0)
		Out[pos] = 0;
	else if (BaseDataIn[SC_VOLUME][pos] <= 0 || BaseDataIn[SC_HIGH][pos] == BaseDataIn[SC_LOW][pos])
		Out[pos] = 0;
	else
	{
		pr = (BaseDataIn[SC_HIGH][pos] - BaseDataIn[SC_LOW][pos]) * 8;
		vol = BaseDataIn[SC_VOLUME][pos];
		vol /= VolumeDivisor;
		br = vol / pr;
		mpm = ((BaseDataIn[SC_HIGH][pos] + BaseDataIn[SC_LOW][pos]) / 2) -
			((BaseDataIn[SC_HIGH][pos - 1] + BaseDataIn[SC_LOW][pos - 1]) / 2);
		Out[pos] = mpm / br;
	}

	return Out;
}

/*==========================================================================*/
SCFloatArrayRef ChaikinMoneyFlow_S(SCBaseDataRef BaseDataIn, SCFloatArrayRef Out, SCFloatArrayRef InternalArray, int Index, int Length)
{
	SCFloatArrayRef MoneyFlowVolume = InternalArray;
	SCFloatArrayRef CMF = Out;

	float MoneyFlowMultiplier = 0;

	if (BaseDataIn[SC_HIGH][Index] == BaseDataIn[SC_LOW][Index])
	{
		if (Index <= 0 || BaseDataIn[SC_LAST][Index] >= BaseDataIn[SC_LAST][Index - 1])
			MoneyFlowMultiplier = 1.0f;
		else
			MoneyFlowMultiplier = -1.0f;
	}
	else
	{
		MoneyFlowMultiplier	= 
			( (BaseDataIn[SC_LAST][Index] - BaseDataIn[SC_LOW][Index]) - (BaseDataIn[SC_HIGH][Index] - BaseDataIn[SC_LAST][Index]) )
			/ (BaseDataIn[SC_HIGH][Index] - BaseDataIn[SC_LOW][Index]);
	}

	MoneyFlowVolume[Index] = MoneyFlowMultiplier * BaseDataIn[SC_VOLUME][Index];

	if (Index < Length-1)
	{
		CMF[Index] = 0;
		return CMF;
	}
	
	float SumMoneyFlowVolume = 0;
	float SumVolume = 0;

	for(int SumIndex = Index - Length + 1; SumIndex <= Index; SumIndex++)
	{
		SumVolume += BaseDataIn[SC_VOLUME][SumIndex];
		SumMoneyFlowVolume += MoneyFlowVolume[SumIndex];
	}

	if (SumVolume == 0)
		CMF[Index] = 0;
	else
		CMF[Index] = SumMoneyFlowVolume / SumVolume;
	
	return CMF;
}

/*==========================================================================*/
SCFloatArrayRef Summation_S(SCFloatArrayRef In, SCFloatArrayRef Out, int Index, int Length)
{
	float    sum = 0;

	if( Index < Length - 1 )
	{
		Out[Index] = 0;
	}
	else
	{
		for(int pos = Index-Length+1; pos <= Index; pos++)
		{
			sum += In[pos];
		}
	}
	Out[Index] = sum;
	
	return Out;
}

/*==========================================================================*/
SCFloatArrayRef Dispersion_S(SCFloatArrayRef In, SCFloatArrayRef Out, int Index, int Length)
{
	Out[Index] = GetDispersion(In, Index, Length);
	return Out;
}

/*==========================================================================*/
float GetDispersion(SCFloatArrayRef In, int Index, int Length)
{
	if(Index < Length - 1)
		return 0;


	float accum = 0;
	float mean = 0;
	for (int pos = Index - Length + 1; pos <= Index; pos ++ )
	{
		accum += In[pos];
	}
	mean = accum / Length;

	accum = 0;
	float curr = 0;
	for (int pos = Index - Length + 1; pos <= Index; pos ++ )
	{
		curr = ((mean - In[pos]) * (mean - In[pos])) / Length;
		accum += curr;
	}

	return accum;
}

/*==========================================================================*/
SCFloatArrayRef EnvelopePercent_S(SCFloatArrayRef In, SCFloatArrayRef Out1, SCFloatArrayRef Out2,  float Percent, int Index)
{
	Out1[Index] = In[Index] + In[Index] * Percent;
	Out2[Index] = In[Index] - In[Index] * Percent;
	
	return Out1;
}

/*==========================================================================*/
SCFloatArrayRef EnvelopeFixed_S(SCFloatArrayRef In, SCFloatArrayRef Out1, SCFloatArrayRef Out2,  float FixedValue, int Index)
{
	Out1[Index] = In[Index] + FixedValue;
	Out2[Index] = In[Index] - FixedValue;
	
	return Out1;
}


/*==========================================================================*/
SCFloatArrayRef VerticalHorizontalFilter_S(SCFloatArrayRef In, SCFloatArrayRef Out, int Index, int Length)
{
      
	float    Sum = 0;       
	float    HighestValue;         
	float    LowestValue;         
							

	for (int pos = Index - Length + 1; pos <= Index; pos++)
	{
		Sum += fabs((In[pos] - In[pos - 1]));
	}

	LowestValue = Lowest(In,Index,Length);
	HighestValue = Highest(In,Index,Length);
	
	Out[Index] = (HighestValue - LowestValue) / Sum;

	return Out;
}

/*==========================================================================*/
SCFloatArrayRef RWI_S(SCBaseDataRef BaseDataIn, SCFloatArrayRef Out1, SCFloatArrayRef Out2,  SCFloatArrayRef TrueRangeArray, SCFloatArrayRef LookBackLowArray, SCFloatArrayRef LookBackHighArray, int Index, int Length) 
{
	int   Index2 = 0;
	float  sum=0, avg = 0;
       

	if(Index < Length)
	{
		Out1[Index] = 0;
		Out2[Index] = 0;
	}
	else
	{
		TrueRangeArray[Index-Length] = BaseDataIn[SC_HIGH][Index-Length] - BaseDataIn[SC_LOW][Index-Length];
		for(int i= Index - Length + 1; i < Index;i++)
		{
			TrueRangeArray[i] = max(BaseDataIn[SC_HIGH][i] - BaseDataIn[SC_LOW][i],
			BaseDataIn[SC_LAST][i - 1] - BaseDataIn[SC_LOW][i]);
		}

		for (Index2 = 0; Index2 < Length - 1; Index2++)
		{
			sum=0; avg=0;
			
			for(int i = Index - Index2 - 2; i < Index; i++)
				sum += TrueRangeArray[i];

			avg = sum / (Index2 + 2);

			if (avg != 0)
			{
				LookBackLowArray[Index2] = (BaseDataIn[SC_HIGH][Index - Index2 - 1] - BaseDataIn[SC_LOW][Index]) /
					(avg * sqrt((Index2 + 2.0f)));
				LookBackHighArray[Index2] = (BaseDataIn[SC_HIGH][Index] - BaseDataIn[SC_LOW][Index - Index2 - 1]) /
					(avg * sqrt((Index2 + 2.0f)));
			}
		}

		Out2[Index] = Highest(LookBackLowArray,Length-1,Length);
		Out1[Index] = Highest(LookBackHighArray,Length-1,Length);
	}

	return Out1;
}

/*==========================================================================*/
SCFloatArrayRef UltimateOscillator_S(SCBaseDataRef BaseDataIn, 
									 SCFloatArrayRef Out, 
									 SCFloatArrayRef CalcE,
									 SCFloatArrayRef CalcF,
									 SCFloatArrayRef CalcG,
									 SCFloatArrayRef CalcH,
									 SCFloatArrayRef CalcI,
									 SCFloatArrayRef CalcJ,
									 SCFloatArrayRef CalcK,
									 SCFloatArrayRef CalcL,
									 SCFloatArrayRef CalcM,
									 SCFloatArrayRef CalcN,
									 SCFloatArrayRef CalcO,
									 SCFloatArrayRef CalcP,
									 SCFloatArrayRef CalcQ,

									 int Index, int Length1, int Length2, int Length3)//7,14,28
{
/*	
-Column E calculates the True High, which is the greater of Today's High and Yesterday's Close.								
-		Column F calculates the True Low, which is the lesser of Today's Low and Yesterday's Close.								
-		Column G subtracts the True Low (column F) from the True High (column E) to get the Range (also called Total Activity).								
-		Column H is a 7-day summation of the Range (column G).								
-		Column I subtracts the True Low (column F) from the Close, to get the Buying Units (also called Buying Pressure).								
-		Column J is a 7-day summation of the Buying Units (column I).								
-		Column K divides Buying Units (column J) by Range (column H).								
-		Column L is a 14-day summation of the Range (column G).								
-		Column M is a 14-day summation of the Buying Units (column I).								
-		Column N divides Buying Units (column M) by Range (column L).								
-		Column O is a 28-day summation of the Range (column G).								
-		Column P is a 28-day summation of the Buying Units (column I).								
-		Column Q divides Buying Units (column P) by Range (column O)								
		Column R is the Ultimate Oscillator, which is four times the 7-day division (column K) + 2 times the 14-day division (column N) plus the 28-day division (column Q), divided by 7 and multiplied by 100. Formula: =((4*K40)+(2*N40)+Q40)/7*100	*/							
	CalcE[Index] = GetTrueHigh(BaseDataIn,Index);
	CalcF[Index] = GetTrueLow(BaseDataIn,Index);
	CalcG[Index] = CalcE[Index]  - CalcF[Index] ;
	Summation_S(CalcG,CalcH,Index,Length1);
	CalcI[Index] = BaseDataIn[SC_LAST][Index] - CalcF[Index];
	Summation_S(CalcI,CalcJ,Index,Length1);
	CalcK[Index] = CalcJ[Index] / CalcH[Index];
	Summation_S(CalcG,CalcL, Index, Length2);
	Summation_S(CalcI,CalcM, Index, Length2);
	CalcN[Index] = CalcM[Index] / CalcL[Index];
	Summation_S(CalcG,CalcO, Index, Length3);
	Summation_S(CalcI,CalcP, Index, Length3);
	CalcQ[Index] = CalcP[Index] / CalcO[Index];

	Out[Index] = ((4*CalcK[Index])+(2*CalcN[Index])+CalcQ[Index])/7*100;


	return Out;
}

/*==========================================================================*/
SCFloatArrayRef WilliamsAD_S(SCBaseDataRef BaseDataIn, SCFloatArrayRef Out, int Index)
{
	if(Index > 0)
	{
		if (BaseDataIn[SC_LAST][Index] > BaseDataIn[SC_LAST][Index - 1])
			Out[Index] = Out[Index - 1] + (BaseDataIn[SC_LAST][Index] - min(BaseDataIn[SC_LOW][Index], BaseDataIn[SC_LAST][Index - 1]));

		else if (BaseDataIn[SC_LAST][Index] == BaseDataIn[SC_LAST][Index - 1])
			Out[Index] = Out[Index - 1];

		else if (BaseDataIn[SC_LAST][Index] < BaseDataIn[SC_LAST][Index - 1])
			Out[Index] = Out[Index - 1] +
			( BaseDataIn[SC_LAST][Index] - max(BaseDataIn[SC_HIGH][Index], BaseDataIn[SC_LAST][Index - 1]) );
	}
	else
		Out[Index] = 0;
	
	return Out;
}



/*==========================================================================*/
SCFloatArrayRef WilliamsR_S(SCBaseDataRef BaseDataIn, SCFloatArrayRef Out, int Index, int Length)
{
	return WilliamsR2_S(BaseDataIn[SC_HIGH], BaseDataIn[SC_LOW], BaseDataIn[SC_LAST], Out, Index, Length);
}
/*==========================================================================*/

SCFloatArrayRef WilliamsR2_S(SCFloatArrayRef InputDataHigh, SCFloatArrayRef InputDataLow, SCFloatArrayRef InputDataLast, SCFloatArrayRef Out, int Index, int Length)
{
	if(Index < Length)
		Out[Index]=0;
	else
	{
		float High = Highest(InputDataHigh,Index, Length); 
		float Low = Lowest(InputDataLow,Index,Length);
		Out[Index] = 100 * ( High-InputDataLast[Index]) / (High - Low);
	}

	return Out;
}

/*==========================================================================*/
float GetArrayValueAtNthOccurrence(SCFloatArrayRef TrueFalseIn, SCFloatArrayRef ValueArrayIn, int Index, int NthOccurrence )
{
	int Occurrence = 0;
	int CurrentIndex = Index;

	while(Occurrence < NthOccurrence)
	{
		if(TrueFalseIn[CurrentIndex] != 0)
			Occurrence++;
	
		CurrentIndex--;

		if(CurrentIndex < 0)
			break;
	}

	if((CurrentIndex < 0) && (Occurrence < NthOccurrence))
		return 0;

	return ValueArrayIn[CurrentIndex+1];
}

/*==========================================================================*/
SCFloatArrayRef Parabolic_S(s_Parabolic& ParabolicData)
{
	SCBaseDataRef BaseDataIn = ParabolicData._BaseDataIn;
	int BaseDataIndex = ParabolicData._Index;
	SCSubgraphRef Out = ParabolicData._Out;
	SCDateTimeArrayRef BaseDateTimeIn = ParabolicData._BaseDateTimeIn;
	float InStartAccelFactor = ParabolicData._InStartAccelFactor;
	float InAccelIncrement = ParabolicData._InAccelIncrement;
	float InMaxAccelFactor = ParabolicData._InMaxAccelFactor;
	unsigned int InAdjustForGap = ParabolicData._InAdjustForGap;

	SCFloatArrayRef AccelerationFactor = Out.Arrays[0];
	SCFloatArrayRef ExtremeHighOrLowDuringTrend = Out.Arrays[1];
	SCFloatArrayRef InitialCalculationsLow = Out.Arrays[2];
	SCFloatArrayRef InitialCalculationsHigh = Out.Arrays[3];
	SCFloatArrayRef CurrentParabolicDirection = Out.Arrays[4];

	const int PARABOLIC_LONG = 2;
	const int PARABOLIC_SHORT= 1;

	if (BaseDataIndex < 1)//Reset
	{
		CurrentParabolicDirection[0] = 0;
		AccelerationFactor[0] = 0;
		ExtremeHighOrLowDuringTrend[0] = 0;

		InitialCalculationsLow[0] = ParabolicData.BaseDataLow(0);
		InitialCalculationsHigh[0] = ParabolicData.BaseDataHigh(0);

		return Out;
	}

	//Carry forward persistent values
	ExtremeHighOrLowDuringTrend[BaseDataIndex] = ExtremeHighOrLowDuringTrend[BaseDataIndex - 1];
	AccelerationFactor[BaseDataIndex] = AccelerationFactor[BaseDataIndex - 1];

	InitialCalculationsLow[BaseDataIndex] = InitialCalculationsLow[BaseDataIndex - 1];
	InitialCalculationsHigh[BaseDataIndex] = InitialCalculationsHigh[BaseDataIndex - 1];

	if (CurrentParabolicDirection[BaseDataIndex - 1] == 0) // The CurrentParabolicDirection is unknown.
	{
		AccelerationFactor[BaseDataIndex] = InStartAccelFactor;

		if (InitialCalculationsLow[BaseDataIndex ] > ParabolicData.BaseDataLow(BaseDataIndex))
			InitialCalculationsLow[BaseDataIndex] = ParabolicData.BaseDataLow(BaseDataIndex);

		if (InitialCalculationsHigh[BaseDataIndex] < ParabolicData.BaseDataHigh(BaseDataIndex))
			InitialCalculationsHigh[BaseDataIndex] = ParabolicData.BaseDataHigh(BaseDataIndex);

		if (ParabolicData.BaseDataHigh(BaseDataIndex) > ParabolicData.BaseDataHigh(BaseDataIndex-1) && ParabolicData.BaseDataLow(BaseDataIndex) > ParabolicData.BaseDataLow(BaseDataIndex-1))
		{
			ExtremeHighOrLowDuringTrend[BaseDataIndex] = ParabolicData.BaseDataHigh(BaseDataIndex);
			Out[BaseDataIndex] = InitialCalculationsLow[BaseDataIndex];
			CurrentParabolicDirection[BaseDataIndex] = PARABOLIC_LONG;
			//LastPriceAtReversalIndex[BaseDataIndex] = BaseDataIn[SC_LAST][BaseDataIndex];

			return Out;
		}

		if (ParabolicData.BaseDataHigh(BaseDataIndex) < ParabolicData.BaseDataHigh(BaseDataIndex-1) && ParabolicData.BaseDataLow(BaseDataIndex) < ParabolicData.BaseDataLow(BaseDataIndex-1))
		{
			ExtremeHighOrLowDuringTrend[BaseDataIndex] = ParabolicData.BaseDataLow(BaseDataIndex);
			Out[BaseDataIndex] =InitialCalculationsHigh[BaseDataIndex];
			//LastPriceAtReversalIndex[BaseDataIndex] = BaseDataIn[SC_LAST][BaseDataIndex];
			CurrentParabolicDirection[BaseDataIndex] = PARABOLIC_SHORT;

			return Out;
		}
	}
	else
	{
		if (CurrentParabolicDirection[BaseDataIndex -1] == PARABOLIC_LONG) 
		{
			if (ExtremeHighOrLowDuringTrend[BaseDataIndex - 1] < ParabolicData.BaseDataHigh(BaseDataIndex-1))
			{
				ExtremeHighOrLowDuringTrend[BaseDataIndex] = ParabolicData.BaseDataHigh(BaseDataIndex-1);

				AccelerationFactor[BaseDataIndex] = AccelerationFactor[BaseDataIndex -1] + InAccelIncrement;
			}

			CurrentParabolicDirection[BaseDataIndex] = CurrentParabolicDirection[BaseDataIndex - 1];

		}
		else if (CurrentParabolicDirection[BaseDataIndex - 1] == PARABOLIC_SHORT) 
		{
			if (ExtremeHighOrLowDuringTrend[BaseDataIndex - 1] > ParabolicData.BaseDataLow(BaseDataIndex-1))
			{
				ExtremeHighOrLowDuringTrend[BaseDataIndex] = ParabolicData.BaseDataLow(BaseDataIndex-1);

				AccelerationFactor[BaseDataIndex] = AccelerationFactor[BaseDataIndex] + InAccelIncrement;
			}

			CurrentParabolicDirection[BaseDataIndex] = CurrentParabolicDirection[BaseDataIndex - 1];

		}

		if (AccelerationFactor[BaseDataIndex] > InMaxAccelFactor)
			AccelerationFactor[BaseDataIndex] = InMaxAccelFactor;


		float CalcResult= AccelerationFactor[BaseDataIndex] * (ExtremeHighOrLowDuringTrend[BaseDataIndex] - Out[BaseDataIndex - 1]);

		Out[BaseDataIndex] = Out[BaseDataIndex - 1] +  CalcResult;

		float Gap = 0.0;
		float PreviousTrueHigh = 0;
		if ((BaseDataIndex < 2) || (ParabolicData.BaseDataHigh(BaseDataIndex-1) >= ParabolicData.BaseDataLow(BaseDataIndex-2)))
			PreviousTrueHigh = ParabolicData.BaseDataHigh(BaseDataIndex-1);
		else
			PreviousTrueHigh = ParabolicData.BaseDataLow(BaseDataIndex-2);

		if (InAdjustForGap
			&& BaseDataIn[SC_OPEN][BaseDataIndex] > PreviousTrueHigh
			&& BaseDateTimeIn.DateAt(BaseDataIndex) != BaseDateTimeIn.DateAt(BaseDataIndex - 1)
			)
		{
			Gap = (BaseDataIn[SC_OPEN][BaseDataIndex] - BaseDataIn[SC_LAST][BaseDataIndex - 1]);
			Out[BaseDataIndex] = Out[BaseDataIndex - 1] + Gap;
			ExtremeHighOrLowDuringTrend[BaseDataIndex] = ExtremeHighOrLowDuringTrend[BaseDataIndex - 1] + Gap;
		}

		float PreviousTrueLow = 0;
		if ((BaseDataIndex < 2) || (ParabolicData.BaseDataLow(BaseDataIndex-1) <= ParabolicData.BaseDataHigh(BaseDataIndex-2)))
			PreviousTrueLow = ParabolicData.BaseDataLow(BaseDataIndex-1);
		else 
			PreviousTrueLow = ParabolicData.BaseDataHigh(BaseDataIndex-2);

		if (InAdjustForGap
			&& BaseDataIn[SC_OPEN][BaseDataIndex] < PreviousTrueLow
			&& BaseDateTimeIn.DateAt(BaseDataIndex) != BaseDateTimeIn.DateAt(BaseDataIndex - 1)
			)
		{
			Gap = BaseDataIn[SC_LAST][BaseDataIndex - 1] - BaseDataIn[SC_OPEN][BaseDataIndex];
			Out[BaseDataIndex] = Out[BaseDataIndex - 1] - Gap;

			ExtremeHighOrLowDuringTrend[BaseDataIndex] = ExtremeHighOrLowDuringTrend[BaseDataIndex - 1] - Gap;
		}


		if (Gap == 0.0f && CurrentParabolicDirection[BaseDataIndex] == PARABOLIC_LONG
			&&  (Out[BaseDataIndex] > ParabolicData.BaseDataLow(BaseDataIndex-1) ||  Out[BaseDataIndex] > ParabolicData.BaseDataLow(BaseDataIndex-2))
			)
			Out[BaseDataIndex] = min(ParabolicData.BaseDataLow(BaseDataIndex-1), ParabolicData.BaseDataLow(BaseDataIndex-2));

		else if (Gap == 0.0f && CurrentParabolicDirection[BaseDataIndex] == PARABOLIC_SHORT 
			&& (Out[BaseDataIndex] < ParabolicData.BaseDataHigh(BaseDataIndex-1) || Out[BaseDataIndex] < ParabolicData.BaseDataHigh(BaseDataIndex-2))
		)
			Out[BaseDataIndex] = max(ParabolicData.BaseDataHigh(BaseDataIndex-1), ParabolicData.BaseDataHigh(BaseDataIndex-2));

		//Check to see if we need to reverse

		if (CurrentParabolicDirection[BaseDataIndex] == PARABOLIC_LONG &&
			Out[BaseDataIndex] >= ParabolicData.BaseDataLow(BaseDataIndex))
		{
			CurrentParabolicDirection[BaseDataIndex] = PARABOLIC_SHORT;

			Out[BaseDataIndex] = ExtremeHighOrLowDuringTrend[BaseDataIndex];


			AccelerationFactor[BaseDataIndex] = InStartAccelFactor;
			ExtremeHighOrLowDuringTrend[BaseDataIndex] = ParabolicData.BaseDataLow(BaseDataIndex);

			return Out;
		}
		else if (CurrentParabolicDirection[BaseDataIndex] == PARABOLIC_SHORT &&
			Out[BaseDataIndex] <= ParabolicData.BaseDataHigh(BaseDataIndex))
		{
			CurrentParabolicDirection[BaseDataIndex] = PARABOLIC_LONG;


			Out[BaseDataIndex] = ExtremeHighOrLowDuringTrend[BaseDataIndex];

			AccelerationFactor[BaseDataIndex] = InStartAccelFactor;
			ExtremeHighOrLowDuringTrend[BaseDataIndex] = ParabolicData.BaseDataHigh(BaseDataIndex);

			return Out;
		}
	}

	return Out;
}
/*==========================================================================*/
int GetIslandReversal_S(SCBaseDataRef BaseDataIn, int Index)
{

	if (Index == 0)
		return 0;

	float Range =	(BaseDataIn[SC_HIGH][Index] - BaseDataIn[SC_LOW][Index]) ;

	if (
		BaseDataIn[SC_HIGH][Index] < BaseDataIn[SC_LOW][Index - 1]	
		&&
		(BaseDataIn[SC_LAST][Index] > (BaseDataIn[SC_LOW][Index] +
		(Range * .70f)))
		 )
		return 1;

	if (
		BaseDataIn[SC_LOW][Index] > BaseDataIn[SC_HIGH][Index - 1]
		&& 
		(BaseDataIn[SC_LAST][Index] < (BaseDataIn[SC_HIGH][Index] - 
		(Range* .70f)))
		)
		return -1;

	return 0;
}

/*==========================================================================*/
SCFloatArrayRef Oscillator_S(SCFloatArrayRef In1, SCFloatArrayRef In2, SCFloatArrayRef Out, int Index)
{
	Out[Index] = In1[Index] - In2[Index];
	
	return Out;
}

/*==========================================================================*/
float GetTrueHigh(SCBaseDataRef BaseDataIn, int Index)
{
	if (Index == 0 || BaseDataIn[SC_HIGH][Index] > BaseDataIn[SC_LAST][Index - 1])
		return (BaseDataIn[SC_HIGH][Index]);
	else
		return (BaseDataIn[SC_LAST][Index - 1]);
}

/*==========================================================================*/
float GetTrueLow(SCBaseDataRef BaseDataIn, int Index)
{
	if (Index == 0 || BaseDataIn[SC_LOW][Index] < BaseDataIn[SC_LAST][Index - 1])
		return (BaseDataIn[SC_LOW][Index]);
	else
		return (BaseDataIn[SC_LAST][Index - 1]);
}

/*==========================================================================*/
float GetTrueRange(SCBaseDataRef BaseDataIn, int Index)
{
	return (GetTrueHigh(BaseDataIn, Index) - GetTrueLow(BaseDataIn, Index));
}

/*==========================================================================*/


float GetCorrelationCoefficient(SCFloatArrayRef In1, SCFloatArrayRef In2, int Index, int Length)
{
	//return 1;
	//(const ap::real_1d_array& x,	const ap::real_1d_array& y,		int n)
	float result;
	int i;
	float xmean;
	float ymean;
	float s;
	float xv;
	float yv;
	float t1;
	float t2;

	xv = 0;
	yv = 0;
	if( Length<=1 )
	{
		result = 0;
		return result;
	}

	//
	// Mean
	//
	xmean = 0;
	ymean = 0;
	for(i = 0; i < Length; i++)
	{
		xmean += In1[Index-i];
		ymean += In2[Index-i];
	}
	xmean = xmean/Length;
	ymean = ymean/Length;

	//
	// numerator and denominator
	//
	s = 0;
	xv = 0;
	yv = 0;
	for(i = 0; i <= Length-1; i++)
	{
		t1 = In1[Index-i]-xmean;
		t2 = In2[Index-i]-ymean;
		xv = xv+(t1*t1);
		yv = yv+(t2*t2);
		s = s+t1*t2;
	}
	if( xv==0||yv==0 )
	{
		result = 0;
	}
	else
	{
		result = s/(sqrt(xv)*sqrt(yv));
	}
	return result;
}

/*==========================================================================*/
int NumberOfBarsSinceHighestValue(SCFloatArrayRef In, int Index, int Length)
{
	float max = In[Index];
	int maxIndex = Index;
	for(int i = Index; i > Index - Length && i >= 0; i--)
	{
		if(In[i] > max)
		{
			max = In[i];
			maxIndex = i;
		}
	}
	return Index - maxIndex;
}

/*==========================================================================*/
int NumberOfBarsSinceLowestValue(SCFloatArrayRef In, int Index, int Length)
{
	float min = In[Index];
	int minIndex = Index;

	for(int i = Index; i > Index - Length && i >= 0; i--)
	{
		if(In[i] < min)
		{
			min = In[i];
			minIndex = i;
		}
	}
	return Index - minIndex;
}

/*==========================================================================*/
SCFloatArrayRef PriceVolumeTrend_S(SCBaseDataRef BaseDataIn, SCFloatArrayRef Out, int Index)
{
	if(Index==0)
	{
		Out[Index] = 0;
		return Out;
	}
	
	Out[Index] = ( (BaseDataIn[SC_LAST][Index] - BaseDataIn[SC_LAST][Index-1]) / BaseDataIn[SC_LAST][Index-1] )
		* BaseDataIn[SC_VOLUME][Index] + Out[Index-1];
	
	return Out;
}

/*==========================================================================*/
SCFloatArrayRef Momentum_S(SCFloatArrayRef In, SCFloatArrayRef Out, int Index, int Length)
{
	Out[Index] = (In[Index]/In[Index-Length])*100;
	return Out;
}

/*==========================================================================*/
SCFloatArrayRef TRIX_S(SCFloatArrayRef In, SCFloatArrayRef Out, SCFloatArrayRef InternalEma_1,  SCFloatArrayRef InternalEma_2, SCFloatArrayRef InternalEma_3, int Index, int Length)
{
	const double BIAS_DIVISION_DOUBLE = (double)10e-20;

	ExponentialMovingAverage_S(In,			InternalEma_1, Index, Length);
	ExponentialMovingAverage_S(InternalEma_1, InternalEma_2, Index, Length);
	ExponentialMovingAverage_S(InternalEma_2, InternalEma_3, Index, Length);

	Out[Index] = (float)(100 * (InternalEma_3[Index] - InternalEma_3[Index-1]) / (InternalEma_3[Index-1] + BIAS_DIVISION_DOUBLE));

	return Out;
}

/*==========================================================================*/
SCFloatArrayRef Demarker_S(SCBaseDataRef BaseDataIn, SCFloatArrayRef Out, SCFloatArrayRef DemMax, SCFloatArrayRef DemMin, SCFloatArrayRef SmaDemMax, SCFloatArrayRef SmaDemMin, int Index, int Length)
{
	if(Index > 0)
	{
		float high = BaseDataIn[SC_HIGH][Index];
		float highOld = BaseDataIn[SC_HIGH][Index-1];

		if ( high > highOld )
		{
			DemMax[Index] = high - highOld;
		}
		else
		{
			DemMax[Index] = 0.0f;
		}

		float low = BaseDataIn[SC_LOW][Index];
		float lowOld = BaseDataIn[SC_LOW][Index-1];

		if ( low < lowOld )
		{
			DemMin[Index] = lowOld - low;
		}
		else
		{
			DemMin[Index] = 0.0f;
		}
	}
	else
	{
		DemMax[Index] = 0.0f;
		DemMin[Index] = 0.0f;
	}

	SimpleMovAvg_S(DemMax, SmaDemMax, Index, Length);
	SimpleMovAvg_S(DemMin, SmaDemMin, Index, Length);

	float summ = SmaDemMax[Index] + SmaDemMin[Index];
	if (summ != 0.0f)
	{        
		Out[Index] = SmaDemMax[Index] / summ;
	}
	else
	{
		Out[Index] = Out[Index-1];
	}

	return Out;
}

/*==========================================================================*/
SCFloatArrayRef Old_AroonIndicator_S(SCFloatArrayRef In, SCFloatArrayRef OutUp, SCFloatArrayRef OutDown, int Index, int Length)
{
	int BarsSinceHighest = 0;
	float Highest = In[Index];

	int BarsSinceLowest = 0;
	float Lowest = In[Index];

	for (int i = 1; i <= Length ; i++)
	{
		if (In[Index - i] > Highest)
		{
			Highest = In[Index - i];
			BarsSinceHighest = i;
		}
		if (In[Index - i] < Lowest)
		{
			Lowest = In[Index - i];
			BarsSinceLowest = i;
		}
	}

	OutUp[Index] = ((Length - (float)BarsSinceHighest) / Length) * 100.0f;
	OutDown[Index] = ((Length - (float)BarsSinceLowest) / Length) * 100.0f;

	return OutUp;
}

/*==========================================================================*/
SCFloatArrayRef AroonIndicator_S(SCFloatArrayRef FloatArrayInHigh, SCFloatArrayRef FloatArrayInLow, SCFloatArrayRef OutUp, SCFloatArrayRef OutDown, SCFloatArrayRef OutOscillator, int Index, int Length)
{
	int BarsSinceHighest = 0;
	float Highest = FloatArrayInHigh[Index];

	int BarsSinceLowest = 0;
	float Lowest =FloatArrayInLow[Index];

	for (int i = 1; i <= Length ; i++)
	{
		if (FloatArrayInHigh[Index - i] > Highest)
		{
			Highest = FloatArrayInHigh[Index - i];
			BarsSinceHighest = i;
		}

		if (FloatArrayInLow[Index - i] < Lowest)
		{
			Lowest = FloatArrayInLow[Index - i];
			BarsSinceLowest = i;
		}
	}

	OutUp[Index] = ((Length - (float)BarsSinceHighest) / Length) * 100.0f;
	OutDown[Index] = ((Length - (float)BarsSinceLowest) / Length) * 100.0f;


	OutOscillator [Index] =OutUp[Index] -OutDown[Index];

	return OutUp;
}
/*==========================================================================*/
int IsSwingHighAllowEqual_S(SCStudyInterfaceRef sc, int AllowEqual, int Index, int Length)
{
	for(int i = 1; i <= Length; i++)
	{

		if (AllowEqual)
		{
			if (sc.FormattedEvaluate(sc.BaseData [SC_HIGH][Index] , sc.BasedOnGraphValueFormat, LESS_OPERATOR, sc.BaseData [SC_HIGH][Index-i], sc.BasedOnGraphValueFormat)
				||
				sc.FormattedEvaluate(sc.BaseData [SC_HIGH][Index] , sc.BasedOnGraphValueFormat, LESS_OPERATOR, sc.BaseData [SC_HIGH][Index+i], sc.BasedOnGraphValueFormat)
				)
				return 0;


		}
		else
		{
			if (sc.FormattedEvaluate(sc.BaseData [SC_HIGH][Index] , sc.BasedOnGraphValueFormat, LESS_EQUAL_OPERATOR, sc.BaseData [SC_HIGH][Index-i], sc.BasedOnGraphValueFormat)
				||
				sc.FormattedEvaluate(sc.BaseData [SC_HIGH][Index] , sc.BasedOnGraphValueFormat, LESS_EQUAL_OPERATOR, sc.BaseData [SC_HIGH][Index+i], sc.BasedOnGraphValueFormat)
				)
				return 0;


		}
	}

	return 1;
}

/*==========================================================================*/
int IsSwingLowAllowEqual_S(SCStudyInterfaceRef sc, int AllowEqual, int Index, int Length)
{
	for(int i = 1; i <= Length; i++)
	{

		if (AllowEqual)
		{
			if (sc.FormattedEvaluate(sc.BaseData [SC_LOW][Index] , sc.BasedOnGraphValueFormat, GREATER_OPERATOR, sc.BaseData [SC_LOW][Index-i], sc.BasedOnGraphValueFormat)
				||
				sc.FormattedEvaluate(sc.BaseData [SC_LOW][Index] , sc.BasedOnGraphValueFormat, GREATER_OPERATOR, sc.BaseData [SC_LOW][Index+i], sc.BasedOnGraphValueFormat)
				)
				return 0;


		}
		else
		{
			if (sc.FormattedEvaluate(sc.BaseData [SC_LOW][Index] , sc.BasedOnGraphValueFormat, GREATER_EQUAL_OPERATOR, sc.BaseData [SC_LOW][Index-i], sc.BasedOnGraphValueFormat)
				||
				sc.FormattedEvaluate(sc.BaseData [SC_LOW][Index] , sc.BasedOnGraphValueFormat, GREATER_EQUAL_OPERATOR, sc.BaseData [SC_LOW][Index+i], sc.BasedOnGraphValueFormat)
				)
				return 0;


		}
	}

	return 1;
}

/*==========================================================================*/
SCFloatArrayRef AwesomeOscillator_S(SCFloatArrayRef In, SCFloatArrayRef Out, SCFloatArrayRef TempMA1, SCFloatArrayRef TempMA2, int Index, int Length1, int Length2)
{
	SimpleMovAvg_S(In, TempMA1, Index, Length1);
	SimpleMovAvg_S(In, TempMA2, Index, Length2);

	Out[Index] = TempMA2[Index] - TempMA1[Index];

	return Out;
}

/*==========================================================================*/
int CalculatePivotPoints
(  float Open
 , float High
 , float Low
 , float Close
 , float NextOpen
 , float& PivotPoint
 , float& R_5
 , float& R1, float& R1_5
 , float& R2, float& R2_5
 , float& R3
 , float& S_5
 , float& S1, float& S1_5
 , float& S2, float& S2_5
 , float& S3
 , float &R3_5
 , float &S3_5
 , float& R4
 , float& S4
 , float& R5
 , float& S5
 , int FormulaType

 )
{

	if(  FormulaType == 0 )
	{
		PivotPoint = (High + Low + Close) / 3;
		R1 = (PivotPoint * 2) - Low;
		R_5 = (PivotPoint + R1) / 2;
		R2 = PivotPoint + (High - Low);
		R1_5 = (R1 + R2) / 2;
		S1 = (2 * PivotPoint) - High;
		S_5 = (PivotPoint + S1) / 2;
		S2 = PivotPoint - (High - Low);
		S1_5 = (S1 + S2) / 2;
		R3 = 2 * PivotPoint + (High - 2*Low); 
		R2_5 = (R2 + R3) / 2;
		S3 = 2 * PivotPoint - (2 * High - Low);
		S2_5 = (S2 + S3) / 2;
		R4 = 3 * PivotPoint + (High - 3 * Low);
		S4 = 3 * PivotPoint - (3 * High - Low);
		R3_5 =  (R3 + R4) / 2;
		S3_5 = (S3 + S4) / 2;
		R5 = 4 * PivotPoint + (High - 4 * Low);
		S5 = 4 * PivotPoint - (4 * High - Low);

		/*

		R3.5 = (R3 + R4)/2
		S3.5 = (S3 + S4)/2
		R5 = 4 * PivotPoint + (Yesterday's High - (4 * Yesterday's Low))
		S5 = (4 * PivotPoint) - ((4 * Yesterday's High) - Yesterday's Low)
		R4.5 = (R4 + R5)/2
		S4.5 = (S4 + S5)/2
		*/
	}

	else if( FormulaType == 1 )
	{
		float YesterdaysRange;

		PivotPoint = (High + Low + Close) / 3;
		R1 = (PivotPoint * 2) - Low;
		R_5 = (PivotPoint + R1) / 2;
		R2 = PivotPoint + (High-Low);
		R1_5 = (R1 + R2) / 2;
		S1 = (2 * PivotPoint) - High;
		S_5 = (PivotPoint + S1) / 2;
		S2 = PivotPoint - (High - Low);
		S1_5 = (S1 + S2) / 2;
		YesterdaysRange = (High - Low);

		// This is R3 Pattern Trapper
		R3 = High + YesterdaysRange; // 2 * High - Low
		R2_5 = (R2 + R3)/2;

		// This is S3 Pattern Trapper
		S3 = 2*PivotPoint - High - YesterdaysRange; // 2 * PivotPoint - (2 * High + Low)
		S2_5 = (S2 + S3) / 2;

		R4 = PivotPoint + 3 * (High - Low);
		S4 = PivotPoint - 3 * (High - Low);
	}
	else if(  FormulaType == 2 )
	{
		PivotPoint = (NextOpen + High + Low + Close) / 4;
		R1 = (PivotPoint * 2) - Low;
		R_5 = (PivotPoint + R1) / 2;
		R2 = PivotPoint + (High - Low);
		R1_5 = (R1 + R2) / 2;
		S1 = (2 * PivotPoint) - High;
		S_5 = (PivotPoint + S1) / 2;
		S2 = PivotPoint - (High - Low);
		S1_5 = (S1 + S2) / 2;
		R3 = 2*PivotPoint + (High - 2*Low); // R3 std
		R2_5 = (R2 + R3) / 2;
		S3 = 2*PivotPoint - (2*High - Low); // S3 std
		S2_5 = (S2 + S3) / 2;
		R4 = 3 * PivotPoint + (High - 3 * Low);
		S4 = 3 * PivotPoint - (3 * High - Low);
	}
	else if(FormulaType == 3)
	{
		PivotPoint = (High + Low + NextOpen+NextOpen) / 4;
		R1 = (PivotPoint * 2) - Low;
		R_5 = (PivotPoint + R1) / 2;
		R2 = PivotPoint + (High - Low);
		R1_5 = (R1 + R2) / 2;
		S1 = (2 * PivotPoint) - High;
		S_5 = (PivotPoint + S1) / 2;
		S2 = PivotPoint - (High - Low);
		S1_5 = (S1 + S2) / 2;
		R3 = 2 * PivotPoint + (High - 2*Low); // R3 std
		R2_5 = (R2 + R3) / 2;
		S3 = 2 * PivotPoint - (2 * High - Low); // S3 std
		S2_5 = (S2 + S3) / 2;
		R4 = 3 * PivotPoint + (High - 3 * Low);
		S4 = 3 * PivotPoint - (3 * High - Low);
	}
	else if (FormulaType == 4)
	{
		PivotPoint = (High + Low + Close) / 3;
		R1 = (PivotPoint * 2) - Low;
		R_5 = (PivotPoint + R1) / 2;
		R2 = PivotPoint + (High - Low);
		R1_5 = (R1 + R2) / 2;
		S1 = (2 * PivotPoint) - High;
		S_5 = (PivotPoint + S1) / 2;
		S2 = PivotPoint - (High - Low);
		S1_5 = (S1 + S2) / 2;
		R3 = PivotPoint + 2 * (High - Low); 
		R2_5 = (R2 + R3) / 2;
		S3 = PivotPoint - 2 * (High - Low); 
		S2_5 = (S2 + S3) / 2;
		R4 = PivotPoint + 3 * (High - Low);
		S4 = PivotPoint - 3 * (High - Low);
	}
	else if (FormulaType == 5) // Camarilla Pivot Points
	{
		float Range = High - Low;
		PivotPoint = (High + Low + Close) / 3;

		R_5 = Close + Range * 1.1f/18;
		R1 = Close + Range * 1.1f/12;
		R1_5 = Close + Range * 1.1f/9;
		R2 = Close + Range * 1.1f/6;
		R2_5 = Close + Range * 1.1f/5;
		R3 = Close + Range * 1.1f/4;
		R4 = Close + Range * 1.1f/2;

		S_5 = Close - Range * 1.1f/18;
		S1 = Close - Range * 1.1f/12;
		S1_5 = Close - Range * 1.1f/9;
		S2 = Close - Range * 1.1f/6;
		S2_5 = Close - Range * 1.1f/5;
		S3 = Close - Range * 1.1f/4;
		S4 = Close - Range * 1.1f/2;
		R5 = (High/Low) *Close;
		S5 = Close -(R5-Close);
	}
	else if (FormulaType == 6)  // Tom DeMark's Pivot Points
	{
		float X = 0;
		if (Close < Open)
			X = High + Low + Low + Close;
		else if (Close > Open)
			X = High + High + Low + Close;
		else if (Close == Open)
			X = High + Low + Close + Close;

		PivotPoint = X / 4;
		R1 = X / 2 - Low;
		S1 = X / 2 - High;
		R_5 = R2 = R1_5 = S_5 = S2 = S1_5 = R3 = 
			R2_5 = S3 = S2_5 = R4 = S4 = 0;
	}
	else if (FormulaType == 7) // Frank Dilernia Pivots
	{
		PivotPoint = (High + Low + Close) / 3;
		R1 = PivotPoint + (High - Low)/2;
		R2 = PivotPoint + (High - Low) * 0.618f;
		R3 = PivotPoint + (High - Low);
		S1 = PivotPoint - (High - Low)/2;
		S2 = PivotPoint - (High - Low) * 0.618f;
		S3 = PivotPoint - (High - Low);

		S2_5 = (S3 + S2)/2;				
		S1_5 = (S2 + S1)/2;				
		S_5 = (S1 + PivotPoint)/2;			
		R_5 = (R1 + PivotPoint)/2;			
		R1_5 = (R2 + R1)/2;				
		R2_5 = (R3 + R2)/2;				
	}
	else if (FormulaType == 8) // Shadow Trader. www.shadowtrader.net
	{
		PivotPoint = (High + Low + Close) / 3; //PIVOT = (High + Close + Low)/3
		R1 = (2 * PivotPoint) - Low; //R1 = (2*Pivot) - Low
		S1 = (2 * PivotPoint) - High; //S1 = (2*Pivot) - High
		R2 = PivotPoint + (R1 - S1); // R2 = Pivot + (R1 - S1)
		R3 = 2*(PivotPoint - Low) + High; //R3 = High + 2(Pivot - Low)
		S2 = PivotPoint - (R1 - S1); //S2 = Pivot - (R1 - S1)
		S3 = Low - 2 * (High - PivotPoint); //S3 = Low - 2(Hi - Pivot)

		S2_5 = (S3 + S2)/2;				
		S1_5 = (S2 + S1)/2;				
		S_5 = (S1 + PivotPoint)/2;			
		R_5 = (R1 + PivotPoint)/2;			
		R1_5 = (R2 + R1)/2;				
		R2_5 = (R3 + R2)/2;				

	}
	else if (FormulaType == 9)
	{

		PivotPoint = (High + Low + Close)/3; //PP = H + L + C /3
		float PPSquareRoot = sqrt(sqrt(PivotPoint));
		R1 = PivotPoint + PPSquareRoot; //R1 = PP + SQRT(SQRT(PP))
		R2 = R1 + PPSquareRoot; //R2 = R1 + SQRT(SQRT(PP))
		R3 = R2 + PPSquareRoot; //R3 = R2 + SQRT(SQRT(PP))
		S1 = PivotPoint - PPSquareRoot; //S1 = PP - SQRT(SQRT(PP))
		S2 = S1 - PPSquareRoot; //S2 = S1 - SQRT(SQRT(PP))
		S3 = S2 - PPSquareRoot; //S3 = S2 - SQRT(SQRT(PP))

		S2_5 = (S3 + S2)/2;				
		S1_5 = (S2 + S1)/2;				
		S_5 = (S1 + PivotPoint)/2;			
		R_5 = (R1 + PivotPoint)/2;			
		R1_5 = (R2 + R1)/2;				
		R2_5 = (R3 + R2)/2;				

	}
	else if (FormulaType == 10)
	{
		PivotPoint = (High + Low + Close)/3;	//PIVOT = H + L + C /3
		R1 = 2 * PivotPoint - Low;				//R1 = 2X PIVOT -LOW
		R2 = PivotPoint + High - Low;			//R2 = PIVOT + (HIGH - LOW)
		R3 = R1 + High - Low;					//R3 = R1+ (HIGH - LOW)

		S1 = 2 * PivotPoint - High;			//S1 = 2X PIVOT -HIGH
		S2 = PivotPoint - (High - Low);		//S2 = PIVOT - (HIGH - LOW)
		S3 = S1 - (High - Low);				//S3 = S1 - (HIGH - LOW)

		S2_5 = (S3 + S2)/2;					//M0 = S3+S2 /2
		S1_5 = (S2 + S1)/2;					//M1 = S2+S1 /2
		S_5 = (S1 + PivotPoint)/2;				//M2 = S1+PIVOT /2
		R_5 = (R1 + PivotPoint)/2;				//M3 = R1+PIVOT /2
		R1_5 = (R2 + R1)/2;					//M4 = R2+R1/2
		R2_5 = (R3 + R2)/2;					//M5 = R3 + R2 /2
	}
	else if (FormulaType == 11)
	{
		PivotPoint = (High + Low + Close)/3;
		float DailyRange = High - Low;

		R_5 = PivotPoint + 0.5f*DailyRange;
		R1 = PivotPoint + 0.618f*DailyRange;
		R1_5 = PivotPoint + DailyRange;
		R2 = PivotPoint + 1.272f*DailyRange;
		R2_5 = PivotPoint + 1.618f*DailyRange;
		R3 = PivotPoint + 2*DailyRange;
		R4 = PivotPoint + 2.618f*DailyRange;

		S_5 = PivotPoint - 0.5f*DailyRange;
		S1 = PivotPoint - 0.618f*DailyRange;
		S1_5 = PivotPoint - DailyRange;
		S2 = PivotPoint - 1.272f*DailyRange;
		S2_5 = PivotPoint - 1.618f*DailyRange;
		S3 = PivotPoint - 2*DailyRange;
		S4 = PivotPoint - 2.618f*DailyRange;

	
	}
	else if (FormulaType == 12)
	{
		PivotPoint = NextOpen;

		R_5 = PivotPoint;
		R1 = PivotPoint;
		R1_5 = PivotPoint;
		R2 = PivotPoint;
		R2_5 = PivotPoint;
		R3 = PivotPoint;
		R4 = PivotPoint;

		S_5 = PivotPoint;
		S1 = PivotPoint;
		S1_5 = PivotPoint;
		S2 = PivotPoint;
		S2_5 = PivotPoint;
		S3 = PivotPoint;
		S4 = PivotPoint;
	}
	else if (FormulaType == 13) // Fibonacci Pivot Points
	{
		PivotPoint = (High + Low + Close)/3;
		float DailyRange = High - Low;

	
		R1 = PivotPoint + 0.382f*DailyRange;
		R2 = PivotPoint + 0.618f*DailyRange;
		R3 = PivotPoint + 1*DailyRange;

		S1 = PivotPoint - 0.382f*DailyRange;
		S2 = PivotPoint - 0.618f*DailyRange;
		S3 = PivotPoint - 1*DailyRange;

		S2_5 = (S3 + S2)/2;				
		S1_5 = (S2 + S1)/2;				
		S_5 = (S1 + PivotPoint)/2;			
		R_5 = (R1 + PivotPoint)/2;			
		R1_5 = (R2 + R1)/2;				
		R2_5 = (R3 + R2)/2;				


	}
	else if (FormulaType == 14) 
	{
		PivotPoint = (High + Low + Close) / 3;
		
		S1 = (2 * PivotPoint) - High;
		R1 = (2 * PivotPoint) - Low;
		
		S2 = PivotPoint - (R1 - S1);
		R2 = (PivotPoint - S1) + R1;
		
		S3 = PivotPoint - (R2 - S1) ;
		R3 = (PivotPoint - S1) + R2;
	}
	else if (FormulaType == 15) // Fibonacci Zone Pivots
	{
		PivotPoint = (High + Low + Close) / 3;

		R1 = PivotPoint + (High - Low)/2;
		R2 = PivotPoint + (High - Low);

		S1 = PivotPoint - (High - Low)/2;
		S2 = PivotPoint - (High - Low);

		// 0.5 to 0.618 defines resistance/support band 1
		// 1.0 to 1.382 defines resistance/support band 2
		// the bands could start as lines and if the user 
		// wishes they can use Fill Rect top and bottom
		// or the transparent versions.
		R1_5 = PivotPoint + (High - Low)*0.618f;
		R2_5 = PivotPoint + (High - Low)*1.382f;

		S1_5 = PivotPoint - (High - Low)*0.618f;
		S2_5 = PivotPoint - (High - Low)*1.382f;
	}
	else	if(  FormulaType ==16 )
	{
		/*
		THE CENTRAL PIVOT RANGE FORMULA:

		TC (TOPCENTRAL) = (PIVOT-BC)+PIVOT
		PIVOT = (HIGH+LOW+CLOSE)/3
		BC (BOTTOM CENTRAL) =(HIGH+LOW)/2
		*/
		PivotPoint = (High + Low + Close) / 3;
		S1 = (High + Low) / 2;
		R1 = (PivotPoint - S1) + PivotPoint;
	}
	else if(FormulaType == 17)
	{
		PivotPoint = (High + Low + Close) / 3; // 	PP = (H+L+C)/3


		// H: high, L: low, C: close
			

		// Previous Day H: high, L: low, C: close
		R3 = High+2*(PivotPoint- Low); //R3 = H+2*(PP-L)
		R2 = PivotPoint+(High-Low);  //R2 = PP+(H-L)
		R2_5 = (R2+R3)/2; //M5 = (R2+R3)/2

		S1 = 2*PivotPoint - High; //	S1 = (2*PP)-H
		
		S2 = PivotPoint - (High - Low); //	S2 = PP-(H-L)

		S3 = Low - 2 * (High - PivotPoint);//	S3 = L-2*(H-PP) 

		S2_5 = (S2 + S3)/2; //	M0 = (S2+S3)/2
		
		
		R1 = 2*PivotPoint - Low; //	R1 = (2*PP)-L
		R_5 = (PivotPoint + R2)/2; //	M3 = (PP+R2)/2
		R1_5 = (R_5 + R2)/2; //	M4 = (M3+R2)/2
		S_5 = (PivotPoint + S2)/2; //	M2 = (PP+S2)/2
		S1_5 = (S_5 + S2)/2; //	M1 = (M2+S2)/2
	}
	else if(FormulaType == 18)
	{
		PivotPoint = (High + Low + Close) / 3;

		R1 = PivotPoint + (High - Low) * 0.75f;
		S1 = PivotPoint -  (High - Low) * 0.75f;
	}
	else if(FormulaType == 19)
	{
/*		Pivot Point (PP) = (Yesterday's High + Yesterday's Low + Current Open + Current Open) / 4

			Resistance Level (R1) = PivotPoint + 2 * (Yesterday's High - Yesterday's Low)

			Support Level (S1) = PivotPoint - 2 * (Yesterday's High - Yesterday's Low)
*/
		PivotPoint = (High + Low + 2 * NextOpen) / 4;
		R1 = PivotPoint + 2 * (High - Low);
		S1 = PivotPoint - 2 * (High - Low);
	}
	else	if(  FormulaType ==20 )
	{
		/* Calculations for ACD method
		*/
		PivotPoint = (High + Low + Close) / 3;
		float HighLowAverage = (High + Low)/2;
		float Difference = fabs (HighLowAverage -  PivotPoint);
		S1 = PivotPoint -Difference;
		R1 =PivotPoint  +Difference;
	}
	else if(  FormulaType == 21)
	{
		////////////Fibonacci Calculation///////////////////////////////////////////

		//R10 = OP + ((HighD(1) - LowD(1)) * 3.618);
		//R9 = OP + ((HighD(1) - LOWD(1))* 3.00);
		//R8 = OP + ((HighD(1) - LowD(1)) * 2.618);
		//R7 = OP + ((HighD(1) - LOWD(1))* 2.00);
		//R6 = OP + ((HighD(1) - LowD(1)) * 1.618);
		//R5 = OP + ((HighD(1) - LowD(1)) * 1.27);
		//R4 = OP + ((HighD(1) - LowD(1)) * 1.000);
		//R3 = OP + ((HighD(1) - LowD(1)) * .786);
		//R2 = OP + ((HighD(1) - LowD(1)) * .618);
		//R1 = OP + ((HighD(1) - LowD(1)) * .382);
		//OP = (OpenD(0));// (HighD(1) + LowD(1) + CloseD(1)) / 3;
		//S1 = OP - ((HighD(1) - LowD(1)) * .382);
		//S2 = OP - ((HighD(1) - LowD(1)) * .618);
		//S3 = OP - ((HighD (1) - LowD (1)) * .786);
		//S4 = OP - ((HighD(1) - LowD(1)) * 1.000);
		//S5 = OP - ((HighD(1) - LowD(1)) * 1.27);
		//S6 = OP - ((HighD(1) - LowD(1)) * 1.618);
		//S7 = OP - ((HighD(1) - LowD(1)) * 2.00);
		//S8 = OP - ((HighD(1) - LowD(1)) * 2.618);
		//S9 = OP - ((HighD(1) - LowD(1)) * 3.00);
		//S10 = OP - ((HighD(1) - LowD(1)) * 3.618); 

		PivotPoint = NextOpen;

		R_5 = (float)(PivotPoint + ((High - Low) * 0.382));
		R1 = (float)( PivotPoint + ((High - Low) * 0.618));
		R1_5 = (float)( PivotPoint + ((High - Low) * 0.786));
		R2 = (float)( PivotPoint + ((High - Low) * 1.0));
		R2_5 = (float)(PivotPoint + ((High - Low) * 1.27));
		R3 = (float)( PivotPoint + ((High - Low) * 1.618));
		R3_5 = (float)( PivotPoint + ((High - Low) * 2.0));
		R4 = (float)( PivotPoint + ((High - Low) * 2.618));
		R5 = (float)( PivotPoint + ((High - Low) * 3.0));

		S_5 = (float)( PivotPoint - ((High - Low) * 0.382));
		S1 = (float)( PivotPoint - ((High - Low) * 0.618));
		S1_5 = (float)( PivotPoint - ((High - Low) * 0.786));
		S2 = (float)(  PivotPoint - ((High - Low) * 1.0));
		S2_5 = (float)(  PivotPoint - ((High - Low) * 1.27));
		S3 = (float)(  PivotPoint - ((High - Low) * 1.618));
		S3_5 = (float)(  PivotPoint - ((High - Low) * 2.0));
		S4 = (float)(  PivotPoint - ((High - Low) * 2.618));
		S5 = (float)(  PivotPoint - ((High - Low) * 3.0));
	}
	
	
	
	return 1;
}

/*==========================================================================*/
int CalculateDailyOHLC
( SCStudyInterfaceRef sc
, int IntradayChartDate
, int InNumberOfDaysBack
, int InNumberOfDaysToCalculate
, int InUseSaturdayData
, int InUseThisIntradayChart
, int InDailyChartNumber
, SCGraphData& DailyChartData
, SCDateTimeArray& DailyChartDateTime
, int UseDaySessionOnly
, float& Open
, float& High
, float& Low
, float& Close
 )
{
	if (InUseThisIntradayChart) // Use this Chart
	{
		int ArraySize = sc.ArraySize;
		if (ArraySize < 2)
			return 0;
		
		int LastIndex = ArraySize - 1;
		
		int LastDateInChart = sc.GetTradingDayDate(sc.BaseDateTimeIn[LastIndex]);
		
		// Return and do not calculate pivots if the LastDateInChart to be calculated is further back than the number of days to calculate
		if (LastDateInChart - InNumberOfDaysToCalculate >= IntradayChartDate)
			return 0;
		
		
		// Look for the last good date (not a weekend and has data)
		int FirstIndexOfReferenceDay = 0;
		int ReferenceDay = IntradayChartDate;
		while (InNumberOfDaysBack > 0)
		{
			--ReferenceDay;
			
			
			if (IsWeekend(ReferenceDay, InUseSaturdayData != 0))
				continue;
			
			// It's not a weekend


			FirstIndexOfReferenceDay = sc.GetFirstIndexForDate(sc.ChartNumber, ReferenceDay);
			if (sc.GetTradingDayDate(sc.BaseDateTimeIn[FirstIndexOfReferenceDay]) == ReferenceDay)
				--InNumberOfDaysBack;
			
			if (InNumberOfDaysBack <= 0)
			{
				if (sc.GetTradingDayDate(sc.BaseDateTimeIn[FirstIndexOfReferenceDay]) != ReferenceDay)// Prior day not found
					return 0;
				
				break;
			}
			
			if (FirstIndexOfReferenceDay == 0)
			{
				if (sc.GetTradingDayDate(sc.BaseDateTimeIn[FirstIndexOfReferenceDay]) != ReferenceDay)// Prior day not found
					return 0;

				// At the beginning of the date array, so we can't look back any farther
				break;
			}
		}
		
	

		if(!UseDaySessionOnly)
		{

			if (!sc.GetOHLCForDate(ReferenceDay, Open, High, Low, Close))
				return 0;
		}
		else
		{
			SCDateTime StartDateTime;
			SCDateTime EndDateTime;

			if(StartDateTime <= EndDateTime)
			{
				StartDateTime.SetDate(ReferenceDay);
				StartDateTime.SetTime(sc.StartTime1);

				EndDateTime.SetDate(ReferenceDay);
				EndDateTime.SetTime(sc.EndTime1);
			}
			else
			{
				StartDateTime.SetDate(ReferenceDay - 1);
				StartDateTime.SetTime(sc.StartTime1);

				EndDateTime.SetDate(ReferenceDay);
				EndDateTime.SetTime(sc.EndTime1);
			}

			float NextOpen = 0;

			if(!sc.GetOHLCOfTimePeriod(StartDateTime, EndDateTime, Open, High, Low, Close, NextOpen))
				return 0;
		}
	}
	else // Use Daily Chart
	{
		
		int ArraySize = DailyChartData[0].GetArraySize();
		if (ArraySize < 2)
			return 0;
		
		int LastIndex = ArraySize - 1;
		
		int LastDateInChart = sc.GetTradingDayDate(sc.BaseDateTimeIn[sc.ArraySize - 1]);
		
		// Return and do not calculate pivots if the LastDateInChart to be calculated is further back than the number of days to calculate
		if (LastDateInChart - InNumberOfDaysToCalculate >= IntradayChartDate)
			return 0;
		
		
		// Look for the last good date (not a weekend and has data)
		int FirstIndexOfReferenceDay = 0;
		int ReferenceDay = IntradayChartDate;
		if (InNumberOfDaysBack == 0)
		{
			FirstIndexOfReferenceDay = sc.GetFirstIndexForDate(InDailyChartNumber, ReferenceDay);
		}
		while (InNumberOfDaysBack > 0)
		{
			--ReferenceDay;
			
			
			if (IsWeekend(ReferenceDay, InUseSaturdayData != 0))
				continue;
			
			// It's not a weekend


			FirstIndexOfReferenceDay = sc.GetFirstIndexForDate(InDailyChartNumber, ReferenceDay);
			if (sc.GetTradingDayDateForChartNumber(InDailyChartNumber, DailyChartDateTime[FirstIndexOfReferenceDay]) == ReferenceDay)
				--InNumberOfDaysBack;
			
			if (InNumberOfDaysBack <= 0)
			{
				if (sc.GetTradingDayDateForChartNumber(InDailyChartNumber, DailyChartDateTime[FirstIndexOfReferenceDay]) != ReferenceDay)// Prior day not found
					return 0;
				
				break;
			}
			
			if (FirstIndexOfReferenceDay == 0)
			{
				if (sc.GetTradingDayDateForChartNumber(InDailyChartNumber, DailyChartDateTime[FirstIndexOfReferenceDay]) != ReferenceDay)// Prior day not found
					return 0;

				// At the beginning of the date array, so we can't look back any farther
				break;
			}
		}
		
		Open = DailyChartData[SC_OPEN][FirstIndexOfReferenceDay];
		High = DailyChartData[SC_HIGH][FirstIndexOfReferenceDay];
		Low = DailyChartData[SC_LOW][FirstIndexOfReferenceDay];
		Close = DailyChartData[SC_LAST][FirstIndexOfReferenceDay];
	}
	
	return 1;
}

/*==========================================================================*/
int CalculateDailyPivotPoints
(SCStudyInterfaceRef sc
 , int IntradayChartDate
 , int FormulaType
 , int DailyChartNumber
 , SCGraphData& DailyChartData
 , SCDateTimeArray& DailyChartDateTime
 , int NumberOfDays
 , int UseSaturdayData
 , int UseThisChart
 , int UseManualValues
 , float UserOpen
 , float UserHigh
 , float UserLow
 , float UserClose
 , int  UseDaySessionOnly
 , float& PivotPoint
 , float& R_5
 , float& R1, float& R1_5
 , float& R2, float& R2_5
 , float& R3
 , float& S_5
 , float& S1, float& S1_5
 , float& S2, float& S2_5
 , float& S3
 , float & R3_5
 , float & S3_5
 , float& R4
 , float& S4
 , float& R5
 , float& S5
 )
{

	float Open = 0.0, High = 0.0, Low = 0.0, Close = 0.0, NextOpen = 0.0;

	if (UseThisChart != 0) // Use this Chart
	{
		int ArraySize = sc.BaseDateTimeIn.GetArraySize();
		if (ArraySize < 2)
			return 0;

		int LastIndex = ArraySize - 1;

		int CurrentDate = sc.GetTradingDayDate(sc.BaseDateTimeIn[LastIndex]);
		
		// Return and do not calculate pivots if the current bar to be calculated is further back than the number of days to calculate
		if (CurrentDate - NumberOfDays >= IntradayChartDate)
			return 0;


		// Look for the last good date which is not a weekend and has data
		int FirstIndexOfPriorDay;
		int PreviousDay = IntradayChartDate;
		while (true)
		{
			--PreviousDay;

		
			if (IsWeekend(PreviousDay, UseSaturdayData != 0))
				continue;

			// It is not a weekend
			FirstIndexOfPriorDay = sc.GetFirstIndexForDate(sc.ChartNumber, PreviousDay);
			if (sc.GetTradingDayDate(sc.BaseDateTimeIn[FirstIndexOfPriorDay]) == PreviousDay)
				break;

			if (FirstIndexOfPriorDay == 0)
			{
				// At the beginning of the date array, so we can't look back any farther
				break;
			}
		}

		if (sc.GetTradingDayDate(sc.BaseDateTimeIn[FirstIndexOfPriorDay]) != PreviousDay)
		{
			// Previous day not found
			return 0;
		}


		if(!UseDaySessionOnly)
		{


			if (!sc.GetOHLCForDate(PreviousDay, Open, High, Low, Close))
				return 0;

			float NextHigh, NextLow, NextClose;  // The values returned for these are unused
			if (!sc.GetOHLCForDate(IntradayChartDate, NextOpen, NextHigh, NextLow, NextClose))
				NextOpen = Close;  // If there is a failure, use the prior Close
		}
		else
		{
			SCDateTime StartDateTime;
			SCDateTime EndDateTime;

			if(sc.StartTime1 <= sc.EndTime1)
			{
				StartDateTime.SetDate(PreviousDay);
				StartDateTime.SetTime(sc.StartTime1);

				EndDateTime.SetDate(PreviousDay);
				EndDateTime.SetTime(sc.EndTime1);
			}
			else
			{
				StartDateTime.SetDate(PreviousDay - 1);
				StartDateTime.SetTime(sc.StartTime1);

				EndDateTime.SetDate(PreviousDay);
				EndDateTime.SetTime(sc.EndTime1);
			}


			
			if(!sc.GetOHLCOfTimePeriod(StartDateTime, EndDateTime, Open, High, Low, Close, NextOpen))
				return 0;
		}
	}
	else // Use Daily Chart
	{
		if (DailyChartData[0].GetArraySize() < 2)
			return 0;

		int p = -1; 
		int CountBack = DailyChartData[SC_OPEN].GetArraySize() - NumberOfDays - 1;
		for(int index = DailyChartData[SC_OPEN].GetArraySize() - 1; index >= CountBack && index >= 0; index-- )
		{
			if (sc.GetTradingDayDateForChartNumber(DailyChartNumber, DailyChartDateTime[index]) < IntradayChartDate)
			{
				p = index;
				break;
			}
		}

		if(p == -1)
			return 0;

		Open = DailyChartData[SC_OPEN][p];
		High = DailyChartData[SC_HIGH][p];
		Low = DailyChartData[SC_LOW][p];
		Close = DailyChartData[SC_LAST][p];

		if( p == DailyChartData[0].GetArraySize() - 1 )
			NextOpen = DailyChartData[SC_LAST][p];
		else
			NextOpen = DailyChartData[SC_OPEN][p+1];
	}
	
	// If we are calculating the last day, and the user has manually entered OHLC values, then use those
	if (UseManualValues)
	{
		int ArraySize = sc.BaseDateTimeIn.GetArraySize();

		int LastIndex = ArraySize - 1;

		int CurrentDate = sc.GetTradingDayDate(sc.BaseDateTimeIn[LastIndex]);

		if (CurrentDate == IntradayChartDate)
		{
			Open = (UserHigh + UserLow + UserClose) / 3.0f;  // This is not normally used, but we need to set it to something reasonable
			High = UserHigh;
			Low = UserLow;
			Close = UserClose;
			NextOpen = UserOpen;
		}
	}

	return CalculatePivotPoints
		(  Open
		,  High
		,  Low
		,  Close
		,  NextOpen
		,  PivotPoint
		,  R_5
		,  R1,  R1_5
		,  R2,  R2_5
		,  R3
		,  S_5
		,  S1,  S1_5
		,  S2,  S2_5
		,  S3
		,  R3_5
		,  S3_5
		,  R4
		,  S4
		,  R5
		,  S5
		,  FormulaType
		);

}

/*==========================================================================*/
SCFloatArrayRef Slope_S(SCFloatArrayRef In, SCFloatArrayRef Out, int Index)
{
	Out[Index] = In[Index] - In[Index - 1];

	return Out;
}

/*==========================================================================*/
SCFloatArrayRef DoubleStochastic_S(SCBaseDataRef BaseDataIn, SCFloatArrayRef Out, SCFloatArrayRef MovAvgIn, SCFloatArrayRef MovAvgOut,
								   SCFloatArrayRef MovAvgIn2, SCFloatArrayRef Unused, int Index, int Length, int MovAvgLength, int MovAvgType)
{
	/*-------------------------------
	MovingAvgX ((((Pl - Lowest (Pl , 10)) / Deler2) * 100) , 3 , False)

	where:

	PL= MovingAvgX (((Close - Lowest Low (10)) / Deler) * 100 , 3 , False)
	Deler= IFF (highest_minus_lowest > 0 , highest_minus_lowest , 1)
	Deler2= IFF (highest_minus_lowest#2 > 0 , highest_minus_lowest#2 , 1)
	Highest_minus_lowest= Highest High (10) - Lowest Low (10)
	Highest_minus_lowest#2= Highest High (5) - Lowest Low (5)
	-----------------------------*/

	float HighestMinusLowest1 = Highest(BaseDataIn[SC_HIGH], Index, Length) - Lowest(BaseDataIn[SC_LOW], Index, Length);
	float Calc1 = HighestMinusLowest1 > 0 ? HighestMinusLowest1 : 1;

	MovAvgIn[Index] = (BaseDataIn[SC_LAST][Index] - Lowest(BaseDataIn[SC_LOW], Index, Length))/Calc1*100.0f;


	MovingAverage_S(MovAvgIn, MovAvgOut,MovAvgType, Index, MovAvgLength); 

	float HighestMinusLowest2 = Highest(MovAvgOut, Index, Length) - Lowest(MovAvgOut, Index, Length);
	float Calc2 = HighestMinusLowest2 > 0 ? HighestMinusLowest2 : 1;


	MovAvgIn2[Index]= (MovAvgOut[Index] - Lowest(MovAvgOut, Index, Length))/Calc2*100.0f;

	MovingAverage_S(MovAvgIn2, Out, MovAvgType, Index, MovAvgLength);

	return Out;
}




/*==========================================================================*/
double GetStandardError(SCFloatArrayRef In, int Index, int Length)
{
	if (Length <= 1)
		return 0.0;

	if (Index < Length - 1)
		return 0.0;

	double  AvgX = (Length - 1) * 0.5f;

	double AvgY = 0;
	for (int n = 0; n < Length; n++)
		AvgY += In[Index - n];

	AvgY /= Length;

	double SumDxSqr = 0;
	double SumDySqr = 0;
	double SumDxDy = 0;
	for (int n = 0; n < Length; n++)
	{
		double Dx = n - AvgX;
		double Dy = In[Index - n] - AvgY;
		SumDxSqr += Dx * Dx;
		SumDySqr += Dy * Dy;
		SumDxDy += Dx * Dy;
	}

	return sqrt((SumDySqr - (SumDxDy * SumDxDy) / SumDxSqr) / (Length - 2));
}

/*==========================================================================*/
SCFloatArray& StandardError_S(SCFloatArrayRef In, SCFloatArray& Out, int Index, int Length)
{
	Out[Index] = (float) GetStandardError(In,Index,Length);

	return Out;
}

/*==========================================================================*/
int CrossOver(SCFloatArrayRef First, SCFloatArrayRef Second, int Index)
{
	float X1 = First[Index-1];
	float X2 = First[Index];
	float Y1 = Second[Index-1];
	float Y2 = Second[Index];

	if (X2 != Y2)  // The following is not useful if X2 and Y2 are equal
	{
		// Find non-equal values for prior values
		int PriorIndex = Index - 1;
		while (X1 == Y1 && PriorIndex > 0 && PriorIndex > Index - 100)
		{
			--PriorIndex;
			X1 = First[PriorIndex];
			Y1 = Second[PriorIndex];
		}
	}

	if (X1 > Y1 && X2 < Y2)
		return CROSS_FROM_TOP;
	else if (X1 < Y1 && X2 > Y2)
		return CROSS_FROM_BOTTOM;
	else
		return NO_CROSS;
}

/*==========================================================================*/
SCFloatArrayRef CumulativeDeltaVolume_S(SCBaseDataRef BaseDataIn, SCFloatArrayRef Open, SCFloatArrayRef High, SCFloatArrayRef Low, SCFloatArrayRef Close, int Index, int ResetCumulativeCalculation)
{
	SCFloatArrayRef BidVolume      = BaseDataIn[SC_BIDVOL];
	SCFloatArrayRef AskVolume      = BaseDataIn[SC_ASKVOL];
	SCFloatArrayRef DifferenceHigh = BaseDataIn[SC_ASKBID_DIFF_HIGH];
	SCFloatArrayRef DifferenceLow  = BaseDataIn[SC_ASKBID_DIFF_LOW];

	if (Index == 0 || ResetCumulativeCalculation)
	{
		Open[Index]  = 0;
		High[Index]  = DifferenceHigh[Index];
		Low[Index]   = DifferenceLow[Index];
		Close[Index] = AskVolume[Index] - BidVolume[Index];

		if (Open[Index] > High[Index]) 
			Open[Index] = High[Index];

		if (Open[Index] < Low[Index])
			Open[Index] = Low[Index];
	}
	else
	{
		Open[Index]  = Close[Index-1];
		High[Index]  = Close[Index-1] + DifferenceHigh[Index];
		Low[Index]   = Close[Index-1] + DifferenceLow[Index];
		Close[Index] = Close[Index-1] + (AskVolume[Index] - BidVolume[Index]);

		if (Open[Index] > High[Index]) 
			Open[Index] = High[Index];

		if (Open[Index] < Low[Index])
			Open[Index] = Low[Index];
	}

	return Close;
}

/*==========================================================================*/
SCFloatArrayRef CumulativeDeltaTicks_S(SCBaseDataRef BaseDataIn, SCFloatArrayRef Open, SCFloatArrayRef High, SCFloatArrayRef Low, SCFloatArrayRef Close, int Index, int ResetCumulativeCalculation)
{
	SCFloatArrayRef BidNumberTrades = BaseDataIn[SC_BIDNT];
	SCFloatArrayRef AskNumberTrades = BaseDataIn[SC_ASKNT];
	SCFloatArrayRef DifferenceHigh  = BaseDataIn[SC_ASKBID_NT_DIFF_HIGH];
	SCFloatArrayRef DifferenceLow   = BaseDataIn[SC_ASKBID_NT_DIFF_LOW];

	if (Index == 0 || ResetCumulativeCalculation)
	{
		Open[Index]  = 0;
		High[Index]  = DifferenceHigh[Index];
		Low[Index]   = DifferenceLow[Index];
		Close[Index] = AskNumberTrades[Index] - BidNumberTrades[Index];

		if (Open[Index] > High[Index]) 
			Open[Index] = High[Index];

		if (Open[Index] < Low[Index])
			Open[Index] = Low[Index];
	}
	else
	{
		Open[Index]  = Close[Index-1];
		High[Index]  = Close[Index-1] + DifferenceHigh[Index];
		Low[Index]   = Close[Index-1] + DifferenceLow[Index];
		Close[Index] = Close[Index-1] + (AskNumberTrades[Index] - BidNumberTrades[Index]);

		if (Open[Index] > High[Index]) 
			Open[Index] = High[Index];

		if (Open[Index] < Low[Index])
			Open[Index] = Low[Index];
	}

	return Close;
}

/*==========================================================================*/
SCFloatArrayRef CumulativeDeltaTickVolume_S(SCBaseDataRef BaseDataIn, SCFloatArrayRef Open, SCFloatArrayRef High, SCFloatArrayRef Low, SCFloatArrayRef Close, int Index, int ResetCumulativeCalculation)
{
	SCFloatArrayRef UpTickVolume   = BaseDataIn[SC_UPVOL];
	SCFloatArrayRef DownTickVolume = BaseDataIn[SC_DOWNVOL];
	SCFloatArrayRef DifferenceHigh = BaseDataIn[SC_UPDOWN_VOL_DIFF_HIGH];
	SCFloatArrayRef DifferenceLow  = BaseDataIn[SC_UPDOWN_VOL_DIFF_LOW];

	if (Index == 0 || ResetCumulativeCalculation)
	{
		Open[Index]  = 0;
		High[Index]  = DifferenceHigh[Index];
		Low[Index]   = DifferenceLow[Index];
		Close[Index] = UpTickVolume[Index] - DownTickVolume[Index];

		if (Open[Index] > High[Index]) 
			Open[Index] = High[Index];

		if (Open[Index] < Low[Index])
			Open[Index] = Low[Index];
	}
	else
	{
		Open[Index]  = Close[Index-1];
		High[Index]  = Close[Index-1] + DifferenceHigh[Index];
		Low[Index]   = Close[Index-1] + DifferenceLow[Index];
		Close[Index] = Close[Index-1] + (UpTickVolume[Index] - DownTickVolume[Index]);

		if (Open[Index] > High[Index]) 
			Open[Index] = High[Index];

		if (Open[Index] < Low[Index])
			Open[Index] = Low[Index];
	}

	return Close;
}

/*==========================================================================*/
SCFloatArrayRef ZigZag_S(SCFloatArrayRef InputDataHigh, SCFloatArrayRef InputDataLow, SCFloatArrayRef ZigZagValues, SCFloatArrayRef ZigZagPeakType, SCFloatArrayRef ZigZagPeakIndex, int Index, float ReversalPercent, float ReversalAmount)
{
	return ResettableZigZag_S(InputDataHigh, InputDataLow, ZigZagValues, ZigZagPeakType, ZigZagPeakIndex, 0, Index, ReversalPercent, ReversalAmount);
}

/*==========================================================================*/
//  ZigZagValues: value of zig zag line
//  ZigZagPeakType: +1=high peak, -1=low peak, 0=not peak
//  ZigZagPeakIndex: index of current peak, -1=no peak yet
SCFloatArrayRef ResettableZigZag_S(SCFloatArrayRef InputDataHigh, SCFloatArrayRef InputDataLow, SCFloatArrayRef ZigZagValues, SCFloatArrayRef ZigZagPeakType, SCFloatArrayRef ZigZagPeakIndex, int StartIndex, int Index, float ReversalPercent, float ReversalAmount)
{
	if (Index == StartIndex)
	{
		for(int ZeroIndex = StartIndex ; ZeroIndex < ZigZagValues.GetArraySize(); ZeroIndex++)
		{
			ZigZagValues[ZeroIndex] = 0;
			ZigZagPeakType[ZeroIndex] = 0;
			ZigZagPeakIndex[ZeroIndex] = 0;
		}
		ZigZagPeakIndex[StartIndex] = -1;
		return ZigZagValues;
	}


	if (ZigZagPeakIndex[Index] == 0)
	{		
		if (ZigZagPeakIndex[Index-1] != -1)
		{
			// new bar, copy values forward
			ZigZagPeakIndex[Index] = ZigZagPeakIndex[Index-1];
			ZigZagValues[Index] = 0;
			ZigZagPeakType[Index] = 0;
		}
		else
		{
			// still looking for initial trend
			if (InputDataHigh[Index] > InputDataHigh[StartIndex] && InputDataLow[Index] > InputDataLow[StartIndex]) //bullish trend
			{
				ZigZagPeakIndex[Index] = (float)Index;
				ZigZagValues[Index] = InputDataHigh[Index];
				ZigZagPeakType[Index] = 1;

				ZigZagPeakIndex[StartIndex] = (float)StartIndex;
				ZigZagValues[StartIndex] = InputDataLow[StartIndex];
				ZigZagPeakType[StartIndex] = -1;

				float Increment = (InputDataHigh[Index] - InputDataLow[StartIndex])/(Index-StartIndex);

				for (int n=StartIndex+1, Count=1; n<Index; n++, Count++)
				{
					ZigZagValues[n] = InputDataLow[StartIndex] + Count*Increment;
					ZigZagPeakIndex[n] = (float)StartIndex;
					ZigZagPeakType[n] = 0;
				}
			}
			else if (InputDataHigh[Index] < InputDataHigh[StartIndex] && InputDataLow[Index] < InputDataLow[StartIndex]) //bearish trend
			{
				ZigZagPeakIndex[Index] = (float)Index;
				ZigZagValues[Index] = InputDataLow[Index];
				ZigZagPeakType[Index] = -1;

				ZigZagPeakIndex[StartIndex] = (float)StartIndex;
				ZigZagValues[StartIndex] = InputDataHigh[StartIndex];
				ZigZagPeakType[StartIndex] = 1;

				float Increment = (InputDataHigh[StartIndex] - InputDataLow[Index])/(Index-StartIndex);

				for (int n=StartIndex+1, Count=1; n<Index; n++, Count++)
				{
					ZigZagValues[n] = InputDataHigh[StartIndex] - Count*Increment;
					ZigZagPeakIndex[n] = (float)StartIndex;
					ZigZagPeakType[n] = 0;
				}
			}
			else
			{
				ZigZagPeakIndex[Index] = -1;
				ZigZagValues[Index] = 0;
				ZigZagPeakType[Index] = 0;
			}

			return ZigZagValues;
		}
	}


	int   CurrentPeakIndex = (int)ZigZagPeakIndex[Index];
	int   CurrentPeakType  = (int)ZigZagPeakType[CurrentPeakIndex];
	float CurrentPeakValue = ZigZagValues[CurrentPeakIndex];

	int   PriorPeakIndex   = (int)ZigZagPeakIndex[CurrentPeakIndex-1];
	float PriorPeakValue   = ZigZagValues[PriorPeakIndex];

	bool CurrentBarIsPeak = Index == CurrentPeakIndex;

	float BarLow = InputDataLow[Index];
	float BarHigh = InputDataHigh[Index];


	if (CurrentPeakType > 0)
	{
		float ReversalPrice;
		if (ReversalPercent == 0.0f)
			ReversalPrice = CurrentPeakValue - ReversalAmount;
		else
			ReversalPrice = CurrentPeakValue - (CurrentPeakValue*ReversalPercent);

		if (!CurrentBarIsPeak && BarLow < ReversalPrice)
		{
			ZigZagPeakIndex[Index] = (float)Index;
			ZigZagValues[Index] = BarLow;
			ZigZagPeakType[Index] = -1;

			float Increment = (CurrentPeakValue - BarLow)/(Index - CurrentPeakIndex);

			for (int n = CurrentPeakIndex+1, Count=1; n < Index; n++, Count++)
			{
				ZigZagValues[n] = CurrentPeakValue - Count*Increment;
			}
		}
		else if (BarHigh > CurrentPeakValue)
		{
			ZigZagPeakIndex[Index] = (float)Index;
			ZigZagValues[Index] = BarHigh;
			ZigZagPeakType[Index] = 1;

			float Increment = (BarHigh - PriorPeakValue)/(Index - PriorPeakIndex);

			for (int n = PriorPeakIndex+1, Count=1; n < Index; n++, Count++)
			{
				ZigZagValues[n] = PriorPeakValue + Count*Increment;
				ZigZagPeakIndex[n] = (float)PriorPeakIndex;
				ZigZagPeakType[n] = 0;
			}
		}
	}
	else if (CurrentPeakType < 0)
	{
		float ReversalPrice;
		if (ReversalPercent == 0.0f)
			ReversalPrice = CurrentPeakValue + ReversalAmount;
		else
			ReversalPrice = CurrentPeakValue + (CurrentPeakValue*ReversalPercent);

		if (!CurrentBarIsPeak && BarHigh > ReversalPrice)
		{
			ZigZagPeakIndex[Index] = (float)Index;
			ZigZagValues[Index] = BarHigh;
			ZigZagPeakType[Index] = 1;

			float Increment = (BarHigh - CurrentPeakValue)/(Index - CurrentPeakIndex);

			for (int n = CurrentPeakIndex+1, Count=1; n < Index; n++, Count++)
			{
				ZigZagValues[n] = CurrentPeakValue + Count*Increment;
			}
		}
		else if (BarLow < CurrentPeakValue)
		{
			ZigZagPeakIndex[Index] = (float)Index;
			ZigZagValues[Index] = BarLow;
			ZigZagPeakType[Index] = -1;

			float Increment = (PriorPeakValue - BarLow)/(Index - PriorPeakIndex);

			for (int n = PriorPeakIndex+1, Count=1; n < Index; n++, Count++)
			{
				ZigZagValues[n] = PriorPeakValue - Count*Increment;
				ZigZagPeakIndex[n] = (float)PriorPeakIndex;
				ZigZagPeakType[n] = 0;
			}
		}
	}

	return ZigZagValues;
}


/*==========================================================================*/
SCFloatArrayRef ZigZag2_S(SCFloatArrayRef InputDataHigh, SCFloatArrayRef InputDataLow, SCFloatArrayRef ZigZagValues, SCFloatArrayRef ZigZagPeakType, SCFloatArrayRef ZigZagPeakIndex, int Index, int NumberOfBars, float ReversalAmount)
{
	return ResettableZigZag2_S(InputDataHigh, InputDataLow, ZigZagValues, ZigZagPeakType, ZigZagPeakIndex, 0, Index, NumberOfBars, ReversalAmount);
}

/*==========================================================================*/
// ZigZagPeakType : +1=high peak, -1=low peak, 0=not peak
// ZigZagPeakIndex: index of current peak, -1=no peak yet
SCFloatArrayRef ResettableZigZag2_S(SCFloatArrayRef InputDataHigh, SCFloatArrayRef InputDataLow, SCFloatArrayRef ZigZagValues, SCFloatArrayRef ZigZagPeakType, SCFloatArrayRef ZigZagPeakIndex, int StartIndex, int Index, int NumberOfBars, float ReversalAmount)
{
	if (Index == StartIndex)
	{
		for(int ZeroIndex = StartIndex ; ZeroIndex < ZigZagValues.GetArraySize(); ZeroIndex++)
		{
			ZigZagValues[ZeroIndex] = 0;
			ZigZagPeakType[ZeroIndex] = 0;
			ZigZagPeakIndex[ZeroIndex] = 0;
		}
		ZigZagPeakIndex[StartIndex] = -1;
		return ZigZagValues;
	}

	
	if (ZigZagPeakIndex[Index] == 0)
	{		
		if (ZigZagPeakIndex[Index-1] != -1)
		{
			// new bar, copy values forward
			ZigZagPeakIndex[Index] = ZigZagPeakIndex[Index-1];
			ZigZagValues[Index] = 0;
			ZigZagPeakType[Index] = 0;
		}
		else
		{
			// still looking for initial trend
			if (InputDataHigh[Index] > InputDataHigh[StartIndex] && InputDataLow[Index] > InputDataLow[StartIndex]) //bullish trend
			{
				ZigZagPeakIndex[Index] = (float)Index;
				ZigZagValues[Index] = InputDataHigh[Index];
				ZigZagPeakType[Index] = 1;

				ZigZagPeakIndex[StartIndex] = (float)StartIndex;
				ZigZagValues[StartIndex] = InputDataLow[StartIndex];
				ZigZagPeakType[StartIndex] = -1;

				float Increment = (InputDataHigh[Index] - InputDataLow[StartIndex])/(Index-StartIndex);

				for (int n=StartIndex+1, Count=1; n<Index; n++, Count++)
				{
					ZigZagValues[n] = InputDataLow[StartIndex] + Count*Increment;
					ZigZagPeakIndex[n] = (float)StartIndex;
					ZigZagPeakType[n] = 0;
				}
			}
			else if (InputDataHigh[Index] < InputDataHigh[StartIndex] && InputDataLow[Index] < InputDataLow[StartIndex]) //bearish trend
			{
				ZigZagPeakIndex[Index] = (float)Index;
				ZigZagValues[Index] = InputDataLow[Index];
				ZigZagPeakType[Index] = -1;

				ZigZagPeakIndex[StartIndex] = (float)StartIndex;
				ZigZagValues[StartIndex] = InputDataHigh[StartIndex];
				ZigZagPeakType[StartIndex] = 1;

				float Increment = (InputDataHigh[StartIndex] - InputDataLow[Index])/(Index-StartIndex);

				for (int n=StartIndex+1, Count=1; n<Index; n++, Count++)
				{
					ZigZagValues[n] = InputDataHigh[StartIndex] - Count*Increment;
					ZigZagPeakIndex[n] = (float)StartIndex;
					ZigZagPeakType[n] = 0;
				}
			}
			else
			{
				ZigZagPeakIndex[Index] = -1;
				ZigZagValues[Index] = 0;
				ZigZagPeakType[Index] = 0;
			}
			return ZigZagValues;
		}
	}


	int   CurrentPeakIndex = (int)ZigZagPeakIndex[Index];
	int   CurrentPeakType  = (int)ZigZagPeakType[CurrentPeakIndex];
	float CurrentPeakValue = ZigZagValues[CurrentPeakIndex];

	int   PriorPeakIndex   = (int)ZigZagPeakIndex[CurrentPeakIndex-1];
	float PriorPeakValue   = ZigZagValues[PriorPeakIndex];

	bool CurrentBarIsPeak = Index == CurrentPeakIndex;

	float BarLow = InputDataLow[Index];
	float BarHigh = InputDataHigh[Index];


	if (CurrentPeakType > 0)
	{
		if (BarHigh >= CurrentPeakValue)
		{
			ZigZagPeakIndex[Index] = (float)Index;
			ZigZagValues[Index] = BarHigh;
			ZigZagPeakType[Index] = 1;

			float Increment = (BarHigh - PriorPeakValue)/(Index - PriorPeakIndex);

			for (int n = PriorPeakIndex+1, Count=1; n < Index; n++, Count++)
			{
				ZigZagValues[n] = PriorPeakValue + Count*Increment;
				ZigZagPeakIndex[n] = (float)PriorPeakIndex;
				ZigZagPeakType[n] = 0;
			}
		}
		else
		{
			int SkipCount = Index - CurrentPeakIndex;

			if (BarLow < PriorPeakValue ||
				(SkipCount >= NumberOfBars && CurrentPeakValue - BarLow > ReversalAmount)) //change to bearish
			{
				ZigZagPeakIndex[Index] = (float)Index;
				ZigZagValues[Index] = BarLow;
				ZigZagPeakType[Index] = -1;

				float Increment = (CurrentPeakValue - BarLow)/(Index - CurrentPeakIndex);

				for (int n = CurrentPeakIndex+1, Count=1; n < Index; n++, Count++)
				{
					ZigZagValues[n] = CurrentPeakValue - Count*Increment;
				}
			}
		}
	}
	else if (CurrentPeakType < 0)
	{
		if (BarLow <= CurrentPeakValue)
		{
			ZigZagPeakIndex[Index] = (float)Index;
			ZigZagValues[Index] = BarLow;
			ZigZagPeakType[Index] = -1;

			float Increment = (PriorPeakValue - BarLow)/(Index - PriorPeakIndex);

			for (int n = PriorPeakIndex+1, Count=1; n < Index; n++, Count++)
			{
				ZigZagValues[n] = PriorPeakValue - Count*Increment;
				ZigZagPeakIndex[n] = (float)PriorPeakIndex;
				ZigZagPeakType[n] = 0;
			}
		}
		else
		{
			int SkipCount = Index - CurrentPeakIndex;

			if (BarHigh > PriorPeakValue ||
				(SkipCount >= NumberOfBars && BarHigh - CurrentPeakValue > ReversalAmount)) //change to bullish
			{
				ZigZagPeakIndex[Index] = (float)Index;
				ZigZagValues[Index] = BarHigh;
				ZigZagPeakType[Index] = 1;

				float Increment = (BarHigh - CurrentPeakValue)/(Index - CurrentPeakIndex);

				for (int n = CurrentPeakIndex+1, Count=1; n < Index; n++, Count++)
				{
					ZigZagValues[n] = CurrentPeakValue + Count*Increment;
				}
			}
		}
	}

	return ZigZagValues;
}

/*==========================================================================*/

#include "sierrachart.h"

#include "scstudyfunctions.h"

/***********************************************************************/
SCSFExport scsf_ChaikinOsc(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Oscillator = sc.Subgraph[0];
	SCSubgraphRef Overbought = sc.Subgraph[2];
	SCSubgraphRef Oversold = sc.Subgraph[3];


	SCInputRef LongMALength = sc.Input[2];
	SCInputRef ShortMALength = sc.Input[3];
	SCInputRef Divisor = sc.Input[4];
	SCInputRef OverboughtValue = sc.Input[5];
	SCInputRef OversoldValue = sc.Input[6];
	
	if(sc.SetDefaults)
	{
		sc.GraphName = "Chaikin Oscillator";
		
		Oscillator.Name = "Oscillator";
		Oscillator.DrawStyle = DRAWSTYLE_LINE;
		Oscillator.PrimaryColor = RGB(0,255,0);

		Overbought.Name = "Overbought";
		Overbought.DrawStyle = DRAWSTYLE_LINE;
		Overbought.PrimaryColor = RGB(255,255,0);
		Overbought.DrawZeros = false;

		Oversold.Name = "Oversold";
		Oversold.DrawStyle = DRAWSTYLE_LINE;
		Oversold.PrimaryColor = RGB(255,127,0);
		Oversold.DrawZeros = false;

		LongMALength.Name = "Long MovAvg Length";
		LongMALength.SetInt(10);
		LongMALength.SetIntLimits(1,MAX_STUDY_LENGTH);

		ShortMALength.Name = "Short MovAvg Length";
		ShortMALength.SetInt(3);
		ShortMALength.SetIntLimits(1,MAX_STUDY_LENGTH);

		Divisor.Name = "Divisor";
		Divisor.SetFloat(1);
		Divisor.SetFloatLimits(.00001f, (float)MAX_STUDY_LENGTH);

		OverboughtValue.Name = "Overbought";
		OverboughtValue.SetInt(0);

		OversoldValue.Name = "Oversold";
		OversoldValue.SetInt(0);

		sc.FreeDLL = 0;

		sc.AutoLoop = 1;

		return;
	}
	
	sc.DataStartIndex = max(LongMALength.GetInt(),ShortMALength.GetInt()) - 1;


	sc.AccumulationDistribution(sc.BaseDataIn, Oscillator.Arrays[0], sc.Index);
	sc.ExponentialMovAvg(Oscillator.Arrays[0], Oscillator.Arrays[1], sc.Index, LongMALength.GetInt());
	sc.ExponentialMovAvg(Oscillator.Arrays[0], Oscillator.Arrays[2], sc.Index, ShortMALength.GetInt());

	Oscillator[sc.Index] = (Oscillator.Arrays[2][sc.Index] - Oscillator.Arrays[1][sc.Index]) / Divisor.GetInt();
	Overbought[sc.Index] = (float)OverboughtValue.GetInt();
	Oversold[sc.Index] = (float)OversoldValue.GetInt();
}

/*==========================================================================*/
SCSFExport scsf_HistoricalVolatilityRatio(SCStudyInterfaceRef sc)
{
	SCSubgraphRef HVR = sc.Subgraph[0];
	SCFloatArrayRef TempLog = sc.Subgraph[0].Arrays[0];
	SCFloatArrayRef TempShort = sc.Subgraph[0].Arrays[1];
	SCFloatArrayRef TempLong = sc.Subgraph[0].Arrays[2];

	SCInputRef ShortLength = sc.Input[2];
	SCInputRef LongLength = sc.Input[3];

	SCFloatArrayRef Close = sc.Close;

	// Configuration
	if (sc.SetDefaults)
	{
		sc.GraphName = "Historical Volatility Ratio";
		
		HVR.Name = "HVR";
		HVR.DrawStyle = DRAWSTYLE_LINE;
		HVR.PrimaryColor = RGB(0,255,0);
		
		ShortLength.Name = "Short Length";
		ShortLength.SetInt(6);
		ShortLength.SetIntLimits(1, INT_MAX);
		
		LongLength.Name = "Long Length";
		LongLength.SetInt(100);
		LongLength.SetIntLimits(1, INT_MAX);

		//During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;

		sc.AutoLoop = 1;
		
		return;
	}
	
	const float BIAS_DIVISION_FLOAT = (float)10e-10;

	// Data Processing
		
	sc.DataStartIndex = max(LongLength.GetInt(), ShortLength.GetInt()) + 1;

	if (sc.Index < sc.DataStartIndex)
		return;
	
	TempLog[sc.Index] = logf(Close[sc.Index] / (Close[sc.Index - 1] + BIAS_DIVISION_FLOAT));

	sc.StdDeviation(TempLog, TempShort, sc.Index, ShortLength.GetInt());
	sc.StdDeviation(TempLog, TempLong, sc.Index, LongLength.GetInt());

	HVR[sc.Index] = TempShort[sc.Index] / (TempLong[sc.Index] + BIAS_DIVISION_FLOAT);
}

/*==========================================================================*/
SCSFExport scsf_CurrentPriceLine(SCStudyInterfaceRef sc)
{
	SCSubgraphRef CPL = sc.Subgraph[0];

	SCInputRef NumberOfBarsBack = sc.Input[0];
	SCInputRef InputData = sc.Input[1];
	SCInputRef Version = sc.Input[2];
	SCInputRef DisplayCPLOnBars = sc.Input[3];
	SCInputRef ProjectForward = sc.Input[4];
	SCInputRef UseLastFromCurrentQuoteData = sc.Input [5];
	
	if(sc.SetDefaults)
	{
		sc.GraphName = "Current Price Line";

		CPL.Name = "CPL";
		CPL.DrawStyle = DRAWSTYLE_LINE;
		CPL.PrimaryColor = RGB(0,255,0);
		CPL.DrawZeros = false;
		CPL.LineStyle = LINESTYLE_DOT;
		
		sc.GraphRegion = 0;

		NumberOfBarsBack.Name = "Number of Bars to Display On. 0 = Current Date";
		NumberOfBarsBack.SetInt(0);
		NumberOfBarsBack.SetIntLimits(0,MAX_STUDY_LENGTH);

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);
		
		DisplayCPLOnBars.Name = "Display Current Price Line on Chart Bars";
		DisplayCPLOnBars.SetYesNo(true);

		ProjectForward.Name = "Forward Project Current Price Line";
		ProjectForward.SetYesNo(false);

		UseLastFromCurrentQuoteData.Name = "Use Last Trade Price From Current Quote Data";
		UseLastFromCurrentQuoteData.SetYesNo(false);
		
		Version.SetInt(2);

		sc.AutoLoop = 0;

		//During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;
		return;
	}

	if (Version.GetInt() < 1)
		InputData.SetInputDataIndex(SC_LAST);

	if(Version.GetInt() < 2)
		DisplayCPLOnBars.SetYesNo(true);

	Version.SetInt(2);

	
	SCFloatArrayRef InputDataArray = sc.BaseData[InputData.GetInputDataIndex()];

	float LineValue;

	if(UseLastFromCurrentQuoteData.GetYesNo())
		LineValue = sc.LastTradePrice;
	else
		LineValue = InputDataArray[sc.ArraySize - 1];

	int &PriorLineStartIndex = sc.PersistVars->i1;
	
	if(DisplayCPLOnBars.GetYesNo())
	{

		int TodayDate = sc.BaseDateTimeIn[sc.ArraySize-1].GetDate();
		int BarsBack = NumberOfBarsBack.GetInt();

		
		if (BarsBack == 0)
		{
			for (int Index = sc.ArraySize - 1; sc.BaseDateTimeIn[Index].GetDate() == TodayDate && Index >= 0; Index--)
			{
				CPL[Index] = LineValue;
			}
		}
		else
		{
			int StartingIndex = sc.ArraySize - BarsBack;

			for (int Index = sc.UpdateStartIndex - BarsBack; Index < sc.ArraySize; ++Index)
			{
				if(Index >= StartingIndex)
					break;

				CPL[Index] = 0;
			}

			for (int Index = sc.ArraySize - 1; BarsBack > 0 && Index >=0; Index--)
			{
				CPL[Index] = LineValue;
				BarsBack--;
			}
		}

	}
	else if (PriorLineStartIndex != 0)
	{
		//Clear prior array elements for when projecting forward
		for(int Index = PriorLineStartIndex; Index < sc.ArraySize; Index ++)
			CPL[Index] = 0.0;
	}

	if (ProjectForward.GetYesNo())
	{
		CPL.ExtendedArrayElementsToGraph = 10;
		for(int Index = 0; Index < 10; Index++)
		{
			CPL[sc.ArraySize + Index] = LineValue;
		}

		PriorLineStartIndex = sc.ArraySize;
	}
	else
		CPL.ExtendedArrayElementsToGraph = 0;

}
/*==========================================================================*/
SCSFExport scsf_OutOfSequence(SCStudyInterfaceRef sc) 
{	
	SCSubgraphRef OOOT = sc.Subgraph[0];

	if(sc.SetDefaults)
	{
		sc.GraphName = "Out of Order Timestamps Detector";

		OOOT.Name = "OoOT";
		OOOT.PrimaryColor = RGB(255,0,0);
		OOOT.DrawStyle = DRAWSTYLE_BACKGROUND;
		OOOT.LineWidth = 5;
		OOOT.DrawZeros = false;

		sc.GraphRegion = 0;
		
		sc.DisplayStudyInputValues= false;

		sc.ScaleRangeType= SCALE_INDEPENDENT;

		//During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;

		sc.AutoLoop = 1;

		return;
	}
	
	sc.DataStartIndex = 0;
	if (sc.Index <= 0)
		return;

    double prevDateTime = sc.BaseDateTimeIn[sc.Index-1];
    double curDateTime = sc.BaseDateTimeIn[sc.Index];
            
    if(curDateTime < prevDateTime)
        OOOT.Data[sc.Index] = 1;
    else 
		OOOT.Data[sc.Index] = 0;
	
	return;
}

/************************************************************
*  MoneyFlowIndex
*
*		typical price:			(high + low + close )/3
*		money flow:				typical price * volume
*		money ratio:				positive money flow / negative money flow
************************************************************/
SCSFExport scsf_MoneyFlowIndex(SCStudyInterfaceRef sc)
{
	SCSubgraphRef MFI = sc.Subgraph[0];

	SCInputRef Length = sc.Input[2];
		
	if (sc.SetDefaults)
	{
		sc.GraphName = "Money Flow Index";
		sc.GraphRegion = 1;		
		sc.ValueFormat = 3;  

		MFI.DrawStyle = DRAWSTYLE_LINE;	
		MFI.Name   = "MFI";
		MFI.PrimaryColor = RGB(0,255,0);
		MFI.DrawZeros = true;
		
		Length.Name = "Length";
		Length.SetInt(14); 
		Length.SetIntLimits(1,MAX_STUDY_LENGTH);

		//During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;

		sc.AutoLoop = 1;

		return;
	}
	
	const double BIAS_DIVISION_DOUBLE = (double)10e-20;
	double typ_price,prev_typ_price, money_flow,volume, N_pos_mon_flow, N_neg_mon_flow, d_mfi, money_ratio;

	sc.DataStartIndex = Length.GetInt();
	//if (sc.Index < sc.DataStartIndex)
	//	return;

	N_pos_mon_flow=0;
	N_neg_mon_flow=0;
	prev_typ_price = (sc.Index - Length.GetInt() -1)== 0 ? 0	: sc.HLCAvg[sc.Index-Length.GetInt()];																		
	for(int nn= 0; nn < Length.GetInt();nn++)
	{
		volume			= sc.Volume[sc.Index-Length.GetInt()+1+nn];
		typ_price		= sc.HLCAvg[sc.Index-Length.GetInt()+1+nn];
		money_flow		= typ_price * volume;
		N_pos_mon_flow += typ_price > prev_typ_price ? money_flow : 0;
		N_neg_mon_flow += typ_price < prev_typ_price ? money_flow : 0;
		prev_typ_price	= typ_price;
	}
	money_ratio = N_pos_mon_flow / (N_neg_mon_flow+BIAS_DIVISION_DOUBLE);
	d_mfi = 100.0 - (100.0 / (1.0+money_ratio));
	MFI[sc.Index]	= (float)d_mfi;		
}


/************************************************************************/

SCSFExport scsf_VolumeWeightedAveragePrice(SCStudyInterfaceRef sc)
{
	SCSubgraphRef   VWAPSubgraph               = sc.Subgraph[0];
	SCFloatArrayRef CumulativePriceTimesVolume = VWAPSubgraph.Arrays[0];
	SCFloatArrayRef CumulativeVolume           = VWAPSubgraph.Arrays[1];
	SCFloatArrayRef SumOfStandardDeviationCalc = VWAPSubgraph.Arrays[2];

	SCSubgraphRef   SD1Top    = sc.Subgraph[1];
	SCSubgraphRef   SD1Bottom = sc.Subgraph[2];
	SCSubgraphRef   SD2Top    = sc.Subgraph[3];
	SCSubgraphRef   SD2Bottom = sc.Subgraph[4];
	SCSubgraphRef   SD3Top    = sc.Subgraph[5];
	SCSubgraphRef   SD3Bottom = sc.Subgraph[6];
	SCSubgraphRef   SD4Top    = sc.Subgraph[7];
	SCSubgraphRef   SD4Bottom = sc.Subgraph[8];
	

    SCInputRef InputData              = sc.Input[0];
    SCInputRef TimePeriodType         = sc.Input[3];	
	SCInputRef TimePeriodLength       = sc.Input[4];	
	SCInputRef Version                = sc.Input[5];	
	SCInputRef DistanceMultiplier     = sc.Input[6];
	SCInputRef KeepStdDevCalc         = sc.Input[7];
	SCInputRef ContinuousNonResetting = sc.Input[8];
	SCInputRef BaseOnUnderlyingData   = sc.Input[9];
	SCInputRef UseFixedOffset         = sc.Input[10];
	SCInputRef StartDateTime      = sc.Input[11];
	

	if(sc.SetDefaults)
	{
		sc.GraphName = "Volume Weighted Average Price";
		sc.GraphRegion = 0;
		sc.ValueFormat = VALUEFORMAT_INHERITED;
		sc.AutoLoop = 1;
		sc.ScaleRangeType=SCALE_SAMEASREGION;
		
		VWAPSubgraph.Name = "VWAP";
		VWAPSubgraph.DrawStyle = DRAWSTYLE_DASH;
		VWAPSubgraph.LineWidth = 2;
		VWAPSubgraph.PrimaryColor = RGB (237, 13, 255);
		VWAPSubgraph.DrawZeros = false;

		SD1Top.Name = "Top Band 1";
		SD1Top.DrawStyle = DRAWSTYLE_DASH;
		SD1Top.LineWidth = 2;
		SD1Top.PrimaryColor = RGB (192, 192, 192);
		SD1Top.DrawZeros = false;

		SD1Bottom.Name = "Bottom Band 1";
		SD1Bottom.DrawStyle = DRAWSTYLE_DASH;
		SD1Bottom.LineWidth = 2;
		SD1Bottom.PrimaryColor = RGB (192, 192, 192);
		SD1Bottom.DrawZeros = false;

		SD2Top.Name = "Top Band 2";
		SD2Top.DrawStyle = DRAWSTYLE_IGNORE;
		SD2Top.LineWidth = 2;
		SD2Top.PrimaryColor = RGB (192, 192, 192);
		SD2Top.DrawZeros = false;

		SD2Bottom.Name = "Bottom Band 2";
		SD2Bottom.DrawStyle = DRAWSTYLE_IGNORE;
		SD2Bottom.LineWidth = 2;
		SD2Bottom.PrimaryColor = RGB (192, 192, 192);
		SD2Bottom.DrawZeros = false;

		SD3Top.Name = "Top Band 3";
		SD3Top.DrawStyle = DRAWSTYLE_IGNORE;
		SD3Top.LineWidth = 2;
		SD3Top.PrimaryColor = RGB (192, 192, 192);
		SD3Top.DrawZeros = false;

		SD3Bottom.Name = "Bottom Band 3";
		SD3Bottom.DrawStyle = DRAWSTYLE_IGNORE;
		SD3Bottom.LineWidth = 2;
		SD3Bottom.PrimaryColor = RGB (192, 192, 192);
		SD3Bottom.DrawZeros = false;

		SD4Top.Name = "Top Band 4";
		SD4Top.DrawStyle = DRAWSTYLE_IGNORE;
		SD4Top.LineWidth = 2;
		SD4Top.PrimaryColor = RGB (192, 192, 192);
		SD4Top.DrawZeros = false;

		SD4Bottom.Name = "Bottom Band 4";
		SD4Bottom.DrawStyle = DRAWSTYLE_IGNORE;
		SD4Bottom.LineWidth = 2;
		SD4Bottom.PrimaryColor = RGB (192, 192, 192);
		SD4Bottom.DrawZeros = false;


		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);  

		TimePeriodType.Name = "Time Period Type";
		TimePeriodType.SetTimePeriodType(TIME_PERIOD_LENGTH_UNIT_DAYS);
		
		TimePeriodLength.Name = "Time Period Length";
		TimePeriodLength.SetInt(1);

		DistanceMultiplier.Name = "Std. Deviation Distance Multiplier/Fixed Offset";
		DistanceMultiplier.SetFloat(0.5);

		KeepStdDevCalc.Name = "Continuous Non Resetting Std. Deviation Lines";
		KeepStdDevCalc.SetYesNo(0);

		ContinuousNonResetting.Name = "Continuous Non Resetting Mode";
		ContinuousNonResetting.SetYesNo(0);

		BaseOnUnderlyingData.Name = "Base On Underlying Data";
		BaseOnUnderlyingData.SetYesNo(0);

		UseFixedOffset.Name = "Use Fixed Offset Instead of Std. Deviation";
		UseFixedOffset.SetYesNo(0);

		StartDateTime.Name = "Start Date-Time";
		StartDateTime.SetDateTime(0.0);

		Version.SetInt(3);

		//During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;

		return;
	}

	if(Version.GetInt() < 2)
	{
		TimePeriodType.SetTimePeriodType(TIME_PERIOD_LENGTH_UNIT_DAYS);
		TimePeriodLength.SetInt(1);

		Version.SetInt(2);
	}

	if(Version.GetInt() < 3)
	{
		DistanceMultiplier.SetFloat(0.5);

		Version.SetInt(3);
	}
	
	sc.DataStartIndex = 1;

	SCDateTime & PriorCurrentPeriodStartDateTime = sc.PersistVars->scdt1;

	if(sc.Index == 0)
	{
		PriorCurrentPeriodStartDateTime = 0.0;

		CumulativePriceTimesVolume[sc.Index] = 0;
		CumulativeVolume[sc.Index] = 0;
		SumOfStandardDeviationCalc[sc.Index] = 0;

		if (BaseOnUnderlyingData.GetYesNo())
			sc.MaintainVolumeAtPriceData = 1;
		else
			sc.MaintainVolumeAtPriceData = 0;
	}

	if (StartDateTime.GetDate() != 0 &&  sc.BaseDateTimeIn[sc.Index] < StartDateTime.GetDateTime())
		return;

	SCDateTime CurrentPeriodStartDateTime = sc.GetStartOfPeriodForDateTime(sc.BaseDateTimeIn[sc.Index], TimePeriodType.GetTimePeriodType(), TimePeriodLength.GetInt(), 0);

	if(!ContinuousNonResetting.GetYesNo() && CurrentPeriodStartDateTime != PriorCurrentPeriodStartDateTime)
	{
		CumulativePriceTimesVolume[sc.Index - 1] = 0;
		CumulativeVolume[sc.Index - 1] = 0;

		if (!KeepStdDevCalc.GetYesNo())
			SumOfStandardDeviationCalc[sc.Index - 1] = 0;
	}

	PriorCurrentPeriodStartDateTime = CurrentPeriodStartDateTime;

	float CurrentPrice = sc.BaseDataIn[InputData.GetInputDataIndex()][sc.Index];

	if (BaseOnUnderlyingData.GetYesNo())
	{
		if ((int)sc.VolumeAtPriceForBars->GetNumberOfBars() < sc.ArraySize)
			return;

		int CurrentVAPPriceInTicks = INT_MIN; // This will be incremented from the lowest to highest price tick in the bar
		const s_VolumeAtPriceV2 *p_VolumeAtPrice;

		CumulativePriceTimesVolume[sc.Index] = CumulativePriceTimesVolume[sc.Index - 1];
		CumulativeVolume[sc.Index] = CumulativeVolume[sc.Index -1];

		while (sc.VolumeAtPriceForBars->GetNextHigherVAPElement(sc.Index, CurrentVAPPriceInTicks, &p_VolumeAtPrice))
		{
			float Price = p_VolumeAtPrice->PriceInTicks * sc.TickSize;

			CumulativePriceTimesVolume[sc.Index] += (Price * p_VolumeAtPrice->Volume);
			CumulativeVolume[sc.Index] += p_VolumeAtPrice->Volume;
		}
	}
	else
	{
		CumulativePriceTimesVolume[sc.Index] = CumulativePriceTimesVolume[sc.Index - 1] + sc.Volume[sc.Index] * CurrentPrice;
		CumulativeVolume[sc.Index] = CumulativeVolume[sc.Index -1] + sc.Volume[sc.Index];
	}

	
	float VolumeWeightedAveragePrice = 0.0f;
	if (CumulativeVolume[sc.Index] != 0.0f)
		VolumeWeightedAveragePrice = CumulativePriceTimesVolume[sc.Index] / CumulativeVolume[sc.Index];

	VWAPSubgraph[sc.Index] = VolumeWeightedAveragePrice;

	float Diff = CurrentPrice - VolumeWeightedAveragePrice;
	SumOfStandardDeviationCalc[sc.Index] = Diff * Diff * sc.Volume[sc.Index] + SumOfStandardDeviationCalc[sc.Index - 1];

	float StdDevDistance;
	if (UseFixedOffset.GetYesNo())
		StdDevDistance = DistanceMultiplier.GetFloat();
	else
		StdDevDistance = CumulativeVolume[sc.Index] <= 0 ? 0 :
						 sqrt(SumOfStandardDeviationCalc[sc.Index] / CumulativeVolume[sc.Index]) * DistanceMultiplier.GetFloat();

	SD1Top[sc.Index]    = VWAPSubgraph[sc.Index] + 1 * StdDevDistance;
	SD1Bottom[sc.Index] = VWAPSubgraph[sc.Index] - 1 * StdDevDistance;

	SD2Top[sc.Index]    = VWAPSubgraph[sc.Index] + 2 * StdDevDistance;
	SD2Bottom[sc.Index] = VWAPSubgraph[sc.Index] - 2 * StdDevDistance;

	SD3Top[sc.Index]    = VWAPSubgraph[sc.Index] + 3 * StdDevDistance;
	SD3Bottom[sc.Index] = VWAPSubgraph[sc.Index] - 3 * StdDevDistance;

	SD4Top[sc.Index]    = VWAPSubgraph[sc.Index] + 4 * StdDevDistance;
	SD4Bottom[sc.Index] = VWAPSubgraph[sc.Index] - 4 * StdDevDistance;
}

/**********************************************************************************************************************/
SCSFExport scsf_MovingLinearRegressionLine(SCStudyInterfaceRef sc)
{
	SCSubgraphRef MLR = sc.Subgraph[0];

	SCInputRef InputData = sc.Input[0];
	SCInputRef Length = sc.Input[2];

	if(sc.SetDefaults)
	{
		sc.GraphName = "Moving Linear Regression";

		sc.GraphRegion = 0;
		sc.ValueFormat = 3;  
		sc.AutoLoop = 1;

		MLR.Name = "MLR";
		MLR.DrawStyle = DRAWSTYLE_LINE;
		MLR.PrimaryColor = RGB(0,255,0);
		MLR.DrawZeros = false;
		
		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST); 
		
		Length.Name     = "Length";
		Length.SetInt(14);
		Length.SetIntLimits(1, MAX_STUDY_LENGTH);


		//During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;

		return;
	}

	sc.DataStartIndex = Length.GetInt()-1;	
	
	sc.LinearRegressionIndicator(sc.BaseDataIn[InputData.GetInputDataIndex()], MLR,  Length.GetInt());		
}
/************************************************************/

SCSFExport scsf_MACD(SCStudyInterfaceRef sc)
{
	SCSubgraphRef MACD		= sc.Subgraph[0];
	SCSubgraphRef MovAvgOfMACD	= sc.Subgraph[1];
	SCSubgraphRef MACDDiff	= sc.Subgraph[2];
	SCSubgraphRef RefLine	= sc.Subgraph[3];

	SCInputRef InputData	= sc.Input[0];
	SCInputRef FastLength		= sc.Input[3];
	SCInputRef SlowLength		= sc.Input[4];
	SCInputRef MACDLength		= sc.Input[5];
	SCInputRef MovingAverageType		= sc.Input[6];


	if(sc.SetDefaults)
	{
		sc.GraphName = "MACD";
		sc.AutoLoop					= 1;

		sc.GraphRegion				= 1;
		sc.ValueFormat				= 3;
		
		MACD.Name = "MACD";
		MACD.DrawStyle = DRAWSTYLE_LINE;
		MACD.DrawZeros = true;
		MACD.PrimaryColor = RGB(0,255,0);

		MovAvgOfMACD.Name	= "MA of MACD";
		MovAvgOfMACD.DrawStyle = DRAWSTYLE_LINE;
		MovAvgOfMACD.DrawZeros = true;
		MovAvgOfMACD.PrimaryColor = RGB(255,0,255);

		MACDDiff.Name = "MACD Diff";
		MACDDiff.DrawStyle = DRAWSTYLE_BAR;
		MACDDiff.DrawZeros = true;
		MACDDiff.PrimaryColor = RGB(255,255,0);

		RefLine.Name = "Line";
		RefLine.DrawStyle = DRAWSTYLE_LINE;
		RefLine.DrawZeros = true;
		RefLine.PrimaryColor = RGB(255,127,0);

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST); 

		FastLength.Name ="Fast Moving Average Length";
		FastLength.SetInt(12);  
		FastLength.SetIntLimits(1,MAX_STUDY_LENGTH);

		SlowLength.Name = "Slow Moving Average Length";
		SlowLength.SetInt(26);  
		SlowLength.SetIntLimits(1,MAX_STUDY_LENGTH);

		MACDLength.Name = "MACD Moving Average Length";
		MACDLength.SetInt(9);   
		MACDLength.SetIntLimits(1,MAX_STUDY_LENGTH);

		MovingAverageType.Name = "Moving Average Type";
		MovingAverageType.SetMovAvgType(MOVAVGTYPE_EXPONENTIAL);

		return;
	}

	if (MovingAverageType.GetMovAvgType()==MOVAVGTYPE_EXPONENTIAL)
			sc.DataStartIndex = 2;
	else
		sc.DataStartIndex = MACDLength.GetInt() + max(FastLength.GetInt(), SlowLength.GetInt());

	sc.MACD(sc.BaseDataIn[InputData.GetInputDataIndex()], MACD, sc.Index, FastLength.GetInt(), SlowLength.GetInt(), MACDLength.GetInt(), MovingAverageType.GetInt()); 

	MovAvgOfMACD[sc.Index]	= MACD.Arrays[2][sc.Index];
	MACDDiff[sc.Index]	= MACD.Arrays[3][sc.Index];
	RefLine[sc.Index]	= 0;	
}
/************************************************************/
SCSFExport scsf_StandardErrorBands(SCStudyInterfaceRef sc)
{
	SCSubgraphRef UpperBand = sc.Subgraph[0];
	SCSubgraphRef MidBand = sc.Subgraph[1];
	SCSubgraphRef LowerBand = sc.Subgraph[2];
	

	SCInputRef InputData = sc.Input[0];
	SCInputRef Length = sc.Input[2];
	SCInputRef StandardDeviations = sc.Input[3];

	if(sc.SetDefaults)
	{
		sc.GraphName = "Standard Error Bands";
		sc.GraphRegion = 0;
		sc.ValueFormat = 3;  
		
		MidBand.Name = "Middle Band";
		MidBand.DrawStyle = DRAWSTYLE_LINE;
		MidBand.DrawZeros = false;
		MidBand.PrimaryColor = RGB(255,0,255);

		LowerBand.Name = "Bottom Band";
		LowerBand.DrawStyle = DRAWSTYLE_LINE;
		LowerBand.DrawZeros = false;
		LowerBand.PrimaryColor = RGB(255,255,0);

		UpperBand.Name = "Top Band";
		UpperBand.DrawStyle = DRAWSTYLE_LINE;
		UpperBand.DrawZeros = false;
		UpperBand.PrimaryColor = RGB(0,255,0);
		
		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);  
		
		Length.Name = "Length";
		Length.SetInt(14);
		Length.SetIntLimits(1,MAX_STUDY_LENGTH);
		
		StandardDeviations.Name = "Standard Deviations";
		StandardDeviations.SetFloat(2.0f);
		StandardDeviations.SetFloatLimits(.001f,(float)MAX_STUDY_LENGTH);
		

		//During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;

		sc.AutoLoop = 1;

		return;
	}

	sc.DataStartIndex = Length.GetInt()-1;	



	sc.LinearRegressionIndicator(sc.BaseDataIn[InputData.GetInputDataIndex()], MidBand, Length.GetInt());		

	double StandardErrorValue = sc.GetStandardError(sc.BaseDataIn[InputData.GetInputDataIndex()], Length.GetInt());
	UpperBand[sc.Index] = (float)(MidBand[sc.Index] + StandardErrorValue * StandardDeviations.GetFloat());
	LowerBand[sc.Index] = (float)(MidBand[sc.Index] - StandardErrorValue * StandardDeviations.GetFloat());
}

/***********************************************************************************************************************/
SCSFExport scsf_BollingerBands(SCStudyInterfaceRef sc)
{
	SCSubgraphRef UpperBand = sc.Subgraph[0];
	SCSubgraphRef MidBand = sc.Subgraph[1];
	SCSubgraphRef LowerBand = sc.Subgraph[2];

	SCInputRef InputData = sc.Input[0];
	SCInputRef Length = sc.Input[3];
	SCInputRef StandardDeviations = sc.Input[4];
	SCInputRef VersionCheck = sc.Input[5];
	SCInputRef MAType = sc.Input[6];

	if(sc.SetDefaults)
	{
		sc.GraphName = "Bollinger Bands";
		sc.GraphRegion = 0;
		sc.ValueFormat = 3;  
		sc.AutoLoop = true;
		
		MidBand.Name = "Middle Band";
		MidBand.DrawStyle = DRAWSTYLE_LINE;
		MidBand.PrimaryColor = RGB(255,0,255);

		UpperBand.Name = "Upper Band";
		UpperBand.DrawStyle = DRAWSTYLE_LINE;
		UpperBand.PrimaryColor = RGB(0,255,0);

		LowerBand.Name = "Lower Band";
		LowerBand.DrawStyle = DRAWSTYLE_LINE;
		LowerBand.PrimaryColor = RGB(255,255,0);
		
		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_HLC);
		
		Length.Name = "Length";
		Length.SetInt(10);
		Length.SetIntLimits(1,MAX_STUDY_LENGTH);
		
		StandardDeviations.Name = "Standard Deviations";
		StandardDeviations.SetFloat(2.0f);
		StandardDeviations.SetFloatLimits(.0001f,(float)MAX_STUDY_LENGTH);

		VersionCheck.SetInt(1);

		MAType.Name = "Moving Average Type";
		MAType.SetMovAvgType(MOVAVGTYPE_SIMPLE);

		//During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;

		return;
	}

	// version check
	if (VersionCheck.GetInt() != 1)
	{
		MAType.SetMovAvgType(MOVAVGTYPE_SIMPLE);
		VersionCheck.SetInt(1);
	}

	sc.DataStartIndex = Length.GetInt()-1; 
	
	sc.BollingerBands(sc.BaseDataIn[InputData.GetInputDataIndex()],MidBand,Length.GetInt(),StandardDeviations.GetFloat(),MAType.GetMovAvgType());

	UpperBand[sc.Index] = MidBand.Arrays[0][sc.Index];
	LowerBand[sc.Index] = MidBand.Arrays[1][sc.Index];
}

/************************************************************************************************************************/
SCSFExport scsf_PositiveVolumeIndex(SCStudyInterfaceRef sc)
{
	SCSubgraphRef PVI = sc.Subgraph[0];

	SCInputRef InputData = sc.Input[0];
	SCInputRef InitialValue = sc.Input[2];
	
	if(sc.SetDefaults)
	{
		sc.GraphName = "Positive Volume Index";
		sc.GraphRegion = 1;
		sc.ValueFormat = 3;
		sc.AutoLoop = 1; // true
		
		PVI.Name = "PVI"; 
		PVI.DrawStyle = DRAWSTYLE_LINE;
		PVI.PrimaryColor = RGB(0,255,0);
		PVI.DrawZeros = false;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);
		
		InitialValue.Name = "Initial Value";
		InitialValue.SetFloat(2.5f);
		InitialValue.SetFloatLimits(.01f,(float)MAX_STUDY_LENGTH);

		//During development set this flag to 1, so the DLL can be modified. When developmsent is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;

		return;
	}
	
	float prev_PVI;
	float prev_data;
	float data;
	float PVIval;
	float volume;
	float prev_volume;

	sc.DataStartIndex = 0;
	
	int pos = sc.Index;
	if(pos == 0)// We are starting a new PVI study
	{
		PVI[0] = InitialValue.GetFloat();
		pos++;
	}

	volume = sc.Volume[pos]; 
	prev_volume = sc.Volume[pos-1];
	prev_PVI = PVI[pos-1];
	if(volume > prev_volume) // current volume less then equal to previous volume. 
	{
		data = sc.BaseDataIn[InputData.GetInputDataIndex()][pos]; 
		prev_data = sc.BaseDataIn[InputData.GetInputDataIndex()][pos-1];							
		PVIval = prev_PVI + ((data-prev_data) / prev_data * prev_PVI );
	}
	else
	{
		PVIval = prev_PVI;				
	}

	PVI[pos] = PVIval;
}
/************************************************************************************************************************/
SCSFExport scsf_NegativeVolumeIndex(SCStudyInterfaceRef sc)
{
	SCSubgraphRef NVI = sc.Subgraph[0];

	SCInputRef InputData = sc.Input[0];
	SCInputRef InitialValue = sc.Input[2];

	if(sc.SetDefaults)
	{
		sc.GraphName = "Negative Volume Index";

		sc.GraphRegion = 1;
		sc.ValueFormat = 3;
		sc.AutoLoop= true;

		NVI.Name = "NVI";
		NVI.DrawStyle = DRAWSTYLE_LINE;
		NVI.PrimaryColor = RGB(0,255,0);
		NVI.DrawZeros = false;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);  

		InitialValue.Name = "Initial Value";
		InitialValue.SetFloat(2.5f);
		InitialValue.SetFloatLimits(0.01f,(float)MAX_STUDY_LENGTH);

		//During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;

		return;
	}

	sc.DataStartIndex = 0;

	float prev_NVI;
	float prev_data;
	float data;
	float NVIval;
	float volume;
	float prev_volume;

	int pos = sc.Index;
	if(pos == 0)// We are starting a new NVI study
	{
		NVI[0] = InitialValue.GetFloat();
		pos++;
	}

	volume = sc.Volume[pos]; 
	prev_volume	= sc.Volume[pos-1];
	prev_NVI = NVI[pos-1];
	if(volume < prev_volume) // current volume greater then equal to previous volume. 
	{
		data = sc.BaseDataIn[InputData.GetInputDataIndex()][pos]; 
		prev_data = sc.BaseDataIn[InputData.GetInputDataIndex()][pos-1];							
		NVIval = prev_NVI + ((data-prev_data) / prev_data * prev_NVI);
	}
	else
	{
		NVIval	= prev_NVI;
	}

	NVI[pos] = NVIval;
}
/************************************************************************************************************************/
SCSFExport scsf_DoubleStochasticNew(SCStudyInterfaceRef sc)
{
	SCSubgraphRef DS = sc.Subgraph[0];
	SCSubgraphRef Line1 = sc.Subgraph[5];
	SCSubgraphRef Line2 = sc.Subgraph[6];
		
	SCInputRef Length = sc.Input[0];
	SCInputRef EMALength = sc.Input[2];
	SCInputRef MovAvgType = sc.Input[3];
	SCInputRef Line1Value = sc.Input[4];
	SCInputRef Line2Value = sc.Input[5];

	if(sc.SetDefaults)
	{
		sc.GraphName = "Double Stochastic";
		sc.GraphRegion = 1;
		sc.ValueFormat = 3;  
		sc.AutoLoop = true;

		DS.Name = "DS";
		DS.DrawStyle = DRAWSTYLE_LINE;
		DS.PrimaryColor = RGB(0,255,0);
		DS.DrawZeros = false;
		
		Line1.Name = "Line 1";
		Line1.DrawStyle = DRAWSTYLE_LINE;
		Line1.PrimaryColor = RGB(0,255,0);
		Line1.DrawZeros = false;
		
		Line2.Name = "Line 2";
		Line2.DrawStyle = DRAWSTYLE_LINE;
		Line2.PrimaryColor = RGB(0,255,0);
		Line2.DrawZeros = false;

		Length.Name = "Length";
		Length.SetInt(10);
		Length.SetIntLimits(1,MAX_STUDY_LENGTH);
		
		EMALength.Name = "Moving Average Length";
		EMALength.SetInt(3);
		EMALength.SetIntLimits(1,MAX_STUDY_LENGTH);

		MovAvgType.Name = "Moving Average Type";
		MovAvgType.SetMovAvgType(MOVAVGTYPE_EXPONENTIAL);
		
		Line1Value.Name = "Line 1";
		Line1Value.SetInt(10);
		
		Line2Value.Name = "Line 2";
		Line2Value.SetInt(85);

		// During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;
		
		return;
	}

	sc.DoubleStochastic(sc.BaseDataIn, DS, Length.GetInt(), EMALength.GetInt(), MovAvgType.GetMovAvgType());

	Line1[sc.Index] = (float)Line1Value.GetInt();
	Line2[sc.Index] = (float)Line2Value.GetInt();
}

/*==========================================================================*/
SCSFExport scsf_CandlesNoTails(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Open = sc.Subgraph[0];
	SCSubgraphRef Last = sc.Subgraph[3];
	SCSubgraphRef Volume = sc.Subgraph[4];
	SCSubgraphRef NumTrades = sc.Subgraph[5];
	SCSubgraphRef OHLCAvg = sc.Subgraph[6];
	SCSubgraphRef HLCAvg = sc.Subgraph[7];
	SCSubgraphRef HLAvg = sc.Subgraph[8];
	SCSubgraphRef BidVol = sc.Subgraph[9];
	SCSubgraphRef AskVol = sc.Subgraph[10];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults
		
		sc.GraphName = "Candles without Tails";
	
		sc.GraphDrawType = GDT_CANDLESTICK;
		sc.StandardChartHeader = 1;
		sc.DisplayAsMainPriceGraph = 1;
		sc.GraphRegion = 0;
			
		Open.Name = "Open";
		Open.DrawStyle = DRAWSTYLE_LINE;
		Open.PrimaryColor = RGB(0,255,0);
		Open.DrawZeros = false;
 		
		Last.Name = "Last";
		Last.DrawStyle = DRAWSTYLE_LINE;
		Last.PrimaryColor = RGB(255,127,0);
		Last.DrawZeros = false;

		Volume.Name = "Volume";
		Volume.DrawStyle = DRAWSTYLE_LINE;
		Volume.PrimaryColor = RGB(255,0,0);
		Volume.DrawZeros = false;

		NumTrades.Name = "# of Trades / OI";
		NumTrades.DrawStyle = DRAWSTYLE_IGNORE;
		NumTrades.PrimaryColor = RGB(0,0,255);
		NumTrades.DrawZeros = false;

		OHLCAvg.Name = "OHLC Avg";
		OHLCAvg.DrawStyle = DRAWSTYLE_IGNORE;
		OHLCAvg.PrimaryColor = RGB(127,0,255);
		OHLCAvg.DrawZeros = false;

		HLCAvg.Name = "HLC Avg";
		HLCAvg.DrawStyle = DRAWSTYLE_IGNORE;
		HLCAvg.PrimaryColor = RGB(0,255,255);
		HLCAvg.DrawZeros = false;

		HLAvg.Name = "HL Avg";
		HLAvg.DrawStyle = DRAWSTYLE_IGNORE;
		HLAvg.PrimaryColor = RGB(0,127,255);
		HLAvg.DrawZeros = false;

		BidVol.Name = "Bid Vol";
		BidVol.DrawStyle = DRAWSTYLE_IGNORE;
		BidVol.PrimaryColor = RGB(0,255,0);
		BidVol.DrawZeros = false;

		AskVol.Name = "Ask Vol";
		AskVol.DrawStyle = DRAWSTYLE_IGNORE;
		AskVol.PrimaryColor = RGB(0,255,0);
		AskVol.DrawZeros = false;

		//During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;

		sc.AutoLoop = 1;
		
		return;
	}
	//create a new name that includes the base graph name.
	sc.GraphName.Format("%s No Tails", sc.GetStudyName(0).GetChars());
	
	// Do data processing
	
	Open[sc.Index] = sc.Open[sc.Index];
	Last[sc.Index] = sc.Close[sc.Index];
	Volume[sc.Index] = sc.Volume[sc.Index];
	NumTrades[sc.Index] = sc.NumberOfTrades[sc.Index];
}

/**************************************************************************************************/
SCSFExport scsf_KiwisTrailingStop(SCStudyInterfaceRef sc)
{	
	SCSubgraphRef TSUp = sc.Subgraph[0];
	SCSubgraphRef TSDown = sc.Subgraph[1];	

	SCSubgraphRef direction = sc.Subgraph[3];
	SCSubgraphRef hiclose = sc.Subgraph[4];
	SCSubgraphRef loclose = sc.Subgraph[5];

	SCFloatArrayRef e1 = sc.Subgraph[6];
	SCFloatArrayRef e2 = sc.Subgraph[7];
	SCFloatArrayRef e3 = sc.Subgraph[8];
	SCFloatArrayRef e4 = sc.Subgraph[9];
	SCFloatArrayRef e5 = sc.Subgraph[10];
	SCFloatArrayRef e6 = sc.Subgraph[11];

	SCSubgraphRef TempRange = sc.Subgraph[6];
	SCSubgraphRef TempAvgRange = sc.Subgraph[7];
	SCSubgraphRef diff2 = sc.Subgraph[8];
	SCSubgraphRef atrmod = sc.Subgraph[9];

	SCInputRef Use2Subgraphs = sc.Input[0];
	SCInputRef ATRLength = sc.Input[2];
	SCInputRef ATRFactor = sc.Input[3];
	SCInputRef Vervoort = sc.Input[4];
	SCInputRef ATRMax = sc.Input[5];



	if(sc.SetDefaults)
	{
		sc.GraphName="Kiwi's Trailing Stop";
		sc.StudyDescription = "Kiwi's Trailing Stop (Chandelier) plus Sylvain Vervoort variation";

		TSUp.Name="TS";
		TSUp.DrawStyle = DRAWSTYLE_LINE_SKIPZEROS;
		TSUp.SecondaryColorUsed = 1;
		TSUp.PrimaryColor = RGB(0,255,0);
		TSUp.SecondaryColor = RGB(255,0,0);
		TSUp.DrawZeros = false;

		TSDown.Name = "TS";
		TSDown.DrawStyle = DRAWSTYLE_LINE_SKIPZEROS;
		TSDown.PrimaryColor = RGB(255,0,0);
		TSDown.DrawZeros = false;

		TempRange.Name = "TR";
		TempRange.DrawStyle = DRAWSTYLE_IGNORE;
		TempRange.PrimaryColor = RGB(127,0,255);
		TempRange.DrawZeros = false;

		TempAvgRange.Name = "TAR";
		TempAvgRange.DrawStyle = DRAWSTYLE_IGNORE;
		TempAvgRange.PrimaryColor = RGB(0,255,255);
		TempAvgRange.DrawZeros = false;

		diff2.Name = "diff2";
		diff2.DrawStyle = DRAWSTYLE_IGNORE;
		diff2.PrimaryColor = RGB(0,127,255);
		diff2.DrawZeros = false;

		atrmod.Name = "atrmod";
		atrmod.DrawStyle = DRAWSTYLE_IGNORE;
		atrmod.PrimaryColor = RGB(0,255,0);
		atrmod.DrawZeros = false;

		sc.AutoLoop = 0;

		ATRLength.Name="ATR Length";
		ATRLength.SetFloat(9.0);

		ATRFactor.Name="ATR Factor";
		ATRFactor.SetFloat(2.5);

		Vervoort.Name="Vervoort Variation?";
		Vervoort.SetYesNo(false);

		ATRMax.Name="Vervoort Maximum Range";
		ATRMax.SetFloat(1.5);

		Use2Subgraphs.Name="Use 2 subgraphs";
		Use2Subgraphs.SetYesNo(true);

		sc.GraphRegion = 0;

		return;
	}

	int 	pos;
	float 	Ave;
	float  	b,b2,b3,c1,c2,c3,c4,w1=0,w2=0;

	sc.DataStartIndex=10;

	if(Use2Subgraphs.GetYesNo())
	{
		TSUp.Name = "TS Up";
		TSDown.Name = "TS Down";

		TSUp.SecondaryColorUsed = false;
	}
	else
	{
		TSUp.Name="TS";

		TSUp.SecondaryColorUsed = 1;
	}


	if (sc.UpdateStartIndex == 0)
	{
		direction[sc.UpdateStartIndex] = 1;
		loclose[sc.UpdateStartIndex] = sc.Close[sc.UpdateStartIndex]; 
		hiclose[sc.UpdateStartIndex] = loclose[sc.UpdateStartIndex];

		e1[sc.UpdateStartIndex]=sc.High[sc.UpdateStartIndex]-sc.Low[sc.UpdateStartIndex]; 
		e2[sc.UpdateStartIndex]=e1[sc.UpdateStartIndex]; 
		e3[sc.UpdateStartIndex]=e1[sc.UpdateStartIndex]; 
		e4[sc.UpdateStartIndex]=e1[sc.UpdateStartIndex]; 
		e5[sc.UpdateStartIndex]=e1[sc.UpdateStartIndex]; 
		e6[sc.UpdateStartIndex]=e1[sc.UpdateStartIndex];
	}

	if(!Vervoort.GetYesNo()) 
	{
		b=0.5;					// 0.5
		b2=(b*b);				// 0.25
		b3=(b2*b);				// 0.125

		c1=-b3;					// - 0.125
		c2=(3*(b2+b3));			// 0.45
		c3=(-3*(2*b2+b+b3)); 
		c4=(1+3*b+b3+3*b2);
		w1 = 2/(ATRLength.GetFloat()+1);  
		w2 = 1-w1;

		for (pos=max(sc.UpdateStartIndex, 1); pos < sc.ArraySize; pos++)
		{
			float temp = max(sc.High[pos]-sc.Low[pos], sc.High[pos]-sc.Close[pos-1]);
			float P   = max(temp,sc.Close[pos-1]-sc.Low[pos]);

			e1[pos] = w1*P + w2*e1[pos - 1];
			e2[pos] = w1*e1[pos] + w2*e2[pos - 1];
			e3[pos] = w1*e2[pos] + w2*e3[pos - 1];
			e4[pos] = w1*e3[pos] + w2*e4[pos - 1];
			e5[pos] = w1*e4[pos] + w2*e5[pos - 1];
			e6[pos] = w1*e5[pos] + w2*e6[pos - 1];

			Ave = c1*e6[pos] + c2*e5[pos] + c3*e4[pos] + c4*e3[pos];

			if((direction[pos-1]==1 && sc.Close[pos]<(loclose[pos-1]))
				|| 
				(direction[pos-1]==-1 && sc.Close[pos]>(hiclose[pos-1])))
			{
				if(direction[pos-1]==1)
				{
					direction[pos] = -1; //reverse short
					hiclose[pos] = sc.Close[pos]+(Ave*ATRFactor.GetFloat());
					loclose[pos] = 0;
				} 
				else
				{
					direction[pos] = 1; //reverse long
					loclose[pos] = sc.Close[pos]-(Ave*ATRFactor.GetFloat());
					hiclose[pos] = 0;
				}
			} 
			else
			{
				if(direction[pos-1]==1)
				{
					if(sc.Close[pos]-(Ave*ATRFactor.GetFloat())>loclose[pos-1])
					{
						loclose[pos] = sc.Close[pos]-(Ave*ATRFactor.GetFloat());
						hiclose[pos] = hiclose[pos-1];
					}
					else
					{
						loclose[pos] = loclose[pos-1];
						hiclose[pos] = hiclose[pos-1];
					}
				} 
				else
				{
					if(sc.Close[pos]+(Ave*ATRFactor.GetFloat())<hiclose[pos-1])
					{
						loclose[pos] = loclose[pos-1];
						hiclose[pos] = sc.Close[pos]+(Ave*ATRFactor.GetFloat());
					}
					else
					{
						loclose[pos] = loclose[pos-1];
						hiclose[pos] = hiclose[pos-1];
					}
				};

				direction[pos] = direction[pos-1]; // no change
			};

			if(direction[pos]==1) 
			{
				if(Use2Subgraphs.GetYesNo() == false)
				{
					TSUp[pos] = (loclose[pos]); 
					TSUp.DataColor[pos] = TSUp.PrimaryColor;
				}
				else
				{
					TSUp[pos] = (loclose[pos]); 
					TSDown[pos] = 0;
				}
			} 
			else
			{
				if(Use2Subgraphs.GetYesNo() == false)
				{
					TSUp[pos] = (hiclose[pos]); 
					TSUp.DataColor[pos] =  TSUp.SecondaryColor;
				}
				else
				{
					TSDown[pos] = (hiclose[pos]);  
					TSUp[pos] = 0;
				}
			}	
		}
	} else
	{

		/*	
		HiLo:=If(H-L<1.5*Mov(H-L,period,S),H-L, 1.5*Mov
		(H-L,period,S));
		Href:=If(L<=Ref(H,-1),H-Ref(C,-1),(H-Ref(C,-1))-(L-Ref(H,-1))/2);
		Lref:=If(H>=Ref(L,-1),Ref(C,-1)-L,(Ref(C,-1)-L)-(Ref(L,-1)-H)/2);
		diff1:=Max(HiLo,Href);
		diff2:=Max(diff1,Lref);
		atrmod:=Wilders(diff2,period);
		loss:=atrfact*atrmod;
		trail:= 
		If(C>PREV AND Ref(C,-1)>PREV,
		Max(PREV,C-loss),
		If(C<PREV AND Ref(C,-1)<PREV,
		Min(PREV,C+loss),
		If(C>PREV,C-loss,C+loss)));
		*/	


		for (pos=max(sc.UpdateStartIndex, 1); pos < sc.ArraySize; pos++)
		{
			float HiLo,Href,Lref;
			int period = (int)ATRLength.GetFloat();
			TempRange[pos] = sc.High[pos]-sc.Low[pos];
			sc.MovingAverage(TempRange, TempAvgRange, MOVAVGTYPE_SIMPLE, pos, period);

			HiLo=min(TempRange[pos],ATRMax.GetFloat()*TempAvgRange[pos]);
			if(sc.Low[pos]<=sc.High[pos-1]) Href=sc.High[pos]-sc.Close[pos-1]; else Href=(sc.High[pos]-sc.Close[pos-1])-(sc.Low[pos]-sc.High[pos-1])/2;
			if(sc.High[pos]>=sc.Low[pos-1]) Lref=sc.Close[pos-1]-sc.Low[pos];  else Lref=(sc.Close[pos-1]-sc.Low[pos])- (sc.Low[pos-1]-sc.High[pos])/2;

			diff2[pos] = max(Lref,max(HiLo,Href));
			sc.MovingAverage(diff2, atrmod, MOVAVGTYPE_WILDERS, pos, period);

			Ave = atrmod[pos];

			if((direction[pos-1]==1 && sc.Close[pos]<(loclose[pos-1]))
				|| 
				(direction[pos-1]==-1 && sc.Close[pos]>(hiclose[pos-1])))
			{
				if(direction[pos-1]==1)
				{
					direction[pos] = -1; //reverse short
					hiclose[pos] = sc.Close[pos]+(Ave*ATRFactor.GetFloat());
					loclose[pos] = 0;
				} 
				else
				{
					direction[pos] = 1; //reverse long
					loclose[pos] = sc.Close[pos]-(Ave*ATRFactor.GetFloat());
					hiclose[pos] = 0;
				}
			} 
			else
			{
				if(direction[pos-1]==1)
				{
					if(sc.Close[pos]-(Ave*ATRFactor.GetFloat())>loclose[pos-1])
					{
						loclose[pos] = sc.Close[pos]-(Ave*ATRFactor.GetFloat());
						hiclose[pos] = hiclose[pos-1];
					}
					else
					{
						loclose[pos] = loclose[pos-1];
						hiclose[pos] = hiclose[pos-1];
					}
				} 
				else
				{
					if(sc.Close[pos]+(Ave*ATRFactor.GetFloat())<hiclose[pos-1])
					{
						loclose[pos] = loclose[pos-1];
						hiclose[pos] = sc.Close[pos]+(Ave*ATRFactor.GetFloat());
					}
					else
					{
						loclose[pos] = loclose[pos-1];
						hiclose[pos] = hiclose[pos-1];
					}
				};

				direction[pos] = direction[pos-1]; // no change
			};

			if(direction[pos]==1) 
			{
				if(Use2Subgraphs.GetYesNo() == false)
				{
					TSUp[pos] = (loclose[pos]); 
					TSUp.DataColor[pos] = TSUp.PrimaryColor;
				}
				else
				{
					TSUp[pos] = (loclose[pos]); 
					TSDown[pos] = 0;
				}
			} 
			else
			{
				if(Use2Subgraphs.GetYesNo() == false)
				{
					TSUp[pos] = (hiclose[pos]); 
					TSUp.DataColor[pos] =  TSUp.SecondaryColor;
				}
				else
				{
					TSDown[pos] = (hiclose[pos]);  
					TSUp[pos] = 0;
				}
			}
		}
	}
}

/*============================================================================*/
SCSFExport scsf_LargeTextDisplayForStudyFromChart(SCStudyInterfaceRef sc)
{
	SCSubgraphRef TextDisplay = sc.Subgraph[0];

	SCInputRef HorizontalPosition = sc.Input[0];
	SCInputRef VerticalPosition = sc.Input[1];
	SCInputRef DisplayInFillSpace = sc.Input[2];
	SCInputRef StudySubgraphReference = sc.Input[3];
	SCInputRef DisplaySubgraphName = sc.Input[4];
	SCInputRef SubgraphOffset = sc.Input[5];

	
	if (sc.SetDefaults)
	{
		// Set the configuration and defaults
		
		sc.GraphName = "Text Display For Study from Chart";
		
		sc.AutoLoop = 0;
		sc.GraphRegion = 0;

		TextDisplay.Name = "Text Display";
		TextDisplay.LineWidth = 20;
		TextDisplay.DrawStyle = DRAWSTYLE_CUSTOM_TEXT;
		TextDisplay.PrimaryColor = RGB(0, 0, 0); //black
		TextDisplay.SecondaryColor = RGB(128, 255, 255);
		TextDisplay.SecondaryColorUsed = true;
		TextDisplay.DisplayNameValueInWindowsFlags = 0;

		HorizontalPosition.Name = "Horizontal Position From Left (1-150)";
		HorizontalPosition.SetInt(20);
		HorizontalPosition.SetIntLimits(1,150);

		VerticalPosition.Name = "Vertical Position From Bottom (1-100)";
		VerticalPosition.SetInt(90);
		VerticalPosition.SetIntLimits(1,100);

		DisplayInFillSpace.Name = "Display in Fill Space";
		DisplayInFillSpace.SetYesNo(false);

		StudySubgraphReference.Name = "Study And Subgraph To Display";
		StudySubgraphReference.SetChartStudySubgraphValues(1,1, 0);

		DisplaySubgraphName.Name = "Display Subgraph Name";
		DisplaySubgraphName.SetYesNo(false);

		SubgraphOffset.Name = "Subgraph Columns Back";
		SubgraphOffset.SetInt(0);

		sc.TextInputName= "Prefix Text for Display Value";

		
		// During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = false;

		return;
	}
	
	// Do data processing

	SCFloatArray StudyReference;
	sc.GetStudyArrayFromChartUsingID(StudySubgraphReference.GetChartStudySubgraphValues(), StudyReference);
	
	
	s_UseTool Tool;

	Tool.Clear(); // Reset tool structure for our next use. Unnecessary in this case, but good practice.
	Tool.ChartNumber = sc.ChartNumber;
	Tool.DrawingType = DRAWING_TEXT;
	
	Tool.LineNumber = 79212343+ (sc.StudyGraphInstanceID);
	if(!DisplayInFillSpace.GetYesNo() && !sc.Input[8].GetYesNo())
		Tool.BeginDateTime = HorizontalPosition.GetInt();
	else
		Tool.BeginDateTime = -3;

	Tool.Region = sc.GraphRegion;
	Tool.BeginValue = (float)VerticalPosition.GetInt();
	Tool.UseRelativeValue = true;
	Tool.Color = TextDisplay.PrimaryColor;
	Tool.FontBackColor = TextDisplay.SecondaryColor;
	Tool.FontBold = true;

	int ColumnsBack = SubgraphOffset.GetInt();
	SCString ValueText;
	if (sc.TextInput.GetLength()>0)
	{
		ValueText+= sc.TextInput;
		ValueText += " ";
	}

	 ValueText +=sc.FormatGraphValue(StudyReference[StudyReference.GetArraySize()-1-ColumnsBack], sc.GetValueFormat());
	Tool.Text = ValueText;
	
	Tool.FontSize = TextDisplay.LineWidth;
	Tool.AddMethod = UTAM_ADD_OR_ADJUST;
	Tool.ReverseTextColor = false;
	
	
	sc.UseTool(Tool);
}


/*============================================================================*/
SCSFExport scsf_LargeTextDisplayForStudy(SCStudyInterfaceRef sc)
{
	SCSubgraphRef TextDisplay = sc.Subgraph[0];

	SCInputRef HorizontalPosition = sc.Input[0];
	SCInputRef VerticalPosition = sc.Input[1];
	SCInputRef DisplayInFillSpace = sc.Input[2];
	SCInputRef StudySubgraphReference = sc.Input[3];
	SCInputRef DisplaySubgraphName = sc.Input[4];
	SCInputRef SubgraphOffset = sc.Input[5];


	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Text Display For Study";

		sc.AutoLoop = 0;
		sc.GraphRegion = 0;

		TextDisplay.Name = "Text Display";
		TextDisplay.LineWidth = 20;
		TextDisplay.DrawStyle = DRAWSTYLE_CUSTOM_TEXT;
		TextDisplay.PrimaryColor = RGB(0, 0, 0); //black
		TextDisplay.SecondaryColor = RGB(128, 255, 255);
		TextDisplay.SecondaryColorUsed = true;
		TextDisplay.DisplayNameValueInWindowsFlags = 0;

		HorizontalPosition.Name = "Horizontal Position From Left (1-150)";
		HorizontalPosition.SetInt(20);
		HorizontalPosition.SetIntLimits(1,150);

		VerticalPosition.Name = "Vertical Position From Bottom (1-100)";
		VerticalPosition.SetInt(90);
		VerticalPosition.SetIntLimits(1,100);

		DisplayInFillSpace.Name = "Display in Fill Space";
		DisplayInFillSpace.SetYesNo(false);

		StudySubgraphReference.Name = "Study And Subgraph To Display";
		StudySubgraphReference.SetStudySubgraphValues(1, 0);

		DisplaySubgraphName.Name = "Display Subgraph Name";
		DisplaySubgraphName.SetYesNo(false);

		SubgraphOffset.Name = "Subgraph Columns Back";
		SubgraphOffset.SetInt(0);

		sc.TextInputName= "Prefix Text for Display Value";


		// During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = false;

		return;
	}

	// Do data processing

	SCFloatArray StudyReference;
	sc.GetStudyArrayUsingID(StudySubgraphReference.GetStudyID(), StudySubgraphReference.GetSubgraphIndex(), StudyReference);


	s_UseTool Tool;

	Tool.Clear(); // Reset tool structure for our next use. Unnecessary in this case, but good practice.
	Tool.ChartNumber = sc.ChartNumber;
	Tool.DrawingType = DRAWING_TEXT;

	Tool.LineNumber = 79712343+ (sc.StudyGraphInstanceID);
	if(!DisplayInFillSpace.GetYesNo() && !sc.Input[8].GetYesNo())
		Tool.BeginDateTime = HorizontalPosition.GetInt();
	else
		Tool.BeginDateTime = -3;

	Tool.Region = sc.GraphRegion;
	Tool.BeginValue = (float)VerticalPosition.GetInt();
	Tool.UseRelativeValue = true;
	Tool.Color = TextDisplay.PrimaryColor;
	Tool.FontBackColor = TextDisplay.SecondaryColor;
	Tool.FontBold = true;

	int ColumnsBack = SubgraphOffset.GetInt();
	SCString ValueText;
	if (sc.TextInput.GetLength()>0)
	{
		ValueText+= sc.TextInput;
		ValueText += " ";
	}

	double StudyValue = StudyReference[StudyReference.GetArraySize()-1-ColumnsBack];
	ValueText +=sc.FormatGraphValue( StudyValue, sc.GetValueFormat());
	Tool.Text = ValueText;

	Tool.FontSize = TextDisplay.LineWidth;
	Tool.AddMethod = UTAM_ADD_OR_ADJUST;
	Tool.ReverseTextColor = false;


	sc.UseTool(Tool);
}

/*============================================================================ */
SCSFExport scsf_ProfitLossTextStudy(SCStudyInterfaceRef sc)
{
	SCInputRef HorizontalPosition = sc.Input[0];
	SCInputRef VerticalPosition   = sc.Input[1];
	SCInputRef DisplayInFillSpace = sc.Input[2];
	SCInputRef ProfitLossFormat   = sc.Input[3];
	SCInputRef RTCommission       = sc.Input[4];

	SCInputRef ShowOpenPL         = sc.Input[5];
	SCInputRef ShowClosedPL       = sc.Input[6];
	SCInputRef ShowDailyPL        = sc.Input[7];
	SCInputRef IncludeOpenPLInDailyPL = sc.Input[8];

	SCInputRef TextSize                = sc.Input[10];
	SCInputRef TextColor               = sc.Input[11];
	SCInputRef TextBackColor           = sc.Input[12];
	SCInputRef ShowTotalTrades         = sc.Input[13];
	SCInputRef ShowTotalQuantityFilled = sc.Input[14];
	SCInputRef ShowDailyTotalTrades    = sc.Input[15];
	SCInputRef ShowDailyTotalQuantityFilled = sc.Input[16];


	if (sc.SetDefaults)
	{
		// Set the configuration and defaults
		sc.GraphName = "Trading: Profit/Loss Text";

		sc.AutoLoop = 0;
		sc.GraphRegion = 0;
		sc.MaintainTradeStatisticsAndTradesData = 1;

		HorizontalPosition.Name = "Horizontal Position From Left (1-150)";
		HorizontalPosition.SetInt(20);
		HorizontalPosition.SetIntLimits(1,150);

		VerticalPosition.Name = "Vertical Position From Bottom (1-100)";
		VerticalPosition.SetInt(90);
		VerticalPosition.SetIntLimits(1,100);

		DisplayInFillSpace.Name = "Display in Fill Space";
		DisplayInFillSpace.SetYesNo(false);

		ShowOpenPL.Name = "Show Open Profit/Loss";
		ShowOpenPL.SetYesNo(1);

		ShowClosedPL.Name = "Show Closed Profit/Loss";
		ShowClosedPL.SetYesNo(1);

		ShowDailyPL.Name = "Show Daily Profit/Loss";
		ShowDailyPL.SetYesNo(1);

		IncludeOpenPLInDailyPL.Name = "Include Open Profit/Loss in Daily Profit/Loss";
		IncludeOpenPLInDailyPL.SetYesNo(0);

		TextSize.Name = "Text Size";
		TextSize.SetInt(14);
		TextSize.SetIntLimits(3, 50);

		TextColor.Name = "Text Color";
		TextColor.SetColor(0,0,0);  // black

		TextBackColor.Name = "Text Background Color";
		TextBackColor.SetColor(128,255,255);

		ShowTotalTrades.Name = "Show Total Trades";
		ShowTotalTrades.SetYesNo(0);

		ShowTotalQuantityFilled.Name = "Show Total Quantity Filled";
		ShowTotalQuantityFilled.SetYesNo(0);

		ShowDailyTotalTrades.Name = "Show Daily Total Trades";
		ShowDailyTotalTrades.SetYesNo(0);

		ShowDailyTotalQuantityFilled.Name = "Show Daily Total Quantity Filled";
		ShowDailyTotalQuantityFilled.SetYesNo(0);

		ProfitLossFormat.Name = "Profit/Loss Display Format";
		ProfitLossFormat.SetCustomInputStrings("Currency Value;Points (P);Points - Ignore Quantity (p);Ticks (T);Ticks - Ignore Quantity (t);Currency Value & Points (P);Currency Value & Points - Ignore Quantity (p);Currency Value & Ticks (T);Currency Value & Ticks - Ignore Quantity (t)");
		ProfitLossFormat.SetCustomInputIndex(0);

		RTCommission.Name = "Round Turn Commission To Include";
		RTCommission.SetFloat(0);

		// During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = false;

		return;
	}

	sc.SendOrdersToTradeService = !sc.GlobalTradeSimulationIsOn;

	// Do data processing
	s_UseTool Tool;

	Tool.Clear(); // Reset tool structure for our next use. Unnecessary in this case, but good practice.
	Tool.DrawingType = DRAWING_TEXT;
	Tool.ChartNumber = sc.ChartNumber;
	Tool.Region = sc.GraphRegion;
	Tool.LineNumber = 79295300+ (sc.StudyGraphInstanceID*100);
	Tool.AddMethod = UTAM_ADD_OR_ADJUST;
	if(DisplayInFillSpace.GetYesNo())
		Tool.BeginDateTime = -3;
	else
		Tool.BeginDateTime = HorizontalPosition.GetInt();
	Tool.BeginValue = (float)VerticalPosition.GetInt();
	Tool.UseRelativeValue = true;
	Tool.Color = TextColor.GetColor();
	Tool.FontBackColor = TextBackColor.GetColor();
	Tool.FontSize = TextSize.GetInt();
	Tool.FontBold = true;
	Tool.ReverseTextColor = false;
	Tool.MultiLineLabel = true;
	Tool.TextAlignment = DT_LEFT;

	PositionProfitLossDisplayEnum ProfitLossDisplayFormat = (PositionProfitLossDisplayEnum)(ProfitLossFormat.GetIndex()+1);

	s_SCPositionData PositionData;
	sc.GetTradePosition(PositionData) ;

	double RTCommissionRate = sc.CurrencyValuePerTick == 0 ? 0 : RTCommission.GetFloat();

	SCString Text;

	s_ACSTradeStatistics AllStats;
	s_ACSTradeStatistics DailyStats;

	sc.GetTradeStatisticsForSymbol(sc.GlobalTradeSimulationIsOn != 0, false, AllStats);
	sc.GetTradeStatisticsForSymbol(sc.GlobalTradeSimulationIsOn != 0, true, DailyStats);

	if (ShowOpenPL.GetYesNo())
	{
		SCString PLString;
		double Commission = RTCommissionRate * PositionData.PositionQuantity / 2.0;
		sc.CreateProfitLossDisplayString(PositionData.OpenProfitLoss-Commission, PositionData.PositionQuantity, ProfitLossDisplayFormat, PLString);
		Text.Format("Open PL: %s", PLString.GetChars());
	}

	if (ShowClosedPL.GetYesNo())
	{
		if (Text != "")
			Text.Append("\n");

		SCString PLString;
		double Commission = RTCommissionRate * AllStats.TotalClosedQuantity();
		sc.CreateProfitLossDisplayString(AllStats.ClosedProfitLoss()-Commission, AllStats.TotalClosedQuantity(), ProfitLossDisplayFormat, PLString);
		SCString ClosedPLText;
		ClosedPLText.Format("Closed PL: %s", PLString.GetChars());

		Text.Append(ClosedPLText);
	}

	if (ShowDailyPL.GetYesNo())
	{
		if (Text != "")
			Text.Append("\n");

		double DailyPL = DailyStats.ClosedProfitLoss();
		int    DailyClosedQuantity = DailyStats.TotalClosedQuantity();
		double Commission = RTCommissionRate * DailyClosedQuantity;

		if (IncludeOpenPLInDailyPL.GetYesNo())
		{
			DailyPL += PositionData.OpenProfitLoss;
			//DailyPL +=PositionData.TradeStatsOpenProfitLoss;
			int OpenQuantity = abs(PositionData.PositionQuantity);
			//int OpenQuantity = abs(PositionData.TradeStatsOpenQuantity);
			DailyClosedQuantity += OpenQuantity;
			Commission += (RTCommissionRate * OpenQuantity / 2.0);
			Text.Append("Daily Net PL: ");
		}
		else
			Text.Append("Daily PL: ");


		SCString PLString;
		sc.CreateProfitLossDisplayString(DailyPL-Commission, DailyClosedQuantity, ProfitLossDisplayFormat, PLString);


		Text.Append(PLString);
	}

	if (ShowTotalTrades.GetYesNo())
	{
		if (Text != "")
			Text.Append("\n");

		SCString TotalTradesText;
		TotalTradesText.Format("Total Trades: %i", AllStats.TotalTrades());

		Text.Append(TotalTradesText);
	}

	if (ShowTotalQuantityFilled.GetYesNo())
	{
		if (Text != "")
			Text.Append("\n");

		SCString TotalFilledText;
		TotalFilledText.Format("Total # Filled: %i", AllStats.TotalFilledQuantity);

		Text.Append(TotalFilledText);
	}

	if (ShowDailyTotalTrades.GetYesNo())
	{
		if (Text != "")
			Text.Append("\n");

		SCString TotalTradesText;
		TotalTradesText.Format("Daily Total Trades: %i", DailyStats.TotalTrades());

		Text.Append(TotalTradesText);
	}

	if (ShowDailyTotalQuantityFilled.GetYesNo())
	{
		if (Text != "")
			Text.Append("\n");

		SCString TotalFilledText;
		TotalFilledText.Format("Daily Total # Filled: %i", DailyStats.TotalFilledQuantity);

		Text.Append(TotalFilledText);
	}

	Tool.Text = Text;

	sc.UseTool(Tool);
}

/*==========================================================================*/
/*
   ColorNormLength( 14 ), { Number of bars over which to normalize the indicat or
	     for gradient coloring.  See also:  comments in function NormGradientColor. }
    UpColor( Yellow ), { Color to use for indicator values that are relatively high
		  over ColorNormLength bars. }
	 DnColor( Cyan ) ;  { Color to use for indicator values that are relatively low
		   over ColorNormLength bars. }
	    
	  { Set either UpColor and/or DnColor to -1 to disable gradient plot coloring.
		   When disabled, Plot1 color is determined by settings in indicator  properties
		    dialog box.  Plot2 (ZeroLine) color always comes from indicator properties
			    dialog box. }

	    { Gradient coloring }
		 if UpColor >= 0 and DnColor >= 0 then
			   begin
			     ColorLevel = NormGradientColor( VolRatioAvg, true, ColorNormLength, UpColor,
				    DnColor ) ;
		   SetPlotColor( 1, ColorLevel ) ;
		     end ;
*/

 /*==========================================================================*/
 SCSFExport scsf_UpDownVolumeRatio(SCStudyInterfaceRef sc)
 {
	 SCSubgraphRef UpDownVolumeRatio = sc.Subgraph[0];
	 SCSubgraphRef ZeroLine = sc.Subgraph[1];

	 SCInputRef MovingAverageLength = sc.Input[0];
	 SCInputRef VolumeOrTrades = sc.Input[1];
	 SCInputRef MovingAverageType = sc.Input [2];


	 if (sc.SetDefaults)
	 {
		 // Set the configuration and defaults

		 sc.GraphName = "Up/Down Volume Ratio";
		 sc.MaintainAdditionalChartDataArrays = 1;

		 sc.GraphRegion =1;

		 UpDownVolumeRatio.Name = "VR";
		 UpDownVolumeRatio.DrawStyle = DRAWSTYLE_BAR;
		 UpDownVolumeRatio.PrimaryColor = RGB(0, 255, 0);
		 UpDownVolumeRatio.SecondaryColor = RGB(255, 0, 0);
		 UpDownVolumeRatio.LineWidth = 3;
		 UpDownVolumeRatio.AutoColoring = AUTOCOLOR_POSNEG;
		 UpDownVolumeRatio.DrawZeros = false;  // false

		 ZeroLine.Name = "Zero";
		 ZeroLine.DrawStyle = DRAWSTYLE_LINE;
		 ZeroLine.PrimaryColor = RGB(128, 128,128);
		 ZeroLine.LineWidth = 2;
		 ZeroLine.DrawZeros = true;  // true

		 MovingAverageLength.Name = "Moving Average Length";
		 MovingAverageLength.SetInt(14);
		 MovingAverageLength.SetIntLimits(1, INT_MAX);

		 VolumeOrTrades.Name = "Calculation Based On";
		 VolumeOrTrades.SetCustomInputStrings("Up/Down Volume;Ask/Bid Volume;Up/Down Trades");
		 VolumeOrTrades.SetCustomInputIndex(0);

		 MovingAverageType.Name = "Moving Average Type";
		 MovingAverageType.SetMovAvgType(MOVAVGTYPE_EXPONENTIAL);
		
		 sc.FreeDLL = 0;

		 sc.AutoLoop = 1;

		 return;
	 }
	
	
	 // Do data processing
	int Up = SC_UPVOL;
	int Down = SC_DOWNVOL;

	if(VolumeOrTrades.GetIndex() == 0)
	{	
		Up = SC_UPVOL;
		Down = SC_DOWNVOL;
	}
	else if(VolumeOrTrades.GetIndex() == 1)
	{	
		Up = SC_ASKVOL;
		Down = SC_BIDVOL;
	}
	else
	{
		Up = SC_ASKNT;
		Down = SC_BIDNT;
	}

	 float TotalTrades = sc.BaseData[Up][sc.Index] + sc.BaseData[Down][sc.Index];
	
	 if (TotalTrades > 0)
		 UpDownVolumeRatio.Arrays[0][sc.Index] = 100 * (sc.BaseData[Up][sc.Index] - sc.BaseData[Down][sc.Index]) / TotalTrades;
	 else
		 UpDownVolumeRatio.Arrays[0][sc.Index] = 0.0f;
	
	sc.MovingAverage(UpDownVolumeRatio.Arrays[0], UpDownVolumeRatio, MovingAverageType.GetMovAvgType(), MovingAverageLength.GetInt());
}

/*==========================================================================*/

 SCSFExport scsf_MovAvgCross(SCStudyInterfaceRef sc)
 { 
	 SCInputRef MA1Price = sc.Input[0];
	 SCInputRef MA1Type = sc.Input[1];
	 SCInputRef MA1Length = sc.Input[2];
	 SCInputRef MA2Price = sc.Input[3];
	 SCInputRef MA2Type = sc.Input[4];
	 SCInputRef MA2Length = sc.Input[5];
	 SCInputRef DotsSpacing = sc.Input[6];

	 if (sc.SetDefaults)
	 {
		 // Set the configuration and defaults

		 sc.GraphName = "MovAvg Cross";

		 sc.AutoLoop = 1;

		 sc.GraphRegion = 0;

		 sc.Subgraph[0].Name = "MA1";
		 sc.Subgraph[0].DrawStyle = DRAWSTYLE_LINE;
		 sc.Subgraph[0].PrimaryColor = RGB(0,255,0);

		 sc.Subgraph[1].Name = "MA2";
		 sc.Subgraph[1].DrawStyle = DRAWSTYLE_LINE;
		 sc.Subgraph[1].PrimaryColor = RGB(255,0,255);

		 sc.Subgraph[2].Name = "Dots";
		 sc.Subgraph[2].SecondaryColorUsed = 1;
		 sc.Subgraph[2].PrimaryColor = RGB(0,255,0);
		 sc.Subgraph[2].SecondaryColor = RGB(255,0,0);
		 sc.Subgraph[2].DrawStyle = DRAWSTYLE_POINT;
		 sc.Subgraph[2].LineWidth = 4;

		 MA1Price.Name = "MA1 Input Data";
		 MA1Price.SetInputDataIndex(SC_LAST);
		 MA1Type.Name = "MA1.Type";
		 MA1Type.SetMovAvgType(MOVAVGTYPE_EXPONENTIAL);
		 MA1Length.Name = "MA1.Length";
		 MA1Length.SetInt(5);
		 MA1Length.SetIntLimits(1,MAX_STUDY_LENGTH);

		 MA2Price.Name = "MA2 Input Data";
		 MA2Price.SetInputDataIndex(SC_OPEN);
		 MA2Type.Name = "MA2.Type";
		 MA2Type.SetMovAvgType(MOVAVGTYPE_WEIGHTED);
		 MA2Length.Name = "MA2.Length";
		 MA2Length.SetInt(6);
		 MA2Length.SetIntLimits(1,MAX_STUDY_LENGTH);

		 DotsSpacing.Name = "Dots Spacing as Value";
		 DotsSpacing.SetFloat(1);

		 sc.FreeDLL=0;
		 return;
	 }

	 // Do data processing
	 sc.MovingAverage(sc.BaseData[MA1Price.GetInputDataIndex()], sc.Subgraph[0],MA1Type.GetMovAvgType(),MA1Length.GetInt());

	 sc.MovingAverage(sc.BaseData[MA2Price.GetInputDataIndex()], sc.Subgraph[1],MA2Type.GetMovAvgType(),MA2Length.GetInt());

	 int direction = sc.CrossOver(sc.Subgraph[0],sc.Subgraph[1]);
	 if ( direction == CROSS_FROM_TOP )
	 {
		 sc.Subgraph[2][sc.Index] = sc.BaseData[SC_HIGH][sc.Index] + DotsSpacing.GetFloat();
		 sc.Subgraph[2].DataColor[sc.Index] = sc.Subgraph[2].SecondaryColor;
	 }
	 else if ( direction == CROSS_FROM_BOTTOM )
	 {	sc.Subgraph[2][sc.Index] = sc.BaseData[SC_LOW][sc.Index] - DotsSpacing.GetFloat();
	 sc.Subgraph[2].DataColor[sc.Index] = sc.Subgraph[2].PrimaryColor;
	 }
	 else
	 {
		 sc.Subgraph[2][sc.Index] = 0;
	 }
 }


 /*==========================================================================*/
 SCSFExport scsf_BillWilliamsAC(SCStudyInterfaceRef sc)
 {
	 SCSubgraphRef Up = sc.Subgraph[0];
	 SCSubgraphRef Down = sc.Subgraph[1];
	 SCSubgraphRef Flat = sc.Subgraph[2];
	 SCSubgraphRef All = sc.Subgraph[3];
	 SCSubgraphRef Temp4 = sc.Subgraph[4];
	 SCSubgraphRef Sum1 = sc.Subgraph[5];
	 SCSubgraphRef Avg1 = sc.Subgraph[6];
	 SCSubgraphRef Sum2 = sc.Subgraph[7];
	 SCSubgraphRef Avg2 = sc.Subgraph[8];
	 SCSubgraphRef Sum4 = sc.Subgraph[9];
	 SCSubgraphRef Avg4 = sc.Subgraph[10];

	 SCInputRef LongMaLength = sc.Input[0];
	 SCInputRef ShortMaLength = sc.Input[1];
	 SCInputRef SignalMaLength = sc.Input[2];
	 SCInputRef MaType = sc.Input[3];

	 if (sc.SetDefaults)
	 {
		 sc.GraphName = "Bill Williams AC";

		 sc.StudyDescription = "";

		 

		 Up.Name = "Up";
		 Up.DrawStyle = DRAWSTYLE_DASH;
		 Up.PrimaryColor = RGB( 0, 255, 0);
		 Up.DrawZeros = false;

		 Down.Name = "Down";
		 Down.DrawStyle = DRAWSTYLE_DASH;
		 Down.PrimaryColor = RGB( 255, 0, 255);
		 Down.DrawZeros = false;

		 Flat.Name = "Flat";
		 Flat.DrawStyle = DRAWSTYLE_DASH;
		 Flat.PrimaryColor = RGB( 192, 192, 192);
		 Flat.DrawZeros = false;

		 All.Name = "All";
		 All.DrawStyle = DRAWSTYLE_IGNORE;
		 All.PrimaryColor = RGB( 255, 127, 0);
		 All.DrawZeros = false;

		 LongMaLength.Name = "Long Moving Average Length";
		 LongMaLength.SetInt(34);
		 LongMaLength.SetIntLimits(1, INT_MAX);

		 ShortMaLength.Name = "Short Moving Average Length";
		 ShortMaLength.SetInt(5);
		 ShortMaLength.SetIntLimits(1, INT_MAX);

		 SignalMaLength.Name = "Signal Moving Average Length";
		 SignalMaLength.SetInt(5);
		 SignalMaLength.SetIntLimits(1, INT_MAX);

		 MaType.Name = "Moving Average Type";
		 MaType.SetCustomInputStrings("Bill Williams Smoothed;Exponential;Simple");
		 MaType.SetCustomInputIndex(0);


		 sc.AutoLoop = 1;

		 return;
	 }

	 // Do data processing

	 const float cfactor1 = 2.0f / (LongMaLength.GetInt() + 1);		const float cinvfactor1 = 1.0f - cfactor1;
	 const float cfactor2 = 2.0f / (ShortMaLength.GetInt() + 1);	const float cinvfactor2 = 1.0f - cfactor2;
	 const float cfactor4 = 2.0f / (SignalMaLength.GetInt() + 1);	const float cinvfactor4 = 1.0f - cfactor4;

	 sc.DataStartIndex = 50;

	 int pos = sc.Index;
	 if (pos < 50)
		 return;

	 int j;
	 float avg3, avg4;

	 switch (MaType.GetInt())
	 {
	 case 0:
		 {
			 Sum1[pos] = Sum1[pos - 1] + sc.Close[pos];
			 if (pos >= 50 + LongMaLength.GetInt())
				 Sum1[pos] = Sum1[pos] - Avg1[pos - 1];
			 Avg1[pos] = Sum1[pos] / LongMaLength.GetInt();

			 Sum2[pos] = Sum2[pos - 1] + sc.Close[pos];
			 if (pos >= 50 + ShortMaLength.GetInt())
				 Sum2[pos] = Sum2[pos] - Avg2[pos - 1];
			 Avg2[pos] = Sum2[pos] / ShortMaLength.GetInt();

			 avg3 = Avg2[pos] - Avg1[pos];

			 Temp4.Data[pos] = avg3;
			 Sum4[pos] = Sum4[pos - 1] + avg3;
			 avg4 = Sum4[pos] / SignalMaLength.GetInt();
			 j = pos - SignalMaLength.GetInt() + 1;
			 if (pos > 50 + SignalMaLength.GetInt() - 2)
				 Sum4[pos] = Sum4[pos] - Temp4.Data[j];
		 }
		 break;

	 case 1:
		 {
			 Avg1[pos] = Avg1[pos - 1] * cinvfactor1 + sc.Close[pos] * cfactor1;
			 Avg2[pos] = Avg2[pos - 1] * cinvfactor2 + sc.Close[pos] * cfactor2;
			 avg3 = Avg2[pos] - Avg1[pos];
			 //Temp4.Data[pos] = avg3;
			 avg4 = Avg4[pos] = Avg4[pos - 1] * cinvfactor4 + avg3 * cfactor4;
		 }
		 break;

	 case 2:
	 default:
		 {
			 Sum1[pos] = Sum1[pos - 1] + sc.Close[pos];
			 float avg1 = Sum1[pos] / LongMaLength.GetInt();
			 j = pos - LongMaLength.GetInt() + 1;
			 if (pos > 50 + LongMaLength.GetInt() - 2)
				 Sum1[pos] = Sum1[pos] - sc.Close[j];

			 Sum2[pos] = Sum2[pos - 1] + sc.Close[pos];
			 float avg2 = Sum2[pos] / ShortMaLength.GetInt();
			 j = pos - ShortMaLength.GetInt() + 1;
			 if (pos > 50 + ShortMaLength.GetInt() - 2)
				 Sum2[pos] = Sum2[pos] - sc.Close[j];

			 avg3 = avg2 - avg1;

			 Temp4.Data[pos] = avg3;
			 Sum4[pos] = Sum4[pos - 1] + avg3;
			 avg4 = Sum4[pos] / SignalMaLength.GetInt();
			 j = pos - SignalMaLength.GetInt() + 1;
			 if (pos > 50 + SignalMaLength.GetInt() - 2)
				 Sum4[pos] = Sum4[pos] - Temp4.Data[j];
		 }
		 break;
	 }


	 All.Data[pos] = avg3 - avg4;

	 if (All.Data[pos] > All.Data[pos - 1])
	 {
		 Up.Data[pos] = All.Data[pos];
		 Down.Data[pos] = 0;
		 Flat.Data[pos] = 0;
	 }
	 else if(All.Data[pos] < All.Data[pos - 1])
	 {
		 Up.Data[pos] = 0;
		 Down.Data[pos] = All.Data[pos];
		 Flat.Data[pos] = 0;
	 }
	 else
	 {
		 Up.Data[pos] = 0;
		 Down.Data[pos] = 0;
		 Flat.Data[pos] = All.Data[pos];
	 }
 }

 /*==========================================================================*/
 SCSFExport scsf_BillWilliamsAO(SCStudyInterfaceRef sc)
 {
	 SCSubgraphRef AO = sc.Subgraph[0];
	 SCSubgraphRef ZeroLine = sc.Subgraph[1];

	 SCInputRef InputData = sc.Input[0];
	 SCInputRef MA1Length = sc.Input[2];
	 SCInputRef MA2Length = sc.Input[3];
	 SCInputRef MAType = sc.Input[4];

	 if (sc.SetDefaults)
	 {
		 sc.GraphName = "Bill Williams Awesome Oscillator";

		 sc.GraphRegion = 1;
		 sc.ValueFormat = 3;
		 sc.AutoLoop = 1;

		 AO.Name = "AO";
		 AO.DrawStyle = DRAWSTYLE_BAR;
		 AO.LineWidth = 2;
		 AO.SecondaryColor = RGB(255,0,0);
		 AO.AutoColoring = AUTOCOLOR_SLOPE;
		 AO.DrawZeros = false;

		 ZeroLine.Name ="Zero Line";
		 ZeroLine.DrawStyle = DRAWSTYLE_LINE;
		 ZeroLine.LineWidth =1;
		 ZeroLine.PrimaryColor = RGB(255,0,255);
		 ZeroLine.DrawZeros = true;

		 InputData.SetInputDataIndex(SC_HL);

		 MA1Length.Name = "Moving Average 1 Length";
		 MA1Length.SetInt(34);
		 MA1Length.SetIntLimits(1, MAX_STUDY_LENGTH);

		 MA2Length.Name = "Moving Average 2 Length";
		 MA2Length.SetInt(5);
		 MA2Length.SetIntLimits(1, MAX_STUDY_LENGTH);

		 MAType.Name = "Moving Average Type";
		 MAType.SetMovAvgType(MOVAVGTYPE_SIMPLE);


		 return;
	 }

	 sc.DataStartIndex = max(MA1Length.GetInt(), MA2Length.GetInt());



	 sc.MovingAverage(sc.BaseDataIn[InputData.GetInputDataIndex()], AO.Arrays[0], MAType.GetMovAvgType(), MA1Length.GetInt());
	 sc.MovingAverage(sc.BaseDataIn[InputData.GetInputDataIndex()], AO.Arrays[1], MAType.GetMovAvgType(), MA2Length.GetInt());

	 AO.Data[sc.Index] = AO.Arrays[1][sc.Index] - AO.Arrays[0][sc.Index];

 }

 /*==========================================================================*/
 SCSFExport scsf_BillWilliamsMA(SCStudyInterfaceRef sc)
 {
	 SCSubgraphRef MovAvg = sc.Subgraph[0];

	 SCInputRef InputData = sc.Input[0];
	 SCInputRef MovingAverageLength = sc.Input[2];
	 SCInputRef MovingAverageType = sc.Input[4];
	 SCInputRef Version = sc.Input[5];

	 if (sc.SetDefaults)
	 {
		 sc.GraphName = "Bill Williams Moving Average";

		 sc.StudyDescription = "To offset the moving average, use the Displacement setting for the MovAvg Subgraph.";
		 sc.GraphRegion= 0;

		 MovAvg.Name = "MovAvg";
		 MovAvg.DrawStyle = DRAWSTYLE_LINE;
		 MovAvg.PrimaryColor = RGB(0,255,0);

		 InputData.Name = "Input Data";
		 InputData.SetInputDataIndex(SC_LAST);

		 MovingAverageLength.Name = "MovAvg Length";
		 MovingAverageLength.SetInt(34);
		 MovingAverageLength.SetIntLimits(1, INT_MAX);

		 MovingAverageType.Name = "Moving Average Type";
		 MovingAverageType.SetCustomInputStrings(";Bill Willams EMA;Smoothed Moving Average");
		 MovingAverageType.SetCustomInputIndex(2);

		 Version.SetInt(1);

		 sc.AutoLoop = 1;

		 return;
	 }

	 // Do data processing

	 sc.DataStartIndex = 0;

	 if (Version.GetInt() < 1)
	 {
		 InputData.SetInputDataIndex(SC_LAST);
		 Version.SetInt(1);
	 }

	 if (MovingAverageType.GetIndex() == 0)
		 MovingAverageType.SetCustomInputIndex(1);

	 unsigned long InputIndex = InputData.GetInputDataIndex();

	 if (MovingAverageType.GetIndex() == 1)
	 {
		 if (sc.Index == 0)
		 {
			 MovAvg[0] = sc.BaseData[InputIndex][0];
			 MovAvg.Arrays[4][sc.Index] = sc.BaseData[InputIndex][0]*MovingAverageLength.GetInt();
		 }
		 else
		 {
			 MovAvg.Arrays[4][sc.Index] = MovAvg.Arrays[4][sc.Index - 1] + sc.BaseData[InputIndex][sc.Index];
			 MovAvg.Arrays[4][sc.Index] -=  MovAvg[sc.Index - 1];
			 MovAvg[sc.Index] = MovAvg.Arrays[4][sc.Index] / MovingAverageLength.GetInt();
		 }
	 }
	 else
	 {
		 sc.SmoothedMovingAverage(sc.BaseData[InputIndex], MovAvg, sc.Index, MovingAverageLength.GetInt());
	 }
 }

 /*==========================================================================*/
 SCSFExport scsf_BillWilliamsAlligator(SCStudyInterfaceRef sc)
 {
	 const int jaw = 0;
	 const int teeth = 1;
	 const int lips = 2;

	 SCSubgraphRef Jaw = sc.Subgraph[0];
	 SCSubgraphRef Teeth = sc.Subgraph[1];
	 SCSubgraphRef Lips = sc.Subgraph[2];

	 SCInputRef InputData = sc.Input[0];
	 SCInputRef JawLength = sc.Input[1];
	 SCInputRef TeethLength = sc.Input[3];
	 SCInputRef LipsLength = sc.Input[5];
	 SCInputRef VersionUpdate1 = sc.Input[7];
	 SCInputRef MovAvgType = sc.Input[8];
	 SCInputRef VersionUpdate2 = sc.Input[9];

	 if (sc.SetDefaults)
	 {
		 sc.GraphName = "Bill Williams Alligator";

		 sc.StudyDescription = "";

		 // Set the region to draw the graph in.  Region zero is the main
		 // price graph region.
		 sc.GraphRegion = 0;
		 

		 // Set the name of the first subgraph
		 // Set the color and style of the graph line.  If these are not set
		 // the default will be used.
		 Jaw.Name = "Jaw";		
		 Jaw.PrimaryColor = RGB(0,0,255);  // Blue
		 Jaw.DrawStyle = DRAWSTYLE_LINE;
		 Jaw.GraphicalDisplacement = 8;
		 Jaw.DrawZeros = false;

		 // Set the color and style of the graph line.  If these are not set
		 // the default will be used.
		 Teeth.Name = "Teeth";
		 Teeth.PrimaryColor = RGB(255,0,0);  // Red
		 Teeth.DrawStyle = DRAWSTYLE_LINE;
		 Teeth.GraphicalDisplacement = 5;
		 Teeth.DrawZeros = false;

		 // Set the color and style of the graph line.  If these are not set
		 // the default will be used.
		 Lips.Name = "Lips";		
		 Lips.PrimaryColor = RGB(0, 255, 0);  // Red
		 Lips.DrawStyle = DRAWSTYLE_LINE;
		 Lips.GraphicalDisplacement = 3;
		 Lips.DrawZeros = false;

		 sc.AutoLoop = 1;

		 InputData.Name = "Input Data";
		 InputData.SetInputDataIndex(SC_HL);

		 JawLength.Name = "Jaw Length";
		 JawLength.SetIntLimits(1,MAX_STUDY_LENGTH);
		 JawLength.SetInt(13);

		 TeethLength.Name = "Teeth Length";
		 TeethLength.SetIntLimits(1,MAX_STUDY_LENGTH);
		 TeethLength.SetInt(8);

		 LipsLength.Name = "Lips Length";
		 LipsLength.SetIntLimits(1,MAX_STUDY_LENGTH);
		 LipsLength.SetInt(5);

		 VersionUpdate1.SetYesNo(true); //Used For Settings Updating

		 MovAvgType.Name = "Moving Average Type";
		 MovAvgType.SetMovAvgType(MOVAVGTYPE_SMOOTHED);

		 VersionUpdate2.SetYesNo(true);
		 return;
	 }

	 // data processing

	 if (VersionUpdate2.GetYesNo() == false)
		 MovAvgType.SetMovAvgType(MOVAVGTYPE_SMOOTHED);

	 if ( VersionUpdate1.GetYesNo() == false )
	 {
		 Jaw.GraphicalDisplacement = 8;
		 Teeth.GraphicalDisplacement = 5;
		 Lips.GraphicalDisplacement = 3;
	 }

	 // calculate jaw, teeth and lips

	 sc.MovingAverage(sc.BaseData[InputData.GetInputDataIndex()], Jaw, MovAvgType.GetMovAvgType(), JawLength.GetInt());
	 sc.MovingAverage(sc.BaseData[InputData.GetInputDataIndex()], Teeth, MovAvgType.GetMovAvgType(), TeethLength.GetInt());
	 sc.MovingAverage(sc.BaseData[InputData.GetInputDataIndex()], Lips, MovAvgType.GetMovAvgType(), LipsLength.GetInt());
 }



/*==========================================================================*/

 SCSFExport scsf_AverageOfAverage(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Avg = sc.Subgraph[0];
	SCSubgraphRef Temp1 = sc.Subgraph[1];

	SCInputRef Length = sc.Input[0];
	SCInputRef EmaLength = sc.Input[1];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults
		
		sc.GraphName = "Average of Average";
		
		sc.StudyDescription = "";

		sc.AutoLoop = 1;  // true
		
		Avg.Name = "Avg";
		Avg.DrawStyle = DRAWSTYLE_LINE;
		Avg.PrimaryColor = RGB(0,255,0);
		
		Length.Name = "Length";
		Length.SetInt(10);
		Length.SetIntLimits(1, INT_MAX);
		
		EmaLength.Name = "ExpMA Length";
		EmaLength.SetInt(10);
		EmaLength.SetIntLimits(1, INT_MAX);
		
		return;
	}
	
	
	// Do data processing
	
	
	/* This calculates the Exponential Moving Average of a Linear Regression Indicator
		by passing the output array from the Linear Regression Indicator function to the
		Input data parameter of Exponential Moving Average function.*/
	
	sc.LinearRegressionIndicator(sc.Close, Temp1, sc.Index, Length.GetInt());
	sc.ExponentialMovAvg(Temp1, Avg, sc.Index, EmaLength.GetInt());
}



/*==========================================================================*/
 SCSFExport scsf_WoodieEMA(SCStudyInterfaceRef sc)
 {
	 SCSubgraphRef EMA = sc.Subgraph[0];
	 SCSubgraphRef UpDownColors = sc.Subgraph[1];
	 SCSubgraphRef DataEma = sc.Subgraph[2];

	 SCInputRef Bars = sc.Input[0];
	 SCInputRef PriceChange = sc.Input[1];
	 SCInputRef EMALength = sc.Input[2];
	 SCInputRef FixedValue = sc.Input[4];
	 SCInputRef InputData = sc.Input[6];

	 if(sc.SetDefaults)
	 {
		 sc.GraphName="Woodies EMA";

		 sc.AutoLoop = 1;

		 sc.GraphRegion = 0;

		 EMA.Name = "EMA";
		 EMA.DrawStyle = DRAWSTYLE_LINE;
		 EMA.PrimaryColor = RGB(0,255,0);
		 EMA.DrawZeros = false;

		 UpDownColors.Name = "Up / Down Color";
		 UpDownColors.DrawStyle = DRAWSTYLE_IGNORE;
		 UpDownColors.PrimaryColor = RGB(0,255,0);
		 UpDownColors.SecondaryColor = RGB(255,0,0);
		 UpDownColors.SecondaryColorUsed = 1;
		 UpDownColors.DrawZeros = false;

		 InputData.Name = "Input Data";
		 InputData.SetInputDataIndex(SC_HLC);

		 Bars.Name = "Number of Bars for Angle";
		 Bars.SetInt(4);
		 Bars.SetIntLimits(1,MAX_STUDY_LENGTH);

		 PriceChange.Name = "Price Change for Angle";
		 PriceChange.SetFloat(0.000001f);


		 EMALength.Name = "EMA Length";
		 EMALength.SetInt(34);
		 EMALength.SetIntLimits(1,MAX_STUDY_LENGTH);

		 FixedValue.Name = "Fixed Value to Use";
		 FixedValue.SetFloat(0);

		 return;
	 }

	 if (!InputData.GetInputDataIndex())	 // backward compatibility
		 InputData.SetInputDataIndex(SC_HLC);

	 sc.DataStartIndex = EMALength.GetInt() + Bars.GetInt();

	 sc.ExponentialMovAvg(sc.BaseData[InputData.GetInputDataIndex()], DataEma, EMALength.GetInt());

	 if (sc.Index < Bars.GetInt())
		 return;

	 int pos = sc.Index;

	 if(FixedValue.GetFloat() == 0.0f) 
		 EMA[pos] = DataEma[pos];
	 else 
		 EMA[pos] = FixedValue.GetFloat();

	 if (DataEma[pos] - DataEma[pos-Bars.GetInt()] >= PriceChange.GetFloat())
	 {
		 EMA.DataColor[pos] = UpDownColors.PrimaryColor; 
		 UpDownColors[pos] = (float)UpDownColors.PrimaryColor; 
	 }
	 else if (DataEma[pos] - DataEma[pos-Bars.GetInt()] <= -PriceChange.GetFloat())
	 {
		 EMA.DataColor[pos] = UpDownColors.SecondaryColor; 
		 UpDownColors[pos] = (float)UpDownColors.SecondaryColor; 
	 }
	 else
	 {
		 EMA.DataColor[pos] = EMA.PrimaryColor;
		 UpDownColors[pos] = (float)EMA.PrimaryColor;
	 }

 }

/*==========================================================================*/
 SCSFExport scsf_WoodieCCITrend(SCStudyInterfaceRef sc)
 {
	 SCSubgraphRef Above = sc.Subgraph[0];
	 SCSubgraphRef Below = sc.Subgraph[1];
	 SCSubgraphRef AboveColorValues = sc.Subgraph[2];
	 SCSubgraphRef DoNotDraw1 = sc.Subgraph[3];
	 SCSubgraphRef DoNotDraw2 = sc.Subgraph[4];
	 SCSubgraphRef CCI = sc.Subgraph[5];

	 SCInputRef WarningBar = sc.Input[0];
	 SCInputRef AlternateColor = sc.Input[1];
	 SCInputRef CCILength = sc.Input[2];
	 SCInputRef NumberOfBars = sc.Input[3];
	 SCInputRef BarsIntertrendState = sc.Input[4];
	 SCInputRef BarsFixPos = sc.Input[5];

	 if(sc.SetDefaults)
	 {
		 sc.GraphName="Woodies CCI Trend - Old";

		 sc.AutoLoop = 0;

		 Above.Name = "Above";
		 Above.DrawStyle = DRAWSTYLE_BAR;
		 Above.PrimaryColor = RGB(0,0,255);
		 Above.DrawZeros = false;

		 Below.Name = "Below";
		 Below.DrawStyle = DRAWSTYLE_BAR;
		 Below.PrimaryColor = RGB(255,0,0);
		 Below.DrawZeros = false;

		 AboveColorValues.Name = "Color Value Output";
		 AboveColorValues.PrimaryColor = RGB(255, 255, 255);
		 AboveColorValues.DrawStyle = DRAWSTYLE_IGNORE;
		 AboveColorValues.DrawZeros = false;

		 DoNotDraw1.DrawStyle = DRAWSTYLE_HIDDEN;
		 DoNotDraw1.PrimaryColor = RGB(255,255,0);
		 DoNotDraw1.DrawZeros = false;

		 DoNotDraw2.DrawStyle = DRAWSTYLE_HIDDEN;
		 DoNotDraw2.PrimaryColor = RGB(98,98,98);
		 DoNotDraw2.DrawZeros = false;

		 CCI.Name = "CCI";
		 CCI.DrawStyle = DRAWSTYLE_IGNORE;
		 CCI.PrimaryColor = RGB(0,0,255);
		 CCI.DrawZeros = false;


		 WarningBar.Name = "Use a Warning Bar";
		 WarningBar.SetYesNo(false);		

		 AlternateColor.Name = "Alternate Colour for uncertain trend states 0, 1, 2 or 4";
		 AlternateColor.SetInt(0);
		 AlternateColor.SetIntLimits(0,4);

		 CCILength.Name = "CCI Length";
		 CCILength.SetInt(14);
		 CCILength.SetIntLimits(1,MAX_STUDY_LENGTH);

		 NumberOfBars.Name = "Number of Bars";
		 NumberOfBars.SetInt(6);
		 NumberOfBars.SetIntLimits(1,MAX_STUDY_LENGTH);

		 BarsIntertrendState.Name = "# of Bars for Intertrend State";
		 BarsIntertrendState.SetInt(1);
		 BarsIntertrendState.SetIntLimits(1,MAX_STUDY_LENGTH);

		 BarsFixPos.Name = "0 = Bars or n = Fixed Position (n/1000)";
		 BarsFixPos.SetFloat(0);	

		 return;
	 }
	

	 AboveColorValues.DrawStyle = DRAWSTYLE_IGNORE;
	 DoNotDraw1.DrawStyle = DRAWSTYLE_HIDDEN;
	 DoNotDraw2.DrawStyle = DRAWSTYLE_HIDDEN;

	 COLORREF Color0 = Above.PrimaryColor;
	 COLORREF Color1 = Below.PrimaryColor;
	 COLORREF Color2 = DoNotDraw1.PrimaryColor;
	 COLORREF Color3 = DoNotDraw2.PrimaryColor;

	 int ArraySize = sc.ArraySize;

	 switch (AlternateColor.GetInt())
	 {
	 case 4:
	 case 2:
		 DoNotDraw1.Name ="Change on Next Bar";
		 DoNotDraw2.Name ="Trend & Position Confused";
		 break;
	 case 1:
		 DoNotDraw1.Name = "Trend Up but below Zero";
		 DoNotDraw2.Name = "Trend Dn but above Zero";
		 break;
	 case 0:
	 default:
		 DoNotDraw1.Name = "Change on Next Bar";
	 }

	 int ud = -2, goingpos = 0, goingneg = 0, posbar = 0, negbar = 0, up = 0, 
		 dn = 0, next = 0;


	 for(int i = sc.UpdateStartIndex; i < ArraySize;i++)
	 {
		 sc.CCI(sc.HLCAvg, CCI.Arrays[0], CCI, i, CCILength.GetInt(), 0.015f);
	 }

	 int Start = max (sc.UpdateStartIndex, CCILength.GetInt()*2 - 1);

	 float cci0;
	 for (int i = Start-1; i >= CCILength.GetInt()*2; i--)
	 {
		 cci0 = sc.Subgraph[5][i];
		 if (cci0 > 0)
		 {
			 dn = 0;
			 up++;
		 }

		 else if (cci0 < 0)
		 {
			 up = 0;
			 dn++;
		 }

		 if(up >= NumberOfBars.GetInt()) 
		 {
			 Start = i + NumberOfBars.GetInt();
			 ud=2;
			 break;
		 }		
		 else if(dn >= NumberOfBars.GetInt()) 
		 {
			 ud=-2;
			 Start = i + NumberOfBars.GetInt();
			 break;
		 }
	 }

	 sc.DataStartIndex = CCILength.GetInt()*2 - 1;

	 for (int pos=Start; pos < ArraySize; pos++)
	 {
		 cci0 = CCI[pos];

		 if (cci0>0) 
		 {
			 if(AlternateColor.GetInt() == 4 && BarsIntertrendState.GetInt() <= up+1) 
				 dn = 0;

			 if(ud <= 0)
			 {
				 if (goingpos == 0)
				 {
					 goingpos = 1;
					 up = 1;
					 dn = 0; 
					 negbar = 0; 
					 if(AlternateColor.GetInt() == 4 && BarsIntertrendState.GetInt() == 1) 
						 ud=0;
				 }
				 else
				 {
					 up++; 
					 if(up == NumberOfBars.GetInt()) 
						 dn = 0;

					 if(AlternateColor.GetInt() == 4 && BarsIntertrendState.GetInt() <= up) 
						 ud = 0;
				 }
			 } 
			 else	if(ud >= 2)
			 {
				 if (goingneg == 1) 
				 {
					 posbar++; 
					 if(posbar >= 2) 
					 {
						 goingneg = 0; 
						 dn = 0;
					 } 
				 }
				 else
					 up++;
			 } 
		 }

		 if (cci0<0) 
		 {
			 if(AlternateColor.GetInt()==4 && BarsIntertrendState.GetInt()<=dn+1)
				 up=0;

			 if(ud>= 0) 
			 {
				 if (goingneg==0)
				 {
					 goingneg=1;
					 dn=1; 
					 up=0; 
					 posbar=0; 
					 if(AlternateColor.GetInt()==4 && BarsIntertrendState.GetInt()==1) 
						 ud=0;
				 }
				 else
				 {
					 dn++; 
					 if(dn==NumberOfBars.GetInt()) 
						 up=0;

					 if(AlternateColor.GetInt()==4 && BarsIntertrendState.GetInt()<=dn) 
						 ud=0;
				 }
			 } 
			 else	if(ud <= -2) 
			 {
				 if (goingpos == 1)
				 {
					 negbar++; 
					 if(negbar>=2) 
					 {
						 goingpos=0; 
						 up=0;
					 } 
				 }
				 else
					 dn++;
			 } 
		 };


		 next=0;
		 if(up==(NumberOfBars.GetInt()-1) && (ud<=-2 || (ud==0 && AlternateColor.GetInt()==4)) && WarningBar.GetYesNo()!=0) 
			 next=1; 

		 else	if(dn==(NumberOfBars.GetInt()-1) && (ud>= 2 || (ud==0 && AlternateColor.GetInt()==4)) && WarningBar.GetYesNo()!=0) 
			 next=1;

		 if(up>=NumberOfBars.GetInt()) 
			 ud=2;

		 if(dn>=NumberOfBars.GetInt()) 
			 ud=-2;

		 if (BarsFixPos.GetFloat())
			 Above[pos] = BarsFixPos.GetFloat()/1000;
		 else
			 Above[pos] = cci0; 

		 if (next==1)
		 {
			 if(AlternateColor.GetInt()==0 || AlternateColor.GetInt()==2 || AlternateColor.GetInt()==4)
			 {
				 Above.DataColor[pos] = Color2;
			 } 
			 else	if(AlternateColor.GetInt()==1)
			 { 
				 if(BarsFixPos.GetFloat()==0) 
				 { 
					 Above.DataColor[pos] = Color0; 
					 Below[pos]=cci0/2; 
				 };
			 };
		 } 
		 else
		 {
			 if (ud==0 && AlternateColor.GetInt()==4)
			 { 
				 Above.DataColor[pos] = Color3;
			 } 
			 else if (ud>=2)
			 {
				 if(AlternateColor.GetInt()==0 || cci0>=0)
				 { 
					 Above.DataColor[pos] = Color0;
				 } 
				 else	if(AlternateColor.GetInt()==1)
				 { 
					 Above.DataColor[pos] = Color2;
				 } 
				 else if(AlternateColor.GetInt()==2 || AlternateColor.GetInt()==4)
				 { 
					 Above.DataColor[pos] = Color3;
				 }
			 } 
			 else if (ud<=-2)
			 {
				 if(AlternateColor.GetInt()==0 || cci0<=0)
				 { 
					 Above.DataColor[pos] = Color1;
				 } 
				 else
				 {
					 Above.DataColor[pos] = Color3;
				 }
			 }
		 }

		 if (Above.DataColor[pos] == Color0)
		 {
			 AboveColorValues[pos] = 1;
		 }
		 else if (Above.DataColor[pos] == Color1)
		 {
			 AboveColorValues[pos] = 2;
		 }
		 else if (Above.DataColor[pos] == Color2)
		 {
			 AboveColorValues[pos] = 3;
		 }
		 else if (Above.DataColor[pos] == Color3)
		 {
			 AboveColorValues[pos] = 4;
		 }
	 }
 }

/*==========================================================================*/
 SCSFExport scsf_Sidewinder(SCStudyInterfaceRef sc)
 {
	 SCSubgraphRef SwTop = sc.Subgraph[0];
	 SCSubgraphRef SwBottom = sc.Subgraph[1];
	 SCSubgraphRef FlatColor = sc.Subgraph[2];
	 SCSubgraphRef OutWorksheet = sc.Subgraph[3];
	 SCSubgraphRef Lsma = sc.Subgraph[4];
	 SCSubgraphRef LsmaAngle = sc.Subgraph[5];
	 SCSubgraphRef Ema = sc.Subgraph[6];
	 SCSubgraphRef EmaAngle = sc.Subgraph[7];

	 SCInputRef LsmaLength = sc.Input[0];
	 SCInputRef EmaLength = sc.Input[1];
	 SCInputRef TrendingThreshold = sc.Input[3];
	 SCInputRef NormalThreshold = sc.Input[4];
	 SCInputRef LineValue = sc.Input[5];
	 SCInputRef TickValueOverride = sc.Input[6];

	 if(sc.SetDefaults)
	 {
		 sc.GraphName="Sidewinder";
		 sc.StudyDescription="Sidewinder";

		 sc.AutoLoop = 0;

		 SwTop.Name = "SW Top";
		 SwTop.DrawStyle = DRAWSTYLE_LINE;
		 SwTop.LineWidth = 2;
		 SwTop.PrimaryColor = RGB(255,255,0);
		 SwTop.SecondaryColor = RGB(0,255,0);
		 SwTop.SecondaryColorUsed = 1;  // true
		 SwTop.DrawZeros = false;

		 SwBottom.Name = "SW Bottom";
		 SwBottom.DrawStyle = DRAWSTYLE_LINE;
		 SwBottom.LineWidth = 2;
		 SwBottom.PrimaryColor = RGB(255,255,0);
		 SwBottom.SecondaryColor = RGB(0,255,0);
		 SwBottom.SecondaryColorUsed = 1;  // true
		 SwBottom.DrawZeros = false;

		 FlatColor.Name = "Flat Color";
		 FlatColor.DrawStyle = DRAWSTYLE_IGNORE;
		 FlatColor.PrimaryColor = RGB(255,0,0);
		 FlatColor.DrawZeros = false;

		 OutWorksheet.Name = "Output for Spreadsheets";
		 OutWorksheet.DrawStyle = DRAWSTYLE_IGNORE;
		 OutWorksheet.PrimaryColor = RGB(255,255,255);
		 OutWorksheet.DrawZeros = false;

		 Lsma.DrawStyle = DRAWSTYLE_IGNORE;
		 Lsma.DrawZeros = false;

		 LsmaAngle.DrawStyle = DRAWSTYLE_IGNORE;
		 LsmaAngle.DrawZeros = false;

		 Ema.DrawStyle = DRAWSTYLE_IGNORE;
		 Ema.DrawZeros = false;

		 EmaAngle.DrawStyle = DRAWSTYLE_IGNORE;
		 EmaAngle.DrawZeros = false;

		 LsmaLength.Name = "LSMA Length";
		 LsmaLength.SetInt(25);
		 LsmaLength.SetIntLimits(1,MAX_STUDY_LENGTH);

		 EmaLength.Name = "EMA Length";
		 EmaLength.SetInt(34);
		 EmaLength.SetIntLimits(1,MAX_STUDY_LENGTH);

		 TrendingThreshold.Name = "Trending Threshold";
		 TrendingThreshold.SetFloat(100.0f);

		 NormalThreshold.Name = "Normal Threshold";
		 NormalThreshold.SetFloat(30.0f);

		 LineValue.Name = "Line Value";
		 LineValue.SetFloat(200.0f);

		 TickValueOverride.Name = "Tick Value (Enter number to override default)";
		 TickValueOverride.SetFloat(0.0f);

		 sc.AutoLoop = 1;

		 return;
	 }

	 OutWorksheet.DrawStyle = DRAWSTYLE_IGNORE;
	 OutWorksheet.PrimaryColor = RGB(255,255,255);

	 const float RAD_TO_DEG = (180.0f/3.14159265358979f);

	 // Colors
	 COLORREF colorFlat = FlatColor.PrimaryColor;
	 COLORREF colorNormalUp = SwTop.PrimaryColor;
	 COLORREF colorTrendingUp = SwTop.SecondaryColor;
	 COLORREF colorNormalDown = SwBottom.PrimaryColor;
	 COLORREF colorTrendingDown = SwBottom.SecondaryColor;

	 if (TickValueOverride.GetFloat() == 1)
		 TickValueOverride.SetFloat(0);

	 float inTickSize = sc.TickSize;

	 if(TickValueOverride.GetFloat() != 0)
		 inTickSize = TickValueOverride.GetFloat();

	 float &PriorTickSize = sc.PersistVars->f1;

	 if(PriorTickSize != inTickSize)
	 {
		 sc.UpdateStartIndex = 0;
		 PriorTickSize = inTickSize;
	 }


	 float InverseTickSize = 1/inTickSize;


	 sc.DataStartIndex = LsmaLength.GetInt() - 1;

	 // Determine the input array element to begin calculation at
	 int Pos = sc.Index;
	 if (Pos < 2)
		 return;

	 // Calculate the EMA
	 sc.ExponentialMovAvg(sc.Close, Ema, Pos, EmaLength.GetInt());

	 // Calculate the LSMA
	 sc.LinearRegressionIndicator(sc.Close,Lsma,Pos, LsmaLength.GetInt());

	 // Calculate the LSMA angle
	 float lsma_Angle = atan((Lsma[Pos]-((Lsma[Pos-1]+Lsma[Pos-2])/2.0f))*InverseTickSize)*RAD_TO_DEG;
	 LsmaAngle[Pos] = lsma_Angle;

	 // Calculate the EMA angle
	 float ema_Angle	= atan((Ema[Pos]-((Ema[Pos-1]+Ema[Pos-2])/2.0f))*InverseTickSize)*RAD_TO_DEG;
	 EmaAngle[Pos] = ema_Angle;

	 // Calculate SW
	 COLORREF color;
	 float worksheetValue;

	 if (fabs(EmaAngle[Pos]) >= 15.0f
		 && (EmaAngle[Pos] + LsmaAngle[Pos]) >= TrendingThreshold.GetFloat())
	 {
		 color = colorTrendingUp; 
		 worksheetValue = 2;
	 }
	 else if (fabs(EmaAngle[Pos]) >= 15.0f
		 && (EmaAngle[Pos] + LsmaAngle[Pos]) <= -TrendingThreshold.GetFloat())
	 {
		 color = colorTrendingDown; 
		 worksheetValue = -2;
	 }
	 else if (EmaAngle[Pos] > 5.0f && LsmaAngle[Pos] > 0.0f
		 && (EmaAngle[Pos] + LsmaAngle[Pos]) >= NormalThreshold.GetFloat())
	 {
		 color = colorNormalUp;
		 worksheetValue = 1;
	 }
	 else if (EmaAngle[Pos] < 5.0f && LsmaAngle[Pos] < 0.0f
		 && (EmaAngle[Pos] + LsmaAngle[Pos]) <= -NormalThreshold.GetFloat())
	 {
		 color = colorNormalDown;
		 worksheetValue = -1;
	 }
	 else
	 {
		 color = colorFlat;
		 worksheetValue = 0;
	 }

	 SwTop[Pos] = LineValue.GetFloat();
	 SwBottom[Pos] = -LineValue.GetFloat();

	 SwTop.DataColor[Pos] = color;
	 SwBottom.DataColor[Pos] = color;

	 OutWorksheet[Pos] = worksheetValue;
 }


/*==========================================================================*/
 SCSFExport scsf_LSMAAboveBelowCCI(SCStudyInterfaceRef sc)
 {
	 SCSubgraphRef LsmaCCI = sc.Subgraph[0];
	 SCSubgraphRef DoNotDraw1 = sc.Subgraph[1];
	 SCSubgraphRef DoNotDraw2 = sc.Subgraph[2];
	 SCSubgraphRef DoNotDraw3 = sc.Subgraph[3];
	 SCSubgraphRef DoNotDraw4 = sc.Subgraph[4];

	 SCInputRef LsmaLength = sc.Input[0];
	 SCInputRef CCILength = sc.Input[2];
	 SCInputRef BarsPointsLsma = sc.Input[3];
	 

	 if (sc.SetDefaults)
	 {
		 // Set the configuration and defaults
		 sc.GraphName ="LSMA Above/Below CCI";
		 sc.GraphRegion = 1;	

		 LsmaCCI.DrawStyle = DRAWSTYLE_BAR;
		 LsmaCCI.Name = "LSMA Above/Below CCI";
		 LsmaCCI.PrimaryColor = RGB(0, 255, 0);
		 LsmaCCI.SecondaryColorUsed = 1;
		 LsmaCCI.SecondaryColor = RGB(255, 0, 255);
		 LsmaCCI.DrawZeros = false;

		 DoNotDraw4.DrawStyle = DRAWSTYLE_IGNORE;
		 DoNotDraw4.Name = "Spreadsheet Output";
		 DoNotDraw4.PrimaryColor = RGB(255,0,0);
		 DoNotDraw4.DrawZeros = false;

		 LsmaLength.Name = "MA Length";
		 LsmaLength.SetInt(25);
		 LsmaLength.SetIntLimits(1,MAX_STUDY_LENGTH);

		 CCILength.Name = "CCI Length";
		 CCILength.SetInt(14);
		 CCILength.SetIntLimits(1,MAX_STUDY_LENGTH);

		 BarsPointsLsma.Name = "Bars (0) or Points (n/1000) or LSMA (9999)";
		 BarsPointsLsma.SetInt(0);
		 BarsPointsLsma.SetIntLimits(0, 9999);

		 //MovAvgType.Name = "Moving Average Type";
		 //MovAvgType.SetMovAvgType(MOVAVGTYPE_LINEARREGRESSION);

		 sc.AutoLoop = 1;

		 return;
	 }

	 COLORREF color0 = LsmaCCI.PrimaryColor;
	 COLORREF color1 = LsmaCCI.SecondaryColor;

	 sc.DataStartIndex = max(LsmaLength.GetInt(), CCILength.GetInt());

	 int MAType = MOVAVGTYPE_LINEARREGRESSION;


	 sc.MovingAverage(sc.Close, DoNotDraw1, MAType,sc.Index, LsmaLength.GetInt());
	 sc.CCI(sc.HLCAvg, DoNotDraw2, DoNotDraw3, sc.Index, CCILength.GetInt(), 0.015f);

	 float fLSMA = DoNotDraw1[sc.Index];
	 float fCCI = DoNotDraw3[sc.Index];

	 if (BarsPointsLsma.GetInt() == 0) 
		 LsmaCCI[sc.Index] = fCCI; 
	 else if (BarsPointsLsma.GetInt() == 9999) 
		 LsmaCCI[sc.Index] = fLSMA; 
	 else
		 LsmaCCI[sc.Index] = (float)BarsPointsLsma.GetInt() / 1000.0f;

	 if (sc.Close[sc.Index] > fLSMA)
	 {
		 LsmaCCI.DataColor[sc.Index] = color0;
		 DoNotDraw4[sc.Index] = 1;
	 }
	 else if (sc.Close[sc.Index] < fLSMA)
	 {
		 LsmaCCI.DataColor[sc.Index] = color1;
		 DoNotDraw4[sc.Index] = -1;
	 }
 }

/*==========================================================================*/
 SCSFExport scsf_LSMAAboveBelow(SCStudyInterfaceRef sc)
 {
	 SCSubgraphRef Lsma = sc.Subgraph[0];
	 SCSubgraphRef DoNotDraw1 = sc.Subgraph[1];
	 SCSubgraphRef WorksheetOutput = sc.Subgraph[2];

	 SCInputRef LsmaLength = sc.Input[0];

	 if (sc.SetDefaults)
	 {
		 // Set the configuration and defaults
		 sc.GraphName ="LSMA Above/Below Last";
		 sc.GraphRegion = 1;	

		 Lsma.DrawStyle = DRAWSTYLE_SQUARE;
		 Lsma.LineWidth = 2;
		 Lsma.Name = "Above/Below";
		 Lsma.PrimaryColor = RGB(0, 255, 0);
		 Lsma.SecondaryColorUsed = 1;
		 Lsma.SecondaryColor = RGB(255, 0, 0);
		 Lsma.DrawZeros = true;

		 WorksheetOutput.Name = "Spreadsheet Output";
		 WorksheetOutput.DrawStyle = DRAWSTYLE_IGNORE;
		 WorksheetOutput.PrimaryColor = RGB(255,255,0);
		 WorksheetOutput.DrawZeros = true;

		 LsmaLength.Name = "MovAvg Length";
		 LsmaLength.SetInt(25);
		 LsmaLength.SetIntLimits(1,MAX_STUDY_LENGTH);

		 sc.AutoLoop = 1;

		 return;
	 }

	 COLORREF color0 = Lsma.PrimaryColor;
	 COLORREF color1 = Lsma.SecondaryColor;

	 sc.DataStartIndex = LsmaLength.GetInt();

	 int index = sc.Index;

	 sc.MovingAverage(sc.Close, DoNotDraw1, MOVAVGTYPE_LINEARREGRESSION, index, LsmaLength.GetInt());

	 float fLSMA = DoNotDraw1[index];

	 if (sc.Close[index] > fLSMA)
	 {
		 Lsma.DataColor[index] = color0;
		 WorksheetOutput[index] = 1;

	 }
	 else if (sc.Close[index] < fLSMA)
	 {
		 Lsma.DataColor[index] = color1;
		 WorksheetOutput[index] = -1;

	 }
	 else 
	 {
		 Lsma.DataColor[index] = 0;
		 WorksheetOutput[index] = 0;
	 }
 }
/*==========================================================================*/
 SCSFExport scsf_EMAAboveBelow(SCStudyInterfaceRef sc)
 {
	 SCSubgraphRef Ema = sc.Subgraph[0];
	 SCSubgraphRef DoNotDraw1 = sc.Subgraph[1];
	 SCSubgraphRef WorksheetOutput = sc.Subgraph[2];

	 SCInputRef LsmaLength = sc.Input[0];

	 if (sc.SetDefaults)
	 {
		 // Set the configuration and defaults
		 sc.GraphName ="EMA Above/Below Last";
		 sc.GraphRegion = 1;

		 Ema.DrawStyle = DRAWSTYLE_SQUARE;
		 Ema.LineWidth = 2;
		 Ema.Name = "Above/Below";
		 Ema.PrimaryColor = RGB(0, 255, 0);
		 Ema.SecondaryColorUsed = 1;
		 Ema.SecondaryColor = RGB(255, 0, 0);
		 Ema.DrawZeros = true;

		 WorksheetOutput.Name = "Spreadsheet Output";
		 WorksheetOutput.DrawStyle= DRAWSTYLE_IGNORE;
		 WorksheetOutput.PrimaryColor = RGB(255, 255, 0);
		 WorksheetOutput.DrawZeros = true;

		 LsmaLength.Name = "MovAvg Length";
		 LsmaLength.SetInt(25);
		 LsmaLength.SetIntLimits(1,MAX_STUDY_LENGTH);

		 sc.AutoLoop = 1;

		 return;
	 }

	 COLORREF color0 = Ema.PrimaryColor;
	 COLORREF color1 = Ema.SecondaryColor;

	 sc.DataStartIndex = LsmaLength.GetInt();

	 int index = sc.Index;

	 sc.MovingAverage(sc.Close, DoNotDraw1, MOVAVGTYPE_EXPONENTIAL, index, LsmaLength.GetInt());


	 float fLSMA = DoNotDraw1[index];

	 if (sc.Close[index] > fLSMA)
	 {
		 Ema.DataColor[index] = color0;
		 WorksheetOutput[index] = 1;

	 }
	 else if (sc.Close[index] < fLSMA)
	 {
		 Ema.DataColor[index] = color1;
		 WorksheetOutput[index] = -1;

	 }
	 else 
	 {
		 Ema.DataColor[index] = 0;
		 WorksheetOutput[index] = 0;
	 }
 }

/*==========================================================================*/
 SCSFExport scsf_MurrayMath(SCStudyInterfaceRef sc)
 {
	 const int NumberOfVisibleLines = 13;

	 SCInputRef SquareWidth = sc.Input[0];
	 SCInputRef UseLastBarAsEndDT = sc.Input[1];
	 SCInputRef EndDT = sc.Input[2];

	 // Configuration
	 if (sc.SetDefaults)
	 {   
		 sc.GraphName = "Murray Math"; 
		 sc.AutoLoop = 1;  
		 sc.GraphRegion = 0; 

		 for (int i = 0; i< NumberOfVisibleLines; i++)
		 {
			 SCString str;

			 if(i<=10)
				 str.Format("L %d/8", i-2);
			 else
				 str.Format("L +%d/8", i-10);

			 sc.Subgraph[i].Name = str;
			 sc.Subgraph[i].DrawStyle = DRAWSTYLE_LINE;
			 sc.Subgraph[i].LineWidth = 2;

			 if((i != 2) && (i != 10))
			 {
				 sc.Subgraph[i].LineStyle = 	LINESTYLE_DASH;
				 sc.Subgraph[i].LineWidth = 0;
			 }

			 sc.Subgraph[i].PrimaryColor = RGB(200,200,200);
			 sc.Subgraph[i].LineLabel |= LL_NAME_ALIGN_RIGHT|LL_DISPLAY_NAME|LL_NAME_ALIGN_CENTER;

		 }

		 SquareWidth.Name = "Square Width";
		 SquareWidth.SetInt(64);             

		 UseLastBarAsEndDT.Name = "Use Last Bar as End Date-Time";
		 UseLastBarAsEndDT.SetYesNo(true);

		 EndDT.Name = "End Date-Time";
		 EndDT.SetDateTime(0);

		 return;
	 }   

	 if (!sc.AdvancedFeatures )
	 {
		 if (sc.Index == 0)
		 {
			 sc.AddMessageToLog(SPECIAL_FEATURES,1);
		 }

		 return;
	 }

	 // if user set End Date and current bar not on this date don't do anything
	 if(UseLastBarAsEndDT.GetYesNo() == 0)
	 {
		 if( sc.BaseDateTimeIn[sc.Index] != EndDT.GetDateTime())
			 return;
	 }

	 double HIGH =(double) sc.GetHighest(sc.High, sc.Index, SquareWidth.GetInt());
	 double LOW = (double) sc.GetLowest(sc.Low, sc.Index, SquareWidth.GetInt());

	 double PriceRange;
	 double SR, OctaveCount, Si, M, B, TT, Er, MM, NN;
	 float Line[NumberOfVisibleLines];	
	 int I;

	 PriceRange = HIGH - LOW;

	 if(HIGH > 25)
	 {
		 if(log(0.4*HIGH)/log(10.0)-int(log(0.4*HIGH)/log(10.0))>0)
			 SR = exp(log(10.0)*(int(log(0.4*HIGH)/log(10.0))+1));
		 else
			 SR = exp(log(10.0)*(int(log(0.4*HIGH)/log(10.0))));
	 }
	 else 
	 {
		 SR = 100*exp(log(8.0)*(int(log(0.005*HIGH)/log(8.0))));
	 }

	 double RangeMMI =  log(SR/PriceRange)/log(8.0);
	 if(RangeMMI<=0)
	 {
		 OctaveCount = 0;
	 }
	 else
	 {
		 if(log(SR/PriceRange)/log(8.0)- fmod(log(SR/PriceRange),log(8.0))== 0)
		 {
			 OctaveCount = int(RangeMMI);
		 }
		 else
		 {
			 OctaveCount = int(RangeMMI)+1.0;
		 }
	 }


	 Si = SR *exp(-OctaveCount * log(8.0));
	 M = int(((1/log(2.0))*log(PriceRange/Si))+.00001);

	 double TempDoubleToRound = (((HIGH + LOW) * .5)/(Si* exp((M-1)*log(2.0))));
	 I = (int)(TempDoubleToRound >0 ? TempDoubleToRound + 0.5 : TempDoubleToRound - 0.5);

	 B =(I-1) * Si * exp((M-1)* log(2.0));
	 TT =(I+1) * Si * exp((M-1) * log(2.0));

	 Er = (HIGH - TT > 0.25 *(TT-B)) || (B - LOW > 0.25 * (TT-B))? 1:0;

	 if(Er == 0)
	 {
		 MM = M;
		 NN = OctaveCount;
	 }
	 else 
		 if((Er == 1) && (M < 2))
		 {
			 MM = M +1;
			 NN = OctaveCount;
		 }
		 else 
		 {
			 MM = 0;
			 NN = OctaveCount -1;
		 }

		 Si = SR * exp(-NN *log(8.0));

		 TempDoubleToRound = ((HIGH+ LOW )*.5)/	(Si* exp((MM-1)* log(2.0)));
		 I = (int)(TempDoubleToRound >0 ? TempDoubleToRound + 0.5 : TempDoubleToRound - 0.5);

		 for (int n = 0; n < NumberOfVisibleLines; n++)
		 {
			 Line[n] = (float)(B + ((n-2) * 0.125 * (TT -B)));

			 if(UseLastBarAsEndDT.GetYesNo() == 0)
			 {
				 for(int i = sc.CurrentIndex-SquareWidth.GetInt()+1; i <= sc.CurrentIndex; i++)
					 sc.Subgraph[n][i] = Line[n];
			 }
			 else
			 {
				 if(sc.CurrentIndex > SquareWidth.GetInt())
				 {
					 for(int i = sc.CurrentIndex-SquareWidth.GetInt()+1; i <= sc.CurrentIndex; i++)
						 sc.Subgraph[n][i] = Line[n];

					 sc.Subgraph[n][sc.CurrentIndex - SquareWidth.GetInt()] = 0;
				 }
			 }
		 }
 }




 /*==========================================================================*/
 SCSFExport scsf_PreviousCloseLine(SCStudyInterfaceRef sc)
 {
	 SCSubgraphRef PCL = sc.Subgraph[0];

	 SCInputRef NumberOfBarsBack = sc.Input[0];


	 if(sc.SetDefaults)
	 {
		 sc.GraphName = "Previous Close Line";

		 PCL.Name = "PCL";
		 PCL.DrawStyle = DRAWSTYLE_LINE;

		 
		 PCL.LineStyle = LINESTYLE_DOT;
		 PCL.PrimaryColor = RGB(0, 255, 0);
		 PCL.LineWidth = 2;
		 PCL.DrawZeros = false;

		 sc.GraphRegion = 0;

		 NumberOfBarsBack.Name = "Number of Bars to Display On.  Use 0 for current data only.";
		 NumberOfBarsBack.SetInt(0);
		 NumberOfBarsBack.SetIntLimits(0,MAX_STUDY_LENGTH);
		 
		 //During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		 sc.FreeDLL = 0;
		 return;
	 }

	 int CurrentDate = sc.BaseDateTimeIn[sc.ArraySize-1].GetDate();
	 int NumberBarsBack = NumberOfBarsBack.GetInt();

	 
	 if (NumberBarsBack == 0)
	 {
		 for (int pos = sc.ArraySize - 1; sc.BaseDateTimeIn[pos].GetDate() == CurrentDate && pos >= 0; pos--)
		 {
			 PCL[pos] =sc.SymbolData->SettlementPrice;
		 }
	 }
	 else
	 {
		 int StartingIndex = sc.ArraySize - NumberBarsBack;

		 for (int pos = sc.UpdateStartIndex - NumberBarsBack; pos < sc.ArraySize; ++pos)
		 {
			 if(pos >= StartingIndex)
				 break;

			 PCL[pos] = 0;
		 }

		 for (int pos = sc.ArraySize - 1; NumberBarsBack > 0 && pos >=0; pos--)
		 {
			 PCL[pos] = sc.SymbolData->SettlementPrice;
			 NumberBarsBack--;
		 }
	 }
 }

/*==========================================================================*/
 SCSFExport scsf_ValueChartLevels(SCStudyInterfaceRef sc)
 {
	 SCInputRef ModOBLevel = sc.Input[1];
	 SCInputRef SigOBLevel = sc.Input[2];

	 SCSubgraphRef PlusModerateOverboughtLevel = sc.Subgraph[0];
	 SCSubgraphRef PlusSignificantOverboughtLevel= sc.Subgraph[1];
	 SCSubgraphRef  ZeroLevel= sc.Subgraph[2];
	 SCSubgraphRef MinusModerateOverboughtLevel= sc.Subgraph[3];
	 SCSubgraphRef MinusSignificantOverboughtLevel= sc.Subgraph[4];

	 if (sc.SetDefaults)
	 {
		 // Set the configuration and defaults
		 sc.GraphName ="Value Chart Levels";

		 sc.GraphDrawType = GDT_CUSTOM;
		 sc.AutoLoop = 1;
		 sc.FreeDLL = 0;



		 ModOBLevel.Name = "Moderately Overbought Level";
		 ModOBLevel.SetFloat(4.0f);
		 ModOBLevel.SetFloatLimits(0,100.0f);

		 SigOBLevel.Name = "Significantly Overbought Level";
		 SigOBLevel.SetFloat(8.0f);
		 SigOBLevel.SetFloatLimits(0,100.0f);

		 PlusModerateOverboughtLevel.Name = "+ Moderate Overbought Level";
		 PlusModerateOverboughtLevel.PrimaryColor = RGB(0,255,0);
		 PlusModerateOverboughtLevel.DrawStyle = DRAWSTYLE_LINE;
		 PlusModerateOverboughtLevel.DisplayNameValueInWindowsFlags= 0;
		
		 PlusSignificantOverboughtLevel.Name = "+ Significant Overbought Level";
		 PlusSignificantOverboughtLevel.PrimaryColor = RGB(255,0,0);
		 PlusSignificantOverboughtLevel.DrawStyle = DRAWSTYLE_LINE;
		 PlusSignificantOverboughtLevel.DisplayNameValueInWindowsFlags= 0;
		 
		 MinusModerateOverboughtLevel.Name = "- Moderate Overbought Level";
		 MinusModerateOverboughtLevel.PrimaryColor = RGB(0,255,0);
		 MinusModerateOverboughtLevel.DrawStyle = DRAWSTYLE_LINE;
		 MinusModerateOverboughtLevel.DisplayNameValueInWindowsFlags= 0;

		 MinusSignificantOverboughtLevel.Name = "- Significant Overbought Level";
		 MinusSignificantOverboughtLevel.PrimaryColor = RGB(255,0,0);
		 MinusSignificantOverboughtLevel.DrawStyle = DRAWSTYLE_LINE;
		 MinusSignificantOverboughtLevel.DisplayNameValueInWindowsFlags= 0;

		 ZeroLevel.Name = "Zero Level";
		 ZeroLevel.PrimaryColor = RGB(128,128,128);
		 ZeroLevel.DrawStyle = DRAWSTYLE_LINE;
		 ZeroLevel.DisplayNameValueInWindowsFlags= 0;
		 ZeroLevel.DrawZeros = 1;
		

	 }

	 PlusModerateOverboughtLevel[sc.Index] = ModOBLevel.GetFloat();
	 PlusSignificantOverboughtLevel[sc.Index] = SigOBLevel.GetFloat();

	 MinusModerateOverboughtLevel[sc.Index] = ModOBLevel.GetFloat() * -1.0f;
	 MinusSignificantOverboughtLevel[sc.Index] = SigOBLevel.GetFloat() * -1.0f;


 }

/************************************************************************/
SCSFExport scsf_ValueChart(SCStudyInterfaceRef sc)
{
	SCInputRef Length     = sc.Input[0];

	SCSubgraphRef VcOpen  = sc.Subgraph[0];
	SCSubgraphRef VcHigh  = sc.Subgraph[1];
	SCSubgraphRef VcLow   = sc.Subgraph[2];
	SCSubgraphRef VcClose = sc.Subgraph[3];

	SCFloatArrayRef VcHLMA = VcOpen.Arrays[1];
	SCFloatArrayRef TR     = VcOpen.Arrays[2];
	SCFloatArrayRef ATR    = VcOpen.Arrays[3];
	SCFloatArrayRef HH     = VcOpen.Arrays[4];
	SCFloatArrayRef LL     = VcOpen.Arrays[5];


	if (sc.SetDefaults)
	{
		// Set the configuration and defaults
		sc.GraphName ="Value Chart";

		sc.GraphDrawType = GDT_CANDLESTICK;
		sc.AutoLoop = 1;
		sc.FreeDLL = 0;

		// Inputs
		Length.Name = "Length";
		Length.SetInt(5);
		Length.SetIntLimits(2,10000);

		// Subgraphs
		VcOpen.Name = "VC Open";
		VcOpen.DrawStyle = DRAWSTYLE_LINE;
		VcOpen.PrimaryColor = RGB(0,255,0);
		VcOpen.SecondaryColorUsed = true;
		VcOpen.SecondaryColor = RGB(0,255,0);
		VcOpen.DrawZeros = 1;

		VcHigh.Name = "VC High";
		VcHigh.DrawStyle = DRAWSTYLE_LINE;
		VcHigh.PrimaryColor = RGB(128,255,128);
		VcHigh.DrawZeros = 1;

		VcLow.Name = "VC Low";
		VcLow.DrawStyle = DRAWSTYLE_LINE;
		VcLow.PrimaryColor = RGB(255,0,0);
		VcLow.SecondaryColor = RGB(255,0,0);
		VcLow.SecondaryColorUsed = true;
		VcLow.DrawZeros = 1;

		VcClose.Name = "VC Close";
		VcClose.DrawStyle = DRAWSTYLE_LINE;
		VcClose.PrimaryColor = RGB(255,128,128);
		VcClose.DrawZeros = 1;

		return;
	}

	
	
	int NumBars = Length.GetInt();
	float LRange = 1;

	if (NumBars > 7)
	{
		int VarNumBars = sc.Round((float)NumBars / 5.0f);

		sc.Highest(sc.High, HH, VarNumBars);
		sc.Lowest(sc.Low, LL, VarNumBars);

		float R1 = HH[sc.Index] - LL[sc.Index];
		if (R1 == 0 && VarNumBars == 1)
			R1 = abs(sc.Close[sc.Index] - sc.Close[sc.Index-VarNumBars]);

		float R2 = HH[sc.Index-VarNumBars-1] - LL[sc.Index-VarNumBars];
		if (R2 == 0 && VarNumBars == 1)
			R2 = abs(sc.Close[sc.Index-VarNumBars] - sc.Close[sc.Index-(VarNumBars*2)]);

		float R3 = HH[sc.Index-(VarNumBars*2)] - LL[sc.Index-(VarNumBars*2)];
		if (R3 == 0 && VarNumBars == 1)
			R3 = abs(sc.Close[sc.Index-(VarNumBars*2)] - sc.Close[sc.Index-(VarNumBars*3)]);

		float R4 = HH[sc.Index-(VarNumBars*3)] - LL[sc.Index-(VarNumBars*3)];
		if (R4 == 0 && VarNumBars == 1)
			R4 = abs(sc.Close[sc.Index-(VarNumBars*3)] - sc.Close[sc.Index-(VarNumBars*4)]);

		float R5 = HH[sc.Index-(VarNumBars*4)] - LL[sc.Index-(VarNumBars*4)];
		if (R5 == 0 && VarNumBars == 1)
			R5 = abs(sc.Close[sc.Index-(VarNumBars*4)] - sc.Close[sc.Index-(VarNumBars*5)]);

		LRange = (R1 + R2 + R3 + R4 + R5) / 5 / 5;
	}
	else
	{
		sc.ATR(sc.BaseDataIn, TR, ATR, 5, MOVAVGTYPE_SIMPLE);
		LRange = ATR[sc.Index] / 5;
	}


	sc.MovingAverage(sc.HLAvg, VcHLMA, MOVAVGTYPE_SIMPLE, Length.GetInt());
	float AvgVcHL = VcHLMA[sc.Index];

	VcOpen[sc.Index]  = (sc.Open[sc.Index]  - AvgVcHL) / LRange;
	VcHigh[sc.Index]  = (sc.High[sc.Index]  - AvgVcHL) / LRange;
	VcLow[sc.Index]   = (sc.Low[sc.Index]   - AvgVcHL) / LRange;
	VcClose[sc.Index] = (sc.Close[sc.Index] - AvgVcHL) / LRange;
}

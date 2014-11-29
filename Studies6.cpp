#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
// Windows header file
#include <windows.h>

#include "sierrachart.h"
#include "scstudyfunctions.h"

SCSFExport scsf_SumAllChartsBar(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Open = sc.Subgraph[0];
	SCSubgraphRef High = sc.Subgraph[1];
	SCSubgraphRef Low = sc.Subgraph[2];
	SCSubgraphRef Last = sc.Subgraph[3];
	SCSubgraphRef Volume = sc.Subgraph[4];
	SCSubgraphRef OpenInterest = sc.Subgraph[5];
	SCSubgraphRef OHLCAvg = sc.Subgraph[6];
	SCSubgraphRef HLCAvg = sc.Subgraph[7];
	SCSubgraphRef HLAvg = sc.Subgraph[8];
	SCSubgraphRef BidVol = sc.Subgraph[9];
	SCSubgraphRef AskVol = sc.Subgraph[10];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Sum All Charts (Bar)" ;

		sc.StudyDescription = "Sum All Charts (Bar).";

		sc.GraphDrawType = GDT_OHLCBAR;
		sc.StandardChartHeader = 1;
		
		sc.ValueFormat = 2;

		Open.Name = "Open";
		Open.DrawStyle = DRAWSTYLE_LINE;
		Open.PrimaryColor = RGB(0,255,0);
		Open.DrawZeros = false;

		High.Name = "High";
		High.DrawStyle = DRAWSTYLE_LINE;
		High.PrimaryColor = RGB(0,255,0);
		High.DrawZeros = false;

		Low.Name = "Low";
		Low.DrawStyle = DRAWSTYLE_LINE;
		Low.PrimaryColor = RGB(0,255,0);
		Low.DrawZeros = false;

		Last.Name = "Last";
		Last.DrawStyle = DRAWSTYLE_LINE;
		Last.PrimaryColor = RGB(0,255,0);
		Last.DrawZeros = false;

		Volume.Name = "Volume";
		Volume.DrawStyle = DRAWSTYLE_IGNORE;
		Volume.PrimaryColor = RGB(255,0,0);
		Volume.DrawZeros = false;

		OpenInterest.Name = "# of Trades / OI";
		OpenInterest.DrawStyle = DRAWSTYLE_IGNORE;
		OpenInterest.PrimaryColor = RGB(0,0,255);
		OpenInterest.DrawZeros = false;

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

		return;
	}


	// Initialize the data in the destination subgraphs to 0 before looping through the charts.
	
	for (int DataIndex = sc.UpdateStartIndex; DataIndex < sc.ArraySize; DataIndex++)
	{
		// Loop through the subgraphs
		for (int SubgraphIndex = 0; SubgraphIndex < 6; SubgraphIndex++)
		{
			sc.Subgraph[SubgraphIndex][DataIndex]	= 0;
		}
	}


	// Loop through the charts
	for (int ChartIndex = 1; ChartIndex < 201; ChartIndex++)
	{
		SCGraphData ReferenceArrays;
		sc.GetChartBaseData(-ChartIndex, ReferenceArrays);
		
		if (ReferenceArrays[SC_OPEN].GetArraySize() < 1)
			continue;
		
		
		// Loop through the data indexes
		for (int DataIndex = sc.UpdateStartIndex; DataIndex < sc.ArraySize; DataIndex++)
		{

			// Get the matching index 
			int RefDataIndex = sc.GetNearestMatchForDateTimeIndex(ChartIndex, DataIndex);

			// Loop through the subgraphs
			for (int SubgraphIndex = 0; SubgraphIndex < 6; SubgraphIndex++)
			{
				// Add in the value of the current chart
				sc.Subgraph[SubgraphIndex][DataIndex]
				+= ReferenceArrays[SubgraphIndex][RefDataIndex];
			}
			sc.CalculateOHLCAverages(DataIndex);
		}
	}

	return;

}

/************************************************************************/


SCSFExport scsf_MultiplyAllChartsBar(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Open = sc.Subgraph[0];
	SCSubgraphRef High = sc.Subgraph[1];
	SCSubgraphRef Low = sc.Subgraph[2];
	SCSubgraphRef Last = sc.Subgraph[3];
	SCSubgraphRef Volume = sc.Subgraph[4];
	SCSubgraphRef OpenInterest = sc.Subgraph[5];
	SCSubgraphRef OHLCAvg = sc.Subgraph[6];
	SCSubgraphRef HLCAvg = sc.Subgraph[7];
	SCSubgraphRef HLAvg = sc.Subgraph[8];
	SCSubgraphRef BidVol = sc.Subgraph[9];
	SCSubgraphRef AskVol = sc.Subgraph[10];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Multiply All Charts" ;

		sc.GraphDrawType = GDT_OHLCBAR;
		sc.StandardChartHeader = 1;

		sc.ValueFormat = 2;

		Open.Name = "Open";
		Open.DrawStyle = DRAWSTYLE_LINE;
		Open.PrimaryColor = RGB(0,255,0);
		Open.DrawZeros = false;

		High.Name = "High";
		High.DrawStyle = DRAWSTYLE_LINE;
		High.PrimaryColor = RGB(0,255,0);
		High.DrawZeros = false;

		Low.Name = "Low";
		Low.DrawStyle = DRAWSTYLE_LINE;
		Low.PrimaryColor = RGB(0,255,0);
		Low.DrawZeros = false;

		Last.Name = "Last";
		Last.DrawStyle = DRAWSTYLE_LINE;
		Last.PrimaryColor = RGB(0,255,0);
		Last.DrawZeros = false;

		Volume.Name = "Volume";
		Volume.DrawStyle = DRAWSTYLE_IGNORE;
		Volume.PrimaryColor = RGB(0,0,255);
		Volume.DrawZeros = false;

		OpenInterest.Name = "# of Trades / OI";
		OpenInterest.DrawStyle = DRAWSTYLE_IGNORE;
		OpenInterest.PrimaryColor = RGB(127,0,255);
		OpenInterest.DrawZeros = false;

		OHLCAvg.Name = "OHLC Avg";
		OHLCAvg.DrawStyle = DRAWSTYLE_IGNORE;
		OHLCAvg.PrimaryColor = RGB(0,255,0);
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

		return;
	}


	// Initialize the data in the destination subgraphs to 1.0 before looping through the charts.
	for (int DataIndex = sc.UpdateStartIndex; DataIndex < sc.ArraySize; DataIndex++)
	{
		// Loop through the subgraphs
		for (int SubgraphIndex = 0; SubgraphIndex < 6; SubgraphIndex++)
		{
			sc.Subgraph[SubgraphIndex][DataIndex]	= 1.0;
		}
	}


	// Loop through the charts. Up to 200.
	for (int ChartIndex = 1; ChartIndex < 201; ChartIndex++)
	{
		SCGraphData ReferenceArrays;
		sc.GetChartBaseData(-ChartIndex, ReferenceArrays);

		if (ReferenceArrays[SC_OPEN].GetArraySize() < 1)
			continue;


		// Loop through the data indexes
		for (int DataIndex = sc.UpdateStartIndex; DataIndex < sc.ArraySize; DataIndex++)
		{

			// Get the matching index 
			int RefDataIndex = sc.GetNearestMatchForDateTimeIndex(ChartIndex, DataIndex);

			// Loop through the subgraphs
			for (int SubgraphIndex = 0; SubgraphIndex < 6; SubgraphIndex++)
			{
				// Add in the value of the current chart
				sc.Subgraph[SubgraphIndex][DataIndex]
				*= ReferenceArrays[SubgraphIndex][RefDataIndex];
			}
			sc.CalculateOHLCAverages(DataIndex);
		}
	}

	return;

}

/**********************************************************************/
SCSFExport scsf_SumChartsFromList(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Open = sc.Subgraph[0];
	SCSubgraphRef High = sc.Subgraph[1];
	SCSubgraphRef Low = sc.Subgraph[2];
	SCSubgraphRef Last = sc.Subgraph[3];
	SCSubgraphRef Volume = sc.Subgraph[4];
	SCSubgraphRef OpenInterest = sc.Subgraph[5];
	SCSubgraphRef OHLCAvg = sc.Subgraph[6];
	SCSubgraphRef HLCAvg = sc.Subgraph[7];
	SCSubgraphRef HLAvg = sc.Subgraph[8];
	SCSubgraphRef BidVol = sc.Subgraph[9];
	SCSubgraphRef AskVol = sc.Subgraph[10];

	SCInputRef Divisor = sc.Input[0];
	SCInputRef SyncCharts = sc.Input[1];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Sum Charts From List" ;

		sc.StudyDescription = "Sum Charts From List.";

		sc.GraphDrawType = GDT_OHLCBAR;
		sc.StandardChartHeader = 1;
		
		sc.ValueFormat = 2;

		Open.Name = "Open";
		Open.DrawStyle = DRAWSTYLE_LINE;
		Open.PrimaryColor = RGB(0,255,0);
		Open.DrawZeros = false;

		High.Name = "High";
		High.DrawStyle = DRAWSTYLE_LINE;
		High.PrimaryColor = RGB(0,255,0);
		High.DrawZeros = false;

		Low.Name = "Low";
		Low.DrawStyle = DRAWSTYLE_LINE;
		Low.PrimaryColor = RGB(0,255,0);
		Low.DrawZeros = false;

		Last.Name = "Last";
		Last.DrawStyle = DRAWSTYLE_LINE;
		Last.PrimaryColor = RGB(0,255,0);
		Last.DrawZeros = false;

		Volume.Name = "Volume";
		Volume.DrawStyle = DRAWSTYLE_IGNORE;
		Volume.PrimaryColor = RGB(255,0,0);
		Volume.DrawZeros = false;

		OpenInterest.Name = "# of Trades / OI";
		OpenInterest.DrawStyle = DRAWSTYLE_IGNORE;
		OpenInterest.PrimaryColor = RGB(0,0,255);
		OpenInterest.DrawZeros = false;

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


		Divisor.Name = "Divisor. Use 0 for divisor equal to number of charts.";
		Divisor.SetFloat(1);
		
		SyncCharts.Name = "Synchronize charts";
		SyncCharts.SetYesNo(false);


		// During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;

		sc.TextInputName = "List of Chart Numbers (comma separated)";

		return;
	}

	// Do data processing

	int Sync = 1;
	 
	 if(SyncCharts.GetYesNo())
		 Sync = -1;

	// Make a copy of the list of chart numbers from the text input
	SCString ChartNumberList = sc.TextInput;

	// Get each of the chart numbers (from the text input) and put them into a
	// vector.  
	std::vector<int> ChartNumbers;
	std::vector<float> ChartMultipliers;
	
	char* Token = strtok((char *)ChartNumberList.GetChars(), ",");
	while (Token != NULL)
	{
		ChartNumbers.push_back(atoi(Token));

		float Multiplier = 0.0;
		char* FoundMultiplier = strchr(Token, '*');
		if (FoundMultiplier != NULL)
			Multiplier =(float) atof(FoundMultiplier + 1);
		
		if (Multiplier == 0.0)
			Multiplier = 1.0;
		
		ChartMultipliers.push_back(Multiplier);

		// Get the next chart number
		Token = strtok(NULL, ",");
	}
	
	float DivisorValue = Divisor.GetFloat();
	if (DivisorValue == 0)
		DivisorValue = (float)ChartNumbers.size();

	int Index = 0;
	for (std::vector<int>::iterator it = ChartNumbers.begin(); it != ChartNumbers.end(); ++it, ++Index)
	{
		int ChartNumber = *it;
		float ChartMultiplier = ChartMultipliers[Index];

		
		
		SCGraphData ReferenceArrays;
		sc.GetChartBaseData(ChartNumber*Sync, ReferenceArrays);
		
		if (ReferenceArrays[SC_OPEN].GetArraySize() < 1)
			continue;
		
		
		// Loop through the data indexes
		for (int DataIndex = sc.UpdateStartIndex; DataIndex < sc.ArraySize; DataIndex++)
		{
			// Get the matching index 
			int RefDataIndex = sc.GetNearestMatchForDateTimeIndex(ChartNumber, DataIndex);

			// Loop through the subgraphs
			for (int SubgraphIndex = 0; SubgraphIndex <= SC_LAST; SubgraphIndex++)
			{
				// Add in the value of the current chart
				if (Index == 0)
				{
					sc.Subgraph[SubgraphIndex][DataIndex] = ReferenceArrays[SubgraphIndex][RefDataIndex] * ChartMultiplier / DivisorValue;
				}
				else
				{
					sc.Subgraph[SubgraphIndex][DataIndex] += ReferenceArrays[SubgraphIndex][RefDataIndex] * ChartMultiplier / DivisorValue;

				}

			}

			sc.Subgraph[ SC_HIGH][DataIndex] = max(sc.Subgraph[SC_OPEN][DataIndex],
				max(sc.Subgraph[SC_HIGH][DataIndex],
				max(sc.Subgraph[SC_LOW][DataIndex],sc.Subgraph[SC_LAST][DataIndex])
				)
				);

			sc.Subgraph[SC_LOW][DataIndex] = min(sc.Subgraph[SC_OPEN][DataIndex],
				min(sc.Subgraph[SC_HIGH][DataIndex],
				min(sc.Subgraph[SC_LOW][DataIndex],sc.Subgraph[SC_LAST][DataIndex])
				)
				);

			// Loop through the subgraphs
			for (int SubgraphIndex = SC_VOLUME; SubgraphIndex <= SC_NT; SubgraphIndex++)
			{
				// Add in the value of the current chart
				if (Index == 0)
					sc.Subgraph[SubgraphIndex][DataIndex] = ReferenceArrays[SubgraphIndex][RefDataIndex] * (1.0f / ChartMultiplier) / DivisorValue;
				else
					sc.Subgraph[SubgraphIndex][DataIndex] += ReferenceArrays[SubgraphIndex][RefDataIndex] * (1.0f / ChartMultiplier) / DivisorValue;

			}

			sc.CalculateOHLCAverages(DataIndex);
		}
	}
}

/*============================================================================
----------------------------------------------------------------------------*/
SCSFExport scsf_BidAskVolumeRatio(SCStudyInterfaceRef sc)
{
	SCSubgraphRef VolRatioAvg = sc.Subgraph[0];
	SCSubgraphRef ZeroLine = sc.Subgraph[1];

	SCFloatArrayRef ExtraArrayRatio = sc.Subgraph[0].Arrays[0];

	SCInputRef InMALength = sc.Input[0];
	SCInputRef InMAType = sc.Input[1];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Bid Ask Volume Ratio";

		sc.StudyDescription = "Bid Ask Volume Ratio";
		
		sc.AutoLoop = 1;

		//During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;

		VolRatioAvg.Name = "Vol Ratio Avg";
		VolRatioAvg.DrawStyle = DRAWSTYLE_LINE;
		VolRatioAvg.PrimaryColor = RGB(0,255,0);
		VolRatioAvg.DrawZeros = false; 

		ZeroLine.Name = "Zero Line";
		ZeroLine.DrawStyle = DRAWSTYLE_LINE;
		ZeroLine.PrimaryColor = RGB(255,0,255);
		ZeroLine.DrawZeros = true; 
		
		InMALength.Name = "Length";
		InMALength.SetInt(14);
		InMALength.SetIntLimits(1,MAX_STUDY_LENGTH);


		InMAType.Name= "Moving Average Type";
		InMAType.SetMovAvgType(MOVAVGTYPE_EXPONENTIAL);
		
		return;
	}

	//if(sc.GetBarHasClosedStatus() == BHCS_BAR_HAS_NOT_CLOSED)
		//return;

	// Do data processing

	float UpVol = sc.AskVolume[sc.Index];
	float DownVol = sc.BidVolume[sc.Index];

	float TotalVol = UpVol + DownVol;
	if (TotalVol > 0)
		ExtraArrayRatio[sc.Index] = 100.0f * (UpVol - DownVol) / TotalVol;
	else
		ExtraArrayRatio[sc.Index] = 0.0f;
	
	sc.MovingAverage(ExtraArrayRatio, VolRatioAvg, InMAType.GetMovAvgType(), InMALength.GetInt());
}

/*============================================================================
Refer to Technical Studies Reference for more information about this study.
----------------------------------------------------------------------------*/
SCSFExport scsf_StudyAngle(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Angle = sc.Subgraph[0];
	SCSubgraphRef ZeroLine = sc.Subgraph[1];
	
	SCInputRef InInputData = sc.Input[0];
	SCInputRef InLength = sc.Input[1];
	SCInputRef InValuePerPoint = sc.Input[2];
	
	if (sc.SetDefaults)
	{
		// Set the configuration and defaults
		
		sc.GraphName = "Study Angle";
		
		
		sc.AutoLoop = 1;
		
		//During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;
		
		Angle.Name = "Angle";
		Angle.DrawStyle = DRAWSTYLE_LINE;
		Angle.PrimaryColor = RGB(0,255,0);
		Angle.DrawZeros= false;
		
		ZeroLine.Name = "Zero Line";
		ZeroLine.DrawStyle = DRAWSTYLE_LINE;
		ZeroLine.PrimaryColor = RGB(255,0,255);
		ZeroLine.DrawZeros= true;
		
		InInputData.Name = "Input Data";
		InInputData.SetInputDataIndex(0);
		
		InLength.Name = "Length";
		InLength.SetInt(10);
		InLength.SetIntLimits(1,MAX_STUDY_LENGTH);
		
		InValuePerPoint.Name = "Value Per Point";
		InValuePerPoint.SetFloat(1.0f);
		
		return;
	}
	
	int Length = InLength.GetInt();
	
	sc.DataStartIndex = Length;
	
	if (InValuePerPoint.GetFloat() == 0.0f)
		InValuePerPoint.SetFloat(0.01f);
	
	
	// Do data processing
	
	
	SCFloatArrayRef InData = sc.BaseData[InInputData.GetInputDataIndex()];
	
	float BackValue = InData[sc.Index - Length];
	float CurrentValue = InData[sc.Index];
	
	float PointChange = (CurrentValue - BackValue) / InValuePerPoint.GetFloat();
	
	Angle[sc.Index] = (float)(atan2((double)PointChange, (double)Length) * 180.0/M_PI);
}

/*==========================================================================*/
SCSFExport scsf_DetrendedOscillator(SCStudyInterfaceRef sc)
{
	// Section 1 - Set the configuration variables
	
	SCSubgraphRef DPO = sc.Subgraph[0];
	
	SCSubgraphRef overbought = sc.Subgraph[1];
	SCSubgraphRef oversold = sc.Subgraph[2];
	
	SCFloatArrayRef MA = sc.Subgraph[0].Arrays[0];
	
	SCInputRef I_Length = sc.Input[0];
	SCInputRef I_overbought = sc.Input[1];
	SCInputRef I_oversold = sc.Input[2];
	SCInputRef I_MovingAverageType = sc.Input[3];
	
	if (sc.SetDefaults)
	{
		// Set the configuration and defaults
		
		sc.GraphName = "Detrended Oscillator";
		
		sc.StudyDescription = "Detrended Oscillator.";
		sc.FreeDLL = 0;
		
		sc.AutoLoop = 1;  // true
		
		DPO.Name = "DO";
		DPO.DrawStyle = DRAWSTYLE_LINE;
		DPO.PrimaryColor = RGB(0,255,0);
		
		overbought.Name = "Level 1";
		overbought.DrawStyle = DRAWSTYLE_LINE;
		overbought.PrimaryColor = RGB(255,255,0);        
		
		oversold.Name = "Level 2";
		oversold.DrawStyle = DRAWSTYLE_LINE;
		oversold.PrimaryColor = RGB(255,255,0);
		
		I_Length.Name = "Length";
		I_Length.SetInt(20);
		
		I_overbought.Name = "Overbought Level";
		I_overbought.SetFloat(0.5);
		
		I_oversold.Name = "Oversold Level";
		I_oversold.SetFloat(-0.5);
		
		I_MovingAverageType.Name = "Moving Average Type";
		I_MovingAverageType.SetMovAvgType(MOVAVGTYPE_SIMPLE);
		
		return;
	}
	
	
	// Section 2 - Do data processing here
	
	sc.DataStartIndex = I_Length.GetInt();
	
	int N = I_Length.GetInt()/2;
	
	overbought[sc.Index] = I_overbought.GetFloat();
	oversold[sc.Index] = I_oversold.GetFloat();
	
	
	sc.MovingAverage(sc.Close, MA, I_MovingAverageType.GetMovAvgType() , sc.Index, N);
	
	int N2 = (N/2) + 1;
	
	DPO[sc.Index] = (sc.Close[sc.Index] - MA[sc.Index - N2]); 
}

//-----------------------------------------------------------------------------
SCSFExport scsf_RangeBarPredictor(SCStudyInterfaceRef sc)
{
	// Section 1 - Set the configuration variables

	SCSubgraphRef High =     sc.Subgraph[0];
	SCSubgraphRef Low  =     sc.Subgraph[1];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Range Bar Predictor";

		sc.StudyDescription = "Range Bar Predictor.";

		sc.FreeDLL = 0;

		sc.GraphRegion = 0;

		sc.AutoLoop = 1;  // true

		High.Name = "Predicted High";
		High.DrawStyle = DRAWSTYLE_DASH;
		//High.LineStyle = LINESTYLE_DASH;
		High.PrimaryColor = RGB(0,255,0);
		High.LineWidth = 2;
		High.DrawZeros = false;

		Low.Name = "Predicted Low";
		Low.DrawStyle = DRAWSTYLE_DASH;
		//Low.LineStyle = LINESTYLE_DASH;
		Low.PrimaryColor = RGB(255,0,0);
		Low.LineWidth = 2;
		Low.DrawZeros = false;

		return;
	}

	// Section 2 - Do data processing here
	if(sc.Index == sc.ArraySize-1)
	{
		High[sc.Index] = sc.Low[sc.Index] + sc.RangeBarValue;
		Low[sc.Index] =  sc.High[sc.Index] - sc.RangeBarValue; 
	}
	else
	{
		High[sc.Index] = 0;
		Low[sc.Index] =  0;
	}
}  

//-----------------------------------------------------------------------------
SCSFExport scsf_RenkoBarPredictor(SCStudyInterfaceRef sc)
{
	SCSubgraphRef High = sc.Subgraph[0];
	SCSubgraphRef Low  = sc.Subgraph[1];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Renko Bar Predictor";

		sc.StudyDescription = "Renko Bar Predictor.";

		sc.FreeDLL = 0;

		sc.GraphRegion = 0;

		sc.AutoLoop = 1;  // true

		High.Name = "Predicted High";
		High.DrawStyle = DRAWSTYLE_DASH;
		//High.LineStyle = LINESTYLE_DASH;
		High.PrimaryColor = RGB(0,255,0);
		High.LineWidth = 2;
		High.DrawZeros = false;

		Low.Name = "Predicted Low";
		Low.DrawStyle = DRAWSTYLE_DASH;
		//Low.LineStyle = LINESTYLE_DASH;
		Low.PrimaryColor = RGB(255,0,0);
		Low.LineWidth = 2;
		Low.DrawZeros = false;

		return;
	}

	int Index = sc.Index;

	if (Index > 0 && Index == sc.ArraySize-1)
	{
		float PriorRenkoOpen = sc.BaseData[SC_RENKO_OPEN][Index-1];
		float PriorRenkoClose = sc.BaseData[SC_RENKO_CLOSE][Index-1];
		bool  PriorBarUp = PriorRenkoOpen < PriorRenkoClose;

		if (PriorBarUp)
		{
			High[Index] = PriorRenkoClose - sc.RenkoTrendOpenOffsetInTicks * sc.TickSize + sc.RenkoTicksPerBar * sc.TickSize;
			Low[Index]  = PriorRenkoClose + sc.RenkoReversalOpenOffsetInTicks * sc.TickSize - 2.0f * sc.RenkoTicksPerBar * sc.TickSize;
		}
		else
		{
			High[Index] = PriorRenkoClose - sc.RenkoReversalOpenOffsetInTicks * sc.TickSize + 2.0f * sc.RenkoTicksPerBar * sc.TickSize;
			Low[Index]  = PriorRenkoClose + sc.RenkoTrendOpenOffsetInTicks * sc.TickSize - sc.RenkoTicksPerBar * sc.TickSize;
		}
	}
	else
	{
		High[Index] = 0;
		Low[Index]  = 0;
	}
}  

/**********************************************************************/
SCSFExport scsf_FastStochastic(SCStudyInterfaceRef sc)
{
	SCSubgraphRef PercentK = sc.Subgraph[0];
	SCSubgraphRef PercentD = sc.Subgraph[1];
	SCSubgraphRef Line1 = sc.Subgraph[2];
	SCSubgraphRef Line2 = sc.Subgraph[3];

	SCInputRef Unused2 = sc.Input[2];
	SCInputRef KLength = sc.Input[3];
	SCInputRef DLength = sc.Input[4];
	SCInputRef Line1Value = sc.Input[5];
	SCInputRef Line2Value = sc.Input[6];
	SCInputRef MovAvgType = sc.Input[7];
	SCInputRef InputDataHigh = sc.Input[8];
	SCInputRef InputDataLow = sc.Input[9];
	SCInputRef InputDataLast = sc.Input[10];
	
	if ( sc.SetDefaults )
	{
		sc.GraphName = "Stochastic - Fast";

		sc.ValueFormat = 2;

		PercentK.Name = "%K";
		PercentK.DrawStyle = DRAWSTYLE_LINE;
		PercentK.PrimaryColor = RGB(0,255,0);
		PercentK.DrawZeros = true;

		PercentD.Name = "%D";
		PercentD.DrawStyle = DRAWSTYLE_LINE;
		PercentD.PrimaryColor = RGB(255,0,255);
		PercentD.DrawZeros = true;

		Line1.Name = "Line1";
		Line1.DrawStyle = DRAWSTYLE_LINE;
		Line1.PrimaryColor = RGB(255,255,0);
		Line1.DrawZeros = true;

		Line2.Name = "Line2";
		Line2.DrawStyle = DRAWSTYLE_LINE;
		Line2.PrimaryColor = RGB(255,127,0);
		Line2.DrawZeros = true;

		Unused2.Name = "";
		Unused2.SetInt(10);
		Unused2.SetIntLimits(1,MAX_STUDY_LENGTH);

		KLength.Name = "%K Length";
		KLength.SetInt(10);
		KLength.SetIntLimits(1,MAX_STUDY_LENGTH);

		DLength.Name = "%D Length";
		DLength.SetInt(3);
		DLength.SetIntLimits(1,MAX_STUDY_LENGTH);

		Line1Value.Name = "Line1 Value";
		Line1Value.SetFloat(70);

		Line2Value.Name = "Line2 Value";
		Line2Value.SetFloat(30);

		MovAvgType.Name = "Moving Average Type";
		MovAvgType.SetMovAvgType(MOVAVGTYPE_SIMPLE);

		InputDataHigh.Name = "Input Data for High";
		InputDataHigh.SetInputDataIndex(SC_HIGH);
		
		InputDataLow.Name = "Input Data for Low";
		InputDataLow.SetInputDataIndex(SC_LOW);

		InputDataLast.Name = "Input Data for Last";
		InputDataLast.SetInputDataIndex(SC_LAST);

		sc.AutoLoop = true;
		return;
	}


	sc.DataStartIndex = KLength.GetInt() + DLength.GetInt();

	sc.Stochastic(
		sc.BaseData[InputDataHigh.GetInputDataIndex()],
		sc.BaseData[InputDataLow.GetInputDataIndex()],
		sc.BaseData[InputDataLast.GetInputDataIndex()],
		PercentK,  // Data member is Fast %K
		KLength.GetInt(),
		DLength.GetInt(),
		0,
		MovAvgType.GetMovAvgType()
	);

	
	PercentD[sc.Index] = PercentK.Arrays[0][sc.Index];  // Fast %D (Slow %K)

	Line1[sc.Index] = Line1Value.GetFloat();
	Line2[sc.Index] = Line2Value.GetFloat();
}

/**********************************************************************/
SCSFExport scsf_KDFastStochastic(SCStudyInterfaceRef sc)
{
	SCSubgraphRef PercentK = sc.Subgraph[0];
	SCSubgraphRef PercentD = sc.Subgraph[1];
	SCSubgraphRef Line1 = sc.Subgraph[2];
	SCSubgraphRef Line2 = sc.Subgraph[3];

	SCInputRef Unused2 = sc.Input[2];
	SCInputRef KLength = sc.Input[3];
	SCInputRef DLength = sc.Input[4];
	SCInputRef Line1Value = sc.Input[5];
	SCInputRef Line2Value = sc.Input[6];
	SCInputRef MovAvgType = sc.Input[7];
	SCInputRef InputDataHigh = sc.Input[8];
	SCInputRef InputDataLow = sc.Input[9];
	SCInputRef InputDataLast = sc.Input[10];

	if ( sc.SetDefaults )
	{
		sc.GraphName = "KD - Fast";

		sc.ValueFormat = 2;

		PercentK.Name = "%K";
		PercentK.DrawStyle = DRAWSTYLE_LINE;
		PercentK.PrimaryColor = RGB(0,255,0);
		PercentK.DrawZeros = true;

		PercentD.Name = "%D";
		PercentD.DrawStyle = DRAWSTYLE_LINE;
		PercentD.PrimaryColor = RGB(255,0,255);
		PercentD.DrawZeros = true;

		Line1.Name = "Line1";
		Line1.DrawStyle = DRAWSTYLE_LINE;
		Line1.PrimaryColor = RGB(255,255,0);
		Line1.DrawZeros = true;

		Line2.Name = "Line2";
		Line2.DrawStyle = DRAWSTYLE_LINE;
		Line2.PrimaryColor = RGB(255,127,0);
		Line2.DrawZeros = true;

		Unused2.Name = "";
		Unused2.SetInt(10);
		Unused2.SetIntLimits(1,MAX_STUDY_LENGTH);

		KLength.Name = "%K Length";
		KLength.SetInt(10);
		KLength.SetIntLimits(1,MAX_STUDY_LENGTH);

		DLength.Name = "%D Length";
		DLength.SetInt(3);
		DLength.SetIntLimits(1,MAX_STUDY_LENGTH);

		Line1Value.Name = "Line1 Value";
		Line1Value.SetFloat(70);

		Line2Value.Name = "Line2 Value";
		Line2Value.SetFloat(30);

		MovAvgType.Name = "Moving Average Type";
		MovAvgType.SetMovAvgType(MOVAVGTYPE_SIMPLE);

		InputDataHigh.Name = "Input Data for High";
		InputDataHigh.SetInputDataIndex(SC_HIGH);

		InputDataLow.Name = "Input Data for Low";
		InputDataLow.SetInputDataIndex(SC_LOW);

		InputDataLast.Name = "Input Data for Last";
		InputDataLast.SetInputDataIndex(SC_LAST);

		sc.AutoLoop = true;
		return;
	}

	

	sc.DataStartIndex = KLength.GetInt() + DLength.GetInt();

	sc.Stochastic(
		sc.BaseData[InputDataHigh.GetInputDataIndex()],
		sc.BaseData[InputDataLow.GetInputDataIndex()],
		sc.BaseData[InputDataLast.GetInputDataIndex()],
		PercentK,  // Data member is Fast %K
		KLength.GetInt(),
		DLength.GetInt(),
		0,
		MovAvgType.GetMovAvgType()
		);


	PercentD[sc.Index] = PercentK.Arrays[0][sc.Index];  // Fast %D (Slow %K)

	Line1[sc.Index] = Line1Value.GetFloat();
	Line2[sc.Index] = Line2Value.GetFloat();
}

/**********************************************************************/
SCSFExport scsf_SlowStochastic(SCStudyInterfaceRef sc)
{
	SCSubgraphRef SlowK = sc.Subgraph[0];
	SCSubgraphRef SlowD = sc.Subgraph[1];
	SCSubgraphRef Line1 = sc.Subgraph[2];
	SCSubgraphRef Line2 = sc.Subgraph[3];
	SCSubgraphRef Temp4 = sc.Subgraph[4];

	SCInputRef FastKLength = sc.Input[2];
	SCInputRef FastDLength = sc.Input[3];
	SCInputRef SlowDLength = sc.Input[4];
	SCInputRef Line1Value = sc.Input[5];
	SCInputRef Line2Value = sc.Input[6];
	SCInputRef MovAvgType = sc.Input[7];
	SCInputRef InputDataHigh = sc.Input[8];
	SCInputRef InputDataLow = sc.Input[9];
	SCInputRef InputDataLast = sc.Input[10];
	SCInputRef UpdateFlag = sc.Input[11];

	if ( sc.SetDefaults )
	{
		sc.GraphName = "Stochastic - Slow";

		sc.ValueFormat = 2;

		SlowK.Name = "Slow %K";
		SlowK.DrawStyle = DRAWSTYLE_LINE;
		SlowK.PrimaryColor = RGB(0,255,0);
		SlowK.DrawZeros = true;

		SlowD.Name = "Slow %D";
		SlowD.DrawStyle = DRAWSTYLE_LINE;
		SlowD.PrimaryColor = RGB(255,0,255);
		SlowD.DrawZeros = true;

		Line1.Name = "Line1";
		Line1.DrawStyle = DRAWSTYLE_LINE;
		Line1.PrimaryColor = RGB(255,255,0);
		Line1.DrawZeros = true;

		Line2.Name = "Line2";
		Line2.DrawStyle = DRAWSTYLE_LINE;
		Line2.PrimaryColor = RGB(255,127,0);
		Line2.DrawZeros = true;

		FastKLength.Name = "Fast %K Length";
		FastKLength.SetInt(10);
		FastKLength.SetIntLimits(1,MAX_STUDY_LENGTH);

		FastDLength.Name = "Fast %D Length (Slow %K)";
		FastDLength.SetInt(3);
		FastDLength.SetIntLimits(1,MAX_STUDY_LENGTH);

		SlowDLength.Name = "Slow %D Length";
		SlowDLength.SetInt(3);
		SlowDLength.SetIntLimits(1,MAX_STUDY_LENGTH);

		Line1Value.Name = "Line1 Value";
		Line1Value.SetFloat(70);

		Line2Value.Name = "Line2 Value";
		Line2Value.SetFloat(30);

		MovAvgType.Name = "Moving Average Type";
		MovAvgType.SetMovAvgType(MOVAVGTYPE_SIMPLE);

		InputDataHigh.Name = "Input Data for High";
		InputDataHigh.SetInputDataIndex(SC_HIGH);

		InputDataLow.Name = "Input Data for Low";
		InputDataLow.SetInputDataIndex(SC_LOW);

		InputDataLast.Name = "Input Data for Last";
		InputDataLast.SetInputDataIndex(SC_LAST);

		UpdateFlag.SetInt(1); //update flag

		sc.AutoLoop = true;
		return;
	}


	sc.DataStartIndex = FastKLength.GetInt() + FastDLength.GetInt() + SlowDLength.GetInt();

	sc.Stochastic(
		sc.BaseData[InputDataHigh.GetInputDataIndex()],
		sc.BaseData[InputDataLow.GetInputDataIndex()],
		sc.BaseData[InputDataLast.GetInputDataIndex()],
		Temp4,  // Data member is Fast %K
		FastKLength.GetInt(),
		FastDLength.GetInt(),
		SlowDLength.GetInt(),
		MovAvgType.GetMovAvgType()
		);

	SlowK[sc.Index] = Temp4.Arrays[0][sc.Index];  
	SlowD[sc.Index] = Temp4.Arrays[1][sc.Index];  

	Line1[sc.Index] = Line1Value.GetFloat();
	Line2[sc.Index] = Line2Value.GetFloat();

	return;


}


/**********************************************************************/
SCSFExport scsf_KDSlowStochastic(SCStudyInterfaceRef sc)
{
	SCSubgraphRef SlowK = sc.Subgraph[0];
	SCSubgraphRef SlowD = sc.Subgraph[1];
	SCSubgraphRef Line1 = sc.Subgraph[2];
	SCSubgraphRef Line2 = sc.Subgraph[3];
	SCSubgraphRef Temp4 = sc.Subgraph[4];

	SCInputRef FastKLength = sc.Input[2];
	SCInputRef FastDLength = sc.Input[3];
	SCInputRef SlowDLength = sc.Input[4];
	SCInputRef Line1Value = sc.Input[5];
	SCInputRef Line2Value = sc.Input[6];
	SCInputRef MovAvgType = sc.Input[7];
	SCInputRef InputDataHigh = sc.Input[8];
	SCInputRef InputDataLow = sc.Input[9];
	SCInputRef InputDataLast = sc.Input[10];

	if ( sc.SetDefaults )
	{
		sc.GraphName = "KD - Slow";

		sc.ValueFormat = 2;

		SlowK.Name = "Slow %K";
		SlowK.DrawStyle = DRAWSTYLE_LINE;
		SlowK.PrimaryColor = RGB(0,255,0);
		SlowK.DrawZeros = true;

		SlowD.Name = "Slow %D";
		SlowD.DrawStyle = DRAWSTYLE_LINE;
		SlowD.PrimaryColor = RGB(255,0,255);
		SlowD.DrawZeros = true;

		Line1.Name = "Line1";
		Line1.DrawStyle = DRAWSTYLE_LINE;
		Line1.PrimaryColor = RGB(255,255,0);
		Line1.DrawZeros = true;

		Line2.Name = "Line2";
		Line2.DrawStyle = DRAWSTYLE_LINE;
		Line2.PrimaryColor = RGB(255,127,0);
		Line2.DrawZeros = true;

		FastKLength.Name = "Fast %K Length";
		FastKLength.SetInt(10);
		FastKLength.SetIntLimits(1,MAX_STUDY_LENGTH);

		FastDLength.Name = "Fast %D Length (Slow %K)";
		FastDLength.SetInt(3);
		FastDLength.SetIntLimits(1,MAX_STUDY_LENGTH);

		SlowDLength.Name = "Slow %D Length";
		SlowDLength.SetInt(3);
		SlowDLength.SetIntLimits(1,MAX_STUDY_LENGTH);

		Line1Value.Name = "Line1 Value";
		Line1Value.SetFloat(70);

		Line2Value.Name = "Line2 Value";
		Line2Value.SetFloat(30);

		MovAvgType.Name = "Moving Average Type";
		MovAvgType.SetMovAvgType(MOVAVGTYPE_SIMPLE);

		InputDataHigh.Name = "Input Data for High";
		InputDataHigh.SetInputDataIndex(SC_HIGH);

		InputDataLow.Name = "Input Data for Low";
		InputDataLow.SetInputDataIndex(SC_LOW);

		InputDataLast.Name = "Input Data for Last";
		InputDataLast.SetInputDataIndex(SC_LAST);


		sc.AutoLoop = true;
		return;
	}


	sc.DataStartIndex = FastKLength.GetInt() + FastDLength.GetInt() + SlowDLength.GetInt();

	sc.Stochastic(
		sc.BaseData[InputDataHigh.GetInputDataIndex()],
		sc.BaseData[InputDataLow.GetInputDataIndex()],
		sc.BaseData[InputDataLast.GetInputDataIndex()],
		Temp4,  // Data member is Fast %K
		FastKLength.GetInt(),
		FastDLength.GetInt(),
		SlowDLength.GetInt(),
		MovAvgType.GetMovAvgType()
		);

	SlowK[sc.Index] = Temp4.Arrays[0][sc.Index];  
	SlowD[sc.Index] = Temp4.Arrays[1][sc.Index];  

	Line1[sc.Index] = Line1Value.GetFloat();
	Line2[sc.Index] = Line2Value.GetFloat();

	return;


}
/**********************************************************************/
SCSFExport scsf_DoubleTrix(SCStudyInterfaceRef sc)
{
	// Section 1 - Set the configuration variables

	SCInputRef I_TRIXLength = sc.Input[0];
	SCInputRef I_SigLineXMALen = sc.Input[1];

	SCFloatArrayRef EMA1 = sc.Subgraph[0].Arrays[0];
	SCFloatArrayRef EMA2 = sc.Subgraph[0].Arrays[1];
	SCFloatArrayRef EMA3 = sc.Subgraph[0].Arrays[2];

	SCSubgraphRef TRIXLine = sc.Subgraph[0];
	SCSubgraphRef SignalLine = sc.Subgraph[1];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Double Trix";

		sc.StudyDescription = "DoubleTrix.";

		sc.FreeDLL = 0;

		sc.AutoLoop = 1;  // true

		TRIXLine.Name = "TRIXLine";
		TRIXLine.DrawStyle = DRAWSTYLE_LINE;
		TRIXLine.PrimaryColor = RGB(0,255,0);
		TRIXLine.LineWidth = 1;

		SignalLine.Name = "SignalLine";
		SignalLine.DrawStyle = DRAWSTYLE_LINE;
		SignalLine.PrimaryColor = RGB(255,0,0);
		SignalLine.LineWidth = 1;

		I_TRIXLength.Name = "TRIXLength";
		I_TRIXLength.SetInt(5);

		I_SigLineXMALen.Name = "SigLineXMALen";
		I_SigLineXMALen.SetInt(3);

		return;
	}

	// Section 2 - Do data processing here

	int TRIXLength = I_TRIXLength.GetInt();
	int SigLineXMALen = I_SigLineXMALen.GetInt();

	sc.ExponentialMovAvg(sc.Close,EMA1,TRIXLength);
	sc.ExponentialMovAvg(EMA1,EMA2,TRIXLength);
	sc.ExponentialMovAvg(EMA2,EMA3,TRIXLength);

	if(EMA3[sc.Index-1] != 0)
		TRIXLine[sc.Index] = 10 * ( EMA3[sc.Index] - EMA3[sc.Index-1] ) / EMA3[sc.Index-1] ;
	else
		TRIXLine[sc.Index] = 0;

	sc.ExponentialMovAvg(TRIXLine,SignalLine,SigLineXMALen);
}  

/**********************************************************************/
SCSFExport scsf_BollingerSqueeze2(SCStudyInterfaceRef sc)
{
	SCSubgraphRef BollingerSqueezeSubgraph = sc.Subgraph[0];
	SCSubgraphRef SqueezeZerosSubgraph = sc.Subgraph[1];
	SCSubgraphRef KeltnerSubgraph = sc.Subgraph[2];
	SCSubgraphRef BBSubgraph = sc.Subgraph[3];
	SCSubgraphRef AverageTrueRange = sc.Subgraph[4];

	SCInputRef InputInputData = sc.Input[0];
	SCInputRef InputMAType = sc.Input[1];
	SCInputRef InputKeltnerLength = sc.Input[2];
	SCInputRef InputKeltnerTRAvgLength = sc.Input[3];
	SCInputRef InputKeltnerMultiplier = sc.Input[4];
	SCInputRef InputBollingerLength = sc.Input[5];
	SCInputRef InputBollingerBandsMult = sc.Input[6];
	SCInputRef InputMomentumLength = sc.Input[7];

	if (sc.SetDefaults)
	{
		sc.GraphName="Bollinger Squeeze 2";
		sc.StudyDescription="Bollinger Squeeze 2";

		// Subgraphs
		BollingerSqueezeSubgraph.Name = "Momentum Avg";
		BollingerSqueezeSubgraph.DrawStyle = DRAWSTYLE_BAR;
		BollingerSqueezeSubgraph.PrimaryColor = RGB(0,255,0);
		BollingerSqueezeSubgraph.SecondaryColor = RGB(255,0,0);
		BollingerSqueezeSubgraph.SecondaryColorUsed = true;

		SqueezeZerosSubgraph.Name = "Squeeze Indicator";
		SqueezeZerosSubgraph.DrawStyle = DRAWSTYLE_POINT;
		SqueezeZerosSubgraph.LineWidth = 3;
		SqueezeZerosSubgraph.PrimaryColor = RGB(0,255,0);
		SqueezeZerosSubgraph.SecondaryColor = RGB(255,0,0);
		SqueezeZerosSubgraph.SecondaryColorUsed = true;

		// Data Inputs
		InputInputData.Name = "Input Data";
		InputInputData.SetInputDataIndex(SC_LAST);

		InputMAType.Name="Moving Average Type for Internal Calculations";
		InputMAType.SetMovAvgType(MOVAVGTYPE_SIMPLE);

		InputKeltnerLength.Name="Keltner Bands Length";
		InputKeltnerLength.SetInt(20);
		InputKeltnerLength.SetIntLimits(1,MAX_STUDY_LENGTH);

		InputKeltnerTRAvgLength.Name = "Keltner True Range MovAvg Length";
		InputKeltnerTRAvgLength.SetInt(20);
		InputKeltnerTRAvgLength.SetIntLimits(1,MAX_STUDY_LENGTH);

		InputKeltnerMultiplier.Name="Keltner Bands Multiplier";
		InputKeltnerMultiplier.SetFloat(2.0f);
		InputKeltnerMultiplier.SetFloatLimits(.001f,(float)MAX_STUDY_LENGTH);

		InputBollingerLength.Name="Bollinger Bands Length";
		InputBollingerLength.SetInt(20);
		InputBollingerLength.SetIntLimits(1,MAX_STUDY_LENGTH);

		InputBollingerBandsMult.Name="Bollinger Bands Multiplier";
		InputBollingerBandsMult.SetFloat(10.0f);
		InputBollingerBandsMult.SetFloatLimits(.001f,(float)MAX_STUDY_LENGTH);

		InputMomentumLength.Name = "Momentum Length";
		InputMomentumLength.SetInt(20);
		InputMomentumLength.SetIntLimits(1, MAX_STUDY_LENGTH);

		sc.AutoLoop = 1;
		sc.GraphRegion = 1;
		sc.ValueFormat = 3;

		return;
	}


	int InputData = InputInputData.GetInputDataIndex();
	int MAType = InputMAType.GetMovAvgType();
	int KeltnerLength = InputKeltnerLength.GetInt();
	int KeltnetTRAVGLength = InputKeltnerTRAvgLength.GetInt();
	float KeltnerMult = InputKeltnerMultiplier.GetFloat();
	int BollingerLength = InputBollingerLength.GetInt();	
	float BandsMult = InputBollingerBandsMult.GetFloat();
	int MomentumLength = InputMomentumLength.GetInt();

	if ((sc.Index < KeltnerLength) || (sc.Index < BollingerLength) || (sc.Index < MomentumLength))
		return;

	// calculate Bollinger Bands
	sc.BollingerBands(sc.BaseData[InputData], BBSubgraph, BollingerLength,BandsMult,MAType);
	BBSubgraph[sc.Index] = BBSubgraph.Arrays[0][sc.Index]; // store the average into the subgraph itself


	// Calculate BollingerSqueeze: EMA(Base[LAST][Index] - Base[LAST][Index - MomentumLength]). Use Arrays[0] for first calculation.
	BollingerSqueezeSubgraph.Arrays[0][sc.Index] = 
		sc.Close[sc.Index] - sc.Close[sc.Index - MomentumLength];
	sc.ExponentialMovAvg(BollingerSqueezeSubgraph.Arrays[0], BollingerSqueezeSubgraph, MomentumLength);

	// calculate Keltner
	sc.Keltner( sc.BaseData,sc.BaseData[InputData], KeltnerSubgraph, KeltnerLength, MAType,
		KeltnetTRAVGLength, MOVAVGTYPE_WILDERS, KeltnerMult,KeltnerMult);

	// calculate Average True Range: SimpleMovingAverage(True Range), use Arrays[0] to store True Range
	AverageTrueRange.Arrays[0][sc.Index] = sc.GetTrueRange(sc.BaseData, sc.Index);
	sc.SimpleMovAvg(AverageTrueRange.Arrays[0], AverageTrueRange, KeltnerLength);

	float dS1 = KeltnerSubgraph[sc.Index] + BandsMult * AverageTrueRange[sc.Index];
	float dS2 = KeltnerSubgraph[sc.Index] - BandsMult * AverageTrueRange[sc.Index];
	float dUBB = BBSubgraph.Arrays[0][sc.Index];
	float dLBB = BBSubgraph.Arrays[1][sc.Index];

	if ((dUBB > dS1) && (dLBB < dS2)) 
	{
		SqueezeZerosSubgraph.DataColor[sc.Index] = SqueezeZerosSubgraph.PrimaryColor; 
		SqueezeZerosSubgraph[sc.Index] = 0.00001f;
	} 
	else 
	{	
		SqueezeZerosSubgraph.DataColor[sc.Index] = SqueezeZerosSubgraph.SecondaryColor;
		SqueezeZerosSubgraph[sc.Index] = -0.00001f;
	}


	if ((BollingerSqueezeSubgraph[sc.Index] > 0) && 
		(BollingerSqueezeSubgraph[sc.Index] >= BollingerSqueezeSubgraph[sc.Index-1]))
		BollingerSqueezeSubgraph.DataColor[sc.Index] = BollingerSqueezeSubgraph.PrimaryColor;
	else if ((BollingerSqueezeSubgraph[sc.Index] > 0) && 
		(BollingerSqueezeSubgraph[sc.Index] < BollingerSqueezeSubgraph[sc.Index-1]))
	{
		BollingerSqueezeSubgraph.DataColor[sc.Index] = 
			RGB(GetRValue(BollingerSqueezeSubgraph.PrimaryColor)/2, 
			GetGValue(BollingerSqueezeSubgraph.PrimaryColor)/2, 
			GetBValue(BollingerSqueezeSubgraph.PrimaryColor)/2);
	}
	else if ((BollingerSqueezeSubgraph[sc.Index] < 0) && 
		(BollingerSqueezeSubgraph[sc.Index] <= BollingerSqueezeSubgraph[sc.Index-1])) 
		BollingerSqueezeSubgraph.DataColor[sc.Index] = BollingerSqueezeSubgraph.SecondaryColor;
	else if ((BollingerSqueezeSubgraph[sc.Index] < 0) && 
		(BollingerSqueezeSubgraph[sc.Index] > BollingerSqueezeSubgraph[sc.Index-1]))
	{
		BollingerSqueezeSubgraph.DataColor[sc.Index] = 
			RGB(GetRValue(BollingerSqueezeSubgraph.SecondaryColor)/2, 
			GetGValue(BollingerSqueezeSubgraph.SecondaryColor)/2, 
			GetBValue(BollingerSqueezeSubgraph.SecondaryColor)/2);
	}
}

/************************************************************************/
SCSFExport scsf_VolumeColoredBasedOnVolume(SCStudyInterfaceRef sc)
{
	// Section 1 - Set the configuration variables

	SCSubgraphRef Volume = sc.Subgraph[0];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Volume-Colored Based on Volume";


		sc.FreeDLL = 0;

		sc.AutoLoop = 1;  // true

		Volume.Name = "Volume";
		Volume.DrawStyle = DRAWSTYLE_BAR;
		Volume.SecondaryColorUsed = 1;
		Volume.PrimaryColor = RGB(0,255,0);
		Volume.SecondaryColor = RGB(255,0,0);

		return;
	}

	// Section 2 - Do data processing here

	Volume[sc.Index] = sc.Volume[sc.Index];

	if(sc.Volume[sc.Index-1] <= sc.Volume[sc.Index])
		Volume.DataColor[sc.Index] = Volume.PrimaryColor;
	else
		Volume.DataColor[sc.Index] = Volume.SecondaryColor;

} 

/*==========================================================================*/
SCSFExport scsf_ExtendedArrayExample(SCStudyInterfaceRef sc)
{
	SCSubgraphRef ADX = sc.Subgraph[0];
	SCSubgraphRef ADXForwardShifted = sc.Subgraph[1];

	SCSubgraphRef ADXForwardShiftedPlus = sc.Subgraph[2];


	SCInputRef DXLength = sc.Input[3];
	SCInputRef DXMovAngLength = sc.Input[4];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults
		sc.FreeDLL = 0;

		sc.GraphName = "Extended Array Example";
		sc.StudyDescription = "Extended Array Example.  Based on ADX.";

		sc.AutoLoop = 1;
		sc.GraphRegion = 1;

		ADX.Name = "ADX";
		ADX.DrawStyle = DRAWSTYLE_LINE;
		ADX.PrimaryColor = RGB(0,255,0);
		ADX.DrawZeros = false;

		ADXForwardShifted.Name = "ADX Forward Shifted";
		ADXForwardShifted.DrawStyle = DRAWSTYLE_LINE;
		ADXForwardShifted.PrimaryColor = RGB(255,0,255);
		ADXForwardShifted.DrawZeros = false;

		// Set the ADXForwardShifted subgraph to support additional array elements for forward projection
		ADXForwardShifted.ExtendedArrayElementsToGraph = 50;

		ADXForwardShiftedPlus.Name = "ADX Plus Value";
		ADXForwardShiftedPlus.DrawStyle = DRAWSTYLE_LINE;
		ADXForwardShiftedPlus.ExtendedArrayElementsToGraph = 50;
		ADXForwardShiftedPlus.PrimaryColor = RGB(255,255,0);
		ADXForwardShiftedPlus.DrawZeros = false;


		DXLength.Name = "DX Length";
		DXLength.SetInt(14);
		DXLength.SetIntLimits(1, INT_MAX);


		DXMovAngLength.Name = "DX Mov Avg Length";
		DXMovAngLength.SetInt(14);
		DXMovAngLength.SetIntLimits(1, INT_MAX);
		
		
		return;
	}

	// Do data processing

	sc.DataStartIndex = DXLength.GetInt() + DXMovAngLength.GetInt() - 1;

	// Calculate ADX
	sc.ADX(
		sc.BaseData,
		ADX,
		DXLength.GetInt(),
		DXMovAngLength.GetInt());


	int NumberOfBarsToForwardShift = ADXForwardShifted.ExtendedArrayElementsToGraph;

	ADXForwardShifted[sc.Index] = ADX[sc.Index-NumberOfBarsToForwardShift];
	ADXForwardShiftedPlus[sc.Index] = ADXForwardShifted[sc.Index] + 10;

	// Extended elements are cleared when the number of bars in the chart grows. Therefore, we need to fill them in every time. However, only when we are on the final index when using sc.AutoLoop = 1.
	if(sc.Index+1 >= sc.ArraySize)	
	{
		for (int i = sc.ArraySize;i < sc.ArraySize + NumberOfBarsToForwardShift ;i++)
		{	
			ADXForwardShifted[i] = ADX[i-NumberOfBarsToForwardShift];	
			ADXForwardShiftedPlus[i] = ADXForwardShifted[i] + 10;
		}
	}

	return;
}

/*==========================================================================*/
SCSFExport scsf_ForwardProjectionBars(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Open = sc.Subgraph[0];
	SCSubgraphRef High = sc.Subgraph[1];
	SCSubgraphRef Low = sc.Subgraph[2];
	SCSubgraphRef Last = sc.Subgraph[3];


	if (sc.SetDefaults)
	{
		// Set the configuration and defaults
		sc.FreeDLL = 0;

		sc.GraphName = "Forward Projection Bars";

		sc.AutoLoop = 1;
		sc.GraphRegion = 1;
		sc.GraphDrawType=GDT_OHLCBAR;

		Open.Name = "Open";
		Open.DrawStyle = DRAWSTYLE_LINE;
		Open.PrimaryColor = RGB(0,255,0);
		Open.DrawZeros = false;
		 Open.ExtendedArrayElementsToGraph= 100;

		High.Name = "High";
		High.DrawStyle = DRAWSTYLE_LINE;
		High.PrimaryColor = RGB(0,255,0);
		High.DrawZeros = false;
		High.ExtendedArrayElementsToGraph= 100;

		Low.Name = "Low";
		Low.DrawStyle = DRAWSTYLE_LINE;
		Low.PrimaryColor = RGB(0,255,0);
		Low.DrawZeros = false;
		Low.ExtendedArrayElementsToGraph= 100;

		Last.Name = "Last";
		Last.DrawStyle = DRAWSTYLE_LINE;
		Last.PrimaryColor = RGB(0,255,0);
		Last.DrawZeros = false;
		Last.ExtendedArrayElementsToGraph= 100;


		return;
	}

	// Do data processing

	//Repeat last bar into forward projection area.
	if(sc.Index+1 >= sc.ArraySize)	
	{
		for (int Index = sc.ArraySize ;  Index < sc.ArraySize +  Open.ExtendedArrayElementsToGraph ; Index++)
		{	
			Open[Index] = sc.Open[sc.Index];
			High[Index] = sc. High[sc.Index];
			Low[Index] = sc. Low[sc.Index];
			Last[Index] = sc. Close[sc.Index];
		}
	}

	return;
}

/*==========================================================================*/

SCSFExport scsf_ArrayValueAtNthOccurrenceSample(SCStudyInterfaceRef sc)
{   
	SCSubgraphRef StochasticData = sc.Subgraph[0];
	SCSubgraphRef ValueAtOccurence = sc.Subgraph[1];
	SCSubgraphRef Line1 = sc.Subgraph[2];

	SCInputRef KLength = sc.Input[3];
	SCInputRef Line1Value = sc.Input[5];
	SCInputRef MovAvgType = sc.Input[7];
	SCInputRef InputDataHigh = sc.Input[8];
	SCInputRef InputDataLow = sc.Input[9];
	SCInputRef InputDataLast = sc.Input[10];
	SCInputRef NumberOfOccurrences = sc.Input[11];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Array Value At Nth Occurrence Sample";
		sc.StudyDescription = "Array Value At Nth Occurrence.  Based on %K array of Fast Stochastic";

		sc.ValueFormat = 2;
		sc.FreeDLL = 0;

		StochasticData.Name = "%K";
		StochasticData.DrawStyle = DRAWSTYLE_LINE;
		StochasticData.PrimaryColor = RGB(0,255,0);
		StochasticData.DrawZeros = true;

		ValueAtOccurence.Name = "Value At Nth Occurrence";
		ValueAtOccurence.DrawStyle = DRAWSTYLE_LINE;
		ValueAtOccurence.PrimaryColor = RGB(255,0,255);
		ValueAtOccurence.DrawZeros = true;

		Line1.Name = "Line1";
		Line1.DrawStyle = DRAWSTYLE_LINE;
		Line1.PrimaryColor = RGB(255,255,0);
		Line1.DrawZeros = true;

		KLength.Name = "%K Length";
		KLength.SetInt(10);
		KLength.SetIntLimits(1,MAX_STUDY_LENGTH);

		Line1Value.Name = "Line1 Value";
		Line1Value.SetFloat(70);

		MovAvgType.Name = "Moving Average Type";
		MovAvgType.SetMovAvgType(MOVAVGTYPE_SIMPLE);

		InputDataHigh.Name = "Input Data";
		InputDataHigh.SetInputDataIndex(SC_HIGH);

		InputDataLow.Name = "Input Data";
		InputDataLow.SetInputDataIndex(SC_LOW);

		InputDataLast.Name = "Input Data";
		InputDataLast.SetInputDataIndex(SC_LAST);
		
		NumberOfOccurrences.Name = "Number Of Occurrences";
		NumberOfOccurrences.SetInt(10);
		NumberOfOccurrences.SetIntLimits(1,MAX_STUDY_LENGTH);


		sc.AutoLoop = true;
		return;
	}

	// This sample is based on Fast Stochastic Study.
	// The sample study use only %K subgraph
	// If %K value greater than Line1 value, then TRUE will be put to TrueFalse array, otherwise FALSE
	// The ValueAtOccurence Subgraph will have the values returned by sc.ArrayValueAtNthOccurrence function

	int InDLength = 3;
	
	sc.DataStartIndex = KLength.GetInt() + InDLength;

	sc.Stochastic(sc.BaseDataIn, sc.Subgraph[4], KLength.GetInt(), 1, 1, 1);

	sc.Stochastic(
		sc.BaseData[InputDataHigh.GetInputDataIndex()],
		sc.BaseData[InputDataLow.GetInputDataIndex()],
		sc.BaseData[InputDataLast.GetInputDataIndex()],
		StochasticData,  
		KLength.GetInt(),
		InDLength,
		0,
		MovAvgType.GetMovAvgType()
		);

	// fill the TrueFalse array
	if (StochasticData[sc.Index] > Line1Value.GetFloat())
		StochasticData.Arrays[1][sc.Index] = 1.0f;
	else
		StochasticData.Arrays[1][sc.Index] = 0.0f;

	ValueAtOccurence[sc.Index] = sc.ArrayValueAtNthOccurrence(StochasticData.Arrays[1], StochasticData, NumberOfOccurrences.GetInt());

	Line1[sc.Index] = Line1Value.GetFloat();
}

/************************************************************************/
SCSFExport scsf_SwingHighAndLow(SCStudyInterfaceRef sc)
{
	SCSubgraphRef SwingHigh = sc.Subgraph[0];
	SCSubgraphRef SwingLow = sc.Subgraph[1];

	SCInputRef ArrowOffsetValue = sc.Input[0];
	SCInputRef Length = sc.Input[1];
	SCInputRef AllowEqualBars = sc.Input[2];

	if ( sc.SetDefaults )
	{
		sc.GraphName = "Swing High And Low";


		sc.AutoLoop = true;
		sc.FreeDLL = 0;
		sc.GraphRegion = 0;
		sc.ValueFormat= VALUEFORMAT_INHERITED;

		SwingHigh.Name = "Swing High";
		SwingHigh.DrawStyle = DRAWSTYLE_ARROWDOWN;
		SwingHigh.PrimaryColor = RGB(0,255,0);
		SwingHigh.DrawZeros = false;
		SwingHigh.PrimaryColor = RGB(255,0,0);
		SwingHigh.LineWidth = 3;


		SwingLow.Name = "Swing Low";
		SwingLow.DrawStyle = DRAWSTYLE_ARROWUP;
		SwingLow.PrimaryColor = RGB(255,0,255);
		SwingLow.DrawZeros = false;
		SwingLow.LineWidth = 3;
		SwingLow.PrimaryColor = RGB(0,255,0);

		ArrowOffsetValue.Name = "Arrow Offset as Percentage";
		ArrowOffsetValue.SetFloat(3);
		
		Length.Name = "Length";
		Length.SetInt(1);
		Length.SetIntLimits(1,MAX_STUDY_LENGTH);
		
		AllowEqualBars.Name = "Allow Equal High/Low Bars";
		AllowEqualBars.SetYesNo(false);

		return;
	}

	int IndexToEvaluate = sc.Index - Length.GetInt();

	if(IndexToEvaluate -Length.GetInt() < 0)
	{
		return;
	}

	float ArrowOffset=(sc.High[IndexToEvaluate] -sc.Low[IndexToEvaluate] )*(ArrowOffsetValue.GetFloat() * 0.01f);

	SwingHigh[IndexToEvaluate] = 0;
	SwingLow[IndexToEvaluate] = 0;

	// check for Swing High
	if ( !AllowEqualBars.GetYesNo() )
	{
		if (sc.IsSwingHigh(sc.High, IndexToEvaluate, Length.GetInt() ) )
			SwingHigh[IndexToEvaluate] = sc.High[IndexToEvaluate] + ArrowOffset;
	}
	else	if ( IsSwingHighAllowEqual_S( sc, true , IndexToEvaluate,  Length.GetInt()))
		SwingHigh[IndexToEvaluate] = sc.High[IndexToEvaluate] + ArrowOffset;
	

	// check for Swing Low
	if ( !AllowEqualBars.GetYesNo() )
	{
		if (sc.IsSwingLow(sc.Low, IndexToEvaluate, Length.GetInt() ) )
			SwingLow[IndexToEvaluate] = sc.Low[IndexToEvaluate] - ArrowOffset;

	}
	else	if ( IsSwingLowAllowEqual_S( sc, true , IndexToEvaluate,  Length.GetInt())  )
		SwingLow[IndexToEvaluate] = sc.Low[IndexToEvaluate] - ArrowOffset;


}

/******************************************************************************************/
SCSFExport scsf_ForceIndexAverage(SCStudyInterfaceRef sc)
{
	// Section 1 - Set the configuration variables

	SCSubgraphRef ForceAverage = sc.Subgraph[0];

	SCFloatArrayRef ForceIndex = sc.Subgraph[0].Arrays[0];

	SCInputRef AverageLength = sc.Input[0];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Force Index Average";

		sc.StudyDescription = "";

		sc.FreeDLL = 0;

		sc.AutoLoop = 1; 


		sc.ValueFormat = 2;
		sc.GraphRegion = 1;

		ForceAverage.Name = "Force Average";
		ForceAverage.DrawStyle = DRAWSTYLE_LINE;
		ForceAverage.PrimaryColor = RGB(0,255,0);
		ForceAverage.DrawZeros = true;

		AverageLength.Name = "Moving Average Length";
		AverageLength.SetInt(20);
		AverageLength.SetIntLimits(1, MAX_STUDY_LENGTH);

		return;
	}

	// Section 2 - Do data processing here

	sc.DataStartIndex = AverageLength.GetInt(); 

	int i = sc.Index;

	ForceIndex[i] = sc.Volume[i] * (sc.Close[i] - sc.Close[i-1]);

	sc.ExponentialMovAvg(ForceIndex,ForceAverage,AverageLength.GetInt());

}

/******************************************************************************************/
SCSFExport scsf_ForceIndex(SCStudyInterfaceRef sc)
{
	// Section 1 - Set the configuration variables

	SCSubgraphRef ForceIndex = sc.Subgraph[0];


	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Force Index";

		sc.StudyDescription = "";

		sc.FreeDLL = 0;

		sc.AutoLoop = 1; 

		
		sc.ValueFormat = 2;
		sc.GraphRegion = 1;

		ForceIndex.Name = "Force Average";
		ForceIndex.DrawStyle = DRAWSTYLE_LINE;
		ForceIndex.PrimaryColor = RGB(0,255,0);
		ForceIndex.DrawZeros = true;

		return;
	}

	// Section 2 - Do data processing here


	int i = sc.Index;

	ForceIndex[i] = sc.Volume[i] * (sc.Close[i] - sc.Close[i-1]);
}
/******************************************************************************************/

/********************************************************************
*
*		True Strength Index (Also known as Ergodic)
*
*			Array 3-8 are used for calculations
*
*			Numerator = EMA( EMA(Price - LastPrice, LongExpMALength), ShortExpMALength)
*			Denominator = EMA( EMA( Abs(Price - LastPrice), LongExpMALength), ShortExpMaLength)
*
*			TSI = Multiplier * Numerator / Denominator
*			SignalMA = EMA(TSI, SignalMALength)
*			Oscillator = TSI - SignalMA
*
*********************************************************************/
SCSFExport scsf_TrueStrengthIndex(SCStudyInterfaceRef sc)
{
	SCSubgraphRef TSI = sc.Subgraph[0];
	SCSubgraphRef SignalMA = sc.Subgraph[1];
	SCSubgraphRef Oscillator = sc.Subgraph[2];
	SCSubgraphRef Temp3 = sc.Subgraph[3];
	SCSubgraphRef Temp4 = sc.Subgraph[4];
	SCSubgraphRef Temp5 = sc.Subgraph[5];
	SCSubgraphRef Temp6 = sc.Subgraph[6];
	SCSubgraphRef Temp7 = sc.Subgraph[7];
	SCSubgraphRef Temp8 = sc.Subgraph[8];

	SCInputRef InputData = sc.Input[0];
	SCInputRef LongExpMALength = sc.Input[3];
	SCInputRef ShortExpMALength = sc.Input[4];
	SCInputRef Multiplier = sc.Input[5];
	SCInputRef SignalLineMALength = sc.Input[6];
	
	if (sc.SetDefaults)
	{
		sc.GraphName = "True Strength Index";
		
		sc.GraphRegion	= 1;
		sc.ValueFormat	= 2;
		sc.AutoLoop		= 1; 

		TSI.Name = "TSI";
		TSI.DrawStyle = DRAWSTYLE_LINE;
		TSI.PrimaryColor = RGB(0,255,0);
		TSI.DrawZeros = false;

		SignalMA.Name = "Signal MA";
		SignalMA.DrawStyle = DRAWSTYLE_HIDDEN;
		SignalMA.PrimaryColor = RGB(255,0,255);
		SignalMA.DrawZeros = false;

		Oscillator.Name = "Oscillator";
		Oscillator.DrawStyle = DRAWSTYLE_HIDDEN;
		Oscillator.PrimaryColor = RGB(255,255,0);
		Oscillator.DrawZeros = false;

		
		
		
		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		LongExpMALength.Name = "Long Exp MovAvg Length";
		LongExpMALength.SetInt(15);
		LongExpMALength.SetIntLimits(1, MAX_STUDY_LENGTH);

		ShortExpMALength.Name = "Short Exp MovAvg Length";
		ShortExpMALength.SetInt(5);
		ShortExpMALength.SetIntLimits(1, MAX_STUDY_LENGTH);

		Multiplier.Name = "Multiplier";
		Multiplier.SetInt(1);

		SignalLineMALength.Name = "Signal Line MovAvg Length";
		SignalLineMALength.SetInt(10);
		SignalLineMALength.SetIntLimits(1, MAX_STUDY_LENGTH);

		return;
	}

	// Subgraphs 3-7 are used for calculation only
	sc.DataStartIndex = ((LongExpMALength.GetInt() ) + (ShortExpMALength.GetInt()) + SignalLineMALength.GetInt()) ;

	if (sc.Index < 1) // not large enough
		return;

	int LongStart = sc.Index;
	if (LongStart < LongExpMALength.GetInt() - 1)  //2
		LongStart = LongExpMALength.GetInt() - 1;  //2

	int ShortStart = sc.Index;
	if (ShortStart < LongExpMALength.GetInt() - 1 + ShortExpMALength.GetInt() - 1)  //3
		ShortStart = LongExpMALength.GetInt() - 1 + ShortExpMALength.GetInt() - 1;  //3

	int SignalStart = sc.Index;
	if (SignalStart < ShortExpMALength.GetInt() - 1 + SignalLineMALength.GetInt() - 1+LongExpMALength.GetInt() -1)  //4
		SignalStart = ShortExpMALength.GetInt() - 1 + SignalLineMALength.GetInt() - 1 +LongExpMALength.GetInt() -1;  //4


	int DataStart = sc.Index;
	if (DataStart < 1)
		DataStart = 1;

	if(sc.Index >= DataStart)
		Temp3[sc.Index] = sc.BaseDataIn[InputData.GetInputDataIndex()][sc.Index] - sc.BaseDataIn[InputData.GetInputDataIndex()][sc.Index-1];

	// Calculate the Numerator into SubGraphData[0]
	if(sc.Index >= LongStart)
		sc.MovingAverage(Temp3, Temp4, MOVAVGTYPE_EXPONENTIAL, LongExpMALength.GetInt());

	if(sc.Index >= ShortStart)
		sc.MovingAverage(Temp4, Temp5, MOVAVGTYPE_EXPONENTIAL, ShortExpMALength.GetInt());
	
	// Calculate the Denominator into SubGraphData[1]
	if(sc.Index >= DataStart)
		Temp6[sc.Index] = fabs(Temp3[sc.Index]);

	if(sc.Index >= LongStart)
		sc.MovingAverage(Temp6, Temp7, MOVAVGTYPE_EXPONENTIAL, LongExpMALength.GetInt());

	if(sc.Index >= ShortStart)
	{
		sc.MovingAverage(Temp7, Temp8, MOVAVGTYPE_EXPONENTIAL, ShortExpMALength.GetInt());

		// Store the TSI (Numerator / Denominator)
		if (Temp8[sc.Index] != 0)
			TSI[sc.Index] = Multiplier.GetInt() * Temp5[sc.Index]/Temp8[sc.Index];
		else
			TSI[sc.Index] = 0;
	}

	// Calculate the Signal Line ( EMA[TSI] )
	if(sc.Index >= SignalStart)
		sc.MovingAverage(TSI, SignalMA, MOVAVGTYPE_EXPONENTIAL, SignalLineMALength.GetInt());

	// Calculate the Oscillator (TSI - EMA[TSI])
	if(sc.Index >= sc.DataStartIndex)
		Oscillator[sc.Index] = TSI[sc.Index] - SignalMA[sc.Index];
}

/***************************************************************/
SCSFExport scsf_CommodityChannelIndex(SCStudyInterfaceRef sc)
{
	SCInputRef I_InputData = sc.Input[0];
	SCInputRef I_Length = sc.Input[3];
	SCInputRef I_Multiplier = sc.Input[4];
	SCInputRef I_Line2Value = sc.Input[5];
	SCInputRef I_Line3Value = sc.Input[6];
	SCInputRef I_MAType = sc.Input[7];
	SCInputRef I_Version = sc.Input[8];

	SCSubgraphRef CCI = sc.Subgraph[0];
	SCSubgraphRef Line1 = sc.Subgraph[1];
	SCSubgraphRef Line2 = sc.Subgraph[2];
	SCSubgraphRef Line3 = sc.Subgraph[3];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Commodity Channel Index";
		

		sc.FreeDLL = 0;
		sc.AutoLoop = 1;  // true
		sc.GraphRegion = 1;
		sc.ValueFormat = 2;

		CCI.Name = "CCI";
		CCI.DrawStyle = DRAWSTYLE_BAR;
		CCI.PrimaryColor = RGB(0,255,0);
		CCI.DrawZeros= true;

		Line1.Name = "Line 1";
		Line1.DrawStyle = DRAWSTYLE_HIDDEN;
		Line1.PrimaryColor = RGB(255,0,255);
		Line1.DrawZeros= true;

		Line2.Name = "Line 2";
		Line2.DrawStyle = DRAWSTYLE_LINE;
		Line2.PrimaryColor = RGB(255,255,0);
		Line2.DrawZeros = false;

		Line3.Name = "Line 3";
		Line3.DrawStyle = DRAWSTYLE_LINE;
		Line3.PrimaryColor = RGB(255,127,0);
		Line3.DrawZeros = false;


		I_InputData.Name = "Input Data";
		I_InputData.SetInputDataIndex(SC_HLC);

		I_Length.Name = "Length";
		I_Length.SetInt(10);
		I_Length.SetIntLimits(1,MAX_STUDY_LENGTH);

		I_Multiplier.Name = "Multiplier";
		I_Multiplier.SetFloat(0.015f);

		I_Line2Value.Name = "Line2 Value";
		I_Line2Value.SetFloat(100);

		I_Line3Value.Name = "Line3 Value";
		I_Line3Value.SetFloat(-100);

		I_MAType.Name = "Moving Average Type";
		I_MAType.SetMovAvgType(MOVAVGTYPE_SIMPLE);

		 I_Version.SetInt(1);

		return;
	}


	if(I_Version.GetInt() < 1)
	{
		I_Version.SetInt(1);
		I_MAType.SetMovAvgType(MOVAVGTYPE_SIMPLE);
	}

	sc.DataStartIndex = I_Length.GetInt();

	float Multiplier = I_Multiplier.GetFloat();

	if(Multiplier == 0.0f)
		Multiplier = 0.015f;

	sc.CCI(sc.BaseDataIn[I_InputData.GetInputDataIndex()], CCI, sc.Index, I_Length.GetInt(), Multiplier, I_MAType.GetMovAvgType());

	Line2[sc.Index] = I_Line2Value.GetFloat();
	Line3[sc.Index] = I_Line3Value.GetFloat();
} 
 

/***************************************************************/
SCSFExport scsf_TradeVolumeIndex(SCStudyInterfaceRef sc)
{
	SCSubgraphRef TVI = sc.Subgraph[0];

	SCInputRef InputData = sc.Input[0];
	

	if (sc.SetDefaults)
	{
		sc.GraphName = "Trade Volume Index";

		sc.ValueFormat = 0;
		sc.GraphRegion = 1;
		sc.AutoLoop = 1;
		
		TVI.Name = "TVI";
		TVI.DrawStyle = DRAWSTYLE_LINE;
		TVI.PrimaryColor = RGB(0,255,0);
		TVI.DrawZeros = true;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);


		return;
	}

	sc.DataStartIndex = 1;

	if(sc.Index < 1)
		return;

	int Index = sc.Index;

	float Diff = sc.BaseDataIn[InputData.GetInputDataIndex()][Index] - sc.BaseDataIn[InputData.GetInputDataIndex()][Index-1];


	if(sc.FormattedEvaluate((float) Diff,sc.BaseGraphValueFormat,GREATER_OPERATOR,sc.TickSize ,sc.BaseGraphValueFormat) != 0)
		TVI[Index] = TVI[Index-1] + sc.Volume[Index];
	else 	if(sc.FormattedEvaluate((float) Diff,sc.BaseGraphValueFormat,LESS_OPERATOR,sc.TickSize*-1 ,sc.BaseGraphValueFormat) != 0)
		TVI[Index] = TVI[Index-1] - sc.Volume[Index];
	else 
		TVI[Index] = TVI[Index-1];
}

/**************************************************************
*
*		Demand Index
*
*		Array Number Usage
*
*			Array 0 - Demand Index
*			Array 1 - Demand Index Moving Average
*			Array 2 - Zero Line
*			Array 3 - Moving Average Sell Power
*			Array 4 - High + Low + 2 * Last
*			Array 5 - Max of last 2 High's - Min of last 2 Low's
*			Array 6 - EMA of Array 5
*			Array 7 - EMA of Volume
*			Array 8 - BuyPower
*			Array 9 - SellPower
*			Array 10 - Moving Average Buy Power
*			
***************************************************************/
SCSFExport scsf_DemandIndex(SCStudyInterfaceRef sc)
{
	SCSubgraphRef OutDemandIndex = sc.Subgraph[0];  // Demand Index
	SCSubgraphRef OutDemandIndexMA = sc.Subgraph[1];  // Demand Index Moving Average
	SCSubgraphRef OutZeroLine = sc.Subgraph[2];  // Zero Line

	SCFloatArrayRef ArrHL2C = sc.Subgraph[0].Arrays[0];  // High + Low + 2 * Last
	SCFloatArrayRef ArrRange = sc.Subgraph[0].Arrays[1];  // Max of last 2 High's - Min of last 2 Low's
	SCFloatArrayRef ArrRangeMA = sc.Subgraph[1].Arrays[1];  // EMA of Array 5
	SCFloatArrayRef ArrVolumeMA = sc.Subgraph[1].Arrays[0];  // EMA of Volume
	SCFloatArrayRef ArrBuyPower = sc.Subgraph[0].Arrays[2];  // BuyPower
	SCFloatArrayRef ArrSellPower = sc.Subgraph[0].Arrays[3];  // SellPower
	SCFloatArrayRef ArrMABuyPower = sc.Subgraph[1].Arrays[2];  // Moving Average Buy Power
	SCFloatArrayRef ArrMASellPower = sc.Subgraph[1].Arrays[3];  // Moving Average Sell Power

	SCInputRef BSPowerLength = sc.Input[0];
	SCInputRef BSPowerMovAvgLength = sc.Input[1];
	SCInputRef DIMovAvgLength = sc.Input[2];
	SCInputRef DIMovAvgType = sc.Input[3];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Demand Index";
		
		sc.AutoLoop = 1;
		
		OutDemandIndex.Name	= "DI";
		OutDemandIndex.PrimaryColor	= RGB(0,255,0);
		OutDemandIndex.LineStyle = DRAWSTYLE_LINE;
		OutDemandIndex.LineWidth = 1;
		OutDemandIndex.DrawZeros = true;
		
		OutDemandIndexMA.Name = "DI MovAvg";
		OutDemandIndexMA.PrimaryColor = RGB(255,125,125);
		OutDemandIndexMA.LineStyle = DRAWSTYLE_LINE;
		OutDemandIndexMA.LineWidth = 1;
		OutDemandIndexMA.DrawZeros = true;
		
		OutZeroLine.Name = "Line";
		OutZeroLine.PrimaryColor = RGB(125,125,125);
		OutZeroLine.LineStyle = DRAWSTYLE_LINE;
		OutZeroLine.LineWidth = 1;
		OutZeroLine.DrawZeros = true;
		
		BSPowerLength.Name = "Buy/Sell Power Length";
		BSPowerLength.SetInt(19);
		BSPowerLength.SetIntLimits(1, 100);
		
		BSPowerMovAvgLength.Name = "Buy/Sell Power Moving Average Length";
		BSPowerMovAvgLength.SetInt(19);
		BSPowerMovAvgLength.SetIntLimits(1, 100);
		
		DIMovAvgLength.Name = "Demand Index Moving Average Length";
		DIMovAvgLength.SetInt(30);
		DIMovAvgLength.SetIntLimits(1, 100);
		
		DIMovAvgType.Name = "Demand Index Moving Average Type";
		DIMovAvgType.SetMovAvgType(MOVAVGTYPE_EXPONENTIAL);
		
		return;
	}
	
	ArrHL2C[sc.Index] = sc.High[sc.Index] + sc.Low[sc.Index] + 2 * sc.Close[sc.Index];
	
	if (sc.Index == 0)
	{
		ArrRange[0] = sc.High[0] - sc.Low[0];
	}
	else
	{
		ArrRange[sc.Index]
			= max(sc.High[sc.Index], sc.High[sc.Index - 1])
			- min(sc.Low[sc.Index], sc.Low[sc.Index - 1]);
	}
	
	//
	sc.MovingAverage(ArrRange, ArrRangeMA, DIMovAvgType.GetMovAvgType(), BSPowerLength.GetInt());
	sc.MovingAverage(sc.Volume, ArrVolumeMA, DIMovAvgType.GetMovAvgType(), BSPowerLength.GetInt());
	
	//
	if (sc.Index >= 1)
	{
		ArrBuyPower[sc.Index] = sc.Volume[sc.Index] / ArrVolumeMA[sc.Index];
		ArrSellPower[sc.Index] = sc.Volume[sc.Index] / ArrVolumeMA[sc.Index];
		
		if (ArrHL2C[sc.Index] < ArrHL2C[sc.Index - 1])
		{
			ArrBuyPower[sc.Index]
				= ArrBuyPower[sc.Index]
				/ exp(
					(
						0.375f
						* (ArrHL2C[sc.Index] + ArrHL2C[sc.Index - 1])
						/ ArrRangeMA[0]
					)
					* (ArrHL2C[sc.Index - 1] - ArrHL2C[sc.Index])
					/ ArrHL2C[sc.Index]
				);
		}
		else if (ArrHL2C[sc.Index] > ArrHL2C[sc.Index - 1])
		{
			ArrSellPower[sc.Index]
				= ArrSellPower[sc.Index]
				/ exp(
					(
						0.375f
						* (ArrHL2C[sc.Index] + ArrHL2C[sc.Index - 1])
						/ ArrRangeMA[0]
					)
					* (ArrHL2C[sc.Index] - ArrHL2C[sc.Index - 1])
					/ ArrHL2C[sc.Index - 1]
				);
		}
	}
	
	//
	sc.MovingAverage(ArrBuyPower, ArrMABuyPower, DIMovAvgType.GetMovAvgType(), BSPowerMovAvgLength.GetInt());
	sc.MovingAverage(ArrSellPower, ArrMASellPower, DIMovAvgType.GetMovAvgType(), BSPowerMovAvgLength.GetInt());
	
	//
	float divsor, divend;
	if (ArrMABuyPower[sc.Index] > ArrMASellPower[sc.Index])
	{
		divsor = ArrMABuyPower[sc.Index];
		divend = ArrMASellPower[sc.Index];
	}
	else if (ArrMABuyPower[sc.Index] < ArrMASellPower[sc.Index])
	{
		divsor = ArrMASellPower[sc.Index];
		divend = ArrMABuyPower[sc.Index];
	}
	else
	{
		divsor = ArrMASellPower[sc.Index];
		divend = ArrMASellPower[sc.Index];
	}
	
	float var2 = 1 - (divend/divsor);
	float var3 = var2;
	if (ArrMASellPower[sc.Index] > ArrMABuyPower[sc.Index])
		var3 *= -1;
	
	OutDemandIndex[sc.Index] = var3 * 100;
	OutZeroLine[sc.Index] = 0; 
	
	//
	sc.MovingAverage(OutDemandIndex, OutDemandIndexMA, MOVAVGTYPE_SIMPLE, DIMovAvgLength.GetInt());
}

/***************************************************************/
SCSFExport scsf_BollingerBandBandwidth(SCStudyInterfaceRef sc)
{
	SCSubgraphRef BBBandWidth = sc.Subgraph[0];
	SCSubgraphRef OutBollingerBands = sc.Subgraph[1];

	SCInputRef InputData = sc.Input[0];


	SCInputRef MovingAverageType = sc.Input[1];

	SCInputRef Length = sc.Input[3];
	SCInputRef StandardDeviations = sc.Input[4];
	SCInputRef Version = sc.Input[5];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Bollinger Bands: Bandwidth";
		sc.ValueFormat = 3;
		sc.GraphRegion = 1;
		sc.AutoLoop = 1;
		
		BBBandWidth.Name = "BollingerBands: Bandwidth";
		BBBandWidth.DrawStyle = DRAWSTYLE_LINE;
		BBBandWidth.PrimaryColor = RGB(0,255,0);
		BBBandWidth.DrawZeros = true;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_HLC);

		MovingAverageType.Name = "Moving Average Type";
		MovingAverageType.SetMovAvgType(MOVAVGTYPE_SIMPLE);

		Length.Name = "Length";
		Length.SetInt(5);
		Length.SetIntLimits(1,MAX_STUDY_LENGTH);
		
		StandardDeviations.Name = "Standard Deviations";
		StandardDeviations.SetFloat(2.0f);
		StandardDeviations.SetFloatLimits(0.00001f,(float)MAX_STUDY_LENGTH);

		Version.SetInt(1);

		return;
	}

	if (Version.GetInt() < 1)
	{
		MovingAverageType.SetMovAvgType(MOVAVGTYPE_SIMPLE);
		Version.SetInt(1);
	}
	
	int i = sc.Index;

	sc.DataStartIndex=Length.GetInt()-1;
	
	sc.BollingerBands(sc.BaseDataIn[InputData.GetInputDataIndex()], OutBollingerBands, Length.GetInt(), StandardDeviations.GetFloat(),MovingAverageType.GetMovAvgType());

	SCFloatArrayRef MiddleBand	= OutBollingerBands;  
	SCFloatArrayRef UpperBand	= OutBollingerBands.Arrays[0];  
	SCFloatArrayRef LowerBand	= OutBollingerBands.Arrays[1];  

	BBBandWidth[i] = (UpperBand[i] - LowerBand[i]) / MiddleBand[i];
}


/***************************************************************/
SCSFExport scsf_BollingerBandsPercentB(SCStudyInterfaceRef sc)
{
	SCSubgraphRef PercentB = sc.Subgraph[0];
	SCSubgraphRef OutBollingerBands	= sc.Subgraph[1];
	SCSubgraphRef UpperThreshold = sc.Subgraph[2];
	SCSubgraphRef LowerThreshold = sc.Subgraph[3];

	SCInputRef InputData = sc.Input[0];
	SCInputRef MovAvgType = sc.Input[1];
	SCInputRef Length = sc.Input[3];
	SCInputRef StandardDeviations = sc.Input[4];
	SCInputRef BaseBand = sc.Input[5];
	SCInputRef UpperThresholdValue = sc.Input[6];
	SCInputRef LowerThresholdValue = sc.Input[7];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Bollinger Bands %B";
		sc.ValueFormat = 3;
		sc.GraphRegion = 1;
		sc.AutoLoop = 1;
		
		PercentB.Name	= "%B";
		PercentB.DrawStyle = DRAWSTYLE_LINE;
		PercentB.PrimaryColor = RGB(0,255,0);
		PercentB.DrawZeros = true;

		UpperThreshold.Name	= "Upper Threshold";
		UpperThreshold.DrawStyle = DRAWSTYLE_LINE;
		UpperThreshold.LineStyle = LINESTYLE_DASH;
		UpperThreshold.PrimaryColor = COLOR_GRAY;
		UpperThreshold.DrawZeros = false;

		LowerThreshold.Name	= "Lower Threshold";
		LowerThreshold.DrawStyle = DRAWSTYLE_LINE;
		LowerThreshold.LineStyle = LINESTYLE_DASH;
		LowerThreshold.PrimaryColor = COLOR_GRAY;
		LowerThreshold.DrawZeros = false;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_HLC);

		MovAvgType.Name = "Moving Average Type";
		MovAvgType.SetMovAvgType(MOVAVGTYPE_SIMPLE);

		Length.Name = "Length";
		Length.SetInt(10);
		Length.SetIntLimits(1, MAX_STUDY_LENGTH);
		
		StandardDeviations.Name = "Standard Deviations";
		StandardDeviations.SetFloat(2.0f);

		BaseBand.Name = "Base Band";
		BaseBand.SetCustomInputStrings( "Lower Band;Middle Band");
		BaseBand.SetCustomInputIndex(0);

		UpperThresholdValue.Name = "Upper Line Threshold Value";
		UpperThresholdValue.SetFloat(0);

		LowerThresholdValue.Name = "Lower Line Threshold Value";
		LowerThresholdValue.SetFloat(0);

		return;
	}

	int Index = sc.Index;

	sc.DataStartIndex= Length.GetInt() -1;

	// Calculate Bollinger bands
	sc.BollingerBands(sc.BaseDataIn[InputData.GetInputDataIndex()], 
		OutBollingerBands, Length.GetInt(), StandardDeviations.GetFloat(), MovAvgType.GetMovAvgType());

	SCFloatArrayRef MiddleBand	= OutBollingerBands;  
	SCFloatArrayRef UpperBand	= OutBollingerBands.Arrays[0];  
	SCFloatArrayRef LowerBand	= OutBollingerBands.Arrays[1];  

	//Standard calculation: %B = (Price - Lower Band)/(Upper Band - Lower Band)

	if (BaseBand.GetIndex()== 1)
	{
		 float BandsDifference= UpperBand[Index] - MiddleBand[Index];
		 if(BandsDifference)
			PercentB[Index] = (sc.BaseDataIn[InputData.GetInputDataIndex()][Index] - MiddleBand[Index]) / BandsDifference;
		 else
			 PercentB[Index] = 0.0;
	}
	else
	{
		

		float BandsDifference= UpperBand[Index] - LowerBand[Index];
		if(BandsDifference)
			PercentB[Index] = (sc.BaseDataIn[InputData.GetInputDataIndex()][Index] - LowerBand[Index]) / BandsDifference;
		else
			PercentB[Index] = 0.0;
	}

	UpperThreshold[Index] = UpperThresholdValue.GetFloat();
	LowerThreshold[Index] = LowerThresholdValue.GetFloat();
}

/***************************************************************/
SCSFExport scsf_DifferenceBar(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Open = sc.Subgraph[0];
	SCSubgraphRef High = sc.Subgraph[1];
	SCSubgraphRef Low = sc.Subgraph[2];
	SCSubgraphRef Last = sc.Subgraph[3];
	SCSubgraphRef Volume = sc.Subgraph[4];
	SCSubgraphRef OpenInterest = sc.Subgraph[5];
	SCSubgraphRef OHLCAvg = sc.Subgraph[6];
	SCSubgraphRef HLCAvg = sc.Subgraph[7];
	SCSubgraphRef HLAvg = sc.Subgraph[8];
	SCSubgraphRef BidVol = sc.Subgraph[9];
	SCSubgraphRef AskVol = sc.Subgraph[10];

	SCInputRef Chart2Number = sc.Input[3];
	SCInputRef Chart1Multiplier = sc.Input[4];
	SCInputRef Chart2Multiplier = sc.Input[5];
	SCInputRef Chart1Addition = sc.Input[6];

	if (sc.SetDefaults)
	{
		sc.GraphName			= "Difference (Bar)";
		sc.ValueFormat			= 2;
		sc.GraphRegion			= 1;
		sc.UseGlobalChartColors = 0;
		sc.GraphDrawType		= GDT_OHLCBAR;
		sc.StandardChartHeader	= 1;
		
		// We are using Manual looping.
		sc.AutoLoop = false;

		Open.Name = "Open";
		Open.DrawStyle = DRAWSTYLE_LINE;
		Open.PrimaryColor = RGB(0,255,0);
		Open.DrawZeros = false;

		High.Name = "High";
		High.DrawStyle = DRAWSTYLE_LINE;
		High.PrimaryColor = RGB(0,255,0);
		High.DrawZeros = false;

		Low.Name = "Low";
		Low.DrawStyle = DRAWSTYLE_LINE;
		Low.PrimaryColor = RGB(0,255,0);
		Low.DrawZeros = false;

		Last.Name = "Last";
		Last.DrawStyle = DRAWSTYLE_LINE;
		Last.PrimaryColor = RGB(0,255,0);
		Last.DrawZeros = false;

		Volume.Name = "Volume";
		Volume.DrawStyle = DRAWSTYLE_IGNORE;
		Volume.PrimaryColor = RGB(255,0,0);
		Volume.DrawZeros = false;

		OpenInterest.Name = "# of Trades / OI";
		OpenInterest.DrawStyle = DRAWSTYLE_IGNORE;
		OpenInterest.PrimaryColor = RGB(0,0,255);
		OpenInterest.DrawZeros = false;

		OHLCAvg.Name = "OHLC Avg";
		OHLCAvg.DrawStyle = DRAWSTYLE_IGNORE;
		OHLCAvg.PrimaryColor = RGB(0,255,0);
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


		Chart2Number.Name = "Chart 2 Number";
		Chart2Number.SetChartNumber(1);

		Chart1Multiplier.Name = "Chart 1 Multiplier";
		Chart1Multiplier.SetFloat(1.0f);
		
		Chart2Multiplier.Name = "Chart 2 Multiplier";
		Chart2Multiplier.SetFloat(1.0f);

		Chart1Addition.Name = "Chart 1 Addition";
		Chart1Addition.SetFloat(0);

		return;
	}

	// Obtain a reference to the Base Data in the specified chart. This call is relatively efficient, but it should be called as minimally as possible. To reduce the number of calls we have it outside of the primary "for" loop in this study function. And we also use Manual Looping by using sc.AutoLoop = 0. In this way, sc.GetChartBaseData is called only once per call to this study function and there is a minimal number of calls to this study function. sc.GetChartBaseData is a function to get all of the Base Data arrays with one efficient call.
	SCGraphData Chart2BaseData;
	sc.GetChartBaseData(-Chart2Number.GetChartNumber(), Chart2BaseData);

	for (int Index = sc.UpdateStartIndex; Index < sc.ArraySize; Index++)
	{
		int Chart2Index = sc.GetNearestMatchForDateTimeIndex(Chart2Number.GetChartNumber(), Index);

		for (int SubgraphIndex = 0; SubgraphIndex < 6; SubgraphIndex++)
		{
			sc.Subgraph[SubgraphIndex][Index]
			= (sc.BaseDataIn[SubgraphIndex][Index] * Chart1Multiplier.GetFloat() + Chart1Addition.GetFloat())
				- (Chart2BaseData[SubgraphIndex][Chart2Index] * Chart2Multiplier.GetFloat());
		}

		sc.Subgraph[ SC_HIGH][Index] = max(sc.Subgraph[SC_OPEN][Index],
			max(sc.Subgraph[SC_HIGH][Index],
			max(sc.Subgraph[SC_LOW][Index],sc.Subgraph[SC_LAST][Index])
			)
			);

		sc.Subgraph[SC_LOW][Index] = min(sc.Subgraph[SC_OPEN][Index],
			min(sc.Subgraph[SC_HIGH][Index],
			min(sc.Subgraph[SC_LOW][Index],sc.Subgraph[SC_LAST][Index])
			)
			);

		sc.CalculateOHLCAverages(Index);
	}
	
	SCString Chart1Name = sc.GetStudyNameFromChart(sc.ChartNumber, 0);
	SCString Chart2Name = sc.GetStudyNameFromChart(Chart2Number.GetChartNumber(), 0);
	sc.GraphName.Format("Difference %s - %s", Chart1Name.GetChars(), Chart2Name.GetChars());
}

/***************************************************************/
SCSFExport scsf_HighLowForTimePeriodExtendedLines(SCStudyInterfaceRef sc)
{
	SCSubgraphRef HighOfDay = sc.Subgraph[0];
	SCSubgraphRef LowOfDay = sc.Subgraph[1];

	SCInputRef StartTime = sc.Input[0];
	SCInputRef EndTime = sc.Input[1];
	SCInputRef  NumberDaysToCalculate = sc.Input [2];
	SCInputRef Version = sc.Input[4];

	if (sc.SetDefaults)
	{
		sc.GraphName			= "High/Low for Time Period - Extended";
		sc.GraphRegion			= 0;
		sc.AutoLoop				= 1;
		
		HighOfDay.Name = "High";
		HighOfDay.DrawStyle = DRAWSTYLE_STAIR;
		HighOfDay.PrimaryColor = RGB(0,255,0);
		HighOfDay. DrawZeros = false;
		
		LowOfDay.Name = "Low";
		LowOfDay.DrawStyle = DRAWSTYLE_STAIR;
		LowOfDay.PrimaryColor = RGB(255,0,255);
		LowOfDay. DrawZeros = false;

		StartTime.Name = "Start Time";
		StartTime.SetTime(0);

		EndTime.Name = "End Time";
		EndTime.SetTime(SECONDS_PER_DAY - 1);

	
		NumberDaysToCalculate.Name= "Number Of Days To Calculate";
		NumberDaysToCalculate.SetInt(10000);

		Version.SetInt(1);

		return;
	}

	if (NumberDaysToCalculate.GetInt() <= 0)
		NumberDaysToCalculate.SetInt(10000);

	SCDateTime InStartTime = StartTime.GetDateTime();
	SCDateTime InEndTime = EndTime.GetDateTime();
	
	
	bool ShouldDecreaseEndTime = false;
	if (InStartTime.GetTime() <= InEndTime.GetTime())
	{
		ShouldDecreaseEndTime = (InStartTime.GetTime() > sc.StartTimeOfDay && InEndTime.GetTime() < sc.StartTimeOfDay) 
			||
			(InStartTime.GetTime() < sc.StartTimeOfDay && InEndTime.GetTime() > sc.StartTimeOfDay);
	}
	else
	{
		ShouldDecreaseEndTime= (InStartTime.GetTime() < sc.StartTimeOfDay && InEndTime.GetTime() < sc.StartTimeOfDay)
			||
			(InStartTime.GetTime() > sc.StartTimeOfDay && InEndTime.GetTime() > sc.StartTimeOfDay);
	}

	if (ShouldDecreaseEndTime)
	{
		InEndTime.SetTime(sc.StartTimeOfDay);
		if (InEndTime != 0.0)
			InEndTime -=SECONDS;
		else
			InEndTime.SetTime(SECONDS_PER_DAY - 1);
	}

	float Open = 0.0f, High = 0.0f, Low = 0.0f, Close = 0.0f, NextOpen = 0.0f;

	int	Index = 0;

	if(sc.Index == 0)
		return;

	SCDateTime CurrentDateTime = sc.BaseDateTimeIn[sc.Index];
	SCDateTime StartDateTimeOfCurIndex = CurrentDateTime.GetDate();
	StartDateTimeOfCurIndex.SetTime(sc.StartTimeOfDay);
	if(sc.StartTimeOfDay > CurrentDateTime.GetTime())
		StartDateTimeOfCurIndex -= DAYS;

	if (sc.Index < sc.ArraySize - 2)
	{
		SCDateTime PrevDateTime = sc.BaseDateTimeIn[sc.Index - 1];
		SCDateTime StartDateTimeOfPrevIndex = PrevDateTime.GetDate();
		StartDateTimeOfPrevIndex.SetTime(sc.StartTimeOfDay);
		if(sc.StartTimeOfDay > PrevDateTime.GetTime())
			StartDateTimeOfPrevIndex -= DAYS;

		if (StartDateTimeOfPrevIndex == StartDateTimeOfCurIndex)
			return;

		Index = sc.Index - 1;
		StartDateTimeOfCurIndex = StartDateTimeOfPrevIndex;
		CurrentDateTime = PrevDateTime;
	}
	else
	{
		Index = sc.Index;
	}


	SCDateTime DaysToCalculateStartDateTime=sc.GetTradingDayStartDateTimeOfBar (sc.BaseDateTimeIn[sc.ArraySize-1]) - ( NumberDaysToCalculate.GetInt() - 1 )   ;
	if (sc.BaseDateTimeIn[Index]  <  DaysToCalculateStartDateTime)
		return;

	SCDateTime StartDateTime = 0;
	SCDateTime EndDateTime = 0;


	StartDateTime = EndDateTime = StartDateTimeOfCurIndex.GetDate();
	StartDateTime.SetTime(InStartTime.GetTime());
	EndDateTime.SetTime(InEndTime.GetTime());

	if (InStartTime.GetTime() > InEndTime.GetTime())
		EndDateTime += DAYS;

	if(InStartTime.GetTime() < StartDateTimeOfCurIndex.GetTime())
	{
		StartDateTime += DAYS;
		EndDateTime += DAYS;
	}

	sc.GetOHLCOfTimePeriod(StartDateTime, EndDateTime, Open, High, Low, Close, NextOpen);

	// Set and reset all values for current day
	while(true)
	{
		if(Index < 0)
			break;

		SCDateTime IndexDateTime = sc.BaseDateTimeIn[Index];

		if(IndexDateTime < StartDateTimeOfCurIndex)
			break;

		bool DisplayData = false;

		if (InStartTime.GetTime() < InEndTime.GetTime())
		{
			DisplayData = (IndexDateTime.GetTime() <= InEndTime.GetTime() 
				&& IndexDateTime.GetTime() >= InStartTime.GetTime());
		}
		else
		{
			DisplayData = (IndexDateTime.GetTime() <= InEndTime.GetTime() 
				|| IndexDateTime.GetTime() >= InStartTime.GetTime());
		}

		if (IndexDateTime > EndDateTime)
			DisplayData = true;

		if(DisplayData)
		{
			HighOfDay[Index] = High;
			LowOfDay[Index] = Low;
		}
		else
		{
			HighOfDay[Index] = 0.0f;
			LowOfDay[Index] = 0.0f;
		}

		Index--;
	}
}


/***************************************************************/
SCSFExport scsf_HighLowForTimePeriod(SCStudyInterfaceRef sc)
{
	SCSubgraphRef HighOfDay = sc.Subgraph[0];
	SCSubgraphRef LowOfDay = sc.Subgraph[1];

	SCInputRef StartTime = sc.Input[0];
	SCInputRef EndTime = sc.Input[1];
	SCInputRef Version = sc.Input[2];

	if (sc.SetDefaults)
	{
		sc.GraphName			= "High/Low for Time Period";
		sc.GraphRegion			= 0;
		sc.AutoLoop				= 1;
		
		HighOfDay.Name = "High";
		HighOfDay.DrawStyle = DRAWSTYLE_STAIR;
		HighOfDay.PrimaryColor = RGB(0,255,0);
		HighOfDay. DrawZeros = false;
		
		LowOfDay.Name = "Low";
		LowOfDay.DrawStyle = DRAWSTYLE_STAIR;
		LowOfDay.PrimaryColor = RGB(255,0,255);
		LowOfDay. DrawZeros = false;

		StartTime.Name = "Start Time";
		StartTime.SetTime(0);

		EndTime.Name = "End Time";
		EndTime.SetTime(SECONDS_PER_DAY - 1);
		Version.SetInt(1);

		return;
	}


	SCDateTime InStartTime = StartTime.GetDateTime();
	SCDateTime InEndTime = EndTime.GetDateTime();


	SCDateTime StartDateTime = 0;
	SCDateTime EndDateTime = 0;
	
	
	SCDateTime BarDate =sc.BaseDateTimeIn[sc.Index].GetDate();
	if(InStartTime < InEndTime)
	{
		StartDateTime =  sc.BaseDateTimeIn[sc.Index].GetDate() + InStartTime;
		EndDateTime = sc.BaseDateTimeIn[sc.Index].GetDate() + InEndTime;
	}
	//Times are reversed and the current time is greater than or equal to the start time
	else if (sc.BaseDateTimeIn[sc.Index].GetTime() >= InStartTime.GetTime() )
	{
		
		StartDateTime =BarDate+ InStartTime;
		EndDateTime = BarDate+ 1+ InEndTime;
	}
	//Times are reversed and the current time is greater than or equal to the start time
	else if (sc.BaseDateTimeIn[sc.Index].GetTime() <= InEndTime.GetTime() )
	{

		StartDateTime =BarDate+ InStartTime-1;
		EndDateTime = BarDate+ InEndTime;
	}

	//Outside of range
	if(sc.BaseDateTimeIn[sc.Index] > EndDateTime || sc.BaseDateTimeIn[sc.Index] < StartDateTime)
		return;


	float Open = 0.0f, High = 0.0f, Low = 0.0f, Close = 0.0f, NextOpen = 0.0f;
	sc.GetOHLCOfTimePeriod(StartDateTime, EndDateTime, Open, High, Low, Close, NextOpen);


	int	Index = sc.Index;

	while(true)
	{
		if(Index < 0)
			break;

		SCDateTime IndexDateTime = sc.BaseDateTimeIn[Index];

		if(IndexDateTime < StartDateTime)
			break;

		HighOfDay[Index] = High;
		LowOfDay[Index] = Low;


		Index--;
	}
}


/***********************************************************************/
SCSFExport scsf_WriteBarDataToFile(SCStudyInterfaceRef sc)
{
	SCInputRef Separator = sc.Input[0];
	SCInputRef UseGMTTime = sc.Input[1];
	SCInputRef DateFormat = sc.Input[2];
	SCInputRef OutputMilliseconds = sc.Input[3];

	if(sc.SetDefaults)
	{
		sc.GraphName="Write Bar Data To File";
		sc.StudyDescription="Write Bar Data To File";

		sc.GraphRegion = 0;

		Separator.Name = "Separator";
		Separator.SetCustomInputStrings("Space;Comma;Tab");
		Separator.SetCustomInputIndex(1);

		UseGMTTime.Name = "Use GMT Time";
		UseGMTTime.SetYesNo(0);

		DateFormat.Name = "Date Format";
		DateFormat.SetCustomInputStrings("MM/DD/YYYY;YYYY/MM/DD;DD/MM/YYYY");
		DateFormat.SetCustomInputIndex(1);

		OutputMilliseconds.Name="Output Milliseconds";
		OutputMilliseconds.  SetYesNo(false);

		sc.TextInputName = "File Path";

		sc.AutoLoop= 0;//manual looping for efficiency
		return;
	}
	
	SCString OutputPathAndFileName;
	if(sc.TextInput.GetLength() > 0)
	{
		OutputPathAndFileName = sc.TextInput;
	}
	else
	{
		OutputPathAndFileName.Format("%s-BarData.txt", sc.Symbol.GetChars());
	}

	

	HANDLE hFile = INVALID_HANDLE_VALUE;
	DWORD numOfWrittenByte;
	if(sc.UpdateStartIndex == 0)
	{
		// create new file
		hFile = CreateFile(OutputPathAndFileName.GetChars(),GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL, NULL);
		// write file header

		// creating/opening fail
		if(hFile == INVALID_HANDLE_VALUE)
		{
			SCString message;
			message.Format("Write Bar Data To File study: Error creating output data file: %s", OutputPathAndFileName.GetChars());
			sc.AddMessageToLog(message.GetChars(), 1);
			return;
		}

		SCString fileHeader;			
		switch(Separator.GetInt())
		{
		case 0:
			fileHeader = "Date Time Open High Low Close Volume NumberOfTrades BidVolume AskVolume\r\n";
			break;
		case 1:
			fileHeader = "Date, Time, Open, High, Low, Close, Volume, NumberOfTrades, BidVolume, AskVolume\r\n";
			break;
		case 2:
			fileHeader = "Date\tTime\tOpen\tHigh\tLow\tClose\tVolume\tNumberOfTrades\tBidVolume\tAskVolume\r\n";
			break;
		}

		WriteFile(hFile,fileHeader.GetChars(),(DWORD)fileHeader.GetLength(),&numOfWrittenByte,NULL);
	}
	else
	{
		// open existing file
		hFile = CreateFile(OutputPathAndFileName.GetChars(),GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		// creating/opening fail
		if(hFile == INVALID_HANDLE_VALUE)
		{
			SCString message;
			message.Format("Write Bar Data To File study: Error creating output data file: %s", OutputPathAndFileName.GetChars());
			sc.AddMessageToLog(message.GetChars(), 1);
			return;
		}

		SetFilePointer(hFile, 0, NULL, FILE_END);
	}

	bool IsRenko = sc.AreRenkoBars() != 0;

	for (int Index=sc.UpdateStartIndex;Index<(sc.ArraySize-1);Index++)
	{
		if (sc.GetBarHasClosedStatus(Index)!=BHCS_BAR_HAS_CLOSED)
			continue;			// write on close

		int year,month,day;
		int hour, minute, second, millisecond;
		SCDateTimeMS BaseDateTimeIn = sc.BaseDateTimeIn[Index];

		if (UseGMTTime.GetBoolean() != 0)
			BaseDateTimeIn = sc.AdjustDateTimeToGMT(BaseDateTimeIn);

		BaseDateTimeIn.GetDateTimeYMDHMS_MS( year, month, day, hour, minute, second, millisecond);

		SCString DateString;
		switch (DateFormat.GetInt())
		{
			case 0:
				DateString.Format("%02d/%02d/%d", month, day, year);
			break;
			
			case 1:
				DateString.Format("%d/%02d/%02d", year, month, day);
			break;
			
			case 2:
				DateString.Format("%02d/%02d/%d", day, month, year);
			break;
		}

		// write bar data string to file
		int ValueFormat = sc.BaseGraphValueFormat;
			
		char MillisecondsText[16] ={0};
		if(OutputMilliseconds.GetYesNo())
			sprintf_s(MillisecondsText,".%d",millisecond);

		char *formatString ;
		switch(Separator.GetInt()) 
		{
		case 0:
			formatString = "%s %.2d:%.2d:%.2d%s %s %s %s %s %.0f %.0f %.0f %.0f\r\n";
			break;
		case 1:
			formatString = "%s, %.2d:%.2d:%.2d%s, %s, %s, %s, %s, %.0f, %.0f, %.0f, %.0f\r\n";
			break;

		default:
		case 2:
			formatString = "%s\t%.2d:%.2d:%.2d%s\t%s\t%s\t%s\t%s\t%.0f\t%.0f\t%.0f\t%.0f\r\n";
			break;
		}
		
		float OpenValue  = IsRenko ? sc.BaseData[SC_RENKO_OPEN][Index] : sc.Open[Index];
		float HighValue  = sc.High[Index];
		float LowValue   = sc.Low[Index];
		float CloseValue = IsRenko ? sc.BaseData[SC_RENKO_CLOSE][Index] : sc.Close[Index];

		SCString BarDataString;

		BarDataString.Format(formatString,
			DateString.GetChars(),
			hour,
			minute,
			second,
			MillisecondsText,
			sc.FormatGraphValue(OpenValue, ValueFormat).GetChars(),
			sc.FormatGraphValue(HighValue, ValueFormat).GetChars(),
			sc.FormatGraphValue(LowValue, ValueFormat).GetChars(), 
			sc.FormatGraphValue(CloseValue, ValueFormat).GetChars(),
			sc.Volume[Index],
			sc.NumberOfTrades[Index],
			sc.BidVolume[Index],

			sc.AskVolume[Index]);

		WriteFile(hFile,BarDataString.GetChars(),(DWORD)BarDataString.GetLength(),&numOfWrittenByte,NULL);

	}		
	CloseHandle(hFile);

	return;
}

/***********************************************************************/
bool IsFractalBuySignal(SCFloatArrayRef InDataLow, SCFloatArrayRef InDataHigh, int index,int ArraySize); //Long Fractal
bool IsFractalSellSignal(SCFloatArrayRef InDataLow, int index,int ArraySize); //Short Fractal

SCSFExport scsf_FractalSignals(SCStudyInterfaceRef sc)
{
	enum SubgraphIndexes
	{
		INDEX_SIGNAL_BUY = 0,
		INDEX_SIGNAL_SELL		
	};

	SCSubgraphRef Buy = sc.Subgraph[0];
	SCSubgraphRef Sell = sc.Subgraph[1];

	SCInputRef HiddenUpdate = sc.Input[6];

	if(sc.SetDefaults)
	{
		sc.GraphName="Bill Williams Fractals";
		sc.StudyDescription="Bill Williams Fractal Signals";

		sc.AutoLoop = 0;
		sc.GraphRegion = 0;

		Buy.Name = "Buy";
		Buy.DrawStyle = DRAWSTYLE_TRIANGLEUP;
		Buy.PrimaryColor = RGB(0, 255, 0);
		Buy.LineWidth = 5;
		Buy.DrawZeros = false;

		Sell.Name = "Sell";
		Sell.DrawStyle = DRAWSTYLE_TRIANGLEDOWN;
		Sell.PrimaryColor = RGB(255, 0, 0);
		Sell.LineWidth = 5;
		Sell.DrawZeros = false;

		HiddenUpdate.SetYesNo(true);

		sc.AutoLoop = 1;

		return;
	}

	if(HiddenUpdate.GetYesNo() == 0)
	{
		Buy.DrawStyle = DRAWSTYLE_TRIANGLEUP;
		Buy.PrimaryColor = RGB(0, 255, 0);
		Buy.LineWidth = 5;

		Sell.DrawStyle = DRAWSTYLE_TRIANGLEDOWN;
		Sell.PrimaryColor = RGB(255, 0, 0);
		Sell.LineWidth = 5;

		HiddenUpdate.SetYesNo(true);
	}
	
	if (sc.Index < 5)
		return;

	for(int Index = sc.Index - 5; Index < sc.Index - 1; Index++)
	{
		// detect buy fractal signal
		bool is_detected = IsFractalBuySignal(sc.Low, sc.High,Index,sc.ArraySize);
		if(is_detected && (Buy[Index] == 0))
		{
			Buy[Index] = sc.High[Index];

		}
		
		// detect sell fractal signal
		is_detected = IsFractalSellSignal(sc.Low,Index,sc.ArraySize);
		if(is_detected && (Sell[Index] == 0))
		{
			Sell[Index] = sc.Low[Index];
		}
	}
} 

/***********************************************************************/
bool IsFractalBuySignal(SCFloatArrayRef InDataLow, SCFloatArrayRef InDataHigh, int index, int ArraySize)
{
	if (	
		(  ((index+2) <= (ArraySize-1)) &&
		InDataHigh[index] > InDataHigh[index + 1] && InDataHigh[index + 1] > InDataHigh[index + 2] &&
		InDataHigh[index] > InDataHigh[index - 1] && InDataHigh[index - 1] > InDataHigh[index - 2] &&
		InDataLow[index] >= InDataLow[index + 1] && InDataLow[index] >= InDataLow[index - 1] &&
		InDataLow[index - 1] > InDataLow[index - 2]
	) 
		||
		( ((index+3) <= (ArraySize-1)) &&
		InDataHigh[index] > InDataHigh[index + 2] && InDataHigh[index] > InDataHigh[index + 3] &&
		InDataHigh[index] > InDataHigh[index - 1] && InDataHigh[index] > InDataHigh[index - 2] &&
		InDataHigh[index] == InDataHigh[index + 1]
	) 
		||
		( ((index+4) <= (ArraySize-1)) &&
		InDataHigh[index] > InDataHigh[index + 3] && InDataHigh[index] > InDataHigh[index + 4] &&
		InDataHigh[index] > InDataHigh[index - 1] && InDataHigh[index] > InDataHigh[index - 2] &&
		InDataHigh[index] == InDataHigh[index + 1] && InDataHigh[index + 1] == InDataHigh[index + 2]
	)
		)
		return true;

	return false;
} 

/***********************************************************************/
bool IsFractalSellSignal(SCFloatArrayRef InDataLow, int index, int ArraySize)
{
	if (
		( ((index+2) <= (ArraySize-1)) &&
		InDataLow[index] < InDataLow[index + 1] && InDataLow[index + 1] < InDataLow[index + 2] &&
		InDataLow[index] < InDataLow[index - 1] && InDataLow[index] < InDataLow[index - 2] 
	) 
		||
		( ((index+3) <= (ArraySize-1)) &&
		InDataLow[index] < InDataLow[index + 2] && InDataLow[index] < InDataLow[index + 3] &&
		InDataLow[index] < InDataLow[index - 1] && InDataLow[index] < InDataLow[index - 2] &&
		InDataLow[index] == InDataLow[index + 1]
	) 
		||
		( ((index+4) <= (ArraySize-1)) &&
		InDataLow[index] < InDataLow[index + 3] && InDataLow[index] < InDataLow[index + 4] &&
		InDataLow[index] < InDataLow[index - 1] && InDataLow[index] < InDataLow[index - 2] &&
		InDataLow[index] == InDataLow[index + 1] && InDataLow[index + 1] == InDataLow[index + 2]
	)
		)
		return true;

	return false;
}

/***********************************************************************/
SCSFExport scsf_RoundTick(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Value = sc.Subgraph[0];

	SCInputRef InputData = sc.Input[0];

	if(sc.SetDefaults)
	{
		sc.GraphName="Round Study Subgraph to Tick Size";
		sc.StudyDescription="Rounds the elements in the array from the Base Data selected by the Input Data option, to the nearest value based upon the Tick Size setting in the chart";

		Value.Name = "Value";
		Value.DrawStyle = DRAWSTYLE_LINE;
		Value.PrimaryColor = RGB(0, 0, 255);
		Value.LineWidth = 2;
		Value.DrawZeros = false;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_OPEN);

		sc.AutoLoop = 1;
		return;
	}

	Value[sc.Index] = (float)sc.RoundToTickSize(sc.BaseDataIn[ InputData.GetInputDataIndex()][sc.Index], sc.TickSize);
}

/***********************************************************************/
SCSFExport scsf_MACDVolumeWeighted(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Macd		= sc.Subgraph[0];
	SCSubgraphRef MaMacd	= sc.Subgraph[1];
	SCSubgraphRef MacdDiff	= sc.Subgraph[2];
	SCSubgraphRef RefLine	= sc.Subgraph[3];

	SCInputRef FastLen		= sc.Input[0];
	SCInputRef SlowLen		= sc.Input[1];
	SCInputRef MacdLen		= sc.Input[2];
	SCInputRef InputData	= sc.Input[3];

	if(sc.SetDefaults)
	{
		sc.GraphName="MACD-Volume Weighted";

		sc.AutoLoop					= 1;
		sc.GraphRegion				= 1;
		sc.ValueFormat				= 3;
		

		Macd.Name = "MACD";
		Macd.DrawStyle = DRAWSTYLE_LINE;
		Macd.PrimaryColor = RGB(0,255,0);
		Macd.DrawZeros = true;

		MaMacd.Name	= "MA of MACD";
		MaMacd.DrawStyle = DRAWSTYLE_LINE;
		MaMacd.PrimaryColor = RGB(255,0,255);
		MaMacd.DrawZeros = true;

		MacdDiff.Name = "MACD Diff";
		MacdDiff.DrawStyle = DRAWSTYLE_BAR;
		MacdDiff.PrimaryColor = RGB(255,255,0);
		MacdDiff.DrawZeros = true;

		RefLine.Name = "Line";
		RefLine.DrawStyle = DRAWSTYLE_LINE;
		RefLine.PrimaryColor = RGB(255,127,0);
		RefLine.DrawZeros = true;

		FastLen.Name ="Fast Moving Average Length";
		FastLen.SetInt(12);  
		FastLen.SetIntLimits(1,MAX_STUDY_LENGTH);

		SlowLen.Name = "Slow Moving Average Length";
		SlowLen.SetInt(26);  
		SlowLen.SetIntLimits(1,MAX_STUDY_LENGTH);

		MacdLen.Name = "MACD Moving Average Length";
		MacdLen.SetInt(9);   
		MacdLen.SetIntLimits(1,MAX_STUDY_LENGTH);

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST); 

		return;
	}

	sc.DataStartIndex = MacdLen.GetInt() + max(FastLen.GetInt(), SlowLen.GetInt());
	int i = sc.Index;

	SCFloatArrayRef FastOut = Macd.Arrays[0];
	sc.VolumeWeightedMovingAverage(sc.BaseDataIn[InputData.GetInputDataIndex()], sc.Volume, FastOut, FastLen.GetInt());
	SCFloatArrayRef SlowOut = Macd.Arrays[1];
	sc.VolumeWeightedMovingAverage(sc.BaseDataIn[InputData.GetInputDataIndex()], sc.Volume, SlowOut, SlowLen.GetInt());

	Macd[i] = FastOut[i] - SlowOut[i];

	if(i < max(SlowLen.GetInt(), FastLen.GetInt()) + MacdLen.GetInt())
		return;

	sc.MovingAverage(Macd, MaMacd, MOVAVGTYPE_EXPONENTIAL, MacdLen.GetInt());
	
	MacdDiff[i] = Macd[i] - MaMacd[i];
	RefLine[i]	= 0;		
}

/***********************************************************************/
SCSFExport scsf_StandardDeviation(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Deviation	= sc.Subgraph[0];
	SCInputRef InputData	= sc.Input[0];
	SCInputRef Length		= sc.Input[1];

	if(sc.SetDefaults)
	{
		sc.GraphName = "Standard Deviation";

		sc.AutoLoop		= 1;
		

		Deviation.Name = "Deviation";
		Deviation.DrawStyle = DRAWSTYLE_LINE;
		Deviation.PrimaryColor = RGB(0,255,0);
		Deviation.DrawZeros	= true;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST); 

		Length.Name ="Length";
		Length.SetInt(14);  
		Length.SetIntLimits(1,MAX_STUDY_LENGTH);

		return;
	}

	sc.DataStartIndex = Length.GetInt();

	sc.StdDeviation(sc.BaseDataIn[InputData.GetInputDataIndex()], Deviation, Length.GetInt());
}

/***********************************************************************/
SCSFExport scsf_AutoRetracement(SCStudyInterfaceRef sc)
{
	SCSubgraphRef PeriodHigh	= sc.Subgraph[0];
	SCSubgraphRef PeriodLow		= sc.Subgraph[1];
	const int SubgraphLevelsStartIndex = 2;
	const int NumberLevels = 14;

	SCInputRef TimePeriodType   = sc.Input[0];	
	SCInputRef TimePeriodLength = sc.Input[1];	
	SCInputRef PlotType			= sc.Input[2];
	SCInputRef Version			= sc.Input[17];
	SCInputRef NewPeriodDaySessionStart = sc.Input[18] ;
	SCInputRef CalculationReferenceValue =sc.Input[19] ;

	if(sc.SetDefaults)
	{
		sc.GraphName = "Auto Retracement/Projection";
		sc.GraphRegion = 0;
		sc.AutoLoop= 0;//Manual looping
		PeriodHigh.Name = "Period High";
		PeriodHigh.DrawStyle = DRAWSTYLE_LINE;
		PeriodHigh.PrimaryColor = RGB(255, 165, 0);
		PeriodHigh.LineWidth = 2;
		PeriodHigh.DrawZeros = false;

		PeriodLow.Name = "Period Low";
		PeriodLow.DrawStyle = DRAWSTYLE_LINE;
		PeriodLow.PrimaryColor = RGB(255, 165, 0);
		PeriodLow.LineWidth = 2;
		PeriodLow.DrawZeros = false;

		for (int i = 0; i < NumberLevels; i++)
		{
			SCString TmpStr;
			TmpStr.Format("Level%d", i + 1);
			sc.Subgraph[i + SubgraphLevelsStartIndex].Name = TmpStr;
			sc.Subgraph[i + SubgraphLevelsStartIndex].DrawStyle = DRAWSTYLE_DASH;
			sc.Subgraph[i + SubgraphLevelsStartIndex].LineStyle = LINESTYLE_SOLID;
			sc.Subgraph[i + SubgraphLevelsStartIndex].LineWidth = 2;
			sc.Subgraph[i + SubgraphLevelsStartIndex].PrimaryColor =  RGB(0, 255, 255);
			sc.Subgraph[i + SubgraphLevelsStartIndex].DrawZeros = false;

			TmpStr.Format("Level %d Percentage .01 = 1%%", i + 1);
			sc.Input[3 + i].Name = TmpStr;
		}

		PlotType.Name = "Calculation Method";
		PlotType.SetCustomInputStrings("Projection;Retracement");
		PlotType.SetCustomInputIndex( 1);

		sc.Input[3].SetFloat(0.236f);
		sc.Input[4].SetFloat(0.382f);
		sc.Input[5].SetFloat(0.50f);
		sc.Input[6].SetFloat(0.618f);

		for (int i = 7; i < NumberLevels + 3; i++)
			sc.Input[i].SetFloat(0.0f);

		TimePeriodType.Name = "Time Period Unit";
		TimePeriodType.SetTimePeriodType(TIME_PERIOD_LENGTH_UNIT_DAYS);

		TimePeriodLength.Name = "Time Period Unit Length";
		TimePeriodLength.SetInt(1);

		Version.SetInt(1);

		NewPeriodDaySessionStart .Name ="New Period At Day Session Start When Using Evening Session";
		NewPeriodDaySessionStart.SetYesNo(false);

		CalculationReferenceValue.Name = "Calculation Reference Value";
		CalculationReferenceValue.SetCustomInputStrings("Automatic;Use High;Use Low");
		CalculationReferenceValue.SetCustomInputIndex(1);

		return;
	}

	if (Version.GetInt() < 1)
	{
		TimePeriodType.SetTimePeriodType(TIME_PERIOD_LENGTH_UNIT_DAYS);
		TimePeriodLength.SetInt(1);
		Version.SetInt(1);
	}


	float& OpenOfDay = sc.PersistVars->f1;

	int TimePeriodTypeInput = TimePeriodType.GetTimePeriodType();
	int TimePeriodLengthInput = TimePeriodLength.GetInt();

	float LevelsPercentages[NumberLevels];
	for (int Index = 0; Index < NumberLevels; Index++)
	{
		LevelsPercentages[Index] = sc.Input[Index + 3].GetFloat();
	}


	// Main loop
	for (int Index = sc.UpdateStartIndex; Index < sc.ArraySize; Index++)
	{

		SCDateTime PriorCurrentPeriodStartDateTime = sc.GetStartOfPeriodForDateTime(sc.BaseDateTimeIn[Index - 1], TimePeriodTypeInput, TimePeriodLengthInput, 0, NewPeriodDaySessionStart.GetYesNo());

		SCDateTime CurrentPeriodStartDateTime = sc.GetStartOfPeriodForDateTime(sc.BaseDateTimeIn[Index], TimePeriodTypeInput, TimePeriodLengthInput, 0, NewPeriodDaySessionStart.GetYesNo());

		bool IsInNewPeriod = PriorCurrentPeriodStartDateTime != CurrentPeriodStartDateTime ;

		if (IsInNewPeriod) //reset values
		{
	
			OpenOfDay = sc.Open[Index];
			PeriodHigh[Index] = sc.High[Index];
			PeriodLow[Index] = sc.Low[Index];
		}
		else
		{
			if (sc.High[Index] > PeriodHigh[Index - 1] || Index == 0)
				PeriodHigh[Index] = sc.High[Index];
			else
				PeriodHigh[Index] = PeriodHigh[Index - 1];
			
			if (sc.Low[Index] < PeriodLow[Index - 1] || Index == 0)
				PeriodLow[Index] = sc.Low[Index];
			else
				PeriodLow[Index] = PeriodLow[Index - 1];
		}

		float Range = PeriodHigh[Index] - PeriodLow[Index];

		for (int LevelIndex = 0; LevelIndex < NumberLevels; LevelIndex++)
		{
			if ( LevelsPercentages[LevelIndex] == 0)
			continue;

					
			if (PlotType.GetInt() == 1)//Retracement
			{
				//rising prices
				if ( (CalculationReferenceValue.GetIndex()== 0 && sc.Close[Index] >= OpenOfDay)   || CalculationReferenceValue.GetIndex()== 1 )
					sc.Subgraph[SubgraphLevelsStartIndex + LevelIndex][Index] = PeriodHigh[Index] - LevelsPercentages[LevelIndex] * Range;
				else
					sc.Subgraph[SubgraphLevelsStartIndex + LevelIndex][Index] = LevelsPercentages[LevelIndex] * Range + PeriodLow[Index];

			}
			else//Projection
			{
				if ( (CalculationReferenceValue.GetIndex()== 0 && sc.Close[Index] >= OpenOfDay)   || CalculationReferenceValue.GetIndex()== 1 )
					sc.Subgraph[SubgraphLevelsStartIndex + LevelIndex][Index] = LevelsPercentages[LevelIndex] * Range + PeriodHigh[Index];
				else
					sc.Subgraph[SubgraphLevelsStartIndex + LevelIndex][Index] = PeriodLow[Index] - LevelsPercentages[LevelIndex] * Range;
			}
		}
	}
}


/************************************************************************/
SCSFExport scsf_NewHighLowAlert(SCStudyInterfaceRef sc)
{

	SCInputRef HighAlert = sc.Input[0];
	SCInputRef LowAlert = sc.Input[1];

	if(sc.SetDefaults)
	{
		sc.GraphName = "New Daily High/Low Alert";

		sc.AutoLoop		= 0;
		sc.GraphRegion = 0;

		HighAlert.Name =  "High Alert Sound";
		HighAlert.SetAlertSoundNumber(2);

		LowAlert.Name  = "Low Alert Sound";
		LowAlert.SetAlertSoundNumber(2);

		return;
	}

	float & PreviousHigh = sc.PersistVars->f1;
	float & PreviousLow = sc.PersistVars->f2;

	if(sc.UpdateStartIndex == 0)
	{
		PreviousHigh = sc.DailyHigh;
		PreviousLow = sc.DailyLow;

		return;
	}
	SCString Message;
	if(PreviousHigh != 0.0 && sc.DailyHigh > PreviousHigh)
	{
		
		Message.Format("New Daily High: %f. Previous high: %f", sc.DailyHigh, PreviousHigh);
		sc.PlaySound(HighAlert.GetAlertSoundNumber(), Message);
	}

	if(PreviousLow != 0.0 && sc.DailyLow < PreviousLow)
	{
		Message.Format("New Daily Low: %f. Previous low: %f", sc.DailyLow,  PreviousLow);
		sc.PlaySound(LowAlert.GetAlertSoundNumber(),Message);
	}

	PreviousHigh = sc.DailyHigh;
	PreviousLow = sc.DailyLow;

}

/***************************************************************/
SCSFExport scsf_Spread4Chart(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Open = sc.Subgraph[0];
	SCSubgraphRef High = sc.Subgraph[1];
	SCSubgraphRef Low = sc.Subgraph[2];
	SCSubgraphRef Last = sc.Subgraph[3];
	SCSubgraphRef Volume = sc.Subgraph[4];
	SCSubgraphRef OpenInterest = sc.Subgraph[5];
	SCSubgraphRef OHLCAvg = sc.Subgraph[6];
	SCSubgraphRef HLCAvg = sc.Subgraph[7];
	SCSubgraphRef HLAvg = sc.Subgraph[8];


	SCInputRef Chart2Number = sc.Input[0];
	SCInputRef Chart3Number = sc.Input[1];
	SCInputRef Chart4Number = sc.Input[2];


	if (sc.SetDefaults)
	{
		sc.GraphName			= "Spread - 4 Chart";
		sc.GraphRegion			= 1;
		sc.UseGlobalChartColors = 1;
		sc.GraphDrawType		= GDT_OHLCBAR;
		sc.StandardChartHeader	= 1;

		// We are using Manual looping.
		sc.AutoLoop = false;

		Open.Name = "Open";
		Open.DrawStyle = DRAWSTYLE_LINE;
		Open.PrimaryColor = RGB(0,255,0);
		Open.DrawZeros = false;

		High.Name = "High";
		High.DrawStyle = DRAWSTYLE_LINE;
		High.PrimaryColor = RGB(0,255,0);
		High.DrawZeros = false;
		
		Low.Name = "Low";
		Low.DrawStyle = DRAWSTYLE_LINE;
		Low.PrimaryColor = RGB(0,255,0);
		Low.DrawZeros = false;

		Last.Name = "Last";
		Last.DrawStyle = DRAWSTYLE_LINE;
		Last.PrimaryColor = RGB(0,255,0);
		Last.DrawZeros = false;

		Volume.Name = "Volume";
		Volume.DrawStyle = DRAWSTYLE_IGNORE;
		Volume.PrimaryColor = RGB(255,0,0);
		Volume.DrawZeros = false;
		
		OpenInterest.Name = "# of Trades / OI";
		OpenInterest.DrawStyle = DRAWSTYLE_IGNORE;
		OpenInterest.PrimaryColor = RGB(0,0,255);
		OpenInterest.DrawZeros = false;

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

		Chart2Number.Name = "Chart 2 Number";
		Chart2Number.SetChartNumber(1);

		Chart3Number.Name = "Chart 3 Number";
		Chart3Number.SetChartNumber(1);

		Chart4Number.Name = "Chart 4 Number";
		Chart4Number.SetChartNumber(1);

		return;
	}

	// Obtain a reference to the Base Data in the specified chart. This call is relatively efficient, but it should be called as minimally as possible. To reduce the number of calls we have it outside of the primary "for" loop in this study function. And we also use Manual Looping by using sc.AutoLoop = 0. In this way, sc.GetChartBaseData is called only once per call to this study function and there are minimal number of calls to the function. sc.GetChartBaseData is a new function to get all of the Base Data arrays with one efficient call.
	SCGraphData Chart2BaseData;
	sc.GetChartBaseData(-Chart2Number.GetChartNumber(), Chart2BaseData);

	SCGraphData Chart3BaseData;
	sc.GetChartBaseData(-Chart3Number.GetChartNumber(), Chart3BaseData);

	SCGraphData Chart4BaseData;
	sc.GetChartBaseData(-Chart4Number.GetChartNumber(), Chart4BaseData);

	for (int Index = sc.UpdateStartIndex; Index < sc.ArraySize; Index++)
	{
		int Chart2Index = sc.GetNearestMatchForDateTimeIndex(Chart2Number.GetChartNumber(), Index);
		int Chart3Index = sc.GetNearestMatchForDateTimeIndex(Chart3Number.GetChartNumber(), Index);
		int Chart4Index = sc.GetNearestMatchForDateTimeIndex(Chart4Number.GetChartNumber(), Index);

		for (int SubgraphIndex = 0; SubgraphIndex < 6; SubgraphIndex++)
		{
			sc.Subgraph[SubgraphIndex][Index]
			= (sc.BaseDataIn[SubgraphIndex][Index] - Chart2BaseData[SubgraphIndex][Chart2Index] )
				- (Chart3BaseData[SubgraphIndex][Chart3Index] - Chart4BaseData[SubgraphIndex][Chart4Index] );
		}

		sc.Subgraph[ SC_HIGH][Index] = max(sc.Subgraph[SC_OPEN][Index],
			max(sc.Subgraph[SC_HIGH][Index],
			max(sc.Subgraph[SC_LOW][Index],sc.Subgraph[SC_LAST][Index])
			)
			);

		sc.Subgraph[SC_LOW][Index] = min(sc.Subgraph[SC_OPEN][Index],
			min(sc.Subgraph[SC_HIGH][Index],
			min(sc.Subgraph[SC_LOW][Index],sc.Subgraph[SC_LAST][Index])
			)
			);

		sc.CalculateOHLCAverages(Index);
	}

	//SCString Chart1Name = sc.GetStudyNameFromChart(sc.ChartNumber, 0);
	//SCString Chart2Name = sc.GetStudyNameFromChart(Chart2Number.GetChartNumber(), 0);
	//sc.GraphName.Format("Difference %s - %s", Chart1Name.GetChars(), Chart2Name.GetChars());
}

/************************************************************************/


SCSFExport scsf_QStick(SCStudyInterfaceRef sc)
{
	SCSubgraphRef QStick = sc.Subgraph[0];
	SCInputRef Length = sc.Input[0];
	if (sc.SetDefaults)
	{
		// Set the configuration and defaults
		
		sc.GraphName = "Q Stick";
		sc.AutoLoop = 1;
		
		// During development set this flag to 1, so the DLL can be modified. When development is done, set it to 0 to improve performance.
		sc.FreeDLL = 0;

		QStick.Name = "QS";
		QStick.DrawStyle = DRAWSTYLE_LINE;
		QStick.LineWidth = 2;
		QStick.PrimaryColor = RGB(0,255,0);
		Length.Name = "Length";
		Length.SetInt(4);
		sc.GraphRegion = 1;

		
		return;
	}
	
	
	
	// Do data processing
	QStick.Arrays[0][sc.Index] = sc.BaseData[SC_LAST][sc.Index] - sc.BaseData[SC_OPEN][sc.Index] ;
	sc.Summation( QStick.Arrays[0], QStick.Arrays[1], Length.GetInt() );


	QStick.Data[sc.Index] = QStick.Arrays[1][sc.Index] / Length.GetInt() ;
	
	
}


/***************************************************************/
SCSFExport scsf_HorizontalLineAtTime(SCStudyInterfaceRef sc)
{
	SCSubgraphRef HorizontalLine = sc.Subgraph[0];

	SCInputRef InputData = sc.Input[0];

	SCInputRef StartTime = sc.Input[1];
	SCInputRef UseStopTime = sc.Input[2];
	SCInputRef StopTime = sc.Input[3];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Horizontal Line at Time";
		sc.GraphRegion = 0;
		sc.AutoLoop = 1;
		
		HorizontalLine.Name = "Line";
		HorizontalLine.DrawStyle = DRAWSTYLE_STAIR;
		HorizontalLine.LineWidth= 2;
		HorizontalLine.PrimaryColor = RGB(0,255,0);
		HorizontalLine.DrawZeros = false;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		StartTime.Name = "Start Time";
		StartTime.SetTime(HMS_TIME(12, 00, 0));
		
		UseStopTime.Name = "Use Stop Time";
		UseStopTime.SetYesNo(false);

		StopTime.Name = "Stop Time";
		StopTime.SetTime(HMS_TIME(23,59,59));


		return;
	}

	 float &PriorValue = sc.PersistVars->f1;
	 SCDateTime &StartDateTime=sc.PersistVars->scdt1;
	 SCDateTime &StopDateTime=sc.PersistVars->scdt2;

		 if (sc.Index == 0)
		 {
			 StopDateTime=0;
			 PriorValue=FLT_MIN;
		 }

		 if (sc.BaseDateTimeIn[sc.Index]> StopDateTime)
			 PriorValue=FLT_MIN;

		 if( PriorValue==FLT_MIN)
		 {
			 StartDateTime.SetDateTime(sc.BaseDateTimeIn[sc.Index].GetDate(),StartTime.GetTime() );
			 StopDateTime =StartDateTime+SCDateTime(0,HMS_TIME(23, 59, 59));
			 if(UseStopTime.GetYesNo())
			 {
				 if(StartTime.GetTime() <= StopTime.GetTime())
					 StopDateTime.SetDateTime( sc.BaseDateTimeIn[sc.Index].GetDate(),StopTime.GetTime() );
				 else
					 StopDateTime.SetDateTime( sc.BaseDateTimeIn[sc.Index].GetDate()+1,StopTime.GetTime() );
			 }

		 }



	if (sc.BaseDateTimeIn[sc.Index-1] < StartDateTime && sc.BaseDateTimeIn[sc.Index] >= StartDateTime	)
	{
		HorizontalLine[sc.Index] =sc.BaseData [InputData.GetInputDataIndex()][sc.Index];
		PriorValue =HorizontalLine[sc.Index] ;
		
	}
	
	 if (PriorValue != FLT_MIN)
	{
			HorizontalLine[sc.Index] =PriorValue;
	}


}

/***************************************************************/
SCSFExport scsf_BarsWithZeros(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Open = sc.Subgraph[0];
	SCSubgraphRef High = sc.Subgraph[1];
	SCSubgraphRef Low = sc.Subgraph[2];
	SCSubgraphRef Last = sc.Subgraph[3];
	SCSubgraphRef Volume = sc.Subgraph[4];
	SCSubgraphRef OpenInterest = sc.Subgraph[5];
	SCSubgraphRef OHLCAvg = sc.Subgraph[6];
	SCSubgraphRef HLCAvg = sc.Subgraph[7];
	SCSubgraphRef HLAvg = sc.Subgraph[8];
	SCSubgraphRef BidVol = sc.Subgraph[9];
	SCSubgraphRef AskVol = sc.Subgraph[10];

	if (sc.SetDefaults)
	{
		sc.GraphName			= "Chart Bars with Zero Values";
		sc.ValueFormat			=  VALUEFORMAT_INHERITED;
		sc.GraphRegion = 0;
		sc.DisplayAsMainPriceGraph = true;
		sc.GraphUsesChartColors = true;
		sc.GraphDrawType		= GDT_OHLCBAR;
		sc.StandardChartHeader	= 1;

		sc.AutoLoop = true;

		Open.Name = "Open";
		Open.DrawStyle = DRAWSTYLE_LINE;
		Open.PrimaryColor = RGB(0,255,0);
		Open.DrawZeros = true;

		High.Name = "High";
		High.DrawStyle = DRAWSTYLE_LINE;
		High.PrimaryColor = RGB(255,0,255);
		High.DrawZeros = true;

		Low.Name = "Low";
		Low.DrawStyle = DRAWSTYLE_LINE;
		Low.PrimaryColor = RGB(255,255,0);
		Low.DrawZeros = true;

		Last.Name = "Last";
		Last.DrawStyle = DRAWSTYLE_LINE;
		Last.PrimaryColor = RGB(255,127,0);
		Last.DrawZeros = true;

		Volume.Name = "Volume";
		Volume.DrawStyle = DRAWSTYLE_IGNORE;
		Volume.PrimaryColor = RGB(255,0,0);
		Volume.DrawZeros = true;

		OpenInterest.Name = "# of Trades / OI";
		OpenInterest.DrawStyle = DRAWSTYLE_IGNORE;
		OpenInterest.PrimaryColor = RGB(0,0,255);
		OpenInterest.DrawZeros = true;

		OHLCAvg.Name = "OHLC Avg";
		OHLCAvg.DrawStyle = DRAWSTYLE_IGNORE;
		OHLCAvg.PrimaryColor = RGB(127,0,255);
		OHLCAvg.DrawZeros = true;

		HLCAvg.Name = "HLC Avg";
		HLCAvg.DrawStyle = DRAWSTYLE_IGNORE;
		HLCAvg.PrimaryColor = RGB(0,255,255);
		HLCAvg.DrawZeros = true;

		HLAvg.Name = "HL Avg";
		HLAvg.DrawStyle = DRAWSTYLE_IGNORE;
		HLAvg.PrimaryColor = RGB(0,127,255);
		HLAvg.DrawZeros = true;

		BidVol.Name = "Bid Vol";
		BidVol.DrawStyle = DRAWSTYLE_IGNORE;
		BidVol.PrimaryColor = RGB(0,255,0);
		BidVol.DrawZeros = true;

		AskVol.Name = "Ask Vol";
		AskVol.DrawStyle = DRAWSTYLE_IGNORE;
		AskVol.PrimaryColor = RGB(0,255,0);
		AskVol.DrawZeros = true;

		return;
	}

	for(int SubgraphIndex = 0; SubgraphIndex < SC_ASKVOL; SubgraphIndex++)
	{
		sc.Subgraph[SubgraphIndex][sc.Index] = sc.BaseData[SubgraphIndex][sc.Index];
	}
}



/***************************************************************/
SCSFExport scsf_InitialBalance(SCStudyInterfaceRef sc)
{
	SCSubgraphRef IBHExt6  = sc.Subgraph[0];
	SCSubgraphRef IBHExt5  = sc.Subgraph[1];
	SCSubgraphRef IBHExt4  = sc.Subgraph[2];
	SCSubgraphRef IBHExt3  = sc.Subgraph[3];
	SCSubgraphRef IBHExt2  = sc.Subgraph[4];
	SCSubgraphRef IBHExt1  = sc.Subgraph[5];
	SCSubgraphRef IBHigh   = sc.Subgraph[6];
	SCSubgraphRef IBMid    = sc.Subgraph[7];
	SCSubgraphRef IBLow    = sc.Subgraph[8];
	SCSubgraphRef IBLExt1  = sc.Subgraph[9];
	SCSubgraphRef IBLExt2  = sc.Subgraph[10];
	SCSubgraphRef IBLExt3  = sc.Subgraph[11];
	SCSubgraphRef IBLExt4  = sc.Subgraph[12];
	SCSubgraphRef IBLExt5  = sc.Subgraph[13];
	SCSubgraphRef IBLExt6  = sc.Subgraph[14];

	SCInputRef iIBType      = sc.Input[0];
	SCInputRef iStartTime   = sc.Input[1];
	SCInputRef iEndTime     = sc.Input[2];
	SCInputRef iNumDays     = sc.Input[3];
	SCInputRef iRoundExt    = sc.Input[4];
	SCInputRef iNumberDaysToCalculate = sc.Input[5];
	SCInputRef iNumMin      = sc.Input[6];

	SCInputRef iMultiplier1 = sc.Input[10];
	SCInputRef iMultiplier2 = sc.Input[11];
	SCInputRef iMultiplier3 = sc.Input[12];
	SCInputRef iMultiplier4 = sc.Input[13];
	SCInputRef iMultiplier5 = sc.Input[14];
	SCInputRef iMultiplier6 = sc.Input[15];

	if (sc.SetDefaults)
	{
		sc.GraphName		= "Initial Balance";
		sc.DrawZeros		= 0;
		sc.GraphRegion		= 0;
		sc.AutoLoop			= 1;
		sc.FreeDLL			= 0;

		sc.ScaleRangeType = SCALE_SAMEASREGION;

		IBHExt6.Name = "IB High Ext 6";
		IBHExt6.PrimaryColor = RGB(0, 0, 255);
		IBHExt6.DrawStyle = DRAWSTYLE_DASH;
		IBHExt6.DrawZeros = false;

		IBHExt5.Name = "IB High Ext 5";
		IBHExt5.PrimaryColor = RGB(0, 0, 255);
		IBHExt5.DrawStyle = DRAWSTYLE_DASH;
		IBHExt5.DrawZeros = false;

		IBHExt4.Name = "IB High Ext 4";
		IBHExt4.PrimaryColor = RGB(0, 0, 255);
		IBHExt4.DrawStyle = DRAWSTYLE_DASH;
		IBHExt4.DrawZeros = false;

		IBHExt3.Name = "IB High Ext 3";
		IBHExt3.PrimaryColor = RGB(0, 0, 255);
		IBHExt3.DrawStyle = DRAWSTYLE_DASH;
		IBHExt3.DrawZeros = false;

		IBHExt2.Name = "IB High Ext 2";
		IBHExt2.PrimaryColor = RGB(0, 0, 255);
		IBHExt2.DrawStyle = DRAWSTYLE_DASH;
		IBHExt2.DrawZeros = false;

		IBHExt1.Name = "IB High Ext 1";
		IBHExt1.PrimaryColor = RGB(0, 0, 255);
		IBHExt1.DrawStyle = DRAWSTYLE_DASH;
		IBHExt1.DrawZeros = false;

		IBHigh.Name = "IB High";
		IBHigh.PrimaryColor = RGB(0, 0, 255);
		IBHigh.DrawStyle = DRAWSTYLE_DASH;
		IBHigh.DrawZeros = false;

		IBMid.Name = "IB Mid";
		IBMid.PrimaryColor = RGB(255, 255, 255);
		IBMid.DrawStyle = DRAWSTYLE_DASH;
		IBMid.DrawZeros = false;

		IBLow.Name = "IB Low";
		IBLow.PrimaryColor = RGB(255, 0, 0);
		IBLow.DrawStyle = DRAWSTYLE_DASH;
		IBLow.DrawZeros = false;

		IBLExt1.Name = "IB Low Ext 1";
		IBLExt1.PrimaryColor = RGB(255, 0, 0);
		IBLExt1.DrawStyle = DRAWSTYLE_DASH;
		IBLExt1.DrawZeros = false;

		IBLExt2.Name = "IB Low Ext 2";
		IBLExt2.PrimaryColor = RGB(255, 0, 0);
		IBLExt2.DrawStyle = DRAWSTYLE_DASH;
		IBLExt2.DrawZeros = false;

		IBLExt3.Name = "IB Low Ext 3";
		IBLExt3.PrimaryColor = RGB(255, 0, 0);
		IBLExt3.DrawStyle = DRAWSTYLE_DASH;
		IBLExt3.DrawZeros = false;

		IBLExt4.Name = "IB Low Ext 4";
		IBLExt4.PrimaryColor = RGB(255, 0, 0);
		IBLExt4.DrawStyle = DRAWSTYLE_DASH;
		IBLExt4.DrawZeros = false;

		IBLExt5.Name = "IB Low Ext 5";
		IBLExt5.PrimaryColor = RGB(255, 0, 0);
		IBLExt5.DrawStyle = DRAWSTYLE_DASH;
		IBLExt5.DrawZeros = false;

		IBLExt6.Name = "IB Low Ext 6";
		IBLExt6.PrimaryColor = RGB(255, 0, 0);
		IBLExt6.DrawStyle = DRAWSTYLE_DASH;
		IBLExt6.DrawZeros = false;



		// Inputs
		iIBType.Name = "Initial Balance Type";
		iIBType.SetCustomInputStrings("Daily;Weekly;Weekly Include Sunday;Intraday");
		iIBType.SetCustomInputIndex(0);

		iStartTime.Name = "Start Time";
		iStartTime.SetTime(HMS_TIME(9, 30, 0));
		
		iEndTime.Name = "End Time";
		iEndTime.SetTime(HMS_TIME(10, 29, 59));

		iNumDays.Name = "Weekly: Num Days";
		iNumDays.SetInt(2);
		iNumDays.SetIntLimits(1, 5);

		iNumMin.Name = "Intraday: Num Minutes";
		iNumMin.SetInt(15);

		iRoundExt.Name = "Round Extensions to TickSize";
		iRoundExt.SetYesNo(1);

		iNumberDaysToCalculate. Name = "Number of Days to Calculate";
		iNumberDaysToCalculate .SetInt(100000);
		iNumberDaysToCalculate.SetIntLimits(1,INT_MAX);

		iMultiplier1.Name = "Extension Multiplier 1";
		iMultiplier1.SetFloat(.5f);
		iMultiplier2.Name = "Extension Multiplier 2";
		iMultiplier2.SetFloat(1.0f);
		iMultiplier3.Name = "Extension Multiplier 3";
		iMultiplier3.SetFloat(1.5f);
		iMultiplier4.Name = "Extension Multiplier 4";
		iMultiplier4.SetFloat(2.0f);
		iMultiplier5.Name = "Extension Multiplier 5";
		iMultiplier5.SetFloat(2.5f);
		iMultiplier6.Name = "Extension Multiplier 6";
		iMultiplier6.SetFloat(3.0f);

		return;
	}

	// persist vars
	int&   PeriodFirstIndex = sc.PersistVars->i1;

	float& PeriodHigh       = sc.PersistVars->f1;
	float& PeriodLow        = sc.PersistVars->f2;
	float& PeriodMid        = sc.PersistVars->f3;
	float& PeriodHighExt1   = sc.PersistVars->f4;
	float& PeriodHighExt2   = sc.PersistVars->f5;
	float& PeriodHighExt3   = sc.PersistVars->f6;
	float& PeriodHighExt4   = sc.PersistVars->f7;
	float& PeriodHighExt5   = sc.PersistVars->f8;
	float& PeriodHighExt6   = sc.PersistVars->f9;
	float& PeriodLowExt1    = sc.PersistVars->f10;
	float& PeriodLowExt2    = sc.PersistVars->f11;
	float& PeriodLowExt3    = sc.PersistVars->f12;
	float& PeriodLowExt4    = sc.PersistVars->f13;
	float& PeriodLowExt5    = sc.PersistVars->f14;
	float& PeriodLowExt6    = sc.PersistVars->f15;

	//Reset persistent variables upon full calculation
	if (sc.Index == 0)
	{
		PeriodFirstIndex = -1;
		PeriodHigh = -FLT_MAX;
		PeriodLow  =  FLT_MAX;
	}

	SCDateTime LastBarDateTime = sc.BaseDateTimeIn[sc.ArraySize-1];
	SCDateTime FirstCalculationDate = LastBarDateTime.GetDate()- (iNumberDaysToCalculate.GetInt()-1);


	SCDateTime CurrBarDateTime = sc.BaseDateTimeIn[sc.Index];
	SCDateTime PrevBarDateTime = sc.BaseDateTimeIn[sc.Index-1];

	if (CurrBarDateTime.GetDate()<FirstCalculationDate)//Limit calculation to the specified number of days back
		return;

	bool Daily  = iIBType.GetIndex() == 0;
	bool Weekly = iIBType.GetIndex() == 1 || iIBType.GetIndex() == 2;
	bool Intraday = iIBType.GetIndex() == 3;
	bool IncludeSunday = iIBType.GetIndex() == 2;


	// check if starting new period, reset vars
	if (PeriodFirstIndex < 0)
	{
		SCDateTime StartDateTime = CurrBarDateTime;
		StartDateTime.SetTime(iStartTime.GetTime());

		if (Weekly)
		{
			int PeriodStartDayOfWeek = IncludeSunday ? SUNDAY : MONDAY;

			int DayOfWeek = StartDateTime.GetDayOfWeek();

			if (DayOfWeek != PeriodStartDayOfWeek)
				StartDateTime += ((7-DayOfWeek+PeriodStartDayOfWeek) * DAYS);
		}
			
		if (PrevBarDateTime < StartDateTime && CurrBarDateTime >= StartDateTime)
		{
			PeriodFirstIndex = sc.Index;
			PeriodHigh = -FLT_MAX;
			PeriodLow  = FLT_MAX;
		}
	}

	// check end of period
	if (PeriodFirstIndex >= 0)
	{
		SCDateTime EndDateTime = CurrBarDateTime;
		EndDateTime.SetTime(iEndTime.GetTime());

		if (Weekly)
		{
			int PeriodEndDayOfWeek = IncludeSunday ? iNumDays.GetInt() - 1 : iNumDays.GetInt();

			int DayOfWeek = EndDateTime.GetDayOfWeek();

			if (DayOfWeek != PeriodEndDayOfWeek)
				EndDateTime += ((PeriodEndDayOfWeek-DayOfWeek) * DAYS);
		}

		// check start of new intraday period
		if (Intraday)
		{
			SCDateTime IntradayEndDateTime = sc.BaseDateTimeIn[PeriodFirstIndex] + iNumMin.GetInt()*MINUTES;

			if (PrevBarDateTime < IntradayEndDateTime && CurrBarDateTime >= IntradayEndDateTime)
			{
				PeriodFirstIndex = sc.Index;
				PeriodHigh = -FLT_MAX;
				PeriodLow  = FLT_MAX;
			}
		}

		if (PrevBarDateTime < EndDateTime && CurrBarDateTime >= EndDateTime)
		{
			PeriodFirstIndex = -1;
		}
	}



	// collecting data, back propagate if changed
	if (PeriodFirstIndex >= 0)
	{
		bool Changed = false;

		if (sc.High[sc.Index] > PeriodHigh)
		{
			PeriodHigh = sc.High[sc.Index];
			Changed = true;
		}

		if (sc.Low[sc.Index] < PeriodLow)
		{
			PeriodLow = sc.Low[sc.Index];
			Changed = true;
		}

		if (Changed)
		{
			PeriodMid = (PeriodHigh + PeriodLow) / 2.0f;

			float Range = PeriodHigh - PeriodLow;

			PeriodHighExt1 = PeriodHigh + iMultiplier1.GetFloat() * Range; 
			PeriodHighExt2 = PeriodHigh + iMultiplier2.GetFloat() * Range; 
			PeriodHighExt3 = PeriodHigh + iMultiplier3.GetFloat() * Range; 
			PeriodHighExt4 = PeriodHigh + iMultiplier4.GetFloat() * Range; 
			PeriodHighExt5 = PeriodHigh + iMultiplier5.GetFloat() * Range; 
			PeriodHighExt6 = PeriodHigh + iMultiplier6.GetFloat() * Range; 

			PeriodLowExt1 = PeriodLow - iMultiplier1.GetFloat() * Range; 
			PeriodLowExt2 = PeriodLow - iMultiplier2.GetFloat() * Range; 
			PeriodLowExt3 = PeriodLow - iMultiplier3.GetFloat() * Range; 
			PeriodLowExt4 = PeriodLow - iMultiplier4.GetFloat() * Range; 
			PeriodLowExt5 = PeriodLow - iMultiplier5.GetFloat() * Range; 
			PeriodLowExt6 = PeriodLow - iMultiplier6.GetFloat() * Range; 

			if (iRoundExt.GetYesNo())
			{
				PeriodHighExt1 = sc.RoundToTickSize(PeriodHighExt1, sc.TickSize); 
				PeriodHighExt2 = sc.RoundToTickSize(PeriodHighExt2, sc.TickSize); 
				PeriodHighExt3 = sc.RoundToTickSize(PeriodHighExt3, sc.TickSize); 
				PeriodHighExt4 = sc.RoundToTickSize(PeriodHighExt4, sc.TickSize); 
				PeriodHighExt5 = sc.RoundToTickSize(PeriodHighExt5, sc.TickSize); 
				PeriodHighExt6 = sc.RoundToTickSize(PeriodHighExt6, sc.TickSize); 

				PeriodLowExt1 = sc.RoundToTickSize(PeriodLowExt1, sc.TickSize); 
				PeriodLowExt2 = sc.RoundToTickSize(PeriodLowExt2, sc.TickSize); 
				PeriodLowExt3 = sc.RoundToTickSize(PeriodLowExt3, sc.TickSize); 
				PeriodLowExt4 = sc.RoundToTickSize(PeriodLowExt4, sc.TickSize); 
				PeriodLowExt5 = sc.RoundToTickSize(PeriodLowExt5, sc.TickSize); 
				PeriodLowExt6 = sc.RoundToTickSize(PeriodLowExt6, sc.TickSize); 
			}

			for (int Index = PeriodFirstIndex; Index < sc.Index; Index++)
			{
				IBHigh[Index]  = PeriodHigh;
				IBLow[Index]   = PeriodLow;
				IBMid[Index]   = PeriodMid;
				IBHExt1[Index] = PeriodHighExt1;
				IBHExt2[Index] = PeriodHighExt2;
				IBHExt3[Index] = PeriodHighExt3;
				IBHExt4[Index] = PeriodHighExt4;
				IBHExt5[Index] = PeriodHighExt5;
				IBHExt6[Index] = PeriodHighExt6;
				IBLExt1[Index] = PeriodLowExt1;
				IBLExt2[Index] = PeriodLowExt2;
				IBLExt3[Index] = PeriodLowExt3;
				IBLExt4[Index] = PeriodLowExt4;
				IBLExt5[Index] = PeriodLowExt5;
				IBLExt6[Index] = PeriodLowExt6;			
			}
		}
	}

	// plot current values
	if (PeriodLow != FLT_MAX)
	{
		IBHigh[sc.Index]  = PeriodHigh;
		IBLow[sc.Index]   = PeriodLow;
		IBMid[sc.Index]   = PeriodMid;
		IBHExt1[sc.Index] = PeriodHighExt1;
		IBHExt2[sc.Index] = PeriodHighExt2;
		IBHExt3[sc.Index] = PeriodHighExt3;
		IBHExt4[sc.Index] = PeriodHighExt4;
		IBHExt5[sc.Index] = PeriodHighExt5;
		IBHExt6[sc.Index] = PeriodHighExt6;
		IBLExt1[sc.Index] = PeriodLowExt1;
		IBLExt2[sc.Index] = PeriodLowExt2;
		IBLExt3[sc.Index] = PeriodLowExt3;
		IBLExt4[sc.Index] = PeriodLowExt4;
		IBLExt5[sc.Index] = PeriodLowExt5;
		IBLExt6[sc.Index] = PeriodLowExt6;
	}
}

/***************************************************************/
SCSFExport scsf_RenkoVisualOpenCloseValues(SCStudyInterfaceRef sc)
{
	SCSubgraphRef RenkoOpen  = sc.Subgraph[0];
	SCSubgraphRef RenkoClose = sc.Subgraph[1];

	if (sc.SetDefaults)
	{
		sc.GraphName		= "Renko Visual Open/Close Values";
		sc.DrawZeros		= 0;
		sc.GraphRegion		= 0;
		sc.AutoLoop			= 1;
		sc.FreeDLL			= 0;

		sc.ScaleRangeType = SCALE_SAMEASREGION;

		RenkoOpen.Name = "Renko Open";
		RenkoOpen.PrimaryColor = RGB(255, 255, 255);
		RenkoOpen.LineWidth = 3;
		RenkoOpen.DrawStyle = DRAWSTYLE_LEFTHASH;
		RenkoOpen.DrawZeros = false;

		RenkoClose.Name = "Renko Close";
		RenkoClose.PrimaryColor = RGB(255, 255, 255);
		RenkoClose.LineWidth = 3;
		RenkoClose.DrawStyle = DRAWSTYLE_RIGHTHASH;
		RenkoClose.DrawZeros = false;

		return;
	}

	if (sc.RenkoTicksPerBar != 0)
	{
		RenkoOpen[sc.Index]  = sc.BaseData[SC_RENKO_OPEN][sc.Index];
		RenkoClose[sc.Index] = sc.BaseData[SC_RENKO_CLOSE][sc.Index];
	}
}

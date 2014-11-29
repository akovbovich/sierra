#include "sierrachart.h"

#include "scstudyfunctions.h"

/*==========================================================================*/
SCSFExport scsf_DoubleStochastic(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Bressert   = sc.Subgraph[0];
	SCSubgraphRef UpperLine  = sc.Subgraph[1];
	SCSubgraphRef LowerLine  = sc.Subgraph[2];
	SCSubgraphRef DSTrigger = sc.Subgraph[3];

	SCInputRef HighLowPeriodLength = sc.Input[0];
	SCInputRef EmaLength           = sc.Input[1];
	SCInputRef UpperLineValue      = sc.Input[2];
	SCInputRef LowerLineValue      = sc.Input[3];
	SCInputRef Version             = sc.Input[4];
	SCInputRef SmoothingLength     = sc.Input[5];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Double Stochastic - Bressert";
		sc.StudyDescription = "";

		sc.GraphRegion = 2;
		sc.AutoLoop = 1;

		Bressert.Name = "DS";
		Bressert.DrawStyle = DRAWSTYLE_LINE;
		Bressert.PrimaryColor = RGB(0,255,0);
		Bressert.DrawZeros = false;

		DSTrigger.Name = "DS Trigger";
		DSTrigger.DrawStyle = DRAWSTYLE_LINE;
		DSTrigger.PrimaryColor = RGB(255,0,255);
		DSTrigger.DrawZeros = false;

		UpperLine.Name = "Upper Line";
		UpperLine.DrawStyle = DRAWSTYLE_LINE;
		UpperLine.PrimaryColor = RGB(255,255,0);
		UpperLine.DrawZeros = false;
		
		LowerLine.Name = "Lower Line";
		LowerLine.DrawStyle = DRAWSTYLE_LINE;
		LowerLine.PrimaryColor = RGB(255,127,0);
		LowerLine.DrawZeros = false;


		HighLowPeriodLength.Name = "High & Low Period Length";
		HighLowPeriodLength.SetInt(15);
		HighLowPeriodLength.SetIntLimits(1, MAX_STUDY_LENGTH);
		HighLowPeriodLength.DisplayOrder = 1;

		EmaLength.Name = "Stochastic Exponential Mov Avg Length";
		EmaLength.SetInt(18);
		EmaLength.SetIntLimits(1, MAX_STUDY_LENGTH);
		EmaLength.DisplayOrder = 2;

		SmoothingLength.Name = "Smoothing Length";
		SmoothingLength.SetInt(3);
		SmoothingLength.SetIntLimits(1, MAX_STUDY_LENGTH);
		SmoothingLength.DisplayOrder = 3;

		UpperLineValue.Name = "Upper Line Value";
		UpperLineValue.SetFloat(80.0f);
		UpperLineValue.DisplayOrder = 4;

		LowerLineValue.Name = "Lower Line Value";
		LowerLineValue.SetFloat(20.0f);
		LowerLineValue.DisplayOrder = 5;

		Version.SetInt(1);

		//During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;

		return;
	}

	if (Version.GetInt() < 1)
	{
		Version.SetInt(1);
		SmoothingLength.SetInt(3);
	}

	// Do data processing

	int Index = sc.Index;
	UpperLine[Index] = UpperLineValue.GetFloat();
	LowerLine[Index] = LowerLineValue.GetFloat();


	int HighLowPeriod =  HighLowPeriodLength.GetInt();

	float High = sc.GetHighest(sc.High, HighLowPeriod);
	float Low = sc.GetLowest(sc.Low, HighLowPeriod);
	if (High - Low != 0.0f)
		Bressert.Arrays[0][Index] = ((sc.Close[Index] - Low) / (High - Low)) * 100.0f;
	else
		Bressert.Arrays[0][Index] = 0.0f;

	sc.ExponentialMovAvg(Bressert.Arrays[0], DSTrigger, EmaLength.GetInt());

	float TriggerHigh = sc.GetHighest(DSTrigger, HighLowPeriod);
	float TriggerLow = sc.GetLowest(DSTrigger, HighLowPeriod);
	if (TriggerHigh - TriggerLow != 0.0f)
		Bressert.Arrays[1][Index] = ((DSTrigger[Index] - TriggerLow) / (TriggerHigh - TriggerLow)) * 100.0f;
	else
		Bressert.Arrays[1][Index] = 0.0f;

	sc.ExponentialMovAvg(Bressert.Arrays[1], Bressert.Arrays[2], EmaLength.GetInt());
	sc.ExponentialMovAvg(Bressert.Arrays[2], Bressert, SmoothingLength.GetInt());
}

/*==========================================================================*/
SCSFExport scsf_PreviousBarClose(SCStudyInterfaceRef sc)
{
	SCSubgraphRef PrevClose = sc.Subgraph[0];

	SCInputRef InputData= sc.Input[0];
	SCInputRef Version = sc.Input[1];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Previous Bar Close";
		sc.AutoLoop = 1;

		PrevClose.DrawStyle = DRAWSTYLE_STAIR;
		PrevClose.Name = "PB Close";
		PrevClose.LineLabel  |= (LL_DISPLAY_VALUE | LL_VALUE_ALIGN_VALUES_SCALE);
		PrevClose.PrimaryColor = RGB(0,255,0);

		sc.GraphRegion = 0;
		
		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);
		
		Version.SetInt(1);
		
		//During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;


		return;
	}

	if (Version.GetInt() < 1)
	{
		InputData.SetInputDataIndex(SC_LAST);
		Version.SetInt(1);
	}
	
	unsigned long InputDataIndex = InputData.GetInputDataIndex();
	
	// Do data processing
	PrevClose[sc.Index] = sc.BaseData[InputDataIndex][sc.Index - 1];
}

/*==========================================================================*/
SCSFExport scsf_ChangeForDay(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Change = sc.Subgraph[0];

	SCInputRef TickSize = sc.Input[0];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Change For Day";

		sc.GraphRegion = 1;	

		Change.Name = "Change";
		Change.DrawStyle = DRAWSTYLE_DASH;
		Change.SecondaryColorUsed = 1;
		Change.SecondaryColor = RGB(255,0,0);

		TickSize.Name = "Tick Size";
		TickSize.SetFloat(0.25f);

		//During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;


		return;
	}

	// Do data processing
	Change[sc.Index] = sc.Close[sc.ArraySize - 1];

	long OpeningIndex = sc.GetNearestMatchForSCDateTime(sc.ChartNumber, SCDateTime(sc.BaseDateTimeIn.DateAt(sc.ArraySize-1), 0));

	if (sc.BaseDateTimeIn.DateAt(OpeningIndex) != sc.BaseDateTimeIn.DateAt(sc.ArraySize-1))
		OpeningIndex++;

	float ChangeForDay = sc.Close[sc.ArraySize-1] - sc.Open[OpeningIndex];

	for (int i = OpeningIndex; i < sc.ArraySize; i++)
	{
		Change[i] = ChangeForDay/TickSize.GetFloat();
		if (Change[i] >= 0)
			Change.DataColor[i] = Change.PrimaryColor;
		else
			Change.DataColor[i] = Change.SecondaryColor ;

	}
}

/*==========================================================================*/
SCSFExport scsf_jbvs(SCStudyInterfaceRef sc) // scsf_Name
{   
	SCSubgraphRef StopL = sc.Subgraph[0];
	SCSubgraphRef StopS = sc.Subgraph[1];

	SCSubgraphRef LLV = sc.Subgraph[6];
	SCSubgraphRef HHV = sc.Subgraph[7];
	SCSubgraphRef LongTrigger = sc.Subgraph[8];
	SCSubgraphRef ShortTrigger = sc.Subgraph[9];
	SCSubgraphRef ATR = sc.Subgraph[11];
	SCSubgraphRef Temp12 = sc.Subgraph[12];
	SCSubgraphRef Temp13 = sc.Subgraph[13];
	SCSubgraphRef LongStopValue = sc.Subgraph[14];
	SCSubgraphRef ShortStopValue = sc.Subgraph[15];
 
	// Configuration
  	if (sc.SetDefaults)
 	{   
 		sc.GraphName = "JBVS"; // study name shown in F6 menu and on the chart
 		sc.StudyDescription = "Jim Berg's Volatility System"; // F6-study-function - r-pane
		sc.GraphRegion = 0;    // zero based region number
		
		//During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;
		
		StopL.Name = "Stop L";
		StopL.DrawStyle = DRAWSTYLE_STAIR;
		StopL.PrimaryColor = RGB(0,185,47); // green
		
		// sc.Input[0].Name = "ATR Periods";
		// sc.Input[0].SetInt(20);            // default value
		// sc.Input[0].SetIntLimits(1,MAX_STUDY_LENGTH); // min-max value
		
		StopS.Name = "Stop S";
		StopS.DrawStyle = DRAWSTYLE_STAIR;
		StopS.PrimaryColor = RGB(255,0,128); // red
		
		// sc.Input[0].Name = "ATR Periods";
		// sc.Input[0].SetInt(20);            // default value
		// sc.Input[0].SetIntLimits(1,MAX_STUDY_LENGTH); // min-max value

		sc.AutoLoop = 1;
		
 		return;
 	}   
	
	// Data processing
    
    // LongSignal  = C > ( LLV( L, 20 ) + 2 * ATR( 10 ) ); // ATR with Wilder's MA
    // ShortSignal = C < ( HHV( H, 20 ) - 2 * ATR( 10 ) );
    
    // LongStopValue    = HHV( C - 2 * ATR(10), 15 );
    // ShortStopValue   = LLV( C + 2 * ATR(10), 15 );
    
    sc.DataStartIndex = 21; // start drawing subgraph at bar #21 (zero based)
	
	// LLV(L,20): subgraph #6
	sc.Lowest(sc.Low, LLV, sc.Index, 20);
	
	// HHV(H,20): subgraph#7
	sc.Highest(sc.High, HHV, sc.Index, 20);
	
	// ATR(10, Wilder's MA): subgraph #11. Intermediate TR: subgraph #10 (not plotted)
	sc.ATR(sc.BaseDataIn, ATR, sc.Index, 10, MOVAVGTYPE_WILDERS);
		
	LongTrigger[sc.Index] = LLV[sc.Index] + 2 * ATR[sc.Index]; // Long  Signal trigger
	ShortTrigger[sc.Index] = HHV[sc.Index] - 2 * ATR[sc.Index]; // Short Signal trigger
	Temp12[sc.Index] = sc.Close[sc.Index] - 2 * ATR[sc.Index];
	Temp13[sc.Index] = sc.Close[sc.Index] + 2 * ATR[sc.Index];
	
	// LongStopValue: subgraph #14
	sc.Highest(Temp12, LongStopValue, 15);
	
	// ShortStopValue: subgraph #15
	sc.Lowest(Temp13, ShortStopValue, 15);
	
	// If LongSignal=true, plot LongStopValue in Subgraph #0, otherwise plot zero
	if ( sc.Close[sc.Index] > LongTrigger[sc.Index] )
		StopL[sc.Index] = LongStopValue[sc.Index];
	else
		StopL[sc.Index] = 0;
		
	// If ShortSignal=true, plot ShortStopValue in Subgraph #1, otherwise plot zero
	if ( sc.Close[sc.Index] < ShortTrigger[sc.Index] )
		StopS[sc.Index] = ShortStopValue[sc.Index];
	else
		StopS[sc.Index] = 0;
}  


/*==========================================================================*/
SCSFExport scsf_TriangularMovingAverage(SCStudyInterfaceRef sc)
{
	SCSubgraphRef TMA = sc.Subgraph[0];

	SCInputRef InputData = sc.Input[0];
	SCInputRef Length = sc.Input[1];

    // Configuration
    if (sc.SetDefaults)
    {
        sc.GraphName = "Moving Average - Triangular"; 
        sc.StudyDescription = "Triangular Moving Average"; 
        sc.GraphRegion = 0;    // zero based region number

		// During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;

        TMA.Name = "TMA";
        TMA.DrawStyle = DRAWSTYLE_LINE;
        TMA.PrimaryColor = RGB(0,185,47); // green

        InputData.Name = "Input Data";
        InputData.SetInputDataIndex(SC_LAST);

        Length.Name = "Length";
		Length.SetInt(9);          
        Length.SetIntLimits(1,MAX_STUDY_LENGTH);

        sc.AutoLoop = 1;

        return;
    }

    // Data processing

    sc.DataStartIndex = Length.GetInt() + 1;
    sc.TriangularMovingAverage(sc.BaseData[InputData.GetInputDataIndex()], TMA, Length.GetInt());
}

/*==========================================================================*/
SCSFExport scsf_VolumeWeightedMovingAverage(SCStudyInterfaceRef sc)
{
	SCSubgraphRef VWMA = sc.Subgraph[0];

	SCInputRef InputData = sc.Input[0];
	SCInputRef Length = sc.Input[1];

    // Configuration
    if (sc.SetDefaults)
    {
        sc.GraphName = "Moving Average-Volume Weighted"; 
        sc.StudyDescription = "Volume Weighted Moving Average"; 
        sc.GraphRegion = 0;    // zero based region number

		//During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;

        VWMA.Name = "VWMA";
        VWMA.DrawStyle = DRAWSTYLE_LINE;
        VWMA.PrimaryColor = RGB(0,185,47); // green

        InputData.Name = "Input Data";
        InputData.SetInputDataIndex(SC_LAST); 

        Length.Name = "Length";
        Length.SetInt(5);     
        Length.SetIntLimits(1,MAX_STUDY_LENGTH); 

        sc.ValueFormat = 3;

        sc.AutoLoop = 1;

        return;
    }

    // Data processing
 
    sc.DataStartIndex = Length.GetInt();    
    sc.VolumeWeightedMovingAverage(sc.BaseData[InputData.GetInputDataIndex()], sc.Volume, VWMA, Length.GetInt());
}


/*==========================================================================*/
SCSFExport scsf_InsideOrEqualsBar(SCStudyInterfaceRef sc)
{
	SCSubgraphRef IB = sc.Subgraph[0];

	// Set configuration variables

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Inside Or Equals Bar";

		sc.StudyDescription = "";

		//During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;

		sc.GraphRegion = 0;

		IB.Name = "IB";
		IB.DrawStyle = DRAWSTYLE_COLORBAR;
		IB.PrimaryColor = RGB(0, 0, 255);
		IB.DrawZeros = false;

		sc.AutoLoop = 1;

		return;
	}


	// Do data processing

	if (sc.Index < 1)
		return;

	// Array references
	SCFloatArrayRef High = sc.High;
	SCFloatArrayRef Low = sc.Low;

	if (High[sc.Index] <= High[sc.Index - 1] && Low[sc.Index] >= Low[sc.Index - 1])
		IB[sc.Index] = 1;
	else
		IB[sc.Index] = 0;
}

/*==========================================================================*/
SCSFExport scsf_AroonOscillator(SCStudyInterfaceRef sc)
{
	SCSubgraphRef AroonOscillator = sc.Subgraph[0];
	SCSubgraphRef AroonCalculations = sc.Subgraph[1];

	SCInputRef InputDataHigh = sc.Input[0];
	SCInputRef Length = sc.Input[1];
	SCInputRef InputDataLow = sc.Input[2];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Aroon Oscillator";
		sc.AutoLoop = 1;
		sc.GraphRegion = 1;

		AroonOscillator.Name = "Aroon Oscillator";
		AroonOscillator.DrawStyle = DRAWSTYLE_LINE;
		AroonOscillator.PrimaryColor = RGB(0,255,0);

		InputDataHigh.Name = "Input Data High";
		InputDataHigh.SetInputDataIndex(SC_HIGH);

		Length.Name = "Length";
		Length.SetInt(5);           
		Length.SetIntLimits(1,MAX_STUDY_LENGTH);

		InputDataLow.Name = "Input Data Low";
		InputDataLow.SetInputDataIndex(SC_LOW); 


		// During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;

		return;
	}


	// Do data processing


	sc.AroonIndicator(sc.BaseDataIn[InputDataHigh.GetInputDataIndex()], sc.BaseDataIn[InputDataLow.GetInputDataIndex()], AroonCalculations, Length.GetInt());

	AroonOscillator[sc.Index] = AroonCalculations.Arrays[1][sc.Index];
}

/*==========================================================================*/
SCSFExport scsf_AroonIndicator(SCStudyInterfaceRef sc)
{
	SCSubgraphRef AroonUp = sc.Subgraph[0];
	SCSubgraphRef AroonDown = sc.Subgraph[1];


	SCInputRef InputDataHigh = sc.Input[0];
	SCInputRef Length = sc.Input[1];
	SCInputRef InputDataLow = sc.Input[2];


	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Aroon Indicator";
		sc.AutoLoop = 1;
		sc.GraphRegion = 1;	

		AroonUp.Name = "Aroon Up";
		AroonUp.DrawStyle = DRAWSTYLE_LINE;
		AroonUp.PrimaryColor = RGB(0,255,0);
		AroonUp.DrawZeros = true;

		AroonDown.Name = "Aroon Down";
		AroonDown.DrawStyle = DRAWSTYLE_LINE;
		AroonDown.PrimaryColor = RGB(255,0,255);
		AroonDown.DrawZeros = true;

		InputDataHigh.Name = "Input Data High";
		InputDataHigh.SetInputDataIndex(SC_HIGH); // default data field

		Length.Name = "Length";
		Length.SetInt(5);            // default integer value
		Length.SetIntLimits(1,MAX_STUDY_LENGTH); // min-max value

		InputDataLow.Name = "Input Data Low";
		InputDataLow.SetInputDataIndex(SC_LOW); // default data field 

		// During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;

		return;
	}




	sc.AroonIndicator(sc.BaseDataIn[InputDataHigh.GetInputDataIndex()], sc.BaseDataIn[InputDataLow.GetInputDataIndex()], AroonUp, Length.GetInt());

	AroonDown[sc.Index] = AroonUp.Arrays[0][sc.Index];
}

/*============================================================================
Study function for Welles Wilder's Plus and Minus Directional Indicators
+DI and -DI.
----------------------------------------------------------------------------*/
SCSFExport scsf_DMI(SCStudyInterfaceRef sc)
{
	SCSubgraphRef DIplus = sc.Subgraph[0];
	SCSubgraphRef DIminus = sc.Subgraph[1];

	SCInputRef Length = sc.Input[3];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Directional Movement Index";

		sc.StudyDescription = "Welles Wilder's Plus and Minus Directional Indicators +DI and -DI.";

		sc.AutoLoop = 1;  // true

        DIplus.Name = "DI+";
		DIplus.DrawStyle = DRAWSTYLE_LINE;
		DIplus.PrimaryColor = RGB(0,255,0);
		DIplus.DrawZeros = true;

        DIminus.Name = "DI-";
		DIminus.DrawStyle = DRAWSTYLE_LINE;
		DIminus.PrimaryColor = RGB(255,0,255);
		DIminus.DrawZeros = true;

		Length.Name = "Length";
		Length.SetInt(14);
		Length.SetIntLimits(1, INT_MAX);

		//During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;

		return;
	}


	// Do data processing

    sc.DataStartIndex = Length.GetInt();
	
    sc.DMI(
		sc.BaseData,
        DIplus,  // DMI+
        DIminus,  // DMI-
		Length.GetInt());
}

/*============================================================================
----------------------------------------------------------------------------*/
SCSFExport scsf_DMIOscillator(SCStudyInterfaceRef sc)
{
	SCSubgraphRef DMIDiff = sc.Subgraph[0];
	SCSubgraphRef Line = sc.Subgraph[1];

	SCInputRef Length = sc.Input[3];
	SCInputRef ShowAbsValues = sc.Input[4];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Directional Movement Oscillator";

		sc.StudyDescription = "Calculates the difference between the Directional Indicators -DI and the +DI.";

		sc.AutoLoop = 1;  // true

		DMIDiff.Name = "DMI Diff";
		DMIDiff.DrawStyle = DRAWSTYLE_LINE;
		DMIDiff.PrimaryColor = RGB(0,255,0);
		DMIDiff.DrawZeros = true;

		Line.Name = "Line";
		Line.DrawStyle = DRAWSTYLE_LINE;
		Line.PrimaryColor = RGB(255,0,255);
		Line.DrawZeros = true;

		Length.Name = "Length";
		Length.SetInt(14);
		Length.SetIntLimits(1, INT_MAX);

		ShowAbsValues.Name = "Show Absolute Values";
		ShowAbsValues.SetYesNo(0);

		//During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;

		return;
	}

	// Do data processing

	sc.DataStartIndex = Length.GetInt() - 1;

	sc.DMIDiff(sc.BaseData, DMIDiff, sc.Index, Length.GetInt());  // DMI Diff

	if(ShowAbsValues.GetYesNo())
		DMIDiff[sc.Index] = fabs(DMIDiff[sc.Index]);
}

/*============================================================================
Average Directional Movement Index study.  This is calculated according
to the Welles Wilder formulas.
----------------------------------------------------------------------------*/
SCSFExport scsf_ADX(SCStudyInterfaceRef sc)
{
	SCSubgraphRef ADX = sc.Subgraph[0];

	SCInputRef DXLength = sc.Input[3];
	SCInputRef DXMALength = sc.Input[4];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "ADX";

		sc.StudyDescription = "Average Directional Movement Index.  This is calculated according to the Welles Wilder formulas.";

		sc.AutoLoop = 1;  // true
		sc.GraphRegion = 1;

		ADX.Name = "ADX";
		ADX.DrawStyle = DRAWSTYLE_LINE;
		ADX.PrimaryColor = RGB(0,255,0);
		ADX.DrawZeros = true;

		DXLength.Name = "DX Length";
		DXLength.SetInt(14);
		DXLength.SetIntLimits(1, INT_MAX);

		DXMALength.Name = "DX Mov Avg Length";
		DXMALength.SetInt(14);
		DXMALength.SetIntLimits(1, INT_MAX);

		//During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;

		return;
	}


	// Do data processing

	sc.DataStartIndex = DXLength.GetInt() + DXMALength.GetInt() - 1;

    sc.ADX(
        sc.BaseData,
        ADX,
        DXLength.GetInt(),
        DXMALength.GetInt());
}

/*============================================================================
Average Directional Movement Index Rating study.  This is calculated
according to the Welles Wilder formulas.
----------------------------------------------------------------------------*/
SCSFExport scsf_ADXR(SCStudyInterfaceRef sc)
{
	SCSubgraphRef ADXR = sc.Subgraph[0];

	SCInputRef DXLength = sc.Input[3];
	SCInputRef DXMALength = sc.Input[4];
	SCInputRef ADXRInterval = sc.Input[5];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "ADXR";

		sc.StudyDescription = "Average Directional Movement Index Rating.  This is calculated according to the Welles Wilder formulas.";

		sc.AutoLoop = 1;  // true
		sc.GraphRegion = 1;

		ADXR.Name = "ADXR";
		ADXR.DrawStyle = DRAWSTYLE_LINE;
		ADXR.PrimaryColor = RGB(0,255,0);
		ADXR.DrawZeros = true;

		DXLength.Name = "DX Length";
		DXLength.SetInt(14);
		DXLength.SetIntLimits(1, INT_MAX);

		DXMALength.Name = "DX Mov Avg Length";
		DXMALength.SetInt(14);
		DXMALength.SetIntLimits(1, INT_MAX);

		ADXRInterval.Name = "ADXR Interval";
		ADXRInterval.SetInt(14);
		ADXRInterval.SetIntLimits(1, INT_MAX);

		//During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;

		return;
	}


	// Do data processing

	sc.DataStartIndex = DXLength.GetInt() + DXMALength.GetInt() + ADXRInterval.GetInt() - 2;

	sc.ADXR(
		sc.BaseData,
        ADXR,
		DXLength.GetInt(),
		DXMALength.GetInt(),
		ADXRInterval.GetInt());
}


/*============================================================================
This example code calculates a smoothed moving average (5 period by
default).
----------------------------------------------------------------------------*/
SCSFExport scsf_SMMA(SCStudyInterfaceRef sc)
{
	SCSubgraphRef SMMA = sc.Subgraph[0];

	SCInputRef InputData = sc.Input[0];
	SCInputRef Length = sc.Input[1];
	SCInputRef Offset = sc.Input[2];

	// Set configuration variables	
	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Moving Average-Smoothed";

		sc.StudyDescription = "Function for calculating a smooth moving average SMMA.";

		// Set the region to draw the graph in.  Region zero is the main
		// price graph region.
		sc.GraphRegion = 0;


		// Set the name of the first subgraph
		SMMA.Name = "SMMA";
		// Set the color and style of the graph line.  If these are not set the default will be used.
		SMMA.PrimaryColor = RGB(255,0,0);  // Red
		SMMA.DrawStyle = DRAWSTYLE_LINE;
		SMMA.DrawZeros = false;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST); // default data field		

		// Make the Length input and default it to 5
		Length.Name = "Length";
		Length.SetInt(5);
		Length.SetIntLimits(1,MAX_STUDY_LENGTH);

		//  Offset / displacement
		Offset.Name = "Offset";
		Offset.SetInt(0);
		Offset.SetIntLimits(0,MAX_STUDY_LENGTH);

		sc.AutoLoop = 1;

		//During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;

		// Must return before doing any data processing if sc.SetDefaults is set
		return;
	}

	// Do data processing		

	sc.SmoothedMovingAverage(sc.BaseDataIn[InputData.GetInputDataIndex()], SMMA, sc.Index, Length.GetInt(), Offset.GetInt());
}

/*****************************************************/
SCSFExport scsf_PPO(SCStudyInterfaceRef sc)
{
	SCSubgraphRef PPO = sc.Subgraph[0];
	SCSubgraphRef ShortMA = sc.Subgraph[1];
	SCSubgraphRef LongMA = sc.Subgraph[2];

	SCInputRef LongMALength = sc.Input[0];
	SCInputRef ShortMALength = sc.Input[1];
	SCInputRef MAType = sc.Input[2];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Percentage Price Oscillator";

		PPO.Name = "PPO";
		PPO.DrawStyle = DRAWSTYLE_LINE;
		PPO.PrimaryColor = RGB(0,255,0);
		PPO.DrawZeros = true;

		LongMALength.Name = "Long Mov Avg Length";
		LongMALength.SetInt(30);
		LongMALength.SetIntLimits(1, INT_MAX);

		ShortMALength.Name = "Short Mov Avg Length";
		ShortMALength.SetInt(10);
		ShortMALength.SetIntLimits(1, INT_MAX);

		MAType.Name = "Moving Average Type";
		MAType.SetMovAvgType(MOVAVGTYPE_EXPONENTIAL);

		sc.AutoLoop = 1;

		//During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;

		return;
	}

	sc.MovingAverage(sc.Close,LongMA,MAType.GetMovAvgType(),LongMALength.GetInt());
	sc.MovingAverage(sc.Close,ShortMA,MAType.GetMovAvgType(),ShortMALength.GetInt());
	PPO[sc.Index] = ((ShortMA[sc.Index]-LongMA[sc.Index])/LongMA[sc.Index]) * 100;
}
/************************************************************************/


SCSFExport scsf_HerrickPayoffIndex(SCStudyInterfaceRef sc)
{
	SCSubgraphRef HPI = sc.Subgraph[0];
	SCSubgraphRef Zero = sc.Subgraph[1];

	SCInputRef ValueOfMove = sc.Input[0];
	SCInputRef SmoothingMult = sc.Input[1];
	SCInputRef MaximumOrMinimum = sc.Input[2];
	SCInputRef Divisor = sc.Input[3];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Herrick Payoff Index";

		HPI.Name = "HPI";
		HPI.PrimaryColor = RGB(0, 255, 0);  // Red
		HPI.DrawStyle = DRAWSTYLE_LINE;
		HPI.DrawZeros = true;

		Zero.Name = "Zero";
		Zero.PrimaryColor = RGB(255, 255, 0);  // Red
		Zero.DrawStyle = DRAWSTYLE_LINE;
		Zero.DrawZeros = true;

		ValueOfMove.Name = "Value of a .01 Move";
		ValueOfMove.SetFloat(0.5f);
		
		SmoothingMult.Name = "Smoothing multiplier";
		SmoothingMult.SetFloat(0.1f);
		
		MaximumOrMinimum.Name = "Maximum or Minimum Open Interest: 1= maximum, 2= minimum";
		MaximumOrMinimum.SetInt(1);
		MaximumOrMinimum.SetIntLimits(1, 2);

		Divisor.Name = "Divisor";
		Divisor.SetFloat(100000.0f);
		

		sc.AutoLoop = 1;

		//During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;

		return;
	}

	if (sc.Index == 0)
		return;
	
		float CurrentVolume = sc.Volume[sc.Index];

		// current and previous mean price (high + low) / 2
		float AveragePrice = sc.HLAvg[sc.Index];	
		float PriorAveragePrice = sc.HLAvg[sc.Index - 1];   

		
		float CurrentOpenInterest = sc.OpenInterest[sc.Index];	
		float PriorOpenInterest = sc.OpenInterest[sc.Index-1];	

		if (CurrentOpenInterest<=0.0f)
		{
			CurrentOpenInterest = PriorOpenInterest;
		}

		// Determine sign for calculations
		float sign = ( AveragePrice > PriorAveragePrice ) ? 1.0f : -1.0f;

		// the absolute value of the difference between current open interest and
		// prior open interest
		float PositiveOpenInterest = fabs(CurrentOpenInterest - PriorOpenInterest);

		
		float MaximumOrMinimumOpenInterest = (MaximumOrMinimum.GetInt()==1) ? max(CurrentOpenInterest, PriorOpenInterest) : min(CurrentOpenInterest, PriorOpenInterest);

		float PriorHerrickPayoffIndex = HPI[sc.Index - 1];

		if (MaximumOrMinimumOpenInterest>0.0f)
		{
			float CalculationResult = ValueOfMove.GetFloat() * CurrentVolume * (AveragePrice - PriorAveragePrice) * (1 + sign*2*PositiveOpenInterest/MaximumOrMinimumOpenInterest)/Divisor.GetFloat();

			if(sc.Index>1)
			{	
				HPI[sc.Index] = PriorHerrickPayoffIndex + (CalculationResult-PriorHerrickPayoffIndex)*SmoothingMult.GetFloat();
			}
			else
			{
				HPI[sc.Index] = CalculationResult;
			}
		}
		else
		{
			HPI[sc.Index] = PriorHerrickPayoffIndex;
		}
	
	
}

/************************************************************************/
SCSFExport scsf_TRIX(SCStudyInterfaceRef sc)
{	
	SCSubgraphRef TRIX = sc.Subgraph[0];

	SCInputRef InputData = sc.Input[0];
	SCInputRef Length = sc.Input[2];

	if (sc.SetDefaults)
	{
		sc.GraphName = "TRIX";

		TRIX.Name = "TRIX";
		TRIX.DrawStyle = DRAWSTYLE_BAR;
		TRIX.PrimaryColor = RGB(0,255,0);

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST); // default data field		

		// Make the Length input and default it to 3
		Length.Name = "Length";
		Length.SetInt(3);
		Length.SetIntLimits(1,MAX_STUDY_LENGTH);

		sc.AutoLoop = 1;

		//During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;

		return;
	}

	// Data start and update start
	sc.DataStartIndex = (Length.GetInt() - 1) * 3 + 1;
	
	sc.TRIX(sc.BaseData[InputData.GetInputDataIndex()], TRIX, Length.GetInt());
}

/************************************************************
* Method			- Momentum Gauging
* Description	- 
* Price = Close
* Length = 20
* 
* Histo = LinearRegression[Length] (price - ((Highest[Length](High) + Lowest[Length](Low))/2 + ExponentialAverage[Length](close))/2 )
* 
* RETURN Histo as "Histogramme"[/color][/color]
*
************************************************************/
SCSFExport scsf_MomentumGauging(SCStudyInterfaceRef sc)
{
	const int INDEX_EMA = 1;
	const int INDEX_LREG_SRC = 2;

	SCSubgraphRef Momentum = sc.Subgraph[0];
	SCSubgraphRef EMA = sc.Subgraph[1];
	SCSubgraphRef LREG_SRC = sc.Subgraph[2];

	SCInputRef InputData = sc.Input[0];
	SCInputRef Length = sc.Input[2];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Momentum Gauging";

		Momentum.Name = "Momentum";
		Momentum.DrawStyle = DRAWSTYLE_BAR;
		Momentum.PrimaryColor = RGB(0,255,0);
		//Momentum.SecondaryColorUsed = 1;
		Momentum.SecondaryColor = RGB(255,0,0);
		

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST); // default data field		

		// Make the Length input and default it to 3
		Length.Name = "Length";
		Length.SetInt(3);
		Length.SetIntLimits(1,MAX_STUDY_LENGTH);

		sc.AutoLoop = 1;

		//During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;

		return;
	}

	// First output elements are not valid
	sc.DataStartIndex = Length.GetInt();

	sc.ExponentialMovAvg(sc.Close, EMA, sc.Index, Length.GetInt());  // Note: EMA returns close when index is < Length.GetInt()

	float hlh = sc.GetHighest(sc.High, sc.Index, Length.GetInt());
	float lll = sc.GetLowest(sc.Low, sc.Index, Length.GetInt());

	SCFloatArrayRef price = sc.BaseData[InputData.GetInputDataIndex()];

	// populate the source data for linear regression
	LREG_SRC[sc.Index] = price[sc.Index] - ((hlh + lll)/2.0f + EMA[sc.Index])/2.0f;

	sc.LinearRegressionIndicator(LREG_SRC, Momentum, sc.Index, Length.GetInt());

	/*if (Momentum[sc.Index] >= 0)
		Momentum.DataColor[sc.Index] = Momentum.PrimaryColor;
	else
		Momentum.DataColor[sc.Index] = Momentum.SecondaryColor ;*/

}

/************************************************************

function main(nInputLength) {
var vC = close(0);
var vPH = high(-1);
var vPL = low(-1);

if(vC == null || vPH == null || vPL == null) return;

if (vC > vPH) {
setPriceBarColor(Color.lime);
} else if(vC < vPL) {
setPriceBarColor(Color.red);
}

return;	
}
************************************************************/
SCSFExport scsf_HHLLCandles(SCStudyInterfaceRef sc)
{
	SCSubgraphRef HH = sc.Subgraph[0];
	SCSubgraphRef LL = sc.Subgraph[1];
	SCSubgraphRef N = sc.Subgraph[2];

	if (sc.SetDefaults)
	{
		sc.GraphName = "HH LL Bars";
		sc.StudyDescription = "";

		sc.GraphRegion = 0;

		HH.Name = "HH";
		HH.DrawStyle = DRAWSTYLE_COLORBAR_CANDLEFILL;
		HH.PrimaryColor = RGB(0, 255, 0);
		HH.SecondaryColor = RGB(0, 0, 0);
		HH.SecondaryColorUsed = 1;
		HH.DrawZeros = false;

		LL.Name = "LL";
		LL.DrawStyle = DRAWSTYLE_COLORBAR_CANDLEFILL;
		LL.PrimaryColor = RGB(255, 0, 0);
		LL.SecondaryColor = RGB(0, 0, 0);
		LL.SecondaryColorUsed = 1;
		LL.DrawZeros = false;

		N.Name = "N";
		N.DrawStyle = DRAWSTYLE_COLORBAR;
		N.PrimaryColor = RGB(50, 50, 190);
		N.DrawZeros = false;

		sc.AutoLoop = 1;
		//During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;

		return;
	}
	
	sc.ValueFormat = sc.BaseGraphValueFormat;
	
	sc.DataStartIndex = 1;
	if (sc.Index<1)
		return;

	float priorHigh = sc.High[sc.Index - 1];
	float priorLow = sc.Low[sc.Index - 1];

	float currentClose = sc.Close[sc.Index];

	HH[sc.Index] = 0;
	LL[sc.Index] = 0;
	N[sc.Index] = 0;

	if (sc.FormattedEvaluate(currentClose, sc.BaseGraphValueFormat, GREATER_OPERATOR, priorHigh, sc.BaseGraphValueFormat))
	{
		HH[sc.Index] = 1;
		//We need to set these to 0 because conditions can change on the last bar during real-time updating.
		N[sc.Index] = 0;
		LL[sc.Index] = 0;
	}
	else if (sc.FormattedEvaluate(currentClose, sc.BaseGraphValueFormat, LESS_OPERATOR, priorLow, sc.BaseGraphValueFormat))
	{
		LL[sc.Index] = 1;
		N[sc.Index] = 0;
		HH[sc.Index] = 0;
	}
	else
	{
		N[sc.Index] = 1;
		HH[sc.Index] = 0;
		LL[sc.Index] = 0;
	}

}

/***********************************************************************/

SCSFExport scsf_RMO(SCStudyInterfaceRef sc)
{
	SCSubgraphRef ST1 = sc.Subgraph[0];
	SCSubgraphRef ST2 = sc.Subgraph[1];
	SCSubgraphRef ST3 = sc.Subgraph[2];
	SCFloatArrayRef MATemp3 = sc.Subgraph[0].Arrays[0];
	SCFloatArrayRef MATemp4 = sc.Subgraph[0].Arrays[1];
	SCFloatArrayRef MATemp5 = sc.Subgraph[0].Arrays[2];
	SCFloatArrayRef MATemp6 = sc.Subgraph[0].Arrays[3];
	SCFloatArrayRef MATemp7 = sc.Subgraph[0].Arrays[4];
	SCFloatArrayRef MATemp8 = sc.Subgraph[0].Arrays[5];
	SCFloatArrayRef MATemp9 = sc.Subgraph[0].Arrays[6];
	SCFloatArrayRef MATemp10 = sc.Subgraph[0].Arrays[7];
	SCFloatArrayRef MATemp11 = sc.Subgraph[0].Arrays[8];
	SCFloatArrayRef MATemp12 = sc.Subgraph[0].Arrays[9];
	SCFloatArrayRef Highest = sc.Subgraph[0].Arrays[10];
	SCFloatArrayRef Lowest = sc.Subgraph[0].Arrays[11];

	SCInputRef Len1 = sc.Input[2];
	SCInputRef Len2 = sc.Input[3];
	SCInputRef Len3 = sc.Input[4];
	SCInputRef Len4 = sc.Input[5];

	if (sc.SetDefaults)
	{
		sc.GraphName="Rahul Mohindar Oscillator";

		ST1.Name="ST1";
		ST1.DrawStyle= DRAWSTYLE_IGNORE;
		ST1.PrimaryColor = RGB(0,255,0);

		ST2.Name="ST2";
		ST2.DrawStyle = DRAWSTYLE_LINE;
		ST2.PrimaryColor = RGB(255,0,255);

		ST3.Name="ST3";
		ST3.DrawStyle = DRAWSTYLE_LINE;
		ST3.PrimaryColor = RGB(255,255,0);
		
		Len1.Name="Length 1";
		Len1.SetInt(2);
		
		Len2.Name="Length 2";
		Len2.SetInt(10);
		
		Len3.Name="Length 3";
		Len3.SetInt(30);

		Len4.Name="Length 4";
		Len4.SetInt(81);
		
		//During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;

		sc.AutoLoop = 1;

		return;
	}

	sc.SimpleMovAvg(sc.Close,MATemp3,Len1.GetInt());
	sc.SimpleMovAvg(MATemp3,MATemp4,Len1.GetInt());
	sc.SimpleMovAvg(MATemp4,MATemp5,Len1.GetInt());
	sc.SimpleMovAvg(MATemp5,MATemp6,Len1.GetInt());
	sc.SimpleMovAvg(MATemp6,MATemp7,Len1.GetInt());
	sc.SimpleMovAvg(MATemp7,MATemp8,Len1.GetInt());
	sc.SimpleMovAvg(MATemp8,MATemp9,Len1.GetInt());
	sc.SimpleMovAvg(MATemp9,MATemp10,Len1.GetInt());
	sc.SimpleMovAvg(MATemp10,MATemp11,Len1.GetInt());
	sc.SimpleMovAvg(MATemp11,MATemp12,Len1.GetInt());
	sc.Highest(sc.Close, Highest,  Len2.GetInt());
	sc.Lowest(sc.Close, Lowest,  Len2.GetInt());

	float  Range = Highest[sc.Index]-Lowest[sc.Index];

	if ( Range != 0)
	{
		ST1[sc.Index]=
		100* (sc.Close[sc.Index]
		- (MATemp3[sc.Index]
		+MATemp4[sc.Index]
		+MATemp5[sc.Index]
		+MATemp6[sc.Index]
		+MATemp7[sc.Index]
		+MATemp8[sc.Index]
		+MATemp9[sc.Index]+MATemp10[sc.Index]
		+MATemp11[sc.Index]
		+MATemp12[sc.Index])/10) /  Range;
	}


	sc.ExponentialMovAvg(ST1,ST2,Len3.GetInt());
	sc.ExponentialMovAvg(ST2,ST3,Len4.GetInt());
}



/* ****************************************************************
*
* Calculation:
* The value of the DeMarker for the "i" interval is calculated as follows:
*	The DeMax(i) is calculated: 
*
* If high(i) > high(i-1) , then DeMax(i) = high(i)-high(i-1), otherwise DeMax(i) = 0
*	The DeMin(i) is calculated: 
*
* If low(i) < low(i-1), then DeMin(i) = low(i-1)-low(i), otherwise DeMin(i) = 0
*	The value of the DeMarker is calculated as: 
*
* DMark(i) = SMA(DeMax, N)/(SMA(DeMax, N)+SMA(DeMin, N))
*
******************************************************************/
SCSFExport scsf_Demarker(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Demarker = sc.Subgraph[0];
	SCSubgraphRef Demax = sc.Subgraph[1];
	SCSubgraphRef SMA_Demax = sc.Subgraph[2];
	SCSubgraphRef Demin = sc.Subgraph[3];
	SCSubgraphRef SMA_Demin = sc.Subgraph[4];

	if (sc.SetDefaults)
	{
		sc.GraphName="Demarker";
		sc.StudyDescription="Demarker";

		Demarker.Name="Demarker";
		Demarker.DrawStyle=DRAWSTYLE_LINE;
		Demarker.PrimaryColor = RGB(0,255,0);

		sc.Input[0].Name="Length";
		sc.Input[0].SetInt(13);
		sc.Input[0].SetIntLimits(1,MAX_STUDY_LENGTH);

		sc.AutoLoop = 1;

		//During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;

		return;
	}

	int length = sc.Input[0].GetInt();

	sc.DataStartIndex = length;

	if (sc.Index > 0)
	{
		// If high(i) > high(i-1) , then DeMax(i) = high(i)-high(i-1), otherwise DeMax(i) = 0

		float high = sc.High[sc.Index];
		float highOld = sc.High[sc.Index-1];

		if ( high > highOld )
		{
			Demax[sc.Index] = high - highOld;
		}
		else
		{
			Demax[sc.Index] = 0.0f;
		}

		// If low(i) < low(i-1), then DeMin(i) = low(i-1)-low(i), otherwise DeMin(i) = 0

		float low = sc.Low[sc.Index];
		float lowOld = sc.Low[sc.Index-1];

		if ( low < lowOld )
		{
			Demin[sc.Index] = lowOld - low;
		}
		else
		{
			Demin[sc.Index] = 0.0f;
		}
	}
	else
	{
		Demax[sc.Index] = 0.0f;
		Demin[sc.Index] = 0.0f;
	}

	// DMark(i) = SMA(DeMax, N)/(SMA(DeMax, N)+SMA(DeMin, N))

	sc.SimpleMovAvg(Demax, SMA_Demax, length);
	sc.SimpleMovAvg(Demin, SMA_Demin,  length);

	float summ = SMA_Demax[sc.Index] + SMA_Demin[sc.Index];
	if (summ != 0.0f)
	{        
		Demarker[sc.Index] = SMA_Demax[sc.Index] / summ;
	}
	else
	{
		Demarker[sc.Index] = Demarker[sc.Index-1];
	}
}


/***********************************************************************/
SCSFExport scsf_StochasticRSI(SCStudyInterfaceRef sc)
{
 
	SCSubgraphRef StochasticRSI = sc.Subgraph[0];
	SCSubgraphRef RSI = sc.Subgraph[1];

	SCInputRef RSILength = sc.Input[0];
	SCInputRef RSIAvgType = sc.Input[1];

	if(sc.SetDefaults)
	{
		sc.GraphName="Stochastic RSI";
		sc.StudyDescription="Stochastic RSI";

		StochasticRSI.Name="Stochastic RSI";
		StochasticRSI.DrawStyle = DRAWSTYLE_LINE;
		StochasticRSI.PrimaryColor = RGB(0,255,0);
		StochasticRSI.DrawZeros = true;

		RSILength.Name="RSI Length";
		RSILength.SetInt(14);

		RSIAvgType.Name="RSI Average Type";
		RSIAvgType.SetMovAvgType(MOVAVGTYPE_SIMPLE);
		
		sc.AutoLoop = 1;

		//During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;

		return;
	}

	sc.DataStartIndex = RSILength.GetInt(); 

	if (sc.Index<1)
	{
		return;
	}

	sc.RSI(sc.Close, RSI, RSIAvgType.GetMovAvgType(), RSILength.GetInt());

	// computing Stochastic of RSI
	float MaximumRSI = sc.GetHighest(RSI, sc.Index, RSILength.GetInt());
	float MinimumRSI = sc.GetLowest(RSI, sc.Index, RSILength.GetInt());

	if (MaximumRSI-MinimumRSI != 0.0f)
	{
		StochasticRSI[sc.Index] = (RSI[sc.Index]-MinimumRSI)/(MaximumRSI - MinimumRSI);
	}
	else
	{
		StochasticRSI[sc.Index] = StochasticRSI[sc.Index-1];
	}
}
/***********************************************************************/
SCSFExport scsf_MinerDTOscillator(SCStudyInterfaceRef sc)
{
	SCSubgraphRef SK        = sc.Subgraph[0];
	SCSubgraphRef SD        = sc.Subgraph[1];
	SCSubgraphRef UpperLine = sc.Subgraph[2];
	SCSubgraphRef ZeroLine  = sc.Subgraph[3];
	SCSubgraphRef LowerLine = sc.Subgraph[4];

	SCFloatArrayRef RSI                 = SK.Arrays[0];
	SCFloatArrayRef RsiUpSums           = SK.Arrays[1];
	SCFloatArrayRef RsiDownSums         = SK.Arrays[2];
	SCFloatArrayRef RsiSmoothedUpSums   = SK.Arrays[3];
	SCFloatArrayRef RsiSmoothedDownSums = SK.Arrays[4];
	SCFloatArrayRef StochRSI            = SK.Arrays[5];

	SCInputRef RSILength   = sc.Input[0];
	SCInputRef RSIAvgType  = sc.Input[1];
	SCInputRef StochLength = sc.Input[2];
	SCInputRef SKLength    = sc.Input[3];
	SCInputRef SKAvgType   = sc.Input[4];
	SCInputRef SDLength    = sc.Input[5];
	SCInputRef SDAvgType   = sc.Input[6];
	SCInputRef UpperValue  = sc.Input[7];
	SCInputRef LowerValue  = sc.Input[8];

	if(sc.SetDefaults)
	{
		sc.GraphName="DT Oscillator";
		sc.StudyDescription="DT Oscillator - Robert Miner"; 

		SK.Name="DT Osc SK";
		SK.DrawStyle = DRAWSTYLE_LINE;
		SK.PrimaryColor = COLOR_RED;
		SK.DrawZeros = true;

		SD.Name="DT Osc SD";
		SD.DrawStyle = DRAWSTYLE_LINE;
		SD.PrimaryColor = COLOR_GREEN;
		SD.DrawZeros = true;

		UpperLine.Name="Upper";
		UpperLine.DrawStyle = DRAWSTYLE_LINE;
		UpperLine.LineStyle = LINESTYLE_DOT;
		UpperLine.PrimaryColor = COLOR_GRAY;
		UpperLine.DisplayNameValueInWindowsFlags = 0;
		UpperLine.DrawZeros = true;

		ZeroLine.Name="Zero";
		ZeroLine.DrawStyle = DRAWSTYLE_IGNORE;
		ZeroLine.LineStyle = LINESTYLE_DOT;
		ZeroLine.PrimaryColor = COLOR_GRAY;
		ZeroLine.DisplayNameValueInWindowsFlags = 0;
		ZeroLine.DrawZeros = true;

		LowerLine.Name="Lower";
		LowerLine.DrawStyle = DRAWSTYLE_LINE;
		LowerLine.LineStyle = LINESTYLE_DOT;
		LowerLine.PrimaryColor = COLOR_GRAY;
		LowerLine.DisplayNameValueInWindowsFlags = 0;
		LowerLine.DrawZeros = true;

		RSILength.Name="RSI Length";
		RSILength.SetInt(13);

		RSIAvgType.Name="RSI Average Type";
		RSIAvgType.SetMovAvgType(MOVAVGTYPE_SIMPLE);

		StochLength.Name="Stochastic Length";
		StochLength.SetInt(8);

		SKLength.Name="SK Length";
		SKLength.SetInt(5);

		SKAvgType.Name="SK Average Type";
		SKAvgType.SetMovAvgType(MOVAVGTYPE_SIMPLE);

		SDLength.Name="SD Length";
		SDLength.SetInt(3);

		SDAvgType.Name="SD Average Type";
		SDAvgType.SetMovAvgType(MOVAVGTYPE_SIMPLE);

		UpperValue.Name="Upper Line Value";
		UpperValue.SetFloat(75.0f);

		LowerValue.Name="Lower Line Value";
		LowerValue.SetFloat(25.0f);

		sc.AutoLoop = 1;
		sc.FreeDLL = 0;
		

		return;
	}

	sc.DataStartIndex = RSILength.GetInt(); 

	UpperLine[sc.Index] = UpperValue.GetFloat();
	ZeroLine[sc.Index]  = 0;
	LowerLine[sc.Index] = LowerValue.GetFloat();

	if (sc.Index<1)
	{
		return;
	}

	sc.RSI(sc.Close, RSI, RsiUpSums, RsiDownSums, RsiSmoothedUpSums, RsiSmoothedDownSums, RSIAvgType.GetMovAvgType(), RSILength.GetInt());

	// computing Stochastic of RSI
	float maxRSI = sc.GetHighest(RSI, sc.Index, StochLength.GetInt());
	float minRSI = sc.GetLowest(RSI, sc.Index, StochLength.GetInt());

	if (maxRSI-minRSI != 0.0f)
	{
		StochRSI[sc.Index] = 100.0f * (RSI[sc.Index]-minRSI)/(maxRSI - minRSI);
	}
	else
	{
		StochRSI[sc.Index] = StochRSI[sc.Index-1];
	}

	sc.MovingAverage(StochRSI, SK, SKAvgType.GetMovAvgType(), SKLength.GetInt());
	sc.MovingAverage(SK,       SD, SDAvgType.GetMovAvgType(), SDLength.GetInt());
}

/***********************************************************************/
SCSFExport scsf_VerticalTimeLine(SCStudyInterfaceRef sc)
{
	SCSubgraphRef TimeLine = sc.Subgraph[0];
	SCSubgraphRef TimeLineBottom = sc.Subgraph[1];
	SCSubgraphRef CustomLabel = sc.Subgraph[2];

	SCInputRef TimeOfLine = sc.Input[0];
	SCInputRef DisplayCustomLabel = sc.Input[1];
	SCInputRef VerticalPositionPercent = sc.Input[2];
	SCInputRef Version = sc.Input[3];
	SCInputRef ForwardProject = sc.Input[4];

	if(sc.SetDefaults)
	{
		sc.GraphName="Time Line";
		sc.StudyDescription="Draws a vertical line at the time specified with the Time of Line Input.";

		sc.AutoLoop = 0;//Manual looping
		sc.GraphRegion = 0;
		sc.ScaleRangeType = SCALE_USERDEFINED;
		sc.ScaleRangeTop = 2;
		sc.ScaleRangeBottom = 1;

		TimeOfLine.Name = "Time of Line";
		TimeOfLine.SetTime(HMS_TIME(12, 00, 0));

		DisplayCustomLabel.Name = "Display Custom Label";
		DisplayCustomLabel.SetYesNo(false);

		VerticalPositionPercent.Name = "Label Vertical Position %.";
		VerticalPositionPercent.SetInt(5);
		VerticalPositionPercent.SetIntLimits(0,100);

		sc.TextInputName = "Custom Label Text";

		Version.SetInt(2);

		ForwardProject.Name = "Display in Forward Projection Area";
		ForwardProject.SetYesNo(false);

		TimeLine.Name = "TimeLine";
		TimeLine.DrawStyle = DRAWSTYLE_BARTOP;
		TimeLine.PrimaryColor = RGB(127,127,127);
		TimeLine.DrawZeros = false;
		TimeLine.LineWidth = 1;

		TimeLineBottom.Name = "TimeLine";
		TimeLineBottom.DrawStyle = DRAWSTYLE_BARBOTTOM;
		TimeLineBottom.PrimaryColor = RGB(127,127,127);
		TimeLineBottom.DrawZeros =  false;
		TimeLineBottom.LineWidth = 1;

		CustomLabel.Name = "Custom Label Color and Text Size";
		CustomLabel.DrawStyle=DRAWSTYLE_CUSTOM_TEXT;
		CustomLabel.PrimaryColor = RGB(127,127,127);
		CustomLabel.LineWidth =  0;


		sc.FreeDLL = 0;


		return;
	}

	if (Version.GetInt()<2)
	{
		TimeLine.DrawStyle = DRAWSTYLE_BARTOP;
		TimeLineBottom.DrawStyle = DRAWSTYLE_BARBOTTOM;
		Version.SetInt(2);
	}

	int NumberForwardBars = 0;
	if(ForwardProject.GetYesNo())
	{
		NumberForwardBars = 200;
		TimeLine.ExtendedArrayElementsToGraph = NumberForwardBars;
		TimeLineBottom.ExtendedArrayElementsToGraph = NumberForwardBars;
	}
	
	for (int Index = sc.UpdateStartIndex; Index < (sc.ArraySize + NumberForwardBars); Index++)
	{

		SCDateTime TimeLineDateTime(sc.BaseDateTimeIn[Index].GetDate(),TimeOfLine.GetTime() );

		if (sc.BaseDateTimeIn[Index-1] < TimeLineDateTime && sc.BaseDateTimeIn[Index] >= TimeLineDateTime	)
		{
			TimeLine[Index] = 100;
			TimeLineBottom[Index] = -100;
		}
		else
		{
			TimeLine[Index] = 0;
			TimeLineBottom[Index] = 0;
		}

		if (TimeLine[Index] == 0)
			continue;


		if(DisplayCustomLabel.GetYesNo() && !sc.HideStudy )
		{

			int &LastUsedIndex = sc.PersistVars->i1;

			if(Index == 0)
				LastUsedIndex = -1;

			if(Index == LastUsedIndex)
				continue;

			LastUsedIndex = Index;

			s_UseTool Tool;
			Tool.ChartNumber = sc.ChartNumber;
			Tool.DrawingType = DRAWING_TEXT;
			Tool.Region = sc.GraphRegion;
			//Tool.LineNumber //Automatically set
			Tool.AddMethod = UTAM_ADD_OR_ADJUST;

			Tool.BeginIndex = Index;
			Tool.UseRelativeValue = true;
			Tool.BeginValue = (float)VerticalPositionPercent.GetInt();
			Tool.Text = sc.TextInput;

			Tool.Color = CustomLabel.PrimaryColor;
			Tool.FontBold = true;
			Tool.FontSize = CustomLabel.LineWidth;
			sc.UseTool(Tool);

		}
	}

	return;
}

/***********************************************************************/
SCSFExport scsf_RSITS(SCStudyInterfaceRef sc)
{
	SCSubgraphRef RSIT = sc.Subgraph[0];
	SCSubgraphRef LineUpperLimit = sc.Subgraph[1];
	SCSubgraphRef LineLowerLimit = sc.Subgraph[2];
	SCSubgraphRef RSIAvg = sc.Subgraph[3];
	SCSubgraphRef NetChangeAvg = sc.Subgraph[4];
	SCSubgraphRef TotalChangeAvg = sc.Subgraph[5];

	SCInputRef InputData = sc.Input[0];
	SCInputRef RSILength = sc.Input[1];
	SCInputRef Line1 = sc.Input[3];
	SCInputRef Line2 = sc.Input[4];
	SCInputRef RSIMovAvgLength = sc.Input[5];

	if(sc.SetDefaults)
	{
		sc.GraphName="RSI-TS";
		sc.StudyDescription="RSI implemented as in TradeStation";

		// Outputs
		RSIT.Name="RSI-T";
		RSIT.DrawStyle = DRAWSTYLE_LINE;
		RSIT.PrimaryColor = RGB(0,255,0);
		RSIT.DrawZeros = false;

		LineUpperLimit.Name = "Line1";
		LineUpperLimit.DrawStyle = DRAWSTYLE_LINE;
		LineUpperLimit.PrimaryColor = RGB(255,0,255);
		LineUpperLimit.DrawZeros = false;

		LineLowerLimit.Name = "Line2";
		LineLowerLimit.DrawStyle = DRAWSTYLE_LINE;
		LineLowerLimit.PrimaryColor = RGB(255,255,0);
		LineLowerLimit.DrawZeros = false;

		RSIAvg.Name = "RSI Avg";
		RSIAvg.DrawStyle = DRAWSTYLE_LINE;
		RSIAvg.PrimaryColor = RGB(255,127,0);
		RSIAvg.DrawZeros = false;

		// Inputs
		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		RSILength.Name="RSI Length";
		RSILength.SetInt(10);

		Line1.Name="Line 1";
		Line1.SetFloat(70);
		Line1.SetFloatLimits(50, 100);

		Line2.Name="Line 2";
		Line2.SetFloat(30);
		Line2.SetFloatLimits(0, 50);

		RSIMovAvgLength.Name = "RSI Mov Avg Length";
		RSIMovAvgLength.SetInt(3);
		RSIMovAvgLength.SetIntLimits(1, MAX_STUDY_LENGTH);

		sc.AutoLoop = 1;

		//During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;

		return;
	}

	// start charting after Length of elements is available
	sc.DataStartIndex = RSILength.GetInt(); 
	if (sc.Index<RSILength.GetInt())
		return;

	SCFloatArrayRef  price = sc.BaseData[InputData.GetInputDataIndex()];

	// smoothing factor
	float smoothingFactor = 1.0f / RSILength.GetInt();

	if (sc.Index<RSILength.GetInt())
		return;

	// compute net and total changes

	float totalChange;
	float netChange;

	if (sc.Index==RSILength.GetInt())
	{
		// NetChgAvg = (Price - Price[Length] ) / Length
		netChange = (price[sc.Index] - price[sc.Index - RSILength.GetInt()]) / RSILength.GetInt();

		// TotChgAvg = Average( AbsValue( Price - Price[1] ), Length )
		float average = 0.0;
		for (int i=1; i<=sc.Index; i++)
		{
			average += abs(price[i] - price[i - 1]);
		}       
		totalChange = average / RSILength.GetInt();
	}
	else
	{
		// Change = Price - Price[1]
		float change = price[sc.Index] - price[sc.Index - 1];

		// NetChgAvg = NetChgAvg[1] + SF * ( Change - NetChgAvg[1] )
		float previousNetChange = NetChangeAvg[sc.Index - 1];
		netChange =  previousNetChange + smoothingFactor * ( change - previousNetChange );

		// TotChgAvg = TotChgAvg[1] + SF * ( AbsValue(Change) - TotChgAvg[1] )
		float previousTotalChangeAverage = TotalChangeAvg[sc.Index - 1];
		totalChange = previousTotalChangeAverage + smoothingFactor * ( abs(change) - previousTotalChangeAverage );
	}

	TotalChangeAvg[sc.Index] = totalChange;
	NetChangeAvg[sc.Index] = netChange;

	// compute the RSI value
	float changeRatio;
	if (totalChange!=0.0f)
	{
		changeRatio = netChange / totalChange;
	}
	else
	{
		changeRatio = 0.0f;
	}

	// RSI = 50 * ( ChgRatio + 1)
	RSIT[sc.Index] = 50 * (changeRatio + 1.0f);

	// Draw overbought / oversold indicators
	LineUpperLimit[sc.Index] = Line1.GetFloat();
	LineLowerLimit[sc.Index] = Line2.GetFloat();

	if (sc.Index >= RSIMovAvgLength.GetInt() + RSILength.GetInt())
	{
		// Draw smoothed RSI
		sc.MovingAverage(RSIT, RSIAvg, MOVAVGTYPE_SIMPLE, sc.Index, RSIMovAvgLength.GetInt());
	}
}

/******************************************************************************/
/*
Triple Exponential Moving Average, by Patrick Mulloy and published in January 1994 
in the Stocks & Commodities magazine

Algorithm is also described in http://www.visualchart.com/enxx/strategies/indicators/ayuda.asp?Id=2857
*/
SCSFExport scsf_MovingAverageTEMA(SCStudyInterfaceRef sc)
{
	SCSubgraphRef TEMA = sc.Subgraph[0];

	SCInputRef InputData = sc.Input[0];
	SCInputRef Length = sc.Input[1];

	if(sc.SetDefaults)
	{
		sc.GraphName="Moving Average-Triple Exp";
		sc.StudyDescription="Triple Exponential Moving Average";

		sc.GraphRegion = 0;

		TEMA.Name = "TEMA";
		TEMA.DrawStyle = DRAWSTYLE_LINE;
		TEMA.PrimaryColor = RGB(255, 0, 0);
		TEMA.DrawZeros = false;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		Length.Name = "Length";
		Length.SetIntLimits(1,MAX_STUDY_LENGTH);
		Length.SetInt(5);

		sc.AutoLoop = 1;

		//During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;

		return;
	}

	sc.DataStartIndex = Length.GetInt();

	SCFloatArrayRef data = sc.BaseData[InputData.GetInputDataIndex()];
	sc.TEMA(data,TEMA,sc.Index,Length.GetInt());
}

/******************************************************************************/

// http://www.danielstrading.com/education/technical-analysis/historic-volatility.php


SCSFExport scsf_HistoricVolatilityStudy(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Volatility = sc.Subgraph[0];
	SCSubgraphRef LogPriceChange = sc.Subgraph[1];
	SCSubgraphRef SMALog = sc.Subgraph[2];

	SCInputRef InputData = sc.Input[0];
	SCInputRef AlgorithmType = sc.Input[1];
	SCInputRef Length = sc.Input[2];
	SCInputRef NumberBarsPerYear = sc.Input[3];

	if(sc.SetDefaults)
	{
		sc.GraphName="Volatility - Historical";
		sc.StudyDescription="Volatility. Based on the number of specified bars (Length) and annualized.";

		Volatility.Name = "Volatility";
		Volatility.DrawStyle = DRAWSTYLE_LINE;
		Volatility.PrimaryColor = RGB(0, 255,0);
		Volatility.DrawZeros = false;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);


		Length.Name = "Length";
		Length.SetIntLimits(2,MAX_STUDY_LENGTH);
		Length.SetInt(20);

		NumberBarsPerYear.Name = "Number of Bars Per Year";
		NumberBarsPerYear.SetInt(262);

		sc.AutoLoop = 1;
		sc.ValueFormat = 2;

		//During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;

		return;
	}

	sc.DataStartIndex = Length.GetInt();


		// calculate the logarithm of the price change    
		LogPriceChange[sc.Index] = log(sc.BaseData[InputData.GetInputDataIndex()][sc.Index] / sc.BaseData[InputData.GetInputDataIndex()][sc.Index-1]);

		// calculate the average of the logs
		sc.SimpleMovAvg(LogPriceChange, SMALog, sc.Index, Length.GetInt());

		// sum the squares of the difference between the individual logarithms for each period and the average logarithm
		float SumSquareOfDifference = 0;
		for(int SumIndex=max(0, sc.Index - Length.GetInt() + 1); SumIndex <= sc.Index; SumIndex++)
		{
			float diff = LogPriceChange[SumIndex] - SMALog[SumIndex];
			SumSquareOfDifference += diff * diff;
		}

		// Final volatility calculation
		Volatility[sc.Index] = sqrt(SumSquareOfDifference / float(Length.GetInt()-1)) * sqrt(float(NumberBarsPerYear.GetInt())) * 100;


}


/******************************************************************************

Synthetic VIX indicator as described in the December 2007 issue of 
"Active Trader" magazine.  The indicator is also named the "Williams VIX Fix" 
since Larry Williams is credited with the formula's discovery.

Formula:

SynthVIX = (Highest(Close,22) - Low) / (Highest(Close,22)) * 100;

*******************************************************************************/
SCSFExport scsf_SyntheticVIX(SCStudyInterfaceRef sc)
{
	SCSubgraphRef SynthVIX = sc.Subgraph[0];

	SCInputRef Length = sc.Input[0];

    if(sc.SetDefaults)
    {
        sc.GraphName="Synthetic VIX";
        sc.StudyDescription="Synthetic VIX";

        SynthVIX.Name = "SynthVIX";
        SynthVIX.DrawStyle = DRAWSTYLE_LINE;
        SynthVIX.PrimaryColor = RGB(255, 255, 0);
		SynthVIX.DrawZeros = false;

        Length.Name = "Length";
        Length.SetIntLimits(1,MAX_STUDY_LENGTH);
        Length.SetInt(22);
		
		sc.AutoLoop = 1;

		//During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;

        return;
    }

    float HighestClose = sc.GetHighest(sc.Close, sc.Index, Length.GetInt());

    SynthVIX[sc.Index] = 100.0f * (HighestClose - sc.Low[sc.Index]) / HighestClose;
}

/*==========================================================================*/
void PassingSCStrutureExampleFunction(SCStudyInterfaceRef sc)
{
	//The "sc" structure can be used anywhere within this function.
}

/*============================================================================
	"An example of calling a function that receives the Sierra Chart ACSIL structure (sc)."
----------------------------------------------------------------------------*/
SCSFExport scsf_PassingSCStrutureExample(SCStudyInterfaceRef sc)
{
	if (sc.SetDefaults)
	{
		// Set the configuration and defaults
		
		sc.GraphName = "Passing sc Structure Example Function";
		
		sc.StudyDescription = "An example of calling a function that receives the Sierra Chart ACSIL structure (sc).";

		//During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;
		
		return;
	}
	
	// Do data processing

	// The definition of the function called below must be above this function.
	PassingSCStrutureExampleFunction(sc);
}

/************************************************************************/
SCSFExport scsf_InvertStudy(SCStudyInterfaceRef sc)
{


	SCSubgraphRef Open = sc.Subgraph[SC_OPEN];
	SCSubgraphRef High = sc.Subgraph[SC_HIGH];
	SCSubgraphRef Low = sc.Subgraph[SC_LOW];
	SCSubgraphRef Last = sc.Subgraph[SC_LAST];
	SCSubgraphRef Volume = sc.Subgraph[SC_VOLUME];
	SCSubgraphRef OHLCAvg = sc.Subgraph[SC_OHLC];
	SCSubgraphRef HLCAvg = sc.Subgraph[SC_HLC];
	SCSubgraphRef HLAvg = sc.Subgraph[SC_HL];
	
	SCSubgraphRef NumTrades = sc.Subgraph[SC_NT];
	SCSubgraphRef BidVol = sc.Subgraph[ SC_BIDVOL];
	SCSubgraphRef AskVol = sc.Subgraph[SC_ASKVOL];


	if(sc.SetDefaults)
	{
		sc.GraphName="Multiply Bars By -1";
		
		sc.ValueFormat = VALUEFORMAT_INHERITED;
		sc.AutoLoop = 1;
		sc.GraphDrawType = GDT_OHLCBAR;
		sc.StandardChartHeader = true;
		sc.GraphUsesChartColors= 1;

		Open.Name = "Open";
		Open.DrawStyle = DRAWSTYLE_LINE;
		Open.PrimaryColor = RGB(191,255,128);
		Open.DrawZeros = false;

		High.Name = "High";
		High.DrawStyle = DRAWSTYLE_LINE;
		High.PrimaryColor = RGB(255,0,0);
		High.DrawZeros = false;

		Low.Name = "Low";
		Low.DrawStyle = DRAWSTYLE_LINE;
		Low.PrimaryColor = RGB(0,255,0);
		Low.DrawZeros = false;

		Last.Name = "Close";
		Last.DrawStyle = DRAWSTYLE_LINE;
		Last.PrimaryColor = RGB(191,255,128);
		Last.DrawZeros = false;

		OHLCAvg.Name = "OHLCAvg";
		OHLCAvg.DrawStyle = DRAWSTYLE_LINE;
		OHLCAvg.PrimaryColor = RGB(127,0,255);
		OHLCAvg.DrawZeros = false;

		HLCAvg.Name = "HLCAvg";
		HLCAvg.DrawStyle = DRAWSTYLE_LINE;
		HLCAvg.PrimaryColor = RGB(0,255,255);
		HLCAvg.DrawZeros = false;

		HLAvg.Name = "HLAvg";
		HLAvg.DrawStyle = DRAWSTYLE_LINE;
		HLAvg.PrimaryColor = RGB(0,127,255);
		HLAvg.DrawZeros = false;

		Volume.Name = "Volume";
		Volume.DrawStyle = DRAWSTYLE_IGNORE;
		Volume.PrimaryColor = RGB(255,0,0);
		Volume.DrawZeros = false;

		NumTrades.Name = "Num of Trades";
		NumTrades.DrawStyle = DRAWSTYLE_IGNORE;
		NumTrades.PrimaryColor = RGB(0,0,255);
		NumTrades.DrawZeros = false;

		BidVol.Name = "Bid Volume";
		BidVol.DrawStyle = DRAWSTYLE_IGNORE;
		BidVol.PrimaryColor = RGB(0,255,0);
		BidVol.DrawZeros = false;

		AskVol.Name = "Ask Volume";
		AskVol.DrawStyle = DRAWSTYLE_IGNORE;
		AskVol.PrimaryColor = RGB(0,255,0);
		AskVol.DrawZeros = false;

		//During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;

		return;
	}

	sc.GraphName.Format("%s Inverted", sc.GetStudyName(0).GetChars());

	Open[sc.Index] = sc.Open[sc.Index] * -1.0f;
	Last[sc.Index] = sc.Close[sc.Index] * -1.0f;
	High[sc.Index] = sc.High[sc.Index] * -1.0f;
	Low[sc.Index] = sc.Low[sc.Index] * -1.0f;
	OHLCAvg[sc.Index] = sc.OHLCAvg[sc.Index] * -1.0f;
	HLCAvg[sc.Index] = sc.HLCAvg[sc.Index] * -1.0f;
	HLAvg[sc.Index] = sc.HLAvg[sc.Index] * -1.0f;

	Volume[sc.Index] = sc.Volume[sc.Index];
	NumTrades[sc.Index] = sc.NumberOfTrades[sc.Index];
	BidVol[sc.Index] = sc.BidVolume[sc.Index];
	AskVol[sc.Index] = sc.AskVolume[sc.Index];	
}

/***********************************************************************/
SCSFExport scsf_CMO(SCStudyInterfaceRef sc)
{
    enum InputIndexes
    {
        INPUT_DATA= 0,
        INPUT_CMO_LENGTH,
        INPUT_LINE_LEVEL2,
        INPUT_LINE_LEVEL1,
        INPUT_LINE_LEVEL3
    };

	SCSubgraphRef CMO = sc.Subgraph[0];
	SCSubgraphRef Line2 = sc.Subgraph[1];
	SCSubgraphRef Line1 = sc.Subgraph[2];
	SCSubgraphRef Line3 = sc.Subgraph[3];

	SCInputRef InputData = sc.Input[0];
	SCInputRef CMOLength = sc.Input[1];
	SCInputRef Line1Level = sc.Input[2];
	SCInputRef Line2Level = sc.Input[3];
	SCInputRef Line3Level = sc.Input[4];

    if(sc.SetDefaults)
    {
        sc.GraphName="Chande Momentum Oscillator";
        sc.StudyDescription="Chande Momentum Oscillator";

        
        CMO.Name="CMO";
        CMO.DrawStyle = DRAWSTYLE_LINE;
		CMO.PrimaryColor = RGB(0,255,0);
		CMO.DrawZeros = true;

        Line2.Name="Line 2";
        Line2.DrawStyle = DRAWSTYLE_LINE;
        Line2.PrimaryColor = RGB(255,0,0);
		Line2.DrawZeros = true;

        Line1.Name="Line 1";
        Line1.DrawStyle = DRAWSTYLE_LINE;
        Line1.PrimaryColor = RGB(255,0,0);
		Line1.DrawZeros = true;

        Line3.Name="Line 3";
        Line3.DrawStyle = DRAWSTYLE_LINE;
        Line3.PrimaryColor = RGB(255,0,0);
		Line3.DrawZeros = true;

        InputData.Name="Input Data";
        InputData.SetInputDataIndex(SC_LAST);

        CMOLength.Name="CMO Length";
        CMOLength.SetInt(14);

        Line1Level.Name="Line 1 level";
        Line1Level.SetFloat(50.0f);

        Line2Level.Name="Line 2 level";
        Line2Level.SetFloat(0.0f);

        Line3Level.Name="Line 3 level";
        Line3Level.SetFloat(-50.0f);

		//During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;

		sc.AutoLoop = 1;

        return;
    }
    sc.DataStartIndex = CMOLength.GetInt();

	if (sc.Index < CMOLength.GetInt()-1)
		return;

    float sum_up = 0;
    float sum_down = 0;
    for (int j = (sc.Index-CMOLength.GetInt()+1);j<=sc.Index;j++)
    {
        if (sc.BaseData[InputData.GetInputDataIndex()][j]<sc.BaseData[InputData.GetInputDataIndex()][j-1])
        {
            sum_down += sc.BaseData[InputData.GetInputDataIndex()][j-1] - sc.BaseData[InputData.GetInputDataIndex()][j];
        }
        else if (sc.BaseData[InputData.GetInputDataIndex()][j]>sc.BaseData[InputData.GetInputDataIndex()][j-1])
        {
            sum_up += sc.BaseData[InputData.GetInputDataIndex()][j] - sc.BaseData[InputData.GetInputDataIndex()][j-1];
        }
    }
    //CMO(t) = 100 * ((SumUp(t) - SumDown(t)) / (SumUp(t) + SumDown(t)))
    float fCMO;

    if(sum_up+sum_down == 0)
        fCMO = CMO[sc.Index-1];
    else
        fCMO = 100.0f*((sum_up-sum_down)/(sum_up+sum_down));

    CMO[sc.Index] = fCMO;
    Line2[sc.Index] = Line2Level.GetFloat();
    Line1[sc.Index] = Line1Level.GetFloat();
    Line3[sc.Index] = Line3Level.GetFloat();
}

/***********************************************************************/
SCSFExport scsf_ZeroLagEMA(SCStudyInterfaceRef sc)
{
	SCSubgraphRef ZeroLagEma = sc.Subgraph[0];
	SCSubgraphRef Ema1 = sc.Subgraph[1];
	SCSubgraphRef Ema2 = sc.Subgraph[2];
	SCSubgraphRef Difference = sc.Subgraph[3];

	SCInputRef Period = sc.Input[0];
	SCInputRef InputData = sc.Input[1];

    if(sc.SetDefaults)
    {
        sc.GraphName="Zero Lag EMA";
        sc.StudyDescription="Zero lag exponential moving average.";

        sc.AutoLoop = 1;
        sc.GraphRegion = 0;

        ZeroLagEma.Name="Zero Lag EMA";
        ZeroLagEma.DrawStyle = DRAWSTYLE_LINE;
		ZeroLagEma.PrimaryColor = RGB(0,255,0);
		ZeroLagEma.DrawZeros = false;
        
        Ema1.DrawStyle = DRAWSTYLE_IGNORE;
        
        Ema2.DrawStyle = DRAWSTYLE_IGNORE;

        Difference.DrawStyle = DRAWSTYLE_IGNORE;

        Period.Name="Zero Lag EMA Length";
        Period.SetInt(10);
        Period.SetIntLimits(1,MAX_STUDY_LENGTH);

        InputData.Name = "Input Data";
        InputData.SetInputDataIndex(SC_LAST);

		// During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;

        return;
    }
	sc.DataStartIndex = Period.GetInt()-1;

    sc.MovingAverage(sc.BaseData[InputData.GetInputDataIndex()],Ema1,MOVAVGTYPE_EXPONENTIAL,sc.Index,Period.GetInt());
    sc.MovingAverage(Ema1,Ema2,MOVAVGTYPE_EXPONENTIAL,sc.Index,Period.GetInt());
    Difference[sc.Index] = Ema1[sc.Index] - Ema2[sc.Index];
    ZeroLagEma[sc.Index] = Ema1[sc.Index] + Difference[sc.Index];

}
/***********************************************************************/
SCSFExport scsf_Tenkan_Sen(SCStudyInterfaceRef sc)
{
	SCSubgraphRef TenkanSen = sc.Subgraph[0];
	SCFloatArrayRef HH = sc.Subgraph[0].Arrays[0];
	SCFloatArrayRef LL = sc.Subgraph[0].Arrays[1];

	SCInputRef Period = sc.Input[0];
	SCInputRef InputDataHigh = sc.Input[1];
	SCInputRef InputDataLow = sc.Input[2];
	SCInputRef Version = sc.Input[3];

    if(sc.SetDefaults)
    {
        sc.GraphName="Tenkan-Sen";

        sc.AutoLoop = 1;
        sc.GraphRegion = 0;

        TenkanSen.Name="Tenkan-Sen";
        TenkanSen.DrawStyle = DRAWSTYLE_LINE;
        TenkanSen.LineWidth = 2;
        TenkanSen.PrimaryColor = RGB(255,128,0);
		TenkanSen.DrawZeros = false;

        Period.Name="Tenkan-Sen Length";
        Period.SetInt(9);
        Period.SetIntLimits(1,MAX_STUDY_LENGTH);

        InputDataHigh.Name = "Input Data High";
        InputDataHigh.SetInputDataIndex(SC_HIGH);

		InputDataLow.Name = "Input Data Low";
		InputDataLow.SetInputDataIndex(SC_LOW);

		Version.  SetInt(1);

		//During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;

        return;
    }

	if (Version.GetInt()<1)
	{
		InputDataHigh.SetInputDataIndex(SC_HIGH);
		InputDataLow.SetInputDataIndex(SC_LOW);
		Version.  SetInt(1);
	}

    sc.DataStartIndex = Period.GetInt()-1;

    sc.Highest(sc.BaseData[InputDataHigh.GetInputDataIndex()],HH, Period.GetInt());
    sc.Lowest(sc.BaseData[InputDataLow.GetInputDataIndex()],LL, Period.GetInt());
    TenkanSen[sc.Index] = (HH[sc.Index]+LL[sc.Index])/2.0f;

}
/***********************************************************************/
SCSFExport scsf_Kijun_Sen(SCStudyInterfaceRef sc)
{
	SCSubgraphRef KijunSen = sc.Subgraph[0];
	SCFloatArrayRef HH = sc.Subgraph[0].Arrays[0];
	SCFloatArrayRef LL = sc.Subgraph[0].Arrays[1];
	

	SCInputRef Period = sc.Input[0];
	SCInputRef InputDataHigh = sc.Input[1];
	SCInputRef InputDataLow = sc.Input[2];
	SCInputRef Version = sc.Input[3];

    if(sc.SetDefaults)
    {
        sc.GraphName="Kijun-Sen";

        sc.AutoLoop = 1;
        sc.GraphRegion = 0;

        KijunSen.Name="Kijun-Sen";
        KijunSen.DrawStyle = DRAWSTYLE_LINE;
        KijunSen.LineWidth = 2;
        KijunSen.PrimaryColor = RGB(255,0,128);
		KijunSen.DrawZeros = false;

        Period.Name="Kijun-Sen Length";
        Period.SetInt(26);
        Period.SetIntLimits(1,MAX_STUDY_LENGTH);

		InputDataHigh.Name = "Input Data High";
		InputDataHigh.SetInputDataIndex(SC_HIGH);

		InputDataLow.Name = "Input Data Low";
		InputDataLow.SetInputDataIndex(SC_LOW);

		Version.  SetInt(1);
		//During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;

        return;
    }

	if (Version.GetInt()<1)
	{
		InputDataHigh.SetInputDataIndex(SC_HIGH);
		InputDataLow.SetInputDataIndex(SC_LOW);

		Version.  SetInt(1);
	}

	sc.DataStartIndex = Period.GetInt()-1;

	sc.Highest(sc.BaseData[InputDataHigh.GetInputDataIndex()],HH, Period.GetInt());
	sc.Lowest(sc.BaseData[InputDataLow.GetInputDataIndex()],LL, Period.GetInt());
	KijunSen[sc.Index] = (HH[sc.Index] + LL[sc.Index])/2.0f;


}
/***********************************************************************/
SCSFExport scsf_ChikouSpan(SCStudyInterfaceRef sc)
{
	SCSubgraphRef ChikouSpan = sc.Subgraph[0];

	SCInputRef Period = sc.Input[0];
	SCInputRef InputData = sc.Input[1];

    if(sc.SetDefaults)
    {
        sc.GraphName="Chikou Span";

        sc.AutoLoop = 1;
        sc.GraphRegion = 0;

        ChikouSpan.Name="Chikou Span";
        ChikouSpan.DrawStyle = DRAWSTYLE_LINE;
        ChikouSpan.LineWidth = 2;
        ChikouSpan.PrimaryColor = RGB(20,180,240);
		ChikouSpan.DrawZeros = false;

        Period.Name="Chikou Span Length";
        Period.SetInt(26);
        Period.SetIntLimits(1,MAX_STUDY_LENGTH);

        InputData.Name = "Input Data";
        InputData.SetInputDataIndex(SC_LAST);

		//During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;

        return;
    }

	//Chikou Span (Lagging Span): Close plotted 26 days in the past
    ChikouSpan[sc.Index-Period.GetInt()] = sc.BaseData[InputData.GetInputDataIndex()][sc.Index];
}

/***********************************************************************/
SCSFExport scsf_SenkouSpanA(SCStudyInterfaceRef sc)
{
	SCSubgraphRef SpanA = sc.Subgraph[0];
	SCSubgraphRef TenkanHH = sc.Subgraph[1];
	SCSubgraphRef TenkanLL = sc.Subgraph[2];
	SCSubgraphRef KijunHH = sc.Subgraph[3];
	SCSubgraphRef KijunLL = sc.Subgraph[4];
	
	SCInputRef TenkanPeriod = sc.Input[1];
	SCInputRef KijunPeriod = sc.Input[2];

	
    if(sc.SetDefaults)
    {
        sc.GraphName="Senkou Span A";
        sc.AutoLoop = 1;
        sc.GraphRegion = 0;

		SpanA.GraphicalDisplacement = 26;
        SpanA.Name="Span A";
        SpanA.DrawStyle = DRAWSTYLE_LINE;
        SpanA.LineWidth = 4;
        SpanA.PrimaryColor = RGB(0,85,0);
		SpanA.DrawZeros = false;

        TenkanPeriod.Name="Tenkan-Sen Length";
        TenkanPeriod.SetInt(9);
        TenkanPeriod.SetIntLimits(1,MAX_STUDY_LENGTH);

        KijunPeriod.Name="Kijun-Sen Length";
        KijunPeriod.SetInt(26);
        KijunPeriod.SetIntLimits(1,MAX_STUDY_LENGTH);

	   //During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
	   sc.FreeDLL = 0;

        return;
    }

   
    sc.Highest(sc.BaseData[SC_HIGH],TenkanHH, TenkanPeriod.GetInt());
    sc.Lowest(sc.BaseData[SC_LOW],TenkanLL, TenkanPeriod.GetInt());
    sc.Highest(sc.BaseData[SC_HIGH],KijunHH, KijunPeriod.GetInt());
    sc.Lowest(sc.BaseData[SC_LOW],KijunLL, KijunPeriod.GetInt());

    SpanA[sc.Index] = (
       ( 
	   ( (TenkanHH[sc.Index]+TenkanLL[sc.Index])/ 2.0f)+
	   ( (KijunHH[sc.Index]+KijunLL[sc.Index])/ 2.0f)
		)/2.0f);
}

/***********************************************************************/
SCSFExport scsf_SenkouSpan(SCStudyInterfaceRef sc)
{
	SCSubgraphRef SpanA = sc.Subgraph[0];
	SCSubgraphRef SpanB = sc.Subgraph[1];
	SCSubgraphRef SpanBHH = sc.Subgraph[2];
	SCSubgraphRef SpanBLL = sc.Subgraph[3];
	SCSubgraphRef TenkanHH = sc.Subgraph[4];
	SCSubgraphRef TenkanLL = sc.Subgraph[5];
	SCSubgraphRef KijunHH = sc.Subgraph[6];
	SCSubgraphRef KijunLL = sc.Subgraph[7];

	SCInputRef InputDataHigh= sc.Input[0];
	SCInputRef SpanBLength = sc.Input[1];
	SCInputRef TenkanLength = sc.Input[2];
	SCInputRef KijunLength = sc.Input[3];
	SCInputRef InputDataLow = sc.Input[4];
	SCInputRef Version = sc.Input [5];

	if(sc.SetDefaults)
	{
		sc.GraphName="Senkou Span A & B";
		
		sc.AutoLoop = 1;
		sc.GraphRegion = 0;

		SpanA.Name="Span A";
		SpanA.DrawStyle = DRAWSTYLE_FILLTOP;
		SpanA.PrimaryColor = RGB(133,205,24);
		SpanA.GraphicalDisplacement = 26;
		SpanA.DrawZeros = false;

		SpanB.Name="Span B";
		SpanB.DrawStyle = DRAWSTYLE_FILLBOTTOM;
		SpanB.PrimaryColor = RGB(133,205,24);
		SpanB.GraphicalDisplacement = 26;
		SpanB.DrawZeros = false;

		InputDataHigh.Name = "Input Data High";
		InputDataHigh.SetInputDataIndex(SC_HIGH);

		InputDataLow.Name = "Input Data Low";
		InputDataLow.SetInputDataIndex(SC_LOW);

		SpanBLength.Name="Senkou Span B Length";
		SpanBLength.SetInt(52);
		SpanBLength.SetIntLimits(1,MAX_STUDY_LENGTH);

		TenkanLength.Name="Tenkan-Sen Length";
		TenkanLength.SetInt(9);
		TenkanLength.SetIntLimits(1,MAX_STUDY_LENGTH);

		KijunLength.Name="Kijun-Sen Length";
		KijunLength.SetInt(26);
		KijunLength.SetIntLimits(1,MAX_STUDY_LENGTH);

		Version.SetInt(1);

		//During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;

		return;
	}

	if (Version.GetInt() <1)
	{
		InputDataHigh.SetInputDataIndex(SC_HIGH);
		InputDataLow.SetInputDataIndex(SC_LOW);
		Version.SetInt(1);
	}

	// Senkou Span A

	sc.Highest(sc.BaseData[InputDataHigh.GetInputDataIndex()],TenkanHH, TenkanLength.GetInt());
	sc.Lowest(sc.BaseData[InputDataLow.GetInputDataIndex()],TenkanLL, TenkanLength.GetInt());

	sc.Highest(sc.BaseData[InputDataHigh.GetInputDataIndex()],KijunHH, KijunLength.GetInt());
	sc.Lowest(sc.BaseData[InputDataLow.GetInputDataIndex()],KijunLL, KijunLength.GetInt());

	float  Tenkan=(TenkanHH[sc.Index]+TenkanLL[sc.Index]) / 2.0f;
	float Kijun=(KijunHH[sc.Index]+KijunLL[sc.Index]) / 2.0f;
	SpanA[sc.Index] = (Tenkan +Kijun) / 2.0f;



	// Senkou Span B

	sc.Highest(sc.BaseData[InputDataHigh.GetInputDataIndex()],SpanBHH, SpanBLength.GetInt());
	sc.Lowest(sc.BaseData[InputDataLow.GetInputDataIndex()],SpanBLL, SpanBLength.GetInt());

	SpanB[sc.Index] = (SpanBHH[sc.Index]+SpanBLL[sc.Index])/2.0f ;

}

/***********************************************************************/
SCSFExport scsf_SenkouSpanB(SCStudyInterfaceRef sc)
{
	SCSubgraphRef SpanB = sc.Subgraph[0];
	SCSubgraphRef HH = sc.Subgraph[1];
	SCSubgraphRef LL = sc.Subgraph[2];

	SCInputRef InputDataHigh = sc.Input[0];
	SCInputRef StudyLength = sc.Input[1];
	SCInputRef InputDataLow = sc.Input[2];
	SCInputRef HiddenFlag = sc.Input[4];

	

    if(sc.SetDefaults)
    {
        sc.GraphName="Senkou Span B";

        sc.AutoLoop = 1;
        sc.GraphRegion = 0;

		SpanB.GraphicalDisplacement = 26;
        SpanB.Name="Span B";
        SpanB.DrawStyle = DRAWSTYLE_LINE;
        SpanB.LineWidth = 4;
        SpanB.PrimaryColor = RGB(133,205,24);
		SpanB.DrawZeros = false;

        StudyLength.Name="Senkou Span B Length";
        StudyLength.SetInt(52);
        StudyLength.SetIntLimits(1,MAX_STUDY_LENGTH);

        InputDataHigh.Name = "Input Data High";
        InputDataHigh.SetInputDataIndex(SC_HIGH);

		InputDataLow.Name = "Input Data Low";
		InputDataLow.SetInputDataIndex(SC_LOW);

		HiddenFlag.SetInt(2);

	   // During development set this flag to 1, so the dll can be modified. When development is done, set it to 0 to improve performance.
	   sc.FreeDLL = 0;

        return;
    } 
	

	if (HiddenFlag.GetInt() <2)
	{
		InputDataHigh.SetInputDataIndex(SC_HIGH);
		InputDataLow.SetInputDataIndex(SC_LOW);
		HiddenFlag.SetInt(2);
	}

    sc.Highest(sc.BaseData[InputDataHigh.GetInputDataIndex()],HH, StudyLength.GetInt());
    sc.Lowest(sc.BaseData[InputDataLow.GetInputDataIndex()],LL, StudyLength.GetInt());

    SpanB[sc.Index] =  (HH[sc.Index]+LL[sc.Index])/2.0f ;

}
/*-------------------------------------------------
	This study function colors bars according to the slope of a study subgraph it is based on.
----------------------------------------------------------------------------*/
SCSFExport scsf_ColorBarBasedOnSlope(SCStudyInterfaceRef sc)
{
	SCSubgraphRef ColorBar = sc.Subgraph[0];

	SCInputRef InputData = sc.Input[0];
	SCInputRef ModeInput = sc.Input[1];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Color Bar Based On Slope";

		sc.StudyDescription = "Colors the main price graph bars based on the slope of a study Subgraph this study is Based On.";

		sc.GraphRegion = 0;
		sc.AutoLoop = 1;

		ColorBar.Name = "Color Bar";
		ColorBar.DrawStyle = DRAWSTYLE_COLORBAR;
		ColorBar.SecondaryColorUsed = 1;  // true
		ColorBar.PrimaryColor = RGB(0,255,0);
		ColorBar.SecondaryColor = RGB(255,0,0);
		ColorBar.DrawZeros = false;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(0);

		ModeInput.Name = "Use +1 (+slope) and -1 (-slope) for Color Bar Values";
		ModeInput.SetYesNo(0);

		//During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;

		return;
	}
	
	// Do data processing

	if (sc.BaseData[InputData.GetInputDataIndex()][sc.Index] > sc.BaseData[InputData.GetInputDataIndex()][sc.Index-1])
	{
		ColorBar[sc.Index] = 1;
		ColorBar.DataColor[sc.Index] = ColorBar.PrimaryColor;
	}
	else if (sc.BaseData[InputData.GetInputDataIndex()][sc.Index] < sc.BaseData[InputData.GetInputDataIndex()][sc.Index-1])
	{
		if (ModeInput.GetYesNo()== false)
			ColorBar[sc.Index] = 1;
		else
			ColorBar[sc.Index] = -1;

		ColorBar.DataColor[sc.Index] = ColorBar.SecondaryColor;
	}
	else
	{
		ColorBar[sc.Index] = 0;
	}
	
}

/***********************************************************************/

SCFloatArrayRef MyCCI(SCStudyInterfaceRef sc, SCFloatArrayRef In, SCFloatArrayRef SMAOut, SCFloatArrayRef CCIOut, int Index, int Length, float Multiplier)
{
	if (Length < 1)
		return CCIOut;
	
	if (Index < 2*Length - 1)
		return CCIOut;
	
	sc.SimpleMovAvg(In, SMAOut, Index, Length);
	
    float Num0 = 0;
	for (int j = Index; j > Index - Length && j >= 0; j--)
		Num0 += fabs(SMAOut[Index] - In[j]);
	
	Num0 /= Length;
	
	CCIOut[Index] = (In[Index] - SMAOut[Index]) / (Num0 * Multiplier);
	
	return CCIOut;
}

/*************************************************************************/
SCSFExport scsf_CCI(SCStudyInterfaceRef sc)
{
	SCSubgraphRef CCI = sc.Subgraph[0];

	SCInputRef Length = sc.Input[0];

    if(sc.SetDefaults)
    {
        sc.GraphName="Commodity Channel Index";
        sc.StudyDescription="This is an example on how to call a function from your study that is not a member of the 'sc' structure.  It calculates a Commodity Channel Index.";

       
        sc.AutoLoop = 1;
        sc.GraphRegion = 1;

        CCI.Name="CCI";
        CCI.DrawStyle = DRAWSTYLE_LINE;
		CCI.PrimaryColor = RGB(0,255,0);
		CCI.DrawZeros = true;

		Length.Name="Length";
        Length.SetInt(10);
        Length.SetIntLimits(1,MAX_STUDY_LENGTH);

		//During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;


        return;
    }
    
    sc.DataStartIndex = Length.GetInt();
	
   //We will call a custom CCI function.

	MyCCI(sc, sc.Close, CCI.Arrays[0], CCI, sc.Index, Length.GetInt(), 0.015f);
}

/*************************************************************************/
SCSFExport scsf_DMI_ADX_ADXR(SCStudyInterfaceRef sc)
{
	SCSubgraphRef DmiPlus = sc.Subgraph[0];
	SCSubgraphRef DmiMinus = sc.Subgraph[1];
	SCSubgraphRef ADX = sc.Subgraph[2];
	SCSubgraphRef ADXR = sc.Subgraph[3];

	SCInputRef Length = sc.Input[3];
	SCInputRef MALength = sc.Input[4];
	SCInputRef ADXRInterval = sc.Input[5];

    if (sc.SetDefaults)
    {
        // Set the configuration and defaults

        sc.GraphName = "DMI & ADX & ADXR";

        sc.StudyDescription = "DMI: Welles Wilder's Plus and Minus Directional Indicators +DI and -DI.\n "
                              "ADX: Average Directional Movement Index.  This is calculated according to the Welles Wilder formulas.\n"
                              "ADXR: Average Directional Movement Index Rating.  This is calculated according to the Welles Wilder formulas.";

        sc.AutoLoop = 1;  // true



        DmiPlus.Name = "DI+";
        DmiPlus.DrawStyle = DRAWSTYLE_LINE;
		DmiPlus.PrimaryColor = RGB(0,255,0);
		DmiPlus.DrawZeros = true;

        DmiMinus.Name = "DI-";
        DmiMinus.DrawStyle = DRAWSTYLE_LINE;
		DmiMinus.PrimaryColor = RGB(255,0,255);
		DmiMinus.DrawZeros = true;

        ADX.Name = "ADX";
        ADX.DrawStyle = DRAWSTYLE_LINE;
		ADX.PrimaryColor = RGB(255,255,0);
		ADX.DrawZeros = true;

        ADXR.Name = "ADXR";
        ADXR.DrawStyle = DRAWSTYLE_LINE;
		ADXR.PrimaryColor = RGB(255,127,0);
		ADXR.DrawZeros = true;

        Length.Name = "Length";
        Length.SetInt(14);
        Length.SetIntLimits(1, INT_MAX);

        MALength.Name = "Mov Avg Length";
        MALength.SetInt(14);
        MALength.SetIntLimits(1, INT_MAX);

        ADXRInterval.Name = "ADXR Interval";
        ADXRInterval.SetInt(14);
        ADXRInterval.SetIntLimits(1, INT_MAX);

		//During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;

        return;
    }

    // Do data processing

    sc.DataStartIndex = max(max(Length.GetInt(), Length.GetInt() + MALength.GetInt() + ADXRInterval.GetInt() - 2), Length.GetInt() + MALength.GetInt() - 1);

    sc.DMI(sc.BaseData, DmiPlus, DmiMinus, sc.Index, Length.GetInt());

    sc.ADX(sc.BaseData, ADX, sc.Index, Length.GetInt(), MALength.GetInt());

    sc.ADXR(sc.BaseData, ADXR, sc.Index, Length.GetInt(), MALength.GetInt(), ADXRInterval.GetInt());
}

/*****************************************************************************/
SCSFExport scsf_ColorBarHHLL(SCStudyInterfaceRef sc)
{
	SCSubgraphRef ColorBar = sc.Subgraph[0];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults
		
		sc.GraphName = "Color Bar HH/LL";
		
		sc.StudyDescription = "This study function colors bars based on if they made a higher high or lower low.";
		
		sc.GraphRegion = 0;
		
		ColorBar.Name = "Color Bar";
		ColorBar.DrawStyle = DRAWSTYLE_COLORBAR;
		ColorBar.SecondaryColorUsed = 1;  // true
		ColorBar.PrimaryColor = RGB(0,255,0);
		ColorBar.SecondaryColor = RGB(255,0,0);


		sc.AutoLoop = 1;

		//During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;
		
		return;
	}
		
	// Do data processing
		
	float HighValue = sc.High[sc.Index];
	float LowValue = sc.Low[sc.Index];
	float PrevHighValue = sc.High[sc.Index-1];
	float PrevLowValue = sc.Low[sc.Index-1];
	
	if (HighValue > PrevHighValue && LowValue < PrevLowValue)
	{
		ColorBar[sc.Index] = 1;

		COLORREF Pri = ColorBar.PrimaryColor;
		COLORREF Sec = ColorBar.SecondaryColor;
		ColorBar.DataColor[sc.Index] = sc.RGBInterpolate(Pri, Sec, 0.5f);
	}
	else if (HighValue > PrevHighValue)
	{
		ColorBar[sc.Index] = 1;
		ColorBar.DataColor[sc.Index] = ColorBar.PrimaryColor;
	}
	else if (LowValue < PrevLowValue)
	{
		ColorBar[sc.Index] = 1;
		ColorBar.DataColor[sc.Index] = ColorBar.SecondaryColor;
	}
	else
	{
		ColorBar[sc.Index] = 0;
	}
}

/**********************************************/
SCSFExport scsf_ColorVolumeBasedOnSlope(SCStudyInterfaceRef sc)
{
	SCSubgraphRef ColorVolume = sc.Subgraph[0];

	SCInputRef InputData = sc.Input[0];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults
		sc.GraphName = "Color Volume Based On Slope";
		sc.StudyDescription = "Colors Volume bars based on the slope of the study subgraph it is Based On.";

		sc.GraphRegion = 1;
		sc.AutoLoop = 1;

		ColorVolume.Name = "Color Volume";
		ColorVolume.DrawStyle = DRAWSTYLE_BAR;
		ColorVolume.PrimaryColor = RGB(0,255,0);
		ColorVolume.SecondaryColorUsed = 1;  // true
		ColorVolume.SecondaryColor = RGB(0,127,0);

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(0);

		//During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;

		return;
	}
 
 
	// Do data processing
 
	if (sc.BaseData[InputData.GetInputDataIndex()][sc.Index] > sc.BaseData[InputData.GetInputDataIndex()][sc.Index-1])
	{
		ColorVolume[sc.Index] = sc.Volume[sc.Index]; 
		ColorVolume.DataColor[sc.Index] = ColorVolume.PrimaryColor;
	}
	else if (sc.BaseData[InputData.GetInputDataIndex()][sc.Index] < sc.BaseData[InputData.GetInputDataIndex()][sc.Index-1])
	{
		ColorVolume[sc.Index] = sc.Volume[sc.Index]; 
		ColorVolume.DataColor[sc.Index] = ColorVolume.SecondaryColor;
	}
	else
	{
		ColorVolume[sc.Index] = 0;
	}
}

/***************************/
SCSFExport scsf_DetrendedOsc(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Oscillator = sc.Subgraph[0];
	SCSubgraphRef Line1 = sc.Subgraph[1];
	SCSubgraphRef Line2 = sc.Subgraph[2];

	SCInputRef InputData = sc.Input[0];
	SCInputRef MALength = sc.Input[3];
	SCInputRef Line1Value = sc.Input[4];
	SCInputRef Line2Value = sc.Input[5];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Detrended Oscillator - Di Napoli"; 

		sc.GraphRegion = 1;
		sc.ValueFormat = 3;

		Oscillator.Name = "Oscillator";
		Oscillator.DrawStyle = DRAWSTYLE_LINE;
		Oscillator.PrimaryColor = RGB(0,255,0);
		Oscillator.DrawZeros = true;

		Line1.Name = "Line 1";
		Line1.DrawStyle = DRAWSTYLE_IGNORE;
		Line1.PrimaryColor = RGB(255,0,255);
		Line1.DrawZeros = true;

		Line2.Name = "Line 2";
		Line2.DrawStyle = DRAWSTYLE_IGNORE;
		Line2.PrimaryColor = RGB(255,255,0);
		Line2.DrawZeros = true;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		MALength.Name = "Mov Avg Length";
		MALength.SetInt(7);
		MALength.SetIntLimits(1,MAX_STUDY_LENGTH);

		Line1Value.Name = "Line 1 Value";
		Line1Value.SetFloat(10);
	
		Line2Value.Name = "Line 2 Value";
		Line2Value.SetFloat(-10);

		sc.DrawZeros = true;
		//During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;

		sc.AutoLoop = 1;

		return;
	}

	sc.DataStartIndex = MALength.GetInt();

	if (sc.Index < MALength.GetInt())
		return;
	
	sc.SimpleMovAvg(sc.BaseData[InputData.GetInputDataIndex()],Oscillator.Arrays[0], MALength.GetInt());

	Oscillator[sc.Index] = sc.BaseData[InputData.GetInputDataIndex()][sc.Index] - Oscillator.Arrays[0][sc.Index];

	Line1[sc.Index] = Line1Value.GetFloat();
	Line2[sc.Index] = Line2Value.GetFloat();
}

/*============================================================================*/
SCSFExport scsf_CountDownTimer(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Countdown = sc.Subgraph[0];

	SCInputRef HorizontalPosition = sc.Input[0];
	SCInputRef VerticalPosition = sc.Input[1];
	SCInputRef DisplayInFillSpace = sc.Input[2];
	SCInputRef DisplayValueOnly = sc.Input[3];
	SCInputRef DisplayTimeCountdown = sc.Input[4];
	SCInputRef DisplayRangeInTicks = sc.Input[5];
	SCInputRef AlertOnNewBar = sc.Input[6];
	SCInputRef TransparentLabelBackground = sc.Input[7];

	SCInputRef UseCurrentPriceForVP = sc.Input[11];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults
		
		sc.GraphName = "CountDown Timer";
		
		sc.StudyDescription = "A countdown timer.";
		
		sc.AutoLoop = 0;
		sc.GraphRegion = 0;

		Countdown.Name = "Countdown";
		Countdown.LineWidth = 20;
		Countdown.DrawStyle = DRAWSTYLE_CUSTOM_TEXT;
		Countdown.PrimaryColor = RGB(0, 0, 0); //black
		Countdown.SecondaryColor = RGB(255, 127, 0); //Orange
		Countdown.SecondaryColorUsed = true;
		Countdown.DisplayNameValueInWindowsFlags = 0;
		
		HorizontalPosition.Name = "Initial Horizontal Position From Left (1-150)";
		HorizontalPosition.SetInt(20);
		HorizontalPosition.SetIntLimits(1,150);

		VerticalPosition.Name = "Initial Vertical Position From Bottom (1-100)";
		VerticalPosition.SetInt(90);
		VerticalPosition.SetIntLimits(1,100);

		DisplayInFillSpace.Name = "Display in Fill Space";
		DisplayInFillSpace.SetYesNo(false);

		DisplayValueOnly.Name = "Display Value Only";
		DisplayValueOnly.SetYesNo(false);
		
		DisplayTimeCountdown.Name = "Display Continuous Time Countdown Based on Real-Time Clock";
		DisplayTimeCountdown.SetYesNo(false);

		DisplayRangeInTicks.Name = "Display Remaining Range in Ticks";
		DisplayRangeInTicks.SetYesNo(false);

		AlertOnNewBar.Name = "Alert On New Bar";
		AlertOnNewBar.SetYesNo(false);

		TransparentLabelBackground.Name = "Transparent Label Background";
		TransparentLabelBackground.SetYesNo(false);


		UseCurrentPriceForVP.Name = "Use Current Price For Vertical Position";
		UseCurrentPriceForVP.SetYesNo(false);

		// During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = false;

		return;
	}

	static const int CountDownTimerLineNumber = COUNTDOWN_TIMER_CHART_DRAWINGNUMBER;

	if (sc.LastCallToFunction && sc.UserDrawnChartDrawingExists(sc.ChartNumber,CountDownTimerLineNumber) > 0)
	{
		// be sure to delete user drawn type drawing when study removed
		sc.DeleteUserDrawnACSDrawing(sc.ChartNumber, CountDownTimerLineNumber);
		return;
	}


	int& PriorArraySize = sc.PersistVars->i1;
	
	// Disable UpdateAlways.
	sc.UpdateAlways = 0;

	//If these inputs change after the study has been added, then we need to remove the prior drawing that was marked as user drawn
	bool UserDrawn = sc.GraphRegion == 0 && !UseCurrentPriceForVP.GetYesNo() && !DisplayInFillSpace.GetYesNo();


	if (!UserDrawn && sc.UserDrawnChartDrawingExists(sc.ChartNumber,CountDownTimerLineNumber) > 0)
		sc.DeleteUserDrawnACSDrawing(sc.ChartNumber, CountDownTimerLineNumber);

	s_UseTool Tool;
	Tool.ChartNumber = sc.ChartNumber;
	Tool.DrawingType = DRAWING_TEXT;
	Tool.Region = sc.GraphRegion;
	Tool.LineNumber = CountDownTimerLineNumber;
	Tool.AddMethod = UTAM_ADD_OR_ADJUST;
	Tool.AddAsUserDrawnDrawing = UserDrawn ? 1 : 0;

	bool CreateNeeded = !UserDrawn || sc.UserDrawnChartDrawingExists(sc.ChartNumber, CountDownTimerLineNumber) == 0;

	if (CreateNeeded)
	{
		if(!DisplayInFillSpace.GetYesNo() )
			Tool.BeginDateTime = HorizontalPosition.GetInt();
		else
			Tool.BeginDateTime = -3;

		if (Tool.Region == 0 && UseCurrentPriceForVP.GetYesNo() != 0)
		{
			int ValueIndex = sc.ArraySize - 1;

			Tool.BeginValue = sc.Close[ValueIndex];
			Tool.UseRelativeValue = false;
		}
		else
		{
			Tool.BeginValue = (float)VerticalPosition.GetInt();
			Tool.UseRelativeValue = true;
		}

	}

	Tool.Color = Countdown.PrimaryColor;
	Tool.FontBackColor = Countdown.SecondaryColor;
	if (TransparentLabelBackground.GetYesNo() != 0)
		Tool.TransparentLabelBackground = 1; 
	Tool.ReverseTextColor = false;
	Tool.FontBold = true;
	Tool.FontSize = Countdown.LineWidth;


	//Continuous time countdown.
	if (DisplayTimeCountdown.GetYesNo() && sc.AreTimeSpecificBars() && sc.IsReplayRunning() == 0)
	{
		SCDateTime BarEndingDateTime = sc.BaseDateTimeIn[sc.ArraySize-1]+sc.SecondsPerBar*SECONDS;
		SCDateTime RemainingTime = BarEndingDateTime - sc.CurrentSystemDateTime;

		if (RemainingTime < 0.0)
			RemainingTime = 0;

		SCString StrRemainingTime;

		int Hour, Minute, Second;
		RemainingTime.GetTimeHMS(Hour, Minute, Second);

		if (Hour && Minute)
			StrRemainingTime.Format("%d:%02d:%02d",Hour, Minute, Second);
		else if (Minute)
			StrRemainingTime.Format("%d:%02d", Minute, Second);
		else
			StrRemainingTime.Format("%02d", Second);

		Tool.Text = StrRemainingTime;
		

		//Enable update always so our real-time countdown is continuously visible.
		if(RemainingTime == 0.0)
			sc.UpdateAlways = 0;
		else
			sc.UpdateAlways= 1;

	}
	else if(sc.AreRangeBars() && DisplayRangeInTicks.GetYesNo())
	{
		SCString CountDownText(sc.GetCountDownText());
		const char* CharPosition = strchr(CountDownText.GetChars(),':');
		int RangeInTicks = 0;
		if (CharPosition)
		{
			float RR = (float)atof(CharPosition+1);
			RangeInTicks = (int)(RR/sc.TickSize+0.5f);
			Tool.Text.Format("%d ",RangeInTicks);
		}
		else
			Tool.Text = "";
		
	}
	else if(DisplayValueOnly.GetYesNo())
	{
		SCString CountDownText(sc.GetCountDownText());
		const char* CharPosition = strchr(CountDownText.GetChars(),':');
		if (CharPosition)
			Tool.Text.Format("%s ",CharPosition+1);
		else
			Tool.Text = "";
	}
	else 
		Tool.Text.Format("%s ",sc.GetCountDownText().GetChars());
	
	
	sc.UseTool(Tool);

	if (! sc.DownloadingHistoricalData &&  PriorArraySize < sc.ArraySize && sc.UpdateStartIndex != 0 && AlertOnNewBar.GetYesNo())
		sc.PlaySound( sc.SelectedAlertSound, "Bar has completed", 0);



	PriorArraySize = sc.ArraySize;
}


/*============================================================================*/
SCSFExport scsf_AccountBalanceExternalService(SCStudyInterfaceRef sc)
{
	SCSubgraphRef AccountBalance = sc.Subgraph[0];

	SCInputRef HorizontalPosition = sc.Input[0];
	SCInputRef VerticalPosition = sc.Input[1];
	SCInputRef DisplayInFillSpace = sc.Input[2];
	SCInputRef TransparentLabelBackground = sc.Input[4];
	SCInputRef UseCurrentPriceForVP = sc.Input[5];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Account Balance Text - External Service";

		sc.AutoLoop = 0;
		sc.GraphRegion = 0;

		AccountBalance.Name = "Cash Balance";
		AccountBalance.LineWidth = 20;
		AccountBalance.DrawStyle = DRAWSTYLE_CUSTOM_TEXT;
		AccountBalance.PrimaryColor = RGB(0, 0, 0); //black
		AccountBalance.SecondaryColor = RGB(255, 127, 0); //Orange
		AccountBalance.SecondaryColorUsed = true;
		AccountBalance.DisplayNameValueInWindowsFlags = 0;

		HorizontalPosition.Name = "Initial Horizontal Position From Left (1-150)";
		HorizontalPosition.SetInt(20);
		HorizontalPosition.SetIntLimits(1,150);

		VerticalPosition.Name = "Initial Vertical Position From Bottom (1-100)";
		VerticalPosition.SetInt(90);
		VerticalPosition.SetIntLimits(1,100);

		DisplayInFillSpace.Name = "Display in Fill Space";
		DisplayInFillSpace.SetYesNo(false);

		TransparentLabelBackground.Name = "Transparent Label Background";
		TransparentLabelBackground.SetYesNo(false);

		UseCurrentPriceForVP.Name = "Use Current Price For Vertical Position";
		UseCurrentPriceForVP.SetYesNo(false);

		// During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = false;

		return;
	}

	static const int AccountBalanceLineNumber = 407812335;

	if (sc.LastCallToFunction)
	{
		// be sure to delete user drawn type drawing when study removed
		sc.DeleteUserDrawnACSDrawing(sc.ChartNumber, AccountBalanceLineNumber);
		return;
	}
	 int & IndexOfLastUsedSubgraphElementBalanceData= sc.PersistVars->Integers[0];
	
	
	 AccountBalance[ IndexOfLastUsedSubgraphElementBalanceData ] = 0;

	//If these inputs change after the study has been added, then we need to possibly remove the prior drawing that was marked as user drawn
	bool UserDrawn = sc.GraphRegion == 0 && !DisplayInFillSpace.GetYesNo() && !UseCurrentPriceForVP.GetYesNo();

	s_UseTool Tool;
	Tool.ChartNumber = sc.ChartNumber;
	Tool.DrawingType = DRAWING_TEXT;
	Tool.Region = sc.GraphRegion;
	Tool.LineNumber = AccountBalanceLineNumber;
	Tool.AddMethod = UTAM_ADD_OR_ADJUST;
	Tool.AddAsUserDrawnDrawing = UserDrawn ? 1 : 0;

	bool SetPosition = !UserDrawn || sc.UserDrawnChartDrawingExists(sc.ChartNumber, AccountBalanceLineNumber) == 0;

	if (SetPosition)
	{
		if(!DisplayInFillSpace.GetYesNo())
			Tool.BeginDateTime = HorizontalPosition.GetInt();
		else
			Tool.BeginDateTime = -3;

		if (Tool.Region == 0 && UseCurrentPriceForVP.GetYesNo() != 0)
		{
			int ValueIndex = sc.ArraySize - 1;

			Tool.BeginValue = sc.Close[ValueIndex];
			Tool.UseRelativeValue = false;
		}
		else
		{
			Tool.BeginValue = (float)VerticalPosition.GetInt();
			Tool.UseRelativeValue = true;
		}

	}

	Tool.Color = AccountBalance.PrimaryColor;
	Tool.FontBackColor = AccountBalance.SecondaryColor;
	if (TransparentLabelBackground.GetYesNo() != 0)
		Tool.TransparentLabelBackground = 1; 

	Tool.ReverseTextColor = false;
	Tool.FontBold = true;
	Tool.FontSize = AccountBalance.LineWidth;

	Tool.Text.Format("%.2f", sc.TradeServiceAccountBalance);

	sc.UseTool(Tool);
	
	AccountBalance[sc.ArraySize - 1] =  (float)sc.TradeServiceAccountBalance;
	IndexOfLastUsedSubgraphElementBalanceData = sc.ArraySize - 1;

}


/*==========================================================================*/
SCSFExport scsf_AccountBalanceGraph(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Balance = sc.Subgraph[0];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Account Balance Graph - External Service";

		sc.StudyDescription = "";

		sc.AutoLoop = 1;
		sc.GraphRegion = 1;

		// During development set this flag to 1, so the DLL can be modified. When development is done, set it to 0 to improve performance.
		sc.FreeDLL = 0;

		Balance.Name = "Balance";
		Balance.DrawStyle = DRAWSTYLE_LINE;
		Balance.DrawZeros = false;  // false
		Balance.PrimaryColor = RGB(0, 255, 0);  // Green

		return;
	}


	// Do data processing

	Balance[sc.Index] = (float)sc.TradeServiceAccountBalance;
}


/**************************************************/
SCSFExport scsf_WildersMovingAverage(SCStudyInterfaceRef sc)
{
	SCSubgraphRef MovAvg = sc.Subgraph[0];

	SCInputRef InputData = sc.Input[0];
	SCInputRef Length = sc.Input[1];

	// Set configuration variables	
	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Moving Average-Welles Wilders";

		sc.StudyDescription = "";

		// Set the region to draw the graph in.  Region zero is the main
		// price graph region.
		sc.GraphRegion = 0;

		

		// Set the name of the first subgraph
		MovAvg.Name = "MovAvg";

		// Set the color and style of the graph line.  If these are not set
		// the default will be used.
		MovAvg.PrimaryColor = RGB(255,0,0);  // Red
		MovAvg.DrawStyle = DRAWSTYLE_LINE;
		MovAvg.DrawZeros = false;


		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST); // default data field		

		// Make the Length input and default it to 5
		Length.Name = "Length";
		Length.SetInt(10);
		Length.SetIntLimits(1,MAX_STUDY_LENGTH);

		sc.AutoLoop = 1;

		//During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;

		
		return;
	}

	// Do data processing		
	
	sc.WildersMovingAverage(sc.BaseDataIn[InputData.GetInputDataIndex()], MovAvg, sc.Index, Length.GetInt());
}



/**************************************************/
SCSFExport scsf_BarNumbering(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Number = sc.Subgraph[0];
	
	SCInputRef DisplayAtBottom = sc.Input[0];
	SCInputRef TextOffset = sc.Input[1];
	SCInputRef NumberingEveryNBars = sc.Input[2];
	SCInputRef NumberOfBars = sc.Input[3];

	// Set configuration variables	
	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Bar Numbering";

		sc.GraphRegion = 0;

		// Set the name of the first subgraph
		Number.Name = "Number";
		Number.DrawStyle = DRAWSTYLE_CUSTOM_TEXT;
		Number.PrimaryColor = RGB(0,255,0);
		Number.SecondaryColor = RGB(0,0,0);
		Number.LineWidth= 8;
		Number.SecondaryColorUsed = 1;

		DisplayAtBottom.Name = "Display At Bottom";
		DisplayAtBottom.SetYesNo(false);

		TextOffset.Name = "Text Labels Percentage Offset (1 = 1%)";
		TextOffset.SetFloat(5.0f); 

		NumberingEveryNBars.Name = "Number Every N Bars";
		NumberingEveryNBars.SetInt(2);
		NumberingEveryNBars.SetIntLimits(1,MAX_STUDY_LENGTH);

		NumberOfBars.Name ="Number Of Bars to Label";
		NumberOfBars.SetInt(1000);

		sc.AutoLoop = 1;

		//During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;

		
		return;
	}

	if (sc.HideStudy)
		return;


	// Do data processing	

	if (NumberOfBars.GetInt() == 0)
		NumberOfBars.SetInt(1000);

	int &BarCounter = sc.PersistVars->i1;

	int &LastProcessedBarIndex = sc.PersistVars->i2;

	if(sc.Index < (sc.ArraySize - NumberOfBars.GetInt() ) )
	{
		BarCounter = 1;
		LastProcessedBarIndex = -1;
		return;
	}
	
	if (sc.Index == 0)
	{
		BarCounter = 1;
		LastProcessedBarIndex = -1;
	}
	else if(sc.Index != LastProcessedBarIndex)
	{
		//reset counter if a new session has begun.
		SCDateTime StartDateTimeOfDay;

		StartDateTimeOfDay.SetTime(sc.StartTimeOfDay);

		StartDateTimeOfDay.SetDate(sc.BaseDateTimeIn[sc.Index].GetDate());

		if(sc.BaseDateTimeIn[sc.Index] == StartDateTimeOfDay ||
		   (sc.BaseDateTimeIn[sc.Index-1] < StartDateTimeOfDay && sc.BaseDateTimeIn[sc.Index] > StartDateTimeOfDay)
		  )
			BarCounter = 1;
		else
			BarCounter++; //increment counter 
	}

	LastProcessedBarIndex = sc.Index;

	Number[sc.Index] = (float)BarCounter;

	if(BarCounter != 1 && BarCounter % NumberingEveryNBars.GetInt())
		return;

	float VerticalPositionValue;

	float Offset = TextOffset.GetFloat() *0.01f * (sc.High[sc.Index ]-sc.Low[sc.Index ]);
	VerticalPositionValue =sc.Low[sc.Index] - Offset;
	

	const int UniqueID = 1210200915;
	s_UseTool Tool;
	Tool.Clear();
	Tool.DrawingType = DRAWING_TEXT;

	Tool.ChartNumber = sc.ChartNumber;
	Tool.Region = sc.GraphRegion;

	if (sc.Index == sc.ArraySize - 1)
		Tool.LineNumber = UniqueID + 1;
	else
		Tool.LineNumber = UniqueID;

	Tool.BeginIndex =sc.Index;

	if (DisplayAtBottom.GetYesNo())
	{
		Tool.UseRelativeValue= true;
		Tool.BeginValue = 5;
	}
	else
		Tool.BeginValue = VerticalPositionValue;

	Tool.Color = Number.PrimaryColor;
	Tool.FontBackColor = Number.SecondaryColor;
	Tool.FontSize = Number.LineWidth;
	Tool.TextAlignment = DT_CENTER | DT_TOP;

	if (sc.Index == sc.ArraySize - 1)
		Tool.AddMethod = UTAM_ADD_OR_ADJUST;
	else
		Tool.AddMethod = UTAM_ADD_ALWAYS;

	Tool.Text.Format("%d", BarCounter);
	
	sc.UseTool(Tool);

	
}


/***********************************************************************/

SCSFExport scsf_StudySubgraphsReference(SCStudyInterfaceRef sc)
{
	SCSubgraphRef PriceOverlay1 = sc.Subgraph[0];
	SCSubgraphRef PriceOverlay2 = sc.Subgraph[1];


	SCInputRef StudySugbraph1 = sc.Input[0];
	SCInputRef StudySugbraph2 = sc.Input[1];


	// Set configuration variables

	if (sc.SetDefaults)
	{
		sc.GraphName = "Study Subgraphs Reference";

		
		sc.GraphRegion = 1;

		sc.ScaleRangeType = SCALE_INDEPENDENT;

		PriceOverlay1.Name = "Study Subgraph 1";
		PriceOverlay1.DrawStyle = DRAWSTYLE_LINE;
		PriceOverlay1.PrimaryColor = RGB(0,255,0);
		PriceOverlay1.DrawZeros = true;

		PriceOverlay2.Name = "Study Subgraph2";
		PriceOverlay2.DrawStyle = DRAWSTYLE_LINE;
		PriceOverlay2.PrimaryColor = RGB(255,0,255);
		PriceOverlay2.DrawZeros = true;

		StudySugbraph1.Name = "Input Study 1";
		StudySugbraph1.SetStudySubgraphValues(0, 0);

		StudySugbraph2.Name = "Input Study 2";
		StudySugbraph2.SetStudySubgraphValues(0, 0);

		sc.CalculationPrecedence = LOW_PREC_LEVEL;


		//During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;

		sc.AutoLoop = 1;

		return;
	}


	// Do data processing

	// Get the array for the specified Input Data from the specified studies
	SCFloatArray Study1Array;
	sc.GetStudyArrayUsingID(StudySugbraph1.GetStudyID(),StudySugbraph1.GetSubgraphIndex(),Study1Array);

	SCFloatArray Study2Array;
	sc.GetStudyArrayUsingID(StudySugbraph2.GetStudyID(),StudySugbraph2.GetSubgraphIndex(),Study2Array);

	PriceOverlay1[sc.Index] = Study1Array[sc.Index];
	PriceOverlay2[sc.Index] = Study2Array[sc.Index];
}




/*----------------------------------------------------------------------*/


SCSFExport scsf_VolumeCloseToMidpointColoring(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Volume = sc.Subgraph[0];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Volume - Close To Midpoint Colored";

		sc.ValueFormat = 0;

		sc.AutoLoop = true;
		sc.ScaleRangeType = SCALE_ZEROBASED;

		Volume.Name = "Volume";
		Volume.DrawStyle = DRAWSTYLE_BAR;
		Volume.PrimaryColor = RGB(0, 255, 0);
		Volume.SecondaryColor = RGB(255, 0, 0);
		Volume.SecondaryColorUsed = 1;
		Volume.LineWidth = 2;
		Volume.DrawZeros = false;


		sc.DisplayStudyInputValues = false;

		return;
	}


	Volume[sc.Index] = sc.Volume[sc.Index];


	if(sc.Close[sc.Index] >=  sc.HLAvg[sc.Index])
		Volume.DataColor[sc.Index] = Volume.PrimaryColor;
	else
		Volume.DataColor[sc.Index] = Volume.SecondaryColor;

}
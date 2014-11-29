#include "sierrachart.h"

SCSFExport scsf_StochasticCrossover(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Buy = sc.Subgraph[0];
	SCSubgraphRef Sell = sc.Subgraph[1];
	SCSubgraphRef FastD = sc.Subgraph[2];
	SCFloatArrayRef SlowK = FastD.Arrays[0];
	SCFloatArrayRef SlowD = FastD.Arrays[1];

	SCInputRef InputFastK = sc.Input[0];
	SCInputRef InputFastD = sc.Input[1];
	SCInputRef InputSlowD = sc.Input[2];
	SCInputRef Line1 = sc.Input[3];
	SCInputRef Line2 = sc.Input[4];
	SCInputRef DoNotUseBuySell = sc.Input[5];
	SCInputRef ArrowOffsetPercentage = sc.Input[6];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults
		
		sc.GraphName = "Stochastic Crossover System";
		sc.StudyDescription = "This is an example of a Stochastic Crossover Study System.";

		Buy.Name = "Buy";
		Buy.PrimaryColor = RGB(0, 255, 0);	// green
		Buy.DrawStyle = DRAWSTYLE_ARROWUP;
		Buy.LineWidth = 2;	//Width of arrow

		Sell.Name = "Sell";
		Sell.DrawStyle = DRAWSTYLE_ARROWDOWN;
		Sell.PrimaryColor = RGB(255, 0, 0);	// red
		Sell.LineWidth = 2; //Width of arrow

		InputFastK.Name = "Fast %K Length";
		InputFastK.SetInt(10);

		InputFastD.Name = "Fast %D Length";
		InputFastD.SetInt(3);

		InputSlowD.Name = "Slow %D Length";
		InputSlowD.SetInt(3);

		Line1.Name = "Line 1 Value";
		Line1.SetFloat(70);

		Line2.Name = "Line 2 Value";
		Line2.SetFloat(30);

		DoNotUseBuySell.Name = "Do Not Use Buy/Sell Lines";
		DoNotUseBuySell.SetYesNo(false);

		ArrowOffsetPercentage.Name = "Arrow Offset Percentage";
		ArrowOffsetPercentage.SetInt(3);
			 

		sc.GraphRegion = 0; //Main chart region
		sc.DrawZeros= 0;

		sc.AutoLoop = 1; 
		
		return;
	}
	
	// Do data processing

	// The following line calculates our stochastic.
	// The 3 stochastic lines are placed in sc.Subgraph[2].Data, sc.Subgraph[2].Arrays[0] and sc.Subgraph[2].Arrays[1].
	sc.Stochastic(sc.BaseDataIn, FastD, InputFastK.GetInt(), InputFastD.GetInt(), InputSlowD.GetInt(), MOVAVGTYPE_SIMPLE);

	int Index=sc.Index; //Sets the variable Index to our current Bar index.

	/* This is used further down to position a buy or sell arrow below or above a price bar so it does not touch exactly the low or high.  This is not necessary.  It is only for appearance purposes.*/
		float Offset=(sc.High[Index] -sc.Low[Index] )*(ArrowOffsetPercentage.GetInt() * 0.01f);


	/*This code is not currently used:

	//Calculate crossing point
	float CrossY = 0.0f;
	float p1x, p2x, p3x, p4x;
	float p1y, p2y, p3y, p4y;

	p1x = p3x = (float)(Index - 1);
	p2x = p4x = (float)(Index);

	if(SlowK[Index] > SlowD[Index])
	{
		p1y = SlowD[Index - 1];
		p2y = SlowD[Index];
		p3y = SlowK[Index - 1];
		p4y = SlowK[Index];
	}
	else
	{
		p1y = SlowK[Index - 1];
		p2y = SlowK[Index];
		p3y = SlowD[Index - 1];
		p4y = SlowD[Index];
	}

	float t
		= ((p4x - p3x) * (p1y - p3y) - (p4y - p3y) * (p1x - p3x)) /
		(float)((p4y - p3y) * (p2x - p1x) - (p4x - p3x) * (p2y - p1y));

	CrossY = p1y + (t * (p2y - p1y));

	*/

	/*If Slow %k crosses Slow %d from the bottom AND %k is below 30*/
	if (sc.CrossOver( SlowK,  SlowD, Index) == CROSS_FROM_BOTTOM && 
		(DoNotUseBuySell.GetYesNo() || SlowK[Index] < Line2.GetFloat()))
	{

		//Place an Up arrow below the low of the current bar. 2% of the range is used to determine the amount of space below the bar.
		Buy[Index] = sc.Low[Index] - Offset;

		/*Make sure we have no down arrow from a prior calculation.  It is important to set this to zero since we perform calculations on the same bar many times while the bar is being updated and we may have had a prior sell signal when we now have a buy signal.*/
		Sell[Index] = 0;
	}

	/*If Slow %k crosses Slow %d from the top AND %k is over 70*/
	else if  (sc.CrossOver(SlowK, SlowD, Index) == CROSS_FROM_TOP && 
		(DoNotUseBuySell.GetYesNo() || SlowK[Index] > Line1.GetFloat()))	
	{
		Sell[Index] = sc.High[Index] + Offset;
		Buy[Index] = 0;
	}
	else
	{
		Buy[Index] = 0;
		Sell[Index] = 0;
	}
}


/************************************************************************/
SCSFExport scsf_MACDCrossoverSystem(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Buy = sc.Subgraph[0];
	SCSubgraphRef Sell = sc.Subgraph[1];
	SCSubgraphRef MACDData = sc.Subgraph[2];


	SCInputRef InputData	= sc.Input[0];
	SCInputRef FastLen		= sc.Input[1];
	SCInputRef SlowLen		= sc.Input[2];
	SCInputRef MACDLen		= sc.Input[3];
	SCInputRef MAType		= sc.Input[4];
	SCInputRef OffsetPercentInput = sc.Input[5]; 



	if (sc.SetDefaults)
	{
		// Set the configuration and defaults
		
		sc.GraphName = "MACD Crossover System";

		sc.GraphRegion = 0; //Main chart region

		sc.AutoLoop = 1; 

		Buy.Name = "Buy";
		Buy.PrimaryColor = RGB(0, 255, 0);	// green
		Buy.DrawStyle = DRAWSTYLE_ARROWUP;
		Buy.LineWidth = 2;	//Width of arrow
		Buy.DrawZeros = 0;

		Sell.Name = "Sell";
		Sell.DrawStyle = DRAWSTYLE_ARROWDOWN;
		Sell.PrimaryColor = RGB(255, 0, 0);	// red
		Sell.LineWidth = 2; //Width of arrow
		Sell.DrawZeros = 0;


		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST); 

		FastLen.Name ="Fast Moving Average Length";
		FastLen.SetInt(12);  
		FastLen.SetIntLimits(1,MAX_STUDY_LENGTH);

		SlowLen.Name = "Slow Moving Average Length";
		SlowLen.SetInt(26);  
		SlowLen.SetIntLimits(1,MAX_STUDY_LENGTH);

		MACDLen.Name = "MACD Moving Average Length";
		MACDLen.SetInt(9);   
		MACDLen.SetIntLimits(1,MAX_STUDY_LENGTH);

		MAType.Name = "Moving Average Type";
		MAType.SetMovAvgType(MOVAVGTYPE_EXPONENTIAL);

		OffsetPercentInput.Name = "Arrow Offset Percentage";
		OffsetPercentInput.SetFloat(8);
		
		return;
	}
	
	// Do data processing	
	sc.MACD(sc.BaseDataIn[InputData.GetInputDataIndex()], MACDData, sc.Index, FastLen.GetInt(), SlowLen.GetInt(), MACDLen.GetInt(), MAType.GetInt()); 

	float Range = (sc.High[sc.Index] - sc.Low[sc.Index]);


	float OffsetPercent = OffsetPercentInput.GetFloat() * 0.01f;

	if (sc.CrossOver( MACDData,  MACDData.Arrays[2]) == CROSS_FROM_BOTTOM)
	{

		Buy[sc.Index] = sc.Low[sc.Index] - OffsetPercent * Range;

		Sell[sc.Index] = 0;
	}
	else if  (sc.CrossOver(MACDData, MACDData.Arrays[2]) == CROSS_FROM_TOP)	
	{
		Sell[sc.Index] = sc.High[sc.Index] + OffsetPercent * Range;
		Buy[sc.Index] = 0;
	}
	else
	{
		Buy[sc.Index] = 0;
		Sell[sc.Index] = 0;
	}
}

/************************************************************************/


SCSFExport scsf_MACDZeroCrossOverSystem(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Buy = sc.Subgraph[0];
	SCSubgraphRef Sell = sc.Subgraph[1];
	SCSubgraphRef MACDData = sc.Subgraph[2];


	SCInputRef InputData	= sc.Input[0];
	SCInputRef FastLen		= sc.Input[1];
	SCInputRef SlowLen		= sc.Input[2];
	SCInputRef MACDLen		= sc.Input[3];
	SCInputRef MAType		= sc.Input[4];
	SCInputRef OffsetPercentInput = sc.Input[5]; 



	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "MACD Zero Cross Over System";

		sc.GraphRegion = 0; //Main chart region

		sc.AutoLoop = 1; 

		Buy.Name = "Buy";
		Buy.PrimaryColor = RGB(0, 255, 0);	// green
		Buy.DrawStyle = DRAWSTYLE_ARROWUP;
		Buy.LineWidth = 2;	//Width of arrow
		Buy.DrawZeros = 0;

		Sell.Name = "Sell";
		Sell.DrawStyle = DRAWSTYLE_ARROWDOWN;
		Sell.PrimaryColor = RGB(255, 0, 0);	// red
		Sell.LineWidth = 2; //Width of arrow
		Sell.DrawZeros = 0;


		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST); 

		FastLen.Name ="Fast Moving Average Length";
		FastLen.SetInt(12);  
		FastLen.SetIntLimits(1,MAX_STUDY_LENGTH);

		SlowLen.Name = "Slow Moving Average Length";
		SlowLen.SetInt(26);  
		SlowLen.SetIntLimits(1,MAX_STUDY_LENGTH);

		MACDLen.Name = "MACD Moving Average Length";
		MACDLen.SetInt(9);   
		MACDLen.SetIntLimits(1,MAX_STUDY_LENGTH);

		MAType.Name = "Moving Average Type";
		MAType.SetMovAvgType(MOVAVGTYPE_EXPONENTIAL);

		OffsetPercentInput.Name = "Arrow Offset Percentage";
		OffsetPercentInput.SetFloat(8);

		return;
	}

	// Do data processing	
	sc.MACD(sc.BaseDataIn[InputData.GetInputDataIndex()], MACDData, sc.Index, FastLen.GetInt(), SlowLen.GetInt(), MACDLen.GetInt(), MAType.GetInt()); 

	float Range = (sc.High[sc.Index] - sc.Low[sc.Index]);


	float OffsetPercent = OffsetPercentInput.GetFloat() * 0.01f;

	if (sc.CrossOver( MACDData.Arrays[8],  MACDData.Arrays[2]) == CROSS_FROM_TOP)
	{

		Buy[sc.Index] = sc.Low[sc.Index] - OffsetPercent * Range;

		Sell[sc.Index] = 0;
	}
	else if  (sc.CrossOver( MACDData.Arrays[8], MACDData.Arrays[2]) == CROSS_FROM_BOTTOM)	
	{
		Sell[sc.Index] = sc.High[sc.Index] + OffsetPercent * Range;
		Buy[sc.Index] = 0;
	}
	else
	{
		Buy[sc.Index] = 0;
		Sell[sc.Index] = 0;
	}
}


/************************************************************************/
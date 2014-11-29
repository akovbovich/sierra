#include "sierrachart.h"

#include "CandleStickPatternNames.h"


/*==================================================================================*/
SCSFExport scsf_WoodieCCITrend2(SCStudyInterfaceRef sc)
{
	SCSubgraphRef CCI = sc.Subgraph[0];
	SCSubgraphRef TrendDown = sc.Subgraph[1];
	SCSubgraphRef TrendNeutral = sc.Subgraph[2];
	SCSubgraphRef TrendUp = sc.Subgraph[3];
	SCSubgraphRef HiLevel = sc.Subgraph[4];
	SCSubgraphRef LowLevel = sc.Subgraph[5];
	SCSubgraphRef Consecutive = sc.Subgraph[6];
	SCSubgraphRef LastTrend = sc.Subgraph[7];
	SCSubgraphRef WorksheetOutput = sc.Subgraph[8];
	SCSubgraphRef ZLR = sc.Subgraph[9];

	SCInputRef NumberOfBars = sc.Input[0];
	SCInputRef NeutralBars = sc.Input[1];
	SCInputRef CCILength = sc.Input[2];
	SCInputRef Level = sc.Input[3];
	SCInputRef CCIInputData = sc.Input[4];
	SCInputRef Version = sc.Input[5];

    if(sc.SetDefaults)
    {
        sc.GraphName="Woodies CCI Trend";

        sc.AutoLoop = 1;

        TrendDown.Name = "TrendDown";
        TrendDown.DrawStyle = DRAWSTYLE_BAR;
        TrendDown.PrimaryColor = RGB(255,0,0);
		TrendDown.DrawZeros = false;

		TrendNeutral.Name = "TrendNeutral";
        TrendNeutral.PrimaryColor = RGB(255,255,0);
        TrendNeutral.DrawStyle = DRAWSTYLE_BAR;
		TrendNeutral.DrawZeros = false;

        TrendUp.Name = "TrendUp";
		TrendUp.DrawStyle = DRAWSTYLE_BAR;
        TrendUp.PrimaryColor = RGB(0,0,255);
		TrendUp.DrawZeros = false;

		HiLevel.Name = "Hi Level";
		HiLevel.DrawStyle = DRAWSTYLE_LINE;
        HiLevel.PrimaryColor = RGB(0,255,0);
		HiLevel.DrawZeros = false;

		LowLevel.Name = "Low Level";
		LowLevel.DrawStyle = DRAWSTYLE_LINE;
        LowLevel.PrimaryColor = RGB(0,255,0);
        LowLevel.DrawZeros = false;

        CCI.Name = "CCI";
        CCI.DrawStyle = DRAWSTYLE_BAR;
		CCI.PrimaryColor = RGB(98,98,98);
		CCI.DrawZeros = false;

		Consecutive.DrawStyle = DRAWSTYLE_IGNORE;
		Consecutive.DrawZeros = false;

		LastTrend.DrawStyle = DRAWSTYLE_IGNORE;
		LastTrend.DrawZeros = false;

		WorksheetOutput.Name = "Spreadsheet Ouput";
		WorksheetOutput.DrawStyle = DRAWSTYLE_IGNORE;
		WorksheetOutput.PrimaryColor = RGB(0,127,255);
		WorksheetOutput.DrawZeros = false;

		ZLR.Name = "ZLR Output";
		ZLR.DrawStyle = DRAWSTYLE_POINT;
		ZLR.PrimaryColor = RGB(255,0,0);
		ZLR.SecondaryColor = RGB(0,255,0);
		ZLR.SecondaryColorUsed = 1;
		ZLR.LineWidth = 3;
		ZLR.DrawZeros = false;

	    CCILength.Name = "CCI Length";
        CCILength.SetInt(14);
        CCILength.SetIntLimits(1,MAX_STUDY_LENGTH);

        NumberOfBars.Name = "Number of Bars";
        NumberOfBars.SetInt(6);
        NumberOfBars.SetIntLimits(1,MAX_STUDY_LENGTH);
		
		NeutralBars.Name = "Number of Neutral Bars";
        NeutralBars.SetInt(5);
        NeutralBars.SetIntLimits(1,MAX_STUDY_LENGTH);

        Level.Name = "Level";
        Level.SetInt(100);
        Level.SetIntLimits(1,MAX_STUDY_LENGTH);

		CCIInputData.Name = "Input Data";
		CCIInputData.SetInputDataIndex(SC_HLC);
		
		Version.SetInt(1);
		
        return;
    }


	
	
	//Current number of consecutive conditions CCI > 0 (or CCI < 0 if variable less zero)
	float& consec = Consecutive[sc.Index];
	const float& PrevConsec = Consecutive[sc.Index-1];
	
	float& currTrend= LastTrend[sc.Index]; // -1 - TrendDown; 1 - TrendUp
	float& lastTrend= LastTrend[sc.Index-1]; 

	const float& currCCI = CCI[sc.Index];
	int OccurGreatEdge=0;
	int OccurLessEdge=0;

	HiLevel[sc.Index] = (float)Level.GetInt();
	LowLevel[sc.Index] = (float)-Level.GetInt();

	sc.DataStartIndex= CCILength.GetInt()-1;

	sc.CCI(sc.BaseData[CCIInputData.GetInputDataIndex()], CCI, CCILength.GetInt(), 0.015f);
	
	if(sc.Index == 0)
	{	
		if(CCI[sc.Index] > 0)
			consec = 1;
		else if(CCI[sc.Index] < 0)
			consec = -1;
		else
			consec = 0;
	}
	else
	{
		if(currCCI > 0)
		{
			if(PrevConsec < 0) 
				consec = 1;
			else
				consec = PrevConsec + 1;
		}
		else if(currCCI < 0)
		{
			if(PrevConsec > 0) 
				consec = -1;
			else
				consec = PrevConsec - 1;
		}
	}
	
	for(int i = sc.Index; i>sc.Index-NumberOfBars.GetInt(); i--)
	{
		if(CCI[i] > Level.GetInt())
			OccurGreatEdge++;
		else if(CCI[i] < -Level.GetInt())
			OccurLessEdge++;
	}

	bool trendUp = (currCCI > 0) && ((lastTrend == 1)  || ((consec >= NumberOfBars.GetInt()) && (OccurGreatEdge > 0)));
	bool trendDown = (currCCI < 0) && ((lastTrend == -1) || ((consec <= -NumberOfBars.GetInt() ) && (OccurLessEdge > 0)));

	//Zero out subgraphs that color the main subgraph. This needs to be done in case one of these conditions is no longer met when a bar is forming.
	TrendUp[sc.Index] = 0;
	TrendDown[sc.Index] = 0;
	TrendNeutral[sc.Index] = 0;
	WorksheetOutput[sc.Index] = 0;

	if(trendUp)
	{
		TrendUp[sc.Index] = currCCI;//trend up
		currTrend = 1;
		WorksheetOutput[sc.Index] = 3;
	}
	else if(trendDown)
	{
		TrendDown[sc.Index] = currCCI;//trend down
		currTrend = -1;
		WorksheetOutput[sc.Index] = 1;
	}
	else 
	{	
		currTrend = lastTrend;	
		if( (consec >= NeutralBars.GetInt() && !trendUp) || ( -consec >= NeutralBars.GetInt() && !trendDown) )
			TrendNeutral[sc.Index] = currCCI;//trend neutral
		WorksheetOutput[sc.Index] = 2;
	}

	//Calculate the ZLR
	SCFloatArrayRef ZLRHooks = ZLR.Arrays[0]; // First extra array of ZLR subgraph

	if ((CCI[sc.Index-1] > CCI[sc.Index-2] && CCI[sc.Index] < CCI[sc.Index-1])
		||  (ZLRHooks[sc.Index-1] < 0 && CCI[sc.Index] <= CCI[sc.Index-1])
		)
	{
		ZLRHooks[sc.Index] = -1;
		if (ZLRHooks [sc.Index] <0 && CCI [sc.Index] <0)
		{
			ZLR[sc.Index] = -200;
			ZLR.DataColor[sc.Index] = ZLR.PrimaryColor;
		}
		else
			ZLR [sc.Index] = 0;

	}
	else if ((CCI[sc.Index-1]  < CCI[sc.Index-2] && CCI[sc.Index] > CCI[sc.Index-1])
		||  (ZLRHooks[sc.Index-1] > 0 && CCI[sc.Index] >= CCI[sc.Index-1])
		)
	{
		ZLRHooks[sc.Index] = 1;
		if (ZLRHooks [sc.Index] >0 && CCI [sc.Index] >0)
		{
			ZLR[sc.Index] = 200;
			ZLR.DataColor[sc.Index] = ZLR.SecondaryColor;
		}
		else
			ZLR [sc.Index] = 0;

	}
	else
	{
		ZLR[sc.Index] = 0;
		ZLRHooks[sc.Index] = 0;
		
	}
}

/*============================================================================*/


SCSFExport scsf_CCIPredictor(SCStudyInterfaceRef sc)
{
	SCSubgraphRef CCIProjHigh = sc.Subgraph[0];
	SCSubgraphRef CCIProjLow = sc.Subgraph[1];
	SCSubgraphRef CCIOutputHigh = sc.Subgraph[2];
	SCSubgraphRef CCIOutputLow = sc.Subgraph[3];


	SCInputRef CCILength = sc.Input[0];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "CCI Predictor";

		sc.AutoLoop = 1;
		sc.GraphRegion = 1;

		CCIProjHigh.Name = "CCI Proj High";
		CCIProjHigh.DrawStyle = DRAWSTYLE_ARROWDOWN;
		CCIProjHigh.PrimaryColor = RGB(255,255,255);
		CCIProjHigh.LineWidth = 3;
		CCIProjHigh.DrawZeros = false;

		CCIProjLow.Name = "CCI Proj Low";
		CCIProjLow.DrawStyle = DRAWSTYLE_ARROWUP;
		CCIProjLow.PrimaryColor = RGB(255,255,255);
		CCIProjLow.LineWidth = 3;
		CCIProjLow.DrawZeros = false;

		CCILength.Name = "CCI Length";
		CCILength.SetInt(14);

		return;
	}


	//CCI High
	CCIProjHigh.Arrays[0][sc.Index] = sc.HLCAvg[sc.Index];

	if (sc.Index == sc.ArraySize - 1 && sc.AreRangeBars())
	{
		float ProjectedRangeHigh=sc.Low[sc.Index]+sc.RangeBarValue;
		CCIProjHigh.Arrays[0][sc.Index] = ( ProjectedRangeHigh + ProjectedRangeHigh +sc.Low[sc.Index])/3;

	}

	sc.CCI(CCIProjHigh.Arrays[0],  CCIOutputHigh, sc.Index, CCILength.GetInt(), 0.015f);

	if (sc.Index == sc.ArraySize - 1)
		CCIProjHigh[sc.Index] =  CCIOutputHigh[sc.Index];
	else 
		CCIProjHigh[sc.Index] = 0;



	//CCI Low
	CCIProjLow.Arrays[0][sc.Index] = sc.HLCAvg[sc.Index];
	if (sc.Index == sc.ArraySize - 1 && sc.AreRangeBars())
	{
		float ProjectedRangeLow=sc.High[sc.Index]-sc.RangeBarValue;
		CCIProjLow.Arrays[0][sc.Index] = ( ProjectedRangeLow + ProjectedRangeLow +sc.High[sc.Index])/3;


	}

	sc.CCI(CCIProjLow.Arrays[0],  CCIOutputLow, sc.Index, CCILength.GetInt(), 0.015f);

	if (sc.Index == sc.ArraySize - 1)
		CCIProjLow[sc.Index] = CCIOutputLow[sc.Index];
	else 
		CCIProjLow[sc.Index] = 0;


}
/*============================================================================*/
 //////////////////////////////////////////////////////////////////////////////////////////////
 //  Candle Pattern Finder
 //////////////////////////////////////////////////////////////////////////////////////////////
 

struct s_CandleStickPatternsFinderSettings
{
    int PriceRangeNumberOfBars;
    double	PriceRangeMultiplier;
	int UseTrendDetection;
    
    s_CandleStickPatternsFinderSettings()
    {
        // default values
        PriceRangeNumberOfBars	= 100;
        PriceRangeMultiplier		= 0.01;
		UseTrendDetection = true;
    }
};

 // Candle dimensions

 inline double CandleLength(SCBaseDataRef InData, int index);
 inline double BodyLength(SCBaseDataRef InData, int index);
 inline double UpperWickLength(SCBaseDataRef InData, int index);
 inline double LowerWickLength(SCBaseDataRef InData, int index);

 // Helper functions

 int  DetermineTrendForCandlestickPatterns(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& PatternsFinderSettings, int index,int num_of_candles);

 inline bool IsBodyStrong(SCBaseDataRef InData, int index);
 inline bool IsCandleStrength(SCBaseDataRef InData, int index);

 inline bool IsDoji(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index);

 inline bool IsBodyStrong(SCBaseDataRef InData, int index);
 inline bool IsCandleStrength(SCBaseDataRef InData, int index);
 
 inline bool IsWhiteCandle(SCBaseDataRef InData, int index);
 inline bool IsBlackCandle(SCBaseDataRef InData, int index);

 inline double PercentOfCandleLength(SCBaseDataRef InData, int index, double percent);
 inline double PercentOfBodyLength(SCBaseDataRef InData, int index, double percent);

 inline bool IsUpperWickSmall(SCBaseDataRef InData, int index, double percent);
 inline bool IsLowerWickSmall(SCBaseDataRef InData, int index, double percent);

 inline bool IsNearEqual(double value1, double value2, SCBaseDataRef InData, int index, double percent);

 // Formations

 bool IsHammer(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index);
 bool IsHangingMan(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index);
 bool IsBullishEngulfing(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index);
 bool IsBearishEngulfing(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index);
 bool IsDarkCloudCover(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index);
 bool IsPiercingLine(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index);
 bool IsMorningStar(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index);
 bool IsEveningStar(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index);
 bool IsMorningDojiStar(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index);
 bool IsEveningDojiStar(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index);
 bool IsBullishAbandonedBaby(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index);
 bool IsBearishAbandonedBaby(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index);
 bool IsShootingStar(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index);
 bool IsInvertedHammer(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index);
 bool IsBearishHarami(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index);
 bool IsBullishHarami(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index);
 bool IsBearishHaramiCross(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index);
 bool IsBullishHaramiCross(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index);
 bool IsTweezerTop(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index);
 bool IsTweezerBottom(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index);
 bool IsBearishBeltHoldLine(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index);
 bool IsBullishBeltHoldLine(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index); 
 bool IsTwoCrows(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index); 
 bool IsThreeBlackCrows(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index); 
 bool IsBearishCounterattackLine(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index); 
 bool IsBullishCounterattackLine(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index); 
 bool IsThreeInsideUp(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index); 
 bool IsThreeOutsideUp(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index); 
 bool IsThreeInsideDown(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index); 
 bool IsThreeOutsideDown(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index);
 bool IsKicker(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index); 
 bool IsKicking(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index); 
 bool IsThreeWhiteSoldiers(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index);
 bool IsAdvanceBlock(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index);
 bool IsDeliberation(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index);  
 bool IsBearishTriStar(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index);
 bool IsBullishTriStar(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index);
 bool IsUniqueThreeRiverBottom(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index);
 bool IsBearishDojiStar(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index);
 bool IsBullishDojiStar(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index);
 bool IsBearishDragonflyDoji(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index);
 bool IsBullishDragonflyDoji(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index);
 bool IsBearishGravestoneDoji(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index);
 bool IsBullishGravestoneDoji(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index);
 bool IsBearishLongleggedDoji(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index);
 bool IsBullishLongleggedDoji(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index);
 bool IsBearishSideBySideWhiteLines(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index);
 bool IsBullishSideBySideWhiteLines(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index);
 bool IsFallingThreeMethods(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index);
 bool IsRisingThreeMethods(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index);
 bool IsBearishSeparatingLines(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index);
 bool IsBullishSeparatingLines(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index);
 bool IsDownsideTasukiGap(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index);
 bool IsUpsideTasukiGap(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index);
 bool IsBearishThreeLineStrike(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index);
 bool IsBullishThreeLineStrike(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index);
 bool IsDownsideGapThreeMethods(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index);
 bool IsUpsideGapThreeMethods(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index);
 bool IsOnNeck(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index);
 bool IsInNeck(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index);
 bool IsBearishThrusting(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index);
 bool IsMatHold(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index);
 
 static const int CANDLESTICK_TREND_FLAT = 0;
 static const int CANDLESTICK_TREND_UP = 1;
 static const int CANDLESTICK_TREND_DOWN = -1;
 static const int TREND_FOR_PATTERNS = 8;

 
 /***********************************************************************/
 SCSFExport scsf_CandleStickPatternsFinder(SCStudyInterfaceRef sc)
 {
	 SCSubgraphRef Pattern1 = sc.Subgraph[0];
	 SCSubgraphRef Pattern2 = sc.Subgraph[1];
	 SCSubgraphRef Pattern3 = sc.Subgraph[2];
	 SCSubgraphRef Pattern4 = sc.Subgraph[3];
	 SCSubgraphRef Pattern5 = sc.Subgraph[4];
	 SCSubgraphRef Pattern6 = sc.Subgraph[5];
	 SCSubgraphRef TrendUp = sc.Subgraph[6];
	 SCSubgraphRef TrendDown = sc.Subgraph[7];
	 SCSubgraphRef TrendForPatterns = sc.Subgraph[TREND_FOR_PATTERNS];

	 SCInputRef InputPattern1 = sc.Input[0];
	 SCInputRef InputPattern2 = sc.Input[1];
	 SCInputRef InputPattern3 = sc.Input[2];
	 SCInputRef InputPattern4 = sc.Input[3];
	 SCInputRef InputPattern5 = sc.Input[4];
	 SCInputRef InputPattern6 = sc.Input[5];
	 SCInputRef UseNumericValue = sc.Input[6];
	 SCInputRef Distance = sc.Input[7];
	 SCInputRef PriceRangeNumberOfBars = sc.Input[8];
	 SCInputRef PriceRangeMultiplier = sc.Input[9];
	 SCInputRef TrendDetection = sc.Input[10];
	 SCInputRef TrendDetectionLength = sc.Input[11];
	 SCInputRef DisplayText = sc.Input[12];

     if (sc.SetDefaults)
     {
         sc.GraphName="CandleStick Patterns Finder";
         sc.StudyDescription="This study looks for candlestick patterns and identifies them on the chart with an abbreviation on the bar where they occur. It uses very advanced logic.";

         sc.AutoLoop = 1;
         sc.GraphRegion = 0;

         Pattern1.Name = "Pattern 1";
         Pattern1.DrawStyle = DRAWSTYLE_CUSTOM_TEXT;
         Pattern1.PrimaryColor = RGB(255, 255, 0);
         Pattern1.SecondaryColor = RGB(0, 0, 0);
         Pattern1.SecondaryColorUsed = TRUE;
         Pattern1.LineWidth = 8;
		 Pattern1.DrawZeros = false;

         Pattern2.Name = "Pattern 2";
         Pattern2.DrawStyle = DRAWSTYLE_IGNORE;
         Pattern2.PrimaryColor = RGB(255, 255, 255);
		 Pattern2.DrawZeros = false;

         Pattern3.Name = "Pattern 3";
         Pattern3.DrawStyle = DRAWSTYLE_IGNORE;
         Pattern3.PrimaryColor = RGB(255, 255, 255);
		 Pattern3.DrawZeros = false;

         Pattern4.Name = "Pattern 4";
         Pattern4.DrawStyle = DRAWSTYLE_IGNORE;
         Pattern4.PrimaryColor = RGB(255, 255, 255);
		 Pattern4.DrawZeros = false;

         Pattern5.Name = "Pattern 5";
         Pattern5.DrawStyle = DRAWSTYLE_IGNORE;
         Pattern5.PrimaryColor = RGB(255, 255, 255);
		 Pattern5.DrawZeros = false;

         Pattern6.Name = "Pattern 6";
         Pattern6.DrawStyle = DRAWSTYLE_IGNORE;
         Pattern6.PrimaryColor = RGB(255, 255, 255);
		 Pattern6.DrawZeros = false;

		 TrendUp.Name = "Trend Up";
		 TrendUp.DrawStyle = DRAWSTYLE_IGNORE;
		 TrendUp.PrimaryColor = COLOR_GREEN;
		 TrendUp.DrawZeros = false;

		 TrendDown.Name = "Trend Down";
		 TrendDown.DrawStyle = DRAWSTYLE_IGNORE;
		 TrendDown.PrimaryColor = COLOR_RED;
		 TrendDown.DrawZeros = false;

		 InputPattern1.Name="Pattern 1";
         InputPattern1.SetCandleStickPatternIndex(0);

         InputPattern2.Name="Pattern 2";
         InputPattern2.SetCandleStickPatternIndex(0);

         InputPattern3.Name="Pattern 3";
         InputPattern3.SetCandleStickPatternIndex(0);

         InputPattern4.Name="Pattern 4";
         InputPattern4.SetCandleStickPatternIndex(0);

         InputPattern5.Name="Pattern 5";
         InputPattern5.SetCandleStickPatternIndex(0);

         InputPattern6.Name="Pattern 6";
         InputPattern6.SetCandleStickPatternIndex(0);

		 UseNumericValue.Name="Use Numeric Values Instead of Letter Codes";
         UseNumericValue.SetYesNo(0);

         Distance.Name="Distance Between the Codes and the Candle as a Percentage";
         Distance.SetInt(5);

         PriceRangeNumberOfBars.Name="Number of Bars for Price Range Detection (Used for Trend Detection)";
         PriceRangeNumberOfBars.SetInt(100);

         PriceRangeMultiplier.Name="Price Range Multiplier for Determining Value Per Point";
         PriceRangeMultiplier.SetFloat(0.01f);

		 TrendDetection.Name = "Use Trend Detection";
		 TrendDetection.SetYesNo(false);

		 TrendDetectionLength.Name = "Number of Bars Used For Trend Detection";
		 TrendDetectionLength.SetInt(4);
		 TrendDetectionLength.SetIntLimits(1,MAX_STUDY_LENGTH);

		 DisplayText.Name= "Display Text";
		 DisplayText.SetYesNo( true);
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

     int UniqueID = 1546446272 + sc.StudyGraphInstanceID;;



     if (Pattern1.LineWidth <= 1)
     {
         Pattern1.LineWidth = 8;
     }

	 const int NumberOfPatternsToDetect = 6;

     unsigned long PatternToLookFor[NumberOfPatternsToDetect];
     PatternToLookFor[0] = InputPattern1.GetCandleStickPatternIndex();
     PatternToLookFor[1] = InputPattern2.GetCandleStickPatternIndex();
     PatternToLookFor[2] = InputPattern3.GetCandleStickPatternIndex();
     PatternToLookFor[3] = InputPattern4.GetCandleStickPatternIndex();
     PatternToLookFor[4] = InputPattern5.GetCandleStickPatternIndex();
     PatternToLookFor[5] = InputPattern6.GetCandleStickPatternIndex();

     s_CandleStickPatternsFinderSettings PatternsFinderSettings;

     if (PriceRangeNumberOfBars.GetInt()>0)
     {
        PatternsFinderSettings.PriceRangeNumberOfBars = PriceRangeNumberOfBars.GetInt();
     }
     
     if (PriceRangeMultiplier.GetFloat()>0)
     {
        PatternsFinderSettings.PriceRangeMultiplier = PriceRangeMultiplier.GetFloat();
     }

	 PatternsFinderSettings.UseTrendDetection = TrendDetection.GetYesNo();
     
     bool DisplayNumericValue = (UseNumericValue.GetYesNo() != 0 );

     bool PatternDetected = false;
     bool PatternCodesToDisplay = false;

   int Direction = DetermineTrendForCandlestickPatterns(sc, PatternsFinderSettings, sc.Index-1,TrendDetectionLength.GetInt());
   
   TrendForPatterns[sc.Index] = (float)Direction;
   

   if (Direction == CANDLESTICK_TREND_UP)
   {
	   TrendUp[sc.Index] = sc.Low[sc.Index];
	   TrendDown[sc.Index] = 0;
	   
   }
   else if (Direction == CANDLESTICK_TREND_DOWN)
   {
	   TrendUp[sc.Index] = 0;
	   TrendDown[sc.Index] = sc.High[sc.Index];	
   }
   else 
   {
	   TrendUp[sc.Index] = 0;
	   TrendDown[sc.Index] = 0;	
   }


   SCString DisplayTextString;

     for (int Index=0 ; Index < NumberOfPatternsToDetect ; Index++)
     {
         sc.Subgraph[Index][sc.Index] = 0;

         PatternDetected = false;

         switch(PatternToLookFor[Index])
         {
         case 1:
             PatternDetected = IsHammer(sc, PatternsFinderSettings, sc.CurrentIndex);
             break;
         case 2:
             PatternDetected = IsHangingMan(sc, PatternsFinderSettings, sc.CurrentIndex);			 
             break;
         case 3:
             PatternDetected = IsBullishEngulfing(sc, PatternsFinderSettings,sc.CurrentIndex);	
             break;
         case 4:
             PatternDetected = IsBearishEngulfing(sc, PatternsFinderSettings,sc.CurrentIndex);
             break;
         case 5:
             PatternDetected = IsDarkCloudCover(sc, PatternsFinderSettings,sc.CurrentIndex);		
             break;
         case 6:
             PatternDetected = IsPiercingLine(sc, PatternsFinderSettings,sc.CurrentIndex);		
             break;
         case 7:
             PatternDetected = IsMorningStar(sc, PatternsFinderSettings,sc.CurrentIndex);			
             break;
         case 8:			
             PatternDetected = IsEveningStar(sc, PatternsFinderSettings,sc.CurrentIndex);			
             break;
         case 9:
             PatternDetected = IsMorningDojiStar(sc, PatternsFinderSettings,sc.CurrentIndex);		
             break;
         case 10:
             PatternDetected = IsEveningDojiStar(sc, PatternsFinderSettings,sc.CurrentIndex);		
             break;
         case 11:
             PatternDetected = IsBullishAbandonedBaby(sc, PatternsFinderSettings,sc.CurrentIndex);
             break;
         case 12:
             PatternDetected = IsBearishAbandonedBaby(sc, PatternsFinderSettings,sc.CurrentIndex);
             break;
         case 13:
             PatternDetected = IsShootingStar(sc, PatternsFinderSettings,sc.CurrentIndex);		
             break;
         case 14:
             PatternDetected = IsInvertedHammer(sc, PatternsFinderSettings,sc.CurrentIndex);		
             break;
         case 15:
             PatternDetected = IsBearishHarami(sc, PatternsFinderSettings,sc.CurrentIndex);		
             break;
         case 16:
             PatternDetected = IsBullishHarami(sc, PatternsFinderSettings,sc.CurrentIndex);		
             break;
         case 17:
             PatternDetected = IsBearishHaramiCross(sc, PatternsFinderSettings,sc.CurrentIndex);	
             break;
         case 18:
             PatternDetected = IsBullishHaramiCross(sc, PatternsFinderSettings,sc.CurrentIndex);	
             break;
         case 19:
             PatternDetected = IsTweezerTop(sc, PatternsFinderSettings,sc.CurrentIndex);	
             break;
         case 20:
             PatternDetected = IsTweezerBottom(sc, PatternsFinderSettings,sc.CurrentIndex);	
             break;
         case 21:
             PatternDetected = IsBearishBeltHoldLine(sc, PatternsFinderSettings,sc.CurrentIndex);	
             break;
         case 22:
             PatternDetected = IsBullishBeltHoldLine(sc, PatternsFinderSettings,sc.CurrentIndex);			
             break;
         case 23:
             PatternDetected = IsTwoCrows(sc, PatternsFinderSettings,sc.CurrentIndex);	
             break;
         case 24:
             PatternDetected = IsThreeBlackCrows(sc, PatternsFinderSettings,sc.CurrentIndex);	
             break;
         case 25:
             PatternDetected = IsBearishCounterattackLine(sc, PatternsFinderSettings,sc.CurrentIndex);	
             break;
         case 26:
             PatternDetected = IsBullishCounterattackLine(sc, PatternsFinderSettings,sc.CurrentIndex);	
             break;
         case 27:
             PatternDetected = IsThreeInsideUp(sc, PatternsFinderSettings,sc.CurrentIndex);	
             break;
         case 28:
             PatternDetected = IsThreeOutsideUp(sc, PatternsFinderSettings,sc.CurrentIndex);	
             break;
         case 29:
             PatternDetected = IsThreeInsideDown(sc, PatternsFinderSettings,sc.CurrentIndex);	
             break;
         case 30:
             PatternDetected = IsThreeOutsideDown(sc, PatternsFinderSettings,sc.CurrentIndex);	
             break;
         case 31:
             PatternDetected = IsKicker(sc, PatternsFinderSettings,sc.CurrentIndex);	
             break;
         case 32:
             PatternDetected = IsKicking(sc, PatternsFinderSettings,sc.CurrentIndex);	
             break;		
         case 33:
             PatternDetected = IsThreeWhiteSoldiers(sc, PatternsFinderSettings,sc.CurrentIndex);	
             break;
         case 34:
             PatternDetected = IsAdvanceBlock(sc, PatternsFinderSettings,sc.CurrentIndex);	
             break;	
         case 35:
             PatternDetected = IsDeliberation(sc, PatternsFinderSettings,sc.CurrentIndex);	
             break;
         case 36:
             PatternDetected = IsBearishTriStar(sc, PatternsFinderSettings,sc.CurrentIndex);	
             break;
         case 37:
             PatternDetected = IsBullishTriStar(sc, PatternsFinderSettings,sc.CurrentIndex);	
             break;
         case 38:
             PatternDetected = IsUniqueThreeRiverBottom(sc, PatternsFinderSettings,sc.CurrentIndex);	
             break;
         case 39:
             PatternDetected = IsBearishDojiStar(sc, PatternsFinderSettings,sc.CurrentIndex);	
             break;
         case 40:
             PatternDetected = IsBullishDojiStar(sc, PatternsFinderSettings,sc.CurrentIndex);	
             break;
         case 41:
             PatternDetected = IsBearishDragonflyDoji(sc, PatternsFinderSettings,sc.CurrentIndex);	
             break;
         case 42:
             PatternDetected = IsBullishDragonflyDoji(sc, PatternsFinderSettings,sc.CurrentIndex);	
             break;
         case 43:
             PatternDetected = IsBearishGravestoneDoji(sc, PatternsFinderSettings,sc.CurrentIndex);	
             break;
         case 44:
             PatternDetected = IsBullishGravestoneDoji(sc, PatternsFinderSettings,sc.CurrentIndex);	
             break;
         case 45:
             PatternDetected = IsBearishLongleggedDoji(sc, PatternsFinderSettings,sc.CurrentIndex);	
             break;
         case 46:
             PatternDetected = IsBullishLongleggedDoji(sc, PatternsFinderSettings,sc.CurrentIndex);	
             break;			 
         case 47:
             PatternDetected = IsBearishSideBySideWhiteLines(sc, PatternsFinderSettings,sc.CurrentIndex);	
             break;
         case 48:
             PatternDetected = IsBullishSideBySideWhiteLines(sc, PatternsFinderSettings,sc.CurrentIndex);	
             break;
         case 49:
             PatternDetected = IsFallingThreeMethods(sc, PatternsFinderSettings,sc.CurrentIndex);	
             break;
         case 50:
             PatternDetected = IsRisingThreeMethods(sc, PatternsFinderSettings,sc.CurrentIndex);	
             break;
         case 51:
             PatternDetected = IsBearishSeparatingLines(sc, PatternsFinderSettings,sc.CurrentIndex);	
             break;
         case 52:
             PatternDetected = IsBullishSeparatingLines(sc, PatternsFinderSettings,sc.CurrentIndex);	
             break;
         case 53:
             PatternDetected = IsDownsideTasukiGap(sc, PatternsFinderSettings,sc.CurrentIndex);	
             break;
         case 54:
             PatternDetected = IsUpsideTasukiGap(sc, PatternsFinderSettings,sc.CurrentIndex);	
             break;
         case 55:
             PatternDetected = IsBearishThreeLineStrike(sc, PatternsFinderSettings,sc.CurrentIndex);	
             break;
         case 56:
             PatternDetected = IsBullishThreeLineStrike(sc, PatternsFinderSettings,sc.CurrentIndex);	
             break;
         case 57:
             PatternDetected = IsDownsideGapThreeMethods(sc, PatternsFinderSettings,sc.CurrentIndex);	
             break;	
         case 58:
             PatternDetected = IsUpsideGapThreeMethods(sc, PatternsFinderSettings,sc.CurrentIndex);	
             break;	
         case 59:
             PatternDetected = IsOnNeck(sc, PatternsFinderSettings,sc.CurrentIndex);	
             break;	
         case 60:
             PatternDetected = IsInNeck(sc, PatternsFinderSettings,sc.CurrentIndex);	
             break;	
         case 61:
             PatternDetected = IsBearishThrusting(sc, PatternsFinderSettings,sc.CurrentIndex);	
             break;	
         case 62:
             PatternDetected = IsMatHold(sc, PatternsFinderSettings,sc.CurrentIndex);	
             break;	
		 case 63: 
			 PatternDetected = IsDoji(sc, PatternsFinderSettings,sc.CurrentIndex);	
         } 

         if (PatternDetected)
         {
             sc.Subgraph[Index][sc.CurrentIndex] = (float)PatternToLookFor[Index];

             SCString str_Temp = DisplayTextString;

			 if (DisplayText.GetYesNo() )
			 {
				 PatternCodesToDisplay = true;

				 if(DisplayNumericValue)
					 DisplayTextString.Format("%s%.2d%s", str_Temp.GetChars(), PatternToLookFor[Index], "\r\n");
				 else
					 DisplayTextString.Format("%s%s%s", str_Temp.GetChars(), CandleStickPatternNames[PatternToLookFor[Index]][1], "\r\n");
			 }
         }
     }

     if (PatternCodesToDisplay && sc.Index < sc.ArraySize -1)
     {
         s_UseTool Tool; 

         Tool.ChartNumber = sc.ChartNumber;
         Tool.DrawingType = DRAWING_TEXT;
         Tool.LineNumber = UniqueID;
		 Tool.BeginIndex = sc.CurrentIndex;
         Tool.BeginValue = sc.Low[sc.Index] - ( (sc.High[sc.Index] - sc.Low[sc.Index])  * Distance.GetInt() * 0.01f);

         Tool.Color = Pattern1.PrimaryColor;
         Tool.FontBackColor = Pattern1.SecondaryColor;
         Tool.FontSize = Pattern1.LineWidth;
         Tool.FontBold = TRUE;
         Tool.Text.Format("%s",DisplayTextString);
         Tool.TextAlignment = DT_CENTER;
         Tool.AddMethod = UTAM_ADD_ALWAYS;

         sc.UseTool(Tool);
     }
	 else if(sc.Index == sc.ArraySize -1)
	 {
		 s_UseTool Tool; 

		 Tool.ChartNumber = sc.ChartNumber;
		 Tool.DrawingType = DRAWING_TEXT;
		 Tool.LineNumber = UniqueID +1;
		 Tool.BeginIndex = sc.CurrentIndex;
		 Tool.BeginValue = sc.Low[sc.Index] - ( (sc.High[sc.Index] - sc.Low[sc.Index])  * Distance.GetInt() * 0.01f);

		 Tool.Color = Pattern1.PrimaryColor;
		 Tool.FontBackColor = Pattern1.SecondaryColor;
		 Tool.FontSize = Pattern1.LineWidth;
		 Tool.FontBold = TRUE;
		 
		 if(DisplayTextString.GetLength() == 0)
			 DisplayTextString = " ";

		 Tool.Text.Format("%s",DisplayTextString);
		 Tool.TextAlignment = DT_CENTER;
		 Tool.AddMethod = UTAM_ADD_OR_ADJUST;

		 sc.UseTool(Tool);

	 }


     return;
 } 

 //////////////////////////////////////////////////////////////////////////////////////////////
 inline double UpperWickLength(SCBaseDataRef InData, int index)
 {
     double upperBoundary = max(InData[SC_LAST][index], InData[SC_OPEN][index]);

     double upperWickLength = InData[SC_HIGH][index] - upperBoundary;

     return upperWickLength;
 }

 //////////////////////////////////////////////////////////////////////////////////////////////
 inline double LowerWickLength(SCBaseDataRef InData, int index)
 {
     double lowerBoundary = min(InData[SC_LAST][index], InData[SC_OPEN][index]);

     double lowerWickLength = lowerBoundary - InData[SC_LOW][index];

     return lowerWickLength;
 }

 //////////////////////////////////////////////////////////////////////////////////////////////
 inline double CandleLength(SCBaseDataRef InData, int index)
 {
     return InData[SC_HIGH][index]-InData[SC_LOW][index];
 }

 //////////////////////////////////////////////////////////////////////////////////////////////
 inline double BodyLength(SCBaseDataRef InData, int index)
 {
     return fabs(InData[SC_OPEN][index] - InData[SC_LAST][index]);
 }

 //////////////////////////////////////////////////////////////////////////////////////////////
 inline double PercentOfCandleLength(SCBaseDataRef InData, int index, double percent)
 {
     return CandleLength(InData, index) * (percent / 100.0);
 }

 //////////////////////////////////////////////////////////////////////////////////////////////
 inline double PercentOfBodyLength(SCBaseDataRef InData, int index, double percent)
 {
     return BodyLength(InData, index) * percent / 100.0;
 }

 //////////////////////////////////////////////////////////////////////////////////////////////
 const int k_Body_NUM_OF_CANDLES = 5;				// number of previous candles to calculate body strength

 inline bool IsBodyStrong(SCBaseDataRef InData, int index)
 {
     // detecting whether candle has a strong body or not
     bool ret_flag = false;

     // calculating average length of bodies of last NUM_OF_CANDLES 
     float mov_aver = 0;		
     for (int i = 1; i<k_Body_NUM_OF_CANDLES+1; i++)
     {
         mov_aver += (float)BodyLength(InData, index - i);
     }
     mov_aver /= k_Body_NUM_OF_CANDLES;

     // compare with length of current body with average length.
     if(BodyLength(InData, index)>mov_aver)
     {
         ret_flag = true;
     }
     return ret_flag;
 }

 //////////////////////////////////////////////////////////////////////////////////////////////
 const int k_Candle_NUM_OF_CANDLES = 5;				// number of previous candles to calculate candle strength
 const double k_CandleStrength_Multiplier = 1.0;

 inline bool IsCandleStrength(SCBaseDataRef InData, int index)
 {
     // detecting whether candle has a strong body or not
     bool ret_flag = false;

     // calculating average length of bodies of last NUM_OF_CANDLES 
     float mov_aver = 0;
     for (int i = 1; i<k_Candle_NUM_OF_CANDLES+1; i++)
         mov_aver += (float)CandleLength(InData, index-i);

     mov_aver /=k_Candle_NUM_OF_CANDLES;

     // compare with length of body with average
     if(CandleLength(InData, index) > mov_aver*k_CandleStrength_Multiplier)
     {
         ret_flag = true;
     }
     return ret_flag;
 }

 //////////////////////////////////////////////////////////////////////////////////////////////



 int DetermineTrendForCandlestickPatterns(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& PatternsFinderSettings, int index, int num_of_candles)
 {
     // detecting trend
     // index: index of the last candle in trend
     // num_of_candles: minimum number of candles required

	 const double UptrendPercent = 1.0;
	 const double DowntrendPercent = -1.0;

     // Using linear regression to estimate the slope
     SCBaseDataRef InData = sc.BaseData;
     double sumx = 0.0;
     double sumy = 0.0;
     double sumxx = 0.0;
     double sumyy = 0.0;
     double sumxy = 0.0;

     for (int IndexOffset=1; IndexOffset<=num_of_candles;IndexOffset++)
     {
         double value = InData[SC_HL][index-(IndexOffset-1)];

         sumx  += -IndexOffset;
         sumy  += value;
         sumxx += IndexOffset*IndexOffset;
         sumyy += value*value;
         sumxy += -IndexOffset*value;
     }

     double n = double(num_of_candles);
     double Sxy = n*sumxy-sumx*sumy;
     double Sxx = n*sumxx-sumx*sumx;

     double Slope = Sxy/Sxx;	// slope value

     // estimate the value per point:

     double high=sc.GetHighest(InData[SC_HIGH], index, PatternsFinderSettings.PriceRangeNumberOfBars);

     double low=sc.GetLowest(InData[SC_LOW], index, PatternsFinderSettings.PriceRangeNumberOfBars);

     double range = high - low;

     double valuePerPoint = range * PatternsFinderSettings.PriceRangeMultiplier;    

     // detect trend

     double CorrectSlope = Slope / valuePerPoint;

     if (CorrectSlope>UptrendPercent)
     {
         return CANDLESTICK_TREND_UP;
     }
     else if (CorrectSlope<DowntrendPercent)
     {
         return CANDLESTICK_TREND_DOWN;
     }
     else
     {
         return CANDLESTICK_TREND_FLAT;
     }

   

 }

 //////////////////////////////////////////////////////////////////////////////////////////////
 

 inline bool IsDoji(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index)
 {
	 const double Doji_BodyPercent = 5.0;
     SCBaseDataRef InData = sc.BaseData;
     if (BodyLength(InData, index) <= PercentOfCandleLength(InData, index, Doji_BodyPercent))
     {
         return true;
     }

     return false;
 }

 //////////////////////////////////////////////////////////////////////////////////////////////
 inline bool IsWhiteCandle(SCBaseDataRef InData, int index)
 {
     return InData[SC_LAST][index]>InData[SC_OPEN][index];
 }

 //////////////////////////////////////////////////////////////////////////////////////////////
 inline bool IsBlackCandle(SCBaseDataRef InData, int index)
 {
     return InData[SC_LAST][index]<InData[SC_OPEN][index];
 }


 //////////////////////////////////////////////////////////////////////////////////////////////
 inline bool IsNearEqual(double value1, double value2, SCBaseDataRef InData, int index, double percent)
 {
     return abs(value1 - value2) < PercentOfCandleLength(InData, index, percent);
 }

 //////////////////////////////////////////////////////////////////////////////////////////////
 inline bool IsUpperWickSmall(SCBaseDataRef InData, int index, double percent)
 {
     return UpperWickLength(InData, index) < PercentOfCandleLength(InData, index, percent);
 }

 //////////////////////////////////////////////////////////////////////////////////////////////
 inline bool IsLowerWickSmall(SCBaseDataRef InData, int index, double percent)
 {
     return LowerWickLength(InData, index) < PercentOfCandleLength(InData, index, percent);
 }


 //////////////////////////////////////////////////////////////////////////////////////////////
 


 bool IsHammer(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index)
 {

	 const double LowerWickPercent = 200.0;
	 const double UpperWickPercent = 7.0;

	 // HAMMER
	 // 1. Downtrend
	 // 2. Body [index] of the candle in the upper part of the candle
	 // 3. Lower wick [index] of candle is 2 longer then candle body
	 // 4. Upper wick [index] of candle is either absent or small
	 SCBaseDataRef InData = sc.BaseData;

	 bool ret_flag = false;

	 if(settings.UseTrendDetection == false || sc.Subgraph[TREND_FOR_PATTERNS][index] == CANDLESTICK_TREND_DOWN)		// downtrend
	 {
		 // check that body is in the upper part of the candle
		 if(   ((InData[SC_HIGH][index]-InData[SC_OPEN][index])<(InData[SC_OPEN][index]-InData[SC_LOW][index]))
			 &&((InData[SC_HIGH][index]-InData[SC_LAST][index])<(InData[SC_LAST][index]-InData[SC_LOW][index]))
			 && (InData[SC_OPEN][index] != InData[SC_LAST][index]))
		 {

			 if (	// check of the length of the lower wick
				 IsLowerWickSmall(InData, index, LowerWickPercent) &&
				 // check of the length of the upper wick
				 IsUpperWickSmall(InData, index, UpperWickPercent)
				 )
			 {
				 ret_flag = true;
			 }
		 }
	 }
	 return ret_flag;
 }

 //////////////////////////////////////////////////////////////////////////////////////////////
 
 const double k_HangingMan_LowerWickPercent = 200.0;
 const double k_HangingMan_UpperWickPercent = 7.0;

 bool IsHangingMan(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index)
 {
	 // HANGING MAN
	 // 1. Uptrend
	 // 2. Body [index] is in the upper part of the candle
	 // 3. Lower wick [index] is 2 times longer then the body
	 // 4. Upper wick [index] is either absent or small
	 SCBaseDataRef InData = sc.BaseData;
	 bool ret_flag = false;
	 if(settings.UseTrendDetection == false || sc.Subgraph[TREND_FOR_PATTERNS][index] == CANDLESTICK_TREND_UP)	// check uptrend
	 {
		 // is body in the upper part of the candle
		 if(   ((InData[SC_HIGH][index]-InData[SC_OPEN][index])<(InData[SC_OPEN][index]-InData[SC_LOW][index]))
			 &&((InData[SC_HIGH][index]-InData[SC_LAST][index])<(InData[SC_LAST][index]-InData[SC_LOW][index]))
			 && (InData[SC_OPEN][index] != InData[SC_LAST][index]))
		 {
			 // check the length of lower wick
			 if(IsLowerWickSmall(InData, index, k_HangingMan_LowerWickPercent))
			 {
				 // check the length of the upper wick
				 if(IsUpperWickSmall(InData, index, k_HangingMan_UpperWickPercent))
				 {
					 ret_flag = true;
				 }
			 }
		 }
	 }
	 return ret_flag;
 }

 //////////////////////////////////////////////////////////////////////////////////////////////
 
 bool IsBullishEngulfing(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index)
 {
	 SCBaseDataRef InData = sc.BaseData;
	 bool ret_flag = false;
	 if(settings.UseTrendDetection == false || sc.Subgraph[TREND_FOR_PATTERNS][index] == CANDLESTICK_TREND_DOWN)		// down
	 {
		 if(InData[SC_LAST][index-1]>InData[SC_OPEN][index-1])
		 {
			 if( (InData[SC_LAST][index]<InData[SC_OPEN][index]) &&
				 (InData[SC_LAST][index-1] < InData[SC_OPEN][index]) && 
				 (InData[SC_OPEN][index-1] > InData[SC_LAST][index]))
			 {
				 ret_flag = true;	
			 }
		 }
		 else
		 {
			 if( (InData[SC_LAST][index]>InData[SC_OPEN][index]) &&
				 (InData[SC_OPEN][index-1] < InData[SC_LAST][index]) && 
				 (InData[SC_LAST][index-1] > InData[SC_OPEN][index]))
			 {
				 ret_flag = true;	
			 }
		 }
	 }
	 return ret_flag;
 }

 //////////////////////////////////////////////////////////////////////////////////////////////

 bool IsBearishEngulfing(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index)
 {
	 SCBaseDataRef InData = sc.BaseData;
	 bool ret_flag = false;
	 if(settings.UseTrendDetection == false || sc.Subgraph[TREND_FOR_PATTERNS][index] == CANDLESTICK_TREND_UP)	// up
	 {
		 if(InData[SC_LAST][index-1]>InData[SC_OPEN][index-1])
		 {
			 if( (InData[SC_LAST][index]<InData[SC_OPEN][index]) &&
				 (InData[SC_LAST][index-1] < InData[SC_OPEN][index]) && 
				 (InData[SC_OPEN][index-1] > InData[SC_LAST][index]))
			 {
				 ret_flag = true;	
			 }
		 }
		 else
		 {
			 if( (InData[SC_LAST][index]>InData[SC_OPEN][index]) &&
				 (InData[SC_OPEN][index-1] < InData[SC_LAST][index]) && 
				 (InData[SC_LAST][index-1] > InData[SC_OPEN][index]))
			 {
				 ret_flag = true;	
			 }
		 }
	 }
	 return ret_flag;
 }

 //////////////////////////////////////////////////////////////////////////////////////////////
 const double k_DarkCloudCover_OverlapPercent = 50.0;

 bool IsDarkCloudCover(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index)
 {	
	 // DARK CLOUD COVER
	 // 1. Uptrend
	 // 2. Strong white body [index-1]
	 // 3. Open [index] is higher then High of candle [index-1] 
	 // 4. Close [index] is lower then OVERLAP_PERCENT of white body of candle [index-1] 

	 /*1. Market is characterized by an uptrend.
	 2. We see a long white candlestick in the first day.
	 3. Then we see a black body characterized by an open above the high of the previous day on the second day.
	 4. The second black candlestick closes within and below the midpoint of the previous white body.*/

	 SCBaseDataRef InData = sc.BaseData;
	 bool ret_flag = false;
	 if(
		 (settings.UseTrendDetection == false || sc.Subgraph[TREND_FOR_PATTERNS][index] == CANDLESTICK_TREND_UP)	// uptrend
		 && IsWhiteCandle(InData, index-1) // check for white body
		 && IsBodyStrong(InData,index-1)
		 && IsBlackCandle(InData, index)  // 0 candle is black
		 && (InData[SC_OPEN][index]>InData[SC_HIGH][index-1]) // open 0 is higher then high of -1
		 && (InData[SC_LAST][index]<(InData[SC_LAST][index-1]-PercentOfBodyLength(InData, index-1, k_DarkCloudCover_OverlapPercent))) // check for the overlap between white body and black OVERLAP_PERCENT
		 )
	 {
		 ret_flag = true;
	 }		

	 return ret_flag;
 }

 //////////////////////////////////////////////////////////////////////////////////////////////
 const double k_PiercingLine_OverlapPercent = 50.0; //~~ This should be 50

 bool IsPiercingLine(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index)
 {
	 // PIERCING LINE
	 // 1. Downtrend
	 // 2. Strong black body [index-1]
	 // 3. Open [index] less then low of candle [index-1]
	 // 4. Close [index] is higher than white body of candle [index-1] by OverlapPercent
	 SCBaseDataRef InData = sc.BaseData;
	 bool ret_flag = false;
	 if(settings.UseTrendDetection == false || sc.Subgraph[TREND_FOR_PATTERNS][index] == CANDLESTICK_TREND_DOWN)	// downtrend
	 {
		 if (IsBlackCandle(InData, index-1))	// check for black body, -1th candle is black
		 {
			 if (IsWhiteCandle(InData, index) &&	// 0th candle is white
				 (InData[SC_OPEN][index]<InData[SC_LOW][index-1])) // open of 0 the below low of -1th
			 {
				 // check for the overlap between black body and white by OverlapPercent
				 if (InData[SC_LAST][index]>(InData[SC_OPEN][index-1]-PercentOfBodyLength(InData, index-1, k_PiercingLine_OverlapPercent)))
				 {
					 if(IsBodyStrong(InData,index-1))	// strong body of -1th candle
					 {
						 ret_flag = true;
					 }

				 }		
			 }
		 }		
	 }
	 return ret_flag;
 }

 //////////////////////////////////////////////////////////////////////////////////////////////
const double k_MorningStar_OverlapPercent = 50.0;

 bool IsMorningStar(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index)
 {
	 // MORNING STAR
	 // 1. Downtrend
	 // 2. Strong black body of candle [index-2]
	 // 3. Any weak body of candle [index-1], but not Doji
	 // 4. Strong white body of candle [index] overlap more then OVERLAP_PERCENT of the body of candle [index-2]

	 /*1. Market is characterized by downtrend.
	 2. We see a long black candlestick in the first day.
	 3. Then we see a small body on the second day gapping in the direction of the previous downtrend.
	 4. Finally we see a white candlestick on the third day*/
	 SCBaseDataRef InData = sc.BaseData;
	 bool ret_flag = false;
	 if(
		 (settings.UseTrendDetection == false || sc.Subgraph[TREND_FOR_PATTERNS][index] == CANDLESTICK_TREND_DOWN)
		 && IsBlackCandle(InData, index-2)
		 && IsBodyStrong(InData,index-2)
		 && (InData[SC_OPEN][index-1] < InData[SC_LOW][index-2])// body of -1 candle below low of -2 candle
		 && (InData[SC_LAST][index-1] < InData[SC_LOW][index-2])
		 //&& !IsDoji(sc, settings, index-1) // -1 candle is not Doji
		 && !IsBodyStrong(InData,index-1)	// body of -1 candle is weak
		 && IsWhiteCandle(InData,  index)
		 && IsBodyStrong(InData,index)
		 && (InData[SC_OPEN][index]<InData[SC_LAST][index-2])	// body of 0 candle starts below the body of -2 candle
		 && (InData[SC_LAST][index]>(InData[SC_OPEN][index-2]-PercentOfBodyLength(InData, index, k_MorningStar_OverlapPercent))) // body of the candle 0 overlaps the body of the candle -2
		 )
	 {
		 ret_flag = true;
	 }
	 return ret_flag;
 }

 //////////////////////////////////////////////////////////////////////////////////////////////
  const double k_EveningStar_OverlapPercent = 50.0;

 bool IsEveningStar(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index)
 {
	 // EVENING STAR
	 // 1. Uptrend
	 // 2. Strong white body of candle [index-2]
	 // 3. Any weak body of candle [index-1], but not Doji
	 // 4. Strong black body of candle [index] overlaps more then OVERLAP_PERCENT of body of candle [index-2]. -I have not found any info about overlapping
	 SCBaseDataRef InData = sc.BaseData;
	 bool ret_flag = false;
	 if(settings.UseTrendDetection == false || sc.Subgraph[TREND_FOR_PATTERNS][index] == CANDLESTICK_TREND_UP)	// check for uptrend
	 {
		 if(IsWhiteCandle(InData, index-2))	// body of candle -2 is white
		 {
			 if(IsBodyStrong(InData,index-2))	// body of candle -2 is strong
			 {
				 if ((InData[SC_OPEN][index-1] > InData[SC_HIGH][index-2])&& // body of candle -1 higher then maximum. - This line does not correspond to what I found online it should not matter, the gap should be between the bodies.
					 (InData[SC_LAST][index-1] > InData[SC_LOW][index-2]) 
					 //&&	  !IsDoji(sc,settings,index-1)	// candle -1 is not Doji
					 )
				 {
					 if(!IsBodyStrong(InData,index-1)) // body of the candle -1 is weak
					 {
						 if (IsBlackCandle(InData,index) &&		// body of the candle 0 is black
							 (InData[SC_OPEN][index]>InData[SC_LAST][index-2]) &&	// body of the candle 0 starts higher then body of candle -2
							 (InData[SC_LAST][index]<(InData[SC_LAST][index-2]-PercentOfBodyLength(InData, index-2, k_EveningStar_OverlapPercent))))	// body of candle 0 overlaps body of candle -2 ~~Did not find written reference to this, but all images support this.
						 {
							 if (IsBodyStrong(InData,index)) // body of candle 0 is strong
							 {
								 ret_flag = true;
							 }
						 }
					 }

				 }
			 }
		 }
	 }
	 return ret_flag;
 }

 //////////////////////////////////////////////////////////////////////////////////////////////
 const double k_MorningDojiStar_OverlapPercent = 50.0;

 bool IsMorningDojiStar(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index)
 {
	 // MORNING DOJI STAR
	 // 1. Downtrend
	 // 2. Strong black body of candle [index-2]  
	 // 3. [index-1] candle is Doji   ~~(2nd day is a doji which gaps below the 1st day's close.)
	 // 4. Strong white body of candle [index] overlap more then OVERLAP_PERCENT of body of the candle [index-2]
	 SCBaseDataRef InData = sc.BaseData;
	 bool ret_flag = false;
	 if(
		 (settings.UseTrendDetection == false || sc.Subgraph[TREND_FOR_PATTERNS][index] == CANDLESTICK_TREND_DOWN) 			// check for downtrend
		 && IsBlackCandle(InData,index-2)	// body of the candle -2 is black
		 && IsBodyStrong(InData,index-2)	// body of the candle -2 is strong
		 && (InData[SC_HIGH][index-1] < InData[SC_LAST][index-2])// the -1 candle is below the close of candle -2
	 && IsDoji(sc, settings, index-1) 	// -1 candle is Doji
		 && IsWhiteCandle(InData,index)		// body of the candle 0 is white
		 && (InData[SC_OPEN][index]<InData[SC_LAST][index-2]) // body of the candle 0 starts below the body of candle -2
	 && (InData[SC_LAST][index]>(InData[SC_OPEN][index-2]-PercentOfBodyLength(InData, index-2, k_MorningDojiStar_OverlapPercent)))// body of the candle 0 overlaps body of the candle -2
		 && IsBodyStrong(InData,index)		// body of the candle 0 is strong
		 )
	 {
		 ret_flag = true;
	 }
	 return ret_flag;
 }

 //////////////////////////////////////////////////////////////////////////////////////////////
 const double k_EveningDojiStar_OverlapPercent = 50.0;

 bool IsEveningDojiStar(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index)
 {
	 // EVENING DOJI STAR
	 // 1. Uptrend
	 // 2. Strong white body of candle [index-2]
	 // 3. candle [index-1]  2nd day is a doji which gaps above the  [index-2] 1st day's close.
	 // 4. Strong black body of candle [index]
	 SCBaseDataRef InData = sc.BaseData;
	 bool ret_flag = false;
	 if(
		 (settings.UseTrendDetection == false || sc.Subgraph[TREND_FOR_PATTERNS][index] == CANDLESTICK_TREND_UP)	// check for uptrend
		 && IsWhiteCandle(InData, index-2)	// body of the -2 candle is white
		 && IsBodyStrong(InData,index-2)	// body of th -2 candle is strong
		 && InData[SC_OPEN][index-1] > InData[SC_HIGH][index-2] // body of the -1 candle is higher then maximum of -2 candle
	 && IsDoji(sc, settings, index-1)	// -1 candle is Doji
		 && IsBlackCandle(InData, index)		// body of the 0 candle is black 
		 && InData[SC_OPEN][index]>InData[SC_LAST][index-2] // body of the 0 candle starts higher then body of -2 candle
	 //&&(InData[SC_LAST][index]<(InData[SC_LAST][index-2]-PercentOfBodyLength(InData, index-2, k_EveningDojiStar_OverlapPercent))))	// body of the 0 candle overlaps the body of -2 candle //This is not a requirement
	 && IsBodyStrong(InData,index)	// body of candle 0 is strong
		 )
	 {
		 ret_flag = true;
	 }
	 return ret_flag;
 }

 //////////////////////////////////////////////////////////////////////////////////////////////
 const double k_BullishAbandonedBaby_OverlapPercent = 50.0;

 bool IsBullishAbandonedBaby(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index)
 {
	 // BULLISH ABNDONED BABY
	 // 1. Downtrend
	 // 2. Strong black body of candle [index-2]
	 // 3. Candle [index-1] - Doji
	 // 4. Strong white body of candle [index] overlaps more then OVERLAP_PERCENT of the body of candle [index-2] ~~Is not included in web descriptions but is logical.
	 // 5. Gap between -1 and 0 candles, -1 and -2 candles
	 SCBaseDataRef InData = sc.BaseData;
	 bool ret_flag = false;
	 if(settings.UseTrendDetection == false || sc.Subgraph[TREND_FOR_PATTERNS][index] == CANDLESTICK_TREND_DOWN)	// check for downtrend
	 {
		 if(IsBlackCandle(InData, index-2))	// body of candle -2 is black
		 {
			 if(IsBodyStrong(InData,index-2))	// body of the candle -2 is strong
			 {
				 if ((InData[SC_OPEN][index-1] < InData[SC_LOW][index-2]) && // body of the candle -1 is below the minimum of candle -2
					 IsDoji(sc, settings, index-1))	// -1 candle is Doji
				 {
					 if (IsWhiteCandle(InData, index) &&	// body of candle 0 is white
						 (InData[SC_OPEN][index]<InData[SC_LAST][index-2]) &&// body of candle 0 starts below of the body of the candle -2
						 (InData[SC_LAST][index]>(InData[SC_OPEN][index-2]-PercentOfBodyLength(InData, index-2, k_BullishAbandonedBaby_OverlapPercent))))	// body of candle 0 overlaps with body of candle -2
					 {
						 if (IsBodyStrong(InData,index))	// body of candle 0 is strong
						 {
							 if ((InData[SC_LOW][index-2]>InData[SC_HIGH][index-1]) &&	// gap between candle -2 and -1
								 (InData[SC_LOW][index]>InData[SC_HIGH][index-1]))		// gap between candle -0 and -1
							 {
								 ret_flag = true;
							 }							
						 }
					 }
				 }
			 }
		 }
	 }
	 return ret_flag;
 }

 //////////////////////////////////////////////////////////////////////////////////////////////
const double k_BearishAbandonedBaby_OverlapPercent = 50.0;

 bool IsBearishAbandonedBaby(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index)
 {
	 // BEARISH ABNDONED BABY
	 // 1. Uptrend
	 // 2. Strong white body of candle [index-2]
	 // 3. Weak body of candle [index-1]
	 // 4. Candle [index-1] is Doji
	 // 5. Strong black body of candle [index] overlaps more then OVERLAP_PERCENT of body of candle [index-2]
	 // 6. Gap between candle -1 and 0, -1 and 2 candles
	 SCBaseDataRef InData = sc.BaseData;
	 bool ret_flag = false;
	 if(settings.UseTrendDetection == false || sc.Subgraph[TREND_FOR_PATTERNS][index] == CANDLESTICK_TREND_UP)	// check for uptrend
	 {
		 if(IsWhiteCandle(InData, index-2))	// body of candle -2 is white
		 {
			 if(IsBodyStrong(InData,index-2))	// body of candle -2 is strong
			 {
				 if ((InData[SC_OPEN][index-1] > InData[SC_HIGH][index-2]) && // body of candle -1 is higher than maximum of candle -2
					 IsDoji(sc, settings, index-1))	 // candle -1 is Doji
				 {
					 if (IsBlackCandle(InData, index) &&	// body candle 0 is black
						 (InData[SC_OPEN][index]>InData[SC_LAST][index-2]) &&// body of candle 0 starts higher then body of candle -2
						 (InData[SC_LAST][index]<(InData[SC_LAST][index-2]-PercentOfBodyLength(InData, index-2, k_BearishAbandonedBaby_OverlapPercent))))	// body of candle 0 overlaps body of candle -2
					 {
						 if (IsBodyStrong(InData,index))	// body of candle 0 is strong
						 {
							 if ((InData[SC_HIGH][index-2]<InData[SC_LOW][index-1]) &&	// gap between -2 and -1 candle
								 (InData[SC_HIGH][index]<InData[SC_LOW][index-1]))		// gap between -0 and -1 candle
							 {
								 ret_flag = true;
							 }					
						 }
					 }
				 }
			 }
		 }
	 }
	 return ret_flag;
 }

 //////////////////////////////////////////////////////////////////////////////////////////////
 const double k_ShootingStar_LowerWickPercent = 7.0;
 const double k_ShootingStar_UpperWickPercent = 300.0;

 bool IsShootingStar(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index)
 {
	 // SHOOTING STAR
	 // 1. Uptrend
	 // 2. Small body of candle [index] is in the lower part of candle
	 // 3. Upper wick of the candle [index] is  at least three times as long as the body
	 // 4. Lower wick [index] of the candle is either absent or small
	 SCBaseDataRef InData = sc.BaseData;
	 bool ret_flag = false;
	 if(settings.UseTrendDetection == false || sc.Subgraph[TREND_FOR_PATTERNS][index] == CANDLESTICK_TREND_UP)	// uptrend
	 {
		 if (!IsBodyStrong(InData,index))	// weak body of the candle 0
		 {
			 // check that the body of the candle is in the lower part of the candle
			 if(   ((InData[SC_HIGH][index]-InData[SC_OPEN][index])>(InData[SC_OPEN][index]-InData[SC_LOW][index]))
				 &&((InData[SC_HIGH][index]-InData[SC_LAST][index])>(InData[SC_LAST][index]-InData[SC_LOW][index]))
				 )
			 {
				 // Upper wick is 3 times longer then the body
				 if(UpperWickLength(InData, index) >= PercentOfBodyLength(InData, index, k_ShootingStar_UpperWickPercent))
				 {
					 // Lower wick is insignificant
					 if(IsLowerWickSmall(InData, index, k_ShootingStar_LowerWickPercent))
					 {
						 ret_flag = true;
					 }
				 }
			 }
		 }			
	 }
	 return ret_flag;
 }

 //////////////////////////////////////////////////////////////////////////////////////////////
 const double k_InvertedHammer_LowerWickPercent = 7.0;
 const double k_InvertedHammer_UpperWickPercent = 200.0;

 bool IsInvertedHammer(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index)
 {
	 // INVERTED HAMMER
	 // 1. Downtrend
	 // 2. Small body of candle [index] is in the lower part of the candle
	 // 3. The upper shadow is no more than two times as long as the body
	 // 4. Lower wick [index] of the candle is either absent or small
	 SCBaseDataRef InData = sc.BaseData;
	 bool ret_flag = false;
	 if(settings.UseTrendDetection == false || sc.Subgraph[TREND_FOR_PATTERNS][index] == CANDLESTICK_TREND_DOWN)	// downtrend
	 {
		 if (!IsBodyStrong(InData,index))	// candle 0 has weak body
		 {
			 // check that body is in the lower part of the candle
			 if(   ((InData[SC_HIGH][index]-InData[SC_OPEN][index])>(InData[SC_OPEN][index]-InData[SC_LOW][index]))
				 &&((InData[SC_HIGH][index]-InData[SC_LAST][index])>(InData[SC_LAST][index]-InData[SC_LOW][index]))
				 && !IsDoji(sc, settings, index)) // candle is not Doji
			 {
				 // The upper shadow is no more than two times as long as the body.
				 if(UpperWickLength(InData, index)<=PercentOfBodyLength(InData, index, k_InvertedHammer_UpperWickPercent))
				 {
					 // lower wick is insignificant
					 if(IsLowerWickSmall(InData, index, k_InvertedHammer_LowerWickPercent))
					 {
						 ret_flag = true;
					 }
				 }
			 }
		 }			
	 }
	 return ret_flag;
 }
 //////////////////////////////////////////////////////////////////////////////////////////////

 bool IsBearishHarami(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index)
 {
	 // BEARISH HARAMI
	 // 1. Uptrend
	 // 2. Strong body of candle [index-1] 
	 // 3. Weak body of candle [index] does not extend beyond the limits of the body of candle [index-1] 
	 // 4. Candle [index] is not Doji
	 SCBaseDataRef InData = sc.BaseData;
	 bool ret_flag = false;
	 if(settings.UseTrendDetection == false || sc.Subgraph[TREND_FOR_PATTERNS][index] == CANDLESTICK_TREND_UP)	// Uptrend
	 {
		 if (IsBodyStrong(InData,index-1))	// Strong body of candle -1
		 {
			 if(!IsDoji(sc, settings, index)) // candle 0 is not Doji
			 {
				 if(!IsBodyStrong(InData,index))		// weak body of candle 0
				 {
					 if(InData[SC_OPEN][index-1] > InData[SC_LAST][index-1])	// part of a check that one body does not extend beyond the limits of the other body 
					 {
						 if(InData[SC_OPEN][index] > InData[SC_LAST][index])	// part of a check that one body does not extend beyond the limits of the other body
						 {
							 // body of the candle 0 does not extend beyond the limits of the body of candle -1 
							 if ((InData[SC_OPEN][index-1]>=InData[SC_OPEN][index]) &&
								 (InData[SC_LAST][index-1]<=InData[SC_LAST][index]))
							 {
								 ret_flag = true;
							 }
						 }
						 else	// part of a check that one body does not extend beyond the limits of the other body
						 {
							 // body of the candle 0 does not extend beyond the limits of the body of candle -1
							 if ((InData[SC_OPEN][index-1]>=InData[SC_LAST][index]) &&
								 (InData[SC_LAST][index-1]<=InData[SC_OPEN][index]))
							 {
								 ret_flag = true;
							 }
						 }
					 }
					 else	// part of a check that one body does not extend beyond the limits of the other body
					 {
						 if(InData[SC_OPEN][index] > InData[SC_LAST][index])		// part of a check that one body does not extend beyond the limits of the other body
						 {
							 // body of the candle 0 does not extend beyond the limits of the body of candle -1
							 if ((InData[SC_LAST][index-1]>=InData[SC_OPEN][index]) &&
								 (InData[SC_OPEN][index-1]<=InData[SC_LAST][index]))
							 {
								 ret_flag = true;
							 }
						 }
						 else	// part of a check that one body does not extend beyond the limits of the other body
						 {
							 // body of the candle 0 does not extend beyond the limits of the body of candle -1
							 if ((InData[SC_LAST][index-1]>=InData[SC_LAST][index]) &&
								 (InData[SC_OPEN][index-1]<=InData[SC_OPEN][index]))
							 {
								 ret_flag = true;
							 }
						 }
					 }
				 }
			 }
		 }
	 }
	 return ret_flag;
 }
 //////////////////////////////////////////////////////////////////////////////////////////////
 
 bool IsBullishHarami(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index)
 {
	 // BULLISH HARAMI
	 // 1. Downtrend
	 // 2. Strong body of candle [index-1] 
	 // 3. Weak body of candle [index] does not extend beyond the limits of the body of candle [index-1] 
	 // 4. Candle [index] is not Doji
	 SCBaseDataRef InData = sc.BaseData;
	 bool ret_flag = false;
	 if(settings.UseTrendDetection == false || sc.Subgraph[TREND_FOR_PATTERNS][index] == CANDLESTICK_TREND_DOWN)	// downtrend
	 {
		 if (IsBodyStrong(InData,index-1))	// Strong body of candle [index-1]
		 {
			 if(!IsDoji(sc, settings, index))	// candle 0 is not Doji
			 {
				 if(!IsBodyStrong(InData,index))	 // weak body of candle 0
				 {
					 if(InData[SC_OPEN][index-1] > InData[SC_LAST][index-1])	// part of a check that one body does not extend beyond the limits of the other body
					 {
						 if(InData[SC_OPEN][index] > InData[SC_LAST][index])	// part of a check that one body does not extend beyond the limits of the other body
						 {
							 // body of the candle 0 does not extend beyond the limits of the body of candle -1
							 if ((InData[SC_OPEN][index-1]>=InData[SC_OPEN][index]) &&
								 (InData[SC_LAST][index-1]<=InData[SC_LAST][index]))
							 {
								 ret_flag = true;
							 }
						 }
						 else	// part of a check that one body does not extend beyond the limits of the other body
						 {
							 // body of the candle 0 does not extend beyond the limits of the body of candle -1
							 if ((InData[SC_OPEN][index-1]>=InData[SC_LAST][index]) &&
								 (InData[SC_LAST][index-1]<=InData[SC_OPEN][index]))
							 {
								 ret_flag = true;
							 }
						 }
					 }
					 else	// part of a check that one body does not extend beyond the limits of the other body
					 {
						 if(InData[SC_OPEN][index] >= InData[SC_LAST][index])	// part of a check that one body does not extend beyond the limits of the other body
						 {
							 // body of the candle 0 does not extend beyond the limits of the body of candle -1
							 if ((InData[SC_LAST][index-1]>=InData[SC_OPEN][index]) &&
								 (InData[SC_OPEN][index-1]<=InData[SC_LAST][index]))
							 {
								 ret_flag = true;
							 }
						 }
						 else	// part of a check that one body does not extend beyond the limits of the other body
						 {
							 // body of the candle 0 does not extend beyond the limits of the body of candle -1
							 if ((InData[SC_LAST][index-1]>=InData[SC_LAST][index]) &&
								 (InData[SC_OPEN][index-1]<=InData[SC_OPEN][index]))
							 {
								 ret_flag = true;
							 }
						 }
					 }
				 }
			 }
		 }
	 }
	 return ret_flag;
 }
 //////////////////////////////////////////////////////////////////////////////////////////////

 bool IsBearishHaramiCross(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index)
 {
	 // BEARISH HARAMI CROSS
	 // 1. Uptrend
	 // 2. Strong body of candle [index-1] 
	 // 3. Candle [index] is Doji
	 // 4. Body of candle [index] does not extend beyond the limits of the body of the candle [index-1] 
	 SCBaseDataRef InData = sc.BaseData;
	 bool ret_flag = false;
	 if(settings.UseTrendDetection == false || sc.Subgraph[TREND_FOR_PATTERNS][index] == CANDLESTICK_TREND_UP)	// uptrend
	 {
		 if (IsBodyStrong(InData,index-1))		// strong body of the candle -1
		 {
			 if(IsDoji(sc, settings, index))	// candle 0 is Doji
			 {
				 if(InData[SC_OPEN][index-1] > InData[SC_LAST][index-1])		// part of a check that one body does not extend beyond the limits of the other body
				 {
					 // body of the candle 0 does not extend beyond the limits of the body of candle -1
					 if ((InData[SC_OPEN][index-1]>InData[SC_LAST][index]) &&
						 (InData[SC_LAST][index-1]<InData[SC_LAST][index]))
					 {
						 ret_flag = true;
					 }					
				 }
				 else	// part of a check that one body does not extend beyond the limits of the other body
				 {
					 // body of the candle 0 does not extend beyond the limits of the body of candle -1 
					 if ((InData[SC_LAST][index-1]>=InData[SC_LAST][index]) &&
						 (InData[SC_OPEN][index-1]<=InData[SC_LAST][index]))
					 {
						 ret_flag = true;
					 }					
				 }				
			 }
		 }
	 }
	 return ret_flag;
 }
 //////////////////////////////////////////////////////////////////////////////////////////////
 

 bool IsBullishHaramiCross(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index)
 {
	 // BULLISH HARAMI CROSS
	 // 1. Downtrend
	 // 2. Strong body of the candle [index-1] 
	 // 3. candle [index] is Doji
	 // 4. Candle [index] does not extend beyond the limits of the body of the candle [index-1] 
	 SCBaseDataRef InData = sc.BaseData;
	 bool ret_flag = false;
	 if(settings.UseTrendDetection == false || sc.Subgraph[TREND_FOR_PATTERNS][index] == CANDLESTICK_TREND_DOWN)	// Downtrend
	 {
		 if (IsBodyStrong(InData,index-1))		// strong body of the candle -1
		 {
			 if(IsDoji(sc, settings, index))	// candle 0 is Doji
			 {
				 if(InData[SC_OPEN][index-1] > InData[SC_LAST][index-1])		// part of a check that one body does not extend beyond the limits of the other body
				 {
					 // body of the candle 0 does not extend beyond the limits of the body of candle -1
					 if ((InData[SC_OPEN][index-1]>InData[SC_LAST][index]) &&
						 (InData[SC_LAST][index-1]<InData[SC_LAST][index]))
					 {
						 ret_flag = true;
					 }					
				 }
				 else		// part of a check that one body does not extend beyond the limits of the other body
				 {
					 // body of the candle 0 does not extend beyond the limits of the body of candle -1
					 if ((InData[SC_LAST][index-1]>InData[SC_LAST][index]) &&
						 (InData[SC_OPEN][index-1]<InData[SC_LAST][index]))
					 {
						 ret_flag = true;
					 }					
				 }				
			 }
		 }
	 }
	 return ret_flag;
 }
 //////////////////////////////////////////////////////////////////////////////////////////////
const int    k_TweezerTop_PAST_INDEX = 3;
 const double k_TweezerTop_SimularityPercent = 7.0;

 bool IsTweezerTop(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index)
 {
	 // TWEEZER TOP
	 // 1. Maximum of the candle [index] is equal to the maximum of one of the [index-X] candles (where X is from 1 to PAST_INDEX)
	 // 2. uptrend for the candle [index-X]


	 SCBaseDataRef InData = sc.BaseData;
	 bool ret_flag = false;
	 // Iterating previous candles
	 /*for (int i=1; i<k_TweezerTop_PAST_INDEX+1;i++)
	 {
	 // Searching for a candle with matching maximum 
	 if(IsNearEqual(InData[SC_HIGH][index], InData[SC_HIGH][index-i], InData, index, k_TweezerTop_SimularityPercent))
	 {
	 // checking the trend
	 if(settings.UseTrendDetection == false || DetectTendency(sc, settings,index-i,k_TweezerTop_TrendArea) == 1)
	 {
	 ret_flag = true;
	 break;
	 }
	 }
	 }*/

	 //-----------------------------------------------
	 // 1. 1st day consists of a long body candle.
	 // 2. 2nd day consists of a short body candle that has a high equal to the prior day's high.
	 // 3. The color of each candle body is not considered in the matching of this pattern.	

	 if(
		 (settings.UseTrendDetection == false || sc.Subgraph[TREND_FOR_PATTERNS][index] == CANDLESTICK_TREND_UP)
		 && IsNearEqual(InData[SC_HIGH][index], InData[SC_HIGH][index-1], InData, index, k_TweezerTop_SimularityPercent)
		 && IsBodyStrong(InData,index-1)
		 && !IsBodyStrong(InData,index)
		 )
	 {
		 ret_flag = true;

	 }

	 return ret_flag;	
 }
 //////////////////////////////////////////////////////////////////////////////////////////////
 const int    k_TweezerBottom_PAST_INDEX = 3;
 const double k_TweezerBottom_SimularityPercent = 7.0;

 bool IsTweezerBottom(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index)
 {
	 // TWEEZER BOTTOM
	 // 1. Minimum of the candle [index] matches the minimum of one of the candles [index-X] (where X is from 1 to PAST_INDEX)
	 // 2. Downtrend for the candle [index-X] 
	 SCBaseDataRef InData = sc.BaseData;
	 bool ret_flag = false;
	 // Iterating previous candles
	 // for (int i=1; i<k_TweezerBottom_PAST_INDEX+1;i++)
	 // {
	 //Searching for a candle with matching minimum
	 // if(IsNearEqual(InData[SC_LOW][index], InData[SC_LOW][index-i], InData, index, k_TweezerBottom_SimularityPercent))
	 // {
	 //Checking the trend
	 // if(settings.UseTrendDetection == false || DetectTendency(sc, settings,index-i,k_TweezerBottom_TrendArea) == -1)
	 // {
	 // ret_flag = true;
	 // break;
	 // }
	 // }
	 // }

	 // 1. 1st day consists of a long body candle.
	 // 2. 2nd day consists of a short body candle that has a low equal to the prior day's low.
	 // 3. The color of each candle body is not considered in the matching of this pattern.

	 if(
		 (settings.UseTrendDetection == false || sc.Subgraph[TREND_FOR_PATTERNS][index] == CANDLESTICK_TREND_DOWN)
		 && IsNearEqual(InData[SC_LOW][index], InData[SC_LOW][index-1], InData, index, k_TweezerBottom_SimularityPercent)
		 && IsBodyStrong(InData,index-1)
		 && !IsBodyStrong(InData,index)
		 )
	 {
		 ret_flag = true;

	 }
	 return ret_flag;
 }
 //////////////////////////////////////////////////////////////////////////////////////////////
const double k_BearishBeltHoldLine_LowerWickPercent = 7.0;
 const double k_BearishBeltHoldLine_SimularityPercent = 7.0;

 bool IsBearishBeltHoldLine(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index)
 {
	 // BEARISH BELT HOLD LINE
	 // 1. Uptrend
	 // 2. Long black day where the open is equal to the high.
	 // 3. No upper shadow.
	 SCBaseDataRef InData = sc.BaseData;
	 bool ret_flag = false;
	 if(
		 (settings.UseTrendDetection == false || sc.Subgraph[TREND_FOR_PATTERNS][index] == CANDLESTICK_TREND_UP)	// uptrend
		 && IsBlackCandle(InData, index)		// Black body of the candle 0
		 && IsBodyStrong(InData,index) //Strong body of candle0
		 && IsNearEqual(InData[SC_HIGH][index], InData[SC_OPEN][index], InData, index, k_BearishBeltHoldLine_SimularityPercent)	// OPEN = HIGH
		 )
	 {
		 ret_flag = true;
	 }
	 return ret_flag;
 }
 //////////////////////////////////////////////////////////////////////////////////////////////
 const double k_BullishBeltHoldLine_UpperWickPercent = 7.0;
 const double k_BullishBeltHoldLine_SimularityPercent = 7.0;

 bool IsBullishBeltHoldLine(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index)
 {
	 // BULLISH BELT HOLD LINE
	 // 1. Downtrend
	 // 2. Long white day where the open is equal to the low.
	 // 3. No lower shadow.
	 SCBaseDataRef InData = sc.BaseData;
	 bool ret_flag = false;
	 if(
		 (settings.UseTrendDetection == false || sc.Subgraph[TREND_FOR_PATTERNS][index] == CANDLESTICK_TREND_DOWN)	// downtrend
		 && IsWhiteCandle(InData,index)		// white body of the candle 0
		 && IsBodyStrong(InData,index) //Strong body of candle0
		 && IsNearEqual(InData[SC_LOW][index], InData[SC_OPEN][index], InData, index, k_BullishBeltHoldLine_SimularityPercent) // OPEN = LOW
		 )
	 {
		 ret_flag = true;
	 }

	 return ret_flag;
 }
 //////////////////////////////////////////////////////////////////////////////////////////////

 bool IsTwoCrows(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index)
 {
	 // UPSIDE-GAP TWO CROWS
	 // 1. Uptrend
	 // 2. White strong body of the candle [index-2] 
	 // 3. Candles [index-1] and [index] are black
	 // 4. Gap between the bodies of candles  [index-2] and [index-1] 
	 // 5. The body of the candle [index] engulfs the body of the candle [index-1] 
	 // 6. 3rd day closes above the close of the 1st day.
	 SCBaseDataRef InData = sc.BaseData;
	 bool ret_flag = false;
	 if(
		 (settings.UseTrendDetection == false || sc.Subgraph[TREND_FOR_PATTERNS][index] == CANDLESTICK_TREND_UP)	// uptrend
		 && IsWhiteCandle(InData, index-2) // the candle -2 is white 
		 && IsBodyStrong(InData,index-2)	// the body of the candle -2 is strong
		 && IsBlackCandle(InData, index-1)  // the body of the candle -1 is black 
		 && IsBlackCandle(InData, index)		// the body of the candle 0 is black
		 && InData[SC_LAST][index-2]<InData[SC_LAST][index-1]	// gap between the bodies of candles -2 and -1
	 && InData[SC_OPEN][index-1]<=InData[SC_OPEN][index] // the body of the candle 0 engulfs the body of the candle -1
	 && InData[SC_LAST][index-1]>=InData[SC_LAST][index]
	 && InData[SC_LAST][index]>InData[SC_LAST][index-2] // 3rd day closes above the close of the 1st day.
	 )
	 {
		 ret_flag = true;
	 }

	 return ret_flag;
 }
 //////////////////////////////////////////////////////////////////////////////////////////////
const double k_ThreeBlackCrows_LowerWickPercent= 7.0;

 bool IsThreeBlackCrows(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index)
 {
	 // THREE BLACK CROWS
	 // 1. Uptrend
	 // 2. Three consecutive black days with lower closes each day.
	 // 3. Each day opens within the body of the previous day.
	 SCBaseDataRef InData = sc.BaseData;
	 bool ret_flag = false;
	 if(
		 (settings.UseTrendDetection == false || sc.Subgraph[TREND_FOR_PATTERNS][index] == CANDLESTICK_TREND_UP)	// uptrend
		 && (IsBlackCandle(InData, index-2)
		 && IsBlackCandle(InData, index-1)
		 && IsBlackCandle(InData, index))
		 && InData[SC_LAST][index-2]>InData[SC_LAST][index-1]
	 && InData[SC_LAST][index-1]>InData[SC_LAST][index]
	 && InData[SC_OPEN][index-1]<=InData[SC_OPEN][index-2]
	 && InData[SC_OPEN][index-1]>=InData[SC_LAST][index-2]
	 && InData[SC_OPEN][index]<=InData[SC_OPEN][index-1]
	 && InData[SC_OPEN][index]>=InData[SC_LAST][index-1])
	 {
		 ret_flag = true;
	 }

	 return ret_flag;
 }
 //////////////////////////////////////////////////////////////////////////////////////////////
  const double k_BearishCounterattackLine_SimularityPercent = 7.0;

 bool IsBearishCounterattackLine(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index)
 {
	 // BEARISH COUNTERATTACK LINE
	 // 1. Uptrend
	 // 2. The candles [index] and [index-1] are of contrast colors and their close prices are equal
	 //Bearish Counterattack Line
	 //In an uptrending market, a large white candlestick is following by a large black candlestick that opens on a big gap higher and then slumps back during the period to close at the same price as the previous close. The bearish black candlestick needs followup action to the downside to confirm the turn to a downtrend. 
	 SCBaseDataRef InData = sc.BaseData;
	 bool ret_flag = false;
	 if(settings.UseTrendDetection == false || sc.Subgraph[TREND_FOR_PATTERNS][index] == CANDLESTICK_TREND_UP)	// uptrend
	 {
		 // close prices are equal
		 if(IsNearEqual(InData[SC_LAST][index], InData[SC_LAST][index-1], InData, index, k_BearishCounterattackLine_SimularityPercent))
		 {
			 // check for the colors contrast 
			 if (IsWhiteCandle(InData, index-1)
				 &&IsBodyStrong(InData,index-1))
			 {
				 if(IsBlackCandle(InData, index)
					 && IsBodyStrong(InData,index))
				 {
					 ret_flag = true;
				 }
			 }
			 /*else if (IsBlackCandle(InData, index-1))
			 {
			 if(IsWhiteCandle(InData, index))
			 {
			 ret_flag = true;
			 }
			 }*/
		 }
	 }
	 return ret_flag;
 }
 //////////////////////////////////////////////////////////////////////////////////////////////
const double k_BullishCounterattackLine_SimularityPercent = 7.0;

 bool IsBullishCounterattackLine(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index)
 {
	 // BULLISH COUNTERATTACK LINE
	 // 1. Downtrend
	 // 2. The candles [index] and [index-1]  are of contrast colors and their close prices are equal 
	 //Bullish Counterattack Line
	 //In a downtrending market, a large black candlestick is following by a large white candlestick that opens on a big gap lower and then rallies during the period to close at the same price as the previous close. The bullish white candlestick needs followup action to the upside to confirm the turn to an uptrend. 
	 SCBaseDataRef InData = sc.BaseData;
	 bool ret_flag = false;
	 if(settings.UseTrendDetection == false || sc.Subgraph[TREND_FOR_PATTERNS][index] == CANDLESTICK_TREND_DOWN)	// downtrend
	 {
		 // close prices are equal 
		 if(IsNearEqual(InData[SC_LAST][index], InData[SC_LAST][index-1], InData, index, k_BullishCounterattackLine_SimularityPercent))
		 {
			 // check for the colors contrast
			 /*if (IsWhiteCandle(InData, index-1))
			 {
			 if(IsBlackCandle(InData, index))
			 {
			 ret_flag = true;
			 }
			 }
			 else*/ if (IsBlackCandle(InData, index-1)
				 && IsBodyStrong(InData,index-1))
			 {
				 if(IsWhiteCandle(InData, index)
					 && IsBodyStrong(InData,index))
				 {
					 ret_flag = true;
				 }
			 }
		 }
	 }
	 return ret_flag;
 }
 //////////////////////////////////////////////////////////////////////////////////////////////
 
 bool IsThreeInsideUp(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index)
 {
	 // THREE INSIDE UP
	 // 1. Downtrend
	 // 2. The candle [index] is white and the close price higher than the close price of the candle [index-1] 
	 // 3. The candles [index-2] and [index-3] form Bullish Harami Pattern
	 SCBaseDataRef InData = sc.BaseData;
	 bool ret_flag = false;
	 if(settings.UseTrendDetection == false || sc.Subgraph[TREND_FOR_PATTERNS][index] == CANDLESTICK_TREND_DOWN)	// downtrend
	 {
		 if( IsWhiteCandle(InData,index) &&	// the candle 0 is white 
			 (InData[SC_LAST][index]>InData[SC_LAST][index-1]))	// the close price of the candle 0 is higher than the close price of the candle -1
		 {
			 if(IsBullishHarami(sc,settings,index-1))
			 {
				 ret_flag = true;
			 }
		 }
	 }
	 return ret_flag;
 }
 //////////////////////////////////////////////////////////////////////////////////////////////
 
 bool IsThreeOutsideUp(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index)
 {
	 // THREE OUTSIDE UP
	 // 1. Downtrend
	 // 2. The candle [index] is white, and its close price is higher than the close price of the candle [index-1] 
	 // 3. Candles [index-2] and [index-3] form Bullish Engulfing Pattern 
	 SCBaseDataRef InData = sc.BaseData;
	 bool ret_flag = false;
	 if(settings.UseTrendDetection == false || sc.Subgraph[TREND_FOR_PATTERNS][index] == CANDLESTICK_TREND_DOWN)	// downtrend
	 {
		 if( IsWhiteCandle(InData, index) &&	// the candle 0 is white 
			 (InData[SC_LAST][index]>InData[SC_LAST][index-1]))	// CLOSE of the candle 0 is higher than CLOSE of the candle -1 
		 {
			 if(IsBullishEngulfing(sc,settings,index-1))
			 {
				 ret_flag = true;
			 }
		 }
	 }
	 return ret_flag;
 }
 //////////////////////////////////////////////////////////////////////////////////////////////

 bool IsThreeInsideDown(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index)
 {
	 // THREE INSIDE DOWN
	 // 1. Uptrend
	 // 2. The candle [index] is black, and its close price is lower than the close price of the the candle [index-1] 
	 // 3. The candles [index-2] and [index-3] form Bearish Harami Pattern
	 SCBaseDataRef InData = sc.BaseData;
	 bool ret_flag = false;
	 if(settings.UseTrendDetection == false || sc.Subgraph[TREND_FOR_PATTERNS][index] == CANDLESTICK_TREND_UP)	// uptrend
	 {
		 if(IsBlackCandle(InData, index) &&	// the candle 0 is black 
			 (InData[SC_LAST][index]<InData[SC_LAST][index-1]))	// CLOSE of the candle 0 is lower than CLOSE of the candle -1 
		 {
			 if(IsBearishHarami(sc,settings,index-1))
			 {
				 ret_flag = true;
			 }
		 }
	 }
	 return ret_flag;
 }
 //////////////////////////////////////////////////////////////////////////////////////////////

 bool IsThreeOutsideDown(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index)
 {
	 // THREE OUTSIDE DOWN
	 // 1. Uptrend
	 // 2. The candle [index] is black, and its close price is lower than close price of the candle [index-1] 
	 // 3. The candles [index-2] and [index-3] form Bearish Engulfing Pattern 
	 SCBaseDataRef InData = sc.BaseData;
	 bool ret_flag = false;
	 if(settings.UseTrendDetection == false || sc.Subgraph[TREND_FOR_PATTERNS][index] == CANDLESTICK_TREND_UP)	// uptrend
	 {
		 if(IsBlackCandle(InData, index) &&	// the candle 0 is black 
			 (InData[SC_LAST][index]<InData[SC_LAST][index-1]))	// CLOSE of the candle 0 is lower than CLOSE of the candle -1 
		 {
			 if(IsBearishEngulfing(sc,settings,index-1))
			 {
				 ret_flag = true;
			 }
		 }
	 }
	 return ret_flag;
 }
 //////////////////////////////////////////////////////////////////////////////////////////////
 const double k_Kicker_UpperWickPercent = 7.0;
 const double k_Kicker_LowerWickPercent = 7.0;

 bool IsKicker(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index)
 {
	 // KICKER
	 // 1. The candle [index-1] is black, without wicks 
	 // 2. The candle [index] is white, without wicks
	 // 3. The bodies of the candles [index] and [index-1] are strong
	 // 4. A gap between the bodies of the candles [index] and [index-1]
	 SCBaseDataRef InData = sc.BaseData;
	 bool ret_flag = false;
	 // the body of the candle -1 is black and without wicks 
	 if (IsBlackCandle(InData, index-1) &&
		 IsUpperWickSmall(InData, index-1, k_Kicker_UpperWickPercent) &&
		 IsLowerWickSmall(InData, index-1, k_Kicker_LowerWickPercent))
	 {
		 // the body of the candle 0 is white and without wicks 
		 if (IsWhiteCandle(InData, index) &&
			 IsUpperWickSmall(InData, index, k_Kicker_UpperWickPercent) &&
			 IsLowerWickSmall(InData, index, k_Kicker_LowerWickPercent))
		 {
			 if(IsBodyStrong(InData,index-1))	// the body of the candle -1 is strong 
			 {
				 if(IsBodyStrong(InData,index))	// the body of the candle 0 is strong 
				 {
					 //  a gap between bodies 
					 if(InData[SC_OPEN][index-1]<InData[SC_OPEN][index])
					 {
						 ret_flag = true;
					 }
				 }
			 }
		 }
	 }
	 return ret_flag;
 }

 //////////////////////////////////////////////////////////////////////////////////////////////
 const double k_Kicking_UpperWickPercent = 7.0;
 const double k_Kicking_LowerWickPercent = 7.0;

 bool IsKicking(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index)
 {
	 // KICKING
	 // 1. The candle [index-1] is white and without wicks
	 // 2. The candle [index] is black and without wicks
	 // 3. Bodies of the candles [index] and [index-1] are strong
	 // 4. A gap between the bodies of the candles [index] and [index-1] 
	 SCBaseDataRef InData = sc.BaseData;
	 bool ret_flag = false;
	 // the body of the candle -1 is white and without wicks 
	 if (IsWhiteCandle(InData, index-1) &&
		 IsUpperWickSmall(InData, index-1, k_Kicking_UpperWickPercent) &&
		 IsLowerWickSmall(InData, index-1, k_Kicking_LowerWickPercent))
	 {
		 // the body of the candle 0 is black and without wicks
		 if (IsBlackCandle(InData, index) &&
			 IsUpperWickSmall(InData, index, k_Kicking_UpperWickPercent) &&
			 IsLowerWickSmall(InData, index, k_Kicking_LowerWickPercent))
		 {
			 if(IsBodyStrong(InData,index-1))	// the body of the candle -1 is strong 
			 {
				 if(IsBodyStrong(InData,index))	// the body of the candle 0 is strong
				 {
					 // a gap between the bodies 
					 if(InData[SC_OPEN][index]<InData[SC_OPEN][index-1])
					 {
						 ret_flag = true;
					 }
				 }
			 }
		 }
	 }
	 return ret_flag;
 }
 //////////////////////////////////////////////////////////////////////////////////////////////
 
 const double k_ThreeWhiteSoldiers_UpperWickPercent = 7.0;

 bool IsThreeWhiteSoldiers(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index)
 {
	 // THREE WHITE SOLDIERS
	 // 1. Downtrend till the candle 0 
	 // 2. The candles [index], [index-1] and [index-2] are white
	 // 3. OPENs of candles [index], [index-1] are inside the bodies of the previous candles
	 // 4. Upper wicks of the candles [index], [index-1] and [index-2] are insignificant
	 SCBaseDataRef InData = sc.BaseData;
	 bool ret_flag = false;
	 if(	settings.UseTrendDetection == false || sc.Subgraph[TREND_FOR_PATTERNS][index] == CANDLESTICK_TREND_DOWN)	// Downtrend
	 {
		 if (IsWhiteCandle(InData,index) &&		// the candle 0 is white 
			 IsWhiteCandle(InData, index-1) &&	// the candle -1 is white
			 IsWhiteCandle(InData, index-2))	// the candle -2 is white 
		 {
			 // OPEN of the candle 0 inside the body of the candle -1 
			 if ((InData[SC_OPEN][index]<=InData[SC_LAST][index-1])&&
				 (InData[SC_OPEN][index]>=InData[SC_OPEN][index-1]))
			 {
				 // OPEN of the candle -1 inside the body of the candle -2 
				 if ((InData[SC_OPEN][index-1]<=InData[SC_LAST][index-2])&&
					 (InData[SC_OPEN][index-1]>=InData[SC_OPEN][index-2]))
				 {
					 // check for upper wicks 
					 if(IsUpperWickSmall(InData, index, k_ThreeWhiteSoldiers_UpperWickPercent) &&
						 IsUpperWickSmall(InData, index-1, k_ThreeWhiteSoldiers_UpperWickPercent)&&
						 IsUpperWickSmall(InData, index-2, k_ThreeWhiteSoldiers_UpperWickPercent)
						 )
					 {
						 ret_flag = true;
					 }
				 }

			 }
		 }
	 }
	 return ret_flag;
 }
 //////////////////////////////////////////////////////////////////////////////////////////////
 

 bool IsAdvanceBlock(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index)
 {
	 // ADVANCE BLOCK 
	 // 1. Uptrend till the candle 0
	 // 2. The candles [index], [index-1] and [index-2] are white
	 // 3. Opens of candles [index], [index-1] are inside the bodies of the previous candles 
	 // 4. The body of the candle [index] is smaller than the body of the candle [index-1], 
	 //and the body of the candle [index-1] is smaller than the body of the candle [index-2]
	 SCBaseDataRef InData = sc.BaseData;
	 bool ret_flag = false;
	 if(settings.UseTrendDetection == false || sc.Subgraph[TREND_FOR_PATTERNS][index] == CANDLESTICK_TREND_UP)	// Uptrend
	 {
		 if (IsWhiteCandle(InData, index) &&	// the candle 0 is white
			 IsWhiteCandle(InData, index-1) &&	// the candle -1 is white
			 IsWhiteCandle(InData, index-2))	// the candle -2 is white
		 {
			 // OPEN of the candle 0 is inside the body of the candle -1
			 if ((InData[SC_OPEN][index]<=InData[SC_LAST][index-1])&&
				 (InData[SC_OPEN][index]>=InData[SC_OPEN][index-1]))
			 {
				 // OPEN of the candle -1 is inside the body of the candle -2
				 if ((InData[SC_OPEN][index-1]<=InData[SC_LAST][index-2])&&
					 (InData[SC_OPEN][index-1]>=InData[SC_OPEN][index-2]))
				 {
					 // check for the size of the body
					 if(BodyLength(InData,index) < BodyLength(InData, index-1) &&
						 BodyLength(InData, index-1) < BodyLength(InData, index-2))
					 {
						 ret_flag = true;
					 }
				 }

			 }
		 }
	 }
	 return ret_flag;
 }
 //////////////////////////////////////////////////////////////////////////////////////////////
 
 const int    k_Deliberation_UpperWickPercent = 7;

 bool IsDeliberation(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index)
 {
	 // DELIBERATION
	 // 1. Uptrend till the candle 0
	 // 2. The candles [index], [index-1] and [index-2] are white
	 // 3. OPENs of the candle [index-1] is inside the body of the previous candle
	 // 4. The candle [index-1] has a strong body
	 // 5. Candle [index] has a weak body
	 SCBaseDataRef InData = sc.BaseData;
	 bool ret_flag = false;
	 if(settings.UseTrendDetection == false || sc.Subgraph[TREND_FOR_PATTERNS][index] == CANDLESTICK_TREND_UP)	// uptrend
	 {
		 if (IsWhiteCandle(InData, index) &&		// Candle 0 is white
			 IsWhiteCandle(InData, index-1) &&  	// Candle -1 is white
			 IsWhiteCandle(InData, index-2))    	// Candle -2 is white
		 {
			 // OPEN of the candle 0 is inside of the body of the candle -1
			 /*if ((InData[SC_OPEN][index]<InData[SC_LAST][index-1])&&
			 (InData[SC_OPEN][index]>InData[SC_OPEN][index-1]))*/
			 {
				 // OPEN of the candle -1 is inside of the body of the candle -2
				 if ((InData[SC_OPEN][index-1]<=InData[SC_LAST][index-2])&&
					 (InData[SC_OPEN][index-1]>=InData[SC_OPEN][index-2]))
				 {
					 // body of the candle -1 is strong
					 if(IsBodyStrong(InData,index-1))
					 {
						 // candle -1 does not have an upper wick
						 //if (IsUpperWickSmall(InData, index-1, k_Deliberation_UpperWickPercent))
						 {
							 // body of the candle 0 is weak
							 if(!IsBodyStrong(InData,index))
							 {
								 ret_flag = true;
							 }
						 }						
					 }
				 }

			 }
		 }
	 }
	 return ret_flag;
 }
 //////////////////////////////////////////////////////////////////////////////////////////////

 const int    k_BearishTriStar_PAST_INDEX = 3;

 bool IsBearishTriStar(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index)
 {
	 // BEARISH TRI STAR
	 //1. Market is characterized by uptrend.
	 //2. We see three Dojis on three consecutive days.
	 //3. The second day Doji has a gap above the first and third.

	 SCBaseDataRef InData = sc.BaseData;
	 bool ret_flag = false;
	 // the candle 0 is Doji
	 if((settings.UseTrendDetection == false || sc.Subgraph[TREND_FOR_PATTERNS][index] == CANDLESTICK_TREND_UP)
		 && IsDoji(sc, settings, index)
		 && IsDoji(sc, settings, index-1)
		 && IsDoji(sc, settings, index-2)
		 && (InData[SC_OPEN][index-1] > InData[SC_LAST][index])
		 && (InData[SC_OPEN][index-1] > InData[SC_LAST][index-2])
		 )
	 {
		 ret_flag = true;
	 }
	 return ret_flag;
 }
 //////////////////////////////////////////////////////////////////////////////////////////////
 const int    k_BullishTriStar_PAST_INDEX = 3;

 bool IsBullishTriStar(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index)
 {
	 // BULLISH TRI STAR
	 //1. Market is characterized by downtrend.
	 //2. Then we see three consecutive Doji.
	 //3. The second day Doji gaps below the first and third.
	 SCBaseDataRef InData = sc.BaseData;
	 bool ret_flag = false;
	 // the candle 0 is Doji
	 if((settings.UseTrendDetection == false || sc.Subgraph[TREND_FOR_PATTERNS][index] == CANDLESTICK_TREND_DOWN)
		 && IsDoji(sc, settings, index)
		 && IsDoji(sc, settings, index-1)
		 && IsDoji(sc, settings, index-2)
		 && (InData[SC_OPEN][index-1] < InData[SC_LAST][index])
		 && (InData[SC_OPEN][index-1] < InData[SC_LAST][index-2])
		 )
	 {
		 ret_flag = true;
	 }
	 return ret_flag;
 }
 //////////////////////////////////////////////////////////////////////////////////////////////

 bool IsUniqueThreeRiverBottom(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index)
 {
	 // UNIQUE THREE RIVER BOTTOM
	 // 1. Downtrend till the candle [index-2]
	 // 2. The candle [index-2] is black and with a strong body
	 // 3. The candle [index-1] forms HAMMER PATERN
	 // 4. The minimum of the candle [index-1] is smaller than the minimum of the candles [index-2] and [index]
	 // 5. The candle [index] has a weak body that lower than the body of the candle [index-1]
	 SCBaseDataRef InData = sc.BaseData;
	 bool ret_flag = false;
	 if(settings.UseTrendDetection == false || sc.Subgraph[TREND_FOR_PATTERNS][index] == CANDLESTICK_TREND_DOWN)	// Downtrend
	 {
		 if(InData[SC_LAST][index-2]<InData[SC_OPEN][index-2]) //the candle -2 is black
		 {
			 if(IsBodyStrong(InData,index-2))	//the body of the candle -2 is strong
			 {
				 if(IsHammer(sc,settings,index-1))	// The candle -1 is HAMMER
				 {
					 // lower wick of the candle -1 forms new minimum
					 if( (InData[SC_LOW][index-1]<InData[SC_LOW][index])&&
						 (InData[SC_LOW][index-1]<InData[SC_LOW][index-2]))
					 {
						 if(IsWhiteCandle(InData, index)) // the candle 0 is white
						 {
							 if(!IsBodyStrong(InData,index))	// the body of the candle 0 is weak
							 {
								 if( (InData[SC_OPEN][index]<InData[SC_OPEN][index-1])&&
									 (InData[SC_OPEN][index]<InData[SC_LAST][index-1]))	
									 // the body of the candle 0 is lower than the body of the candle -1
								 {
									 ret_flag = true;
								 }
							 }
						 }
					 }
				 }
			 }
		 }
	 }
	 return ret_flag;
 }
 //////////////////////////////////////////////////////////////////////////////////////////////
 const double k_BearishDojiStar_UpperWickPercent = 25.0;
 const double k_BearishDojiStar_LowerWickPercent = 25.0;

 bool IsBearishDojiStar(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index)
 {
	 // BEARISH DOJI STAR
	 // 1. Uptrend
	 // 2. A strong white body of the candle [index-1]
	 // 3. The candle [index] is Doji
	 // 4. The body of the candle [index] is higher than the maximum of the candle [index-1]
	 // 5. The wicks of the candles are "not long"
	 SCBaseDataRef InData = sc.BaseData;
	 bool ret_flag = false;
	 if(settings.UseTrendDetection == false || sc.Subgraph[TREND_FOR_PATTERNS][index] == CANDLESTICK_TREND_UP)	// check for the uptrend
	 {
		 if(IsWhiteCandle(InData, index-1))	// the body of the candle -1 is white
		 {
			 if(IsBodyStrong(InData,index-1))	// the body of the candle -1 is strong
			 {
				 if (IsDoji(sc, settings, index)) // the candle 0 is Doji
				 {
					 if (InData[SC_LAST][index] > InData[SC_HIGH][index-1]) 
						 // the body of the candle 0 is higher than the maximum of the candle -1
					 {
						 // check for the wicks length 
						 if (UpperWickLength(InData, index)<=PercentOfCandleLength(InData, index-1, k_BearishDojiStar_UpperWickPercent) &&	    // upper wick of Doji
							 LowerWickLength(InData, index)<=PercentOfCandleLength(InData, index-1, k_BearishDojiStar_LowerWickPercent)) 
						 {
							 ret_flag = true;	
						 }						
					 }
				 }
			 }
		 }
	 }
	 return ret_flag;
 }
 //////////////////////////////////////////////////////////////////////////////////////////////
 const double k_BullishDojiStar_LowerWickPercent = 25.0;
 const double k_BullishDojiStar_UpperWickPercent = 25.0;

 bool IsBullishDojiStar(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index)
 {
	 // BULLISH DOJI STAR
	 // 1. Downtrend
	 // 2. Strong black body of the candle [index-1]
	 // 3. The candle [index] is Doji
	 // 4. The body of the candle [index] is lower than minimum of the candle [index-1]
	 // 5. The wicks of the candle [index] are "not long"
	 SCBaseDataRef InData = sc.BaseData;
	 bool ret_flag = false;
	 if(settings.UseTrendDetection == false || sc.Subgraph[TREND_FOR_PATTERNS][index] == CANDLESTICK_TREND_DOWN)	// check for the downtrend
	 {
		 if(IsBlackCandle(InData, index-1))	// the body of the candle -1 is black
		 {
			 if(IsBodyStrong(InData,index-1))	// the body of the candle -1 is strong
			 {
				 if (IsDoji(sc, settings, index)) // the candle 0 is Doji
				 {
					 if (InData[SC_LAST][index] < InData[SC_LOW][index-1]) 
						 // the body of the candle 0 is lower than minimum of the candle -1
					 {
						 // check for the wicks length 
						 if (UpperWickLength(InData, index)<=PercentOfCandleLength(InData, index-1, k_BearishDojiStar_UpperWickPercent) &&	    // upper wick of Doji
							 LowerWickLength(InData, index)<=PercentOfCandleLength(InData, index-1, k_BearishDojiStar_LowerWickPercent))      // lower wick of Doji
							 
						 {
							 ret_flag = true;	
						 }						
					 }
				 }
			 }
		 }
	 }
	 return ret_flag;
 }
 //////////////////////////////////////////////////////////////////////////////////////////////
 const double k_BearishDragonflyDoji_UpperWickPercent = 7.0;

 bool IsBearishDragonflyDoji(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index)
 {
	 // BEARISH DRAGONFLY DOJI
	 // 1. Uptrend
	 // 2. The candle [index] is Doji
	 // 3. The candle [index] does not have an upper wick
	 // 4. The lower wick of the candle [index] is very long
	 SCBaseDataRef InData = sc.BaseData;
	 bool ret_flag = false;
	 if(settings.UseTrendDetection == false || sc.Subgraph[TREND_FOR_PATTERNS][index] == CANDLESTICK_TREND_UP)	// check for the uptrend
	 {
		 if (IsDoji(sc, settings, index)) // the candle 0 is Doji
		 {
			 if (IsUpperWickSmall(InData, index, k_BearishDragonflyDoji_UpperWickPercent)) //the candle 0 does not have an upper wick
			 {
				 // The candle 0 has a long lower wick
				 //
				 // Note: 
				 //   if body is small and upper wick is small while the candle is
				 //   long => the lower wick is long
				 if (IsCandleStrength(InData,index))
				 {
					 ret_flag = true;	
				 }
			 }
		 }
	 }
	 return ret_flag;
 }
 //////////////////////////////////////////////////////////////////////////////////////////////

 const double k_BullishDragonflyDoji_UpperWickPercent = 7.0;

 bool IsBullishDragonflyDoji(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index)
 {
	 // BULLISH DRAGONFLY DOJI
	 // 1. Downtrend
	 // 2. The candle [index] is Doji 
	 // 3. The candle [index] does not have an upper wick
	 // 4. The lower wick of the candle [index] is very long
	 SCBaseDataRef InData = sc.BaseData;
	 bool ret_flag = false;
	 if(settings.UseTrendDetection == false || sc.Subgraph[TREND_FOR_PATTERNS][index] == CANDLESTICK_TREND_DOWN)	// check for the downtrend
	 {
		 if (IsDoji(sc, settings, index)) // the candle 0 is Doji
		 {
			 if (IsUpperWickSmall(InData, index, k_BullishDragonflyDoji_UpperWickPercent)) //the candle 0 does not have an upper wick
			 {
				 // The candle 0 has a long lower wick
				 //
				 // Note: 
				 //   if body is small and upper wick is small while the candle is
				 //   long => the lower wick is long
				 if (IsCandleStrength(InData,index))
				 {
					 ret_flag = true;	
				 }
			 }
		 }
	 }
	 return ret_flag;
 }
 //////////////////////////////////////////////////////////////////////////////////////////////
 
 const double k_BearishGravestoneDoji_LowerWickPercent = 7.0;

 bool IsBearishGravestoneDoji(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index)
 {
	 // BEARISH GRAVESTONE DOJI
	 // 1. Uptrend
	 // 2. The candle [index-1] has a white body
	 // 3. The candle [index] is Doji
	 // 4. The lower wick of the candle [index] is insignificant
	 // 5. Upper wick of the candle [index] is very long
	 // 6. OPEN of the candle [index] is higher than the maximum of the candle [index-1]
	 SCBaseDataRef InData = sc.BaseData;
	 bool ret_flag = false;
	 if(settings.UseTrendDetection == false || sc.Subgraph[TREND_FOR_PATTERNS][index] == CANDLESTICK_TREND_UP)	// check for the uptrend
	 {
		 if(IsWhiteCandle(InData, index-1))	// the body of the candle -1 is white
		 {
			 if (IsDoji(sc, settings, index)) // the candle [index] is Doji
			 {
				 // lower wick of the candle 0 is insignificant
				 if (IsLowerWickSmall(InData, index, k_BearishGravestoneDoji_LowerWickPercent))
				 {
					 // upper wick of the candle 0 is long (please, note that it's Doji, so there is no body)
					 if (IsCandleStrength(InData,index))
					 {
						 // OPEN of the candle 0 is higher than the maximum of the candle -1
						 if(InData[SC_OPEN][index] > InData[SC_HIGH][index-1])
						 {
							 ret_flag = true;
						 }						
					 }
				 }
			 }			
		 }
	 }
	 return ret_flag;	
 }
 //////////////////////////////////////////////////////////////////////////////////////////////

 const double k_BullishGravestoneDoji_LowerWickPercent = 7;

 bool IsBullishGravestoneDoji(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index)
 {
	 // BULLISH GRAVESTONE DOJI
	 // 1. Downtrend
	 // 2. The candle [index-1] has a black body
	 // 3. The candle [index] is Doji
	 // 4. The candle [index] does not have a lower wick
	 // 5. The upper wick of the candle [index] is very long
	 // 6. Absence of the gap between the candles [index] and [index-1] is necessary
	 SCBaseDataRef InData = sc.BaseData;
	 bool ret_flag = false;
	 if(settings.UseTrendDetection == false || sc.Subgraph[TREND_FOR_PATTERNS][index] == CANDLESTICK_TREND_DOWN)	// check for the downtrend
	 {
		 if(IsBlackCandle(InData, index-1))	// the body of the candle -1 is black
		 {
			 if (IsDoji(sc, settings,index)) // the candle 0 is Doji
			 {
				 // candle 0 does not have a lower wick
				 if (IsLowerWickSmall(InData, index, k_BullishGravestoneDoji_LowerWickPercent))
				 {
					 // candle 0 has a long upper wick 
					 //
					 // Note: 
					 //   if body is small and lower wick is small and the candle is
					 //   long => the lower wick is long
					 //   (please, note that it's Doji, so there is no body)
					 if (IsCandleStrength(InData,index))
					 {
						 // check for the absence of the gap
						 if (InData[SC_LOW][index-1]<InData[SC_HIGH][index])
						 {
							 ret_flag = true;	
						 }						
					 }
				 }
			 }
		 }
	 }
	 return ret_flag;
 }
 //////////////////////////////////////////////////////////////////////////////////////////////

 const double k_BearishLongleggedDoji_SimularityPercent = 10.0;

 bool IsBearishLongleggedDoji(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index)
 {
	 // BEARISH LONGLEGGED DOJI
	 // 1. Uptrend
	 // 2. The candle [index] is Doji
	 // 3. The candle [index] is long
	 // 4. The wicks of the candle [index] are nearly the same
	 // 5. The body of the candle [index] is higher than the maximum of the candle [index-1]
	 SCBaseDataRef InData = sc.BaseData;
	 bool ret_flag = false;
	 if(settings.UseTrendDetection == false || sc.Subgraph[TREND_FOR_PATTERNS][index] == CANDLESTICK_TREND_UP)	// check for the uptrend
	 {
		 if (IsDoji(sc, settings, index)) // the candle 0 is Doji
		 {
			 // the candle 0 is long
			 if (IsCandleStrength(InData,index))
			 {
				 // the body of the candle 0 is higher than the maximum of the candle -1
				 if (max(InData[SC_LAST][index], InData[SC_OPEN][index]) > InData[SC_HIGH][index-1])
				 {
					 // the wicks of the candle 0 are nearly the same
					 if (abs(LowerWickLength(InData, index) - UpperWickLength(InData, index))<
						 PercentOfCandleLength(InData, index, k_BearishLongleggedDoji_SimularityPercent))
					 {
						 ret_flag = true;	
					 }					
				 }				
			 }			
		 }
	 }
	 return ret_flag;
 }
 //////////////////////////////////////////////////////////////////////////////////////////////

 const double k_BullishLongleggedDoji_SimularityPercent = 10.0;

 bool IsBullishLongleggedDoji(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index)
 {
	 // BULLISH LONGLEGGED DOJI
	 // 1. Downtrend
	 // 2. The candle [index] is Doji
	 // 3. The candle [index] is long
	 // 4. The wicks of the candle [index] are nearly the same
	 // 5. The body of the candle [index] is lower than the minimum of the candle [index-1]
	 SCBaseDataRef InData = sc.BaseData;
	 bool ret_flag = false;
	 if(settings.UseTrendDetection == false || sc.Subgraph[TREND_FOR_PATTERNS][index] == CANDLESTICK_TREND_DOWN)	// check for the downtrend
	 {
		 if (IsDoji(sc, settings, index)) // the candle 0 is Doji
		 {
			 // The candle 0 is long
			 if (IsCandleStrength(InData,index))
			 {
				 // the body of the candle 0 is lower than the minimum of the candle -1
				 if (max(InData[SC_LAST][index], InData[SC_OPEN][index]) < InData[SC_LOW][index-1])
				 {
					 // the wicks of the candle 0 are nearly the same
					 if (abs(LowerWickLength(InData, index) - UpperWickLength(InData, index))<
						 PercentOfCandleLength(InData, index, k_BullishLongleggedDoji_SimularityPercent))
					 {
						 ret_flag = true;	
					 }					
				 }				
			 }			
		 }
	 }
	 return ret_flag;
 }
 //////////////////////////////////////////////////////////////////////////////////////////////
 //const int    k_BearishSideBySideWhiteLines_TrendArea = 3;
 const int    k_BearishSideBySideWhiteLines_PAST_INDEX = 3;
 const double k_BearishSideBySideWhiteLines_OpenSimularityPercent = 14.0;
 const double k_BearishSideBySideWhiteLines_CandleSimularityPercent = 14.0;

 bool IsBearishSideBySideWhiteLines(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index)
 {
	 // BEARISH SIDE-BY-SIDE WHITE LINES
	 // 1. Downtrend
	 // 2. The candles [index], [index-1] are white
	 // 3. The candle [index-2] is black
	 // 4. The maximum of the candle [index-1] is lower than the minimum of the candle [index-2]
	 // 5. Open prices and the sizes of the candles [index-1] and [index] are ALMOST the same

	 /*	1. 1st day is a black day.
	 2. 2nd day is a white day which gaps below the 1st day's open.
	 3. 3rd day is a white day about the same size as the 2nd day, opening at about the same price.*/

	 SCBaseDataRef InData = sc.BaseData;
	 bool ret_flag = false;
	 if(settings.UseTrendDetection == false || sc.Subgraph[TREND_FOR_PATTERNS][index] == CANDLESTICK_TREND_DOWN)	// check for the downtrend
	 {
		 if (IsWhiteCandle(InData, index) &&		// the candle 0 is white
			 IsWhiteCandle(InData, index-1) &&	    // the candle -1 is white
			 IsBlackCandle(InData, index-2))		// the candle -2 is black
		 {
			 //  the candle -2 is higher than the candle -1 with a gap
			 if (InData[SC_LOW][index-2] > InData[SC_HIGH][index-1])
			 {
				 // compare of the OPENS of the candles 0 and -1 
				 if (IsNearEqual(InData[SC_OPEN][index-1], InData[SC_OPEN][index], InData, index, k_BearishSideBySideWhiteLines_OpenSimularityPercent))
				 {
					 // compare of the length of the candles 0 and -1
					 if (IsNearEqual(CandleLength(InData,index), CandleLength(InData, index-1), InData, index, k_BearishSideBySideWhiteLines_CandleSimularityPercent))
					 {
						 ret_flag = true;	
					 }					
				 }				
			 }			
		 }
	 }
	 return ret_flag;
 }
 //////////////////////////////////////////////////////////////////////////////////////////////
 //const int    k_BullishSideBySideWhiteLines_TrendArea = 3;
 const int    k_BullishSideBySideWhiteLines_PAST_INDEX = 3;
 const double k_BullishSideBySideWhiteLines_OpenSimularityPercent = 7.0;
 const double k_BullishSideBySideWhiteLines_CandleSimularityPercent = 7.0;

 bool IsBullishSideBySideWhiteLines(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index)
 {
	 // BULLISH SIDE-BY-SIDE WHITE LINES
	 // 1. Uptrend
	 // 2. The candles [index], [index-1] and [index-2] are white
	 // 3. Maximum of the candle [index-2] is lower than minimum of the candle [index-1]
	 // 4. Open prices and the sizes of the candles [index-1] and [index] are ALMOST the same

	 /*  1. Market is characterized by uptrend.
	 2. We see a white candlestick in the first day.
	 3. Then we see another white candlestick on the second day with an upward gap.
	 4. Finally, we see a white candlestick on the third day characterized by the same body length and whose closing price is equal to the close of the second day and a new high is established.*/

	 SCBaseDataRef InData = sc.BaseData;
	 bool ret_flag = false;
	 if(
		 (settings.UseTrendDetection == false || sc.Subgraph[TREND_FOR_PATTERNS][index] == CANDLESTICK_TREND_UP)	// check for the uptrend
		 && IsWhiteCandle(InData, index)
		 && IsWhiteCandle(InData, index-1)
		 && IsWhiteCandle(InData, index-2)
		 && (InData[SC_LOW][index-1] > InData[SC_HIGH][index-2])
		 && (IsNearEqual(InData[SC_LAST][index-1], InData[SC_LAST][index], InData, index, k_BullishSideBySideWhiteLines_OpenSimularityPercent))
		 && (IsNearEqual(InData[SC_OPEN][index-1], InData[SC_OPEN][index], InData, index, k_BullishSideBySideWhiteLines_OpenSimularityPercent))
		 && (InData[SC_HIGH][index] > InData[SC_HIGH][index-1])
			)
	 {
		 ret_flag = true;	

	 }
	 return ret_flag;
 }
 //////////////////////////////////////////////////////////////////////////////////////////////

 const int    k_FallingThreeMethods_PAST_INDEX = 3;
 const double k_FallingThreeMethods_CandleCloseClosenessPercent = 30.0;

 bool IsFallingThreeMethods(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index)
 {
	 // FALLING THREE METHODS
	 
	 SCBaseDataRef InData = sc.BaseData;
	 bool ret_flag = false;

	 // 1. Market is characterized by downtrend.
	 // 2. We see a long black candlestick in the first day.
	 // 3. We then see three small real bodies defining a brief uptrend on the second, third, and fourth days. However these bodies stay within the range of the first day.
	 // 4. Finally we see a long black candlestick on the fifth day opening near the previous day’s close and also closing below the close of the initial day to define a new low.


	 if(
		 (settings.UseTrendDetection == false || sc.Subgraph[TREND_FOR_PATTERNS][index] == CANDLESTICK_TREND_DOWN)

		 && IsBodyStrong(InData,index-4)
		 && IsBlackCandle(InData, index-4)

		 //&& IsWhiteCandle(InData, index-1)
		 //&& IsWhiteCandle(InData, index-2)
		 //&& IsWhiteCandle(InData, index-3)
		 && !IsBodyStrong(InData,index-1)
		 && !IsBodyStrong(InData,index-2)
		 && !IsBodyStrong(InData,index-3)
		 && (InData[SC_OHLC][index-3] < InData[SC_OHLC][index-2])
		 && (InData[SC_OHLC][index-2] < InData[SC_OHLC][index-1])

		 && (InData[SC_HIGH][index-3] < InData[SC_HIGH][index-4])
		 && (InData[SC_HIGH][index-2] < InData[SC_HIGH][index-4])
		 && (InData[SC_HIGH][index-1] < InData[SC_HIGH][index-4])
		 && (InData[SC_LOW][index-3] > InData[SC_LOW][index-4])
		 && (InData[SC_LOW][index-2] > InData[SC_LOW][index-4])
		 && (InData[SC_LOW][index-1] > InData[SC_LOW][index-4])

		 && IsBodyStrong(InData,index)
		 && IsBlackCandle(InData, index)
		 && (IsNearEqual(InData[SC_OPEN][index], InData[SC_LAST][index-1], InData, index, k_FallingThreeMethods_CandleCloseClosenessPercent))
		 && (InData[SC_LAST][index] < InData[SC_LAST][index-4])
		 )
	 {
		 ret_flag = true;
	 }


	 return ret_flag;
 }
 //////////////////////////////////////////////////////////////////////////////////////////////

 const int    k_RisingThreeMethods_PAST_INDEX = 3;

 bool IsRisingThreeMethods(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index)
 {
	 // RISING THREE METHODS
	 
	 //  . Market is characterized by downtrend.
	 // 1. 1st day is a long white day.
	 // 2. Three small body candlesticks follow the 1st day. Each trends downward and closes within the range of the 1st day.
	 // 3. The last day is a long white day and closes above the 1st day's close.
	 SCBaseDataRef InData = sc.BaseData;
	 bool ret_flag = false;
	
	 if(
		 (settings.UseTrendDetection == false || sc.Subgraph[TREND_FOR_PATTERNS][index] == CANDLESTICK_TREND_UP)

		 && IsBodyStrong(InData,index-4)
		 && IsWhiteCandle(InData, index-4)

		 && !IsBodyStrong(InData,index-1)
		 && !IsBodyStrong(InData,index-2)
		 && !IsBodyStrong(InData,index-3)
		 && (InData[SC_OHLC][index-3] > InData[SC_OHLC][index-2])
		 && (InData[SC_OHLC][index-2] > InData[SC_OHLC][index-1])

		 && (InData[SC_HIGH][index-3] <= InData[SC_HIGH][index-4])
		 && (InData[SC_HIGH][index-2] <= InData[SC_HIGH][index-4])
		 && (InData[SC_HIGH][index-1] <= InData[SC_HIGH][index-4])
		 && (InData[SC_LOW][index-3] >= InData[SC_LOW][index-4])
		 && (InData[SC_LOW][index-2] >= InData[SC_LOW][index-4])
		 && (InData[SC_LOW][index-1] >= InData[SC_LOW][index-4])

		 && IsBodyStrong(InData,index)
		 && IsWhiteCandle(InData, index)
		 && (InData[SC_LAST][index] > InData[SC_LAST][index-4])
		 )
	 {
		 ret_flag = true;
	 }
	
	 return ret_flag;
 }
 //////////////////////////////////////////////////////////////////////////////////////////////

 const double k_BearishSeparatingLines_SimularityPercent = 5.0;

 bool IsBearishSeparatingLines(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index)
 {
	 // BEARISH SEPARATING LINE
	 // 1. Downtrend
	 // 2. The candles [index] and [index-1] are of contrast colors and their open prices are equal
	 SCBaseDataRef InData = sc.BaseData;
	 bool ret_flag = false;
	 if(	settings.UseTrendDetection == false || sc.Subgraph[TREND_FOR_PATTERNS][index] == CANDLESTICK_TREND_DOWN)	// downtrend
	 {
		 // OPENs are equal
		 if(IsNearEqual(InData[SC_OPEN][index], InData[SC_OPEN][index-1], InData, index, k_BearishSeparatingLines_SimularityPercent))
		 {
			 // check for the colors contrast
			 bool colorIndex0 = IsWhiteCandle(InData, index);
			 bool colorIndex1 = IsWhiteCandle(InData, index-1);

			 ret_flag = (colorIndex1!=colorIndex0);
		 }
	 }
	 return ret_flag;

 }
 //////////////////////////////////////////////////////////////////////////////////////////////
const double k_BullishSeparatingLines_SimularityPercent = 5.0;

 bool IsBullishSeparatingLines(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index)
 {
	 // BULLISH SEPARATING LINE
	 // 1. Uptrend
	 // 2. The candles [index] and [index-1] are of contrast colors and their open prices are equal
	 SCBaseDataRef InData = sc.BaseData;
	 bool ret_flag = false;
	 if(settings.UseTrendDetection == false || sc.Subgraph[TREND_FOR_PATTERNS][index] == CANDLESTICK_TREND_UP)	// uptrend
	 {
		 // OPENs are equal
		 if(IsNearEqual(InData[SC_OPEN][index], InData[SC_OPEN][index-1], InData, index, k_BullishSeparatingLines_SimularityPercent))
		 {
			 // check for the colors contrast
			 // check for the colors contrast
			 bool colorIndex0 = IsWhiteCandle(InData, index);
			 bool colorIndex1 = IsWhiteCandle(InData, index-1);

			 ret_flag = (colorIndex1!=colorIndex0);
		 }
	 }
	 return ret_flag;
 }
 //////////////////////////////////////////////////////////////////////////////////////////////
 
 bool IsDownsideTasukiGap(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index)
 {
	 // DOWNSIDE TASUKI GAP
	 // 1. Downtrend
	 // 2. The candle [index] is white and its open price is inside the body of the candle [index-1]
	 // 3. The candles [index-1] and [index-2] are black with strong bodies
	 // 4.  There is a gap between the candles [index-1] and [index-2]
	 // 5.  CLOSE of the candle [index] is in the gap between the candles [index-1] and [index-2]
	 SCBaseDataRef InData = sc.BaseData;
	 bool ret_flag = false;
	 if(settings.UseTrendDetection == false || sc.Subgraph[TREND_FOR_PATTERNS][index] == CANDLESTICK_TREND_DOWN)	// downtrend
	 {
		 if(IsWhiteCandle(InData, index) &&	//  the candle 0 is white
			 IsBlackCandle(InData, index-1) &&	// the candle -1 is black
			 IsBlackCandle(InData, index-2))		// the candle -2 is black
		 {
			 if(IsBodyStrong(InData, index-1))		// the body of the candle -1 is strong
			 {
				 if(IsBodyStrong(InData, index-2))	// the body of the candle -2 is strong
				 {
					 if (InData[SC_LOW][index-2] > InData[SC_HIGH][index-1])	// a gap between the candles -1 and -2
					 {
						 // OPEN of the candle 0 is inside the body of the candle -1
						 if( (InData[SC_OPEN][index] > InData[SC_LAST][index-1]) &&
							 (InData[SC_OPEN][index] < InData[SC_OPEN][index-1]))	
						 {
							 // CLOSE of the candle 0 is higher than maximum of the candle -1 and lower than the minimum of the candle -2
							 if( (InData[SC_LAST][index]>InData[SC_HIGH][index-1]) &&
								 (InData[SC_LAST][index]<InData[SC_LOW][index-2]))
							 {
								 ret_flag = true;
							 }			
						 }
					 }					
				 }
			 }			
		 }
	 }
	 return ret_flag;
 }
 //////////////////////////////////////////////////////////////////////////////////////////////


 bool IsUpsideTasukiGap(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index)
 {
	 // UPSIDE TASUKI GAP
	 // 1. Uptrend
	 // 2. The candle [index] is black and its open price is inside the body of the candle [index-1]
	 // 3. The candles [index-1] and [index-2] are white with strong bodies
	 // 4. There is a gap between the candles [index-1] and [index-2]
	 // 5. CLOSE of the candle [index] is in the gap between the candles [index-1] and [index-2]
	 SCBaseDataRef InData = sc.BaseData;
	 bool ret_flag = false;
	 if(settings.UseTrendDetection == false || sc.Subgraph[TREND_FOR_PATTERNS][index] == CANDLESTICK_TREND_UP)	// uptrend
	 {
		 if( IsBlackCandle(InData, index)   &&	//  the candle 0 is black
			 IsWhiteCandle(InData, index-1) &&	// the candle -1 is white
			 IsWhiteCandle(InData, index-2))	// the candle -2 is white
		 {
			 if(IsBodyStrong(InData, index-1))		// the body of the candle -1 is strong
			 {
				 if(IsBodyStrong(InData, index-2))	// the body of the candle -2 is strong
				 {
					 if (InData[SC_HIGH][index-2] < InData[SC_LOW][index-1])	// a gap between the candles -1 and -2
					 {
						 // OPEN of the candle 0 is inside the body of the candle -1
						 if( (InData[SC_OPEN][index] > InData[SC_OPEN][index-1]) &&
							 (InData[SC_OPEN][index] < InData[SC_LAST][index-1]))	
						 {
							 // CLOSE of the candle 0 is lower than minimum of the candle -1 and higher than the maximum of the candle -2
							 if( (InData[SC_LAST][index]>InData[SC_HIGH][index-2]) &&
								 (InData[SC_LAST][index]<InData[SC_LOW][index-1]))
							 {
								 ret_flag = true;
							 }			
						 }
					 }					
				 }
			 }			
		 }
	 }
	 return ret_flag;
 }
 //////////////////////////////////////////////////////////////////////////////////////////////
 
 bool IsBearishThreeLineStrike(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index)
 {
	 // BEARISH THREE LINE STRIKE
	 // 1. Downtrend
	 // 2. The candle [index] is white with a strong body
	 // 3. The candles [index-1], [index-2] and [index-3] are black with strong bodies
	 // 4.  OPEN of the candle [index] is lower than CLOSE of the candle [index-1]
	 // 5.  CLOSE of the candle [index] is higher than OPEN of the candle [index-3]
	 SCBaseDataRef InData = sc.BaseData;
	 bool ret_flag = false;
	 if(settings.UseTrendDetection == false || sc.Subgraph[TREND_FOR_PATTERNS][index] == CANDLESTICK_TREND_DOWN)	// downtrend
	 {
		 if( IsWhiteCandle(InData, index)	  &&	// the candle 0 is white
			 IsBlackCandle(InData, index-1)   &&	// the candle -1 is black
			 IsBlackCandle(InData, index-2)   &&	// the candle -2 is black
			 IsBlackCandle(InData, index-3))		// the candle -3 is black
		 {
			 if(IsBodyStrong(InData, index-1))		// the body of the candle -1 is strong
			 {
				 if(IsBodyStrong(InData, index-2))	// the body of the candle -2 is strong
				 {
					 if(IsBodyStrong(InData, index-3))	// the body of the candle -1 is strong
					 {
						 // OPEN of the candle 0 is lower than CLOSE of the candle -1
						 if(InData[SC_OPEN][index] < InData[SC_LAST][index-1])	
						 {
							 // CLOSE of the candle 0 is higher than OPEN of the candle -3
							 if(InData[SC_LAST][index]>InData[SC_OPEN][index-3])
							 {
								 ret_flag = true;
							 }			
						 }						
					 }
				 }
			 }			
		 }
	 }
	 return ret_flag;
 }
 //////////////////////////////////////////////////////////////////////////////////////////////
 

 bool IsBullishThreeLineStrike(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index)
 {
	 // BULLISH THREE LINE STRIKE
	 // 1. Uptrend
	 // 2. The candle [index] is black with a strong body
	 // 3. The candles [index-1], [index-2] and [index-3] are white with strong bodies
	 // 4. OPEN of the candle [index] is higher than CLOSE of the candle [index-1]
	 // 5. CLOSE of the candle [index] is lower than OPEN of the candle [index-3]
	 SCBaseDataRef InData = sc.BaseData;
	 bool ret_flag = false;
	 if(settings.UseTrendDetection == false || sc.Subgraph[TREND_FOR_PATTERNS][index] == CANDLESTICK_TREND_UP)	// uptrend
	 {
		 if( IsBlackCandle(InData, index)   &&	//  the candle 0 is black
			 IsWhiteCandle(InData, index-1) &&	// the candle -1 is white
			 IsWhiteCandle(InData, index-2) &&	// the candle -2 is white
			 IsWhiteCandle(InData, index-3))	// the candle -3 is white
		 {
			 if(IsBodyStrong(InData, index-1))		// the body of the candle -1 is strong
			 {
				 if(IsBodyStrong(InData, index-2))	// the body of the candle -2 is strong
				 {
					 if(IsBodyStrong(InData, index-3))	// the body of the candle -3 is strong
					 {
						 // OPEN of the candle 0 is higher than CLOSE of the candle -1
						 if(InData[SC_OPEN][index] > InData[SC_LAST][index-1])	
						 {
							 // CLOSE of the candle 0 is lower than OPEN of the candle -3
							 if(InData[SC_LAST][index]<InData[SC_OPEN][index-3])
							 {
								 ret_flag = true;
							 }			
						 }						
					 }
				 }
			 }			
		 }
	 }
	 return ret_flag;
 }
 //////////////////////////////////////////////////////////////////////////////////////////////
 
 bool IsDownsideGapThreeMethods(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index) 
 {
	 // DOWNSIDE GAP THREE METHODS
	 // 1. Downtrend
	 // 2. The candle [index] is white and its OPEN is inside the body of the candle [index-1]
	 // 3. The candles [index-1] and [index-2] are black with a strong bodies
	 // 4.  There is a gap between the candles [index-1] and [index-2]
	 // 5.  The body of the candle [index] closes the gap between the candles [index-1] and [index-2]
	 SCBaseDataRef InData = sc.BaseData;
	 bool ret_flag = false;
	 if(settings.UseTrendDetection == false || sc.Subgraph[TREND_FOR_PATTERNS][index] == CANDLESTICK_TREND_DOWN)	// downtrend
	 {
		 if( IsWhiteCandle(InData, index)	    &&	//  the candle 0 is white
			 IsBlackCandle(InData, index-1)     &&	// the candle -1 is black
			 IsBlackCandle(InData, index-2))		// the candle -2 is black
		 {
			 if(IsBodyStrong(InData, index-1))		// the body of the candle -1 is strong
			 {
				 if(IsBodyStrong(InData, index-2))	// the body of the candle -2 is strong
				 {
					 if (InData[SC_LOW][index-2] > InData[SC_HIGH][index-1])	// a gap between the candles -1 and -2
					 {
						 // OPEN of the candle 0 is inside the body of the candle -1
						 if( (InData[SC_OPEN][index] > InData[SC_LAST][index-1]) &&
							 (InData[SC_OPEN][index] < InData[SC_OPEN][index-1]))	
						 {
							 // CLOSE of the candle 0 is higher than minimum of the candle -2
							 if(InData[SC_LAST][index]>InData[SC_LOW][index-2])
							 {
								 ret_flag = true;
							 }			
						 }
					 }					
				 }
			 }			
		 }
	 }
	 return ret_flag;
 }
 //////////////////////////////////////////////////////////////////////////////////////////////

 bool IsUpsideGapThreeMethods(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index)
 {
	 // UPSIDE GAP THREE METHODS
	 // 1. Uptrend
	 // 2.  The candle [index] is black and its OPEN is inside the body of the candle [index-1]
	 // 3. The candles [index-1] and [index-2] are white with a strong bodies
	 // 4.  There is a gap between the candles [index-1] and [index-2]
	 // 5.  The body of the candle [index] closes the gap between the candles [index-1] and [index-2]
	 SCBaseDataRef InData = sc.BaseData;
	 bool ret_flag = false;
	 if(	settings.UseTrendDetection == false || sc.Subgraph[TREND_FOR_PATTERNS][index] == CANDLESTICK_TREND_UP)	// uptrend
	 {
		 if( IsBlackCandle(InData, index)	  &&	// the candle 0 is black
			 IsWhiteCandle(InData, index-1)   &&	// the candle -1 is white
			 IsWhiteCandle(InData, index-2))		// the candle -2 is white
		 {
			 if(IsBodyStrong(InData, index-1))		//  the body of the candle -1 is strong
			 {
				 if(IsBodyStrong(InData, index-2))	//  the body of the candle -1 is strong
				 {
					 if (InData[SC_HIGH][index-2] < InData[SC_LOW][index-1])	// a gap between the candles -1 and -2
					 {
						 // OPEN of the candle 0 is inside the body of the candle -1
						 if( (InData[SC_OPEN][index] > InData[SC_OPEN][index-1]) &&
							 (InData[SC_OPEN][index] < InData[SC_LAST][index-1]))	
						 {
							 // CLOSE of the candle 0 is lower than maximum of the candle -2
							 if(InData[SC_LAST][index]<InData[SC_HIGH][index-2])
							 {
								 ret_flag = true;
							 }			
						 }
					 }					
				 }
			 }			
		 }
	 }
	 return ret_flag;
 }
 //////////////////////////////////////////////////////////////////////////////////////////////

 const double k_OnNeck_SimularityPercent = 15.0;

 bool IsOnNeck(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index)
 {
	 // ON NECK
	 // 1. Downtrend
	 // 2.  The candle [index-1] is black and with a strong body
	 // 3.  The candle [index] is white
	 // 4. CLOSE of the candle [index] is ALMOST equal the minimum of the candle [index-1]
	 SCBaseDataRef InData = sc.BaseData;
	 bool ret_flag = false;
	 if(settings.UseTrendDetection == false || sc.Subgraph[TREND_FOR_PATTERNS][index] == CANDLESTICK_TREND_DOWN)	// downtrend
	 {
		 if(IsBlackCandle(InData, index-1))		// the candle -1 is black
		 {
			 if(IsBodyStrong(InData, index-1))		// the body of the candle -1 is strong
			 {
				 if(IsWhiteCandle(InData, index))		// the candle 0 is white
				 {
					 // CLOSE of the candle 0 is ALMOST equal the minimum of the candle -1
					 if (IsNearEqual(InData[SC_LAST][index], InData[SC_LOW][index-1], InData, index, k_OnNeck_SimularityPercent))
					 {
						 ret_flag = true;					
					 }
				 }
			 }			
		 }
	 }
	 return ret_flag;
 }
 //////////////////////////////////////////////////////////////////////////////////////////////

 const double k_InNeck_SimularityPercent = 15.0;

 bool IsInNeck(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index)
 {
	 // IN NECK
	 // 1. Downtrend
	 // 2. The candle [index-1] is black and with a strong body
	 // 3. The candle [index] is white
	 // 4. OPEN of the candle [index] is lower than the minimum of the candle [index-1]
	 // 5. CLOSE of the candle [index] is a little bit higher or equal the CLOSE of the candle -1
	 SCBaseDataRef InData = sc.BaseData;
	 bool ret_flag = false;
	 if(settings.UseTrendDetection == false || sc.Subgraph[TREND_FOR_PATTERNS][index] == CANDLESTICK_TREND_DOWN)	// downtrend
	 {
		 if(IsBlackCandle(InData, index-1))		// the candle -1 is black
		 {
			 if(IsBodyStrong(InData, index-1))		// the body of the candle -1 is strong
			 {
				 if(IsWhiteCandle(InData, index))		// the candle 0 is white
				 {
					 // OPEN of the candle 0 is lower than the minimum of the candle -1, 
					 // and CLOSE of the candle 0 is a higher or equal the CLOSE of the candle -1
					 if ((InData[SC_LOW][index-1] > InData[SC_OPEN][index]) &&
						 (InData[SC_LAST][index] >= InData[SC_LAST][index-1]))
					 {
						 // CLOSE of the candle 0 is equal the CLOSE of the candle -1
						 if(IsNearEqual(InData[SC_LAST][index], InData[SC_LAST][index-1], InData, index, k_InNeck_SimularityPercent))
						 {
							 ret_flag = true;
						 }
						 // CLOSE of the candle 0 is a little bit higher or equal the CLOSE of the candle -1
						 else if((InData[SC_LAST][index]-InData[SC_LAST][index-1])<
							 PercentOfCandleLength(InData, index, k_InNeck_SimularityPercent))
						 {
							 ret_flag = true;
						 }						
					 }
				 }
			 }			
		 }
	 }
	 return ret_flag;
 }
 //////////////////////////////////////////////////////////////////////////////////////////////
 
 const double BearishThrusting_SignificantlyLowerPercent = 10.0;

 bool IsBearishThrusting(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index)
 {
	 // Bearish Thrusting
	 // 1. Downtrend
	 // 2. Candle [index-1] is black
	 // 3. Candle [index] is white
	 // 4. OPEN of the candle [index] SIGNIFICANTLY lower the minimum of the candle [index-1]
	 // 5. CLOSE of the candle [index] is higher than the CLOSE of candle [index-1], but lower than the middle of body of candle [index-1]
	 SCBaseDataRef InData = sc.BaseData;
	 bool ret_flag = false;
	 if(settings.UseTrendDetection == false || sc.Subgraph[TREND_FOR_PATTERNS][index] == CANDLESTICK_TREND_DOWN)	// downtrend
	 {
		 if(IsBlackCandle(InData, index-1))		// candle -1 is black
		 {
			 if(IsWhiteCandle(InData, index))	// candle 0 is white
			 {
				 // OPEN of candle 0 is SIGNIFICANTLY lower than the minimum of candle -1
				 if ((InData[SC_LOW][index-1] > InData[SC_OPEN][index]) &&
					 (InData[SC_LOW][index-1]-InData[SC_OPEN][index])>
					 PercentOfCandleLength(InData, index, BearishThrusting_SignificantlyLowerPercent))
				 {
					 // CLOSE of candle 0 is higher than the CLOSE of -1, but lower than the middle of body of the candle -1
					 if( (InData[SC_LAST][index] > InData[SC_LAST][index-1]) &&
						 (InData[SC_LAST][index] < (InData[SC_LAST][index-1]+(InData[SC_OPEN][index-1]-InData[SC_LAST][index-1])/2)))
					 {
						 ret_flag = true;
					 }							
				 }
			 }

		 }
	 }
	 return ret_flag;
 }
 //////////////////////////////////////////////////////////////////////////////////////////////
 
 const int    k_MatHold_PAST_INDEX = 3;

 bool IsMatHold(SCStudyInterfaceRef sc, const s_CandleStickPatternsFinderSettings& settings, int index)
 {
	 // MAT HOLD
	 // 1. Candle [index] is white with strong body
	 // 2. OPEN of th candle [index] is higher than CLOSE of candle [index-1]
	 // 3. Candles [index-1] and [index-2] has weak bodies
	 // 4. CLOSE of candle [index-2] is higher than the CLOSE of candle [index-1]
	 // 5. Candle [index-X] has strong white body (where X is from 3 to PAST_INDEX+1)
	 // 6. CLOSE of candle [index] is higher than CLOSE of candle [index-X]
	 // 7. Uptrend till candle [index-X]
	 // 8. Candles [index-Y] has weak bodies (where Y is from 1 to X-1)
	 // 9. CLOSE of candle [index-Y] is higher than CLOSE of [index-Y-1] (where Y is from 1 to X-1)
	 // 10. Minimum of candle [index-Y+1] is higher than maximum of candle [index-Y]
	 SCBaseDataRef InData = sc.BaseData;
	 bool ret_flag = false;
	 if( IsWhiteCandle(InData, index) &&		// candle 0 is white
		 (InData[SC_OPEN][index] > InData[SC_LAST][index-1]))		// OPEN of candle 0 is higher than CLOSE of -1
	 {
		 if (IsBodyStrong(InData,index))		// body of the candle 0 is strong
		 {
			 if (InData[SC_LAST][index-2] > InData[SC_LAST][index-1])		// CLOSE of candle -2 is higher than CLOSE of candle -1
			 {
				 // bodies of candles -1 and -2 are weak
				 if ((!IsBodyStrong(InData,index-1)) && (!IsBodyStrong(InData,index-2)))
				 {
					 int X = 0;
					 // searching for a strong white body
					 for (int i =3;i<(k_MatHold_PAST_INDEX+2);i++)
					 {						
						 if (IsBodyStrong(InData,index-i) && IsWhiteCandle(InData, index-i))
						 {
							 // CLOSE of candle 0 is higher than CLOSE of candle -X
							 if(InData[SC_LAST][index] > InData[SC_LAST][index-i])
							 {			
								 // found candle -X	
								 X = i; 								
							 }
							 break;	
						 }
					 }
					 if( X!=0)
					 {
						 // uptrend till candle -Õ
						 if (settings.UseTrendDetection == false || sc.Subgraph[TREND_FOR_PATTERNS][index] == CANDLESTICK_TREND_UP)
						 {
							 // Minimum of candle [index-Y+1] is higher than maximum of candle [index-Y]; candle [index-Y+1] is black
							 if((InData[SC_LOW][index-X+1] > InData[SC_HIGH][index-X]) &&
								 IsBlackCandle(InData, index-X+1))
							 {
								 ret_flag = true;
								 // checking the candles between two white candles
								 if ((IsBodyStrong(InData,index-1)) ||						  // check if body is strong
									 (InData[SC_HIGH][index-1] > InData[SC_HIGH][index-X]) ||  // check if maximum is higher than maximum of candle -Õ
									 (InData[SC_LOW][index-1] < InData[SC_LOW][index-X]))	  // check if minimum is lower than minimum of candle -Õ
								 {
									 ret_flag = false;
								 }
								 else
								 {
									 for (int i = 2;i<X;i++)
									 {									
										 // searching for a presence of a candle that does not match conditions
										 if ((IsBodyStrong(InData,index-i)) ||						  // check whether body is strong
											 (InData[SC_LAST][index-i] < InData[SC_LAST][index-i+1])) //  check whether CLOSE is lower than the CLOSE of a next candle	
										 {
											 ret_flag = false;	
											 break;
										 }									
									 }	
								 }
							 }
						 }						
					 }				
				 }				
			 }			
		 }
	 }
	 return ret_flag;
 }



/*==========================================================================*/
SCSFExport scsf_WoodiesZLR(SCStudyInterfaceRef sc)
{
	SCSubgraphRef LongTrade = sc.Subgraph[0];
	SCSubgraphRef ShortTrade = sc.Subgraph[1];
	SCSubgraphRef ChopzoneAverage = sc.Subgraph[2];
	
	SCInputRef CCITrendRef = sc.Input[0];
	SCInputRef SideWinderRef = sc.Input[1];
	SCInputRef ChopzoneRef = sc.Input[2];
	SCInputRef LowestCCILevelforShort = sc.Input[3];
	SCInputRef HighestCCIDifferenceforShort = sc.Input[4];
	SCInputRef LowestCCIDifferenceforLong = sc.Input[5];
	SCInputRef HighestCCILevelforLong = sc.Input[6];
	SCInputRef ChopzoneLength = sc.Input[7];
	SCInputRef ChopzoneThreshhold = sc.Input[8];
	SCInputRef AlertNumber = sc.Input[9];

	if (sc.SetDefaults)
	{
		// Name and description
		sc.GraphName = "Woodies ZLR System";
		sc.StudyDescription = "Woodies ZLR for using in \"Woodies for Range Charts-New\" Study Collction.";
		
		// Settings
		sc.AutoLoop		= 0;
		sc.GraphRegion	= 1;
		sc.CalculationPrecedence = LOW_PREC_LEVEL;

		// Subgraphs
        LongTrade.Name = "Long Trade";
        LongTrade.DrawStyle = DRAWSTYLE_ARROWUP;
        LongTrade.PrimaryColor = RGB(0,255,0);
		LongTrade.LineWidth = 2;
		LongTrade.DrawZeros = false;

		ShortTrade.Name = "Short Trade";
        ShortTrade.DrawStyle = DRAWSTYLE_ARROWDOWN;
        ShortTrade.PrimaryColor = RGB(255,0,0);
		ShortTrade.LineWidth = 2;
		ShortTrade.DrawZeros = false;

		ChopzoneAverage.Name = "Chopzone Average";
		ChopzoneAverage.DrawStyle = DRAWSTYLE_IGNORE;
		ChopzoneAverage.PrimaryColor = RGB(255,255,0);
		ChopzoneAverage.DrawZeros = false;

		// Inputs
		CCITrendRef.Name = "Woodies CCI Trend Study Reference";
		CCITrendRef.SetStudyID(1);

		SideWinderRef.Name = "Sidewinder Study Reference";
		SideWinderRef.SetStudyID(1);

		ChopzoneRef.Name = "Chopzone Study Reference";
		ChopzoneRef.SetStudyID(1);

		LowestCCILevelforShort.Name = "Lowest CCI Level for Short";
		LowestCCILevelforShort.SetFloat(-120);

		HighestCCIDifferenceforShort.Name = "Highest CCI Difference for Short";
		HighestCCIDifferenceforShort.SetFloat(-15);

		LowestCCIDifferenceforLong.Name = "Lowest CCI Difference for Long";
		LowestCCIDifferenceforLong.SetFloat(15);

		HighestCCILevelforLong.Name = "Highest CCI Level for Long";
		HighestCCILevelforLong.SetFloat(120);

		ChopzoneThreshhold.Name = "Chopzone Threshold";
		ChopzoneThreshhold.SetFloat(4.5);

		ChopzoneLength.Name = "Chopzone Length";
		ChopzoneLength.SetInt(7);

		AlertNumber.Name = "Alert Number";
		AlertNumber.SetAlertSoundNumber(0);

		return;
	}

	

	// see CCI Trend New study to find correct color values
	enum CCITrendColors
	{
		CCI_TREND_UP_COLOR = 3,
		CCI_TREND_DOWN_COLOR = 1
	};

	// see Sidewinder study to find correct color values
	enum SidewinderColors
	{
		SW_TREND_FLAT = 0	
	};

	const int UniqueID = 1546833579;
	const int LastBarUniqueID = UniqueID+1;


	SCFloatArray CCITrendColor;
	sc.GetStudyArrayUsingID(CCITrendRef.GetStudyID(), 8, CCITrendColor);

	SCFloatArray CCI;
	sc.GetStudyArrayUsingID(CCITrendRef.GetStudyID(), 0, CCI);

	SCFloatArray SidewinderColors;
	sc.GetStudyArrayUsingID(SideWinderRef.GetStudyID(), 3, SidewinderColors);

	SCFloatArray ChopzoneOutput;
	sc.GetStudyArrayUsingID(ChopzoneRef.GetStudyID(), 2, ChopzoneOutput);

	SCFloatArray HooksConditions;
	sc.GetStudyArrayUsingID(CCITrendRef.GetStudyID(), 9, HooksConditions);


	bool ZLRLongTrade = false;
	bool ZLRShortTrade = false;

	for (int index = sc.UpdateStartIndex; index < sc.ArraySize; index++)
	{
		if(index == 0)
			continue;

		float CCIDiff = CCI[index] - CCI[index-1]; 

		ZLRLongTrade = false;
		ZLRShortTrade = false;
		LongTrade[index] = 0;
		ShortTrade[index] = 0;

		if(sc.GetBarHasClosedStatus(index)==BHCS_BAR_HAS_NOT_CLOSED)
		{
			sc.DeleteLineOrText(sc.ChartNumber, TOOL_DELETE_CHARTDRAWING, LastBarUniqueID);
		}

		// Clear the alert state
		sc.SetAlert(0);

		// check ZLR LONG TRADE CONDITION
		if(CCITrendColor[index] == CCI_TREND_UP_COLOR) // trend is UP
		{
			if(CCIDiff < LowestCCIDifferenceforLong.GetFloat())
				continue;

			if(CCI[index] > HighestCCILevelforLong.GetFloat())
				continue;

			if(CCI[index] < 0)
				continue;

			if(SidewinderColors[index] == SW_TREND_FLAT)
				continue;

			if(HooksConditions[index] <= 0)
				continue;

			// check Chopzone
			SCFloatArrayRef ChopzoneAVG = ChopzoneAverage;
			sc.WeightedMovingAverage(ChopzoneOutput,ChopzoneAVG,index,ChopzoneLength.GetInt());

			float ChopzoneAVGval = ChopzoneAVG[index];

			if (ChopzoneAVG[index] >= ChopzoneThreshhold.GetFloat())
			{
				ZLRLongTrade = true;
			}

			
			if(!ZLRLongTrade)
				continue;
		}
		else if(CCITrendColor[index] == CCI_TREND_DOWN_COLOR) // trend is DOWN
		{
			if(CCIDiff > HighestCCIDifferenceforShort.GetFloat())
				continue;

			if(CCI[index] < LowestCCILevelforShort.GetFloat())
				continue;

			if(CCI[index] >= 0)
				continue;

			if(SidewinderColors[index] == SW_TREND_FLAT)
				continue;

			if(HooksConditions[index] >= 0)
				continue;

			// check Chopzone
			SCFloatArrayRef ChopzoneAVG = ChopzoneAverage;
			sc.WeightedMovingAverage(ChopzoneOutput,ChopzoneAVG,index,ChopzoneLength.GetInt());

			if (ChopzoneAVG[index] <= -1 * ChopzoneThreshhold.GetFloat())
			{
				ZLRShortTrade = true;
			}

			if(!ZLRShortTrade)
				continue;
		}
		else
			continue;

		// put values to arrays

		s_UseTool Tool;
		Tool.Clear(); // reset tool structure for our next use
		Tool.ChartNumber	= sc.ChartNumber;
		Tool.DrawingType	= DRAWING_TEXT;
		Tool.Region			= sc.GraphRegion;
		Tool.DrawingType	= DRAWING_TEXT;
		Tool.ReverseTextColor = 0;
		Tool.Text			= "ZLR";
		Tool.BeginDateTime	= sc.BaseDateTimeIn[index];
		Tool.LineNumber		= UniqueID;
		Tool.FontSize		= 8;
		Tool.AddMethod		= UTAM_ADD_ALWAYS;

		if(sc.GetBarHasClosedStatus(index)==BHCS_BAR_HAS_NOT_CLOSED)
		{
			Tool.LineNumber = LastBarUniqueID;
			
		}

		if(ZLRLongTrade)
		{
			LongTrade[index] = -200;

			Tool.BeginValue		= -200;
			Tool.Color			= LongTrade.PrimaryColor;
			Tool.TextAlignment	= DT_CENTER | DT_BOTTOM;

			sc.UseTool(Tool);

			sc.SetAlert(AlertNumber.GetAlertSoundNumber());
		}

		if(ZLRShortTrade)
		{
			ShortTrade[index] = 200;

			Tool.BeginValue		= 200;
			Tool.Color			= ShortTrade.PrimaryColor;
			Tool.TextAlignment	= DT_CENTER | DT_TOP;

			sc.UseTool(Tool);

			sc.SetAlert(AlertNumber.GetAlertSoundNumber());
		}
	}
}


/************************************************************************/
SCSFExport scsf_ChopZone(SCStudyInterfaceRef sc)
{

	SCSubgraphRef Czi1 = sc.Subgraph[0];
	SCSubgraphRef Czi2 = sc.Subgraph[1];
	SCSubgraphRef OutWorksheets = sc.Subgraph[2];
	SCSubgraphRef Ema = sc.Subgraph[3];
	SCSubgraphRef ColorControl3 = sc.Subgraph[4];
	SCSubgraphRef ColorControl4 = sc.Subgraph[5];

	SCInputRef InputData = sc.Input[0];
	SCInputRef PosCzi1 = sc.Input[2];
	SCInputRef PosCzi2 = sc.Input[3];
	SCInputRef EmaLength = sc.Input[5];	
	SCInputRef TickValue = sc.Input[6];

	if(sc.SetDefaults)
	{
		sc.GraphName = "Chop Zone";
		sc.StudyDescription = "Chop Zone";

		sc.AutoLoop = 1;

		Czi1.Name = "CZI Top";
		Czi1.DrawStyle = DRAWSTYLE_LINE;
		Czi1.LineWidth = 2;
		Czi1.PrimaryColor = RGB(255,255,0);
		Czi1.SecondaryColor = RGB(170,255,85);
		Czi1.SecondaryColorUsed = true;
		Czi1.DrawZeros = false;

		Czi2.Name = "CZI Bottom";
		Czi2.DrawStyle = DRAWSTYLE_LINE;
		Czi2.LineWidth = 2;
		Czi2.PrimaryColor = RGB(85,255,170);
		Czi2.SecondaryColor = RGB(0,255,255);
		Czi2.SecondaryColorUsed = true;
		Czi2.DrawZeros = false;

		OutWorksheets.Name = "Output for Spreadsheets";
		OutWorksheets.DrawStyle = DRAWSTYLE_IGNORE;
		OutWorksheets.PrimaryColor = RGB(255,255,255);
		OutWorksheets.DrawZeros = false;

		ColorControl3.Name = "Down Colors 1 & 2";
		ColorControl3.DrawStyle = DRAWSTYLE_IGNORE;
		ColorControl3.PrimaryColor = RGB(213,170,0);
		ColorControl3.SecondaryColor = RGB(170,85,0);
		ColorControl3.SecondaryColorUsed = true;
		ColorControl3.DrawZeros = false;

		ColorControl4.Name = "Down Color 3";
		ColorControl4.DrawStyle = DRAWSTYLE_IGNORE;
		ColorControl4.PrimaryColor = RGB(128,0,0);
		ColorControl4.DrawZeros = false;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		PosCzi1.Name = "Y Pos CZI 1";
		PosCzi1.SetFloat(100.0f);

		PosCzi2.Name = "Y Pos CZI 2";
		PosCzi2.SetFloat(-100.0f);

		EmaLength.Name = "EMA Length";
		EmaLength.SetInt(34);
		EmaLength.SetIntLimits(1,MAX_STUDY_LENGTH);

		TickValue.Name = "Tick Value (Enter number to override default)";
		TickValue.SetFloat(0.0f);

		return;
	}


	if (Czi1.PrimaryColor == RGB(255,255,0)
		&& Czi1.SecondaryColor == RGB(0,255,255))
	{
		Czi1.PrimaryColor = RGB(255,255,0);
		Czi1.SecondaryColor = RGB(170,255,85);
		Czi2.PrimaryColor = RGB(85,255,170);
		Czi2.SecondaryColor = RGB(0,255,255);
		ColorControl3.PrimaryColor = RGB(213,170,0);
		ColorControl3.SecondaryColor = RGB(170,85,0);
		ColorControl4.PrimaryColor = RGB(128,0,0);
	}

	float inTickSize = sc.TickSize;

	if(TickValue.GetFloat() != 0)
		inTickSize = TickValue.GetFloat();

	float &PriorTickSize = sc.PersistVars->f1;

	if(PriorTickSize != inTickSize)
	{
		sc.UpdateStartIndex = 0;
		PriorTickSize = inTickSize;
	}

	// Colors	

	COLORREF ColorLevel = Czi1.PrimaryColor;
	COLORREF ColorUp1 = Czi1.SecondaryColor;
	COLORREF ColorUp2 = Czi2.PrimaryColor;
	COLORREF ColorUp3 = Czi2.SecondaryColor;
	COLORREF ColorDown1 = ColorControl3.PrimaryColor;
	COLORREF ColorDown2 = ColorControl3.SecondaryColor;
	COLORREF ColorDown3 = ColorControl4.PrimaryColor;


	sc.DataStartIndex = EmaLength.GetInt();

	int pos = sc.Index;

	sc.ExponentialMovAvg(sc.BaseData[InputData.GetInputDataIndex()], Ema, pos, EmaLength.GetInt());

	int TickDifference = sc.Round((sc.BaseData[InputData.GetInputDataIndex()][pos] - Ema[pos])/inTickSize);

	Czi1[pos] = PosCzi1.GetFloat();
	Czi2[pos] = PosCzi2.GetFloat();

	/*
	cyan   Woodies CZI >= 5
	dark green Woodies CZI >= 4 And Woodies CZI < 5  
	light green   Woodies CZI >= 3 And Woodies CZI < 4
	lime		  Woodies CZI > 1 And Woodies CZI < 3

	yellow		  -1 <= (Woodies CZI) <= 1

	light orange  Woodies CZI > -3 And Woodies CZI < -1
	dark orange   Woodies CZI > -4 And Woodies CZI <= -3
	light red	  Woodies CZI > -5 And Woodies CZI <= -4
	brown         <=-5 
	*/

	if (TickDifference <= 1 && TickDifference >= -1)
		Czi1.DataColor[pos] = Czi2.DataColor[pos] = ColorLevel;  
	else if (TickDifference > 1 && TickDifference < 3) 
		Czi1.DataColor[pos] = Czi2.DataColor[pos] = ColorUp1;  
	else if (TickDifference >= 3 && TickDifference < 4) 
		Czi1.DataColor[pos] = Czi2.DataColor[pos] = ColorUp2; 
	else if (TickDifference >= 4)
		Czi1.DataColor[pos] = Czi2.DataColor[pos] = ColorUp3;


	else if (TickDifference < -1 && TickDifference > -3)
		Czi1.DataColor[pos] = Czi2.DataColor[pos] = ColorDown1; 
	else if (TickDifference <= -3 && TickDifference > -4)
		Czi1.DataColor[pos] = Czi2.DataColor[pos] = ColorDown2; 
	else if (TickDifference <= -4)
		Czi1.DataColor[pos] = Czi2.DataColor[pos] = ColorDown3; 

	// for worksheets
	OutWorksheets[pos] = (float)TickDifference;
	
}


/***********************************************************************/
SCSFExport scsf_StochBands(SCStudyInterfaceRef sc)
{
	SCSubgraphRef SubgraphK = sc.Subgraph[0];
	SCSubgraphRef SubgraphD = sc.Subgraph[1];
	SCSubgraphRef SubgraphK2 = sc.Subgraph[2];
	SCSubgraphRef SubgraphD2 = sc.Subgraph[3];

	SCSubgraphRef SubgraphBackgroundCalcForStoch1 = sc.Subgraph[8];
	SCSubgraphRef SubgraphBackgroundCalcForStoch2 = sc.Subgraph[9];

	SCInputRef InputDataType = sc.Input[0];
	SCInputRef InputMAType = sc.Input[1];
	SCInputRef InputLength = sc.Input[2];
	SCInputRef InputK = sc.Input[3];
	SCInputRef InputD = sc.Input[4];
	SCInputRef InputLength2 = sc.Input[5];
    SCInputRef InputK2 = sc.Input[6];
    SCInputRef InputD2 = sc.Input[7];

    if (sc.SetDefaults)
    {
        sc.GraphName = "Stochastic Bands";
		sc.StudyDescription = "Stochastic Bands";
        
		sc.AutoLoop = 1;
        sc.FreeDLL = 0; 
		
		SubgraphK.Name = "K";
		SubgraphK.DrawStyle = DRAWSTYLE_LINE;
		SubgraphK.LineWidth = 2;
		SubgraphK.PrimaryColor = RGB(0, 200, 0);
		SubgraphK.SecondaryColor = RGB(0, 200, 0);
		SubgraphK.SecondaryColorUsed = 1;
		SubgraphK.DrawZeros = true;

		SubgraphD.Name = "D";
		SubgraphD.DrawStyle = DRAWSTYLE_LINE;
		SubgraphD.LineWidth = 2;
		SubgraphD.PrimaryColor = RGB(0, 200, 0);
		SubgraphD.SecondaryColor = RGB(0, 200, 0);
		SubgraphD.SecondaryColorUsed = 1;
		SubgraphD.DrawZeros = true;

		SubgraphK2.Name = "K2";
		SubgraphK2.DrawStyle = DRAWSTYLE_LINE;
		SubgraphK2.LineWidth = 2;
		SubgraphK2.PrimaryColor = RGB(0, 200, 0);
		SubgraphK2.SecondaryColor = RGB(0, 200, 0);
		SubgraphK2.SecondaryColorUsed = 1;
		SubgraphK2.DrawZeros = true;

		SubgraphD2.Name = "D2";
		SubgraphD2.DrawStyle = DRAWSTYLE_LINE;
		SubgraphD2.LineWidth = 2;
		SubgraphD2.PrimaryColor = RGB(0, 200, 0);
		SubgraphD2.SecondaryColor = RGB(0, 200, 0);
		SubgraphD2.SecondaryColorUsed = 1;
		SubgraphD2.DrawZeros = true;

		InputDataType.Name = "Input Data";
		InputDataType.SetInputDataIndex(SC_HL);

		InputMAType.Name = "Moving Average Type";
		InputMAType.SetMovAvgType(MOVAVGTYPE_SIMPLE);		

		InputLength.Name = "Length";
		InputLength.SetInt(7);

		InputK.Name = "K";
		InputK.SetInt(3);

		InputD.Name = "D";
		InputD.SetInt(3);

		InputLength2.Name = "Length2";
		InputLength2.SetInt(21);

		InputK2.Name = "K2";
		InputK2.SetInt(10);

		InputD2.Name = "D2";
		InputD2.SetInt(4);

		sc.DataStartIndex = 50;

        return;
    }


	if (sc.Index < 1)
		return;

	int Length = InputLength.GetInt();
	int K = InputK.GetInt();
	int D = InputD.GetInt();

	int Length2 = InputLength2.GetInt();
	int K2 = InputK2.GetInt();
	int D2 = InputD2.GetInt();

	int DataIndex = InputDataType.GetInputDataIndex();
	int MAType = InputMAType.GetMovAvgType();

	// First Stochastic
	sc.Stochastic(sc.BaseDataIn[DataIndex], sc.BaseDataIn[DataIndex], sc.BaseDataIn[DataIndex], 
					SubgraphBackgroundCalcForStoch1, sc.Index, 
					Length, K, D, MAType);

	float sk = SubgraphBackgroundCalcForStoch1.Arrays[0][sc.Index];
	float sd = SubgraphBackgroundCalcForStoch1.Arrays[1][sc.Index];

	// Second Stochastic
	sc.Stochastic(sc.BaseDataIn[DataIndex], sc.BaseDataIn[DataIndex], sc.BaseDataIn[DataIndex], 
					SubgraphBackgroundCalcForStoch2, sc.Index, 
					Length2, K2, D2, MAType);
	
	float sk2 = SubgraphBackgroundCalcForStoch2.Arrays[0][sc.Index];
	float sd2 = SubgraphBackgroundCalcForStoch2.Arrays[1][sc.Index];


	SubgraphK[sc.Index] = sk;
	SubgraphD[sc.Index] = sd;

	if (sk > sd)
	{
		SubgraphK.DataColor[sc.Index] = SubgraphK.PrimaryColor;
		SubgraphD.DataColor[sc.Index] = SubgraphD.PrimaryColor;
	}
	else
	{
		SubgraphK.DataColor[sc.Index] = SubgraphK.SecondaryColor;
		SubgraphD.DataColor[sc.Index] = SubgraphD.SecondaryColor;
	}

	SubgraphK2[sc.Index] = sk2;
	SubgraphD2[sc.Index] = sd2;

	if (sk2 > sd2)
	{
		SubgraphK2.DataColor[sc.Index] = SubgraphK2.PrimaryColor;
		SubgraphD2.DataColor[sc.Index] = SubgraphD2.PrimaryColor;
	}
	else 
	{
		SubgraphK2.DataColor[sc.Index] = SubgraphK2.SecondaryColor;
		SubgraphD2.DataColor[sc.Index] = SubgraphD2.SecondaryColor;
	}
}

/***********************************************************************/
SCSFExport scsf_BLine(SCStudyInterfaceRef sc)
{
	SCSubgraphRef SubgraphBaseline = sc.Subgraph[0];
	SCSubgraphRef SubgraphOversold = sc.Subgraph[1];
	SCSubgraphRef SubgraphOverbought = sc.Subgraph[2];

	SCSubgraphRef SubgraphBackgroundCalcForStoch = sc.Subgraph[5];

	SCInputRef InputMATypeSimple = sc.Input[0];
	SCInputRef InputLength = sc.Input[1];
	SCInputRef InputK = sc.Input[2];
	SCInputRef InputD = sc.Input[3];
    SCInputRef InputUseHL = sc.Input[4];

    if (sc.SetDefaults)
    {
        sc.GraphName = "Buffys B-Line";
		sc.StudyDescription = "Buffys B-Line";
        
		sc.AutoLoop = 1;
        sc.FreeDLL = 0; 
		
		SubgraphBaseline.Name = "Baseline";
		SubgraphBaseline.DrawStyle=DRAWSTYLE_LINE;
		SubgraphBaseline.PrimaryColor=RGB(255,255,255);
		
		SubgraphOversold.Name = "Oversold";
		SubgraphOversold.DrawStyle=DRAWSTYLE_POINT;
		SubgraphOversold.PrimaryColor=RGB(0,255,0);

		SubgraphOverbought.Name = "Overbought";
		SubgraphOverbought.DrawStyle=DRAWSTYLE_POINT;
		SubgraphOverbought.PrimaryColor=RGB(255,0,0);


		InputMATypeSimple.Name = "MA Type: Use Simple instead of Ensign Exponential";
		InputMATypeSimple.SetYesNo(false);

		InputLength.Name = "Length";
		InputLength.SetInt(21);

		InputK.Name = "K";
		InputK.SetInt(10);

		InputD.Name = "D";
		InputD.SetInt(4);

		InputUseHL.Name = "Use HL Average Instead of Close";
		InputUseHL.SetYesNo(false);

		sc.DataStartIndex = 50;

        return;
    }

	int Length = InputLength.GetInt();
	int K = InputK.GetInt();
	int D = InputD.GetInt();

	int DataIndex = SC_LAST;
	if (InputUseHL.GetBoolean())
		DataIndex = SC_HL;


	sc.Stochastic(sc.BaseDataIn[DataIndex], sc.BaseDataIn[DataIndex], sc.BaseDataIn[DataIndex],SubgraphBackgroundCalcForStoch, sc.Index, 
					Length, K, D, MOVAVGTYPE_SIMPLE);

	float FastK = SubgraphBackgroundCalcForStoch[sc.Index];
	
	int skArrayIndex, sdArrayIndex;
	if (InputMATypeSimple.GetBoolean())
	{
		skArrayIndex = 0;
		sdArrayIndex = 1;
	}
	else
	{
		skArrayIndex = 2;
		sdArrayIndex = 3;
	}

	float PriorStochasticK, PriorStochasticD;
	if (sc.Index)
	{
		PriorStochasticK = SubgraphBackgroundCalcForStoch.Arrays[skArrayIndex][sc.Index-1];
		PriorStochasticD = SubgraphBackgroundCalcForStoch.Arrays[sdArrayIndex][sc.Index-1];
	}
	else
		PriorStochasticK = PriorStochasticD = sc.BaseDataIn[DataIndex][0];

	if (InputMATypeSimple.GetBoolean() == false)
	{
		// Calculate Ensign Exponential Moving Average
		float temp = SubgraphBackgroundCalcForStoch.Arrays[2][sc.Index] = (FastK-PriorStochasticK)/K + PriorStochasticK;
		SubgraphBackgroundCalcForStoch.Arrays[3][sc.Index] = (temp-PriorStochasticD)/D + PriorStochasticD;
	}

	float sk = SubgraphBackgroundCalcForStoch.Arrays[skArrayIndex][sc.Index];
	float sd = SubgraphBackgroundCalcForStoch.Arrays[sdArrayIndex][sc.Index];

	SubgraphBaseline[sc.Index] = sk;
	if(sk>=80) 
		SubgraphOversold[sc.Index] = sk;
	else if (sk <= 20)
		SubgraphOverbought[sc.Index] = sk;
}

/***********************************************************************/
SCSFExport scsf_BuffyMALines(SCStudyInterfaceRef sc)
{
	SCSubgraphRef SubgraphSMA1 = sc.Subgraph[0];
	SCSubgraphRef SubgraphSMA2 = sc.Subgraph[1];

	SCSubgraphRef SubgraphWMA1 = sc.Subgraph[2];
	SCSubgraphRef SubgraphWMA2 = sc.Subgraph[3];
	
	SCSubgraphRef SubgraphBackgroundCalcForWMA1 = sc.Subgraph[6];
	SCSubgraphRef SubgraphBackgroundCalcForWMA2 = sc.Subgraph[7];

	SCSubgraphRef SubgraphBackgroundCalcForSMA1 = sc.Subgraph[8];
	SCSubgraphRef SubgraphBackgroundCalcForSMA2 = sc.Subgraph[9];

	SCInputRef SMA1DataInput = sc.Input[0];
	SCInputRef SMA1Length = sc.Input[1];
	SCInputRef SMA2DataInput = sc.Input[2];
	SCInputRef SMA2Length = sc.Input[3];

	SCInputRef WMA1DataInput = sc.Input[4];
	SCInputRef WMA1Length = sc.Input[5];
	SCInputRef WMA2DataInput = sc.Input[6];
	SCInputRef WMA2Length = sc.Input[7];

    if (sc.SetDefaults)
    {
        sc.GraphName = "Buffy's MA Lines";
		sc.StudyDescription = "Buffy's MA Lines";
        
		sc.AutoLoop = 1;
        sc.FreeDLL = 0; 
		
		SubgraphSMA1.Name = "SMA1Dn";
		SubgraphSMA1.DrawStyle = DRAWSTYLE_STAIR;
		SubgraphSMA1.LineWidth = 5;
		SubgraphSMA1.PrimaryColor = RGB(0, 0, 255);
		SubgraphSMA1.DrawZeros = false;

		SubgraphSMA2.Name = "SMA2Up";
		SubgraphSMA2.DrawStyle = DRAWSTYLE_STAIR;
		SubgraphSMA2.LineWidth = 5;
		SubgraphSMA2.PrimaryColor = RGB(255, 255, 0);
		SubgraphSMA2.DrawZeros = false;

		SubgraphWMA1.Name = "WMA1Up";
		SubgraphWMA1.DrawStyle = DRAWSTYLE_STAIR;
		SubgraphWMA1.LineWidth = 2;
		SubgraphWMA1.PrimaryColor = RGB(0, 0, 0);
		SubgraphWMA1.DrawZeros = false;

		SubgraphWMA2.Name = "WMA2Dn";
		SubgraphWMA2.DrawStyle = DRAWSTYLE_STAIR;
		SubgraphWMA2.LineWidth = 2;
		SubgraphWMA2.PrimaryColor = RGB(0, 0, 0);
		SubgraphWMA2.DrawZeros = false;

		SMA1DataInput.Name = "SMA1 Input Data";
		SMA1DataInput.SetInputDataIndex(SC_HIGH);

		SMA1Length.Name = "SMA1 Length";
		SMA1Length.SetInt(12);
		SMA1Length.SetIntLimits(1, MAX_STUDY_LENGTH);

		SMA2DataInput.Name = "SMA2 Input Data";
		SMA2DataInput.SetInputDataIndex(SC_LOW);

		SMA2Length.Name = "SMA2 Length";
		SMA2Length.SetInt(12);
		SMA2Length.SetIntLimits(1, MAX_STUDY_LENGTH);

		WMA1DataInput.Name = "WMA1 Input Data";
		WMA1DataInput.SetInputDataIndex(SC_HIGH);

		WMA1Length.Name = "WMA1 (Rounded To Tick): Length";
		WMA1Length.SetInt(6);
		WMA1Length.SetIntLimits(1, MAX_STUDY_LENGTH);

		WMA2DataInput.Name = "WMA2 Input Data";
		WMA2DataInput.SetInputDataIndex(SC_LOW);

		WMA2Length.Name = "WMA2 (Rounded To Tick): Length";
		WMA2Length.SetInt(6);
		WMA2Length.SetIntLimits(1, MAX_STUDY_LENGTH);

		sc.DataStartIndex = 10;

        return;
    }


	sc.SimpleMovAvg(sc.BaseDataIn[SMA1DataInput.GetInputDataIndex()], SubgraphBackgroundCalcForSMA1, sc.Index, SMA1Length.GetInt());
	sc.SimpleMovAvg(sc.BaseDataIn[SMA2DataInput.GetInputDataIndex()], SubgraphBackgroundCalcForSMA2, sc.Index, SMA2Length.GetInt());

	sc.WeightedMovingAverage(sc.BaseDataIn[WMA1DataInput.GetInputDataIndex()], SubgraphBackgroundCalcForWMA1, sc.Index, WMA1Length.GetInt());
	sc.WeightedMovingAverage(sc.BaseDataIn[WMA2DataInput.GetInputDataIndex()], SubgraphBackgroundCalcForWMA2, sc.Index, WMA2Length.GetInt());

	if (sc.Index < 10)
		return; // nothing else to do

	// If Falling, Show High SMA
	if (SubgraphBackgroundCalcForSMA1[sc.Index-1] >= SubgraphBackgroundCalcForSMA1[sc.Index])
		SubgraphSMA1[sc.Index] = SubgraphBackgroundCalcForSMA1[sc.Index];

	// If Rising, Show Low SMA
	if (SubgraphBackgroundCalcForSMA2[sc.Index-1] <= SubgraphBackgroundCalcForSMA2[sc.Index])
		SubgraphSMA2[sc.Index] = SubgraphBackgroundCalcForSMA2[sc.Index];

	// If Falling, Show High WMA
	if (SubgraphBackgroundCalcForWMA1[sc.Index-1] >= SubgraphBackgroundCalcForWMA1[sc.Index])
		SubgraphWMA1[sc.Index] = (float)sc.RoundToTickSize(SubgraphBackgroundCalcForWMA1[sc.Index], sc.TickSize);

	// If Rising, Show Low WMA
	if (SubgraphBackgroundCalcForWMA2[sc.Index-1] <= SubgraphBackgroundCalcForWMA2[sc.Index])
		SubgraphWMA2[sc.Index] = (float)sc.RoundToTickSize(SubgraphBackgroundCalcForWMA2[sc.Index], sc.TickSize);
}

/***************************************************************/
SCSFExport scsf_AutomaticTrendlines(SCStudyInterfaceRef sc)
{
	SCSubgraphRef UpTrendline = sc.Subgraph[0];
	SCSubgraphRef DownTrendline = sc.Subgraph[1];

	SCInputRef LengthInput = sc.Input[0];
	SCInputRef SkipLastBars = sc.Input[1];
	SCInputRef ShowTrendlineUp = sc.Input[3];
	SCInputRef ShowTrendlineDown = sc.Input[4];
	SCInputRef NewBarsBeforeAdjust = sc.Input[5];
	SCInputRef ForwardProjectAutoTrendLines = sc.Input[6];
	SCInputRef VersionUpdate = sc.Input[7];
	SCInputRef InputDataHigh = sc.Input[8];
	SCInputRef InputDataLow = sc.Input[9];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Automatic Trendlines";

		sc.GraphRegion	= 0;
		sc.AutoLoop		= 0;
		sc.ValueFormat = VALUEFORMAT_INHERITED;

		// initialize subgraphs
		UpTrendline.Name = "Up Trendline";
		UpTrendline.PrimaryColor = RGB(0, 128, 255);
		UpTrendline.LineWidth = 2;
		UpTrendline.DrawZeros = false;

		DownTrendline.Name = "Down Trendline";
		DownTrendline.PrimaryColor = RGB(0, 128, 255);
		DownTrendline.LineWidth = 2;
		DownTrendline.DrawZeros = false;

		// Number of bars to use when determining the trendline
		LengthInput.Name = "Length";
		LengthInput.SetInt(50);
		LengthInput.SetIntLimits(5,MAX_STUDY_LENGTH);

		// Number of bars back from the end to begin consideration for the trendline. This allows the user to have the most recent X bars ignored when determining the trendline 
		SkipLastBars.Name = "Skip Last N Bars";
		SkipLastBars.SetInt(0);
		SkipLastBars.SetIntLimits(0,MAX_STUDY_LENGTH);

		// Allows Show/Hide UP trend line. 
		ShowTrendlineUp.Name = "Show Up Trendline";
		ShowTrendlineUp.SetYesNo(1);

		// Allows Show/Hide DOWN trend line. 
		ShowTrendlineDown.Name = "Show Down Trendline";
		ShowTrendlineDown.SetYesNo(1);

		NewBarsBeforeAdjust.Name = "Number Of New Bars Before Auto-Adjustment";
		NewBarsBeforeAdjust.SetInt(5);
		NewBarsBeforeAdjust.SetIntLimits(0,MAX_STUDY_LENGTH);

		ForwardProjectAutoTrendLines.Name = "Forward Project Automatic Trendlines";
		ForwardProjectAutoTrendLines.SetYesNo(1);

		InputDataHigh.Name = "Input Data High";
		InputDataHigh.SetInputDataIndex(SC_HIGH);

		InputDataLow.Name = "Input Data Low";
		InputDataLow.SetInputDataIndex(SC_LOW);

		// version update
		VersionUpdate.SetInt(2);

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

	if (VersionUpdate.GetInt() < 1)
	{
		ForwardProjectAutoTrendLines.SetYesNo(1);
		VersionUpdate.SetInt(1);
	}
	if (VersionUpdate.GetInt() < 2)
	{
		InputDataHigh.SetInputDataIndex(SC_HIGH);
		InputDataLow.SetInputDataIndex(SC_LOW);
	}

	int& TLUIndex1	= sc.PersistVars->i1;
	int& TLUIndex2	= sc.PersistVars->i2;
	int& TLDIndex1	= sc.PersistVars->i3;
	int& TLDIndex2	= sc.PersistVars->i4;
	int& PrevIndex	= sc.PersistVars->i5;

	float& TLU_A	= sc.PersistVars->f1;
	float& TLU_B	= sc.PersistVars->f2;
	float& TLD_A	= sc.PersistVars->f3;
	float& TLD_B	= sc.PersistVars->f4;

	int NumberOfExtendedElements = 0;
	
	if(ForwardProjectAutoTrendLines.GetYesNo() != 0)
	{
		NumberOfExtendedElements = 10 ;
		UpTrendline.ExtendedArrayElementsToGraph = 10;
		DownTrendline.ExtendedArrayElementsToGraph = 10;
	}
	else
	{
		NumberOfExtendedElements = 0;
		UpTrendline.ExtendedArrayElementsToGraph = 0;
		DownTrendline.ExtendedArrayElementsToGraph = 0;
	}

	// set index from which study will look back
	int LastIndex = sc.ArraySize - 1;
	int LastExtendedIndex = LastIndex + NumberOfExtendedElements;
	int StartIndex = LastIndex - SkipLastBars.GetInt();

	SCFloatArrayRef Highs = sc.BaseData[InputDataHigh.GetInputDataIndex()];
	SCFloatArrayRef Lows = sc.BaseData[InputDataLow.GetInputDataIndex()];

	if(sc.UpdateStartIndex == 0)
	{
		// on initial calculation set PrevIndex to last index in array
		// StartIndex already set to correct value
		PrevIndex = sc.ArraySize-1;

		TLUIndex1	= 0;
		TLUIndex2	= 0;
		TLDIndex1	= 0;
		TLDIndex2	= 0;
		PrevIndex	= -1;
	}
	else
	{
		// set initial value of variable or adjust on replay
		if(PrevIndex == -1 || PrevIndex > LastIndex)
			PrevIndex = LastIndex;

		// perform adjust only after specified number of new bars were added
		int diff = LastIndex - PrevIndex;
		
		// if we should readjust trendline on each bar's changing then 
		// do not any additional check
		if(NewBarsBeforeAdjust.GetInt() > 0)
		{
			// if NewBarsBeforeAdjust greater then 0 then 
			// we should readjust trendline only when Nth bar will be closed
			// so if we should readjust at 3rd bar we should wait when 3rd bar will be closed and then readjust trendlines
			if( (diff < NewBarsBeforeAdjust.GetInt()))
			{
				// we shouldn't readjust trendline
				// Only extend it to the last bar
				if(TLUIndex2 < LastExtendedIndex || TLDIndex2 < LastExtendedIndex)
				{
					if(TLUIndex2 > 0) 
					{
						for(int Index=TLUIndex1; Index<=LastExtendedIndex; Index++ )
							UpTrendline[Index]	= (float)(TLU_A*Index + TLU_B);

						TLUIndex2 = LastExtendedIndex;
					}

					if(TLDIndex2 > 0) 
					{
						for(int Index=TLDIndex1; Index<=LastExtendedIndex; Index++ )
							DownTrendline[Index]	= (float)(TLD_A*Index + TLD_B);

						TLDIndex2 = LastExtendedIndex;
					}
				}

				// Exit. Don't readjust trendlines
				return;
			}
		}

		// update previous index and StartIndex to re-adjust
		PrevIndex = LastIndex;
		StartIndex = LastIndex - SkipLastBars.GetInt();
	}

	if(ShowTrendlineUp.GetYesNo() == 0 && ShowTrendlineDown.GetYesNo() == 0)
		return;

	if(StartIndex < LengthInput.GetInt())
		return;


	int StartIndex1 = StartIndex; 
	int LastIndex1 = StartIndex - LengthInput.GetInt(); 


	bool TLDrawn = false;

	// the variables which describe trendline equality
	double K = 0;
	double A = 0;
	double B = 0;

	// calculate UP trendlines
	if(ShowTrendlineUp.GetYesNo() != 0)
	{
		// find LowestLow Index
		int LowestLowIndex1 = StartIndex1;
		int LowestLowIndex2 = -1;

		for(int Index=StartIndex1; Index>=LastIndex1; Index--)
		{
			if(Lows[Index] < Lows[LowestLowIndex1])
				LowestLowIndex1 = Index;
		}

		// set second lowest low index as first lowest low + min trend line length
		LowestLowIndex2 = LowestLowIndex1;

		// search second LowestLow
		while(!TLDrawn)
		{
			// create new region where search lowest low
			int StartIndex2 = LowestLowIndex2 + 1; 
			int LastIndex2 = StartIndex; 

			if(StartIndex2 > LastIndex2)
			{
				TLDrawn = false;
				break;
			}

			LowestLowIndex2 = StartIndex2;
			for(int Index=StartIndex2; Index<=LastIndex2; Index++)
			{
				if(Lows[Index] < Lows[LowestLowIndex2])
					LowestLowIndex2 = Index;
			}

			// check if the line doesn't cross any other bars in region

			// calculate trendline between Low on Index1 and Low on Index2
			// A and B - these are coefficients of line equation
			K = (double)(LowestLowIndex2 - LowestLowIndex1);
			A = 0;
			B = 0;

			if(K != 0)
			{
				A = (Lows[LowestLowIndex2] - Lows[LowestLowIndex1])/K;
				B = Lows[LowestLowIndex1] - A * LowestLowIndex1;
			}

			// check if any of the bar between Index2 and Index1 cross the trendline
			TLDrawn = true;
			for(int Index=StartIndex1; Index>=LastIndex1; Index--)
			{
				// don't check former bars
				if(Index == LowestLowIndex2 || Index == LowestLowIndex1)
					continue;

				double TLValue = A*Index + B;
				if(TLValue > Lows[Index])
				{
					TLDrawn = false;
					break;
				}
			}
		}

		// clean previous trendline
		for(int Index=TLUIndex1; Index<=LastExtendedIndex; Index++)
			UpTrendline[Index] = 0;

		TLUIndex1 = 0;
		TLUIndex2 = 0;

		// draw trendline if it was found
		if(TLDrawn)
		{
			// We found correct trend line. Draw it
			UpTrendline[LowestLowIndex1]	= Lows[LowestLowIndex1];

			// fill all elements
			for(int Index=LowestLowIndex1+1; Index<=LastExtendedIndex; Index++ )
				UpTrendline[Index]	= (float)(A*Index + B);

			// save indexes in order to fast clean last drawn trendline
			TLUIndex1 = LowestLowIndex1;
			TLUIndex2 = LastExtendedIndex;

			TLU_A = (float)A;
			TLU_B = (float)B;
		}
	}

	// calculate DOWN trendlines
	
	TLDrawn = false;

	if(ShowTrendlineDown.GetYesNo() != 0)
	{
		// find LowestLow Index
		int HighestHighIndex1 = StartIndex1;
		int HighestHighIndex2 = -1;

		for(int Index=StartIndex1; Index>=LastIndex1; Index--)
		{
			if(Highs[Index] > Highs[HighestHighIndex1])
				HighestHighIndex1 = Index;
		}

		// set second lowest low index as first lowest low + min trend line length
		HighestHighIndex2 = HighestHighIndex1;

		// search second LowestLow
		while(!TLDrawn)
		{
			// create new region where search lowest low
			int StartIndex2 = HighestHighIndex2 + 1; 
			int LastIndex2 = StartIndex; 

			if(StartIndex2 > LastIndex2)
			{
				TLDrawn = false;
				break;
			}

			HighestHighIndex2 = StartIndex2;
			for(int Index=StartIndex2; Index<=LastIndex2; Index++)
			{
				if(Highs[Index] > Highs[HighestHighIndex2])
					HighestHighIndex2 = Index;
			}

			// check if the line doesn't cross any other bars in region

			// calculate trendline between Low on Index1 and Low on Index2
			// A and B - these are coefficients of line equation
			K = (double)(HighestHighIndex2 - HighestHighIndex1);
			A = 0;
			B = 0;

			if(K != 0)
			{
				A = (Highs[HighestHighIndex2] - Highs[HighestHighIndex1])/K;
				B = Highs[HighestHighIndex1] - A * HighestHighIndex1;
			}

			// check if any of the bar between Index2 and Index1 cross the trendline
			TLDrawn = true;
			for(int Index=StartIndex1; Index>=LastIndex1; Index--)
			{
				// don't check former bars
				if(Index == HighestHighIndex2 || Index == HighestHighIndex1)
					continue;

				double TLValue = A*Index + B;
				if(TLValue < Highs[Index])
				{
					TLDrawn = false;
					break;
				}
			}
		}

		// clean previous trendline
		for(int Index=TLDIndex1; Index<=LastExtendedIndex; Index++)
			DownTrendline[Index] = 0;

		TLDIndex1 = 0;
		TLDIndex2 = 0;

		// draw trendline if it was found
		if(TLDrawn)
		{
			// We found correct trend line. Draw it
			DownTrendline[HighestHighIndex1]	= Highs[HighestHighIndex1];
			
			// fill all elements
			for(int Index=HighestHighIndex1+1; Index<=LastExtendedIndex; Index++ )
				DownTrendline[Index]	= (float)(A*Index + B);

			// save indexes in order to fast clean last drawn trendline
			TLDIndex1 = HighestHighIndex1;
			TLDIndex2 = LastExtendedIndex;

			TLD_A = (float)A;
			TLD_B = (float)B;
		}
	}
}

/***************************************************************/

//
// Enhanced Renko Custom Chart
//

inline int RenkoFillHigh(SCStudyInterfaceRef sc, long BaseGraphIndex, long& RenkoIndex, float price, bool swap)
{
	SCSubgraphRef Open      = sc.Subgraph[SC_OPEN];
	SCSubgraphRef High      = sc.Subgraph[SC_HIGH];
	SCSubgraphRef Low       = sc.Subgraph[SC_LOW];
	SCSubgraphRef Close     = sc.Subgraph[SC_LAST];
	SCSubgraphRef RenkoOpen = sc.Subgraph[SC_RENKO_OPEN];

	float& brickSize   = sc.PersistVars->f1;
	float& renkoHigh   = sc.PersistVars->f2;
	float& renkoLow    = sc.PersistVars->f3;
	int&   barComplete = sc.PersistVars->i2;

	int numBarsAdded = 0;
  
	if (barComplete)
	{
		// if outside range, then start with previous bar close else use passed value
		float openValue = (price > renkoHigh || price < renkoLow) ? Close[RenkoIndex] : price;

		// add new bar, but keep the renko limits
		sc.AddElements(1);
		numBarsAdded++; RenkoIndex++;
		sc.DateTimeOut[RenkoIndex] = sc.BaseDateTimeIn[BaseGraphIndex];
		Open[RenkoIndex]  = openValue;
		High[RenkoIndex]  = openValue;
		Low[RenkoIndex]   = openValue;
		Close[RenkoIndex] = openValue;
		barComplete = false;
	}

	if (sc.FormattedEvaluate(price, sc.BaseGraphValueFormat, LESS_OPERATOR, renkoHigh, sc.BaseGraphValueFormat))
	{
		if (price > High[RenkoIndex])  High[RenkoIndex] = price;
		Close[RenkoIndex] = price;
		sc.CalculateOHLCAverages(RenkoIndex);
		return numBarsAdded;
	}

	// check the range to see if exceeded, and fill gap
	while (sc.FormattedEvaluate(price, sc.BaseGraphValueFormat, GREATER_OPERATOR, renkoHigh, sc.BaseGraphValueFormat))
	{
		// close current bar
		High[RenkoIndex] = renkoHigh;
		Close[RenkoIndex] = renkoHigh;
		RenkoOpen[RenkoIndex] = max(renkoHigh - brickSize, Low[RenkoIndex]);
		if (swap)
		{
			float temp = Open[RenkoIndex];
			Open[RenkoIndex] = RenkoOpen[RenkoIndex];
			RenkoOpen[RenkoIndex] = temp;
		}
		sc.CalculateOHLCAverages(RenkoIndex);

		// add new bar
		sc.AddElements(1);
		numBarsAdded++; RenkoIndex++;
		sc.DateTimeOut[RenkoIndex] = sc.BaseDateTimeIn[BaseGraphIndex];
		Open[RenkoIndex]  = renkoHigh;
		High[RenkoIndex]  = renkoHigh;
		Low[RenkoIndex]   = renkoHigh;
		Close[RenkoIndex] = renkoHigh;
		sc.CalculateOHLCAverages(RenkoIndex);
		renkoHigh += brickSize;
		renkoLow  = renkoHigh - 3.0f * brickSize;

		if (numBarsAdded >= 500)
		{
			sc.AddMessageToLog("Renko Chart Enhanced study: Error due to either data error or Brick Size too small. Stopping processing and skipping to next input bar. Try increasing the Brick Size.", 1);
			Close[RenkoIndex] = price;
			High[RenkoIndex] = price;
			renkoHigh = price;
			renkoLow  = price - 3.0f * brickSize;
		}
	}

	// when done, bar can be complete or not
	if (sc.FormattedEvaluate(price, sc.BaseGraphValueFormat, EQUAL_OPERATOR, renkoHigh, sc.BaseGraphValueFormat))
	{
		High[RenkoIndex] = price;
		Close[RenkoIndex] = price;
		RenkoOpen[RenkoIndex] = max(renkoHigh - brickSize, Low[RenkoIndex]);
		if (swap)
		{
			float temp = Open[RenkoIndex];
			Open[RenkoIndex] = RenkoOpen[RenkoIndex];
			RenkoOpen[RenkoIndex] = temp;
		}
		sc.CalculateOHLCAverages(RenkoIndex);
		renkoHigh += brickSize;
		renkoLow  = renkoHigh - 3.0f * brickSize;
		barComplete = true;
	}
	else
	{
		Close[RenkoIndex] = price;
		sc.CalculateOHLCAverages(RenkoIndex);
		barComplete = false;
	}

	return numBarsAdded;
}


inline int RenkoFillLow(SCStudyInterfaceRef sc, long BaseGraphIndex, long& RenkoIndex, float price, bool swap)
{
	SCSubgraphRef Open      = sc.Subgraph[SC_OPEN];
	SCSubgraphRef High      = sc.Subgraph[SC_HIGH];
	SCSubgraphRef Low       = sc.Subgraph[SC_LOW];
	SCSubgraphRef Close     = sc.Subgraph[SC_LAST];
	SCSubgraphRef RenkoOpen = sc.Subgraph[SC_RENKO_OPEN];

	float& brickSize   = sc.PersistVars->f1;
	float& renkoHigh   = sc.PersistVars->f2;
	float& renkoLow    = sc.PersistVars->f3;
	int&   barComplete = sc.PersistVars->i2;

	int numBarsAdded = 0;

	if (barComplete)
	{
		// if outside range, then start with previous bar close else use passed value
		float openValue = (price > renkoHigh || price < renkoLow) ? Close[RenkoIndex] : price;

		// add new bar, but keep the renko limits
		sc.AddElements(1);
		numBarsAdded++; RenkoIndex++;
		sc.DateTimeOut[RenkoIndex] = sc.BaseDateTimeIn[BaseGraphIndex];
		Open[RenkoIndex]  = openValue;
		High[RenkoIndex]  = openValue;
		Low[RenkoIndex]   = openValue;
		Close[RenkoIndex] = openValue;
		sc.CalculateOHLCAverages(RenkoIndex);
		barComplete = false;
	}

	if (sc.FormattedEvaluate(price, sc.BaseGraphValueFormat, GREATER_OPERATOR, renkoLow, sc.BaseGraphValueFormat))
	{
		if (price < Low[RenkoIndex])  Low[RenkoIndex] = price;
		Close[RenkoIndex] = price;
		sc.CalculateOHLCAverages(RenkoIndex);
		return numBarsAdded;
	}

	// check the range to see if exceeded, and fill gap
	while (sc.FormattedEvaluate(price, sc.BaseGraphValueFormat, LESS_OPERATOR, renkoLow, sc.BaseGraphValueFormat))
	{
		// close current bar
		Low[RenkoIndex] = renkoLow;
		Close[RenkoIndex] = renkoLow;
		RenkoOpen[RenkoIndex] = min(renkoLow + brickSize, High[RenkoIndex]);
		if (swap)
		{
			float temp = Open[RenkoIndex];
			Open[RenkoIndex] = RenkoOpen[RenkoIndex];
			RenkoOpen[RenkoIndex] = temp;
		}
		sc.CalculateOHLCAverages(RenkoIndex);

		// add new bar
		sc.AddElements(1);
		numBarsAdded++; RenkoIndex++;
		sc.DateTimeOut[RenkoIndex] = sc.BaseDateTimeIn[BaseGraphIndex];
		Open[RenkoIndex]  = renkoLow;
		High[RenkoIndex]  = renkoLow;
		Low[RenkoIndex]   = renkoLow;
		Close[RenkoIndex] = renkoLow;
		sc.CalculateOHLCAverages(RenkoIndex);
		renkoLow -= brickSize;
		renkoHigh = renkoLow + 3.0f * brickSize;

		if (numBarsAdded >= 500)
		{
			sc.AddMessageToLog("Renko Chart Enhanced study: Error due to either data error or Brick Size too small. Stopping processing and skipping to next input bar. Try increasing the Brick Size.", 1);
			Close[RenkoIndex] = price;
			Low[RenkoIndex] = price;
			renkoLow = price;
			renkoHigh = price + 3.0f * brickSize;
		}
	}

	// when done, bar can be complete or not
	if (sc.FormattedEvaluate(price, sc.BaseGraphValueFormat, EQUAL_OPERATOR, renkoLow, sc.BaseGraphValueFormat))
	{
		Low[RenkoIndex] = price;
		Close[RenkoIndex] = price;
		RenkoOpen[RenkoIndex] = min(renkoLow + brickSize, High[RenkoIndex]);
		if (swap)
		{
			float temp = Open[RenkoIndex];
			Open[RenkoIndex] = RenkoOpen[RenkoIndex];
			RenkoOpen[RenkoIndex] = temp;
		}
		sc.CalculateOHLCAverages(RenkoIndex);
		renkoLow -= brickSize;
		renkoHigh = renkoLow + 3.0f * brickSize;
		barComplete = true;
	}
	else
	{
		Close[RenkoIndex] = price;
		sc.CalculateOHLCAverages(RenkoIndex);
		barComplete = false;
	}

	return numBarsAdded;
}

/************************************************************************/


inline int RenkoFill(SCStudyInterfaceRef sc, long BaseGraphIndex, long& RenkoIndex, float price, bool swap)
{
	int numBarsAdded = 0;

	numBarsAdded += RenkoFillHigh(sc, BaseGraphIndex, RenkoIndex, price, swap);
	numBarsAdded += RenkoFillLow(sc, BaseGraphIndex, RenkoIndex, price, swap);

	return numBarsAdded;
}

/************************************************************************/

SCSFExport scsf_RenkoChartEnhanced(SCStudyInterfaceRef sc)
{
	// Input references
	SCInputRef brickSizeTicks    = sc.Input[0];
	SCInputRef alignToBrickSize  = sc.Input[1];
	SCInputRef showRealOpen      = sc.Input[2];
	SCInputRef fillSessionGap    = sc.Input[3];

	// Base data references
	SCFloatArrayRef iOpen          = sc.BaseDataIn[SC_OPEN];
	SCFloatArrayRef iHigh          = sc.BaseDataIn[SC_HIGH];
	SCFloatArrayRef iLow           = sc.BaseDataIn[SC_LOW];
	SCFloatArrayRef iClose         = sc.BaseDataIn[SC_LAST];
	SCFloatArrayRef iVolume        = sc.BaseDataIn[SC_VOLUME];
	SCFloatArrayRef iNumTrades     = sc.BaseDataIn[SC_NT];
	SCFloatArrayRef iBidVol        = sc.BaseDataIn[SC_BIDVOL];
	SCFloatArrayRef iAskVol        = sc.BaseDataIn[SC_ASKVOL];
	SCFloatArrayRef iUpTickVol     = sc.BaseDataIn[SC_UPVOL];
	SCFloatArrayRef iDownTickVol   = sc.BaseDataIn[SC_DOWNVOL];
	SCFloatArrayRef iBidTrades     = sc.BaseDataIn[SC_BIDNT];
	SCFloatArrayRef iAskTrades     = sc.BaseDataIn[SC_ASKNT];
	SCFloatArrayRef iBidAskDiffMax = sc.BaseDataIn[SC_ASKBID_DIFF_HIGH];
	SCFloatArrayRef iBidAskDiffMin = sc.BaseDataIn[SC_ASKBID_DIFF_LOW];
	SCFloatArrayRef iUpDownTickDiffMax = sc.BaseDataIn[SC_ASKBID_NT_DIFF_HIGH];
	SCFloatArrayRef iUpDownTickDiffMin = sc.BaseDataIn[SC_ASKBID_NT_DIFF_LOW];

	//  Output bar references
	SCSubgraphRef Open           = sc.Subgraph[SC_OPEN];
	SCSubgraphRef High           = sc.Subgraph[SC_HIGH];
	SCSubgraphRef Low            = sc.Subgraph[SC_LOW];
	SCSubgraphRef Close          = sc.Subgraph[SC_LAST];
	SCSubgraphRef Volume         = sc.Subgraph[SC_VOLUME];
	SCSubgraphRef NumTrades      = sc.Subgraph[SC_NT];
	SCSubgraphRef OHLCAvg        = sc.Subgraph[SC_OHLC];
	SCSubgraphRef HLCAvg         = sc.Subgraph[SC_HLC];
	SCSubgraphRef HLAvg          = sc.Subgraph[SC_HL];
	SCSubgraphRef BidVol         = sc.Subgraph[SC_BIDVOL];
	SCSubgraphRef AskVol         = sc.Subgraph[SC_ASKVOL];
	SCSubgraphRef UpTickVol      = sc.Subgraph[SC_UPVOL];
	SCSubgraphRef DownTickVol    = sc.Subgraph[SC_DOWNVOL];
	SCSubgraphRef BidTrades      = sc.Subgraph[SC_BIDNT];
	SCSubgraphRef AskTrades      = sc.Subgraph[SC_ASKNT];
	SCSubgraphRef BidAskDiffMax  = sc.Subgraph[SC_ASKBID_DIFF_HIGH];
	SCSubgraphRef BidAskDiffMin  = sc.Subgraph[SC_ASKBID_DIFF_LOW];
	SCSubgraphRef UpDownTickDiffMax = sc.Subgraph[SC_ASKBID_NT_DIFF_HIGH];
	SCSubgraphRef UpDownTickDiffMin = sc.Subgraph[SC_ASKBID_NT_DIFF_LOW];
	SCSubgraphRef RenkoOpen         = sc.Subgraph[SC_RENKO_OPEN];  // Renko Open or True Open based on input value


	// Set configuration variables
	if (sc.SetDefaults)
	{
		// Set the defaults
		sc.GraphName = "Renko Chart Enhanced";
		sc.StudyDescription = "Enhanced Renko custom chart.";
		
		sc.IsCustomChart = 1;
		sc.GraphRegion = 0;
		sc.StandardChartHeader = 1;
		sc.GraphUsesChartColors= true;
		sc.GraphDrawType = GDT_CANDLESTICK;
		sc.AutoLoop = 0;
		sc.ValueFormat = VALUEFORMAT_INHERITED;
		sc.MaintainAdditionalChartDataArrays = 1;

		// Inputs		
		brickSizeTicks.Name = "Brick Size In Ticks";
		brickSizeTicks.SetInt(4);
		brickSizeTicks.SetIntLimits(1,MAX_STUDY_LENGTH);

		alignToBrickSize.Name = "Align To Brick Size";
		alignToBrickSize.SetYesNo(1);

		showRealOpen.Name = "Show Real Open (vs Renko Open)";
		showRealOpen.SetYesNo(0);

		fillSessionGap.Name = "Fill Session Gap";
		fillSessionGap.SetYesNo(0);

		// Subgraphs

		Open.Name = "Open";
		Open.DrawStyle = DRAWSTYLE_LINE;
		Open.PrimaryColor = RGB(0, 255, 0);
		Open.SecondaryColorUsed = true;
		Open.SecondaryColor = RGB(0, 255, 0);
		Open.DrawZeros = false;

		High.Name = "High";
		High.DrawStyle = DRAWSTYLE_LINE;
		High.PrimaryColor = RGB(0, 128, 0);
		High.DrawZeros = false;

		Low.Name = "Low";
		Low.DrawStyle = DRAWSTYLE_LINE;
		Low.PrimaryColor = RGB(255,0,0);
		Low.SecondaryColor = RGB(255,0,0);
		Low.SecondaryColorUsed = true;
		Low.DrawZeros = false;

		Close.Name = "Close";
		Close.DrawStyle = DRAWSTYLE_LINE;
		Close.PrimaryColor = RGB(128, 0, 0);
		Close.DrawZeros = false;

		Volume.Name = "Volume";
		Volume.DrawStyle = DRAWSTYLE_IGNORE;
		Volume.PrimaryColor = RGB(255,0,0);
		Volume.DrawZeros = false;
		
		NumTrades.Name = "# of Trades";
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

		UpTickVol.Name = "Up Tick Vol";
		UpTickVol.DrawStyle = DRAWSTYLE_IGNORE;
		UpTickVol.PrimaryColor = RGB(0,255,0);
		UpTickVol.DrawZeros = false;

		DownTickVol.Name = "Down Tick Vol";
		DownTickVol.DrawStyle = DRAWSTYLE_IGNORE;
		DownTickVol.PrimaryColor = RGB(0,255,0);
		DownTickVol.DrawZeros = false;

		BidTrades.Name = "Bid Trades";
		BidTrades.DrawStyle = DRAWSTYLE_IGNORE;
		BidTrades.PrimaryColor = RGB(0,255,0);
		BidTrades.DrawZeros = false;

		AskTrades.Name = "Ask Trades";
		AskTrades.DrawStyle = DRAWSTYLE_IGNORE;
		AskTrades.PrimaryColor = RGB(0,255,0);
		AskTrades.DrawZeros = false;

// BidAskDiffMax.Name = "BidAskDiffMax";
// BidAskDiffMax.DrawStyle = DRAWSTYLE_IGNORE;
// BidAskDiffMin.Name = "BidAskDiffMin";
// BidAskDiffMin.DrawStyle = DRAWSTYLE_IGNORE;
// UpDownTickDiffMax.Name = "UpDownTickDiffMax";
// UpDownTickDiffMax.DrawStyle = DRAWSTYLE_IGNORE;
// UpDownTickDiffMin.Name = "UpDownTickDiffMin";
// UpDownTickDiffMin.DrawStyle = DRAWSTYLE_IGNORE;

		RenkoOpen.Name = "Renko/True Open";
		RenkoOpen.DrawStyle = DRAWSTYLE_IGNORE;
		RenkoOpen.PrimaryColor = RGB(0,255,0);
		RenkoOpen.DrawZeros = false;

		return;
	}

	// refs to persistent vars
	float& brickSize   = sc.PersistVars->f1;  // brickSize in terms of price
	float& renkoHigh   = sc.PersistVars->f2;
	float& renkoLow    = sc.PersistVars->f3;
	int&   lastIndex   = sc.PersistVars->i1;
	int&   barComplete = sc.PersistVars->i2;
	
	SCDateTime& nextSessionStart = sc.PersistVars->scdt1;

	// track previous value of cumulative fields
	float& lastHigh        = sc.PersistVars->f4;
	float& lastLow         = sc.PersistVars->f5;
	float& lastVolume      = sc.PersistVars->f6;
	float& lastNumTrades   = sc.PersistVars->f7;
	float& lastBidVol      = sc.PersistVars->f8;
	float& lastAskVol      = sc.PersistVars->f9;
	float& lastUpTickVol   = sc.PersistVars->f10;
	float& lastDownTickVol = sc.PersistVars->f11;
	float& lastBidTrades   = sc.PersistVars->f12;
	float& lastAskTrades   = sc.PersistVars->f13;

	int& FeatureMessageAdded   = sc.PersistVars->Integers [0];

	if (!sc.AdvancedFeatures)
	{
		if (FeatureMessageAdded == 0)
		{
			sc.AddMessageToLog(SPECIAL_FEATURES, 1);
			sc.AddMessageToLog("Renko Chart study: To restore the chart, remove the Renko chart study.", 1);
			sc.ResizeArrays(1);
			FeatureMessageAdded = 1;
		}
		return;
	}


	if (sc.UpdateStartIndex == 0)
	{
		sc.ResizeArrays(0);

		sc.ValueFormat = sc.BaseGraphValueFormat;
		sc.GraphName.Format("%s Renko - BoxSize: %i", sc.GetStudyName(0).GetChars(), brickSizeTicks.GetInt());
		RenkoOpen.Name = !showRealOpen.GetYesNo() ? "True Open" : "Renko Open";

		// init state
		brickSize = sc.RoundToTickSize(brickSizeTicks.GetInt() * sc.TickSize, sc.TickSize);
		lastIndex = -1;
		barComplete = false;
	}

	long RenkoIndex = sc.OutArraySize - 1;

	for (long BaseGraphIndex = sc.UpdateStartIndex; BaseGraphIndex < sc.ArraySize; BaseGraphIndex++)
	{
		bool newInputBar = BaseGraphIndex != lastIndex;

		// if same bar and no new ticks, then continue
		if (!newInputBar && iNumTrades[BaseGraphIndex] == lastNumTrades)
			continue;

		if (newInputBar)
		{
			// zero out last values
			lastVolume      = 0;
			lastNumTrades   = 0;
			lastBidVol      = 0;
			lastAskVol      = 0;
			lastUpTickVol   = 0;
			lastDownTickVol = 0;
			lastBidTrades   = 0;
			lastAskTrades   = 0;
		}

		int numBarsAdded = 0;
		bool swap = !showRealOpen.GetYesNo();

		// starting new bar at session boundary ensures same bars, regardless of first date loaded in chart
		if (newInputBar && 
			(BaseGraphIndex == 0 || sc.BaseDateTimeIn[BaseGraphIndex] >= nextSessionStart))
		{
			nextSessionStart = sc.GetTradingDayStartDateTimeOfBar(sc.BaseDateTimeIn[BaseGraphIndex]) + 1*DAYS;

			if (BaseGraphIndex != 0)
			{
				if (fillSessionGap.GetYesNo())
				{
					// these bars do not contribute to bars added for volume distribution
					RenkoFill(sc, BaseGraphIndex, RenkoIndex, iOpen[BaseGraphIndex], swap);
				}

				// close and adjust current bar if any
				if (!barComplete) 
				{
					RenkoOpen[RenkoIndex] = Close[RenkoIndex-1];  // do not know if up or down bar
					if (swap)
					{
						float temp = Open[RenkoIndex];
						Open[RenkoIndex] = RenkoOpen[RenkoIndex];
						RenkoOpen[RenkoIndex] = temp;
					}
					sc.CalculateOHLCAverages(RenkoIndex);
				}
			}

			// start new bar for session
			sc.AddElements(1);
			RenkoIndex++; numBarsAdded++;
			sc.DateTimeOut[RenkoIndex] = sc.BaseDateTimeIn[BaseGraphIndex];
			Open[RenkoIndex]  = iOpen[BaseGraphIndex];
			High[RenkoIndex]  = iOpen[BaseGraphIndex];
			Low[RenkoIndex]   = iOpen[BaseGraphIndex];
			Close[RenkoIndex] = iOpen[BaseGraphIndex];
			sc.CalculateOHLCAverages(RenkoIndex);

			// adjust renko limits to align first brick of session to natural alignment
			float mod = !alignToBrickSize.GetYesNo() ? brickSize : sc.RoundToTickSize(fmod(Close[RenkoIndex], brickSize), sc.TickSize);
			float mid = sc.FormattedEvaluate(mod, sc.BaseGraphValueFormat, EQUAL_OPERATOR, brickSize, sc.BaseGraphValueFormat) ? Close[RenkoIndex] : Close[RenkoIndex] - mod;
			renkoHigh = mid + brickSize;
			renkoLow  = mid - brickSize;
			barComplete = false;

			// continue to process the bar
		}
		

		if (newInputBar)
		{
			if (sc.NumberOfTradesPerBar == 1)
			{
				// for tick bars, max of one bar added for volume purposes (i.e. gap)
				numBarsAdded = RenkoFill(sc, BaseGraphIndex, RenkoIndex, iClose[BaseGraphIndex], swap);
				if (numBarsAdded > 1)
					numBarsAdded = 1;
			}
			else
			{
				// open price does not contribute to number of bars added for volume purposes (i.e. gap)
				numBarsAdded = RenkoFill(sc, BaseGraphIndex, RenkoIndex, iOpen[BaseGraphIndex], swap);
				if (numBarsAdded > 1)
					numBarsAdded = 1;

				if (iClose[BaseGraphIndex] > iOpen[BaseGraphIndex])
				{
					numBarsAdded += RenkoFillLow(sc, BaseGraphIndex, RenkoIndex, iLow[BaseGraphIndex], swap);
					numBarsAdded += RenkoFillHigh(sc, BaseGraphIndex, RenkoIndex, iHigh[BaseGraphIndex], swap);
				}
				else
				{
					numBarsAdded += RenkoFillHigh(sc, BaseGraphIndex, RenkoIndex, iHigh[BaseGraphIndex], swap);
					numBarsAdded += RenkoFillLow(sc, BaseGraphIndex, RenkoIndex, iLow[BaseGraphIndex], swap);
				}

				numBarsAdded += RenkoFill(sc, BaseGraphIndex, RenkoIndex, iClose[BaseGraphIndex], swap);
			}
		}
		else
		{
			if (iClose[BaseGraphIndex] > iOpen[BaseGraphIndex])
			{
				if (iLow[BaseGraphIndex] != lastLow)
					numBarsAdded += RenkoFillLow(sc, BaseGraphIndex, RenkoIndex, iLow[BaseGraphIndex], swap);
				if (iHigh[BaseGraphIndex] != lastHigh)
					numBarsAdded += RenkoFillHigh(sc, BaseGraphIndex, RenkoIndex, iHigh[BaseGraphIndex], swap);
			}
			else
			{
				if (iHigh[BaseGraphIndex] != lastHigh)
					numBarsAdded += RenkoFillHigh(sc, BaseGraphIndex, RenkoIndex, iHigh[BaseGraphIndex], swap);
				if (iLow[BaseGraphIndex] != lastLow)
					numBarsAdded += RenkoFillLow(sc, BaseGraphIndex, RenkoIndex, iLow[BaseGraphIndex], swap);
			}

			numBarsAdded += RenkoFill(sc, BaseGraphIndex, RenkoIndex, iClose[BaseGraphIndex], swap);
		}


		bool  HaveBidAskDiff = sc.BaseDataIn.GetArraySize() >= SC_ASKBID_DIFF_LOW+1;

		if (HaveBidAskDiff)
		{
			// BidAskDiff is not distributed, only updated on last bar
			float value;
			
			value = max(AskVol[RenkoIndex] - BidVol[RenkoIndex], iBidAskDiffMax[BaseGraphIndex]);
			if (numBarsAdded > 0 || value > BidAskDiffMax[RenkoIndex]) 
				BidAskDiffMax[RenkoIndex] = value;

			value = min(AskVol[RenkoIndex] - BidVol[RenkoIndex], iBidAskDiffMin[BaseGraphIndex]);
			if (numBarsAdded > 0 || value < BidAskDiffMin[RenkoIndex]) 
				BidAskDiffMin[RenkoIndex] = value;

			value = max(AskTrades[RenkoIndex] - BidTrades[RenkoIndex], iUpDownTickDiffMax[BaseGraphIndex]);
			if (numBarsAdded > 0 || value > UpDownTickDiffMax[RenkoIndex]) 
				UpDownTickDiffMax[RenkoIndex] = value;

			value = min(AskTrades[RenkoIndex] - BidTrades[RenkoIndex], iUpDownTickDiffMin[BaseGraphIndex]);
			if (numBarsAdded > 0 || value < UpDownTickDiffMin[RenkoIndex]) 
				UpDownTickDiffMin[RenkoIndex] = value;
		}


		if (numBarsAdded <= 1)
		{
			Volume[RenkoIndex]      += (iVolume[BaseGraphIndex] - lastVolume);
			NumTrades[RenkoIndex]   += (iNumTrades[BaseGraphIndex] - lastNumTrades);
			BidVol[RenkoIndex]      += (iBidVol[BaseGraphIndex] - lastBidVol);
			AskVol[RenkoIndex]      += (iAskVol[BaseGraphIndex] - lastAskVol);
			UpTickVol[RenkoIndex]   += (iUpTickVol[BaseGraphIndex] - lastUpTickVol);
			DownTickVol[RenkoIndex] += (iDownTickVol[BaseGraphIndex] - lastDownTickVol);
			BidTrades[RenkoIndex]   += (iBidTrades[BaseGraphIndex] - lastBidTrades);
			AskTrades[RenkoIndex]   += (iAskTrades[BaseGraphIndex] - lastAskTrades);
		}
		else
		{
			// split volume across bars added, with mod allocated to last bar
			Volume[RenkoIndex]      = (iVolume[BaseGraphIndex] - lastVolume);
			NumTrades[RenkoIndex]   = (iNumTrades[BaseGraphIndex] - lastNumTrades);
			BidVol[RenkoIndex]      = (iBidVol[BaseGraphIndex] - lastBidVol);
			AskVol[RenkoIndex]      = (iAskVol[BaseGraphIndex] - lastAskVol);
			UpTickVol[RenkoIndex]   = (iUpTickVol[BaseGraphIndex] - lastUpTickVol);
			DownTickVol[RenkoIndex] = (iDownTickVol[BaseGraphIndex] - lastDownTickVol);
			BidTrades[RenkoIndex]   = (iBidTrades[BaseGraphIndex] - lastBidTrades);
			AskTrades[RenkoIndex]   = (iAskTrades[BaseGraphIndex] - lastAskTrades);

			int VolumeAllocation      = (int)Volume[RenkoIndex] / numBarsAdded;
			int NumTradesAllocation   = (int)NumTrades[RenkoIndex] / numBarsAdded;
			int BidVolAllocation      = (int)BidVol[RenkoIndex] / numBarsAdded;
			int AskVolAllocation      = (int)AskVol[RenkoIndex] / numBarsAdded;
			int UpTickVolAllocation   = (int)UpTickVol[RenkoIndex] / numBarsAdded;
			int DownTickVolAllocation = (int)DownTickVol[RenkoIndex] / numBarsAdded;
			int BidTradesAllocation   = (int)BidTrades[RenkoIndex] / numBarsAdded;
			int AskTradesAllocation   = (int)AskTrades[RenkoIndex] / numBarsAdded;
			
			for (long VolumeUpdateIndex = RenkoIndex - numBarsAdded + 1; VolumeUpdateIndex < RenkoIndex; VolumeUpdateIndex++)
			{
				Volume[VolumeUpdateIndex]      = (float)VolumeAllocation;
				NumTrades[VolumeUpdateIndex]   = (float)NumTradesAllocation;
				if (!HaveBidAskDiff)
				{
					BidVol[VolumeUpdateIndex]      = (float)BidVolAllocation;
					AskVol[VolumeUpdateIndex]      = (float)AskVolAllocation;
				}
				UpTickVol[VolumeUpdateIndex]   = (float)UpTickVolAllocation;
				DownTickVol[VolumeUpdateIndex] = (float)DownTickVolAllocation;
				BidTrades[VolumeUpdateIndex]   = (float)BidTradesAllocation;
				AskTrades[VolumeUpdateIndex]   = (float)AskTradesAllocation;

				Volume[RenkoIndex]      -= VolumeAllocation;
				NumTrades[RenkoIndex]   -= NumTradesAllocation;
				if (!HaveBidAskDiff)
				{
					BidVol[RenkoIndex]      -= BidVolAllocation;
					AskVol[RenkoIndex]      -= AskVolAllocation;
				}
				UpTickVol[RenkoIndex]   -= UpTickVolAllocation;
				DownTickVol[RenkoIndex] -= DownTickVolAllocation;
				BidTrades[RenkoIndex]   -= BidTradesAllocation;
				AskTrades[RenkoIndex]   -= AskTradesAllocation;
			}
		}

		// save last values
		lastIndex       = BaseGraphIndex;
		lastLow         = iLow[BaseGraphIndex];
		lastHigh        = iHigh[BaseGraphIndex];
		lastVolume      = iVolume[BaseGraphIndex];
		lastNumTrades   = iNumTrades[BaseGraphIndex];
		lastBidVol      = iBidVol[BaseGraphIndex];
		lastAskVol      = iAskVol[BaseGraphIndex];
		lastUpTickVol   = iUpTickVol[BaseGraphIndex];
		lastDownTickVol = iDownTickVol[BaseGraphIndex];
		lastBidTrades   = iBidTrades[BaseGraphIndex];
		lastAskTrades   = iAskTrades[BaseGraphIndex];
	}
}

/************************************************************************/


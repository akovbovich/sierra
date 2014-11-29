#include  <iterator>

#include "sierrachart.h"
#include "scstudyfunctions.h"



/*==========================================================================*/
SCSFExport scsf_MarketStructure(SCStudyInterfaceRef sc)
{
	SCFloatArrayRef High = sc.High;
	SCFloatArrayRef Low = sc.Low;
	SCFloatArrayRef Open = sc.Open;
	SCFloatArrayRef Close = sc.Close;

	SCSubgraphRef MSH = sc.Subgraph[0];
	SCSubgraphRef MSL = sc.Subgraph[1];
	SCSubgraphRef Labels = sc.Subgraph[2];
	SCSubgraphRef BarTypes = sc.Subgraph[3];

	SCInputRef DrawLabels = sc.Input[0];
	SCInputRef LabelsOffset = sc.Input[1];
	SCInputRef DisplayPriceValues = sc.Input[2];
	
	// Set configuration variables
	if (sc.SetDefaults)
	{

		sc.GraphName="Market Structure MSL/MSH";
		

		sc.GraphRegion = 0; // Use the main price graph region

		MSH.Name = "MSH";
		MSH.PrimaryColor = RGB(255,0 ,255); 
		MSH.DrawStyle = DRAWSTYLE_COLORBAR;
		MSH.LineWidth = 2;
		MSH.DrawZeros = false;

		MSL.Name = "MSL";
		MSL.PrimaryColor = RGB(255, 128, 0);
		MSL.DrawStyle = DRAWSTYLE_COLORBAR;
		MSL.LineWidth = 2;
		MSL.DrawZeros = false;

		Labels.Name = "Labels";
		Labels.DrawStyle = DRAWSTYLE_CUSTOM_TEXT;
		Labels.PrimaryColor = RGB(255, 0 ,255);
		Labels.SecondaryColor = RGB(255, 128, 0);
		Labels.LineWidth = 10;
		Labels.SecondaryColorUsed = true;
		Labels.DrawZeros = false;

		DrawLabels.Name = "Draw Labels";
		DrawLabels.SetYesNo(true);

		LabelsOffset.Name = "Text Labels Percentage Offset";
		LabelsOffset.SetFloat(5.0f);


		DisplayPriceValues.Name = "Display Price Values";
		DisplayPriceValues.SetYesNo(0);

		sc.AutoLoop				= 1;

		return;
	}

	// define constants
	enum MarketStructureValues
	{
		NONE	= 0,
		HH		= 1,
		LH		= 2,
		LL		= 3,
		HL		= 4
	};

	enum BarTypeValues
	{
		BT_DOWN,
		BT_UP,
		BT_NEUTRAL
	};

	const int UniqueID	= 1546846579;


	if(Close[sc.Index] < Open[sc.Index])
		BarTypes[sc.Index] = BT_DOWN;
	else 
		if(Close[sc.Index] > Open[sc.Index])
			BarTypes[sc.Index] = BT_UP;
		else
			BarTypes[sc.Index] = BT_NEUTRAL;

	// we need at least 3 bars
	if(sc.Index < 2)
		return;

	sc.DataStartIndex = 2;

	// Start calculations here
	int CurrentIndex = sc.Index - 1;

	if(BarTypes[CurrentIndex-2] == BT_DOWN && 
		BarTypes[CurrentIndex-1] == BT_DOWN &&
		BarTypes[CurrentIndex] == BT_UP)
	{
		// these three bars can make MSL
		if(Low[CurrentIndex-2] > Low[CurrentIndex-1] && 
			Low[CurrentIndex] > Low[CurrentIndex-1])
		{
			// this is MSL
			MSL[CurrentIndex-1] = 1;
		}
	}

	if(BarTypes[CurrentIndex-2] == BT_UP && 
		BarTypes[CurrentIndex-1] == BT_UP &&
		BarTypes[CurrentIndex] == BT_DOWN)
	{
		// these three bars can make MSH
		if(High[CurrentIndex-2] < High[CurrentIndex-1] && 
			High[CurrentIndex] < High[CurrentIndex-1])
		{
			// this is MSH
			MSH[CurrentIndex-1] = 1;
		}
	}

	// Fill Labels array
	if(MSH[CurrentIndex-1] == 1)
	{
		// look back through Labels array to find previous label HH or LH
		int Index=CurrentIndex-1;
		for(; Index >= 0; Index--)
		{
			if(Labels[Index] == HH || Labels[Index] == LH)
				break;
		}

		// now Index store index of the element
		if(Index < 0)	// this is a first label, assume HH
			Labels[CurrentIndex-1]	= HH;
		else
		{
			// compare High values
			if(High[Index] == High[CurrentIndex-1])
				Labels[CurrentIndex-1] = Labels[Index];
			else
				if(High[Index] < High[CurrentIndex-1])
					Labels[CurrentIndex-1]	= HH;
				else
					Labels[CurrentIndex-1]	= LH;
		}
	}


	if(MSL[CurrentIndex-1] == 1)
	{
		// look back through Labels array to find previous label LL or HL
		int Index=CurrentIndex-1;
		for(; Index >= 0; Index--)
		{
			if(Labels[Index] == LL || Labels[Index] == HL)
				break;
		}

		// now Index store index of the element
		if(Index < 0)	// this is a first label, assume LL
			Labels[CurrentIndex-1]	= LL;
		else
		{
			// compare Low values
			if(Low[Index] == Low[CurrentIndex-1])
				Labels[CurrentIndex-1] = Labels[Index];
			else
				if(Low[Index] > Low[CurrentIndex-1])
					Labels[CurrentIndex-1]	= LL;
				else
					Labels[CurrentIndex-1]	= HL;
		}
	}


	// check if we need draw labels
	if(!DrawLabels.GetYesNo())
		return;

	// if pattern is not set do not add the empty label
	if(Labels[sc.Index-2] == 0)
		return;


	// Since we are using UTAM_ADD_ALWAYS, we must not attempt to draw labels when we are on the last bar, even if the labels do not appear on the last bar. Otherwise, we get the same label added again and again as the bar is updated. 
	if(sc.GetBarHasClosedStatus(sc.Index) == BHCS_BAR_HAS_NOT_CLOSED)
		return;

	s_UseTool Tool;
	Tool.Clear(); // reset tool structure for our next use
	Tool.ChartNumber	= sc.ChartNumber;
	Tool.DrawingType	= DRAWING_TEXT;
	Tool.Region			= sc.GraphRegion;
	Tool.FontFace= "Arial"; 
	Tool.FontBold= true;

	Tool.ReverseTextColor = 0;
	Tool.FontSize = Labels.LineWidth;

	Tool.LineNumber = UniqueID;
	Tool.BeginIndex =sc.Index-2;

	SCString	Label;
	SCString	Value;

	float Offset = LabelsOffset.GetFloat()*0.01f*(sc.High[sc.Index - 2]-sc.Low[sc.Index - 2]);


	switch((int)Labels[sc.Index-2])
	{
	case HH:
		{
			if(!DisplayPriceValues.GetYesNo())
			{
				Label = "HH";
			}
			else
			{
				Value = sc.FormatGraphValue(sc.High[sc.Index-2], sc.BaseGraphValueFormat);
				Label.Format("HH %s", Value.GetChars());
			}
			Tool.Text			= Label;
			Tool.Color			= Labels.PrimaryColor;
			
			Tool.BeginValue		= sc.High[sc.Index-2] + Offset;
			Tool.TextAlignment	= DT_CENTER | DT_BOTTOM;
			break;
		}

	case LH:
		{
			if(!DisplayPriceValues.GetYesNo())
			{
				Label = "LH";
			}
			else
			{
				Value = sc.FormatGraphValue(sc.High[sc.Index-2], sc.BaseGraphValueFormat);
				Label.Format("LH %s", Value.GetChars());
			}
			Tool.Text			= Label;
			Tool.Color			= Labels.PrimaryColor;

			Tool.BeginValue		= sc.High[sc.Index-2] + Offset;
			Tool.TextAlignment	= DT_CENTER | DT_BOTTOM;
			break;
		}

	case LL:
		{
			if(!DisplayPriceValues.GetYesNo())
			{
				Label = "LL";
			}
			else
			{
				Value = sc.FormatGraphValue(sc.Low[sc.Index-2], sc.BaseGraphValueFormat);
				Label.Format("LL %s", Value.GetChars());
			}
			Tool.Text			= Label;
			Tool.Color			= Labels.SecondaryColor;
			
			Tool.BeginValue		= sc.Low[sc.Index-2] - Offset;
			Tool.TextAlignment	= DT_CENTER | DT_TOP;
			break;
		}

	case HL:
		{
			if(!DisplayPriceValues.GetYesNo())
			{
				Label = "HL";
			}
			else
			{
				Value = sc.FormatGraphValue(sc.Low[sc.Index-2], sc.BaseGraphValueFormat);
				Label.Format("HL %s", Value.GetChars());
			}
			Tool.Text			= Label;
			Tool.Color			= Labels.SecondaryColor;
			
			Tool.BeginValue		= sc.Low[sc.Index-2] - Offset;
			Tool.TextAlignment	= DT_CENTER | DT_TOP;
			break;
		}

	default:
		return;
	}

	Tool.AddMethod = UTAM_ADD_ALWAYS;

	sc.UseTool(Tool);
}


/*==========================================================================*/
SCSFExport scsf_WoodiesPanel(SCStudyInterfaceRef sc)
{
	SCSubgraphRef CCIDiff = sc.Subgraph[0];
	SCSubgraphRef ProjectedBuy = sc.Subgraph[1];
	SCSubgraphRef ProjectedSell = sc.Subgraph[2];
	SCSubgraphRef CCIPred = sc.Subgraph[3];
	SCSubgraphRef BackColors = sc.Subgraph[4];
	SCSubgraphRef CCIDifferenceHigh = sc.Subgraph[5];
	SCSubgraphRef CCIDifferenceLow = sc.Subgraph[6];
	SCSubgraphRef HighPC = sc.Subgraph[7];
	SCSubgraphRef LastPC = sc.Subgraph[8];
	SCSubgraphRef LowPC = sc.Subgraph[9];
	SCSubgraphRef EmaAngle = sc.Subgraph[10];

	SCInputRef PanelRangeTopVal = sc.Input[0];
	SCInputRef PanelRangeBottomVal = sc.Input[1];
	SCInputRef ShowLabels = sc.Input[2];
	SCInputRef ColumnsNumber = sc.Input[3];
	SCInputRef CCIStudyRef = sc.Input[4];
	SCInputRef EntryOffset = sc.Input[5];
	SCInputRef UseWoodiesBackgroundColor = sc.Input[6];
	SCInputRef TradingStopTime = sc.Input[7];
	SCInputRef CCIPredictorStudyRef = sc.Input[8];
	SCInputRef ShowPreviousClose = sc.Input[9];
	SCInputRef UseSecondaryColor = sc.Input[10];
	SCInputRef SideWinderStudyRef = sc.Input[11];
	SCInputRef LeftEdgeTextOffset = sc.Input[12];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Woodies Panel by SC";

		sc.AutoLoop = 0;
		sc.GraphRegion = 1;
		sc.CalculationPrecedence = LOW_PREC_LEVEL;

		CCIDiff.Name = "CCIDiff";
		CCIDiff.DrawStyle = DRAWSTYLE_CUSTOM_TEXT;
		CCIDiff.PrimaryColor = RGB(255,255,255);
		CCIDiff.LineWidth = 10; //font size
		CCIDiff.DrawZeros = false;

		ProjectedBuy.Name = "Projected Buy";
		ProjectedBuy.DrawStyle = DRAWSTYLE_CUSTOM_TEXT;
		ProjectedBuy.PrimaryColor = RGB(184,251,197);
		ProjectedBuy.LineWidth = 10; //font size
		ProjectedBuy.DrawZeros = false;

		ProjectedSell.Name = "Projected Sell";
		ProjectedSell.DrawStyle = DRAWSTYLE_CUSTOM_TEXT;
		ProjectedSell.PrimaryColor = RGB(239,204,246);
		ProjectedSell.LineWidth = 10; //font size
		ProjectedSell.DrawZeros = false;

		CCIPred.Name = "CCI Pred. H/L";
		CCIPred.DrawStyle = DRAWSTYLE_CUSTOM_TEXT;
		CCIPred.PrimaryColor = RGB(0,0,0);
		CCIPred.LineWidth = 10; //font size
		CCIPred.DrawZeros = false;

		BackColors.Name = "Background Colors";
		BackColors.DrawStyle = DRAWSTYLE_IGNORE;
		BackColors.PrimaryColor = RGB(64,128,128);
		BackColors.SecondaryColor = RGB(45,89,89);
		BackColors.SecondaryColorUsed = 1;
		BackColors.DrawZeros = false;

		CCIDifferenceHigh.Name = "CCIDiff H";
		CCIDifferenceHigh.DrawStyle = DRAWSTYLE_CUSTOM_TEXT;
		CCIDifferenceHigh.PrimaryColor = RGB(0,255,0);
		CCIDifferenceHigh.LineWidth = 10; //font size
		CCIDifferenceHigh.DrawZeros = false;

		CCIDifferenceLow.Name = "CCIDiff L";
		CCIDifferenceLow.DrawStyle = DRAWSTYLE_CUSTOM_TEXT;
		CCIDifferenceLow.PrimaryColor = RGB(237,80,255);
		CCIDifferenceLow.LineWidth = 10; //font size
		CCIDifferenceLow.DrawZeros = false;

		HighPC.Name = "High Prev/Cur";
		HighPC.DrawStyle = DRAWSTYLE_CUSTOM_TEXT;
		HighPC.PrimaryColor = RGB(0,0,0);
		HighPC.SecondaryColor = RGB(0,255,0);
		HighPC.SecondaryColorUsed = 1;
		HighPC.LineWidth = 10; //font size
		HighPC.DrawZeros = false;

		LastPC.Name = "Last Prev/Cur";
		LastPC.DrawStyle = DRAWSTYLE_CUSTOM_TEXT;
		LastPC.PrimaryColor = RGB(0,0,0);
		LastPC.LineWidth = 14; //font size
		LastPC.DrawZeros = false;

		LowPC.Name = "Low Prev/Cur";
		LowPC.DrawStyle = DRAWSTYLE_CUSTOM_TEXT;
		LowPC.PrimaryColor = RGB(0,0,0);
		LowPC.SecondaryColor = RGB(255,0,0);
		LowPC.SecondaryColorUsed = 1;
		LowPC.LineWidth = 10; //font size
		LowPC.DrawZeros = false;

		EmaAngle.Name = "EMA Angle";
		EmaAngle.DrawStyle = DRAWSTYLE_CUSTOM_TEXT;
		EmaAngle.PrimaryColor = RGB(0,0,0);
		EmaAngle.LineWidth = 10; //font size
		EmaAngle.DrawZeros = false;

		PanelRangeTopVal.Name = "Panel Range Top Value";
		PanelRangeTopVal.SetInt(250);

		PanelRangeBottomVal.Name = "Panel Range Bottom Value";
		PanelRangeBottomVal.SetInt(-250);

		ShowLabels.Name = "Show Descriptive Labels";
		ShowLabels.SetYesNo(true);

		ColumnsNumber.Name = "Number of Fill Space Columns";
		ColumnsNumber.SetInt(10);

		CCIStudyRef.Name = "CCI Study Reference";
		CCIStudyRef.SetStudyID(1);

		EntryOffset.Name = "Projected Entry Offset in Ticks";
		EntryOffset.SetInt(1);

		UseWoodiesBackgroundColor.Name = "Use Woodies Background Color";
		UseWoodiesBackgroundColor.SetYesNo(true);

		TradingStopTime.Name = "Trading Stop Time";
		TradingStopTime.SetTime(HMS_TIME(15, 30, 0));

		CCIPredictorStudyRef.Name = "CCI Predictor Study Reference";
		CCIPredictorStudyRef.SetStudyID(1);

		ShowPreviousClose.Name = "Show Previous Close";
		ShowPreviousClose.SetYesNo(false);

		UseSecondaryColor.Name = "Use Secondary Color after Trading Stop Time";
		UseSecondaryColor.SetYesNo(true);

		SideWinderStudyRef.Name = "SideWinder Study Reference";
		SideWinderStudyRef.SetStudyID(1);

		LeftEdgeTextOffset.Name = "Left Edge Text Offset";
		LeftEdgeTextOffset.SetInt(1);
		LeftEdgeTextOffset.SetIntLimits(0,4);

		sc.UpdateAlways = 1;

		return;
	}

	if (sc.LastCallToFunction)
	{

		sc.PreserveFillSpace = 0;
		sc.NumFillSpaceBars = 0;
		sc.UseGlobalChartColors = true;
		sc.ScaleBorderColor = RGB(128,128,128);

		return;
	}

	{
		sc.PreserveFillSpace = 1;
		sc.NumFillSpaceBars = ColumnsNumber.GetInt();	
	}

	int CurrentVisibleIndex = sc.IndexOfLastVisibleBar;

	if (BackColors.PrimaryColor == RGB(255,0,0))
	{
		BackColors.PrimaryColor = RGB(64,128,128);
		BackColors.SecondaryColor = RGB(45,89,89);
	}

	if (UseWoodiesBackgroundColor.GetYesNo())
	{
		sc.UseGlobalChartColors = false;
		sc.ChartBackgroundColor = BackColors.PrimaryColor;
		sc.ScaleBorderColor = RGB(0,0,0);

		if ((sc.BaseDateTimeIn[CurrentVisibleIndex].GetTime() > TradingStopTime.GetTime())
			&& UseSecondaryColor.GetYesNo())
			sc.ChartBackgroundColor = BackColors.SecondaryColor;


	}
	else
		sc.UseGlobalChartColors = true;


	if (sc.GraphRegion >= 2)
		sc.GraphRegion = 1;



	float RangeIncrement = 0.0f;

	//Determine our value increment for the panel data lines based on what lines are visible.
	int NumberOfLines = 0;
	for(int c= 0; c<16; c++)
	{
		if(sc.Subgraph[c].DrawStyle != DRAWSTYLE_IGNORE && sc.Subgraph[c].DrawStyle != DRAWSTYLE_HIDDEN)
			NumberOfLines++;
	}

	RangeIncrement = (PanelRangeTopVal.GetInt() - PanelRangeBottomVal.GetInt())/(NumberOfLines+1.0f);

	float YValue = PanelRangeTopVal.GetInt()-RangeIncrement;

	int TextOffset = -(2+LeftEdgeTextOffset.GetInt());


	s_UseTool Tool;

	if(CCIDifferenceHigh.DrawStyle != DRAWSTYLE_IGNORE && CCIDifferenceHigh.DrawStyle != DRAWSTYLE_HIDDEN)
	{


		SCFloatArray CCIProjHigh;
		sc.GetStudyArrayUsingID(CCIPredictorStudyRef.GetStudyID(),0,CCIProjHigh);


		float CCIDifferenceHighValue = 0;
		SCFloatArray CCI;
		sc.GetStudyArrayUsingID(CCIStudyRef.GetStudyID(),0,CCI);
		CCIDifferenceHighValue = CCIProjHigh[CurrentVisibleIndex] - CCI[CurrentVisibleIndex-1];


		if (CurrentVisibleIndex != sc.ArraySize-1)
			CCIDifferenceHighValue = CCI[CurrentVisibleIndex] - CCI[CurrentVisibleIndex-1];

		//Worksheet Output
		CCIDifferenceHigh[sc.UpdateStartIndex] = 0;
		CCIDifferenceHigh[sc.ArraySize - 1] = CCIDifferenceHighValue;

		Tool.Clear(); // reset tool structure for our next use
		Tool.ChartNumber = sc.ChartNumber;
		Tool.DrawingType = DRAWING_TEXT;
		Tool.LineNumber = 5035;
		Tool.BeginDateTime = TextOffset;
		Tool.BeginValue = YValue;
		Tool.Color = CCIDifferenceHigh.PrimaryColor;
		Tool.Region = sc.GraphRegion;

		SCString Label;

		if (ShowLabels.GetYesNo())
		{
			Label = CCIDifferenceHigh.Name;
		}


		Tool.Text.Format(" %.2f %s", CCIDifferenceHighValue, Label.GetChars());
		Tool.FontSize = CCIDifferenceHigh.LineWidth;
		Tool.FontBold = true;
		//Tool.FontFace= "Courier";
		Tool.AddMethod = UTAM_ADD_OR_ADJUST;
		Tool.ReverseTextColor = 0;


		sc.UseTool(Tool);
		YValue -= RangeIncrement;
	}


	if(CCIDiff.DrawStyle != DRAWSTYLE_IGNORE && CCIDiff.DrawStyle != DRAWSTYLE_HIDDEN)
	{
		float CCIDiffVal = 0;
		SCFloatArray CCI;
		sc.GetStudyArrayUsingID(CCIStudyRef.GetStudyID(),0,CCI);
		CCIDiffVal = CCI[CurrentVisibleIndex] - CCI[CurrentVisibleIndex-1];

		CCIDiff[sc.UpdateStartIndex] = 0;
		CCIDiff[sc.ArraySize - 1] = CCIDiffVal;

		Tool.Clear(); // reset tool structure for our next use
		Tool.ChartNumber = sc.ChartNumber;
		Tool.DrawingType = DRAWING_TEXT;
		Tool.LineNumber = 5030;
		Tool.BeginDateTime = TextOffset;
		Tool.BeginValue = YValue;
		Tool.Color = CCIDiff.PrimaryColor;
		Tool.Region = sc.GraphRegion;

		SCString Label;

		if (ShowLabels.GetYesNo())
		{
			Label = "CCIDiff";
		}

		Tool.Text.Format(" %.2f %s", CCIDiffVal , Label.GetChars());
		Tool.FontSize = CCIDiff.LineWidth;
		Tool.FontBold = true;
		Tool.AddMethod = UTAM_ADD_OR_ADJUST;
		Tool.ReverseTextColor = 0;

		sc.UseTool(Tool);
		YValue -= RangeIncrement;
	}


	if(CCIDifferenceLow.DrawStyle != DRAWSTYLE_IGNORE && CCIDifferenceLow.DrawStyle != DRAWSTYLE_HIDDEN)
	{
		SCFloatArray CCIProjLow;
		sc.GetStudyArrayUsingID(CCIPredictorStudyRef.GetStudyID(),1,CCIProjLow);


		float CCIDifferenceLowValue = 0;
		SCFloatArray CCI;
		sc.GetStudyArrayUsingID(CCIStudyRef.GetStudyID(),0,CCI);
		CCIDifferenceLowValue = CCIProjLow[CurrentVisibleIndex] - CCI[CurrentVisibleIndex-1];

		if (CurrentVisibleIndex != sc.ArraySize-1)
			CCIDifferenceLowValue = CCI[CurrentVisibleIndex] - CCI[CurrentVisibleIndex-1];

		CCIDifferenceLow[sc.UpdateStartIndex] = 0;
		CCIDifferenceLow[sc.ArraySize - 1] = CCIDifferenceLowValue;

		Tool.Clear(); // reset tool structure for our next use
		Tool.ChartNumber = sc.ChartNumber;
		Tool.DrawingType = DRAWING_TEXT;
		Tool.LineNumber = 5036;
		Tool.BeginDateTime = TextOffset;
		Tool.BeginValue = YValue;
		Tool.Color = CCIDifferenceLow.PrimaryColor;
		Tool.Region = sc.GraphRegion;

		SCString Label;

		if (ShowLabels.GetYesNo())
		{
			Label = CCIDifferenceLow.Name;
		}

		Tool.Text.Format(" %.2f %s", CCIDifferenceLowValue, Label.GetChars());
		Tool.FontSize = CCIDifferenceLow.LineWidth;
		Tool.FontBold = true;
		Tool.AddMethod = UTAM_ADD_OR_ADJUST;
		Tool.ReverseTextColor = 0;

		sc.UseTool(Tool);
		YValue -= RangeIncrement;
	}

	if(HighPC.DrawStyle != DRAWSTYLE_IGNORE && HighPC.DrawStyle != DRAWSTYLE_HIDDEN)
	{	
		Tool.Clear(); // reset tool structure for our next use
		Tool.ChartNumber = sc.ChartNumber;
		Tool.DrawingType = DRAWING_TEXT;
		Tool.LineNumber = 5037;
		Tool.BeginDateTime = TextOffset;
		Tool.BeginValue = YValue;
		if (sc.High[CurrentVisibleIndex] > sc.High[CurrentVisibleIndex-1])
		{
			Tool.Color = HighPC.SecondaryColor;
			Tool.ReverseTextColor = 0;
		}
		else
		{
			Tool.Color = HighPC.PrimaryColor;
			Tool.ReverseTextColor = 0;
		}

		Tool.Region = sc.GraphRegion;

		SCString Label;

		if (ShowLabels.GetYesNo())
		{
			Label = HighPC.Name;
		}

		SCString CurrentHigh = sc.FormatGraphValue(sc.High[CurrentVisibleIndex], sc.BaseGraphValueFormat);
		SCString PrevHigh = sc.FormatGraphValue(sc.High[CurrentVisibleIndex-1], sc.BaseGraphValueFormat);
		Tool.Text.Format(" %s %s %s", PrevHigh.GetChars(), CurrentHigh.GetChars(), Label.GetChars());
		Tool.FontSize = HighPC.LineWidth;
		Tool.FontBold = true;
		Tool.AddMethod = UTAM_ADD_OR_ADJUST;


		sc.UseTool(Tool);
		YValue -= RangeIncrement;
		//Worksheet Output
		HighPC[sc.UpdateStartIndex] = 0;
		HighPC[sc.UpdateStartIndex-1] = 0;
		HighPC[sc.ArraySize - 1] = sc.High[CurrentVisibleIndex];
		HighPC[sc.ArraySize - 2] = sc.High[CurrentVisibleIndex-1];
	}

	if(LastPC.DrawStyle != DRAWSTYLE_IGNORE && LastPC.DrawStyle != DRAWSTYLE_HIDDEN)
	{	
		Tool.Clear(); // reset tool structure for our next use
		Tool.ChartNumber = sc.ChartNumber;
		Tool.DrawingType = DRAWING_TEXT;
		Tool.LineNumber = 5038;
		Tool.BeginDateTime = TextOffset;
		Tool.BeginValue = YValue;


		{
			Tool.Color = LastPC.PrimaryColor;
			Tool.ReverseTextColor = 0;
		}


		Tool.Region = sc.GraphRegion;

		SCString Label;

		if (ShowLabels.GetYesNo())
		{
			if(ShowPreviousClose.GetYesNo())
				Label = LastPC.Name;
			else
				Label = "Last Price";
		}

		SCString CurLast = sc.FormatGraphValue(sc.Close[CurrentVisibleIndex], sc.BaseGraphValueFormat);
		SCString PrevLast = sc.FormatGraphValue(sc.Close[CurrentVisibleIndex-1], sc.BaseGraphValueFormat);

		//Worksheet Output
		LastPC[sc.UpdateStartIndex] = 0;
		LastPC[sc.UpdateStartIndex-1] = 0;
		LastPC[sc.ArraySize - 1] = sc.Close[CurrentVisibleIndex];
		LastPC[sc.ArraySize - 2] = sc.Close[CurrentVisibleIndex-1];

		if (ShowPreviousClose.GetYesNo())
		{
			Tool.Text.Format(" %s %s %s", PrevLast.GetChars(), CurLast.GetChars(), Label.GetChars());
		}
		else
		{
			Tool.Text.Format(" %s %s", CurLast.GetChars(), Label.GetChars());
		}

		Tool.FontSize = LastPC.LineWidth;
		Tool.FontBold = true;
		Tool.AddMethod = UTAM_ADD_OR_ADJUST;



		sc.UseTool(Tool);
		YValue -= RangeIncrement;
	}

	if(LowPC.DrawStyle != DRAWSTYLE_IGNORE && LowPC.DrawStyle != DRAWSTYLE_HIDDEN)
	{	
		Tool.Clear(); // reset tool structure for our next use
		Tool.ChartNumber = sc.ChartNumber;
		Tool.DrawingType = DRAWING_TEXT;
		Tool.LineNumber = 5039;
		Tool.BeginDateTime = TextOffset;
		Tool.BeginValue = YValue;
		if (sc.Low[CurrentVisibleIndex] < sc.Low[CurrentVisibleIndex-1])
		{
			Tool.Color = LowPC.SecondaryColor;
			Tool.ReverseTextColor = 0;
		}
		else
		{
			Tool.Color = LowPC.PrimaryColor;
			Tool.ReverseTextColor = 0;
		}

		Tool.Region = sc.GraphRegion;

		SCString Label;

		if (ShowLabels.GetYesNo())
		{
			Label = LowPC.Name;
		}

		SCString CurLow = sc.FormatGraphValue(sc.Low[CurrentVisibleIndex], sc.BaseGraphValueFormat);
		SCString PrevLow = sc.FormatGraphValue(sc.Low[CurrentVisibleIndex-1], sc.BaseGraphValueFormat);

		//Worksheet Output
		LowPC[sc.UpdateStartIndex] = 0;
		LowPC[sc.UpdateStartIndex-1] = 0;
		LowPC[sc.ArraySize - 1] = sc.Low[CurrentVisibleIndex];
		LowPC[sc.ArraySize - 2] = sc.Low[CurrentVisibleIndex-1];

		Tool.Text.Format(" %s %s %s",PrevLow.GetChars(),CurLow.GetChars(),Label.GetChars());
		Tool.FontSize = LowPC.LineWidth;
		Tool.FontBold = true;
		Tool.AddMethod = UTAM_ADD_OR_ADJUST;


		sc.UseTool(Tool);
		YValue -= RangeIncrement;
	}

	if(ProjectedBuy.DrawStyle != DRAWSTYLE_IGNORE && ProjectedBuy.DrawStyle != DRAWSTYLE_HIDDEN)
	{
		float ProjHigh = 0;

		ProjHigh = sc.Low[CurrentVisibleIndex]+sc.RangeBarValue+(EntryOffset.GetInt()*sc.TickSize);

		//Worksheet Output
		ProjectedBuy[sc.UpdateStartIndex] = 0;
		ProjectedBuy[sc.ArraySize - 1] = ProjHigh;

		Tool.Clear(); // reset tool structure for our next use
		Tool.ChartNumber = sc.ChartNumber;
		Tool.DrawingType = DRAWING_TEXT;
		Tool.LineNumber = 5031;
		Tool.BeginDateTime = TextOffset;
		Tool.BeginValue = YValue;
		Tool.Color = ProjectedBuy.PrimaryColor;
		Tool.Region = sc.GraphRegion;

		SCString Label;

		if (ShowLabels.GetYesNo())
		{
			Label = "ProjHigh";
		}

		SCString StrValue = sc.FormatGraphValue(ProjHigh, sc.BaseGraphValueFormat);
		Tool.Text.Format(" %s %s", StrValue.GetChars(), Label.GetChars());
		Tool.FontSize = ProjectedBuy.LineWidth;
		Tool.FontBold = true;
		Tool.AddMethod = UTAM_ADD_OR_ADJUST;
		Tool.ReverseTextColor = 0;

		sc.UseTool(Tool);
		YValue -= RangeIncrement;

	}

	if(ProjectedSell.DrawStyle != DRAWSTYLE_IGNORE && ProjectedSell.DrawStyle != DRAWSTYLE_HIDDEN)
	{
		float ProjLow = 0;

		ProjLow = sc.High[CurrentVisibleIndex]-(sc.RangeBarValue+(EntryOffset.GetInt()*sc.TickSize));

		//Worksheet Output
		ProjectedSell[sc.UpdateStartIndex] = 0;
		ProjectedSell[sc.ArraySize - 1] = ProjLow;


		Tool.Clear(); // reset tool structure for our next use
		Tool.ChartNumber = sc.ChartNumber;
		Tool.DrawingType = DRAWING_TEXT;
		Tool.LineNumber = 5032;
		Tool.BeginDateTime = TextOffset;
		Tool.BeginValue = YValue;
		Tool.Color = ProjectedSell.PrimaryColor;
		Tool.Region = sc.GraphRegion;

		SCString Label;

		if (ShowLabels.GetYesNo())
		{
			Label = "ProjLow";
		}

		SCString StrValue = sc.FormatGraphValue(ProjLow, sc.BaseGraphValueFormat);
		Tool.Text.Format(" %s %s", StrValue.GetChars(), Label.GetChars());
		Tool.FontSize = ProjectedSell.LineWidth;
		Tool.FontBold = true;
		Tool.AddMethod = UTAM_ADD_OR_ADJUST;
		Tool.ReverseTextColor = 0;

		sc.UseTool(Tool);
		YValue -= RangeIncrement;
	}

	if(CCIPred.DrawStyle != DRAWSTYLE_IGNORE && CCIPred.DrawStyle != DRAWSTYLE_HIDDEN)
	{

		SCFloatArray CCIProjHigh;
		SCFloatArray CCIProjLow;

		sc.GetStudyArrayUsingID(CCIPredictorStudyRef.GetStudyID(),0,CCIProjHigh);
		sc.GetStudyArrayUsingID(CCIPredictorStudyRef.GetStudyID(),1,CCIProjLow);



		Tool.Clear(); // reset tool structure for our next use
		Tool.ChartNumber = sc.ChartNumber;
		Tool.DrawingType = DRAWING_TEXT;
		Tool.LineNumber = 5033;
		Tool.BeginDateTime = TextOffset;
		Tool.BeginValue = YValue;
		Tool.Color = CCIPred.PrimaryColor;
		Tool.Region = sc.GraphRegion;

		SCString Label;

		if (ShowLabels.GetYesNo())
		{
			Label = CCIPred.Name;
		}

		float ProjHigh = CCIProjHigh[sc.ArraySize-1];
		float ProjLow = CCIProjLow[sc.ArraySize-1];

		if (CurrentVisibleIndex != sc.ArraySize-1)
		{
			SCFloatArray CCI;
			sc.GetStudyArrayUsingID(CCIStudyRef.GetStudyID(),0,CCI);
			ProjLow = ProjHigh = CCI[CurrentVisibleIndex];
		}

		Tool.Text.Format(" %.1f  %.1f %s",ProjHigh,ProjLow,Label.GetChars());
		Tool.FontSize = CCIPred.LineWidth;
		Tool.FontBold = true;
		Tool.AddMethod = UTAM_ADD_OR_ADJUST;
		Tool.ReverseTextColor = 0;

		sc.UseTool(Tool);
		YValue -= RangeIncrement;

		//Worksheet Output
		CCIPred[sc.UpdateStartIndex] = 0;
		CCIPred[sc.UpdateStartIndex-1] = 0;
		CCIPred[sc.ArraySize - 1] = ProjHigh;
		CCIPred[sc.ArraySize - 2] = ProjLow;
	}



	if(LowPC.DrawStyle != DRAWSTYLE_IGNORE && LowPC.DrawStyle != DRAWSTYLE_HIDDEN)
	{
		SCFloatArray EMAAngle;
		sc.GetStudyArrayUsingID(SideWinderStudyRef.GetStudyID(),7,EMAAngle);


		Tool.Clear(); // reset tool structure for our next use
		Tool.ChartNumber = sc.ChartNumber;
		Tool.DrawingType = DRAWING_TEXT;
		Tool.LineNumber = 5040;
		Tool.BeginDateTime = TextOffset;
		Tool.BeginValue = YValue;
		Tool.Color = LowPC.PrimaryColor;
		Tool.Region = sc.GraphRegion;

		SCString Label;

		if (ShowLabels.GetYesNo())
		{
			Label = LowPC.Name;
		}


		Tool.Text.Format(" %.1f\u00B0 %s", EMAAngle[CurrentVisibleIndex], Label.GetChars());
		Tool.FontSize = LowPC.LineWidth;
		Tool.FontBold = true;
		Tool.AddMethod = UTAM_ADD_OR_ADJUST;
		Tool.ReverseTextColor = 0;

		sc.UseTool(Tool);
		YValue -= RangeIncrement;

		//Worksheet Output
		LowPC[sc.UpdateStartIndex] = 0;
		LowPC[sc.ArraySize - 1] = EMAAngle[CurrentVisibleIndex];

	}

}

/*==========================================================================*/
void AccumulateZigZagVolume(SCStudyInterfaceRef sc, SCSubgraphRef AccumulatedVolume, SCFloatArrayRef ZigZagPeakType, int VolumeTypeToAccumulate) 
{
	//Accumulate the volume across the last two zigzags

	//Determine starting point
	int Count = 0;//To count the number of zigzag lines
	int StartingIndex = sc.UpdateStartIndex+1;
	while (StartingIndex > 0 && Count < 2)
	{
		StartingIndex--;
		if (ZigZagPeakType[StartingIndex] != 0.0f)
			Count ++;
	}

	double Volume = 0;
	int Direction = 0;

	for (int Index = StartingIndex+1; Index < sc.ArraySize; Index++)
	{
		if(ZigZagPeakType[Index-1] != 0)
		{
			Volume = 0;
			Direction = (int)ZigZagPeakType[Index-1];
		}

		if (Index == 1) // First index, pick up initial volume
		{
			if (VolumeTypeToAccumulate == 0)
				Volume = sc.Volume[0];
			else if (VolumeTypeToAccumulate == 1)
				Volume = sc.BidVolume[0];
			else if (VolumeTypeToAccumulate == 2)
				Volume = sc.AskVolume[0];
			else
				Volume = sc.AskVolume[0] - sc.BidVolume[0];
			AccumulatedVolume[0] = (float)Volume;
			Direction = (int)ZigZagPeakType[0];
		}

		if (Direction == 1)
			AccumulatedVolume.DataColor[Index]= AccumulatedVolume.PrimaryColor;
		else if (Direction == -1)
			AccumulatedVolume.DataColor[Index]= AccumulatedVolume.SecondaryColor;

		if (VolumeTypeToAccumulate == 0)
			Volume += sc.Volume[Index];
		else if (VolumeTypeToAccumulate == 1)
			Volume += sc.BidVolume[Index];
		else if (VolumeTypeToAccumulate == 2)
			Volume += sc.AskVolume[Index];
		else
			Volume += sc.AskVolume[Index] - sc.BidVolume[Index];

		AccumulatedVolume[Index] = (float)Volume;
	}
}

/*==========================================================================*/
SCSFExport scsf_ZigZagStudy(SCStudyInterfaceRef sc)
{
	SCSubgraphRef ZigZagLine        = sc.Subgraph[0];
	SCSubgraphRef TextLabels        = sc.Subgraph[1];
	SCSubgraphRef ReversalPrice     = sc.Subgraph[2];
	SCSubgraphRef ZigzagLength      = sc.Subgraph[3];
	SCSubgraphRef AccumulatedVolume = sc.Subgraph[4];
	SCSubgraphRef ZigzagNumBars     = sc.Subgraph[5];
	SCSubgraphRef ZigzagMidPoint    = sc.Subgraph[6];

	SCInputRef VolumeToAccumulate = sc.Input[0];
	SCInputRef LabelOffsetType = sc.Input[1];
	SCInputRef FormatVolumeWithLargeNumberSuffix = sc.Input[2];
	SCInputRef ColorZigZagLine = sc.Input[3];
	SCInputRef VersionUpdate = sc.Input[14];
	SCInputRef InputDataHigh = sc.Input[15];
	SCInputRef InputDataLow = sc.Input[16];
	SCInputRef CalculationMode = sc.Input[17];
	SCInputRef ReversalPercent = sc.Input[18];
	SCInputRef ReversalAmount = sc.Input[19];
	SCInputRef NumberBarsForReversal = sc.Input[20];
	SCInputRef ShowRevPrice = sc.Input[21];
	SCInputRef DisplayHHHLLLLHLabels = sc.Input[22];
	SCInputRef LabelsOffset = sc.Input[23];
	SCInputRef AdditionalOutputForSpreadsheets = sc.Input[24];
	SCInputRef ShowTime = sc.Input[25];
	SCInputRef ShowLength = sc.Input[26];
	SCInputRef CalcOnBarClose = sc.Input[27];
	SCInputRef CalculateZigZagVolume_UpgradeOnly = sc.Input[28];
	SCInputRef ShowZigZagVolume = sc.Input[29];
	SCInputRef ShowZigZagDuration = sc.Input[30];
	SCInputRef UseMultiLineLabels = sc.Input[31];
	SCInputRef OmitLabelPrefixes = sc.Input[32];
	SCInputRef ShowZigZagNumBars = sc.Input[33];
	SCInputRef ResetAtStartOfSession = sc.Input[34];

	if(sc.SetDefaults)
	{
		sc.GraphName="Zig Zag";
		sc.StudyDescription="Zig Zag";

		//Manual looping
		sc.AutoLoop = 0;

		sc.GraphRegion = 0;

		ZigZagLine.Name = "Zig Zag";
		ZigZagLine.DrawStyle = DRAWSTYLE_LINE;
		ZigZagLine.LineStyle = LINESTYLE_DOT;
		ZigZagLine.PrimaryColor = RGB(255,0,255);
		ZigZagLine.SecondaryColorUsed = 1;
		ZigZagLine.SecondaryColor = RGB(255,0,0);
		ZigZagLine.LineWidth = 3;
		ZigZagLine.DrawZeros = false;

		TextLabels.Name = "Text Labels";
		TextLabels.SecondaryColorUsed = 1;
		TextLabels.PrimaryColor = RGB(0,255,0);
		TextLabels.SecondaryColor = RGB(255,0,0);
		TextLabels.LineWidth = 10;
		TextLabels.DrawStyle = DRAWSTYLE_CUSTOM_TEXT;
		
		ReversalPrice.DrawStyle = DRAWSTYLE_IGNORE;
		ZigzagLength.DrawStyle = DRAWSTYLE_IGNORE;
		ZigzagNumBars.DrawStyle = DRAWSTYLE_IGNORE;
		AccumulatedVolume.DrawStyle = DRAWSTYLE_IGNORE;
		AccumulatedVolume.SecondaryColorUsed = 1;

		ZigzagMidPoint.Name = "Zig Zag Mid-Point";
		ZigzagMidPoint.DrawStyle = DRAWSTYLE_IGNORE;
		ZigzagMidPoint.PrimaryColor = RGB(128,128,255);
		ZigzagMidPoint.LineWidth = 1;
		ZigzagMidPoint.DrawZeros = false;

		VersionUpdate.SetInt(3);

		unsigned short DisplayOrder = 1;

		InputDataHigh.Name = "Input Data for High";
		InputDataHigh.SetInputDataIndex(SC_HIGH);
		InputDataHigh.DisplayOrder = DisplayOrder++;

		InputDataLow.Name = "Input Data for Low";
		InputDataLow.SetInputDataIndex(SC_LOW);
		InputDataLow.DisplayOrder = DisplayOrder++;

		CalculationMode.Name = "Calculation Mode (1,2,3)";
		CalculationMode.SetInt(1);
		CalculationMode.SetIntLimits(1, 3);
		CalculationMode.DisplayOrder = DisplayOrder++;

		ReversalPercent.Name = "Reversal % for Calculation Mode 1";
		ReversalPercent.SetFloat(.5f);
		ReversalPercent.DisplayOrder = DisplayOrder++;

		ReversalAmount.Name = "Reversal Amount for Calculation Mode 2,3";
		ReversalAmount.SetFloat(0.01f);
		ReversalAmount.DisplayOrder = DisplayOrder++;

		NumberBarsForReversal.Name = "Number of Bars Required for Reversal (Calculation Mode 2)";
		NumberBarsForReversal.SetInt(5);
		NumberBarsForReversal.DisplayOrder = DisplayOrder++;

		VolumeToAccumulate.Name = "Volume To Accumulate";
		VolumeToAccumulate.SetCustomInputStrings("None;Total Volume;Bid Volume;Ask Volume;Ask Bid Volume Difference");
		VolumeToAccumulate.SetCustomInputIndex(0);
		VolumeToAccumulate.DisplayOrder = DisplayOrder++;

		ResetAtStartOfSession.Name = "Reset ZigZag At Start Of Trading Day";
		ResetAtStartOfSession.SetYesNo(0);
		ResetAtStartOfSession.DisplayOrder = DisplayOrder++;

		CalcOnBarClose.Name = "Calculate New Values On Bar Close";
		CalcOnBarClose.SetYesNo(0);
		CalcOnBarClose.DisplayOrder = DisplayOrder++;

		AdditionalOutputForSpreadsheets.Name = "Additional Output for Spreadsheets";
		AdditionalOutputForSpreadsheets.SetYesNo(false);
		AdditionalOutputForSpreadsheets.DisplayOrder = DisplayOrder++;

		DisplayHHHLLLLHLabels.Name = "Display HH,HL,LL,LH Labels";
		DisplayHHHLLLLHLabels.SetYesNo(0);
		DisplayHHHLLLLHLabels.DisplayOrder = DisplayOrder++;

		ShowRevPrice.Name = "Display Reversal Price";
		ShowRevPrice.SetYesNo(0);
		ShowRevPrice.DisplayOrder = DisplayOrder++;

		ShowLength.Name = "Display Length of Zig Zag Line";
		ShowLength.SetCustomInputStrings("None;Points;Ticks");
		ShowLength.SetCustomInputIndex(0);
		ShowLength.DisplayOrder = DisplayOrder++;

		ShowZigZagVolume.Name = "Display ZigZag Volume";
		ShowZigZagVolume.SetYesNo(0);
		ShowZigZagVolume.DisplayOrder = DisplayOrder++;

		FormatVolumeWithLargeNumberSuffix.Name = "Format Volume Using Large Number Suffix (K/M)";
		FormatVolumeWithLargeNumberSuffix.SetYesNo(0);
		FormatVolumeWithLargeNumberSuffix.DisplayOrder = DisplayOrder++;

		ShowTime.Name = "Display Time Labels";
		ShowTime.SetYesNo(0);
		ShowTime.DisplayOrder = DisplayOrder++;

		ShowZigZagDuration.Name = "Display ZigZag Time Duration";
		ShowZigZagDuration.SetYesNo(0);
		ShowZigZagDuration.DisplayOrder = DisplayOrder++;

		ShowZigZagNumBars.Name = "Display ZigZag Number Of Bars";
		ShowZigZagNumBars.SetYesNo(0);
		ShowZigZagNumBars.DisplayOrder = DisplayOrder++;

		UseMultiLineLabels.Name = "Use Multi-line Labels";
		UseMultiLineLabels.SetYesNo(0);
		UseMultiLineLabels.DisplayOrder = DisplayOrder++;

		OmitLabelPrefixes.Name = "Omit Label Prefixes";
		OmitLabelPrefixes.SetYesNo(0);
		OmitLabelPrefixes.DisplayOrder = DisplayOrder++;

		LabelOffsetType.Name = "Text Label Offset Specified As";
		LabelOffsetType.SetCustomInputStrings("Percentage Of Bar;Tick Offset");
		LabelOffsetType.SetCustomInputIndex(1);
		LabelOffsetType.DisplayOrder = DisplayOrder++;

		LabelsOffset.Name = "Text Label Offset";
		LabelsOffset.SetFloat(1.0f);
		LabelsOffset.DisplayOrder = DisplayOrder++;

		ColorZigZagLine.Name = "Color ZigZag Line Using";
		ColorZigZagLine.SetCustomInputStrings("None;Slope;Trend Confirmed Volume;Confirmed Volume");
		ColorZigZagLine.SetCustomInputIndex(0);
		ColorZigZagLine.DisplayOrder = DisplayOrder++;

		return;
	}

	SCFloatArrayRef ZigZagPeakType  = ZigZagLine.Arrays[0];  // ZigZagPeakType : +1=high peak, -1=low peak, 0=not peak
	SCFloatArrayRef ZigZagPeakIndex = ZigZagLine.Arrays[1];  // ZigZagPeakIndex: index of current peak, -1=no peak yet
	SCFloatArrayRef ZigZagReset     = ZigZagLine.Arrays[2];  // ZigZagReset: 0=no reset, 1=reset
	SCFloatArrayRef ZigZagTrend     = ZigZagLine.Arrays[3];  // ZigZagTrend: 0=none, 1=confirmed up, -1=confirmed down

	int &LabelCount               = sc.PersistVars->Integers[1]; 
	int &PriorLastZZEndpointIndex = sc.PersistVars->Integers[2];
	int &ZigZagStartIndex         = sc.PersistVars->Integers[3];

	if(sc.UpdateStartIndex == 0)
	{
		LabelCount = 0;
		PriorLastZZEndpointIndex = -1;
		ZigZagStartIndex = 0;
		ZigZagReset[0] = 1.0f;

		if (VersionUpdate.GetInt() < 2)
		{
			ResetAtStartOfSession.SetYesNo(0);
			VersionUpdate.SetInt(2);
		}
		
		if (VersionUpdate.GetInt() < 3)
		{
			LabelOffsetType.SetCustomInputIndex(0);
			FormatVolumeWithLargeNumberSuffix.SetYesNo(0);
			ZigZagLine.SecondaryColorUsed = 1;
			ZigZagLine.SecondaryColor = RGB(255,0,0);
			ColorZigZagLine.SetCustomInputIndex(0);
			VolumeToAccumulate.SetCustomInputIndex(VolumeToAccumulate.GetIndex()+1);
			if (CalculateZigZagVolume_UpgradeOnly.GetYesNo() == 0)
				VolumeToAccumulate.SetCustomInputIndex(0);
			VersionUpdate.SetInt(3);
		}

		if (AdditionalOutputForSpreadsheets.GetYesNo())
		{
			ReversalPrice.Name = "Reversal Price";
			ZigzagLength.Name = "Zig Zag Line Length";
			ZigzagNumBars.Name = "Zig Zag Num Bars";
		}
		else
		{
			ReversalPrice.Name = "";
			ZigzagLength.Name = "";
			ZigzagNumBars.Name = "";
		}

		if(VolumeToAccumulate.GetIndex() != 0)
			AccumulatedVolume.Name= "AccumulatedVolume";
		else
			AccumulatedVolume.Name= "";
	}
	
	//Unique ID is used for chart drawings.  This is a random large number and is multiplied by the graph instance ID for an instance of the study, so that if there are multiple instances of the study on the chart, they will not conflict with each other.
	int UniqueID = 11062009 * sc.StudyGraphInstanceID;

	bool ShowLabelPrefixes = OmitLabelPrefixes.GetYesNo() == 0;

	SCDateTime NextSessionStart = 0;
	if (sc.UpdateStartIndex > 0)
		NextSessionStart = sc.GetTradingDayStartDateTimeOfBar(sc.BaseDateTimeIn[sc.UpdateStartIndex - 1]) + 1*DAYS;

	// Standard Manual Looping loop
	for (int Index = sc.UpdateStartIndex; Index < sc.ArraySize; Index++)
	{
		int CalcIndex = Index;

		if (CalcOnBarClose.GetYesNo())
			CalcIndex--;

		if (CalcIndex < 0)
			continue;

		if (ResetAtStartOfSession.GetYesNo() != 0)
		{
			SCDateTime IndexDateTime = sc.BaseDateTimeIn[CalcIndex];
		
			if (IndexDateTime >= NextSessionStart)
			{
				ZigZagStartIndex = CalcIndex;
				ZigZagReset[CalcIndex] = 1.0f;
				NextSessionStart = sc.GetTradingDayStartDateTimeOfBar(IndexDateTime) + 1*DAYS;
			}
		}

		//Calculate and set the zigzag lines
		switch (CalculationMode.GetInt())
		{
		case 1:
			sc.ZigZag(
				sc.BaseData[InputDataHigh.GetInputDataIndex()],
				sc.BaseData[InputDataLow.GetInputDataIndex()],
				ZigZagLine,
				CalcIndex,
				ReversalPercent.GetFloat() * 0.01f,
				ZigZagStartIndex);
			break;
		case 2:
			sc.ZigZag2(
				sc.BaseData[InputDataHigh.GetInputDataIndex()],
				sc.BaseData[InputDataLow.GetInputDataIndex()],
				ZigZagLine,
				CalcIndex,
				NumberBarsForReversal.GetInt(),
				ReversalAmount.GetFloat(),
				ZigZagStartIndex);
			break;
		case 3:
			sc.ZigZag(
				sc.BaseData[InputDataHigh.GetInputDataIndex()],
				sc.BaseData[InputDataLow.GetInputDataIndex()],
				ZigZagLine,
				CalcIndex,
				0.0f,
				ReversalAmount.GetFloat(),
				ZigZagStartIndex);
			break;
		}
	}
	
	int VolumeTypeToAccumulate = VolumeToAccumulate.GetIndex();

	if (VolumeTypeToAccumulate != 0)
		AccumulateZigZagVolume(sc, AccumulatedVolume, ZigZagPeakType, VolumeTypeToAccumulate - 1); // adjust for None index

	if (PriorLastZZEndpointIndex != -1)
	{
		for (int ZeroIndex = PriorLastZZEndpointIndex; ZeroIndex < sc.ArraySize; ZeroIndex++)
		{
			TextLabels[ZeroIndex] = 0;
			ReversalPrice[ZeroIndex] = 0;
			ZigzagLength[ZeroIndex] = 0;
			ZigzagNumBars[ZeroIndex] = 0;
		}
	}

	if (PriorLastZZEndpointIndex == -1)
		PriorLastZZEndpointIndex = 0;

	// Get the index of the last endpoint
	int IndexOfLastEndPoint;
	if (CalcOnBarClose.GetYesNo() && sc.ArraySize > 1)
		IndexOfLastEndPoint = (int)ZigZagPeakIndex[sc.ArraySize-2];
	else
		IndexOfLastEndPoint = (int)ZigZagPeakIndex[sc.ArraySize-1];

	for (int ZigZagIndex = PriorLastZZEndpointIndex; ZigZagIndex < sc.ArraySize; ZigZagIndex++)
	{
		if (ZigZagLine[ZigZagIndex] != 0 && ColorZigZagLine.GetIndex() != 0)
		{
			if (ColorZigZagLine.GetIndex() == 1 && ZigZagIndex > 0)
			{
				if (ZigZagLine[ZigZagIndex] > ZigZagLine[ZigZagIndex-1])
					ZigZagLine.DataColor[ZigZagIndex] = ZigZagLine.PrimaryColor;
				else
					ZigZagLine.DataColor[ZigZagIndex] = ZigZagLine.SecondaryColor;
			}
			else if (ColorZigZagLine.GetIndex() == 2 || ColorZigZagLine.GetIndex() == 3)
			{
				if (ZigZagTrend[ZigZagIndex] > 0)
					ZigZagLine.DataColor[ZigZagIndex] = ZigZagLine.PrimaryColor;
				else if (ZigZagTrend[ZigZagIndex] < 0)
					ZigZagLine.DataColor[ZigZagIndex] = ZigZagLine.SecondaryColor;
			}
		}

		if (ZigZagPeakType[ZigZagIndex] == 0.0f) //not a peak, nothing to be drawn
			continue;

		if (ZigZagReset[ZigZagIndex] != 0)
		{
			LabelCount++;
			continue;
		}

		int PrevEndPointIndex = -1;
		if (ZigZagIndex > 0)
			PrevEndPointIndex = (int)ZigZagPeakIndex[ZigZagIndex-1];

		if (PrevEndPointIndex >= 0)
		{
			float MidPoint = (ZigZagLine[ZigZagIndex] + ZigZagLine[PrevEndPointIndex]) / 2.0f;
			if (ZigzagMidPoint[ZigZagIndex] != MidPoint)
			{
				for (int UpdateIndex=PrevEndPointIndex+1; UpdateIndex<=ZigZagIndex; UpdateIndex++)
					ZigzagMidPoint[UpdateIndex] = MidPoint;
			}
		}

		if (ColorZigZagLine.GetIndex() == 2 || ColorZigZagLine.GetIndex() == 3) // determine zz trend color with confirmed volume
		{
			if (PrevEndPointIndex >= 0)
			{
				float Trend = ZigZagPeakType[ZigZagIndex];  // default trend is current direction

				if (PrevEndPointIndex > 0 && ZigZagReset[PrevEndPointIndex] == 0)	// have more than one swing
				{
					if (((ColorZigZagLine.GetIndex() == 2 && Trend != ZigZagTrend[PrevEndPointIndex]) || // use current trend and volume
						  ColorZigZagLine.GetIndex() == 3)  // only use volume
						&& AccumulatedVolume[ZigZagIndex] < AccumulatedVolume[PrevEndPointIndex])
						Trend = -Trend;
				}

				if (ZigZagTrend[ZigZagIndex] != Trend)
				{
					for (int UpdateIndex=PrevEndPointIndex+1; UpdateIndex<=ZigZagIndex; UpdateIndex++)
					{
						ZigZagTrend[UpdateIndex] = Trend;

						if (Trend > 0)
							ZigZagLine.DataColor[UpdateIndex] = ZigZagLine.PrimaryColor;
						else
							ZigZagLine.DataColor[UpdateIndex] = ZigZagLine.SecondaryColor;
					}
				}
			}
		}

		// at this point we have a label to draw
		PriorLastZZEndpointIndex = ZigZagIndex;

		//If not the last endpoint
		if (ZigZagIndex != IndexOfLastEndPoint || sc.UpdateStartIndex == 0)
			LabelCount++;

		int VerticalTextAlignment = ZigZagPeakType[ZigZagIndex] == 1.0 ? DT_BOTTOM : DT_TOP;
		int LabelID = UniqueID + LabelCount;

		double ZigzagLineLength = 0;
		int CurrentZigZagNumBars = 0;
		SCString BarLabelText;

		if (ZigZagReset[ZigZagIndex] == 0)
		{
			int BackRefIndex = (int)ZigZagPeakIndex[ZigZagIndex-1];

			if (ZigZagPeakType[BackRefIndex] == -1 * ZigZagPeakType[ZigZagIndex])
			{
				if (ShowRevPrice.GetYesNo())
				{
					SCString ReversalPrice;
					ReversalPrice = sc.FormatGraphValue(ZigZagLine[ZigZagIndex], sc.GetValueFormat());
					if (UseMultiLineLabels.GetYesNo())
						ReversalPrice += "\n";
					else
						ReversalPrice += " ";
					BarLabelText += ReversalPrice;
				}

				ZigzagLineLength = ZigZagLine[ZigZagIndex] - ZigZagLine[BackRefIndex];
				if (ShowLength.GetIndex() != 0)
				{
					SCString LengthStr;
					if (ShowLabelPrefixes)
						LengthStr = "L:";

					if (ShowLength.GetIndex() == 1)
						LengthStr += sc.FormatGraphValue(ZigzagLineLength, sc.GetValueFormat());
					else
					{
						double Ticks = sc.TickSize == 0 ? 0 : ZigzagLineLength / sc.TickSize;
						LengthStr += sc.FormatGraphValue(Ticks, 0);
						LengthStr += "T";
					}

					if (UseMultiLineLabels.GetYesNo())
						LengthStr += "\n";
					else
						LengthStr += " ";

					BarLabelText += LengthStr;
				}

 				if (ShowTime.GetYesNo())
 				{
 					SCDateTime DT = sc.BaseDateTimeIn[ZigZagIndex];
					SCString TimeStr;
					TimeStr.Format("%i:%02i:%02i", DT.GetHour(), DT.GetMinute(), DT.GetSecond());
					if (UseMultiLineLabels.GetYesNo())
						TimeStr += "\n";
					else
						TimeStr += " ";

					BarLabelText += TimeStr;
 				}

				if (ShowZigZagVolume.GetYesNo())
				{
					SCString LengthStr;
					if (ShowLabelPrefixes)
						LengthStr = "V:";
					LengthStr += sc.FormatVolumeValue((__int64)AccumulatedVolume[ZigZagIndex], 0, FormatVolumeWithLargeNumberSuffix.GetYesNo() != 0);

					if (UseMultiLineLabels.GetYesNo())
						LengthStr += "\n";
					else
						LengthStr += " ";

					BarLabelText += LengthStr;
				}

				if (ShowZigZagDuration.GetYesNo())
				{
					double ZigZagTimeSpan = 0;
					if (sc.ChartDataType == INTRADAY_DATA)
					{
						if (ZigZagIndex < sc.ArraySize - 1)
							ZigZagTimeSpan =  sc.BaseDateTimeIn[ZigZagIndex+1] - sc.BaseDateTimeIn[BackRefIndex];
						else
							ZigZagTimeSpan = sc.LatestDateTimeForLastBar - sc.BaseDateTimeIn[BackRefIndex];
					}
					else
					{
						ZigZagTimeSpan =  sc.BaseDateTimeIn[ZigZagIndex] - sc.BaseDateTimeIn[BackRefIndex];
					}

					SCString TimeDurationStr;
					if (sc.ChartDataType == INTRADAY_DATA)
					{
						TimeDurationStr = sc.FormatGraphValue(ZigZagTimeSpan, 20);

						if (TimeDurationStr.GetLength() >= 3 && TimeDurationStr[0] == '0' && TimeDurationStr[1] == '0' && TimeDurationStr[2] == ':')
							TimeDurationStr = TimeDurationStr.GetSubString(TimeDurationStr.GetLength()-3, 3);
					}

					SCString LengthStr;
					if (ShowLabelPrefixes)
						LengthStr = "D:";
					if (ZigZagTimeSpan >= 1.0)
					{
						SCString DaysStr;
						if (sc.ChartDataType == INTRADAY_DATA)
							DaysStr.Format("%iD ", (int)ZigZagTimeSpan);
						else
							DaysStr.Format("%i ", (int)ZigZagTimeSpan);

						LengthStr += DaysStr;
					}
					LengthStr += TimeDurationStr;
					if (UseMultiLineLabels.GetYesNo())
						LengthStr += "\n";
					else
						LengthStr += " ";

					BarLabelText += LengthStr;
				}

				CurrentZigZagNumBars = ZigZagIndex - BackRefIndex;
				if (ShowZigZagNumBars.GetYesNo())
				{
					SCString LengthStr;
					if (ShowLabelPrefixes)
						LengthStr = "B:";

					SCString NumStr;
					NumStr.Format("%i ", CurrentZigZagNumBars);
					LengthStr += NumStr;
					
					if (UseMultiLineLabels.GetYesNo())
						LengthStr += "\n";
					else
						LengthStr += " ";
					BarLabelText += LengthStr;
				}
			}

			// now go back one more swing
			if (ZigZagReset[BackRefIndex] == 0)
			{
				BackRefIndex = (int)ZigZagPeakIndex[BackRefIndex-1];

				if (ZigZagPeakType[BackRefIndex] == ZigZagPeakType[ZigZagIndex])
				{
					if (DisplayHHHLLLLHLabels.GetYesNo())
					{	
						if (ZigZagPeakType[BackRefIndex] == 1.0f)//High
						{
							if (ZigZagLine[BackRefIndex] > ZigZagLine[ZigZagIndex]) //LowerHigh
							{
								BarLabelText += "LH";
								TextLabels.Data[ZigZagIndex] = 1;
							}
							else //HigherHigh
							{
								BarLabelText += "HH";
								TextLabels.Data[ZigZagIndex] = 2;
							}
						}
						else //Low
						{
							if (ZigZagLine[BackRefIndex] < ZigZagLine[ZigZagIndex]) //HigherLow
							{
								BarLabelText += "HL";
								TextLabels.Data[ZigZagIndex] = 3;
							}
							else //LowerLow
							{
								BarLabelText += "LL";
								TextLabels.Data[ZigZagIndex] = 4;
							}
						}
					}
				}
			}
		}

		if (BarLabelText.GetLength() > 0 && !sc.HideStudy)
		{
			s_UseTool Tool; 
			Tool.Clear();
			Tool.ChartNumber = sc.ChartNumber;
			Tool.Region = sc.GraphRegion;
			Tool.DrawingType = DRAWING_TEXT;
			Tool.ReverseTextColor = 0;
			Tool.BeginIndex = ZigZagIndex;
	
			float Offset;
			if (LabelOffsetType.GetIndex() == 0)
				Offset = LabelsOffset.GetFloat()*0.01f*(sc.High[ZigZagIndex]-sc.Low[ZigZagIndex]);
			else
				Offset = LabelsOffset.GetFloat()*sc.TickSize;

			if(ZigZagPeakType[ZigZagIndex] == 1.0f)
				Tool.BeginValue = ZigZagLine[ZigZagIndex] + Offset;
			else
				Tool.BeginValue = ZigZagLine[ZigZagIndex] - Offset;				

			if (ZigZagPeakType[ZigZagIndex] == 1.0f)
				Tool.Color =  TextLabels.PrimaryColor;
			else
				Tool.Color = TextLabels.SecondaryColor;

			Tool.FontFace = "Arial";
			Tool.FontSize = TextLabels.LineWidth;
			Tool.FontBold = true;
			Tool.Text = BarLabelText;
			Tool.AddMethod = UTAM_ADD_OR_ADJUST;
			Tool.LineNumber = LabelID;
			if (UseMultiLineLabels.GetYesNo())
				Tool.TextAlignment	= DT_LEFT | VerticalTextAlignment;
			else
				Tool.TextAlignment	= DT_CENTER | VerticalTextAlignment;

			sc.UseTool(Tool);
		}

		ReversalPrice[ZigZagIndex] = ZigZagLine[ZigZagIndex];
		ZigzagLength[ZigZagIndex] = (float)ZigzagLineLength;
		ZigzagNumBars[ZigZagIndex] = (float)CurrentZigZagNumBars;
	}
}

/*==========================================================================*/
SCSFExport scsf_ZigZagCumulativeVolumeStudy(SCStudyInterfaceRef sc)
{
	SCSubgraphRef ZigZagLine = sc.Subgraph[0];
	SCSubgraphRef AccumulatedVolume = sc.Subgraph[4];

	SCInputRef VolumeToAccumulate = sc.Input [0];
	SCInputRef VersionUpdate = sc.Input[14];
	SCInputRef InputDataHigh = sc.Input[15];
	SCInputRef InputDataLow = sc.Input[16];
	SCInputRef CalculationMode = sc.Input[17];
	SCInputRef ReversalPercent = sc.Input[18];
	SCInputRef ReversalAmount = sc.Input[19];
	SCInputRef NumberBarsForReversal = sc.Input[20];
	SCInputRef CalcOnBarClose = sc.Input[27];
	
	
	if(sc.SetDefaults)
	{
		sc.GraphName="Zig Zag Cumulative Volume";
	
		//Manual looping
		sc.AutoLoop = 0;
		sc.ValueFormat= 0;
		sc.GraphRegion = 1;

		ZigZagLine.Name = "Zig Zag";
		ZigZagLine.DrawStyle = DRAWSTYLE_IGNORE;

		AccumulatedVolume.Name= "AccumulatedVolume";
		AccumulatedVolume.DrawStyle = DRAWSTYLE_BAR;
		AccumulatedVolume.SecondaryColorUsed = 1;

		VolumeToAccumulate.Name= "Volume To Accumulate";
		VolumeToAccumulate.SetCustomInputStrings("Total Volume;Bid Volume;Ask Volume;Ask Bid Volume Difference");
		VolumeToAccumulate.SetCustomInputIndex(0);

		VersionUpdate.SetInt(1);

		InputDataHigh.Name = "Input Data for High";
		InputDataHigh.SetInputDataIndex(SC_HIGH);

		InputDataLow.Name = "Input Data for Low";
		InputDataLow.SetInputDataIndex(SC_LOW);

		CalculationMode.Name = "Calculation Mode (1,2,3)";
		CalculationMode.SetInt(1);
		CalculationMode.SetIntLimits(1, 3);

		ReversalPercent.Name = "Reversal % for Calculation Mode 1";
		ReversalPercent.SetFloat(.5f);

		ReversalAmount.Name = "Reversal Amount for Calculation Mode 2,3";
		ReversalAmount.SetFloat(0.01f);

		NumberBarsForReversal.Name = "Number of Bars Required for Reversal (Calculation Mode 2)";
		NumberBarsForReversal.SetInt(5);

		CalcOnBarClose.Name = "Calculate New Values On Bar Close";
		CalcOnBarClose.SetYesNo(0);

		return;
	}



	// Standard Manual Looping loop
	for (int Index = sc.UpdateStartIndex; Index < sc.ArraySize; Index++)
	{
		int CalcIndex = Index;

		if (CalcOnBarClose.GetYesNo())
			CalcIndex--;

		if (CalcIndex >= 0)
		{
			switch (CalculationMode.GetInt())
			{
			case 1:
				sc.ZigZag(
					sc.BaseData[InputDataHigh.GetInputDataIndex()],
					sc.BaseData[InputDataLow.GetInputDataIndex()],
					ZigZagLine,
					CalcIndex,
					ReversalPercent.GetFloat() * 0.01f
					);
				break;
			case 2:
				sc.ZigZag2(
					sc.BaseData[InputDataHigh.GetInputDataIndex()],
					sc.BaseData[InputDataLow.GetInputDataIndex()],
					ZigZagLine,
					CalcIndex,
					NumberBarsForReversal.GetInt(),
					ReversalAmount.GetFloat()
					);
				break;
			case 3:
				sc.ZigZag(
					sc.BaseData[InputDataHigh.GetInputDataIndex()],
					sc.BaseData[InputDataLow.GetInputDataIndex()],
					ZigZagLine,
					CalcIndex,
					0.0f,
					ReversalAmount.GetFloat()
					);
				break;
			}
		}
	}


	SCFloatArrayRef ZigZagPeakType = ZigZagLine.Arrays[0];

	int VolumeTypeToAccumulate = VolumeToAccumulate.GetIndex();

	AccumulateZigZagVolume(sc, AccumulatedVolume, ZigZagPeakType, VolumeTypeToAccumulate);
}

/*==========================================================================*/
SCSFExport scsf_VolumeColoredBasedOnVolume2(SCStudyInterfaceRef sc)
{

	SCSubgraphRef Volume = sc.Subgraph[0];
	SCSubgraphRef Volume3rdColor = sc.Subgraph[1];
	SCSubgraphRef Volume4thColor = sc.Subgraph[2];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Volume - Colored Based on Volume 2";

		sc.StudyDescription = "This study displays the Volume per bar. Each volume bar is colored based upon whether the volume is up or down compared to the previous bar volume. It uses a 3rd color when the Volume is lower then the 2 previous bars.";

		sc.FreeDLL = 0;

		sc.AutoLoop = 1;  // true

		Volume.Name = "Volume";
		Volume.DrawStyle = DRAWSTYLE_BAR;
		Volume.SecondaryColorUsed = 1;
		Volume.PrimaryColor = RGB(0,255,0);
		Volume.SecondaryColor = RGB(255,0,0);

		Volume3rdColor.Name = "Volume 3rd Color";
		Volume3rdColor.PrimaryColor = RGB(255,128,64);

		Volume4thColor.Name = "Default Color";
		Volume4thColor.PrimaryColor = RGB(0,128,255);

		return;
	}

	// Section 2 - Do data processing here

	Volume[sc.Index] = sc.Volume[sc.Index];

	if(sc.Volume[sc.Index] < sc.Volume[sc.Index-1]
	&& sc.Volume[sc.Index] < sc.Volume[sc.Index-2])
		Volume.DataColor[sc.Index] = Volume3rdColor.PrimaryColor;
	else if(sc.Volume[sc.Index-1] > sc.Volume[sc.Index])
		Volume.DataColor[sc.Index] = Volume.SecondaryColor;
	else if(sc.Volume[sc.Index-1] < sc.Volume[sc.Index])
		Volume.DataColor[sc.Index] = Volume.PrimaryColor;
	else //If volume is equal
		Volume.DataColor[sc.Index] = Volume4thColor.PrimaryColor;

}

/*==========================================================================*/
SCSFExport scsf_BackgroundColoring(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Top = sc.Subgraph[0];
	SCSubgraphRef Bottom = sc.Subgraph[1];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Background Coloring";

		sc.AutoLoop = 1;
		sc.GraphRegion = 0;

		Top.Name ="Top";
		Top.DrawStyle = DRAWSTYLE_FILLRECTTOP;
		Top.PrimaryColor = RGB(0,255,0);
		Top.DrawZeros = false;

		Bottom.Name ="Bottom";
		Bottom.DrawStyle = DRAWSTYLE_FILLRECTBOTTOM;
		Bottom.DrawZeros = RGB(255,0,255);
		Bottom.DrawZeros = false;

		sc.ScaleRangeType = SCALE_INDEPENDENT;

		// During development set this flag to 1, so the DLL can be modified. When development is done, set it to 0 to improve performance.
		sc.FreeDLL = 0;

		return;
	}

	// Do data processing

	Top[sc.ArraySize - 1] = 2;
	Bottom[sc.ArraySize - 1] = 1;
}

/*==========================================================================*/
SCSFExport scsf_MovingAverageSimple(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Avg = sc.Subgraph[0];

	SCInputRef InputData = sc.Input[0];
	SCInputRef Length = sc.Input[3];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Moving Average-Simple";

		sc.GraphRegion = 0;
		sc.ValueFormat = 2;
		sc.AutoLoop = 1;

		Avg.Name = "Avg";
		Avg.DrawStyle = DRAWSTYLE_LINE;
		Avg.PrimaryColor = RGB(0, 255, 0);
		Avg.LineWidth = 1;
		Avg.DrawZeros = true;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		Length.Name = "Length";
		Length.SetInt(10);
		Length.SetIntLimits(1, MAX_STUDY_LENGTH);
		
		sc.FreeDLL = 0;

		return;
	}

	sc.DataStartIndex = Length.GetInt() - 1;

	sc.SimpleMovAvg(sc.BaseDataIn[InputData.GetInputDataIndex()], Avg,  Length.GetInt());
}

/*==========================================================================*/
SCSFExport scsf_AverageDailyRange(SCStudyInterfaceRef sc)
{
	SCSubgraphRef ADR = sc.Subgraph[0];

	SCInputRef Length = sc.Input[0];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Average Daily Range";

		sc.GraphRegion = 0;
		sc.ValueFormat = VALUEFORMAT_INHERITED;

		sc.AutoLoop = 1;

		ADR.Name = "ADR";
		ADR.DrawStyle = DRAWSTYLE_LINE;
		ADR.PrimaryColor = RGB(0, 255, 0);
		ADR.LineWidth = 2;
		ADR.DrawZeros = true;


		Length.Name = "Length";
		Length.SetInt(30);
		Length.SetIntLimits(1, MAX_STUDY_LENGTH);

		sc.FreeDLL = 0;

		return;
	}

	sc.DataStartIndex = Length.GetInt() - 1;
	ADR.Arrays[0][sc.Index] = sc.High[sc.Index] - sc.Low[sc.Index];


	sc.SimpleMovAvg(ADR.Arrays[0], ADR,  Length.GetInt());
}
/*==========================================================================*/
SCSFExport scsf_OBVWithMovAvg(SCStudyInterfaceRef sc)
{
	SCSubgraphRef OBV = sc.Subgraph[0];
	SCSubgraphRef Avg = sc.Subgraph[1];
	SCInputRef OBVLength = sc.Input[3];
	SCInputRef MovAvgLength = sc.Input[4];

	if (sc.SetDefaults)
	{
		sc.GraphName = "On Balance Volume with Mov Avg";

		sc.ValueFormat = 0;
		sc.AutoLoop = 1;
		
		OBV.Name = "OBV";
		OBV.DrawStyle = DRAWSTYLE_LINE;
		OBV.PrimaryColor = RGB(0,255,0);
		OBV.DrawZeros = true;

		Avg.Name = "Avg";
		Avg.DrawStyle = DRAWSTYLE_LINE;
		Avg.PrimaryColor = RGB(0,0,255);
		Avg.DrawZeros = true;

		//OBVLength.Name = "OBV Length";
		OBVLength.SetInt(10);
		OBVLength.SetIntLimits(1, MAX_STUDY_LENGTH);

		MovAvgLength.Name = "Mov Avg Length";
		MovAvgLength.SetInt(10);
		MovAvgLength.SetIntLimits(1, MAX_STUDY_LENGTH);

		return;
	}

	sc.DataStartIndex = MovAvgLength.GetInt();

	int i = (sc.Index >=1 ? sc.Index : 1);

	if(sc.Close[i] > sc.Close[i - 1])
	{
		OBV[i] = OBV[i - 1] + sc.Volume[i];
	}
	else if(sc.Close[i] < sc.Close[i - 1])
	{
		OBV[i] = OBV[i - 1] - sc.Volume[i];
	}
	else 
		OBV[i] = OBV[i - 1]; 

	sc.SimpleMovAvg(OBV, Avg, MovAvgLength.GetInt());
}

/*==========================================================================*/
SCSFExport scsf_OpenInterest(SCStudyInterfaceRef sc)
{
	SCSubgraphRef OI = sc.Subgraph[0];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Open Interest";

		sc.ValueFormat = 0;
		sc.AutoLoop = 1;

		OI.Name = "OI";
		OI.PrimaryColor = RGB(0,255,0);
		OI.DrawStyle = DRAWSTYLE_BAR;
		OI.DrawZeros = false;

		return;
	}

	OI[sc.Index]= sc.OpenInterest[sc.Index];
}

/*==========================================================================*/
SCSFExport scsf_NumberOfTrades(SCStudyInterfaceRef sc)
{
	SCSubgraphRef T = sc.Subgraph[0];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Number of Trades";

		sc.ValueFormat = 0;
		sc.AutoLoop = 1;

		T.Name = "T";
		T.PrimaryColor = RGB(0,255,0);
		T.DrawStyle = DRAWSTYLE_BAR;
		T.LineWidth= 2;
		T.DrawZeros = false;

		return;
	}

	T[sc.Index]= sc.NumberOfTrades[sc.Index];
}

/*==========================================================================*/
SCSFExport scsf_Sum(SCStudyInterfaceRef sc)
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

	SCInputRef InChart2Number = sc.Input[3];
	SCInputRef InChart1Multiplier = sc.Input[4];
	SCInputRef InChart2Multiplier = sc.Input[5];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Sum";
		
		sc.ValueFormat = 2;
		sc.GraphRegion = 1;
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
		OHLCAvg.DrawZeros = false;
		OHLCAvg.PrimaryColor = RGB(127,0,255);

		HLCAvg.Name = "HLC Avg";
		HLCAvg.DrawStyle = DRAWSTYLE_IGNORE;
		HLCAvg.DrawZeros = false;
		HLCAvg.PrimaryColor = RGB(0,255,255);

		HLAvg.Name = "HL Avg";
		HLAvg.DrawStyle = DRAWSTYLE_IGNORE;
		HLAvg.DrawZeros = false;
		HLAvg.PrimaryColor = RGB(0,127,255);
		
		BidVol.Name = "Bid Vol";
		BidVol.DrawStyle = DRAWSTYLE_IGNORE;
		BidVol.DrawZeros = false;
		BidVol.PrimaryColor = RGB(0,255,0);
		
		AskVol.Name = "Ask Vol";
		AskVol.DrawStyle = DRAWSTYLE_IGNORE;
		AskVol.DrawZeros = false;
		AskVol.PrimaryColor = RGB(0,255,0);


		InChart2Number.Name = "Chart 2 Number";
		InChart2Number.SetChartNumber(1);

		InChart1Multiplier.Name = "Chart 1 Multiplier";
		InChart1Multiplier.SetFloat(1.0f);

		InChart2Multiplier.Name = "Chart 2 Multiplier";
		InChart2Multiplier.SetFloat(1.0f);

		return;
	}
	
	// Obtain a reference to the Base Data in the specified chart.  This call
	// is relatively efficient, but it should be called as minimally as
	// possible.  To reduce the number of calls we have it outside of the
	// primary "for" loop in this study function.  And we also use Manual
	// Looping by not defining sc.AutoLoop = 1.  In this way,
	// sc.GetChartBaseData is called only once per call to this study
	// function.  sc.GetChartBaseData is a new function to get all of the Base
	// Data arrays with one efficient call.
	SCGraphData Chart2BaseData;
	sc.GetChartBaseData(-InChart2Number.GetChartNumber(), Chart2BaseData);
	
	for (int Index = sc.UpdateStartIndex; Index < sc.ArraySize; Index++)
	{
		int Chart2Index = sc.GetNearestMatchForDateTimeIndex(InChart2Number.GetChartNumber(), Index);
		
		for (int SubgraphIndex = SC_OPEN; SubgraphIndex <= SC_NT; SubgraphIndex++)
		{
			sc.Subgraph[SubgraphIndex][Index]
				= (sc.BaseDataIn[SubgraphIndex][Index] * InChart1Multiplier.GetFloat())
				+ (Chart2BaseData[SubgraphIndex][Chart2Index] * InChart2Multiplier.GetFloat());
		}
		
		sc.Subgraph[SC_HIGH][Index]
			= max(sc.Subgraph[SC_OPEN][Index],
				max(sc.Subgraph[SC_HIGH][Index],
					max(sc.Subgraph[SC_LOW][Index], sc.Subgraph[SC_LAST][Index])
				)
			);
		
		sc.Subgraph[SC_LOW][Index]
			= min(sc.Subgraph[SC_OPEN][Index],
				min(sc.Subgraph[SC_HIGH][Index],
					min(sc.Subgraph[SC_LOW][Index], sc.Subgraph[SC_LAST][Index])
				)
			);
		
		sc.CalculateOHLCAverages(Index);
	}
	
	SCString Chart1Name = sc.GetStudyNameFromChart(sc.ChartNumber, 0);
	SCString Chart2Name = sc.GetStudyNameFromChart(InChart2Number.GetChartNumber(), 0);
	sc.GraphName.Format("Sum %s + %s", Chart1Name.GetChars(), Chart2Name.GetChars());
}


/*==========================================================================*/
SCSFExport scsf_RatioBar(SCStudyInterfaceRef sc)
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
	SCSubgraphRef Unused11 = sc.Subgraph[11];
	SCSubgraphRef Unused12 = sc.Subgraph[12];

	SCInputRef Chart2Number = sc.Input[3];
	SCInputRef Chart1Multiplier = sc.Input[4];
	SCInputRef Chart2Multiplier = sc.Input[5];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Ratio - Bar";

		sc.UseGlobalChartColors = 0;
		
		sc.ValueFormat = 2;
		sc.GraphRegion = 1;

		sc.GraphDrawType = GDT_OHLCBAR;
		sc.StandardChartHeader = 1;

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

		Unused11.DrawStyle = DRAWSTYLE_IGNORE;
		Unused11.DrawZeros = false;

		Unused12.DrawStyle = DRAWSTYLE_IGNORE;
		Unused12.DrawZeros = false;


		Chart2Number.Name = "Chart 2 Number";
		Chart2Number.SetChartNumber(1);

		Chart1Multiplier.Name = "Chart 1 Multiplier";
		Chart1Multiplier.SetFloat(1.0f);

		Chart2Multiplier.Name = "Chart 2 Multiplier";
		Chart2Multiplier.SetFloat(1.0f);

		return;
	}
	
	SCGraphData Chart2Arrays;
	sc.GetChartBaseData(-Chart2Number.GetChartNumber(), Chart2Arrays);
	
	if (Chart2Arrays[0].GetArraySize() == 0)
		return;
	
	for (int Index = sc.UpdateStartIndex; Index < sc.ArraySize; Index++)
	{
		int i1 = sc.GetNearestMatchForDateTimeIndex(Chart2Number.GetChartNumber(), Index);
		
		for (int i2 = SC_OPEN; i2 <= SC_NT; i2++)
		{
			if (sc.BaseDataIn[i2][Index] == 0 || Chart2Arrays[i2][i1] == 0)
			{
				sc.Subgraph[i2][Index] = 0;
				continue;
			}
			
			sc.Subgraph[i2][Index] = (sc.BaseDataIn[i2][Index] * Chart1Multiplier.GetFloat())
				/ (Chart2Arrays[i2][i1] * Chart2Multiplier.GetFloat());
		}

		sc.Subgraph[1][Index]
		= max(sc.Subgraph[0][Index],
			max(sc.Subgraph[1][Index],
			max(sc.Subgraph[2][Index],sc.Subgraph[3][Index])
			)
			);

		sc.Subgraph[2][Index]
		= min(sc.Subgraph[0][Index],
			min(sc.Subgraph[1][Index],
			min(sc.Subgraph[2][Index],sc.Subgraph[3][Index])
			)
			);

		sc.CalculateOHLCAverages(Index);
	}

	SCString Chart1Name = sc.GetStudyNameFromChart(sc.ChartNumber, 0);
	SCString Chart2Name = sc.GetStudyNameFromChart(Chart2Number.GetChartNumber(), 0);
	sc.GraphName.Format("Ratio %s / %s", Chart1Name.GetChars(), Chart2Name.GetChars());
}

/*==========================================================================*/
SCSFExport scsf_MultiplyBar(SCStudyInterfaceRef sc)
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
	SCSubgraphRef Unused11 = sc.Subgraph[11];
	SCSubgraphRef Unused12 = sc.Subgraph[12];

	SCInputRef Chart2Number = sc.Input[3];
	SCInputRef Chart1Multiplier = sc.Input[4];
	SCInputRef Chart2Multiplier = sc.Input[5];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Multiply Charts - Bar";

		sc.GraphUsesChartColors= 1;

		sc.ValueFormat = VALUEFORMAT_INHERITED;
		sc.GraphRegion = 1;
		sc.GraphDrawType = GDT_OHLCBAR;
		sc.StandardChartHeader = 1;

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

		Unused11.DrawStyle = DRAWSTYLE_IGNORE;
		Unused11.DrawZeros = false;

		Unused12.DrawStyle = DRAWSTYLE_IGNORE;
		Unused12.DrawZeros = false;

		Chart2Number.Name = "Chart 2 Number";
		Chart2Number.SetChartNumber(1);

		Chart1Multiplier.Name = "Chart 1 Multiplier";
		Chart1Multiplier.SetFloat(1.0f);

		Chart2Multiplier.Name = "Chart 2 Multiplier";
		Chart2Multiplier.SetFloat(1.0f);

		return;
	}

	SCGraphData Chart2Arrays;
	sc.GetChartBaseData(-Chart2Number.GetChartNumber(), Chart2Arrays);

	if (Chart2Arrays[0].GetArraySize() == 0)
		return;

	for (int Index = sc.UpdateStartIndex; Index < sc.ArraySize; Index++)
	{
		int i1 = sc.GetNearestMatchForDateTimeIndex(Chart2Number.GetChartNumber(), Index);

		for (int i2 = SC_OPEN; i2 <= SC_NT; i2++)
		{
			

			sc.Subgraph[i2][Index] = (sc.BaseDataIn[i2][Index] * Chart1Multiplier.GetFloat())
				* (Chart2Arrays[i2][i1] * Chart2Multiplier.GetFloat());
		}

		sc.Subgraph[1][Index]
		= max(sc.Subgraph[0][Index],
			max(sc.Subgraph[1][Index],
			max(sc.Subgraph[2][Index],sc.Subgraph[3][Index])
			)
			);

		sc.Subgraph[2][Index]
		= min(sc.Subgraph[0][Index],
			min(sc.Subgraph[1][Index],
			min(sc.Subgraph[2][Index],sc.Subgraph[3][Index])
			)
			);

		sc.CalculateOHLCAverages(Index);
	}

	SCString Chart1Name = sc.GetStudyNameFromChart(sc.ChartNumber, 0);
	SCString Chart2Name = sc.GetStudyNameFromChart(Chart2Number.GetChartNumber(), 0);
	sc.GraphName.Format("Multiply %s * %s", Chart1Name.GetChars(), Chart2Name.GetChars());
}

/*==========================================================================*/
SCSFExport scsf_OnBalanceVolume(SCStudyInterfaceRef sc)
{
	SCSubgraphRef OBV = sc.Subgraph[0];

	if (sc.SetDefaults)
	{
		sc.GraphName = "On Balance Volume";
		sc.ValueFormat = 0;
		sc.AutoLoop = 1;

		OBV.Name = "OBV";
		OBV.PrimaryColor = RGB(0,255,0);
		OBV.DrawStyle = DRAWSTYLE_LINE;
		OBV.DrawZeros = true;

		return;
	}

	sc.DataStartIndex = 1;

	sc.OnBalanceVolume(sc.BaseDataIn, OBV);
}

/*==========================================================================*/
SCSFExport scsf_Line(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Line = sc.Subgraph[0];
	SCInputRef Value = sc.Input[3];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Line";

		sc.GraphRegion = 1;
		sc.ValueFormat = 2;
		sc.AutoLoop = 1;

		sc.FreeDLL= 0;

		Line.Name = "Line";
		Line.DrawStyle = DRAWSTYLE_LINE;
		Line.PrimaryColor = RGB(0,255,0);
		Line.DrawZeros = 1;

		Value.Name = "Value";
		Value.SetFloat(1.0f);

		return;
	}

	sc.DataStartIndex = 0;

	Line[sc.Index] = Value.GetFloat();
}

/*==========================================================================*/
SCSFExport scsf_AverageTrueRange(SCStudyInterfaceRef sc)
{
	SCSubgraphRef ATR = sc.Subgraph[0];
	SCInputRef MAType = sc.Input[0];
	SCInputRef Length = sc.Input[3];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Average True Range";

		sc.GraphRegion = 1;
		sc.ValueFormat = 2;
		sc.AutoLoop = 1;

		ATR.Name = "ATR";
		ATR.DrawZeros = false;
		ATR.PrimaryColor = RGB(0,255,0);
		ATR.DrawStyle = DRAWSTYLE_LINE;

		MAType.Name = "Moving Average Type";
		MAType.SetMovAvgType(MOVAVGTYPE_SIMPLE);


		Length.Name = "Moving Average Length";
		Length.SetInt(14);
		Length.SetIntLimits(1, MAX_STUDY_LENGTH);

		return;
	}

	sc.DataStartIndex = Length.GetInt() - 1;
	sc.ATR(sc.BaseDataIn, ATR, Length.GetInt(), MAType.GetMovAvgType());
}


/*==========================================================================*/
SCSFExport scsf_TrueRange(SCStudyInterfaceRef sc)
{
	SCSubgraphRef TR = sc.Subgraph[0];

	if (sc.SetDefaults)
	{
		sc.GraphName = "True Range";

		sc.GraphRegion = 1;
		sc.ValueFormat = VALUEFORMAT_INHERITED;
		sc.AutoLoop = 1;

		TR.Name = "TR";
		TR.DrawZeros = false;
		TR.PrimaryColor = RGB(0,255,0);
		TR.DrawStyle = DRAWSTYLE_LINE;


		return;
	}

	sc.TrueRange(sc.BaseDataIn, TR);
}

/*==========================================================================*/
SCSFExport scsf_DailyRangeBand(SCStudyInterfaceRef sc)
{
	SCSubgraphRef UpperBand = sc.Subgraph[0];
	SCSubgraphRef LowerBand = sc.Subgraph[1];

	SCInputRef DailyRangeStudy        = sc.Input[0];
	SCInputRef IncludeYesterdaysClose = sc.Input[1];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Daily Range Band";

		sc.GraphRegion = 0;
		sc.AutoLoop = 1;

		UpperBand.Name = "Upper Range Band";
		UpperBand.DrawZeros = false;
		UpperBand.PrimaryColor = RGB(0,128,192);
		UpperBand.DrawStyle = DRAWSTYLE_STAIR;

		LowerBand.Name = "Lower Range Band";
		LowerBand.DrawZeros = false;
		LowerBand.PrimaryColor = RGB(192,0,0);
		LowerBand.DrawStyle = DRAWSTYLE_STAIR;

		DailyRangeStudy.Name = "Daily Band Range Subgraph";
		DailyRangeStudy.SetChartStudySubgraphValues(1, 1, 0);

		IncludeYesterdaysClose.Name = "Gap Adjust With Yesterdays Close";
		IncludeYesterdaysClose.SetYesNo(1);

		return;
	}

	float& TodaysHigh = sc.PersistVars->f1;
	float& TodaysLow  = sc.PersistVars->f2;
	SCDateTime& CurrentSessionStartDateTime = sc.PersistVars->scdt1;

	int TradeDate = sc.GetTradingDayDate(sc.BaseDateTimeIn[sc.Index]);

	if (sc.Index == 0 || sc.BaseDateTimeIn[sc.Index] >= CurrentSessionStartDateTime + 1*DAYS)
	{
		TodaysHigh = sc.High[sc.Index];
		TodaysLow = sc.Low[sc.Index];
		CurrentSessionStartDateTime = sc.GetStartDateTimeForTradingDate(TradeDate);
	}

	if (sc.High[sc.Index] > TodaysHigh)
		TodaysHigh = sc.High[sc.Index];
	if (sc.Low[sc.Index] < TodaysLow)
		TodaysLow = sc.Low[sc.Index];

	int DailyChartNumber = DailyRangeStudy.GetChartNumber();

	SCFloatArray DailyClose;
	sc.GetChartArray(DailyChartNumber, SC_LAST, DailyClose);

	SCFloatArray DailyBandRange;
	sc.GetStudyArrayFromChartUsingID(DailyRangeStudy.GetChartStudySubgraphValues(), DailyBandRange);

	int DailyIndex = sc.GetFirstIndexForDate(DailyChartNumber, TradeDate);

	if (DailyIndex < 0)
		return;

	float BandRange = DailyBandRange[DailyIndex];

	float GapAdjustedHigh = TodaysHigh;
	float GapAdjustedLow  = TodaysLow;

	if (IncludeYesterdaysClose.GetYesNo())
	{
		if (DailyIndex > 0)
		{
			float PriorClose = DailyClose[DailyIndex-1];
			if (PriorClose != 0)
			{
				GapAdjustedLow  = min(TodaysLow, PriorClose);
				GapAdjustedHigh = max(TodaysHigh, PriorClose);
			}
		}
	}

	UpperBand[sc.Index] = GapAdjustedLow  + BandRange;
	LowerBand[sc.Index] = GapAdjustedHigh - BandRange;
}

/*==========================================================================*/
SCSFExport scsf_MovingAverageExponential(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Avg = sc.Subgraph[0];
	SCInputRef InputData = sc.Input[0];
	SCInputRef Length = sc.Input[3];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Moving Average-Exponential";

		sc.GraphRegion = 0;
		sc.ValueFormat = 3;
		sc.AutoLoop = 1;

		Avg.Name = "Avg";
		Avg.DrawStyle = DRAWSTYLE_LINE;
		Avg.PrimaryColor = RGB(0,255,0);
		Avg.DrawZeros = true;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		Length.Name = "Length";
		Length.SetInt(10);
		Length.SetIntLimits(1, MAX_STUDY_LENGTH);

		return;
	}

	sc.DataStartIndex = Length.GetInt() - 1;

	sc.ExponentialMovAvg(sc.BaseDataIn[InputData.GetInputDataIndex()], Avg, Length.GetInt());
}

/*==========================================================================*/
SCSFExport scsf_MovingAverageWeighted(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Avg = sc.Subgraph[0];
	SCInputRef InputData = sc.Input[0];
	SCInputRef Length = sc.Input[3];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Moving Average-Weighted";

		sc.GraphRegion = 0;
		sc.ValueFormat = 3;
		sc.AutoLoop = 1;

		Avg.Name = "Avg";
		Avg.DrawStyle = DRAWSTYLE_LINE;
		Avg.PrimaryColor = RGB(0,255,0);
		Avg.DrawZeros = true;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		Length.Name = "Length";
		Length.SetInt(10);
		Length.SetIntLimits(1, MAX_STUDY_LENGTH);

		return;
	}

	sc.DataStartIndex = Length.GetInt() - 1;

	sc.WeightedMovingAverage(sc.BaseDataIn[InputData.GetInputDataIndex()], Avg, Length.GetInt());
}

/*==========================================================================*/
SCSFExport scsf_MovingAverageLeastSquares(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Avg = sc.Subgraph[0];
	SCInputRef InputData = sc.Input[0];
	SCInputRef Length = sc.Input[3];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Moving Average-Least Squares";

		sc.GraphRegion = 0;
		sc.ValueFormat = 3;
		sc.AutoLoop = 1;

		Avg.Name = "Avg";
		Avg.DrawStyle = DRAWSTYLE_LINE;
		Avg.PrimaryColor = RGB(0,255,0);
		Avg.DrawZeros = true;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		Length.Name = "Length";
		Length.SetInt(10);
		Length.SetIntLimits(1, MAX_STUDY_LENGTH);

		return;
	}

	sc.DataStartIndex = Length.GetInt() - 1;

	sc.LinearRegressionIndicator(sc.BaseDataIn[InputData.GetInputDataIndex()], Avg, Length.GetInt());
}

/*==========================================================================*/
SCSFExport scsf_MovingAverageEnvelope(SCStudyInterfaceRef sc)
{
	SCSubgraphRef TopBand = sc.Subgraph[0];
	SCSubgraphRef BottomBand = sc.Subgraph[1];
	SCSubgraphRef Average = sc.Subgraph[2];

	SCInputRef InputData = sc.Input[0];
	SCInputRef PercentageOrFixed = sc.Input[1];
	SCInputRef Percentage = sc.Input[3];
	SCInputRef MAType = sc.Input[4];
	SCInputRef MALength = sc.Input[5];
	SCInputRef FixedValue = sc.Input[7];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Moving Average Envelope";

		sc.GraphRegion = 0;
		sc.ValueFormat = 3;
		sc.AutoLoop = 1;

		TopBand.Name = "TopBand";
		TopBand.DrawStyle = DRAWSTYLE_LINE;
		TopBand.PrimaryColor = RGB(0,255,0);
		TopBand.DrawZeros = true;

		BottomBand.Name = "BottomBand";
		BottomBand.DrawStyle = DRAWSTYLE_LINE;
		BottomBand.PrimaryColor = RGB(255,0,255);
		BottomBand.DrawZeros = true;

		Average.Name = "Average";
		Average.DrawStyle = DRAWSTYLE_LINE;
		Average.PrimaryColor = RGB(255,255,0);
		Average.DrawZeros = true;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);
		InputData.DisplayOrder = 1;

		PercentageOrFixed.Name = "Percentage or Fixed Value";
		PercentageOrFixed.SetCustomInputStrings( "Percentage;Fixed Value");
		PercentageOrFixed.SetCustomInputIndex( 0);
		PercentageOrFixed.DisplayOrder = 2;

		Percentage.Name = "Percentage (.01 = 1%)";
		Percentage.SetFloat(0.005f);
		Percentage.DisplayOrder = 3;

		FixedValue.Name = "Fixed Value";
		FixedValue.SetFloat(10.0f);
		FixedValue.DisplayOrder = 4;

		MAType.Name = "Moving Average Type";
		MAType.SetMovAvgType(MOVAVGTYPE_SIMPLE);
		MAType.DisplayOrder = 5;

		MALength.Name = "Moving Average Length";
		MALength.SetInt(10);
		MALength.SetIntLimits(1, MAX_STUDY_LENGTH);
		MALength.DisplayOrder = 6;

		return;
	}

	sc.MovingAverage(sc.BaseDataIn[InputData.GetInputDataIndex()], Average, MAType.GetMovAvgType(), MALength.GetInt());

	sc.DataStartIndex = MALength.GetInt() - 1;

	if (sc.Index < sc.DataStartIndex)
		return;

	if (PercentageOrFixed.GetIndex()  ==  1)//fixed value
	{
		TopBand[sc.Index] = Average[sc.Index] + FixedValue.GetFloat();
		BottomBand[sc.Index] = Average[sc.Index] - FixedValue.GetFloat();
	}
	else//percentage
	{
		TopBand[sc.Index] = Average[sc.Index] * (1 + Percentage.GetFloat());
		BottomBand[sc.Index] = Average[sc.Index] * (1 - Percentage.GetFloat());
	}
}

/*==========================================================================*/
SCSFExport scsf_MarketFacilitationIndex(SCStudyInterfaceRef sc)
{
	SCSubgraphRef MFI = sc.Subgraph[0];
	SCInputRef Multiplier = sc.Input[0];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Market Facilitation Index";

		sc.GraphRegion = 1;
		sc.ValueFormat = 2;
		sc.AutoLoop = 1;
		
		MFI.Name = "MFI";
		MFI.DrawStyle = DRAWSTYLE_LINE;
		MFI.PrimaryColor = RGB(0,255,0);
		MFI.DrawZeros = true;

		Multiplier.Name = "Multiplier";
		Multiplier.SetFloat(1.0f);

		return;
	}

	MFI[sc.Index] = Multiplier.GetFloat() * (sc.High[sc.Index] - sc.Low[sc.Index]) / 
					sc.Volume[sc.Index];
}

/*==========================================================================*/
SCSFExport scsf_Momentum(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Momentum = sc.Subgraph[0];
	SCSubgraphRef CenterLine = sc.Subgraph[1];
	SCInputRef InputData = sc.Input[0];
	SCInputRef Length = sc.Input[3];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Momentum";

		sc.GraphRegion = 1;
		sc.ValueFormat = 2;
		sc.AutoLoop = 1;

		Momentum.Name = "Momentum";
		Momentum.DrawStyle = DRAWSTYLE_LINE;
		Momentum.PrimaryColor = RGB(0,255,0);
		Momentum.DrawZeros = true;
		
		CenterLine.Name = "Line";
		CenterLine.DrawStyle = DRAWSTYLE_LINE;
		CenterLine.PrimaryColor = RGB(128,128,128);
		CenterLine.DrawZeros = true;


		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		Length.Name = "Length";
		Length.SetInt(10);
		Length.SetIntLimits(1, MAX_STUDY_LENGTH);

		return;
	}

	sc.DataStartIndex = Length.GetInt();

	if (sc.Index < sc.DataStartIndex)
		return;

	Momentum[sc.Index] = (sc.BaseDataIn[InputData.GetInputDataIndex()][sc.Index] 
						/ sc.BaseDataIn[InputData.GetInputDataIndex()][sc.Index - Length.GetInt()]) * 100;

	CenterLine[sc.Index] = 100.0;
}

/*==========================================================================*/
SCSFExport scsf_OnBalanceOpenInterest(SCStudyInterfaceRef sc)
{
	SCSubgraphRef OBOI = sc.Subgraph[0];
	SCInputRef Length = sc.Input[3];

	if (sc.SetDefaults)
	{
		sc.GraphName = "On Balance Open Interest";

		sc.AutoLoop = 1;

		OBOI.Name = "OBOI";
		OBOI.DrawStyle = DRAWSTYLE_LINE;
		OBOI.PrimaryColor = RGB(0,255,0);
		OBOI.DrawZeros = true;

		//Length.Name = "Length";
		Length.SetInt(10);
		Length.SetIntLimits(1, MAX_STUDY_LENGTH);

		return;
	}

	sc.DataStartIndex = 1;

	if (sc.Index == 0)
		return;

	if (sc.Close[sc.Index] > sc.Close[sc.Index - 1])
		OBOI[sc.Index] = OBOI[sc.Index - 1] + sc.OpenInterest[sc.Index];
	else if (sc.Close[sc.Index] < sc.Close[sc.Index - 1])
		OBOI[sc.Index] = OBOI[sc.Index - 1] - sc.OpenInterest[sc.Index];
	else
		OBOI[sc.Index] = OBOI[sc.Index - 1];
}

/*==========================================================================*/
SCSFExport scsf_RateOfChangePercentage(SCStudyInterfaceRef sc)
{
	SCSubgraphRef ROC = sc.Subgraph[0];
	SCInputRef InputData = sc.Input[0];
	SCInputRef Length = sc.Input[3];
	SCInputRef MultFactor = sc.Input[4];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Rate Of Change - Percentage";

		sc.GraphRegion = 1;
		sc.ValueFormat = 2;
		sc.AutoLoop = 1;

		ROC.Name = "ROC";
		ROC.DrawStyle = DRAWSTYLE_LINE;
		ROC.PrimaryColor = RGB(0,255,0);
		ROC.DrawZeros = true;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		Length.Name = "Length";
		Length.SetInt(10);
		Length.SetIntLimits(1, MAX_STUDY_LENGTH);

		MultFactor.Name = "Multiplication Factor";
		MultFactor.SetFloat(100.0f);
		MultFactor.SetFloatLimits(1.0f, (float)MAX_STUDY_LENGTH);

		return;
	}

	sc.DataStartIndex = Length.GetInt();

	if (sc.Index < sc.DataStartIndex)
		return;

	ROC[sc.Index] = (sc.BaseDataIn[InputData.GetInputDataIndex()][sc.Index] - 
					sc.BaseDataIn[InputData.GetInputDataIndex()][sc.Index - Length.GetInt()]) /
					sc.BaseDataIn[InputData.GetInputDataIndex()][sc.Index - Length.GetInt()] * MultFactor.GetFloat();
}

/*==========================================================================*/
SCSFExport scsf_KeltnerChannel(SCStudyInterfaceRef sc)
{
	SCSubgraphRef KeltnerAverage = sc.Subgraph[0];
	SCSubgraphRef TopBand = sc.Subgraph[1];
	SCSubgraphRef BottomBand = sc.Subgraph[2];
	SCSubgraphRef AtrTemp3 = sc.Subgraph[3];
	SCSubgraphRef AtrTemp4 = sc.Subgraph[4];

	SCInputRef InputData = sc.Input[0];
	SCInputRef KeltnerMALength = sc.Input[3];
	SCInputRef TrueRangeAvgLength = sc.Input[4];
	SCInputRef TopBandMult = sc.Input[5];
	SCInputRef BottomBandMult = sc.Input[6];
	SCInputRef KeltnerMAType = sc.Input[7];
	SCInputRef ATR_MAType = sc.Input[8];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Keltner Channel";

		sc.GraphRegion = 0;
		sc.ValueFormat = 3;
		sc.AutoLoop = 1;

		KeltnerAverage.Name = "Keltner Average";
		KeltnerAverage.DrawStyle = DRAWSTYLE_LINE;
		KeltnerAverage.PrimaryColor = RGB(0,255,0);
		KeltnerAverage.DrawZeros = true;

		TopBand.Name = "Top";
		TopBand.DrawStyle = DRAWSTYLE_LINE;
		TopBand.PrimaryColor = RGB(255,0,255);
		TopBand.DrawZeros = true;

		BottomBand.Name = "Bottom";
		BottomBand.DrawStyle = DRAWSTYLE_LINE;
		BottomBand.PrimaryColor = RGB(255,255,0);
		BottomBand.DrawZeros = true;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_OHLC);

		KeltnerMALength.Name = "Keltner Mov Avg Length";
		KeltnerMALength.SetInt(10);
		KeltnerMALength.SetIntLimits(1, MAX_STUDY_LENGTH);

		TrueRangeAvgLength.Name = "True Range Avg Length";
		TrueRangeAvgLength.SetInt(10);
		TrueRangeAvgLength.SetIntLimits(1, MAX_STUDY_LENGTH);

		TopBandMult.Name = "Top Band Multiplier";
		TopBandMult.SetFloat(1.6f);

		BottomBandMult.Name="Bottom Band Multiplier";
		BottomBandMult.SetFloat(1.6f);

		KeltnerMAType.Name = "Keltner Mov Avg Type (Center line)";
		KeltnerMAType.SetMovAvgType(MOVAVGTYPE_SIMPLE);

		ATR_MAType.Name = "ATR Mov Avg Type";
		ATR_MAType.SetMovAvgType(MOVAVGTYPE_WILDERS);

		return;
	}

	sc.DataStartIndex = max(KeltnerMALength.GetInt(), TrueRangeAvgLength.GetInt());

	sc.MovingAverage(sc.BaseDataIn[(int)InputData.GetInputDataIndex()], KeltnerAverage, KeltnerMAType.GetMovAvgType(), KeltnerMALength.GetInt());
	
	sc.ATR(sc.BaseDataIn, AtrTemp3, TrueRangeAvgLength.GetInt(), ATR_MAType.GetMovAvgType());
	sc.ATR(sc.BaseDataIn, AtrTemp4, TrueRangeAvgLength.GetInt(), ATR_MAType.GetMovAvgType());

	TopBand[sc.Index] = AtrTemp3[sc.Index] * TopBandMult.GetFloat() + KeltnerAverage[sc.Index];
	BottomBand[sc.Index] = KeltnerAverage[sc.Index] - (AtrTemp4[sc.Index] * BottomBandMult.GetFloat());
}

/*==========================================================================*/
SCSFExport scsf_OnBalanceVolumeShortTerm(SCStudyInterfaceRef sc)
{
	SCSubgraphRef OBV = sc.Subgraph[0];
	SCInputRef Length = sc.Input[3];

	if (sc.SetDefaults)
	{
		sc.GraphName = "On Balance Volume - Short Term";

		sc.ValueFormat = 0;
		sc.AutoLoop = 1;

		OBV.Name = "OBV Length";
		OBV.DrawStyle = DRAWSTYLE_LINE;
		OBV.PrimaryColor = RGB(0,255,0);
		OBV.DrawZeros = true;

		Length.Name = "Length";
		Length.SetInt(10);
		Length.SetIntLimits(1, MAX_STUDY_LENGTH);

		return;
	}

	sc.OnBalanceVolumeShortTerm(sc.BaseDataIn, OBV, Length.GetInt());
}

/*==========================================================================*/

SCSFExport scsf_RSI(SCStudyInterfaceRef sc)
{
	SCSubgraphRef RSI = sc.Subgraph[0];
	SCSubgraphRef Line1 = sc.Subgraph[1];
	SCSubgraphRef Line2 = sc.Subgraph[2];
	SCSubgraphRef RSIAvg = sc.Subgraph[3];

	SCInputRef InputData = sc.Input[0];
	SCInputRef RSILength = sc.Input[3];
	SCInputRef RSI_MALength = sc.Input[4];
	SCInputRef Line1Value = sc.Input[5];
	SCInputRef Line2Value = sc.Input[6];
	SCInputRef AvgType = sc.Input[7];

	if (sc.SetDefaults)
	{
		sc.GraphName = "RSI";
		sc.GraphRegion = 1;
		sc.ValueFormat = 2;
		sc.AutoLoop = 1;

		RSI.Name = "RSI";
		RSI.DrawStyle = DRAWSTYLE_LINE;
		RSI.PrimaryColor = RGB(0,255,0);
		RSI.DrawZeros = true;

		Line1.Name = "Line1";
		Line1.DrawStyle = DRAWSTYLE_LINE;
		Line1.PrimaryColor = RGB(255,0,255);
		Line1.DrawZeros = true;
		
		Line2.Name = "Line2";
		Line2.DrawStyle = DRAWSTYLE_LINE;
		Line2.PrimaryColor = RGB(255,255,0);
		Line2.DrawZeros = true;
		
		RSIAvg.Name = "RSI Avg";
		RSIAvg.DrawStyle = DRAWSTYLE_LINE;
		RSIAvg.PrimaryColor = RGB(255,127,0);
		RSIAvg.DrawZeros = true;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		RSILength.Name = "RSI Length";
		RSILength.SetInt(10);
		RSILength.SetIntLimits(1, MAX_STUDY_LENGTH);

		RSI_MALength.Name = "RSI Mov Avg Length";
		RSI_MALength.SetInt(3);
		RSI_MALength.SetIntLimits(1, MAX_STUDY_LENGTH);

		Line1Value.Name = "Line1 Value";
		Line1Value.SetFloat(70.0f);

		Line2Value.Name = "Line2 Value";
		Line2Value.SetFloat(30.0f);

	
		AvgType.Name = "Average Type";
		AvgType.SetMovAvgType(MOVAVGTYPE_SIMPLE);

		return;
	}

	sc.DataStartIndex = (RSILength.GetInt() + RSI_MALength.GetInt());


	sc.RSI(sc.BaseDataIn[InputData.GetInputDataIndex()], RSI, AvgType.GetMovAvgType(), RSILength.GetInt());
	sc.MovingAverage(RSI, RSIAvg, AvgType.GetMovAvgType(), RSI_MALength.GetInt());

	Line1[sc.Index] = Line1Value.GetFloat();
	Line2[sc.Index] = Line2Value.GetFloat();
}

/*==========================================================================*/
SCSFExport scsf_RSI_W(SCStudyInterfaceRef sc)
{
	SCSubgraphRef RSIW = sc.Subgraph[0];
	SCSubgraphRef Line1 = sc.Subgraph[1];
	SCSubgraphRef Line2 = sc.Subgraph[2];
	SCSubgraphRef RSIWAvg = sc.Subgraph[3];

	SCInputRef InputData = sc.Input[0];
	SCInputRef RSILength = sc.Input[3];
	SCInputRef RSI_MALength = sc.Input[4];
	SCInputRef Line1Value = sc.Input[5];
	SCInputRef Line2Value = sc.Input[6];
	SCInputRef AvgType = sc.Input[7];

	if (sc.SetDefaults)
	{
		sc.GraphName = "RSI-W";

		sc.GraphRegion = 1;
		sc.ValueFormat = 2;
		sc.AutoLoop = 1;

		RSIW.Name = "RSI-W";
		RSIW.DrawStyle = DRAWSTYLE_LINE;
		RSIW.PrimaryColor = RGB(0,255,0);
		RSIW.DrawZeros = true;

		Line1.Name = "Line1";
		Line1.DrawStyle = DRAWSTYLE_LINE;
		Line1.PrimaryColor = RGB(255,0,255);
		Line1.DrawZeros = true;
		
		Line2.Name = "Line2";
		Line2.DrawStyle = DRAWSTYLE_LINE;
		Line2.PrimaryColor = RGB(255,255,0);
		Line2.DrawZeros = true;
		
		RSIWAvg.Name = "RSI-W Avg";
		RSIWAvg.DrawStyle = DRAWSTYLE_LINE;
		RSIWAvg.PrimaryColor = RGB(255,127,0);
		RSIWAvg.DrawZeros = true;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		RSILength.Name = "RSI Length";
		RSILength.SetInt(10);
		RSILength.SetIntLimits(1, MAX_STUDY_LENGTH);

		RSI_MALength.Name = "RSI Mov Avg Length";
		RSI_MALength.SetInt(3);
		RSI_MALength.SetIntLimits(1, MAX_STUDY_LENGTH);

		Line1Value.Name = "Line1 Value";
		Line1Value.SetFloat(70.0f);

		Line2Value.Name = "Line2 Value";
		Line2Value.SetFloat(30.0f);

		AvgType.Name = "Average Type";
		AvgType.SetMovAvgType(MOVAVGTYPE_WILDERS);

		return;
	}

	sc.DataStartIndex = (RSILength.GetInt() + RSI_MALength.GetInt());


	sc.RSI(sc.BaseDataIn[InputData.GetInputDataIndex()], RSIW, sc.Index, AvgType.GetMovAvgType(), RSILength.GetInt());
	sc.MovingAverage(RSIW, RSIWAvg, AvgType.GetMovAvgType(), RSI_MALength.GetInt());

	Line1[sc.Index] = Line1Value.GetFloat();
	Line2[sc.Index] = Line2Value.GetFloat();
}

/*==========================================================================*/
SCSFExport scsf_AccumulationDistributionFlow(SCStudyInterfaceRef sc)
{
	SCSubgraphRef AccumulationDistribution = sc.Subgraph[0];
	SCSubgraphRef ADFlowAvg = sc.Subgraph[1];
	SCInputRef MALength = sc.Input[3];
	SCInputRef UsePrevClose = sc.Input[4];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Accumulation/Distribution Flow";

		sc.ValueFormat = 2;
		sc.AutoLoop = 1;

		AccumulationDistribution.Name = "A/D Flow";
		AccumulationDistribution.DrawStyle = DRAWSTYLE_LINE;
		AccumulationDistribution.PrimaryColor = RGB(0,255,0);
		AccumulationDistribution.DrawZeros = true;

		ADFlowAvg.Name = "A/D Flow Avg";
		ADFlowAvg.DrawStyle = DRAWSTYLE_LINE;
		ADFlowAvg.PrimaryColor = RGB(255,0,255);
		ADFlowAvg.DrawZeros = true;

		MALength.Name = "Moving Average Length";
		MALength.SetInt(10);
		MALength.SetIntLimits(1, MAX_STUDY_LENGTH);

		UsePrevClose.Name = "Use Previous Close";
		UsePrevClose.SetYesNo(true);

		return;
	}

	sc.DataStartIndex = MALength.GetInt();

	if (sc.Index == 0)
	{
		AccumulationDistribution[sc.Index] = 5000.0f;
		return;
	}

	float OpenOrClose = 0.0f;
	if (UsePrevClose.GetYesNo())
	{
		OpenOrClose = sc.Close[sc.Index - 1];
	}
	else
	{
		OpenOrClose = sc.Open[sc.Index];
	}

	if (sc.High[sc.Index] - sc.Low[sc.Index] == 0.0f)
	{
		AccumulationDistribution[sc.Index] = AccumulationDistribution[sc.Index - 1];
	}
	else
	{
		AccumulationDistribution[sc.Index] = ((sc.Close[sc.Index] - OpenOrClose) / (sc.High[sc.Index] - sc.Low[sc.Index]) *
			sc.Volume[sc.Index]) + AccumulationDistribution[sc.Index - 1];
	}

	sc.SimpleMovAvg(AccumulationDistribution, ADFlowAvg, MALength.GetInt());
}

/*==========================================================================*/
SCSFExport scsf_AccumulationDistribution(SCStudyInterfaceRef sc)
{


	//Subgraph references
	SCSubgraphRef AccumulationDistribution = sc.Subgraph[0];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Accumulation/Distribution";

		sc.ValueFormat = 3;
		sc.AutoLoop = 1;

		AccumulationDistribution.Name = "A/D";
		AccumulationDistribution.DrawStyle = DRAWSTYLE_LINE;
		AccumulationDistribution.PrimaryColor = RGB(0,255,0);
		AccumulationDistribution.DrawZeros = true;

		return;
	}

	sc.DataStartIndex = 0;

	float RangeVolume = (sc.High[sc.Index] - sc.Low[sc.Index]) * sc.Volume[sc.Index];

	if(RangeVolume == 0)
		AccumulationDistribution[sc.Index] = AccumulationDistribution[sc.Index - 1];
	else
		// Accumulation/Distribution = ((Close – Low) – (High – Close)) / (High – Low) * Bar Volume
		AccumulationDistribution[sc.Index] = ((sc.Close[sc.Index] - sc.Low[sc.Index]) - (sc.High[sc.Index] - sc.Close[sc.Index])) /  RangeVolume;



}


/*==========================================================================*/
SCSFExport scsf_VolumeBarRangeRatio(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Ratio = sc.Subgraph[0];

	SCFloatArrayRef High = sc.High;
	SCFloatArrayRef Low = sc.Low;
	SCFloatArrayRef Volume = sc.Volume;

	if (sc.SetDefaults)
	{
		sc.GraphName = "Volume Bar Range Ratio";

		sc.StudyDescription = "Formula: Volume / (High - Low)";
		sc.AutoLoop = 1;

		Ratio.Name = "Ratio";
		Ratio.DrawStyle = DRAWSTYLE_BAR;
		Ratio.PrimaryColor = RGB(0,255,0);

		return;
	}

	float Range = High[sc.Index] - Low[sc.Index];

	if (Range>0.0)
		Ratio[sc.Index] = Volume[sc.Index] / Range;
	else//If we have no range, then use the prior calculated value.
		Ratio[sc.Index] = Ratio[sc.Index-1];
	
}

/*==========================================================================*/
SCSFExport scsf_MovingAverages(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Avg1 = sc.Subgraph[0];
	SCSubgraphRef Avg2 = sc.Subgraph[1];
	SCSubgraphRef Avg3 = sc.Subgraph[2];
	
	SCInputRef MAType1 = sc.Input[0];
	SCInputRef InputData1 = sc.Input[1];
	SCInputRef Length1 = sc.Input[2];

	SCInputRef MAType2 = sc.Input[3];
	SCInputRef InputData2 = sc.Input[4];
	SCInputRef Length2 = sc.Input[5];

	SCInputRef MAType3 = sc.Input[6];
	SCInputRef InputData3 = sc.Input[7];
	SCInputRef Length3 = sc.Input[8];

	SCInputRef Version = sc.Input[12];
	
	
	if (sc.SetDefaults)
	{
		sc.GraphName = "Moving Averages";
		
		sc.GraphRegion = 0;
		sc.ValueFormat = 3;
		sc.AutoLoop = 1;
		
		Avg1.Name = "Avg1";
		Avg1.DrawStyle = DRAWSTYLE_LINE;
		Avg1.PrimaryColor = RGB(0,255,0);

		Avg2.Name = "Avg2";
		Avg2.DrawStyle = DRAWSTYLE_LINE;
		Avg2.PrimaryColor = RGB(255,0,255);

		Avg3.Name = "Avg3";
		Avg3.DrawStyle = DRAWSTYLE_LINE;
		Avg3.PrimaryColor = RGB(255,255,0);
		
		MAType1.Name = "Moving Average Type 1";
		MAType1.SetMovAvgType(MOVAVGTYPE_SIMPLE);
		
		InputData1.Name = "Input Data 1";
		InputData1.SetInputDataIndex(SC_LAST);
		
		Length1.Name = "Length 1";
		Length1.SetInt(5);
		Length1.SetIntLimits(1, INT_MAX);
		
		MAType2.Name = "Moving Average Type 2";
		MAType2.SetMovAvgType(MOVAVGTYPE_SIMPLE);
		
		InputData2.Name = "Input Data 2";
		InputData2.SetInputDataIndex(SC_LAST);
		
		Length2.Name = "Length 2";
		Length2.SetInt(10);
		Length2.SetIntLimits(1, INT_MAX);
		
		MAType3.Name = "Moving Average Type 3";
		MAType3.SetMovAvgType(MOVAVGTYPE_SIMPLE);
		
		InputData3.Name = "Input Data 3";
		InputData3.SetInputDataIndex(SC_LAST);
		
		Length3.Name = "Length 3";
		Length3.SetInt(15);
		Length3.SetIntLimits(1, INT_MAX);
		
		Version.SetInt(2);
		
		return;
	}


	if (Version.GetInt() < 2)
	{
		unsigned long OldInputData = sc.Input[0].GetInputDataIndex();
		int OldLength1 = sc.Input[3].GetInt();
		int OldLength2 = sc.Input[4].GetInt();
		int OldLength3 = sc.Input[5].GetInt();
		unsigned long OldMAtype = sc.Input[7].GetMovAvgType();

		MAType1.SetMovAvgType(OldMAtype);
		InputData1.SetInputDataIndex(OldInputData);
		Length1.SetInt(OldLength1);

		MAType2.SetMovAvgType(OldMAtype);
		InputData2.SetInputDataIndex(OldInputData);
		Length2.SetInt(OldLength2);

		MAType3.SetMovAvgType(OldMAtype);
		InputData3.SetInputDataIndex(OldInputData);
		Length3.SetInt(OldLength3);

		Version.SetInt(2);
	}

	// 
	if(Length1.GetInt()  <= 0)
	{
		Length1.SetInt(10);
		MAType1.SetMovAvgType(MOVAVGTYPE_SIMPLE);
		InputData1.SetInputDataIndex(SC_LAST);
	}

	if(Length2.GetInt() <= 0)
	{
		Length2.SetInt(20);
		MAType2.SetMovAvgType(MOVAVGTYPE_SIMPLE);
		InputData2.SetInputDataIndex(SC_LAST);
	}

	if(Length3.GetInt()  <= 0)
	{
		Length3.SetInt(50);
		MAType3.SetMovAvgType(MOVAVGTYPE_SIMPLE);
		InputData3.SetInputDataIndex(SC_LAST);
	}

	

	
	sc.DataStartIndex = max(Length3.GetInt(), max(Length1.GetInt(), Length2.GetInt()));
	
	sc.MovingAverage(sc.BaseDataIn[InputData1.GetInputDataIndex()], Avg1, MAType1.GetMovAvgType(), Length1.GetInt());
	sc.MovingAverage(sc.BaseDataIn[InputData2.GetInputDataIndex()], Avg2, MAType2.GetMovAvgType(), Length2.GetInt());
	sc.MovingAverage(sc.BaseDataIn[InputData3.GetInputDataIndex()], Avg3, MAType3.GetMovAvgType(), Length3.GetInt());
}

/*==========================================================================*/
SCSFExport scsf_STIX(SCStudyInterfaceRef sc)
{
	SCSubgraphRef STIX = sc.Subgraph[0];
	SCInputRef DecliningIssuesChartNumber = sc.Input[3];

	if (sc.SetDefaults)
	{
		sc.GraphName = "STIX";
		sc.GraphRegion = 1;
		sc.ValueFormat = 0;

		STIX.Name = "STIX";
		STIX.DrawStyle = DRAWSTYLE_LINE;
		STIX.PrimaryColor = RGB(0,255,0);
		STIX.DrawZeros = true;

		DecliningIssuesChartNumber.Name = "Declining Issues Chart #";
		DecliningIssuesChartNumber.SetChartNumber(1);

		return;
	}

	SCGraphData ChartBaseData;
	sc.GetChartBaseData(-DecliningIssuesChartNumber.GetChartNumber(), ChartBaseData);
	
	SCFloatArrayRef Chart2Array = ChartBaseData[SC_LAST];
	
	if (Chart2Array.GetArraySize() == 0)
		return;

	for (int i = sc.UpdateStartIndex; i < sc.ArraySize; i++)
	{
		int i1 = sc.GetNearestMatchForDateTimeIndex(DecliningIssuesChartNumber.GetChartNumber(), i);

		if (sc.Close[i] == 0 || Chart2Array[i1] == 0)
		{
			STIX[i] = STIX[i - 1];
		}
		else
		{
			STIX[i] = sc.Close[i] / (sc.Close[i] + Chart2Array[i1]) * 100.0f * 0.09f + 
				STIX[i - 1] * 0.91f;
		}
	}
}

/*==========================================================================*/
SCSFExport scsf_AwesomeOscillator(SCStudyInterfaceRef sc)
{
	SCSubgraphRef AO = sc.Subgraph[0];
	SCSubgraphRef ZeroLine = sc.Subgraph[1];

	SCInputRef InputData = sc.Input[0];
	SCInputRef MA1Length = sc.Input[2];
	SCInputRef MA2Length = sc.Input[3];
	SCInputRef MAType = sc.Input[4];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Awesome Oscillator";

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

	sc.DataStartIndex = max(MA1Length.GetInt(), MA2Length.GetInt()) - 1;



	sc.MovingAverage(sc.BaseDataIn[InputData.GetInputDataIndex()], AO.Arrays[0], MAType.GetMovAvgType(), MA1Length.GetInt());
	sc.MovingAverage(sc.BaseDataIn[InputData.GetInputDataIndex()], AO.Arrays[1], MAType.GetMovAvgType(), MA2Length.GetInt());

	AO.Data[sc.Index] = AO.Arrays[1][sc.Index] - AO.Arrays[0][sc.Index];

}

/*==========================================================================*/
SCSFExport scsf_MovingAverageDifference(SCStudyInterfaceRef sc)
{
	SCSubgraphRef MovAvgDiff = sc.Subgraph[0];
	SCInputRef InputData = sc.Input[0];
	SCInputRef MA1Length = sc.Input[2];
	SCInputRef MA2Length = sc.Input[3];
	SCInputRef MovAvgType = sc.Input[4];
	SCInputRef Version = sc.Input[5];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Moving Average Difference";
	
		sc.GraphRegion = 1;
		sc.ValueFormat = 3;
		sc.AutoLoop = 1;

		MovAvgDiff.Name = "MovAvg Diff";
		MovAvgDiff.DrawStyle = DRAWSTYLE_BAR;
		MovAvgDiff.SecondaryColor = RGB(255,0,0);
		MovAvgDiff.AutoColoring = AUTOCOLOR_SLOPE;
		MovAvgDiff.DrawZeros = true;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_HL);

		MA1Length.Name = "Moving Average 1 Length";
		MA1Length.SetInt(30);
		MA1Length.SetIntLimits(1, MAX_STUDY_LENGTH);

		MA2Length.Name = "Moving Average 2 Length";
		MA2Length.SetInt(10);
		MA2Length.SetIntLimits(1, MAX_STUDY_LENGTH);

		MovAvgType.Name = "Moving Average Type";
		MovAvgType.SetMovAvgType(MOVAVGTYPE_SIMPLE);

		Version.SetInt(1);

		return;
	}

	if(Version.GetInt() < 1)
	{
		MovAvgType.SetMovAvgType(MOVAVGTYPE_SIMPLE);
		Version.SetInt(1);
	}

	sc.DataStartIndex = max(MA1Length.GetInt(), MA2Length.GetInt());


	sc.MovingAverage(sc.BaseDataIn[InputData.GetInputDataIndex()], MovAvgDiff.Arrays[0], MovAvgType.GetMovAvgType(), MA1Length.GetInt());
	sc.MovingAverage(sc.BaseDataIn[InputData.GetInputDataIndex()], MovAvgDiff.Arrays[1], MovAvgType.GetMovAvgType(), MA2Length.GetInt());
	MovAvgDiff[sc.Index] = MovAvgDiff.Arrays[0][sc.Index] - MovAvgDiff.Arrays[1][sc.Index];
}

/*==========================================================================*/

SCSFExport scsf_Volume(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Volume = sc.Subgraph[0];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Volume";

		sc.ValueFormat = 0;
		sc.AutoLoop = true;
		sc.ScaleRangeType = SCALE_ZEROBASED;

		Volume.Name = "Volume";
		Volume.DrawStyle = DRAWSTYLE_BAR;
		Volume.AutoColoring = AUTOCOLOR_BASEGRAPH;
		Volume.PrimaryColor = RGB(0, 255, 0);
		Volume.SecondaryColor = RGB(255, 0, 0);
		Volume.DrawZeros = false;
		Volume.LineWidth = 2;


		sc.DisplayStudyInputValues = false;

		return;
	}


	Volume[sc.Index] = sc.Volume[sc.Index];

}


/*==========================================================================*/

SCSFExport scsf_VolumeWithZeroColor(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Volume = sc.Subgraph[0];
	SCSubgraphRef NoPriceChangeColor = sc.Subgraph[1];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Volume with 0 Color";

		sc.ValueFormat = 0;
		sc.AutoLoop = true;
		sc.ScaleRangeType = SCALE_ZEROBASED;

		Volume.Name = "Volume";
		Volume.DrawStyle = DRAWSTYLE_BAR;
		Volume.PrimaryColor = RGB(0, 255, 0);
		Volume.SecondaryColor = RGB(255, 0, 0);
		Volume.SecondaryColorUsed = true;
		Volume.DrawZeros = false;
		Volume.LineWidth = 2;

		NoPriceChangeColor.Name = "No Price Change Color";
		NoPriceChangeColor.DrawStyle = DRAWSTYLE_CUSTOM_TEXT;
		NoPriceChangeColor.PrimaryColor = RGB(0, 255, 0);
		NoPriceChangeColor.DrawZeros = false;
		NoPriceChangeColor.DisplayNameValueInWindowsFlags = 0;
		sc.DisplayStudyInputValues = false;


		return;
	}


	Volume[sc.Index] = sc.Volume[sc.Index]; 

	if(sc.FormattedEvaluate(sc.BaseData[SC_OPEN][sc.Index], sc.BaseGraphValueFormat, LESS_OPERATOR, sc.BaseData[SC_LAST][sc.Index], sc.BaseGraphValueFormat) )
		Volume.DataColor[sc.Index] =	Volume.PrimaryColor;
	else if(sc.FormattedEvaluate(sc.BaseData[SC_OPEN][sc.Index], sc.BaseGraphValueFormat, GREATER_OPERATOR, sc.BaseData[SC_LAST][sc.Index], sc.BaseGraphValueFormat) )
		Volume.DataColor[sc.Index] =	Volume.SecondaryColor;
	else 
		Volume.DataColor[sc.Index] =	NoPriceChangeColor.PrimaryColor;
}

/*==========================================================================*/


SCSFExport scsf_VolumeColoredBasedOnBarCloses(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Volume = sc.Subgraph[0];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Volume-Colored Based on Bar Closes";

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


		if(sc.Close[sc.Index] >=  sc.Close[sc.Index - 1])
			Volume.DataColor[sc.Index] = Volume.PrimaryColor;
		else
			Volume.DataColor[sc.Index] = Volume.SecondaryColor;

}

/*==========================================================================*/

SCSFExport scsf_Spread3Chart(SCStudyInterfaceRef sc)
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

	SCInputRef InChart2Number = sc.Input[3];
	SCInputRef InChart3Number = sc.Input[4];
	SCInputRef InChart1Multiplier = sc.Input[5];
	SCInputRef InChart2Multiplier = sc.Input[6];
	SCInputRef InChart3Multiplier = sc.Input[7];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Spread - 3 Chart";

		
		sc.ValueFormat = 2;
		sc.GraphRegion = 1;
		
		sc.UseGlobalChartColors = 0;
		sc.GraphDrawType		= GDT_OHLCBAR;
		sc.StandardChartHeader	= 1;
		
		Open.Name = "Open";
		Open.DrawStyle = DRAWSTYLE_LINE;
		Open.PrimaryColor = RGB(0,255,0);
		Open.SecondaryColor = RGB(0,255,0);

		High.Name = "High";
		High.DrawStyle = DRAWSTYLE_LINE;
		High.PrimaryColor = RGB(0,255,0);
		High.SecondaryColor = RGB(0,255,0);

		Low.Name = "Low";
		Low.DrawStyle = DRAWSTYLE_LINE;
		Low.PrimaryColor = RGB(0,255,0);
		Low.SecondaryColor = RGB(0,255,0);

		Last.Name = "Last";
		Last.DrawStyle = DRAWSTYLE_LINE;
		Last.PrimaryColor = RGB(0,255,0);
		Last.SecondaryColor = RGB(0,255,0);

		Volume.Name = "Volume";
		Volume.DrawStyle = DRAWSTYLE_IGNORE;
		Volume.PrimaryColor = RGB(255,0,0);

		OpenInterest.Name = "# of Trades / OI";
		OpenInterest.DrawStyle = DRAWSTYLE_IGNORE;
		OpenInterest.PrimaryColor = RGB(0,0,255);

		OHLCAvg.Name = "OHLC Avg";
		OHLCAvg.DrawStyle = DRAWSTYLE_IGNORE;
		OHLCAvg.PrimaryColor = RGB(0,255,0);

		HLCAvg.Name = "HLC Avg";
		HLCAvg.DrawStyle = DRAWSTYLE_IGNORE;
		HLCAvg.PrimaryColor = RGB(0,255,255);

		HLAvg.Name = "HL Avg";
		HLAvg.DrawStyle = DRAWSTYLE_IGNORE;
		HLAvg.PrimaryColor = RGB(0,127,255);

		BidVol.Name = "Bid Vol";
		BidVol.DrawStyle = DRAWSTYLE_IGNORE;
		BidVol.PrimaryColor = RGB(0,255,0);

		AskVol.Name = "Ask Vol";
		AskVol.DrawStyle = DRAWSTYLE_IGNORE;
		AskVol.PrimaryColor = RGB(0,255,0);


		
		InChart2Number.Name = "Chart 2 Number";
		InChart2Number.SetChartNumber(1);

		InChart3Number.Name = "Chart 3 Number";
		InChart3Number.SetChartNumber(1);
		
		InChart1Multiplier.Name = "Chart 1 Multiplier";
		InChart1Multiplier.SetFloat(1.0f);
		
		InChart2Multiplier.Name = "Chart 2 Multiplier";
		InChart2Multiplier.SetFloat(1.0f);

		InChart3Multiplier.Name = "Chart 3 Multiplier";
		InChart3Multiplier.SetFloat(1.0f);

		return;
	}
	
	
	SCGraphData Chart2BaseData;
	sc.GetChartBaseData(-InChart2Number.GetChartNumber(), Chart2BaseData);
	
	SCGraphData Chart3BaseData;
	sc.GetChartBaseData(-InChart3Number.GetChartNumber(), Chart3BaseData);
	
	// Iterate through the bars
	for (int Index = sc.UpdateStartIndex; Index < sc.ArraySize; Index++)
	{
		int Chart2Index = sc.GetNearestMatchForDateTimeIndex(InChart2Number.GetChartNumber(), Index);
		int Chart3Index = sc.GetNearestMatchForDateTimeIndex(InChart3Number.GetChartNumber(), Index);
		
		// Iterate through the subgraphs
		for (int SubgraphIndex = SC_OPEN; SubgraphIndex <= SC_NT; SubgraphIndex++)
		{
			sc.Subgraph[SubgraphIndex][Index]
				= (sc.BaseDataIn[SubgraphIndex][Index] * InChart1Multiplier.GetFloat())
				+ (Chart2BaseData[SubgraphIndex][Chart2Index] * InChart2Multiplier.GetFloat())
				- (Chart3BaseData[SubgraphIndex][Chart3Index] * InChart3Multiplier.GetFloat())
				;
		}
		
		sc.Subgraph[SC_HIGH][Index]
			= max(
				sc.Subgraph[SC_OPEN][Index],
				max(
					sc.Subgraph[SC_HIGH][Index],
					max(
						sc.Subgraph[SC_LOW][Index],
						sc.Subgraph[SC_LAST][Index]
					)
				)
			);
		
		sc.Subgraph[SC_LOW][Index]
			= min(
				sc.Subgraph[SC_OPEN][Index],
				min(
					sc.Subgraph[SC_HIGH][Index],
					min(
						sc.Subgraph[SC_LOW][Index],
						sc.Subgraph[SC_LAST][Index]
					)
				)
			);
		
		sc.CalculateOHLCAverages(Index);
	}
	
	
	SCString Chart1Name = sc.GetChartName(sc.ChartNumber);
	SCString Chart2Name = sc.GetChartName(InChart2Number.GetChartNumber());
	SCString Chart3Name = sc.GetChartName(InChart3Number.GetChartNumber());
	sc.GraphName.Format("Spread - 3 Chart: %s + %s - %s", Chart1Name.GetChars(), Chart2Name.GetChars(), Chart3Name.GetChars());
}

/*==========================================================================*/
SCSFExport scsf_SpreadButterfly(SCStudyInterfaceRef sc)
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


	SCInputRef UseLatestSourceDataForLastBar = sc.Input[2];
	SCInputRef InChart2Number = sc.Input[3];
	SCInputRef InChart3Number = sc.Input[4];
	SCInputRef InChart1Multiplier = sc.Input[5];
	SCInputRef InChart2Multiplier = sc.Input[6];
	SCInputRef InChart3Multiplier = sc.Input[7];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Spread - Butterfly";

		
		sc.ValueFormat = 2;
		sc.GraphRegion = 1;

		sc.UseGlobalChartColors = 0;
		sc.GraphDrawType		= GDT_OHLCBAR;
		sc.StandardChartHeader	= 1;

		Open.Name = "Open";
		Open.DrawStyle = DRAWSTYLE_LINE;
		Open.PrimaryColor = RGB(0,255,0);
		Open.SecondaryColor = RGB(0,255,0);

		High.Name = "High";
		High.DrawStyle = DRAWSTYLE_LINE;
		High.PrimaryColor = RGB(0,255,0);
		High.SecondaryColor = RGB(0,255,0);

		Low.Name = "Low";
		Low.DrawStyle = DRAWSTYLE_LINE;
		Low.PrimaryColor = RGB(0,255,0);
		Low.SecondaryColor = RGB(0,255,0);

		Last.Name = "Last";
		Last.DrawStyle = DRAWSTYLE_LINE;
		Last.PrimaryColor = RGB(0,255,0);
		Last.SecondaryColor = RGB(0,255,0);

		Volume.Name = "Volume";
		Volume.DrawStyle = DRAWSTYLE_IGNORE;
		Volume.PrimaryColor = RGB(255,0,0);

		OpenInterest.Name = "# of Trades / OI";
		OpenInterest.DrawStyle = DRAWSTYLE_IGNORE;
		OpenInterest.PrimaryColor = RGB(0,0,255);

		OHLCAvg.Name = "OHLC Avg";
		OHLCAvg.DrawStyle = DRAWSTYLE_IGNORE;
		OHLCAvg.PrimaryColor = RGB(0,255,0);

		HLCAvg.Name = "HLC Avg";
		HLCAvg.DrawStyle = DRAWSTYLE_IGNORE;
		HLCAvg.PrimaryColor = RGB(0,255,255);

		HLAvg.Name = "HL Avg";
		HLAvg.DrawStyle = DRAWSTYLE_IGNORE;
		HLAvg.PrimaryColor = RGB(0,127,255);

		BidVol.Name = "Bid Vol";
		BidVol.DrawStyle = DRAWSTYLE_IGNORE;
		BidVol.PrimaryColor = RGB(0,255,0);

		AskVol.Name = "Ask Vol";
		AskVol.DrawStyle = DRAWSTYLE_IGNORE;
		AskVol.PrimaryColor = RGB(0,255,0);


		UseLatestSourceDataForLastBar.Name = "Use Latest Source Data For Last Bar";
		UseLatestSourceDataForLastBar.SetYesNo(0);

		InChart2Number.Name = "Chart 2 Number";
		InChart2Number.SetChartNumber(1);

		InChart3Number.Name = "Chart 3 Number";
		InChart3Number.SetChartNumber(1);

		InChart1Multiplier.Name = "Chart 1 Multiplier";
		InChart1Multiplier.SetFloat(1.0f);

		InChart2Multiplier.Name = "Chart 2 Multiplier";
		InChart2Multiplier.SetFloat(2.0f);

		InChart3Multiplier.Name = "Chart 3 Multiplier";
		InChart3Multiplier.SetFloat(1.0f);

		return;
	}


	//Formula: Buy1 Sell2 Buy1

	SCGraphData Chart2BaseData;
	sc.GetChartBaseData(-InChart2Number.GetChartNumber(), Chart2BaseData);
	
	SCGraphData Chart3BaseData;
	sc.GetChartBaseData(-InChart3Number.GetChartNumber(), Chart3BaseData);
	
	// Iterate through the bars
	for (int Index = sc.UpdateStartIndex; Index < sc.ArraySize; Index++)
	{
		int Chart2Index = sc.GetNearestMatchForDateTimeIndex(InChart2Number.GetChartNumber(), Index);
		int Chart3Index = sc.GetNearestMatchForDateTimeIndex(InChart3Number.GetChartNumber(), Index);



		//When use latest source data for last bar is set to Yes, replay is not running and at last bar in the destination chart, then use the data from the very latest bar in the source charts.
		if(UseLatestSourceDataForLastBar.GetYesNo() && !sc.IsReplayRunning() &&  Index == sc.ArraySize - 1)
		{
			Chart2Index = Chart2BaseData[SC_OPEN].GetArraySize() - 1;
			Chart3Index = Chart3BaseData[SC_OPEN].GetArraySize() - 1;
		}

		
		// Iterate through the subgraphs
		for (int SubgraphIndex = SC_OPEN; SubgraphIndex <= SC_NT; SubgraphIndex++)
		{
			sc.Subgraph[SubgraphIndex][Index]
				= (sc.BaseDataIn[SubgraphIndex][Index] * InChart1Multiplier.GetFloat())
				- (Chart2BaseData[SubgraphIndex][Chart2Index] * InChart2Multiplier.GetFloat())
				+ (Chart3BaseData[SubgraphIndex][Chart3Index] * InChart3Multiplier.GetFloat())
				;
		}
		
		sc.Subgraph[SC_HIGH][Index]
			= max(
				sc.Subgraph[SC_OPEN][Index],
				max(
					sc.Subgraph[SC_HIGH][Index],
					max(
						sc.Subgraph[SC_LOW][Index],
						sc.Subgraph[SC_LAST][Index]
					)
				)
			);
		
		sc.Subgraph[SC_LOW][Index]
			= min(
				sc.Subgraph[SC_OPEN][Index],
				min(
					sc.Subgraph[SC_HIGH][Index],
					min(
						sc.Subgraph[SC_LOW][Index],
						sc.Subgraph[SC_LAST][Index]
					)
				)
			);
		
		sc.CalculateOHLCAverages(Index);
	}



	sc.GraphName.Format("Butterfly Spread: +%s - %s + %s", 
		sc.GetChartName(sc.ChartNumber).GetChars(), 
		sc.GetChartName(InChart2Number.GetChartNumber()).GetChars(), 
		sc.GetChartName(InChart3Number.GetChartNumber()).GetChars());
}

/*==========================================================================*/
SCSFExport scsf_HighestHighLowestLowOverNBars(SCStudyInterfaceRef sc)
{
	SCSubgraphRef HighestHigh = sc.Subgraph[0];
	SCSubgraphRef LowestLow = sc.Subgraph[1];

	SCInputRef Length = sc.Input[3];
	SCInputRef InputDataHigh = sc.Input[5];
	SCInputRef InputDataLow = sc.Input[6];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Highest High / Lowest Low Over N Bars";

		sc.GraphRegion = 0;
		sc.AutoLoop = 1;

		HighestHigh.Name= "Highest High";
		HighestHigh.DrawStyle = DRAWSTYLE_LINE;
		HighestHigh.PrimaryColor = RGB(0,255,0);
		HighestHigh.DrawZeros = true;

		LowestLow.Name = "Lowest Low";
		LowestLow.DrawStyle = DRAWSTYLE_LINE;
		LowestLow.PrimaryColor = RGB(255,0,255);
		LowestLow.DrawZeros = true;

		Length.Name = "Length";
		Length.SetInt(5);
		Length.SetIntLimits(1, MAX_STUDY_LENGTH);

		InputDataHigh.Name = "Input Data High";
		InputDataHigh.SetInputDataIndex(SC_HIGH);

		InputDataLow.Name = "Input Data Low";
		InputDataLow.SetInputDataIndex(SC_LOW);

		return;
	}

	sc.DataStartIndex = Length.GetInt()  - 1;

	if (sc.Index < sc.DataStartIndex)
		return;

	double Lowest = FLT_MAX;
	double Highest = -FLT_MAX;
	for(int BarIndex = sc.Index - (Length.GetInt() - 1); BarIndex <= sc.Index; BarIndex++)
	{
		if(Highest < sc.BaseDataIn[InputDataHigh.GetInputDataIndex()][BarIndex])
			Highest = sc.BaseDataIn[InputDataHigh.GetInputDataIndex()][BarIndex];

		if(Lowest > sc.BaseDataIn[InputDataLow.GetInputDataIndex()][BarIndex])
			Lowest = sc.BaseDataIn[InputDataLow.GetInputDataIndex()][BarIndex];
	}

	HighestHigh[sc.Index] = (float)Highest;
	LowestLow[sc.Index] = (float)Lowest;
}

/*==========================================================================*/
SCSFExport scsf_DonchianChannel(SCStudyInterfaceRef sc)
{
	SCSubgraphRef HighestHigh = sc.Subgraph[0];
	SCSubgraphRef LowestLow = sc.Subgraph[1];
	SCSubgraphRef Midline = sc.Subgraph[2];
	SCInputRef Length = sc.Input[3];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Donchian Channel";

		sc.GraphRegion = 0;
		sc.AutoLoop = true;


		HighestHigh.Name= "Highest High";
		HighestHigh.DrawStyle=  DRAWSTYLE_LINE;
		HighestHigh.DrawZeros= true;
		HighestHigh.GraphicalDisplacement= 1;


		LowestLow.Name = "Lowest Low";
		LowestLow.DrawStyle=  DRAWSTYLE_LINE;
		LowestLow.DrawZeros= true;
		LowestLow.GraphicalDisplacement= 1;

		Midline.Name = "Mid Line";
		Midline.DrawStyle=  DRAWSTYLE_LINE;
		Midline.LineStyle= LINESTYLE_DOT;
		Midline.DrawZeros= true;
		Midline.GraphicalDisplacement= 1;

		Length.Name = "Length ";
		Length.SetInt(5);
		Length.SetIntLimits(1, MAX_STUDY_LENGTH);

		return;
	}

	float Lowest = FLT_MAX;
	float Highest = -FLT_MAX;
	for(int BarIndex = sc.Index - (Length.GetInt() - 1); BarIndex<= sc.Index; BarIndex++)
	{
		if(Highest < sc.High[BarIndex])
			Highest = sc.High[BarIndex];

		if(Lowest > sc.Low[BarIndex])
			Lowest = sc.Low[BarIndex];
	}

	HighestHigh[sc.Index] = Highest;
	LowestLow[sc.Index] = Lowest;
	Midline[sc.Index] = (Highest + Lowest)/2.0f;
}

/*==========================================================================*/
SCSFExport scsf_McClellanOscillator(SCStudyInterfaceRef sc)
{
	SCSubgraphRef MO = sc.Subgraph[0];
	SCSubgraphRef Temp2 = sc.Subgraph[2];
	SCSubgraphRef Temp3 = sc.Subgraph[3];

	SCInputRef DecliningIssuesChartNumber = sc.Input[3];
	SCInputRef UseAbsValue = sc.Input[4];
	
	if (sc.SetDefaults)
	{
		sc.GraphName = "McClellan Oscillator" ;
		sc.GraphRegion = 1;
		sc.ValueFormat = 3;

		MO.Name = "MO";
		MO.DrawStyle = DRAWSTYLE_LINE;
		MO.PrimaryColor = RGB(0,255,0);
		MO.DrawZeros = true;

		DecliningIssuesChartNumber.Name = "Declining Issues Chart #";
		DecliningIssuesChartNumber.SetChartNumber(1);

		UseAbsValue.Name = "Use ABS Value";
		UseAbsValue.SetYesNo(0);

		return;
	}
	
	SCGraphData ChartBaseData;
	sc.GetChartBaseData(-DecliningIssuesChartNumber.GetChartNumber(), ChartBaseData);

	SCFloatArrayRef Chart2Array = ChartBaseData[SC_LAST];
	
	if (Chart2Array.GetArraySize() == 0)
		return;

	int i1 = 0;
	int i = 0;
	sc.DataStartIndex = 1;

	Temp3[i] = Temp2[i] = sc.Close[i] - Chart2Array[i1];
	
	for (i = max(1, sc.UpdateStartIndex); i < sc.ArraySize; i++)
	{
		int i1 = sc.GetNearestMatchForDateTimeIndex(DecliningIssuesChartNumber.GetChartNumber(), i);

		Temp3[i] = Temp3[i - 1] * 0.9f +
			((sc.Close[i] - Chart2Array[i1]) * 0.1f);

		if (UseAbsValue.GetYesNo())
		{
			Temp2[i] = Temp2[i - 1] * 0.95f +
				(fabs(
				(sc.Close[i] - Chart2Array[i1]) *0.05f));
		}
		else
		{
			Temp2[i] = Temp2[i - 1] * 0.95f +
				((
				(sc.Close[i] - Chart2Array[i1]) *0.05f));
		}

		MO[i] = Temp3[i] - Temp2[i];
	}
}

/*==========================================================================*/
SCSFExport scsf_BarDifference(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Difference = sc.Subgraph[0];
	SCInputRef InputData1 = sc.Input[0];
	SCInputRef InputData2 = sc.Input[1];
	SCInputRef InputData2Offset = sc.Input[3];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Bar Difference";

		sc.GraphRegion = 1;
		sc.ValueFormat = 2;
		sc.AutoLoop = 1;

		Difference.Name = "Difference";
		Difference.DrawStyle = DRAWSTYLE_LINE;
		Difference.PrimaryColor = RGB(0,255,0);
		Difference.DrawZeros = true;

		InputData1.Name = "Input Data 1";
		InputData1.SetInputDataIndex(SC_HIGH);

		InputData2.Name = "Input Data 2";
		InputData2.SetInputDataIndex(SC_LOW);

		InputData2Offset.Name = "Input Data 2 Offset";
		InputData2Offset.SetInt(0);
		InputData2Offset.SetIntLimits(0, 100);

		return;
	}

	if (sc.Index < InputData2Offset.GetInt())
		return;

	Difference[sc.Index] = sc.BaseDataIn[InputData1.GetInputDataIndex()][sc.Index] -
		sc.BaseDataIn[InputData2.GetInputDataIndex()][sc.Index - InputData2Offset.GetInt()];
}

/*==========================================================================*/
SCSFExport scsf_McClellanSummationIndex(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Sum = sc.Subgraph[0];
	SCSubgraphRef Temp2 = sc.Subgraph[2];
	SCSubgraphRef Temp3 = sc.Subgraph[3];

	SCInputRef DecliningIssuesChartNumber = sc.Input[3];

	if (sc.SetDefaults)
	{
		sc.GraphName = "McClellan Summation Index";

		sc.GraphRegion = 1;
		sc.ValueFormat = 0;


		Sum.Name = "Sum";
		Sum.DrawStyle = DRAWSTYLE_LINE;
		Sum.PrimaryColor = RGB(0,255,0);
		Sum.DrawZeros = true;

		DecliningIssuesChartNumber.Name = "Declining Issues Chart #";
		DecliningIssuesChartNumber.SetChartNumber(1);

		return;
	}

	SCGraphData ChartBaseData;
	sc.GetChartBaseData(-DecliningIssuesChartNumber.GetChartNumber(), ChartBaseData);
	
	SCFloatArrayRef Chart2Array = ChartBaseData[SC_LAST];
	if (Chart2Array.GetArraySize() == 0)
		return;

	sc.DataStartIndex = 1;

	Temp3[0] = Temp2[0] = sc.Close[0] - Chart2Array[0];

	for (int i = max(1, sc.UpdateStartIndex); i < sc.ArraySize; i++)
	{
		int i1 = sc.GetNearestMatchForDateTimeIndex(DecliningIssuesChartNumber.GetChartNumber(), i);

		if (sc.Close[i] == 0 || Chart2Array[i1] == 0)
		{
			Sum[i] = Sum[i - 1];
			continue;
		}

		Temp3[i] = Temp3[i - 1] * 0.9f +
			((sc.Close[i] - Chart2Array[i1]) * 0.1f);

		Temp2[i] = Temp2[i - 1] * 0.95f +
			((
			(sc.Close[i] - Chart2Array[i1]) * 0.05f));

		Sum[i] = Sum[i - 1] + (Temp3[i] - Temp2[i]);
	}
}

/*==========================================================================*/
SCSFExport scsf_MomentumTrend(SCStudyInterfaceRef sc)
{
	SCSubgraphRef MomentumUp = sc.Subgraph[0];
	SCSubgraphRef MomentumDown = sc.Subgraph[1];
	SCSubgraphRef Temp2 = sc.Subgraph[2];

	SCInputRef InputData = sc.Input[0];
	SCInputRef Length = sc.Input[3];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Momentum Trend";

		sc.GraphRegion = 0;
		sc.ValueFormat = 2;
		sc.AutoLoop = 1;

		MomentumUp.Name = "Momentum Up";
		MomentumUp.DrawStyle = DRAWSTYLE_POINT;
		MomentumUp.PrimaryColor = RGB(0,255,0);
		MomentumUp.LineWidth = 5;
		MomentumUp.DrawZeros = false;

		MomentumDown.Name = "Momentum Down";
		MomentumDown.DrawStyle = DRAWSTYLE_POINT;
		MomentumDown.PrimaryColor = RGB(255,0,255);
		MomentumDown.LineWidth = 5;
		MomentumDown.DrawZeros = false;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		Length.Name = "Length";
		Length.SetInt(10);
		Length.SetIntLimits(1, MAX_STUDY_LENGTH);

		return;
	}

	sc.DataStartIndex = Length.GetInt() + 1;

	if (sc.Index < sc.DataStartIndex - 1)
		return;

	Temp2[sc.Index] = sc.BaseDataIn[InputData.GetInputDataIndex()][sc.Index] / sc.BaseDataIn[InputData.GetInputDataIndex()][sc.Index - Length.GetInt()] * 100.0f;

	if (sc.Index < sc.DataStartIndex)
		return;

	if (Temp2[sc.Index] > Temp2[sc.Index - 1])
	{
		MomentumUp[sc.Index] = sc.High[sc.Index];
		MomentumDown[sc.Index] = 0;
	}
	else
	{
		MomentumDown[sc.Index] = sc.Low[sc.Index];
		MomentumUp[sc.Index] = 0;
	}
}

/*==========================================================================*/
SCSFExport scsf_SumAllCharts(SCStudyInterfaceRef sc)
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
		sc.GraphName = "Sum All Charts";
		

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
		
		return;
	}
	
	// Initialize elements to 0
	for (int Index = sc.UpdateStartIndex; Index < sc.ArraySize; Index++)
	{
		for (int SubgraphIndex = SC_OPEN; SubgraphIndex <= SC_NT; SubgraphIndex++)
		{
			sc.Subgraph[SubgraphIndex][Index] = 0;
		}
	}
	
	// Sum charts
	const int MaxCharts = 200;
	for (int ChartIndex = 1; ChartIndex <= MaxCharts; ChartIndex++)
	{
		SCGraphData RefChartBaseData;
		sc.GetChartBaseData(-ChartIndex, RefChartBaseData);
		
		if (RefChartBaseData[SC_OPEN].GetArraySize() == 0)
			continue;
		
		for (int Index = sc.UpdateStartIndex; Index < sc.ArraySize; Index++)
		{
			int RefChartIndex = sc.GetNearestMatchForDateTimeIndex(ChartIndex, Index);
			
			for (int SubgraphIndex = SC_OPEN; SubgraphIndex <= SC_NT; SubgraphIndex++)
			{
				sc.Subgraph[SubgraphIndex][Index] += RefChartBaseData[SubgraphIndex][RefChartIndex];
			}
			
			sc.Subgraph[SC_HIGH][Index]
				= max(sc.Subgraph[SC_OPEN][Index],
					max(sc.Subgraph[SC_HIGH][Index],
						max(sc.Subgraph[SC_LOW][Index], sc.Subgraph[SC_LAST][Index])
					)
				);
			
			sc.Subgraph[SC_LOW][Index]
				= min(sc.Subgraph[SC_OPEN][Index],
					min(sc.Subgraph[SC_HIGH][Index],
						min(sc.Subgraph[SC_LOW][Index], sc.Subgraph[SC_LAST][Index])
					)
				);
		}
	}
	
	// Calculate averages
	for (int Index = sc.UpdateStartIndex; Index < sc.ArraySize; Index++)
		sc.CalculateOHLCAverages(Index);
}

/*==========================================================================*/
SCSFExport scsf_Kurtosis(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Kurtosis1 = sc.Subgraph[0];
	SCSubgraphRef Kurtosis2 = sc.Subgraph[1];
	SCSubgraphRef Temp2 = sc.Subgraph[2];
	SCSubgraphRef Temp3 = sc.Subgraph[3];
	SCSubgraphRef Temp4 = sc.Subgraph[4];
	SCSubgraphRef Temp5 = sc.Subgraph[5];


	SCInputRef InputData = sc.Input[0];
	SCInputRef Weight = sc.Input[2];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Kurtosis";

		sc.GraphRegion = 1;
		sc.ValueFormat = 4;
		sc.AutoLoop = 1;

		Kurtosis1.Name = "Kurtosis";
		Kurtosis1.DrawStyle = DRAWSTYLE_BAR;
		Kurtosis1.PrimaryColor = RGB(0,255,0);
		Kurtosis1.DrawZeros = false;

		Kurtosis2.Name = "Kurtosis";
		Kurtosis2.DrawStyle = DRAWSTYLE_BAR;
		Kurtosis2.PrimaryColor = RGB(255,0,0);
		Kurtosis2.DrawZeros = false;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		Weight.Name = "Weight";
		Weight.SetFloat(0.8f);
		Weight.SetFloatLimits(0.0f, 1.0f);

		return;
	}

	Temp2[0] = sc.BaseDataIn[InputData.GetInputDataIndex()][0];

	if (sc.Index == 0)
		return;

	double k = 0.0;
	double k1 = 0.0;
	double p = sc.BaseDataIn[InputData.GetInputDataIndex()][sc.Index];
	double w = Weight.GetFloat();

	Temp2[sc.Index] = (float)(w * p + (1.0 - w) * Temp2[sc.Index - 1]);

	Temp3[sc.Index] = (float)(w * (p - sc.BaseDataIn[InputData.GetInputDataIndex()][sc.Index - 1]) / p +
								((1.0 - w) * Temp3[sc.Index - 1]));

	Temp4[sc.Index] = (float)(w * (Temp3[sc.Index] - Temp3[sc.Index - 1]) +
		((1.0 - w) * Temp4[sc.Index - 1]));

	Temp5[sc.Index] = (float)(w * (Temp4[sc.Index] - Temp4[sc.Index - 1]) +
		((1.0 - w) * Temp5[sc.Index - 1]));

	k = w * (Temp5[sc.Index] - Temp5[sc.Index - 1]) + ((1.0 - w) * max(Kurtosis1[sc.Index - 1], Kurtosis2[sc.Index - 1]));

	if (Kurtosis1[sc.Index - 1] == 0)
		k1 = Kurtosis2[sc.Index - 1];
	else
		k1 = Kurtosis1[sc.Index - 1];

	if (k1 >= k)
	{
		Kurtosis2[sc.Index] = (float)k;
		Kurtosis1[sc.Index] = (float)k;
	}
	else
	{
		Kurtosis2[sc.Index] = 0.0f;
		Kurtosis1[sc.Index] = (float)k;
	}
}

/*==========================================================================*/
SCSFExport scsf_AC_DC_Histogram(SCStudyInterfaceRef sc)
{
	SCSubgraphRef ACDC = sc.Subgraph[0];
	SCSubgraphRef Temp3 = sc.Subgraph[3];
	SCSubgraphRef Temp4 = sc.Subgraph[4];
	SCSubgraphRef Temp5 = sc.Subgraph[5];
	SCSubgraphRef Temp6 = sc.Subgraph[6];
	SCSubgraphRef Temp7 = sc.Subgraph[7];

	SCInputRef InputData = sc.Input[0];	
	SCInputRef MA1Length = sc.Input[2];
	SCInputRef MA2Length = sc.Input[3];
	SCInputRef MA3Length = sc.Input[4];
	SCInputRef MA4Length = sc.Input[5];

	if (sc.SetDefaults)
	{
		sc.GraphName = "AC/DC Histogram";
		
		sc.GraphRegion = 1;
		sc.ValueFormat = 3;
		sc.AutoLoop = 1;
		sc.FreeDLL = 0;

		ACDC.Name = "AC/DC";
		ACDC.DrawStyle = DRAWSTYLE_BAR;
		ACDC.SecondaryColor = RGB(255,0,0);
		ACDC.AutoColoring = AUTOCOLOR_SLOPE;
		ACDC.DrawZeros = false;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		MA1Length.Name = "Moving Average 1 Length";
		MA1Length.SetInt(34);
		MA1Length.SetIntLimits(1, MAX_STUDY_LENGTH);

		MA2Length.Name = "Moving Average 2 Length";
		MA2Length.SetInt(5);
		MA2Length.SetIntLimits(1, MAX_STUDY_LENGTH);

		MA3Length.Name = "Moving Average 3 Length";
		MA3Length.SetInt(34);
		MA3Length.SetIntLimits(1, MAX_STUDY_LENGTH);

		MA4Length.Name = "Moving Average 4 Length";
		MA4Length.SetInt(5);
		MA4Length.SetIntLimits(1, MAX_STUDY_LENGTH);

		return;
	}

	sc.DataStartIndex = max(MA1Length.GetInt(), MA2Length.GetInt()) + MA3Length.GetInt() + MA4Length.GetInt();

	sc.SimpleMovAvg(sc.BaseDataIn[InputData.GetInputDataIndex()], Temp3, MA1Length.GetInt());
	sc.SimpleMovAvg(sc.BaseDataIn[InputData.GetInputDataIndex()], Temp4, MA2Length.GetInt());

	if (sc.Index > 0)
		Temp5[sc.Index] = Temp4[sc.Index] - Temp3[sc.Index];
	
	sc.SimpleMovAvg(Temp5, Temp6, MA4Length.GetInt());
	Temp7[sc.Index]  = Temp5[sc.Index] - Temp6[sc.Index];
	sc.SimpleMovAvg(Temp7, ACDC, MA3Length.GetInt());
}

/*==========================================================================*/
SCSFExport scsf_MovingAverageTriangular(SCStudyInterfaceRef sc)
{
	SCSubgraphRef MA = sc.Subgraph[0];
	SCSubgraphRef TempMA = sc.Subgraph[1];

	SCInputRef InputData = sc.Input[0];
	SCInputRef Length = sc.Input[3];
	SCInputRef Unused = sc.Input[4];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Moving Average-Triangular";

		sc.GraphRegion = 0;
		sc.ValueFormat = 2;
		sc.AutoLoop = 1;

		MA.Name = "MA";
		MA.DrawStyle = DRAWSTYLE_LINE;
		MA.PrimaryColor = RGB(0,255,0);
		MA.DrawZeros = true;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		Length.Name = "Length";
		Length.SetInt(9);
		Length.SetIntLimits(1, MAX_STUDY_LENGTH);

		return;
	}

	sc.DataStartIndex = (long)(Length.GetInt() + Unused.GetFloat()) - 1 + 2;

	int n0 = 0;
	int n1 = 0;

	if(Length.GetInt() % 2)
	{
		n0 = n1 = Length.GetInt() / 2 + 1;
	}
	else
	{
		n0 = Length.GetInt() / 2;
		n1 = n0 + 1;
	}

	sc.SimpleMovAvg(sc.BaseDataIn[InputData.GetInputDataIndex()], TempMA, n0);
	sc.SimpleMovAvg(TempMA, MA, n1);
}

/*==========================================================================*/
SCSFExport scsf_1_p(SCStudyInterfaceRef sc)
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
		sc.GraphName = "1 Divided by Price";

		sc.GraphUsesChartColors= true;
		
		sc.ValueFormat = VALUEFORMAT_INHERITED;
		sc.GraphRegion = 0;
		sc.DisplayAsMainPriceGraph= true;
		sc.AutoLoop = 1;
		sc.FreeDLL = 0;
		sc.GraphDrawType = GDT_OHLCBAR;
		sc.StandardChartHeader = 1;

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

		OpenInterest.Name = "Num Trades / OpenInterest";
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

		return;
	}

		
		if (sc.BaseDataIn[SC_OPEN][sc.Index] == 0.0f)
			sc.Subgraph[SC_OPEN][sc.Index] = 0.0f;
		else
			sc.Subgraph[SC_OPEN][sc.Index] = 1.0f / sc.BaseDataIn[SC_OPEN][sc.Index];

		if (sc.BaseDataIn[SC_HIGH][sc.Index] == 0.0f)
			sc.Subgraph[SC_LOW][sc.Index] = 0.0f;
		else
			sc.Subgraph[SC_LOW][sc.Index] = 1.0f / sc.BaseDataIn[SC_HIGH][sc.Index];

		if (sc.BaseDataIn[SC_LOW][sc.Index] == 0.0f)
			sc.Subgraph[SC_HIGH][sc.Index] = 0.0f;
		else
			sc.Subgraph[SC_HIGH][sc.Index] = 1.0f / sc.BaseDataIn[SC_LOW][sc.Index];

		if (sc.BaseDataIn[SC_LAST][sc.Index] == 0.0f)
			sc.Subgraph[SC_LAST][sc.Index] = 0.0f;
		else
			sc.Subgraph[SC_LAST][sc.Index] = 1.0f / sc.BaseDataIn[SC_LAST][sc.Index];


		sc.Subgraph[SC_VOLUME][sc.Index] =sc.BaseDataIn[SC_VOLUME][sc.Index];
		sc.Subgraph[SC_NT][sc.Index] =sc.BaseDataIn[SC_NT][sc.Index];

		if (sc.BaseDataIn[SC_OHLC][sc.Index] == 0.0f)
			sc.Subgraph[SC_OHLC][sc.Index] = 0.0f;
		else
			sc.Subgraph[SC_OHLC][sc.Index] = 1.0f / sc.BaseDataIn[SC_OHLC][sc.Index];

		if (sc.BaseDataIn[SC_HLC][sc.Index] == 0.0f)
			sc.Subgraph[SC_HLC][sc.Index] = 0.0f;
		else
			sc.Subgraph[SC_HLC][sc.Index] = 1.0f / sc.BaseDataIn[SC_HLC][sc.Index];

		if (sc.BaseDataIn[SC_HL][sc.Index] == 0.0f)
			sc.Subgraph[SC_HL][sc.Index] = 0.0f;
		else
			sc.Subgraph[SC_HL][sc.Index] = 1.0f / sc.BaseDataIn[SC_HL][sc.Index];


		sc.Subgraph[SC_ASKVOL][sc.Index] = sc.BaseDataIn[SC_ASKVOL][sc.Index];
		sc.Subgraph[SC_BIDVOL][sc.Index] = sc.BaseDataIn[SC_BIDVOL][sc.Index];



	SCString ChartName = sc.GetStudyNameFromChart(sc.ChartNumber, 0);
	sc.GraphName.Format("1/P %s", ChartName.GetChars());
}

/*==========================================================================*/

SCSFExport scsf_RoundPriceBarsToTickSize(SCStudyInterfaceRef sc)
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
		sc.GraphName = "Round Price Bars To Tick Size";

		sc.StudyDescription= "This study when applied to a chart will round the bar values to the Tick Size set in Chart >> Chart Settings.";

		sc.GraphUsesChartColors= true;

		sc.ValueFormat = VALUEFORMAT_INHERITED;
		sc.GraphRegion = 0;
		sc.DisplayAsMainPriceGraph= true;
		sc.AutoLoop = 1;
		sc.FreeDLL = 0;
		sc.GraphDrawType = GDT_OHLCBAR;
		sc.StandardChartHeader = 1;

		Open.Name = "Open";
		Open.DrawStyle = DRAWSTYLE_LINE;
		Open.PrimaryColor = RGB(255, 255,255);
		
		High.Name = "High";
		High.DrawStyle = DRAWSTYLE_LINE;
		High.PrimaryColor = RGB(255, 255,255);
		
		Low.Name = "Low";
		Low.DrawStyle = DRAWSTYLE_LINE;
		Low.PrimaryColor = RGB(255, 255,255);
		
		Last.Name = "Last";
		Last.DrawStyle = DRAWSTYLE_LINE;
		Last.PrimaryColor = RGB(255, 255,255);

		Volume.Name = "Volume";
		Volume.DrawStyle = DRAWSTYLE_IGNORE;
		Volume.PrimaryColor = RGB(255,0,0);

		OpenInterest.Name = "Num Trades / OpenInterest";
		OpenInterest.DrawStyle = DRAWSTYLE_IGNORE;
		OpenInterest.PrimaryColor = RGB(0,0,255);

		OHLCAvg.Name = "OHLC Avg";
		OHLCAvg.DrawStyle = DRAWSTYLE_IGNORE;
		OHLCAvg.PrimaryColor = RGB(0,255,0);

		HLCAvg.Name = "HLC Avg";
		HLCAvg.DrawStyle = DRAWSTYLE_IGNORE;
		HLCAvg.PrimaryColor = RGB(0,255,255);

		HLAvg.Name = "HL Avg";
		HLAvg.DrawStyle = DRAWSTYLE_IGNORE;
		HLAvg.PrimaryColor = RGB(0,127,255);

		BidVol.Name = "Bid Vol";
		BidVol.DrawStyle = DRAWSTYLE_IGNORE;
		BidVol.PrimaryColor = RGB(0,255,0);

		AskVol.Name = "Ask Vol";
		AskVol.DrawStyle = DRAWSTYLE_IGNORE;
		AskVol.PrimaryColor = RGB(0,255,0);

		return;
	}


	
		sc.Subgraph[SC_OPEN][sc.Index] = sc.RoundToTickSize(sc.BaseDataIn[SC_OPEN][sc.Index] , sc.TickSize);
		sc.Subgraph[SC_HIGH][sc.Index] = sc.RoundToTickSize(sc.BaseDataIn[SC_HIGH][sc.Index] , sc.TickSize);
		sc.Subgraph[SC_LOW][sc.Index] = sc.RoundToTickSize(sc.BaseDataIn[SC_LOW][sc.Index] , sc.TickSize);
		sc.Subgraph[SC_LAST][sc.Index] = sc.RoundToTickSize(sc.BaseDataIn[SC_LAST][sc.Index] , sc.TickSize);


		for(int SubgraphIndex =SC_VOLUME; SubgraphIndex <=SC_ASKVOL;SubgraphIndex++)
			sc.Subgraph[SubgraphIndex][sc.Index] =sc.BaseDataIn[SubgraphIndex][sc.Index];




	
	sc.GraphName= sc.GetStudyNameFromChart(sc.ChartNumber, 0);
}

/*==========================================================================*/

SCSFExport scsf_DifferenceSingleLine(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Difference = sc.Subgraph[0];

	SCInputRef InputData = sc.Input[0];
	SCInputRef UseLatestSourceDataForLastBar = sc.Input[1];
	SCInputRef Chart2Number = sc.Input[3];
	SCInputRef Chart1Multiplier = sc.Input[4];
	SCInputRef Chart2Multiplier = sc.Input[5];
	SCInputRef Chart1Addition = sc.Input[6];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Difference (Single Line)";

		sc.ValueFormat = 2;
		sc.GraphRegion = 1;

		Difference.Name = "Difference";
		Difference.DrawStyle = DRAWSTYLE_LINE;
		Difference.PrimaryColor = RGB(0,255,0);
		Difference.DrawZeros = false;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);
		
		UseLatestSourceDataForLastBar.Name = "Use Latest Source Data For Last Bar";
		UseLatestSourceDataForLastBar.SetYesNo(0);

		Chart2Number.Name = "Chart 2 Number";
		Chart2Number.SetChartNumber(1);

		Chart1Multiplier.Name = "Chart 1 Multiplier";
		Chart1Multiplier.SetFloat(1.0f);

		Chart2Multiplier.Name = "Chart 2 Multiplier";
		Chart2Multiplier.SetFloat(1.0f);

		Chart1Addition.Name = "Chart 1 Addition";
		Chart1Addition.SetFloat(0.0f);

		return;
	}

	SCFloatArray Chart2Array;
	sc.GetChartArray(Chart2Number.GetChartNumber()*-1, InputData.GetInputDataIndex(), Chart2Array); 

	if (Chart2Array.GetArraySize() == 0)
		return;

	sc.DataStartIndex = 0;

	for (int DestinationIndex = sc.UpdateStartIndex; DestinationIndex < sc.ArraySize; DestinationIndex++)
	{
		int Chart2Index = sc.GetNearestMatchForDateTimeIndex(Chart2Number.GetChartNumber(), DestinationIndex);


		//When use latest source data for last bar is set to Yes, replay is not running and at last bar in the destination chart, then use the data from the very latest bar in the source chart.
		if(UseLatestSourceDataForLastBar.GetYesNo() && !sc.IsReplayRunning() && DestinationIndex == sc.ArraySize - 1)
			Chart2Index = Chart2Array.GetArraySize() - 1;

		Difference[DestinationIndex] = (sc.BaseDataIn[InputData.GetInputDataIndex()][DestinationIndex] * Chart1Multiplier.GetFloat() + Chart1Addition.GetFloat()) -
			(Chart2Array[Chart2Index] * Chart2Multiplier.GetFloat());
	}

	SCString Chart1Name = sc.GetStudyNameFromChart(sc.ChartNumber, 0);
	SCString Chart2Name = sc.GetStudyNameFromChart(Chart2Number.GetChartNumber(), 0);
	sc.GraphName.Format("Diff %s - %s", Chart1Name.GetChars(), Chart2Name.GetChars());
}
/*==========================================================================*/

SCSFExport scsf_DifferenceNetChange(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Difference = sc.Subgraph[0];
	SCInputRef Chart2Number = sc.Input[3];


	if (sc.SetDefaults)
	{
		sc.GraphName = "Difference of Net Change";

		sc.ValueFormat = 2;
		sc.GraphRegion = 1;

		Difference.Name = "Diff Net Change";
		Difference.DrawStyle = DRAWSTYLE_LINE;
		Difference.PrimaryColor = RGB(0,255,0);
		Difference.DrawZeros = false;


		Chart2Number.Name = "Chart 2 Number";
		Chart2Number.SetChartNumber(1);


	
		return;
	}

	SCFloatArray Chart2Array;
	sc.GetChartArray(Chart2Number.GetChartNumber()*-1, SC_CLOSE, Chart2Array); 

	if (Chart2Array.GetArraySize() == 0)
		return;

	sc.DataStartIndex = 1;

	for (int i = max(1, sc.UpdateStartIndex); i < sc.ArraySize; i++)
	{
		int Chart2Index = sc.GetNearestMatchForDateTimeIndex(Chart2Number.GetChartNumber(), i);

		Difference[i] = (sc.BaseDataIn[SC_CLOSE][i] - sc.BaseDataIn[SC_CLOSE][i - 1] ) - ( Chart2Array[Chart2Index] - Chart2Array[Chart2Index - 1]);
	}

	SCString Chart1Name = sc.GetStudyNameFromChart(sc.ChartNumber, 0);
	SCString Chart2Name = sc.GetStudyNameFromChart(Chart2Number.GetChartNumber(), 0);
	sc.GraphName.Format("Diff Net Change %s - %s", Chart1Name.GetChars(), Chart2Name.GetChars());
}

/*==========================================================================*/
SCSFExport scsf_RatioSingleLine(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Ratio = sc.Subgraph[0];
	SCInputRef InputData = sc.Input[0];
	SCInputRef Chart2Number = sc.Input[3];
	SCInputRef Chart1Multiplier = sc.Input[4];
	SCInputRef Chart2Multiplier = sc.Input[5];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Ratio - Single Line";

		
		sc.ValueFormat = 2;
		sc.GraphRegion = 1;

		Ratio.Name = "Ratio";
		Ratio.DrawStyle = DRAWSTYLE_LINE;
		Ratio.PrimaryColor = RGB(0,255,0);
		Ratio.DrawZeros = false;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		Chart2Number.Name = "Chart 2 Number";
		Chart2Number.SetChartNumber(1);

		Chart1Multiplier.Name = "Chart 1 Multiplier";
		Chart1Multiplier.SetFloat(1.0f);

		Chart2Multiplier.Name = "Chart 2 Multiplier";
		Chart2Multiplier.SetFloat(1.0f);
		
		return;
	}
	
	
	SCFloatArray Chart2Array;
	sc.GetChartArray(Chart2Number.GetChartNumber()*-1, InputData.GetInputDataIndex(), Chart2Array); 

	if (Chart2Array.GetArraySize() == 0)
		return;

	for (int i = sc.UpdateStartIndex; i < sc.ArraySize; i++)
	{
		int i1 = sc.GetNearestMatchForDateTimeIndex(Chart2Number.GetChartNumber(), i);

		if (sc.BaseDataIn[InputData.GetInputDataIndex()][i] == 0 || Chart2Array[i1] == 0)
		{
			Ratio[i] = 0;
			continue;
		}

		Ratio[i] = (sc.BaseDataIn[InputData.GetInputDataIndex()][i] * Chart1Multiplier.GetFloat())
					/ (Chart2Array[i1] * Chart2Multiplier.GetFloat());
	}


	SCString Chart1Name = sc.GetStudyNameFromChart(sc.ChartNumber, 0);
	SCString Chart2Name = sc.GetStudyNameFromChart(Chart2Number.GetChartNumber(), 0);
	sc.GraphName.Format("Ratio %s / %s", Chart1Name.GetChars(), Chart2Name.GetChars());
}

/*==========================================================================*/
SCSFExport scsf_OverlayBar(SCStudyInterfaceRef sc)
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
	SCSubgraphRef Unused11 = sc.Subgraph[11];
	SCSubgraphRef Unused12 = sc.Subgraph[12];

	SCInputRef ChartOverlayNumber = sc.Input[3];
	SCInputRef Multiplier = sc.Input[4];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Overlay (Bar)";

		sc.UseGlobalChartColors = 0;
		
		sc.ValueFormat = 2;
		sc.GraphRegion =1;
		sc.ScaleRangeType = SCALE_INDEPENDENT;
		sc.GraphDrawType = GDT_OHLCBAR;
		sc.StandardChartHeader = 1;

		Open.Name = "Open";
		Open.DrawStyle = DRAWSTYLE_LINE;
		Open.PrimaryColor = RGB(0,255,0);
		Open.SecondaryColor = RGB(0,255,0);
		Open.SecondaryColorUsed = 1;
		Open.DrawZeros = false;

		High.Name = "High";
		High.DrawStyle = DRAWSTYLE_LINE;
		High.PrimaryColor = RGB(128,255,128);
		High.DrawZeros = false;

		Low.Name = "Low";
		Low.DrawStyle = DRAWSTYLE_LINE;
		Low.PrimaryColor = RGB(255,0,0);
		Low.SecondaryColor = RGB(255,0,0);
		Low.SecondaryColorUsed = 1;
		Low.DrawZeros = false;

		Last.Name = "Last";
		Last.DrawStyle = DRAWSTYLE_LINE;
		Last.PrimaryColor = RGB(255,128,128);
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


		Unused11.DrawStyle = DRAWSTYLE_IGNORE;
		Unused12.DrawStyle = DRAWSTYLE_IGNORE;

		ChartOverlayNumber.Name = "Chart Number To Overlay";
		ChartOverlayNumber.SetChartNumber(1);

		Multiplier.Name = "Multiplier";
		Multiplier.SetFloat(1.0f);

		return;
	}
	
	
	SCGraphData ChartOverlayArrays;
	sc.GetChartBaseData(-ChartOverlayNumber.GetChartNumber(), ChartOverlayArrays);
	
	if (ChartOverlayArrays[SC_OPEN].GetArraySize() == 0)
		return;
	
	for (int DataIndex = sc.UpdateStartIndex; DataIndex < sc.ArraySize; DataIndex++)
	{
		int i1 = sc.GetNearestMatchForDateTimeIndex(ChartOverlayNumber.GetChartNumber(), DataIndex);
		
		for (int SubgraphIndex = 0; SubgraphIndex < ChartOverlayArrays.GetArraySize(); SubgraphIndex++)
			sc.Subgraph[SubgraphIndex][DataIndex] = ChartOverlayArrays[SubgraphIndex][i1] * Multiplier.GetFloat();
	}
	
	SCString ChartOverlayName = sc.GetStudyNameFromChart(ChartOverlayNumber.GetChartNumber(), 0);
	sc.GraphName.Format("%s Overlay", ChartOverlayName.GetChars());
}

/*==========================================================================*/
SCSFExport scsf_HorizontalLines(SCStudyInterfaceRef sc)
{
	const int NumberOfLines = 40;
	if (sc.SetDefaults)
	{
		sc.GraphName = "Horizontal Lines";

		sc.ValueFormat = 3;
		sc.AutoLoop = 1;
		sc.DisplayStudyInputValues = false;

		for (int i = 0; i < NumberOfLines; i++)
		{

			sc.Subgraph[i].Name.Format("Line%d", i + 1);
			sc.Subgraph[i].DrawStyle = DRAWSTYLE_IGNORE;
			sc.Subgraph[i].PrimaryColor = RGB(0,255,0);
			sc.Subgraph[i].DrawZeros = true;
			
				
			sc.Input[i].Name.Format("Line%d Value", i + 1);
			sc.Input[i].SetFloat(1.0f);
			sc.Input[i].SetFloatLimits(-FLT_MAX, FLT_MAX);
		}

		sc.Subgraph[0].DrawStyle = DRAWSTYLE_LINE;

		return;
	}

	for (int SubgraphIndex = 0; SubgraphIndex < NumberOfLines; SubgraphIndex++)
	{
		//Only do this on the first iteration for efficiency
		if (sc.Index == 0)
			sc.Input[SubgraphIndex].Name.Format("%s Value",sc.Subgraph[SubgraphIndex].Name); 

		sc.Subgraph[SubgraphIndex][sc.Index] = sc.Input[SubgraphIndex].GetFloat();
	}
}

/*==========================================================================*/
SCSFExport scsf_RMI(SCStudyInterfaceRef sc)
{
	SCSubgraphRef RMI = sc.Subgraph[0];
	SCSubgraphRef Line1 = sc.Subgraph[1];
	SCSubgraphRef Line2 = sc.Subgraph[2];
	SCSubgraphRef Midline = sc.Subgraph[3];

	SCInputRef InputData = sc.Input[0];
	SCInputRef RMILength = sc.Input[3];
	SCInputRef RMI_MALength = sc.Input[4];
	SCInputRef RMI_MAType = sc.Input[5];
	SCInputRef Line1Value = sc.Input[6];
	SCInputRef Line2Value = sc.Input[7];
	SCInputRef MidlineValue = sc.Input[8];

	SCFloatArrayRef UpTempArray = RMI.Arrays[0];
	SCFloatArrayRef DownTempArray = RMI.Arrays[1];
	SCFloatArrayRef SmoothedUpTempArray = RMI.Arrays[2];
	SCFloatArrayRef SmoothedDownTempArray = RMI.Arrays[3];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Relative Momentum Index";

		sc.GraphRegion = 1;
		sc.ValueFormat = 2;
		sc.AutoLoop = 1;

		RMI.Name = "RMI";
		RMI.DrawStyle = DRAWSTYLE_LINE;
		RMI.PrimaryColor = RGB(0, 255, 0);
		RMI.DrawZeros = true;

		Line1.Name = "Overbought";
		Line1.DrawStyle = DRAWSTYLE_LINE;
		Line1.PrimaryColor = RGB(128, 128, 128);
		Line1.DrawZeros = true;

		Line2.Name = "Oversold";
		Line2.DrawStyle = DRAWSTYLE_LINE;
		Line2.PrimaryColor = RGB(128, 128, 128);
		Line2.DrawZeros = true;

		Midline.Name = "Midline";
		Midline.DrawStyle = DRAWSTYLE_LINE;
		Midline.PrimaryColor = RGB(128, 128, 128);
		Midline.DrawZeros = true;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		RMILength.Name = "RMI Length";
		RMILength.SetInt(5);
		RMILength.SetIntLimits(1, MAX_STUDY_LENGTH);

		RMI_MALength.Name = "RMI Moving Average Length";
		RMI_MALength.SetInt(9);
		RMI_MALength.SetIntLimits(1, MAX_STUDY_LENGTH);

		RMI_MAType.Name = "RMI Moving Average Type";
		RMI_MAType.SetMovAvgType(MOVAVGTYPE_SIMPLE);

		Line1Value.Name = "Overbought Value";
		Line1Value.SetFloat(70.0f);
		

		Line2Value.Name = "Oversold Value";
		Line2Value.SetFloat(30.0f);

		MidlineValue.Name = "Midline Value";
		MidlineValue.SetFloat(50.0f);
		

		return;
	}

	sc.DataStartIndex = max(RMILength.GetInt(), RMI_MALength.GetInt());

	float previousValue = sc.BaseDataIn[InputData.GetInputDataIndex()][sc.Index - RMILength.GetInt()];
	float currentValue = sc.BaseDataIn[InputData.GetInputDataIndex()][sc.Index];

	if (currentValue > previousValue) 
	{
		UpTempArray[sc.Index] = currentValue - previousValue;
		DownTempArray[sc.Index] = 0.0f;
	}
	else
	{
		UpTempArray[sc.Index] = 0;
		DownTempArray[sc.Index] = previousValue - currentValue;
	}

	sc.MovingAverage(UpTempArray, SmoothedUpTempArray, RMI_MAType.GetMovAvgType(), RMI_MALength.GetInt());
	sc.MovingAverage(DownTempArray, SmoothedDownTempArray, RMI_MAType.GetMovAvgType(), RMI_MALength.GetInt());

	if (SmoothedUpTempArray[sc.Index] + SmoothedDownTempArray[sc.Index] != 0.0f)
	{
		RMI[sc.Index] = 100 * SmoothedUpTempArray[sc.Index] / (SmoothedUpTempArray[sc.Index] + SmoothedDownTempArray[sc.Index]);
	}
	else
	{
		RMI[sc.Index] = RMI[sc.Index - 1];
	}

	Line1[sc.Index] = Line1Value.GetFloat();
	Line2[sc.Index] = Line2Value.GetFloat();
	Midline[sc.Index] = MidlineValue.GetFloat();
}

/*==========================================================================*/
SCSFExport scsf_OverlayNonSync(SCStudyInterfaceRef sc)
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
	SCSubgraphRef Unused11 = sc.Subgraph[11];
	SCSubgraphRef Unused12 = sc.Subgraph[12];

	SCInputRef NumberOfBars = sc.Input[2];
	SCInputRef ChartOverlayNumber = sc.Input[3];
	SCInputRef Multiplier = sc.Input[4];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Overlay (Non-Sync)";

		sc.UseGlobalChartColors = 0;
		
		sc.ValueFormat = 2;
		sc.GraphRegion = 0;
		sc.ScaleRangeType = SCALE_INDEPENDENT;
		sc.GraphDrawType = GDT_OHLCBAR;
		sc.StandardChartHeader = 1;

		Open.Name = "Open";
		Open.DrawStyle = DRAWSTYLE_LINE;
		Open.PrimaryColor = RGB(0,255,0);
		Open.SecondaryColor = RGB(0,255,0);
		Open.SecondaryColorUsed = 1;
		Open.DrawZeros = false;

		High.Name = "High";
		High.DrawStyle = DRAWSTYLE_LINE;
		High.PrimaryColor = RGB(128,255,128);
		High.DrawZeros = false;

		Low.Name = "Low";
		Low.DrawStyle = DRAWSTYLE_LINE;
		Low.PrimaryColor = RGB(255,0,0);
		Low.SecondaryColor = RGB(255,0,0);
		Low.SecondaryColorUsed = 1;
		Low.DrawZeros = false;

		Last.Name = "Last";
		Last.DrawStyle = DRAWSTYLE_LINE;
		Last.PrimaryColor = RGB(255,128,128);
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


		Unused11.DrawStyle = DRAWSTYLE_IGNORE;
		Unused12.DrawStyle = DRAWSTYLE_IGNORE;

		NumberOfBars.Name = "Number of bars to Overlay";
		NumberOfBars.SetInt(500);
		NumberOfBars.SetIntLimits(1,MAX_STUDY_LENGTH);

		ChartOverlayNumber.Name = "Chart Number To Overlay";
		ChartOverlayNumber.SetChartNumber(1);

		Multiplier.Name = "Multiplier";
		Multiplier.SetFloat(1.0f);

		return;
	}

	SCGraphData ChartOverlayArrays;
	sc.GetChartBaseData(ChartOverlayNumber.GetChartNumber(), ChartOverlayArrays);
	
	int OverlayArraySize = ChartOverlayArrays[0].GetArraySize();
	if (OverlayArraySize == 0)
		return;
	
	for (int Index = sc.ArraySize - 1; (Index >= sc.ArraySize - NumberOfBars.GetInt()) && (Index >= 0); Index--)
	{
		OverlayArraySize--;
		for (int i2 = 0; i2 < ChartOverlayArrays.GetArraySize(); i2++)
		{
			if (OverlayArraySize >= 0)
			{
				sc.Subgraph[i2][Index] = ChartOverlayArrays[i2][OverlayArraySize] * Multiplier.GetFloat();
			}
			else
				sc.Subgraph[i2][Index] = 0.0f;
		}
	}

	for (int Index = sc.ArraySize - NumberOfBars.GetInt() - 1; (Index >= sc.ArraySize - sc.UpdateStartIndex) && (Index >= 0); Index--)
	{
		for (int i2 = 0; i2 < ChartOverlayArrays.GetArraySize(); i2++)
			sc.Subgraph[i2][Index] = 0.0f;
	}

	SCString ChartOverlayName = sc.GetStudyNameFromChart(ChartOverlayNumber.GetChartNumber(), 0);
	sc.GraphName.Format("%s Overlay", ChartOverlayName.GetChars());
}

/*==========================================================================*/
SCSFExport scsf_OnBalanceOpenInterestShortTerm(SCStudyInterfaceRef sc)
{
	SCSubgraphRef OBOI = sc.Subgraph[0];
	SCSubgraphRef Temp = sc.Subgraph[1];
	SCInputRef Length = sc.Input[0];

	if (sc.SetDefaults)
	{
		sc.GraphName = "On Balance Open Interest - Short Term";

		sc.ValueFormat = 0;
		sc.AutoLoop = 1;

		OBOI.Name = "OBOI";
		OBOI.PrimaryColor = RGB(0,255,0);
		OBOI.DrawStyle= DRAWSTYLE_LINE;
		OBOI.DrawZeros= true;

		Length.Name = "Length";
		Length.SetInt(10);
		Length.SetIntLimits(1, MAX_STUDY_LENGTH);
		
		return;
	}


	sc.DataStartIndex= Length.GetInt();
	if (sc.Index < 1)
		return;

	if(sc.Close[sc.Index ] > sc.Close[sc.Index-1])
	{
		Temp[sc.Index] = sc.OpenInterest[sc.Index];
	}
	else if(sc.Close[sc.Index ] < sc.Close[sc.Index-1])
	{
		Temp[sc.Index] = -sc.OpenInterest[sc.Index];
	}
	else
	{
		Temp[sc.Index] = 0.0f;
	}

	if(sc.Index < Length.GetInt())
	{
		OBOI[sc.Index] = OBOI[sc.Index - 1] + Temp[sc.Index];
	}
	else
	{
		OBOI[sc.Index] = OBOI[sc.Index - 1] + Temp[sc.Index] - Temp[sc.Index - Length.GetInt()];
	}
}

/*==========================================================================*/
SCSFExport scsf_Ergodic(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Erg = sc.Subgraph[0];
	SCSubgraphRef ErgSignalLine = sc.Subgraph[1];
	SCSubgraphRef ErgOsc = sc.Subgraph[2];
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
		sc.GraphName = "Ergodic";

		sc.GraphRegion	= 1;
		sc.ValueFormat	= 2;
		sc.AutoLoop		= 1; 

		Erg.Name = "Erg";
		Erg.DrawStyle = DRAWSTYLE_LINE;
		Erg.PrimaryColor = RGB(0,255,0);
		Erg.DrawZeros = true;

		ErgSignalLine.Name = "Erg Signal Line";
		ErgSignalLine.DrawStyle = DRAWSTYLE_IGNORE;
		ErgSignalLine.PrimaryColor = RGB(255,0,255);
		ErgSignalLine.DrawZeros = true;

		ErgOsc.Name = "Erg Osc";
		ErgOsc.DrawStyle = DRAWSTYLE_IGNORE;
		ErgOsc.PrimaryColor = RGB(255,255,0);
		ErgOsc.DrawZeros = true;

		
		

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
			Erg[sc.Index] = Multiplier.GetInt() * Temp5[sc.Index]/Temp8[sc.Index];
		else
			Erg[sc.Index] = 0;
	}

	// Calculate the Signal Line ( EMA[TSI] )
	if(sc.Index >= SignalStart)
		sc.MovingAverage(Erg, ErgSignalLine, MOVAVGTYPE_EXPONENTIAL, SignalLineMALength.GetInt());

	// Calculate the Oscillator (TSI - EMA[TSI])
	if(sc.Index >= sc.DataStartIndex)
		ErgOsc[sc.Index] = Erg[sc.Index] - ErgSignalLine[sc.Index];
}

/*==========================================================================*/
SCSFExport scsf_MovingAverageHull(SCStudyInterfaceRef sc)
{
	SCSubgraphRef HullMA = sc.Subgraph[0];

	SCInputRef InputData = sc.Input[0];
	SCInputRef HullMALength = sc.Input[1];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Moving Average - Hull";

		sc.GraphRegion = 0;
		sc.AutoLoop = 1;

		HullMA.Name ="Hull MA";
		HullMA.DrawStyle = DRAWSTYLE_LINE;
		HullMA.PrimaryColor = RGB(0,255,0);
		HullMA.DrawZeros = true;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		HullMALength.Name = "Hull Moving Average Length";
		HullMALength.SetInt(25);
		HullMALength.SetIntLimits(1, MAX_STUDY_LENGTH);

		return;
	}

	int SqrtLength = (int)(sc.Round(sqrt((float)HullMALength.GetInt())));
	sc.DataStartIndex = HullMALength.GetInt() + SqrtLength - 1;

	sc.WeightedMovingAverage(sc.BaseDataIn[InputData.GetInputDataIndex()], sc.Subgraph[0].Arrays[0], HullMALength.GetInt() / 2);
	sc.WeightedMovingAverage(sc.BaseDataIn[InputData.GetInputDataIndex()], sc.Subgraph[0].Arrays[1], HullMALength.GetInt());
	
	sc.Subgraph[0].Arrays[2][sc.Index] = 2 * sc.Subgraph[0].Arrays[0][sc.Index] - sc.Subgraph[0].Arrays[1][sc.Index];
	
	sc.WeightedMovingAverage(sc.Subgraph[0].Arrays[2], HullMA, SqrtLength);

}

/*==========================================================================*/
SCSFExport scsf_AverageOfTwoSubgraphs(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Average = sc.Subgraph[0];

	SCInputRef InputStudy1 = sc.Input[2];
	SCInputRef Study1Subgraph = sc.Input[3];
	SCInputRef InputStudy2 = sc.Input[4];
	SCInputRef Study2Subgraph = sc.Input[5];
	SCInputRef VersionCheckInput = sc.Input[6];
	SCInputRef StudySubgraph1 = sc.Input[7];
	SCInputRef StudySubgraph2 = sc.Input[8];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Study Subgraphs Average";//Average of 2 Subgraphs

		sc.ValueFormat = 3;
		sc.AutoLoop = 1;

		Average.Name = "Average";
		Average.DrawStyle = DRAWSTYLE_LINE;
		Average.PrimaryColor = RGB(0, 255,0 );
		Average.DrawZeros = false;

		InputStudy1.SetStudyID(0);
	
		InputStudy2.SetStudyID(0);


		Study1Subgraph.SetSubgraphIndex(0);
		Study2Subgraph.SetSubgraphIndex(0);

		StudySubgraph1.Name = "Study Subgraph Reference 1";
		StudySubgraph1.SetStudySubgraphValues(0,0);

		StudySubgraph2.Name = "Study Subgraph Reference 2";
		StudySubgraph2.SetStudySubgraphValues(0,0);

		VersionCheckInput.SetInt(2);

		sc.CalculationPrecedence = LOW_PREC_LEVEL;

		return;
	}

	if (VersionCheckInput.GetInt() < 2)
	{


		StudySubgraph1.SetStudySubgraphValues(InputStudy1.GetStudyID(), StudySubgraph1.GetSubgraphIndex());
		StudySubgraph2.SetStudySubgraphValues(InputStudy2.GetStudyID(), StudySubgraph2.GetSubgraphIndex());


		VersionCheckInput.SetInt(2);
	}

	SCFloatArray Study1Array;
	sc.GetStudyArrayUsingID(StudySubgraph1.GetStudyID(), StudySubgraph1.GetSubgraphIndex(), Study1Array);
	SCFloatArray Study2Array;
	sc.GetStudyArrayUsingID(StudySubgraph2.GetStudyID(), StudySubgraph2.GetSubgraphIndex(), Study2Array);

	Average[sc.Index] = (Study1Array[sc.Index] + Study2Array[sc.Index]) / 2.0f;
}



/*==========================================================================*/
SCSFExport scsf_NarrowRangeBar(SCStudyInterfaceRef sc)
{
	SCSubgraphRef NarrowRange = sc.Subgraph[0];
	SCInputRef NumberOfBars = sc.Input[0];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Narrow Range Bar";

		sc.GraphRegion = 0;
		sc.AutoLoop = 1;

		NarrowRange.Name = "Narrow Range";
		NarrowRange.DrawStyle = DRAWSTYLE_COLORBAR;
		NarrowRange.PrimaryColor = RGB(0, 0, 255);
		NarrowRange.DrawZeros = false;

		NumberOfBars.Name = "Number of Bars to Compare";
		NumberOfBars.SetInt(5);
		NumberOfBars.SetIntLimits(0, MAX_STUDY_LENGTH);

		return;
	}

	int LookBackBars =  NumberOfBars.GetInt();
	float isLowest = 1.0f;
	float RangeOfCurrentBar = sc.High[sc.Index] - sc.Low[sc.Index];

	for (int i = 1; i <= LookBackBars; i++)
	{
		float PriorRange = sc.High[sc.Index - i] - sc.Low[sc.Index - i];

		if (!sc.FormattedEvaluate(PriorRange, sc.BaseGraphValueFormat, GREATER_EQUAL_OPERATOR, RangeOfCurrentBar, sc.BaseGraphValueFormat) )
		{
			isLowest = 0.0f;
			break;
		}
	}

	NarrowRange[sc.Index] = isLowest;
}

/*==========================================================================*/
SCSFExport scsf_WideRangeBar(SCStudyInterfaceRef sc)
{
	SCSubgraphRef WideRange = sc.Subgraph[0];
	SCInputRef NumberOfBars = sc.Input[0];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Wide Range Bar";

		sc.GraphRegion = 0;
		sc.AutoLoop = 1;

		WideRange.Name = "Wide Range";
		WideRange.DrawStyle = DRAWSTYLE_COLORBAR;
		WideRange.PrimaryColor = RGB(128, 128, 255);
		WideRange.DrawZeros = false;

		NumberOfBars.Name = "Number of Bars to Compare";
		NumberOfBars.SetInt(5);
		NumberOfBars.SetIntLimits(0, MAX_STUDY_LENGTH);

		return;
	}

	int LookBackBars =  NumberOfBars.GetInt();
	float isHighest = 1.0f;
	float RangeOfCurrentBar = sc.High[sc.Index] - sc.Low[sc.Index];

	for (int i = 1; i <= LookBackBars; i++)
	{
		float PriorRange = sc.High[sc.Index - i] - sc.Low[sc.Index - i];

		if (!sc.FormattedEvaluate(PriorRange, sc.BaseGraphValueFormat, LESS_EQUAL_OPERATOR, RangeOfCurrentBar, sc.BaseGraphValueFormat) )
		{
			isHighest = 0.0f;
			break;
		}
	}

	WideRange[sc.Index] = isHighest;
}

/*==========================================================================*/
SCSFExport scsf_BidVolume(SCStudyInterfaceRef sc)
{
	SCSubgraphRef BidVol = sc.Subgraph[0];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Bid Volume";

		sc.ValueFormat = 0;
		sc.AutoLoop = 1;

		BidVol.Name = "Bid Vol";
		BidVol.DrawStyle = DRAWSTYLE_BAR;
		BidVol.PrimaryColor = RGB(0,255,0);
		BidVol.DrawZeros = true;

		
		return;
	}

	BidVol[sc.Index] = sc.BidVolume[sc.Index];
}

/*==========================================================================*/
SCSFExport scsf_AskVolume(SCStudyInterfaceRef sc)
{
	SCSubgraphRef AskVol = sc.Subgraph[0];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Ask Volume";

		sc.ValueFormat = 0;
		sc.AutoLoop = 1;

		AskVol.Name = "Ask Vol";
		AskVol.DrawStyle = DRAWSTYLE_BAR;
		AskVol.PrimaryColor = RGB(0,255,0);
		AskVol.DrawZeros = true;

		return;
	}

	AskVol[sc.Index] = sc.AskVolume[sc.Index];
}

/*==========================================================================*/
SCSFExport scsf_UpTickVolume(SCStudyInterfaceRef sc)
{
	SCSubgraphRef UpTickVol = sc.Subgraph[0];

	if (sc.SetDefaults)
	{
		sc.GraphName = "UpTick Volume";

		sc.ValueFormat = 0;
		sc.AutoLoop = 1;
		sc.MaintainAdditionalChartDataArrays = 1;

		UpTickVol.Name = "UpTick Volume";
		UpTickVol.DrawStyle = DRAWSTYLE_BAR;
		UpTickVol.PrimaryColor = RGB(0,255,0);
		UpTickVol.DrawZeros = true;

		return;
	}

	UpTickVol[sc.Index] = sc.UpTickVolume[sc.Index];
}

/*==========================================================================*/
SCSFExport scsf_DownTickVolume(SCStudyInterfaceRef sc)
{
	SCSubgraphRef DownTickVol = sc.Subgraph[0];

	if (sc.SetDefaults)
	{
		sc.GraphName = "DownTick Volume";

		sc.ValueFormat = 0;
		sc.AutoLoop = 1;
		sc.MaintainAdditionalChartDataArrays = 1;

		DownTickVol.Name = "DownTick Volume";
		DownTickVol.DrawStyle = DRAWSTYLE_BAR;
		DownTickVol.PrimaryColor = RGB(0,255,0);
		DownTickVol.DrawZeros = true;

		return;
	}

	DownTickVol[sc.Index] = sc.DownTickVolume[sc.Index];
}

/*==========================================================================*/
SCSFExport scsf_MaximumMinimumUpTickVolumeDowntickVolumeDiff(SCStudyInterfaceRef sc)
{
	SCSubgraphRef MinDifference = sc.Subgraph[0];
	SCSubgraphRef MaxDifference = sc.Subgraph[1];
	SCSubgraphRef MinBidAskVolumeDifference = sc.Subgraph[2];
	SCSubgraphRef MaxBidAskVolumeDifference = sc.Subgraph[3];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Maximum/Minimum UpTick Volume Downtick Volume Difference";

		sc.ValueFormat = 0;
		sc.AutoLoop = 1;
		sc.MaintainAdditionalChartDataArrays = 1;

		MinDifference.Name = "Minimum UpTick Volume Downtick Volume Difference";
		MinDifference.DrawStyle = DRAWSTYLE_BAR;
		MinDifference.PrimaryColor = RGB(0,255,0);
		MinDifference.DrawZeros = true;

		MaxDifference.Name = "Maximum UpTick Volume Downtick Volume Difference";
		MaxDifference.DrawStyle = DRAWSTYLE_BAR;
		MaxDifference.PrimaryColor = RGB(0,255, 255);
		MaxDifference.DrawZeros = true;

		MinBidAskVolumeDifference.Name = "Minimum Bid Ask Volume Difference";
		MinBidAskVolumeDifference.DrawStyle = DRAWSTYLE_BAR;
		MinBidAskVolumeDifference.PrimaryColor = RGB(255,255,0);
		MinBidAskVolumeDifference.DrawZeros = true;

		MaxBidAskVolumeDifference.Name = "Maximum Bid Ask Volume Difference";
		MaxBidAskVolumeDifference.DrawStyle = DRAWSTYLE_BAR;
		MaxBidAskVolumeDifference.PrimaryColor = RGB(255, 0, 0);
		MaxBidAskVolumeDifference.DrawZeros = true;

		return;
	}

	MinDifference[sc.Index] = sc.BaseDataIn[SC_UPDOWN_VOL_DIFF_LOW][sc.Index];
	MaxDifference[sc.Index] = sc.BaseDataIn[SC_UPDOWN_VOL_DIFF_HIGH][sc.Index];
	MinBidAskVolumeDifference[sc.Index] = sc.BaseDataIn[SC_ASKBID_DIFF_LOW][sc.Index];
	MaxBidAskVolumeDifference[sc.Index] = sc.BaseDataIn[SC_ASKBID_DIFF_HIGH][sc.Index];
}


/*==========================================================================*/
SCSFExport scsf_BidNT(SCStudyInterfaceRef sc)
{
	SCSubgraphRef BidNT = sc.Subgraph[0];
	
	if (sc.SetDefaults)
	{
		sc.GraphName = "Number of Trades - Bid";
		
		sc.ValueFormat = 0;
		sc.AutoLoop = 1;
		sc.MaintainAdditionalChartDataArrays = 1;

		BidNT.Name = "Number Of Trades - Bid";
		BidNT.DrawStyle = DRAWSTYLE_BAR;
		BidNT.PrimaryColor = RGB(0,255,0);
		BidNT.DrawZeros = true;
		
		return;
	}
	
	BidNT[sc.Index] = sc.NumberOfBidTrades[sc.Index];
}

/*==========================================================================*/
SCSFExport scsf_AskNT(SCStudyInterfaceRef sc)
{
	SCSubgraphRef AskNT = sc.Subgraph[0];
	
	if (sc.SetDefaults)
	{
		sc.GraphName = "Number of Trades - Ask";
		
		sc.ValueFormat = 0;
		sc.AutoLoop = 1;
		sc.MaintainAdditionalChartDataArrays = 1;

		AskNT.Name = "Number Of Trades - Ask";
		AskNT.DrawStyle = DRAWSTYLE_BAR;
		AskNT.PrimaryColor = RGB(0,255,0);
		AskNT.DrawZeros = true;
		
		return;
	}
	
	AskNT[sc.Index] = sc.NumberOfAskTrades[sc.Index];
}

/*==========================================================================*/
SCSFExport scsf_McClellanOscillator1Chart(SCStudyInterfaceRef sc)
{
	SCSubgraphRef MO = sc.Subgraph[0];
	SCSubgraphRef Temp2 = sc.Subgraph[2];
	SCSubgraphRef Temp3 = sc.Subgraph[3];

	SCInputRef UseAbsValue = sc.Input[0];

	if (sc.SetDefaults)
	{
		sc.GraphName = "McClellan Oscillator - 1 Chart" ;

		sc.GraphRegion = 1;
		sc.ValueFormat = 3;
		sc.AutoLoop = 1;

		MO.Name = "MO";
		MO.DrawStyle = DRAWSTYLE_LINE;
		MO.PrimaryColor = RGB(0,255,0);
		MO.DrawZeros = true;

		UseAbsValue.Name = "Use ABS Value.";
		UseAbsValue.SetYesNo(0);

		return;
	}

	sc.DataStartIndex = 1;

	Temp3[0] = Temp2[0] = sc.Close[0];

	if (sc.Index < sc.DataStartIndex)
		return;

	Temp3[sc.Index] = Temp3[sc.Index - 1] * 0.9f + sc.Close[sc.Index] * 0.1f;

	if (UseAbsValue.GetYesNo())
	{
		Temp2[sc.Index] = Temp2[sc.Index - 1] * 0.95f + fabs(sc.Close[sc.Index] * 0.05f);
	}
	else
	{
		Temp2[sc.Index] = Temp2[sc.Index - 1] * 0.95f + sc.Close[sc.Index] * 0.05f;
	}

	MO[sc.Index] = Temp3[sc.Index] - Temp2[sc.Index];
}

/*==========================================================================*/
SCSFExport scsf_McClellanSumIndex1Chart(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Sum = sc.Subgraph[0];
	SCSubgraphRef Temp2 = sc.Subgraph[2];
	SCSubgraphRef Temp3 = sc.Subgraph[3];

	if (sc.SetDefaults)
	{
		sc.GraphName = "McClellan Sum. Index - 1 Chart" ;

		sc.GraphRegion = 1;
		sc.ValueFormat = 0;
		sc.AutoLoop = 1;

		Sum.Name = "Sum";
		Sum.DrawStyle = DRAWSTYLE_LINE;
		Sum.PrimaryColor = RGB(0,255,0);
		Sum.DrawZeros = true;

		return;
	}

	sc.DataStartIndex = 1;

	Temp3[0] = Temp2[0] = sc.Close[0];

	if (sc.Index < sc.DataStartIndex)
		return;

	if (sc.Close[sc.Index] == 0)
	{
		Sum[sc.Index] = Sum[sc.Index - 1];
	}
	else
	{
		Temp3[sc.Index] = Temp3[sc.Index - 1] * 0.9f + sc.Close[sc.Index] * 0.1f;
		Temp2[sc.Index] = Temp2[sc.Index - 1] * 0.95f + sc.Close[sc.Index] * 0.05f;

		Sum[sc.Index] = Sum[sc.Index - 1] + (Temp3[sc.Index] - Temp2[sc.Index]);
	}
}

/*==========================================================================*/
SCSFExport scsf_CumulativeSumOfStudy(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Sum = sc.Subgraph[0];
	SCInputRef StudyReference = sc.Input[0];
	SCInputRef StudySubgraphNumber = sc.Input[1];
	SCInputRef ResetAtSessionStart = sc.Input[2];
	SCInputRef StudySubgraphReference = sc.Input[3];
	SCInputRef Version = sc.Input[4];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Cumulative Sum Of Study";
		sc.AutoLoop = 0;//Manual looping
		sc.GraphRegion = 1;
		sc.CalculationPrecedence = LOW_PREC_LEVEL;

		Sum.Name = "Sum";
		Sum.PrimaryColor = RGB(0,255,0);
		Sum.DrawStyle = DRAWSTYLE_LINE;
		Sum.LineWidth = 2;
		Sum.DrawZeros = false;


		StudySubgraphReference.Name = "Study and Subgraph To Reference";
		StudySubgraphReference.SetStudySubgraphValues(1,0);

		ResetAtSessionStart.Name = "Reset at Start of Trading Day";
		ResetAtSessionStart.SetYesNo(0);

		Version.SetInt(2);

		return;
	}

	if(Version.GetInt() < 2)
	{
		Version.SetInt(2);

		StudySubgraphReference.SetStudySubgraphValues(StudyReference.GetStudyID(), StudySubgraphNumber.GetSubgraphIndex());

	}

	SCFloatArray StudyArray;
	sc.GetStudyArrayUsingID(StudySubgraphReference.GetStudyID(), StudySubgraphReference.GetSubgraphIndex(), StudyArray);
	if(StudyArray.GetArraySize() == 0)
	{
		sc.AddMessageToLog("Subgraph array being referenced is empty.", 1);
		return;
	}

	sc.DataStartIndex = sc.GetStudyDataStartIndexUsingID(StudySubgraphReference.GetStudyID());

	if (StudyArray.GetArraySize() != sc.ArraySize)
	{
		sc.AddMessageToLog("Subgraph array being referenced is not of the correct array size. Check the settings for the 'Study and Subgraph To Reference' input.", 1);
		return;
	}

	SCDateTime NextSessionStart = sc.GetTradingDayStartDateTimeOfBar(sc.BaseDateTimeIn[sc.UpdateStartIndex - 1]) + 1*DAYS;

	if (sc.UpdateStartIndex == 0)
	{
		Sum[0] = StudyArray[0];
	}

	for (int Index = max(sc.UpdateStartIndex, 1); Index < sc.ArraySize; Index++)
	{
		Sum[Index] = Sum[Index - 1] + StudyArray[Index];

		if (ResetAtSessionStart.GetYesNo() != 0)
		{
			SCDateTime IndexDateTime = sc.BaseDateTimeIn[Index];

			if (IndexDateTime >= NextSessionStart)
			{
				Sum[Index] = StudyArray[Index];

				NextSessionStart = sc.GetTradingDayStartDateTimeOfBar(IndexDateTime) + 1*DAYS;
			}
		}
	}
}

/*==========================================================================*/
SCSFExport scsf_PercentChangeSinceOpen(SCStudyInterfaceRef sc)
{
	SCSubgraphRef PercentChange = sc.Subgraph[0];
	SCSubgraphRef ZeroLine = sc.Subgraph[1];
	SCInputRef StartTime = sc.Input[0];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Percent Change Since Open";
		sc.AutoLoop = 1;
		sc.GraphRegion = 1;
		sc.ValueFormat = 2;

		PercentChange.Name = "Percent Change";
		PercentChange.PrimaryColor = RGB(0,255,0);
		PercentChange.DrawStyle = DRAWSTYLE_LINE;
		PercentChange.LineWidth = 1;
		PercentChange.DrawZeros= true;

		ZeroLine.  Name= "Zero Line";
		ZeroLine.PrimaryColor = RGB(255,255,0);
		ZeroLine.DrawStyle = DRAWSTYLE_LINE;
		ZeroLine.LineWidth = 1;
		ZeroLine.DrawZeros= true;

		StartTime.Name = "Start Time";
		StartTime.SetCustomInputStrings("Trading Day Start Time;First Bar In Chart");
		StartTime.SetCustomInputIndex(0);

		return;
	}

	float&		OpenValue    = sc.PersistVars->f1;
	SCDateTime& SessionStart = sc.PersistVars->scdt1;

	if (sc.Index == 0)
	{
		OpenValue = sc.Open[0];
		SessionStart = sc.GetTradingDayStartDateTimeOfBar(sc.BaseDateTimeIn[0]);
	}

	SCDateTime BarSessionStart = sc.GetTradingDayStartDateTimeOfBar(sc.BaseDateTimeIn[sc.Index]);

	if (BarSessionStart != SessionStart)
	{
		if(StartTime.GetIndex() == 0)
			OpenValue = sc.Open[sc.Index];

		SessionStart = BarSessionStart;
	}

	if(OpenValue == 0)
		PercentChange[sc.Index] = 0;
	else
		PercentChange[sc.Index] = (sc.Close[sc.Index] - OpenValue) / OpenValue * 100.0f;
}

/*==========================================================================*/
double GetFormatStepValue(unsigned long Format)
{
	// Quick lookup return
	switch (Format)
	{
	case 356: return 0.00390625;  // 1/256
	case 228: return 0.0078125;  // 1/128
	case 164: return 0.015625;  // 1/64
		//case 132: return 0.03125;  // 1/32
	case 132: return 0.0078125;  // 1/32 is treated as 1/128
	case 116: return 0.0625;  // 1/16
	case 108: return 0.125;  // 1/8
	case 104: return 0.25;  // 1/4
	case 102: return 0.5;  // 1/2
	case 0: return 1.0; // 1
	case 1: return 0.1; //0.1
	case 2: return 0.01; // 0.01
	case 3: return 0.001; // 0.001
	case 4: return 0.0001; // 0.0001
	case 5: return 0.00001; // 0.00001
	case 6: return 0.000001; // 0.000001
	}

	// For anything else that wasn't in the lookup table
	if (Format > 100)
		return 1.0 / (double)(Format - 100);
	else if (Format < 20)
		return pow(10.0, -(double)Format);
	else
		return 0.0;
}


/*==========================================================================*/
double PointAndFigureAddBoxes(SCStudyInterfaceRef sc, double Value, double BoxSize, int Direction, unsigned long ValueFormat)
{
	double Epsilon = 0.5 * GetFormatStepValue(ValueFormat);

	if (Direction == 1) //up
	{
		int NumberOfBoxes = (int)floor(Value / BoxSize + Epsilon);
		return (NumberOfBoxes * BoxSize);
	}
	else if (Direction == 2) //down
	{
		int NumberOfBoxes = (int)ceil(Value / BoxSize - Epsilon);
		return (NumberOfBoxes * BoxSize);
	}
	else //unknown
		return sc.Round((float)(Value / BoxSize)) * BoxSize;

	return 0;
}
/**************************************************************************************/


SCSFExport scsf_PointAndFigureChart(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Open           = sc.Subgraph[SC_OPEN];
	SCSubgraphRef High           = sc.Subgraph[SC_HIGH];
	SCSubgraphRef Low            = sc.Subgraph[SC_LOW];
	SCSubgraphRef Last           = sc.Subgraph[SC_LAST];
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
	SCSubgraphRef NumberOfBoxes  = sc.Subgraph[PF_NUM_BOXES_ARRAY];
	SCSubgraphRef DirectionArray = sc.Subgraph[PF_DIRECTION_ARRAY];
	SCSubgraphRef TrueLast       = sc.Subgraph[PF_TRUELAST_ARRAY];

	const int ArrayCount = PF_TRUELAST_ARRAY + 1;

	SCInputRef BoxSize                    = sc.Input[0];
	SCInputRef ReversalSize               = sc.Input[1];
	SCInputRef AllowOneBoxReversals       = sc.Input[2];
	SCInputRef NewBarAtStartOfDay         = sc.Input[3];
	SCInputRef IgnoreLastBarUntilComplete = sc.Input[4];
	SCInputRef UsePercentageForBoxSize = sc.Input[5];
	SCInputRef BoxSizePercentage = sc.Input[6];
	SCInputRef CalculatedBoxSize = sc.Input[7];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Point & Figure Chart";

		sc.GraphRegion = 0;
		sc.StandardChartHeader = 1;
		sc.IsCustomChart = 1;
		sc.GraphDrawType = GDT_POINT_AND_FIGURE_XO;
		sc.MaintainAdditionalChartDataArrays = 1;
		sc.ValueFormat =VALUEFORMAT_INHERITED;

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
		Low.PrimaryColor = RGB(255,0,0);
		Low.DrawZeros = false;

		Last.Name = "Last";
		Last.DrawStyle = DRAWSTYLE_LINE;
		Last.PrimaryColor = RGB(255,0,0);
		Last.DrawZeros = false;

		Volume.Name = "Volume";
		Volume.DrawStyle = DRAWSTYLE_IGNORE;
		Volume.PrimaryColor = RGB(255,255,255);
		Volume.DrawZeros = false;

		NumTrades.Name = "# of Trades / OI";
		NumTrades.DrawStyle = DRAWSTYLE_IGNORE;
		NumTrades.PrimaryColor = RGB(255,255,255);
		NumTrades.DrawZeros = false;

		OHLCAvg.Name = "OHLC Avg";
		OHLCAvg.DrawStyle = DRAWSTYLE_IGNORE;
		OHLCAvg.PrimaryColor = RGB(255,255,255);
		OHLCAvg.DrawZeros = false;

		HLCAvg.Name = "HLC Avg";
		HLCAvg.DrawStyle = DRAWSTYLE_IGNORE;
		HLCAvg.PrimaryColor = RGB(255,255,255);
		HLCAvg.DrawZeros = false;

		HLAvg.Name = "HL Avg";
		HLAvg.DrawStyle = DRAWSTYLE_IGNORE;
		HLAvg.PrimaryColor = RGB(255,255,255);
		HLAvg.DrawZeros = false;

		BidVol.Name = "Bid Vol";
		BidVol.DrawStyle = DRAWSTYLE_IGNORE;
		BidVol.PrimaryColor = RGB(255,255,255);
		BidVol.DrawZeros = false;

		AskVol.Name = "Ask Vol";
		AskVol.DrawStyle = DRAWSTYLE_IGNORE;
		AskVol.PrimaryColor = RGB(255,255,255);
		AskVol.DrawZeros = false;

		NumberOfBoxes.Name = "Number of Boxes";
		NumberOfBoxes.DrawStyle = DRAWSTYLE_IGNORE;
		NumberOfBoxes.PrimaryColor = RGB(0xFF,0x99,0x00);
		NumberOfBoxes.DrawZeros = false;

		TrueLast.Name = "True Last";
		TrueLast.DrawStyle = DRAWSTYLE_IGNORE;
		TrueLast.PrimaryColor = RGB(255,255,255);
		TrueLast.DrawZeros = false;

		BoxSize.Name = "Box Size (Fixed)";
		BoxSize.SetFloat(0.0f);
		BoxSize.SetFloatLimits(0, FLT_MAX);

		ReversalSize.Name = "Reversal Size";
		ReversalSize.SetFloat(3.0f);
		ReversalSize.SetFloatLimits(FLT_MIN, FLT_MAX);

		AllowOneBoxReversals.Name = "Allow One Box Reversals";
		AllowOneBoxReversals.SetYesNo(1);
	
		NewBarAtStartOfDay.Name = "New Bar at Start of Day";
		NewBarAtStartOfDay.SetYesNo(0);

		IgnoreLastBarUntilComplete.Name = "Ignore Last Bar Until Completed";
		IgnoreLastBarUntilComplete.SetYesNo(0);

		UsePercentageForBoxSize.Name = "Use Percentage of Last Price for Box Size";
		UsePercentageForBoxSize.SetYesNo(0);

		BoxSizePercentage.Name = "Box Size Percentage";
		BoxSizePercentage.SetFloat(1.0f);
		BoxSizePercentage.SetFloatLimits(0.0001f,5000.0f);

		return;
	}


	struct s_PFStorage
	{
		int		PriorOutArraySize;
		float	PriorOutData[ArrayCount];
		float	High; 
		float	Low;
		int		Index;
		int BoxSizeSet;
	};

	s_PFStorage* p_PFStorage = (s_PFStorage*)sc.StorageBlock;

	SCDateTime& NextSessionStart = sc.PersistVars->scdt1;

	//Determine box size. 
	if(!p_PFStorage->BoxSizeSet && BoxSize.GetFloat() == 0.0f)
	{
		p_PFStorage->BoxSizeSet = 1;
		BoxSize.SetFloat(sc.TickSize*2);
	}
	

	float CurrentBoxSize = BoxSize.GetFloat();

	if(UsePercentageForBoxSize.GetYesNo())
		CurrentBoxSize = sc.BaseData[SC_LAST][sc.ArraySize-1] * BoxSizePercentage.GetFloat() * 0.01f;

	CurrentBoxSize = sc.RoundToTickSize(CurrentBoxSize, sc.TickSize );

	if (CurrentBoxSize == 0.0)
		CurrentBoxSize = 1;

	CalculatedBoxSize.SetFloat(CurrentBoxSize);


	int InputIndex = sc.UpdateStartIndex;
	int AvgStartIndex = 0;

	if (InputIndex == 0)
	{
		sc.ResizeArrays(0);

		AvgStartIndex = 0;

		// Clear the last state data
		memset(p_PFStorage, 0, sizeof(p_PFStorage));
		p_PFStorage->BoxSizeSet = 1;

		p_PFStorage->Index = -1;

		if (!sc.AddElements(1))
			return;

		sc.DateTimeOut[0] = sc.BaseDateTimeIn[0];
		
		Open[0] = sc.Open[0];
		High[0] = (float)sc.RoundToTickSize(sc.Close[0], CurrentBoxSize);//round off to the nearest box size
		Low[0] = (float)sc.RoundToTickSize(sc.Close[0], CurrentBoxSize);//round off to the nearest box size
		Last[0] = sc.Close[0];
		TrueLast[0] = sc.Close[0];

		NextSessionStart = sc.GetTradingDayStartDateTimeOfBar(sc.BaseDateTimeIn[0]) + 1*DAYS;


		if (sc.ChartDataType == DAILY_DATA)
			NumTrades.Name = "Open Interest";
		else
			NumTrades.Name = "# of Trades";

		sc.GraphName.Format("Point & Figure Chart %.6gx%g %s", 
			CurrentBoxSize, 
			ReversalSize.GetFloat(),
			sc.GetStudyNameFromChart(sc.ChartNumber, 0).GetChars());
	}
	else if (!IgnoreLastBarUntilComplete.GetYesNo())
	{	
		AvgStartIndex = p_PFStorage->PriorOutArraySize - 1;

		// Restore array to size just before processing last input bar (sc.BaseDataIn) and restore state of last output bar.
		sc.ResizeArrays(p_PFStorage->PriorOutArraySize);

		for (int SubgraphIndex = 0; SubgraphIndex < ArrayCount; ++SubgraphIndex)
		{
			sc.Subgraph[SubgraphIndex][AvgStartIndex] = p_PFStorage->PriorOutData[SubgraphIndex];
		}
	}

	int OutputIndex = sc.DateTimeOut.GetArraySize() - 1;
	if (InputIndex == 0)
		InputIndex = 1;

	for (; InputIndex < sc.ArraySize; ++InputIndex)
	{
		bool NewBar = false;

		if (IgnoreLastBarUntilComplete.GetYesNo() && sc.GetBarHasClosedStatus(InputIndex) == BHCS_BAR_HAS_NOT_CLOSED)
			return;

		if (NewBarAtStartOfDay.GetYesNo() != 0)
		{
			SCDateTime IndexDateTime = sc.BaseDateTimeIn[InputIndex];

			if (IndexDateTime >= NextSessionStart)
			{
				sc.CalculateOHLCAverages(OutputIndex);

				if (!sc.AddElements(1))
					return;

				NewBar = true;
				OutputIndex = sc.DateTimeOut.GetArraySize() - 1;

				sc.DateTimeOut[OutputIndex] = sc.BaseDateTimeIn[InputIndex];

				Open[OutputIndex] = sc.Open[InputIndex];
				High[OutputIndex] = (float)sc.RoundToTickSize(sc.Close[InputIndex], CurrentBoxSize);//round off to the nearest box size
				Low[OutputIndex] = (float)sc.RoundToTickSize(sc.Close[InputIndex], CurrentBoxSize);//round off to the nearest box size
				Last[OutputIndex] = sc.Close[InputIndex];
				TrueLast[OutputIndex] = sc.Close[InputIndex];

				NextSessionStart = sc.GetTradingDayStartDateTimeOfBar(IndexDateTime) + 1*DAYS;
			}
		}

		// Remember state of last P&F bar when reach last input bar because last
		// input bar can change due to updating, causing for example a new P&F bar
		// to be added when on a later update no new bar is added.
		if (!IgnoreLastBarUntilComplete.GetYesNo() && InputIndex == sc.ArraySize - 1)
		{
			p_PFStorage->PriorOutArraySize = OutputIndex + 1;
			for (int SubgraphIndex = 0; SubgraphIndex < ArrayCount; ++SubgraphIndex)
			{
				p_PFStorage->PriorOutData[SubgraphIndex] = sc.Subgraph[SubgraphIndex][OutputIndex];
			}
		}

		// Determine our initial direction if not known
		if (DirectionArray[OutputIndex] == 0.0f)
		{
			if (Last[OutputIndex] > sc.Close[InputIndex])
				DirectionArray[OutputIndex] = -1;  // Down

			else if (Last[OutputIndex] < sc.Close[InputIndex])
				DirectionArray[OutputIndex] = 1;  // Up
		}

		if (DirectionArray[OutputIndex] == 0.0f)
			continue;  // No direction


		bool UseHighFirst;

		int OpenToHighLow = sc.CompareOpenToHighLow(sc.Open[InputIndex], sc.High[InputIndex], sc.Low[InputIndex], sc.BaseGraphValueFormat);

		if(OpenToHighLow == 1)
			UseHighFirst = true;
		else if(OpenToHighLow == -1)
			UseHighFirst = false;
		else if (sc.FormattedEvaluate(sc.Open[InputIndex] , sc.BaseGraphValueFormat, GREATER_OPERATOR, sc.Close[InputIndex], sc.BaseGraphValueFormat))
			UseHighFirst = true;
		else if (sc.FormattedEvaluate(sc.Open[InputIndex] , sc.BaseGraphValueFormat, LESS_OPERATOR, sc.Close[InputIndex], sc.BaseGraphValueFormat))
			UseHighFirst = false;
		else if (DirectionArray[OutputIndex] == 1) // Up Bar
			UseHighFirst = true;
		else
			UseHighFirst = false;

		for (int PassNumber=0; PassNumber<2; PassNumber++)
		{
			if (DirectionArray[OutputIndex] == 1)
			{
				if ((UseHighFirst && PassNumber == 0 ) || (!UseHighFirst &&PassNumber == 1))
				{
					if (sc.FormattedEvaluate(sc.High[InputIndex], sc.BaseGraphValueFormat, GREATER_EQUAL_OPERATOR, High[OutputIndex] + CurrentBoxSize, sc.BaseGraphValueFormat))
					{
						High[OutputIndex] = (float)PointAndFigureAddBoxes(sc, sc.High[InputIndex], CurrentBoxSize, 1, sc.GetValueFormat());
					}
				}

				if ((PassNumber == 0) || (UseHighFirst && PassNumber == 1))
				{
					if (sc.FormattedEvaluate(sc.Low[InputIndex], sc.BaseGraphValueFormat, LESS_EQUAL_OPERATOR, High[OutputIndex] - CurrentBoxSize * ReversalSize.GetFloat(), sc.BaseGraphValueFormat))
					{
						Last[OutputIndex] = High[OutputIndex];

						bool SetOpen = false;

						if (AllowOneBoxReversals.GetYesNo() || ((High[OutputIndex] - Low[OutputIndex]) >= CurrentBoxSize*0.80f))
						{
							// Calculate the number of boxes for the column
							NumberOfBoxes[OutputIndex] = (float)sc.Round((High[OutputIndex] - Low[OutputIndex]) / CurrentBoxSize + 1);

							sc.CalculateOHLCAverages(OutputIndex);

							if (!sc.AddElements(1))
								return;

							NewBar = true;
							OutputIndex++;
							sc.DateTimeOut[OutputIndex] = sc.BaseDateTimeIn[InputIndex];

							High[OutputIndex] = High[OutputIndex - 1] - CurrentBoxSize;

							SetOpen = true;
						}

						DirectionArray[OutputIndex] = -1;
						Low[OutputIndex] = (float)PointAndFigureAddBoxes(sc, sc.Low[InputIndex], CurrentBoxSize, 2,sc.GetValueFormat());

						if (SetOpen)
							Open[OutputIndex] = Low[OutputIndex];

					}
				}

				if (UseHighFirst)
					PassNumber++;  

			}
			else// down column
			{
				if ((!UseHighFirst && PassNumber == 0) || (UseHighFirst && PassNumber == 1))
				{
					if (sc.FormattedEvaluate(sc.Low[InputIndex] , sc.BaseGraphValueFormat, LESS_EQUAL_OPERATOR, Low[OutputIndex] - CurrentBoxSize, sc.BaseGraphValueFormat))
					{
						Low[OutputIndex] = (float)PointAndFigureAddBoxes(sc, sc.Low[InputIndex], CurrentBoxSize, 2,sc.GetValueFormat());
					}
				}

				if (PassNumber == 0 || (!UseHighFirst && PassNumber == 1))
				{
					if (sc.FormattedEvaluate(sc.High[InputIndex] , sc.BaseGraphValueFormat, GREATER_EQUAL_OPERATOR, Low[OutputIndex] + CurrentBoxSize * ReversalSize.GetFloat(), sc.BaseGraphValueFormat))
					{
						Last[OutputIndex] = Low[OutputIndex];

						bool SetOpen = false;
						if (AllowOneBoxReversals.GetYesNo() || ((High[OutputIndex] - Low[OutputIndex]) >= CurrentBoxSize*0.80f))
						{
							// Calculate the number of boxes for the column
							NumberOfBoxes[OutputIndex] = (float)sc.Round((High[OutputIndex] - Low[OutputIndex]) / CurrentBoxSize + 1);

							sc.CalculateOHLCAverages(OutputIndex);

							if (!sc.AddElements(1))
								return;

							NewBar = true;
							OutputIndex++;
							sc.DateTimeOut[OutputIndex] = sc.BaseDateTimeIn[InputIndex];
	
							Low[OutputIndex] = Low[OutputIndex - 1] + CurrentBoxSize;

							SetOpen = true;
						}

						DirectionArray[OutputIndex] = 1;
						High[OutputIndex] = (float)PointAndFigureAddBoxes(sc, sc.High[InputIndex], CurrentBoxSize, 1,sc.GetValueFormat());

						if (SetOpen)
							Open[OutputIndex] = High[OutputIndex];
					}
				}

				if (!UseHighFirst)
					PassNumber++;  

			}//else down column
		} // PassNumber for loop

		TrueLast[OutputIndex] = sc.Close[InputIndex];
		Last[OutputIndex] = sc.Close[InputIndex];
		sc.CalculateOHLCAverages(OutputIndex);
		NumberOfBoxes[OutputIndex] = (float)sc.Round((High[OutputIndex] - Low[OutputIndex]) / CurrentBoxSize + 1);

		// Add in volume and open interest
		Volume[OutputIndex] += sc.Volume[InputIndex];
		NumTrades[OutputIndex] += sc.OpenInterest[InputIndex];
		float BidAskDiffHigh = 0;
		float BidAskDiffLow = 0;
		if (sc.BaseDataIn.GetArraySize() >= SC_ASKBID_DIFF_LOW+1)
		{
			BidAskDiffHigh = AskVol[OutputIndex] - BidVol[OutputIndex] + sc.BaseDataIn[SC_ASKBID_DIFF_HIGH][InputIndex];
			BidAskDiffLow  = AskVol[OutputIndex] - BidVol[OutputIndex] + sc.BaseDataIn[SC_ASKBID_DIFF_LOW][InputIndex];
		}
		BidVol[OutputIndex] += sc.BidVolume[InputIndex];
		AskVol[OutputIndex] += sc.AskVolume[InputIndex];
		UpTickVol[OutputIndex] += sc.UpTickVolume[InputIndex];
		DownTickVol[OutputIndex] += sc.DownTickVolume[InputIndex];
		BidTrades[OutputIndex] += sc.NumberOfBidTrades[InputIndex];
		AskTrades[OutputIndex] += sc.NumberOfAskTrades[InputIndex];

		if (NewBar || BidAskDiffHigh > BidAskDiffMax[OutputIndex]) 
			BidAskDiffMax[OutputIndex] = BidAskDiffHigh;

		if (NewBar || BidAskDiffLow < BidAskDiffMin[OutputIndex]) 
			BidAskDiffMin[OutputIndex] = BidAskDiffLow;
	}
}

/*==========================================================================*/
SCSFExport scsf_StudyOverlayOHLC(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Open = sc.Subgraph[0];
	SCSubgraphRef High = sc.Subgraph[1];
	SCSubgraphRef Low = sc.Subgraph[2];
	SCSubgraphRef Last = sc.Subgraph[3];
	SCSubgraphRef OHLCAvg = sc.Subgraph[6];
	SCSubgraphRef HLCAvg = sc.Subgraph[7];
	SCSubgraphRef HLAvg = sc.Subgraph[8];

	SCInputRef Multiplier = sc.Input[3];
	SCInputRef ChartStudySubgraphReference = sc.Input[4];
	SCInputRef Version = sc.Input[5];


	if (sc.SetDefaults)
	{
		sc.GraphName = "Study Overlay - OHLC";

		sc.ValueFormat = 3;
		sc.GraphRegion = 1;
		sc.GraphDrawType = GDT_OHLCBAR;

		Open.Name = "Open";
		Open.DrawStyle = DRAWSTYLE_LINE;
		Open.PrimaryColor = RGB(0,255,0);
		Open.SecondaryColor = RGB(0,255,0);
		Open.SecondaryColorUsed = 1;
		Open.DrawZeros = false;

		High.Name = "High";
		High.DrawStyle = DRAWSTYLE_LINE;
		High.PrimaryColor = RGB(128,255,128);
		High.DrawZeros = false;

		Low.Name = "Low";
		Low.DrawStyle = DRAWSTYLE_LINE;
		Low.PrimaryColor = RGB(255,0,0);
		Low.SecondaryColor = RGB(255,0,0);
		Low.SecondaryColorUsed = 1;
		Low.DrawZeros = false;

		Last.Name = "Last";
		Last.DrawStyle = DRAWSTYLE_LINE;
		Last.PrimaryColor = RGB(255,128,128);
		Last.DrawZeros = false;

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

		Multiplier.Name = "Multiplier";
		Multiplier.SetFloat(1.0f);

		ChartStudySubgraphReference.Name = "Chart, Study, Subgraph Reference";
		ChartStudySubgraphReference.SetChartStudySubgraphValues(1,1,0);

		Version.SetInt(1);

		sc.Input[0].SetInt(0);
		sc.Input[1].SetInt(0);
		sc.Input[2].SetInt(0);

		return;
	}

	if (Version.GetInt() < 1)
	{
		Version.SetInt(1);
		
		int ChartNumber = sc.Input[0].GetChartNumber();
		int StudyID = sc.Internal_GetStudyIDByIndex(ChartNumber, sc.Input[1].GetInt());
		int SubgraphIndex = sc.Input[2].GetInt() - 1;
		ChartStudySubgraphReference.SetChartStudySubgraphValues(ChartNumber, StudyID, SubgraphIndex);
	}

	int ChartNumber = ChartStudySubgraphReference.GetChartNumber();
	int StudyID = ChartStudySubgraphReference.GetStudyID();
	int StudySubgraphNumber = ChartStudySubgraphReference.GetSubgraphIndex();

	float MultiplierVal = Multiplier.GetFloat();
	if (MultiplierVal == 0.0f)
		MultiplierVal = 1.0f;

	SCDateTimeArray ReferenceChartDateTimeArray;
	sc.GetChartDateTimeArray(ChartNumber, ReferenceChartDateTimeArray);
	if (ReferenceChartDateTimeArray.GetArraySize() == 0)
		return;
	
	SCGraphData ReferenceArrays;
	sc.GetStudyArraysFromChartUsingID(ChartNumber, StudyID, ReferenceArrays);
	
	SCFloatArrayRef ReferenceSubgraphArray = ReferenceArrays[StudySubgraphNumber];
	
	if (ReferenceSubgraphArray.GetArraySize() == 0)
	{
		sc.AddMessageToLog("Study Overlay - OHLC: Study references a study subgraph that does not exist.", 1);
		return;
	}
	
	if (sc.UpdateStartIndex == 0)
	{
		SCString StudyName = sc.GetStudyNameFromChart(ChartNumber, StudyID);
		if (ChartNumber == sc.ChartNumber && StudyID == sc.StudyGraphInstanceID)
			StudyName = "Myself";

		sc.GraphName.Format("OverlayOHLC of %s", StudyName.GetChars());

		for (int i = 0; i < sc.ArraySize; i++)
		{
			Open[i] = 0.0f;
			High[i] = 0.0f;
			Low[i] = 0.0f;
			Last[i] = 0.0f;
			sc.CalculateOHLCAverages(i);
		}
	}

	int RefDataStartIndex = sc.GetStudyDataStartIndexUsingID(StudyID);

	sc.DataStartIndex = 
		sc.GetContainingIndexForSCDateTime(sc.ChartNumber, ReferenceChartDateTimeArray[RefDataStartIndex]);

	int ArraySize = sc.BaseDateTimeIn.GetArraySize();
	int RefArraySize = ReferenceChartDateTimeArray.GetArraySize();

	int RefIndex = 0;
	int StudyStartIndex = max(sc.DataStartIndex, sc.UpdateStartIndex - 1);

	// Find the appropriate reference index
	RefIndex = sc.GetContainingIndexForSCDateTime(ChartNumber, sc.BaseDateTimeIn[StudyStartIndex]);

	// If no exact match is found, IndexForDateTime returns index right before the specified date and time
	if (sc.BaseDateTimeIn[StudyStartIndex] > ReferenceChartDateTimeArray[RefIndex])
	{
		RefIndex++;
	}

	if (RefIndex >= RefArraySize || StudyStartIndex >= ArraySize)
		return;

	int StartingPoint = StudyStartIndex; // for CalculateAverages afterwards

	float OpenVal = ReferenceSubgraphArray[RefIndex];
	float HighVal = ReferenceSubgraphArray[RefIndex];
	float LowVal = ReferenceSubgraphArray[RefIndex];
	float LastVal = ReferenceSubgraphArray[RefIndex];

	for (int StudyIndex = StudyStartIndex; StudyIndex < ArraySize; StudyIndex++)
	{
		// If DateTime is not contained, then skip. Most likely because referenced graph has a larger duration time period
		if (StudyIndex + 1 < ArraySize && sc.BaseDateTimeIn[StudyIndex + 1] <= ReferenceChartDateTimeArray[RefIndex])
		{
			Open[StudyIndex] = 0;
			Last[StudyIndex] =  0;
			High[StudyIndex] = 0;
			Low[StudyIndex] = 0;
			continue;
		}

		SCDateTime NextIndexDateTime = MAX_DATE;

		if(StudyIndex + 1 < ArraySize)
		{
			NextIndexDateTime = sc.BaseDateTimeIn[StudyIndex + 1];
		}

		// Increment RefIndex while Next BaseTime is greater than RefTime
		while (NextIndexDateTime > ReferenceChartDateTimeArray[RefIndex])
		{
			if (HighVal < ReferenceSubgraphArray[RefIndex])
				HighVal = ReferenceSubgraphArray[RefIndex];
			else if (LowVal > ReferenceSubgraphArray[RefIndex])
				LowVal = ReferenceSubgraphArray[RefIndex];

			LastVal = ReferenceSubgraphArray[RefIndex];

			RefIndex++;

			if (RefIndex >= RefArraySize)
				break;
		}

		Open[StudyIndex] = OpenVal * MultiplierVal;
		Last[StudyIndex] =  LastVal * MultiplierVal;
		High[StudyIndex] = HighVal * MultiplierVal;
		Low[StudyIndex] = LowVal * MultiplierVal;

		if (RefIndex >= RefArraySize)
			break;

		OpenVal = HighVal = LowVal = LastVal = ReferenceSubgraphArray[RefIndex]; // Reset OHLC to Next Value

		sc.CalculateOHLCAverages(StudyIndex);
	}

}

/*==========================================================================*/
SCSFExport scsf_MarketFacilitationIndexColored(SCStudyInterfaceRef sc)
{
	SCSubgraphRef MFI = sc.Subgraph[0];
	SCSubgraphRef MfiUpVolumeUp = sc.Subgraph[1];
	SCSubgraphRef MfiDownVolumeDown = sc.Subgraph[2];
	SCSubgraphRef MfiUpVolumeDown = sc.Subgraph[3];
	SCSubgraphRef MfiDownVolumeUp = sc.Subgraph[4];

	SCInputRef Multiplier = sc.Input[0];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Market Facilitation Index Colored";
		sc.StudyDescription = "Market Facilitation Index with Coloring";

		sc.GraphRegion = 1;
		sc.ValueFormat = 2;
		sc.AutoLoop = 1;

		MFI.Name = "MFI";
		MFI.DrawStyle = DRAWSTYLE_LINE;
		MFI.PrimaryColor = RGB(0,255,0);
		MFI.DrawZeros = true;

		MfiUpVolumeUp.Name = "MFI Up Volume Up Color";
		MfiUpVolumeUp.DrawStyle = DRAWSTYLE_IGNORE;
		MfiUpVolumeUp.PrimaryColor = RGB (0, 255, 0);// Green
		MfiUpVolumeUp.DrawZeros = true;

		MfiDownVolumeDown.Name = "MFI Down Volume Down Color";
		MfiDownVolumeDown.DrawStyle = DRAWSTYLE_IGNORE;
		MfiDownVolumeDown.PrimaryColor = RGB (140, 69, 16);// Brown
		MfiDownVolumeDown.DrawZeros = true;

		MfiUpVolumeDown.Name = "MFI Up Volume Down Color";
		MfiUpVolumeDown.DrawStyle = DRAWSTYLE_IGNORE;
		MfiUpVolumeDown.PrimaryColor = RGB (0, 0, 255);// Blue
		MfiUpVolumeDown.DrawZeros = true;

		MfiDownVolumeUp.Name = "MFI Down Volume Up Color";
		MfiDownVolumeUp.DrawStyle = DRAWSTYLE_IGNORE;
		MfiDownVolumeUp.PrimaryColor = RGB (255, 0, 0);// Red
		MfiDownVolumeUp.DrawZeros = true;

		Multiplier.Name = "Multiplier";
		Multiplier.SetFloat(1.0f);

		return;
	}

	

	MFI[sc.Index] = Multiplier.GetFloat() * (sc.High[sc.Index] - sc.Low[sc.Index]) / 
		sc.Volume[sc.Index];

	if (sc.Index == 0)
	{
		MFI.DataColor[sc.Index] = MfiUpVolumeUp.PrimaryColor;
		return;
	}

	// Coloring
	if (MFI[sc.Index] > MFI[sc.Index - 1] && sc.Volume[sc.Index] > sc.Volume[sc.Index - 1])
	{
		MFI.DataColor[sc.Index] = MfiUpVolumeUp.PrimaryColor;
	}
	else if (MFI[sc.Index] < MFI[sc.Index - 1] && sc.Volume[sc.Index] < sc.Volume[sc.Index - 1])
	{
		MFI.DataColor[sc.Index] = MfiDownVolumeDown.PrimaryColor;
	}
	else if (MFI[sc.Index] > MFI[sc.Index - 1] && sc.Volume[sc.Index] < sc.Volume[sc.Index - 1])
	{
		MFI.DataColor[sc.Index] = MfiUpVolumeDown.PrimaryColor;
	}
	else if (MFI[sc.Index] < MFI[sc.Index - 1] && sc.Volume[sc.Index] > sc.Volume[sc.Index - 1])
	{
		MFI.DataColor[sc.Index] = MfiDownVolumeUp.PrimaryColor;
	}
}

/*==========================================================================*/
SCSFExport scsf_StarcBands(SCStudyInterfaceRef sc)
{
	SCSubgraphRef KeltnerAverage = sc.Subgraph[0];
	SCSubgraphRef TopBand = sc.Subgraph[1];
	SCSubgraphRef BottomBand = sc.Subgraph[2];
	SCSubgraphRef Temp3 = sc.Subgraph[3];
	SCSubgraphRef Temp4 = sc.Subgraph[4];

	SCInputRef InputData = sc.Input[0];
	SCInputRef KeltnerMALength = sc.Input[3];
	SCInputRef TrueRangeAvgLength = sc.Input[4];
	SCInputRef TopBandMult = sc.Input[5];
	SCInputRef BottomBandMult = sc.Input[6];
	SCInputRef KeltnerMAType = sc.Input[7];
	SCInputRef ATR_MAType = sc.Input[8];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Starc Bands";

		sc.GraphRegion = 0;
		sc.ValueFormat = 3;
		sc.AutoLoop = 1;

		KeltnerAverage.Name = "Starc Average";
		KeltnerAverage.DrawStyle = DRAWSTYLE_LINE;
		KeltnerAverage.PrimaryColor = RGB(0,255,0);
		KeltnerAverage.DrawZeros = true;

		TopBand.Name = "Top";
		TopBand.DrawStyle = DRAWSTYLE_LINE;
		TopBand.PrimaryColor = RGB(255,0,255);
		TopBand.DrawZeros = true;

		BottomBand.Name = "Bottom";
		BottomBand.DrawStyle = DRAWSTYLE_LINE;
		BottomBand.PrimaryColor = RGB(255,255,0);
		BottomBand.DrawZeros = true;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		KeltnerMALength.Name = "Starc Mov Avg Length";
		KeltnerMALength.SetInt(6);
		KeltnerMALength.SetIntLimits(1, MAX_STUDY_LENGTH);

		TrueRangeAvgLength.Name = "True Range Avg Length";
		TrueRangeAvgLength.SetInt(15);
		TrueRangeAvgLength.SetIntLimits(1, MAX_STUDY_LENGTH);

		TopBandMult.Name = "Top Band Multiplier";
		TopBandMult.SetFloat(2.0f);

		BottomBandMult.Name="Bottom Band Multiplier";
		BottomBandMult.SetFloat(2.0f);

		KeltnerMAType.Name = "Starc Mov Avg Type (Center line)";
		KeltnerMAType.SetMovAvgType(MOVAVGTYPE_SIMPLE);

		ATR_MAType.Name = "ATR Mov Avg Type";
		ATR_MAType.SetMovAvgType(MOVAVGTYPE_SIMPLE);

		return;
	}

	sc.DataStartIndex = max(KeltnerMALength.GetInt(), TrueRangeAvgLength.GetInt());

	sc.MovingAverage(sc.BaseDataIn[(int)InputData.GetInputDataIndex()], KeltnerAverage, KeltnerMAType.GetMovAvgType(), KeltnerMALength.GetInt());

	sc.ATR(sc.BaseDataIn, Temp3, TrueRangeAvgLength.GetInt(), ATR_MAType.GetMovAvgType());
	sc.ATR(sc.BaseDataIn, Temp4, TrueRangeAvgLength.GetInt(), ATR_MAType.GetMovAvgType());

	TopBand[sc.Index] = Temp3[sc.Index] * TopBandMult.GetFloat() + KeltnerAverage[sc.Index];
	BottomBand[sc.Index] = KeltnerAverage[sc.Index] - (Temp4[sc.Index] * BottomBandMult.GetFloat());
}


/*==========================================================================*/
SCSFExport scsf_BarTimeDuration(SCStudyInterfaceRef sc)
{
	SCSubgraphRef TimeDiff = sc.Subgraph[0];

	SCInputRef Multiplier = sc.Input[0];

	SCInputRef UseMaxDuration =  sc.Input[1];

	SCInputRef MaxDuration =  sc.Input[2];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Bar Time Duration";
		sc.StudyDescription = "This study calculates and displays the time duration of a bar. For the highest accuracy, this study requires that the Intraday Data Storage Time Unit setting is 1 second or less.";

		TimeDiff.Name = "Duration";
		TimeDiff.DrawStyle = DRAWSTYLE_BAR;
		TimeDiff.PrimaryColor = RGB(0,255,0);


		UseMaxDuration.Name = "Use Maximum Duration";
		UseMaxDuration.SetYesNo(0);

		MaxDuration.Name="Maximum Duration";
		MaxDuration.SetTime(0);


		sc.GraphRegion = 1;
		sc.ValueFormat = VALUEFORMAT_TIME;
		sc.AutoLoop = 1;
	}

	if (sc.Index < sc.ArraySize - 1)
	{
		TimeDiff[sc.Index] = (float)(sc.BaseDateTimeIn[sc.Index + 1] - sc.BaseDateTimeIn[sc.Index]);
		
		//Not done
		SCDateTime BarDuration(TimeDiff[sc.Index]);
	
		if (sc .AreTimeSpecificBars()&& BarDuration.GetTime() >sc.SecondsPerBar)
			BarDuration .SetDateTime(0,sc.SecondsPerBar);

	}
	else
		TimeDiff[sc.Index] = (float)(sc.DateTimeOfLastFileRecord - sc.BaseDateTimeIn[sc.Index]);

	if(UseMaxDuration.GetYesNo())
	{

		SCDateTime MaxTime(0,MaxDuration.GetTime());
		if (TimeDiff[sc.Index]  > (double)MaxTime)
			TimeDiff[sc.Index] = (float)MaxTime;
	}
}

/*==========================================================================*/

SCSFExport scsf_ReferenceDataFromAnotherChart(SCStudyInterfaceRef sc)
{
	SCInputRef ChartStudyInput = sc.Input[0];

	if (sc.SetDefaults)
	{

		sc.GraphName = "Reference Data";
		sc.StudyDescription = "This is an example of referencing data from another chart.";

		sc.AutoLoop = 1;
		

		sc.FreeDLL=0; 

		ChartStudyInput.Name = "Study Reference";
		ChartStudyInput.SetChartStudyValues(1,1);


		return;
	}

	// The following code is for getting the High array
	// and corresponding index from another chart.
	
	// Define a graph data object to get all of the base graph data from the specified chart
	SCGraphData BaseGraphData;
	
	// Get the base graph data from the specified chart
	sc.GetChartBaseData(ChartStudyInput.GetChartNumber(), BaseGraphData);
	
	// Define a reference to the High array
	SCFloatArrayRef HighArray = BaseGraphData[SC_HIGH];
	
	// Array is empty. Nothing to do.
	if(HighArray.GetArraySize() == 0)
		return;
	
	
	// Get the index in the specified chart that is 
	// nearest to current index.
	int RefChartIndex = sc.GetNearestMatchForDateTimeIndex(ChartStudyInput.GetChartNumber(), sc.Index);
	float NearestRefChartHigh = HighArray[RefChartIndex];

	// Get the index in the specified chart that contains
	// the DateTime of the bar at the current index.
	RefChartIndex = sc.GetContainingIndexForDateTimeIndex(ChartStudyInput.GetChartNumber(), sc.Index);
	float ContainingRefChartHigh = HighArray[RefChartIndex];

	// Get the index in the specified chart that exactly
	// matches the DateTime of the current index.
	RefChartIndex = sc.GetExactMatchForSCDateTime(ChartStudyInput.GetChartNumber(), sc.BaseDateTimeIn[sc.Index]);
	if(RefChartIndex != -1)
	{
		float ExactMatchRefChartHigh = HighArray[RefChartIndex];
	}
	
	
	// The following code is for getting a study subgraph array
	// and corresponding index from another chart. 
	// For example, this could be a moving average study subgraph.

	// Define a graph data object to get all of the study data
	SCGraphData StudyData;
	
	// Get the study data from the specified chart
	sc.GetStudyArraysFromChartUsingID(ChartStudyInput.GetChartNumber(), ChartStudyInput.GetStudyID(), StudyData);

	//Check if the study has been found.  If it has, GetArraySize() will return the number of Subgraphs in the study.
	if(StudyData.GetArraySize() == 0)
		return;
	
	// Define a reference to the first subgraph array
	SCFloatArrayRef SubgraphArray = StudyData[0];
	
	// Array is empty. Nothing to do.
	if(SubgraphArray.GetArraySize() == 0)
		return;
	
	
	// Get the index in the specified chart that is nearest
	// to current index.
	RefChartIndex = sc.GetNearestMatchForDateTimeIndex(ChartStudyInput.GetChartNumber(), sc.Index);
	float NearestSubgraphValue = SubgraphArray[RefChartIndex];

	// Get the index in the specified chart that contains 
	// the DateTime of the bar at the current index.
	RefChartIndex = sc.GetContainingIndexForDateTimeIndex(ChartStudyInput.GetChartNumber(), sc.Index);
	float ContainingSubgraphValue = SubgraphArray[RefChartIndex];

	// Get the index in the specified chart that exactly
	// matches the DateTime of the current index.
	RefChartIndex = sc.GetExactMatchForSCDateTime(ChartStudyInput.GetChartNumber(), sc.BaseDateTimeIn[sc.Index]);
	if(RefChartIndex != -1)//-1 means that there was not an exact match and therefore we do not have a valid index to work with
	{
		float ExactMatchSubgraphValue = SubgraphArray[RefChartIndex];
	}
}

/*****************************************************************/
SCSFExport scsf_ReferenceStudyData(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Average = sc.Subgraph[0];
	SCInputRef Study1 = sc.Input[0];
	SCInputRef Study1Subgraph = sc.Input[1];

	if (sc.SetDefaults)
	{
 
		sc.GraphName = "Reference Study Data";
		sc.StudyDescription = "This study function is an example of referencing data from other studies on the chart.";

		sc.AutoLoop = 1;

		// We must use a low precedence level to ensure
		// the other studies are already calculated first.
		sc.CalculationPrecedence = LOW_PREC_LEVEL;

		sc.FreeDLL = 0; 

		Average.Name = "Average";
		Average.DrawStyle = DRAWSTYLE_LINE;
		Average.PrimaryColor = RGB(0,255,0);

		Study1.Name = "Input Study 1";
		Study1.SetStudyID(0);

		Study1Subgraph.Name = "Study 1 Subgraph";
		Study1Subgraph.SetSubgraphIndex(0);

		return;
	}

	// Get the subgraph specified with the Study 1
	// Subgraph input from the study specified with
	// the Input Study 1 input.
	SCFloatArray Study1Array;
	sc.GetStudyArrayUsingID(Study1.GetStudyID(),Study1Subgraph.GetSubgraphIndex(),Study1Array);

	// We are getting the value of the study subgraph
	// at sc.Index. For example, this could be
	// a moving average value if the study we got in
	// the prior step is a moving average.
	float RefStudyCurrentValue = Study1Array[sc.Index];

	// Here we will add 10 to this value and compute 
	// an average of it. Since the moving average
	// function we will be calling requires an input
	// array, we will use one of the internal arrays
	// on a subgraph to hold this intermediate 
	// calculation. This internal array could be 
	// thought of as a Worksheet column where you 
	// are performing intermediate calculations.

	Average.Arrays[9][sc.Index] = RefStudyCurrentValue + 10;

	sc.SimpleMovAvg(Average.Arrays[9],Average,15);
}


/*==========================================================================*/
SCSFExport scsf_IntermediateStudyCalculationsUsingArrays(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Average = sc.Subgraph[0];
	SCInputRef Length = sc.Input[0];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Intermediate Study Calculations Using Arrays";
		sc.StudyDescription = "For more information about this study, refer to the Using ACSIL Study Calculation Functions documentation page.";

		sc.AutoLoop = 1;

		sc.FreeDLL = 0; 

		Average.Name = "Average";
		Average.DrawStyle = DRAWSTYLE_LINE;
		Average.PrimaryColor = RGB(0,255,0);

		Length.Name = "Moving Average Length";
		Length.SetInt(10);
		Length.SetIntLimits(1, MAX_STUDY_LENGTH);

		return;
	}


	// Here we will add 10 to the sc.BaseData Last value at the current index
	// and compute an average of it. Since the moving average function we
	// will be calling requires an input array and not a single value, we
	// will use one of the internal extra arrays on a Subgraph to hold 
	// this intermediate calculation. This internal extra array could be
	// thought of as a Worksheet column where you are performing intermediate
	// calculations. We will use one of the internal extra arrays that is
	// part of the Subgraph we are using to hold the output from the moving
	// average study calculation function we will be calling next. Although
	// any Subgraph internal extra array or even a Subgraph Data array
	// could be used.

	SCFloatArrayRef Last =  sc.Close;
	SCFloatArrayRef Array8 = Average.Arrays[8];

	Array8[sc.Index] =Last[sc.Index] + 10;

	// In this function call we are passing this internal extra array and
	// we also pass in, sc.Subgraph[0], to receive the result at the 
	// current index.

	sc.SimpleMovAvg(Array8,Average,Length.GetInt());
}


/*==========================================================================*/
SCSFExport scsf_StudySubgraphMultiplier(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Result = sc.Subgraph[0];
	
	SCInputRef InputData = sc.Input[0];
	SCInputRef Multiplier = sc.Input[1];
	SCInputRef DrawZeros = sc.Input[2];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Study Subgraph Multiply";
	sc.StudyDescription = "This study multiplies selected study subgraph by the specified Multiplier. To select the study to use, set the Based On study setting to that study and set the Input Data input to the specific study subgraph in that study to multiply by the specified Multiplier.";
		sc.AutoLoop = 1;

		sc.FreeDLL = 0; 

		Result.Name = "Result";
		Result.DrawStyle = DRAWSTYLE_LINE;
		Result.PrimaryColor = RGB(0,255,0);

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(0);

		Multiplier.Name = "Multiplier";
		Multiplier.SetFloat(1.0);

		DrawZeros.Name = "Draw Zeros";
		DrawZeros.SetYesNo(false);
		

		return;
	}

	Result.DrawZeros= DrawZeros. GetYesNo();


	Result[sc.Index]= sc.BaseData[InputData.GetInputDataIndex()][sc.Index]*Multiplier.GetFloat();
}

/*==========================================================================*/

SCSFExport scsf_StudySubgraphDivide(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Result = sc.Subgraph[0];

	SCInputRef InputData = sc.Input[0];
	SCInputRef Divider = sc.Input[1];

	SCInputRef DrawZeros = sc.Input[2];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Study Subgraph Divide";
		sc.StudyDescription = "This study divides selected study subgraph by the specified Divider. To select the study to use, set the Based On study setting to that study and set the Input Data input to the specific study subgraph in that study to divide by the specified Divider.";
		sc.AutoLoop = 1;

		sc.FreeDLL = 0; 

		Result.Name = "Result";
		Result.DrawStyle = DRAWSTYLE_LINE;
		Result.PrimaryColor = RGB(0,255,0);

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(0);

		Divider.Name = "Divider";
		Divider.SetFloat(1.0);

		DrawZeros.Name = "Draw Zeros";
		DrawZeros.SetYesNo(false);


		return;
	}

	Result.DrawZeros= DrawZeros. GetYesNo();
	Result[sc.Index]= sc.BaseData[InputData.GetInputDataIndex()][sc.Index]/Divider.GetFloat();
}

/*==========================================================================*/
SCSFExport scsf_StudySubgraphAddition(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Result = sc.Subgraph[0];

	SCInputRef InputData = sc.Input[0];
	SCInputRef AmountToAdd = sc.Input[1];
	SCInputRef DrawZeros = sc.Input[2];

	if (sc.SetDefaults)
	{

		sc.GraphName = "Study Subgraph Add";
		sc.StudyDescription = "This study adds the specified value to the selected study Subgraph. To select the study to use, set the Based On study setting to that study and set the Input Data input to the specific study Subgraph in that study to Add the value to.";

		sc.AutoLoop = 1;

		sc.FreeDLL = 0; 

		Result.Name = "Result";
		Result.DrawStyle = DRAWSTYLE_LINE;
		Result.PrimaryColor = RGB(0,255,0);

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(0);

		AmountToAdd.Name = "Amount to Add";
		AmountToAdd.SetFloat(0);

		DrawZeros.Name = "Draw Zeros";
		DrawZeros.SetYesNo(false);


		return;
	}

	Result.DrawZeros= DrawZeros. GetYesNo();

	Result[sc.Index]= sc.BaseData[InputData.GetInputDataIndex()][sc.Index]+AmountToAdd.GetFloat();
}

/*==========================================================================*/
SCSFExport scsf_StudySubgraphSubtraction(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Result = sc.Subgraph[0];

	SCInputRef InputData = sc.Input[0];
	SCInputRef AmountToSubtract = sc.Input[1];
	SCInputRef DrawZeros = sc.Input[2];

	if (sc.SetDefaults)
	{

		sc.GraphName = "Study Subgraph Subtract";
		sc.StudyDescription = "This study subtracts the specified value from the selected study subgraph. To select the study to use, set the Based On study setting to that study and set the Input Data input to the specific study subgraph in that study to Subtract the value from.";

		sc.AutoLoop = 1;

		sc.FreeDLL = 0; 

		Result.Name = "Result";
		Result.DrawStyle = DRAWSTYLE_LINE;
		Result.PrimaryColor = RGB(0,255,0);

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(0);

		AmountToSubtract.Name = "Amount to Subtract";
		AmountToSubtract.SetFloat(0);

		DrawZeros.Name = "Draw Zeros";
		DrawZeros.SetYesNo(false);

		return;
	}

	Result.DrawZeros= DrawZeros. GetYesNo();


	Result[sc.Index]= sc.BaseData[InputData.GetInputDataIndex()][sc.Index]-AmountToSubtract.GetFloat();
}


/*==========================================================================*/
SCSFExport scsf_StudySubgraphStandardDeviation(SCStudyInterfaceRef sc)
{
	SCSubgraphRef StdDevTop     = sc.Subgraph[0];
	SCSubgraphRef StdDevBottom  = sc.Subgraph[1];

	SCFloatArrayRef StdDev = StdDevTop.Arrays[0];

	SCInputRef InputData        = sc.Input[0];
	SCInputRef StdDevLength     = sc.Input[1];
	SCInputRef StdDevMultiplier = sc.Input[2];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Study Subgraph Standard Deviation";
		sc.StudyDescription = "This study calculates the standard deviation of the selected study subgraph using the specified length and multiplier. To select the study to use, set the Based On study setting to that study and set the Input Data input to the specific study subgraph in that study to calculate the standard deviation on.";
		sc.AutoLoop = 1;

		sc.FreeDLL = 0; 

		StdDevTop.Name = "Standard Deviation Top";
		StdDevTop.DrawStyle = DRAWSTYLE_LINE;
		StdDevTop.PrimaryColor = RGB(0,255,0);

		StdDevBottom.Name = "Standard Deviation Bottom";
		StdDevBottom.DrawStyle = DRAWSTYLE_LINE;
		StdDevBottom.PrimaryColor = RGB(255,0,255);

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(0);

		StdDevLength.Name = "Standard Deviation Length";
		StdDevLength.SetInt(20);
		StdDevLength.SetIntLimits(1, INT_MAX);

		StdDevMultiplier.Name = "Standard Deviation Multiplier";
		StdDevMultiplier.SetFloat(1.0f);

		return;
	}

	sc.StdDeviation(sc.BaseData[InputData.GetInputDataIndex()], StdDev, StdDevLength.GetInt());

	float Value = sc.BaseData[InputData.GetInputDataIndex()][sc.Index];
	float ChannelSize = StdDevMultiplier.GetFloat() * StdDev[sc.Index];

	StdDevTop[sc.Index]    = Value + ChannelSize;
	StdDevBottom[sc.Index] = Value - ChannelSize;
}


/*=====================================================================
	Volume At Price array test.
----------------------------------------------------------------------------*/
SCSFExport scsf_VolumeAtPriceArrayTest(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Volume = sc.Subgraph[0];
	SCSubgraphRef VolumeAtPriceVolume = sc.Subgraph[1];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults
		
		sc.GraphName = "Volume At Price Array Test";
		
		sc.StudyDescription = "This study tests the sc.VolumeAtPriceForBars array.";
		
		sc.AutoLoop = 1;
		
		// During development set this flag to 1, so the DLL can be modified. When development is done, set it to 0 to improve performance.
		sc.FreeDLL = 0;
		
		sc.MaintainVolumeAtPriceData = 1;  // true
		
		Volume.Name = "Volume";
		Volume.DrawStyle = DRAWSTYLE_BAR;
		Volume.PrimaryColor = RGB(0,255,0);
		Volume.LineWidth = 2;
		
		VolumeAtPriceVolume.Name = "VAP Volume";
		VolumeAtPriceVolume.DrawStyle = DRAWSTYLE_DASH;
		VolumeAtPriceVolume.LineWidth = 2;
		
		return;
	}

	// Do data processing
	
	Volume[sc.Index] = sc.Volume[sc.Index];// For comparison to the subgraph VolumeAtPriceVolume
	
	if ((int)sc.VolumeAtPriceForBars->GetNumberOfBars() < sc.ArraySize)
		return;


	// Get the sum of the volumes from all of the prices at this bar by using sc.VolumeAtPriceForBars->GetNextHigherVAPElement()
	unsigned int TotalVolume = 0;
	int CurrentVAPPriceInTicks = INT_MIN ; // This will be incremented from the lowest to highest price tick in the bar
	const s_VolumeAtPriceV2 *p_VolumeAtPrice;

	while (sc.VolumeAtPriceForBars->GetNextHigherVAPElement(sc.Index, CurrentVAPPriceInTicks, &p_VolumeAtPrice))
	{
		TotalVolume += p_VolumeAtPrice->Volume;

		//Calculate the price.  This requires multiplying p_VolumeAtPrice->PriceInTicks by the tick size
		float Price = p_VolumeAtPrice->PriceInTicks * sc.TickSize;

		//Other members available:
		//p_VolumeAtPrice->AskVolume;
		//p_VolumeAtPrice->BidVolume;
		//p_VolumeAtPrice->NumberOfTrades;
	}

	VolumeAtPriceVolume[sc.Index] = (float)TotalVolume;


	// Get the sum of the volumes from all of the prices at this bar by using sc.VolumeAtPriceForBars->GetVAPElementAtIndex()
	TotalVolume = 0;
	s_VolumeAtPriceV2 *p_VolumeAtPriceAtIndex;
	int Count = sc.VolumeAtPriceForBars-> GetSizeAtBarIndex(sc.Index);
	for (int ElementIndex = 0;ElementIndex < Count; ElementIndex ++)
	{
			sc.VolumeAtPriceForBars->GetVAPElementAtIndex(sc.Index, ElementIndex, &p_VolumeAtPriceAtIndex);

			//verify the pointer is not a null, otherwise an exception will occur
			if (p_VolumeAtPriceAtIndex)
				TotalVolume += p_VolumeAtPriceAtIndex->Volume;
		
	}

	VolumeAtPriceVolume[sc.Index] = (float)TotalVolume;

}



/*==========================================================================*/
/*
Here are the other markets (in addition to the e-mini S&P):

* 30-year Bonds (ZB)
o Tick Setting: 6 ticks (Please adjust the calculation of Ping Pong Entries accordingly)
* Euro FX (6E)
o Tick Setting: 16 ticks (Please adjust the calculation of Ping Pong Entries accordingly)
* e-mini Dow (YM)
o Tick Setting: 16 ticks (Please adjust the calculation of Ping Pong Entries accordingly)

*/


SCSFExport scsf_RockwellTrading(SCStudyInterfaceRef sc)
{
	//--------------#1
	SCSubgraphRef BaseColor = sc.Subgraph[0];
	SCSubgraphRef StudySubgraphAddition = sc.Subgraph[1];
	SCSubgraphRef StudySubgraphSubtraction = sc.Subgraph[2];
	SCInputRef Offset = sc.Input[0];

	//--------------#2
	SCSubgraphRef DailyRangeAverage = sc.Subgraph[3];
	SCSubgraphRef StopLoss = sc.Subgraph[4];
	SCSubgraphRef ProfitTarget = sc.Subgraph[5];

	SCInputRef DailyRangeChartNumber = sc.Input[1];
	SCInputRef DailyRangeAverageStudyNumber = sc.Input[2];

	SCInputRef DailyRangeAverageReference = sc.Input[9];

	SCInputRef DisplayTargetAndStopValues=sc.Input[8];

	//--------------#3 Coloring the Main Price Graph
	SCSubgraphRef ColorBarSubgraph = sc.Subgraph[6];
	SCInputRef SelectMACD = sc.Input[3];

	//--------------#4
	SCSubgraphRef PointOnHigh = sc.Subgraph[7];
	SCSubgraphRef PointOnLow = sc.Subgraph[8];
	SCInputRef SelectRSI = sc.Input[4];

	//--------------
	SCInputRef Version = sc.Input[5];
	SCInputRef StopLossMultiplier = sc.Input[6];
	SCInputRef ProfitTargetMultiplier = sc.Input[7];

	

	
	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Rockwell Trading";

		sc.StudyDescription = "The study is for the Rockwell trading method. Notes: The Offset input specifies the amount as an actual value added to the Low of the last bar and subtracted from the High the last bar. These results are displayed as text on the right side of the chart.";

		sc.AutoLoop = 1;

		// During development set this flag to 1, so the DLL can be modified. When development is done, set it to 0 to improve performance.
		sc.FreeDLL = 0;

		sc.GraphRegion = 0; 

		//--------------#1
		//Subgraphs
		BaseColor.Name = "Base Color";
		BaseColor.PrimaryColor = RGB(255,255,255);
		BaseColor.DrawStyle = DRAWSTYLE_COLORBAR;
		BaseColor.LineLabel = 0;

		StudySubgraphAddition.Name = "Addition";
		StudySubgraphAddition.DrawStyle = DRAWSTYLE_TEXT;
		StudySubgraphAddition.LineWidth = 8;
		StudySubgraphAddition.LineLabel = LL_DISPLAY_VALUE | LL_VALUE_ALIGN_RIGHT | LL_VALUE_ALIGN_CENTER;
		StudySubgraphAddition.PrimaryColor = RGB(0,255,0);

		StudySubgraphSubtraction.Name = "Subtraction";
		StudySubgraphSubtraction.DrawStyle = DRAWSTYLE_TEXT;
		StudySubgraphSubtraction.LineWidth = 8;
		StudySubgraphSubtraction.LineLabel = LL_DISPLAY_VALUE | LL_VALUE_ALIGN_RIGHT | LL_VALUE_ALIGN_CENTER;
		StudySubgraphSubtraction.PrimaryColor = RGB(255,0,0);

		//Inputs
		Offset.Name = "Offset";
		Offset.SetFloat(2.25f);

		//--------------#2
		//Subgraphs

		DailyRangeAverage.Name = "Daily Range Average";
		DailyRangeAverage.DrawStyle = DRAWSTYLE_CUSTOM_TEXT;
		DailyRangeAverage.PrimaryColor = RGB(255,255,255);
		DailyRangeAverage.LineWidth = 10;

		StopLoss.Name = "Stop Loss";
		StopLoss.DrawStyle = DRAWSTYLE_CUSTOM_TEXT;
		StopLoss.PrimaryColor = RGB(255,0,0);
		StopLoss.LineWidth = 10;

		ProfitTarget.Name = "Profit Target";
		ProfitTarget.DrawStyle = DRAWSTYLE_CUSTOM_TEXT;
		ProfitTarget.PrimaryColor = RGB(0,255,0);
		ProfitTarget.LineWidth = 10;

		//DailyRangeChartNumber.Name = "Daily Chart # Reference";
		//DailyRangeChartNumber.SetChartNumber(2);

		//DailyRangeAverageStudyNumber.Name = "Daily Range Average Study Number"; 
		//DailyRangeAverageStudyNumber.SetInt(2);


		DailyRangeAverageReference.Name = "Daily Range Average Reference";
		DailyRangeAverageReference.SetChartStudySubgraphValues(2,2, 0);



		//--------------#3 Coloring the Main Price Graph
		ColorBarSubgraph.Name = "Color Bar";
		ColorBarSubgraph.DrawStyle = DRAWSTYLE_COLORBAR;
		ColorBarSubgraph.SecondaryColorUsed = 1;
		ColorBarSubgraph.PrimaryColor = RGB(0,255,0);
		ColorBarSubgraph.SecondaryColor = RGB(255,0,0);

		SelectMACD.Name = "MACD Study Reference"; 
		SelectMACD.SetStudyID(2);

		//--------------#4

		PointOnHigh.Name = "RSI Low";
		PointOnHigh.DrawStyle = DRAWSTYLE_POINTHIGH;
		PointOnHigh.PrimaryColor = RGB(128,0,0); 
		PointOnHigh.LineWidth = 5;

		PointOnLow.Name = "RSI High";
		PointOnLow.DrawStyle = DRAWSTYLE_POINTLOW;
		PointOnLow.PrimaryColor = RGB(0,128,0);
		PointOnLow.LineWidth = 5;

		SelectRSI.Name = "RSI Study Reference"; 
		SelectRSI.SetStudyID(9);
		//--------------

		DisplayTargetAndStopValues.  Name = "Display Target and Stop Values";
		DisplayTargetAndStopValues.SetYesNo(true);

		Version.SetInt(4);

		StopLossMultiplier.Name = "Stop Loss Multiplier";
		StopLossMultiplier.SetFloat(0.1f);
		ProfitTargetMultiplier.Name = "Profit Target Multiplier";
		ProfitTargetMultiplier.SetFloat(0.15f);

		sc.CalculationPrecedence = LOW_PREC_LEVEL;

		return;
	}

	

	//Version update
	if(Version.GetInt()<3)
	{
		StopLossMultiplier.SetFloat(0.1f);
		ProfitTargetMultiplier.SetFloat(0.15f);
	}
	else if (Version.GetInt()<4)
	{
		DisplayTargetAndStopValues.SetYesNo(true);


		DailyRangeAverageReference. SetChartStudySubgraphValues(DailyRangeChartNumber.GetChartNumber(), DailyRangeAverageStudyNumber.GetInt(),0);

	}

	Version.SetInt(4);
	

	// Do data processing

	//--------------#1
	

	StudySubgraphAddition[sc.Index] = sc.Low[sc.Index] + Offset.GetFloat();
	StudySubgraphSubtraction[sc.Index] = sc.High[sc.Index] - Offset.GetFloat();
	
	//--------------#2
	if (sc.Index == sc.ArraySize - 1)
	{


		SCFloatArray  DailyRangeAverageData;

		sc.GetStudyArrayFromChartUsingID(DailyRangeAverageReference.GetChartStudySubgraphValues() , DailyRangeAverageData);

		if(DailyRangeAverageData.GetArraySize() == 0)
			return;

		float PriorDayDailyRangeAverage = DailyRangeAverageData[DailyRangeAverageData.GetArraySize()-2]; // Get the prior day's daily range average
		float CurrentStopLoss = (float)sc.RoundToTickSize(PriorDayDailyRangeAverage*StopLossMultiplier.GetFloat(),sc.TickSize);
		float CurrentProfitTarget = (float)sc.RoundToTickSize(PriorDayDailyRangeAverage*ProfitTargetMultiplier.GetFloat(),sc.TickSize);



		s_UseTool Tool_DailyRangeAverage;

		Tool_DailyRangeAverage.Clear();
		Tool_DailyRangeAverage.ChartNumber = sc.ChartNumber;
		Tool_DailyRangeAverage.DrawingType = DRAWING_TEXT;
		Tool_DailyRangeAverage.LineNumber = 72342;
		Tool_DailyRangeAverage.BeginDateTime = 5;
		Tool_DailyRangeAverage.BeginValue = 95;
		Tool_DailyRangeAverage.UseRelativeValue= true;
		Tool_DailyRangeAverage.Region = sc.GraphRegion;
		Tool_DailyRangeAverage.Color = DailyRangeAverage.PrimaryColor;
		Tool_DailyRangeAverage.FontBold = true;
		Tool_DailyRangeAverage.FontFace = "Lucida Console";
		Tool_DailyRangeAverage.FontSize = DailyRangeAverage.LineWidth;
		Tool_DailyRangeAverage.AddMethod = UTAM_ADD_OR_ADJUST;
		Tool_DailyRangeAverage.ReverseTextColor = false;

		Tool_DailyRangeAverage.Text.Format("DailyRangeAvg: %s",sc.FormatGraphValue(PriorDayDailyRangeAverage,sc.BaseGraphValueFormat).GetChars());

		sc.UseTool(Tool_DailyRangeAverage);


		if (DisplayTargetAndStopValues.GetYesNo())
		{
			s_UseTool Tool_StopLoss;

			Tool_StopLoss.Clear();
			Tool_StopLoss.ChartNumber = sc.ChartNumber;
			Tool_StopLoss.DrawingType = DRAWING_TEXT;
			Tool_StopLoss.LineNumber = 72343;
			Tool_StopLoss.BeginDateTime = 5;
			Tool_StopLoss.BeginValue = 90;
			Tool_StopLoss.UseRelativeValue= true;
			Tool_StopLoss.Region = sc.GraphRegion;
			Tool_StopLoss.Color = StopLoss.PrimaryColor;
			Tool_StopLoss.FontBold = true;
			Tool_StopLoss.FontFace = "Lucida Console";
			Tool_StopLoss.FontSize = StopLoss.LineWidth;
			Tool_StopLoss.AddMethod = UTAM_ADD_OR_ADJUST;
			Tool_StopLoss.ReverseTextColor = false;

			Tool_StopLoss.Text.Format("StopLoss: %s ",sc.FormatGraphValue(CurrentStopLoss,sc.BaseGraphValueFormat).GetChars());

			sc.UseTool(Tool_StopLoss);

		}


		if (DisplayTargetAndStopValues.GetYesNo())
		{

			s_UseTool Tool_ProfitTarget;

			Tool_ProfitTarget.Clear();
			Tool_ProfitTarget.ChartNumber = sc.ChartNumber;
			Tool_ProfitTarget.DrawingType = DRAWING_TEXT;
			Tool_ProfitTarget.LineNumber = 72344;
			Tool_ProfitTarget.BeginDateTime = 5;
			Tool_ProfitTarget.BeginValue = 85;
			Tool_ProfitTarget.UseRelativeValue= true;
			Tool_ProfitTarget.Region = sc.GraphRegion;
			Tool_ProfitTarget.Color = ProfitTarget.PrimaryColor;
			Tool_ProfitTarget.FontBold = true;
			Tool_ProfitTarget.FontFace = "Lucida Console";
			Tool_ProfitTarget.FontSize = ProfitTarget.LineWidth;
			Tool_ProfitTarget.AddMethod = UTAM_ADD_OR_ADJUST;
			Tool_ProfitTarget.ReverseTextColor = false;

			Tool_ProfitTarget.Text.Format("Target: %s ",sc.FormatGraphValue(CurrentProfitTarget,sc.BaseGraphValueFormat).GetChars());

			sc.UseTool(Tool_ProfitTarget);
		}
	}

	//--------------#3 Coloring the Main Price Graph

	SCFloatArray  MACD;
	sc.GetStudyArrayUsingID(SelectMACD.GetStudyID(), 0, MACD);

	SCFloatArray  MACDDiff;
	sc.GetStudyArrayUsingID(SelectMACD.GetStudyID(), 2, MACDDiff);

	//BaseColor 
	ColorBarSubgraph[sc.Index] = 1;
	ColorBarSubgraph.DataColor[sc.Index] = BaseColor.PrimaryColor;

	
	if(MACD[sc.Index] > 0.0 && MACDDiff[sc.Index] > 0.0)
		ColorBarSubgraph.DataColor[sc.Index] = ColorBarSubgraph.PrimaryColor;
	
	else if(MACD[sc.Index] < 0.0 && MACDDiff[sc.Index] < 0.0)
		ColorBarSubgraph.DataColor[sc.Index] = ColorBarSubgraph.SecondaryColor;

	//--------------#4
	SCFloatArray  RSI;
	sc.GetStudyArrayUsingID(SelectRSI.GetStudyID(), 0, RSI);

	SCFloatArray  RSIHigh;
	sc.GetStudyArrayUsingID(SelectRSI.GetStudyID(), 1, RSIHigh);

	SCFloatArray  RSILow;
	sc.GetStudyArrayUsingID(SelectRSI.GetStudyID(), 2, RSILow);

	if(RSI[sc.Index] < RSILow[sc.Index])
		PointOnHigh[sc.Index] = 1;
	else 
		PointOnHigh[sc.Index] = 0;

	if(RSI[sc.Index] > RSIHigh[sc.Index])
		PointOnLow[sc.Index] = 1;
	else
		PointOnLow[sc.Index] = 0;

}

/*==========================================================================*/
SCSFExport scsf_DailyOHLC(SCStudyInterfaceRef sc)
{
	SCInputRef UseDaySessionOnly=sc.Input[7];
	SCInputRef GraphOpenCloseHistorically = sc.Input [8];
	SCInputRef DisplayOnDaySessionOnly = sc.Input [9];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Daily OHLC";
		
		sc.ScaleRangeType = SCALE_SAMEASREGION;
		sc.GraphRegion = 0;
		sc.ValueFormat = VALUEFORMAT_INHERITED;

		sc.Subgraph[SC_OPEN].Name = "Open";
		sc.Subgraph[SC_OPEN].DrawStyle = DRAWSTYLE_DASH;
		sc.Subgraph[SC_OPEN].PrimaryColor = RGB(255,128,0);
		sc.Subgraph[SC_OPEN].DrawZeros = false;

		sc.Subgraph[SC_HIGH].Name = "High";
		sc.Subgraph[SC_HIGH].DrawStyle = DRAWSTYLE_DASH;
		sc.Subgraph[SC_HIGH].PrimaryColor = RGB(0,255,0);
		sc.Subgraph[SC_HIGH].LineWidth = 2;
		sc.Subgraph[SC_HIGH].DrawZeros = false;

		sc.Subgraph[SC_LOW].Name = "Low";
		sc.Subgraph[SC_LOW].DrawStyle = DRAWSTYLE_DASH;
		sc.Subgraph[SC_LOW].PrimaryColor = RGB(255,0,0);
		sc.Subgraph[SC_LOW].LineWidth = 2;
		sc.Subgraph[SC_LOW].DrawZeros = false;

		sc.Subgraph[SC_LAST].Name = "Close";
		sc.Subgraph[SC_LAST].DrawStyle = DRAWSTYLE_DASH;
		sc.Subgraph[SC_LAST].PrimaryColor = RGB(128,0,255);
		sc.Subgraph[SC_LAST].DrawZeros = false;

		sc.Subgraph[SC_OHLC].Name = "OHLC Avg";
		sc.Subgraph[SC_OHLC].DrawStyle = DRAWSTYLE_IGNORE;
		sc.Subgraph[SC_OHLC].PrimaryColor = RGB(128,128,128);
		sc.Subgraph[SC_OHLC].DrawZeros = false;

		sc.Subgraph[SC_HLC].Name= "HLC Avg";
		sc.Subgraph[SC_HLC].DrawStyle = DRAWSTYLE_IGNORE;
		sc.Subgraph[SC_HLC].PrimaryColor = RGB(128,128,128);
		sc.Subgraph[SC_HLC].DrawZeros = false;

		sc.Subgraph[SC_HL].Name= "HL Avg";
		sc.Subgraph[SC_HL].DrawStyle = DRAWSTYLE_IGNORE;
		sc.Subgraph[SC_HL].PrimaryColor = RGB(128,128,128);
		sc.Subgraph[SC_HL].DrawZeros = false;

		sc.Input[0].Name = "Use this Intraday Chart";
		sc.Input[0].SetYesNo(1);

		sc.Input[1].Name = "Daily Chart #";
		sc.Input[1].SetChartNumber(1);
		
		sc.Input[2].Name = "Reference Days Back";
		sc.Input[2].SetInt(1);
		sc.Input[2].SetIntLimits(0,MAX_STUDY_LENGTH);
		
		sc.Input[3].Name = "Graph High and Low Historically (0 Reference Days Back)";
		sc.Input[3].SetYesNo(0);
		
		sc.Input[4].Name = "Use Saturday Data";
		sc.Input[4].SetYesNo(0);
		
		sc.Input[5].Name = "Number of Days To Calculate";
		sc.Input[5].SetInt(50);
		sc.Input[5].SetIntLimits(1,MAX_STUDY_LENGTH);
		
		sc.Input[6].Name = "Round to Tick Size";
		sc.Input[6].SetYesNo(0);

		UseDaySessionOnly.Name = "Use Day Session Only";
		UseDaySessionOnly.SetYesNo(0);

		GraphOpenCloseHistorically.Name ="Graph Open and Close Historically (0 Reference Days Back)";
		GraphOpenCloseHistorically.SetYesNo(0);

		 DisplayOnDaySessionOnly.  Name = "Display on Day Session Only";
		 DisplayOnDaySessionOnly.  SetYesNo(true);

		return;
	}

	int InUseThisIntradayChart = sc.Input[0].GetYesNo();
	int InDailyChartNumber = sc.Input[1].GetInt();
	int InNumberOfDaysBack = sc.Input[2].GetInt();
	int InGraphHLHistorically = sc.Input[3].GetYesNo();
	int InUseSaturdayData = sc.Input[4].GetYesNo();
	int InNumberOfDaysToCalculate = sc.Input[5].GetInt();
	int InRoundToTickSize = sc.Input[6].GetYesNo();
	

	float Open = 0.0f, High = 0.0f, Low = 0.0f, Close = 0.0f;
	int IntradayChartDate = 0;
	int IsValid = 1;

	// we get chart data only once for speed
	SCGraphData DailyChartData;
	SCDateTimeArray DailyChartDateTime;
	if (!InUseThisIntradayChart)
	{
		sc.GetChartBaseData(InDailyChartNumber, DailyChartData);
		sc.GetChartDateTimeArray(InDailyChartNumber, DailyChartDateTime);
	}
	
	float HighestHigh = -FLT_MAX, LowestLow = FLT_MAX;
	
	int StartIndex = sc.UpdateStartIndex;
	if (StartIndex != 0)
	{
		SCDateTime TradingDayStartDateTime = sc.GetTradingDayStartDateTimeOfBar(sc.BaseDateTimeIn[StartIndex]);
		StartIndex = sc.GetFirstIndexForDate(sc.ChartNumber, TradingDayStartDateTime.GetDate());

	}
	
	for (int Index = StartIndex; Index < sc.ArraySize; Index++ ) 
	{

		SCDateTime BarDateTime = sc.BaseDateTimeIn[Index];

		if (IntradayChartDate != sc.GetTradingDayDate(sc.BaseDateTimeIn[Index]))
		{
			IntradayChartDate = sc.GetTradingDayDate(sc.BaseDateTimeIn[Index]);

			IsValid = 
				CalculateDailyOHLC(
					sc,
					IntradayChartDate,
					InNumberOfDaysBack,
					InNumberOfDaysToCalculate,
					InUseSaturdayData,
					InUseThisIntradayChart,
					InDailyChartNumber,
					DailyChartData,
					DailyChartDateTime,
					UseDaySessionOnly.GetYesNo(),
					Open,
					High,
					Low,
					Close
				);
			
			HighestHigh = -FLT_MAX;//sc.High[Index];
			LowestLow = FLT_MAX;//sc.Low[Index];
		}
		
		if (!IsValid)
			continue;

		if (! UseDaySessionOnly.GetYesNo() || ( UseDaySessionOnly.GetYesNo() && !sc.IsDateTimeInDaySession (BarDateTime)) ) 
		{


		}


		if (InGraphHLHistorically)
		{
			if (! UseDaySessionOnly.GetYesNo() || ( UseDaySessionOnly.GetYesNo() && sc.IsDateTimeInDaySession (BarDateTime)) )
			{
				if (sc.High[Index] > HighestHigh)
					HighestHigh = sc.High[Index];

				if (sc.Low[Index] < LowestLow)
					LowestLow = sc.Low[Index];

				High = HighestHigh;
				Low = LowestLow;
			}
			else
			{
				High = 0.0;
				Low = 0.0;
			}
		}

		if (GraphOpenCloseHistorically.  GetYesNo())
		{
			Open = sc.Open [Index];
			Close = sc.Close [Index];
		}

		float SubgraphOpen = Open;
		float SubgraphHigh = High;
		float SubgraphLow = Low;
		float SubgraphClose = Close;

		if (InRoundToTickSize)
		{
			SubgraphOpen = (float)sc.RoundToTickSize(Open, sc.TickSize);
			SubgraphHigh = (float)sc.RoundToTickSize(High, sc.TickSize);
			SubgraphLow = (float)sc.RoundToTickSize(Low, sc.TickSize);
			SubgraphClose = (float)sc.RoundToTickSize(Close, sc.TickSize);
		}


		if (  DisplayOnDaySessionOnly.GetYesNo() && !sc.IsDateTimeInDaySession (BarDateTime) ) 
		{
			SubgraphOpen = 0;
			SubgraphHigh = 0;
			SubgraphLow = 0;
			SubgraphClose = 0;
		}

		sc.Subgraph[SC_OPEN][Index] = SubgraphOpen;
		sc.Subgraph[SC_HIGH][Index] = SubgraphHigh;
		sc.Subgraph[SC_LOW][Index] = SubgraphLow;
		sc.Subgraph[SC_LAST][Index] = SubgraphClose;


		sc.CalculateOHLCAverages( Index);
	} 
}

/*==========================================================================*/
SCSFExport scsf_DailyOHLCSinglePoint(SCStudyInterfaceRef sc)
{
	if (sc.SetDefaults)
	{
		sc.GraphName = "Daily OHLC - Single Point";

		sc.ScaleRangeType = SCALE_SAMEASREGION;
		sc.GraphRegion = 0;
		sc.ValueFormat = VALUEFORMAT_INHERITED;

		sc.Subgraph[SC_OPEN].Name = "Open";
		sc.Subgraph[SC_OPEN].DrawStyle = DRAWSTYLE_HIDDEN;
		sc.Subgraph[SC_OPEN].PrimaryColor = RGB(255,128,0);
		sc.Subgraph[SC_OPEN].DrawZeros = false;

		sc.Subgraph[SC_HIGH].Name = "High";
		sc.Subgraph[SC_HIGH].DrawStyle = DRAWSTYLE_TRIANGLEDOWN;
		sc.Subgraph[SC_HIGH].PrimaryColor = RGB(0,255,0);
		sc.Subgraph[SC_HIGH].LineWidth =5;
		sc.Subgraph[SC_HIGH].DrawZeros = false;

		sc.Subgraph[SC_LOW].Name = "Low";
		sc.Subgraph[SC_LOW].DrawStyle = DRAWSTYLE_TRIANGLEUP;
		sc.Subgraph[SC_LOW].PrimaryColor = RGB(255,0,0);
		sc.Subgraph[SC_LOW].LineWidth =5;
		sc.Subgraph[SC_LOW].DrawZeros = false;

		sc.Subgraph[SC_LAST].Name = "Close";
		sc.Subgraph[SC_LAST].DrawStyle = DRAWSTYLE_HIDDEN;
		sc.Subgraph[SC_LAST].PrimaryColor = RGB(128,0,255);
		sc.Subgraph[SC_LAST].DrawZeros = false;

		sc.Input[0].Name = "Use this Intraday Chart";
		sc.Input[0].SetYesNo(1);

		sc.Input[1].Name = "Daily Chart #";
		sc.Input[1].SetChartNumber(1);

		sc.Input[4].Name = "Use Saturday Data";
		sc.Input[4].SetYesNo(0);

		sc.Input[5].Name = "Number of Days To Calculate";
		sc.Input[5].SetInt(50);
		sc.Input[5].SetIntLimits(1,MAX_STUDY_LENGTH);


		sc.Input[7].Name = "Use Day Session Only";
		sc.Input[7].SetYesNo(0);

		sc.Input[8].Name = "Display High/Low Price Labels";
		sc.Input[8].SetYesNo(0);

		return;
	}

	int InUseThisIntradayChart = sc.Input[0].GetYesNo();
	int InDailyChartNumber = sc.Input[1].GetInt();
	int InUseSaturdayData = sc.Input[4].GetYesNo();
	int InNumberOfDaysToCalculate = sc.Input[5].GetInt();
	int UseDaySessionOnly = sc.Input[7].GetYesNo();
	int DisplayPriceLabels = sc.Input[8].GetYesNo();

	float Open = 0.0f, High = 0.0f, Low = 0.0f, Close = 0.0f;
	int IntradayChartDate = 0;
	int IsValid = 1;
	const int HighUniqueId = 825390000;
	const int LowUniqueId  = 925390000;

	// we get chart data only once for speed
	SCGraphData DailyChartData;
	SCDateTimeArray DailyChartDateTime;
	if (!InUseThisIntradayChart)
	{
		sc.GetChartBaseData(InDailyChartNumber, DailyChartData);
		sc.GetChartDateTimeArray(InDailyChartNumber, DailyChartDateTime);
	}


	int StartIndex = sc.UpdateStartIndex;
	if (StartIndex != 0)
	{
		SCDateTime TradingDayStartDateTime = sc.GetTradingDayStartDateTimeOfBar(sc.BaseDateTimeIn[StartIndex]);
		//StartIndex = sc.GetContainingIndexForSCDateTime(sc.ChartNumber, TradingDayStartDateTime);
		StartIndex = sc.GetFirstIndexForDate(sc.ChartNumber, TradingDayStartDateTime.GetDate());
	}

	for (int Index = StartIndex; Index < sc.ArraySize; Index++ ) 
	{
		if (IntradayChartDate != sc.GetTradingDayDate(sc.BaseDateTimeIn[Index]))
		{
			IntradayChartDate = sc.GetTradingDayDate(sc.BaseDateTimeIn[Index]);

			IsValid = 
				CalculateDailyOHLC(
				sc,
				IntradayChartDate,
				0,
				InNumberOfDaysToCalculate,
				InUseSaturdayData,
				InUseThisIntradayChart,
				InDailyChartNumber,
				DailyChartData,
				DailyChartDateTime,
				UseDaySessionOnly,
				Open,
				High,
				Low,
				Close
				);

		}

		if (!IsValid)
			continue;


		if(sc.FormattedEvaluate(Open,sc.BaseGraphValueFormat,EQUAL_OPERATOR,sc.Open[Index], sc.BaseGraphValueFormat))
			sc.Subgraph[SC_OPEN][Index] = Open;
		else
			sc.Subgraph[SC_OPEN][Index] = 0.0;

		if(sc.FormattedEvaluate(High,sc.BaseGraphValueFormat,EQUAL_OPERATOR,sc.High[Index], sc.BaseGraphValueFormat))
		{
			sc.Subgraph[SC_HIGH][Index] = High;
			
			if (DisplayPriceLabels)
			{
				s_UseTool Tool;
				Tool.ChartNumber = sc.ChartNumber;
				Tool.DrawingType = DRAWING_TEXT;
				Tool.Region		 = sc.GraphRegion;

				Tool.LineNumber  = HighUniqueId + Index;
				Tool.BeginIndex  = Index;
				Tool.BeginValue	 = High;

				Tool.Text = "  ";
				Tool.Text += sc.FormatGraphValue(High, sc.BaseGraphValueFormat);
				Tool.Color = sc.Subgraph[SC_HIGH].PrimaryColor;
				Tool.TextAlignment	= DT_LEFT | DT_VCENTER;

				Tool.AddMethod = UTAM_ADD_OR_ADJUST;

				sc.UseTool(Tool);
			}
		}
		else
			sc.Subgraph[SC_HIGH][Index] = 0.0;


		if(sc.FormattedEvaluate(Low,sc.BaseGraphValueFormat,EQUAL_OPERATOR,sc.Low[Index], sc.BaseGraphValueFormat))
		{
			sc.Subgraph[SC_LOW][Index] = Low;

			if (DisplayPriceLabels)
			{
				s_UseTool Tool;
				Tool.ChartNumber = sc.ChartNumber;
				Tool.DrawingType = DRAWING_TEXT;
				Tool.Region		 = sc.GraphRegion;

				Tool.LineNumber  = LowUniqueId + Index;
				Tool.BeginIndex  = Index;
				Tool.BeginValue	 = Low;

				Tool.Text = "  ";
				Tool.Text += sc.FormatGraphValue(Low, sc.BaseGraphValueFormat);
				Tool.Color = sc.Subgraph[SC_LOW].PrimaryColor;
				Tool.TextAlignment	= DT_LEFT | DT_VCENTER;

				Tool.AddMethod = UTAM_ADD_OR_ADJUST;

				sc.UseTool(Tool);
			}
		}
		else
			sc.Subgraph[SC_LOW][Index] = 0.0;


		if(sc.FormattedEvaluate(Close,sc.BaseGraphValueFormat,EQUAL_OPERATOR,sc.Close[Index], sc.BaseGraphValueFormat))
			sc.Subgraph[SC_LAST][Index] = Close;
		else
			sc.Subgraph[SC_LAST][Index] = 0.0;
	} 
}

/*==========================================================================*/
SCSFExport scsf_PivotPointsDaily(SCStudyInterfaceRef sc)
{
	SCSubgraphRef R1 = sc.Subgraph[0];
	SCSubgraphRef R2 = sc.Subgraph[1];
	SCSubgraphRef S1 = sc.Subgraph[2];
	SCSubgraphRef S2 = sc.Subgraph[3];
	SCSubgraphRef R0_5 = sc.Subgraph[4];
	SCSubgraphRef R1_5 = sc.Subgraph[5];
	SCSubgraphRef R2_5 = sc.Subgraph[6];
	SCSubgraphRef R3 = sc.Subgraph[7];
	SCSubgraphRef S0_5 = sc.Subgraph[8];
	SCSubgraphRef S1_5 = sc.Subgraph[9];
	SCSubgraphRef S2_5 = sc.Subgraph[10];
	SCSubgraphRef S3 = sc.Subgraph[11];
	SCSubgraphRef PP = sc.Subgraph[12];
	SCSubgraphRef R4 = sc.Subgraph[13];
	SCSubgraphRef S4 = sc.Subgraph[14];
	SCSubgraphRef R3_5 = sc.Subgraph[15];
	SCSubgraphRef S3_5 = sc.Subgraph[16];
	SCSubgraphRef R5 = sc.Subgraph[17];
	SCSubgraphRef S5 = sc.Subgraph[18];

	
	SCInputRef FormulaType = sc.Input[4];
	SCInputRef NumberOfDays = sc.Input[5];
	SCInputRef RoundToTickSize = sc.Input[6];
	SCInputRef UseSaturdayData = sc.Input[7];
	SCInputRef DailyChartNumber = sc.Input[9];//previously input 3
	SCInputRef ReferenceDailyChartForData = sc.Input[10];
	SCInputRef ForwardProjectLines = sc.Input[11];
	SCInputRef UseManualValues = sc.Input[12];
	SCInputRef UserOpen = sc.Input[13];
	SCInputRef UserHigh = sc.Input[14];
	SCInputRef UserLow = sc.Input[15];
	SCInputRef UserClose = sc.Input[16];
	SCInputRef UseDaySessionOnly = sc.Input[17];
	SCInputRef Version = sc.Input[18];

	if (sc.SetDefaults)
	{

		sc.GraphName = "Pivot Points-Daily";
		
		sc.ScaleRangeType = SCALE_SAMEASREGION;

		sc.GraphRegion = 0;
		sc.ValueFormat = VALUEFORMAT_INHERITED;

		R1.Name = "R1";
		R2.Name = "R2";
		S1.Name = "S1";
		S2.Name = "S2";
		R0_5.Name = "R.5";
		R1_5.Name = "R1.5";
		R2_5.Name = "R2.5";
		R3.Name = "R3";
		S0_5.Name = "S.5";
		S1_5.Name = "S1.5";
		S2_5.Name = "S2.5";
		S3.Name = "S3";
		PP.Name = "PP";
		R4.Name = "R4";
		S4.Name = "S4";
		R3_5.Name = "R3.5";
		S3_5.Name = "S3.5";
		R5.Name = "R5";
		S5.Name = "S5";

		R1.DrawStyle = DRAWSTYLE_DASH;
		R2.DrawStyle = DRAWSTYLE_DASH;
		S1.DrawStyle = DRAWSTYLE_DASH;
		S2.DrawStyle = DRAWSTYLE_DASH;
		R0_5.DrawStyle = DRAWSTYLE_HIDDEN;
		R1_5.DrawStyle = DRAWSTYLE_HIDDEN;
		R2_5.DrawStyle = DRAWSTYLE_HIDDEN;
		R3.DrawStyle = DRAWSTYLE_DASH;
		S0_5.DrawStyle = DRAWSTYLE_HIDDEN;
		S1_5.DrawStyle = DRAWSTYLE_HIDDEN;
		S2_5.DrawStyle = DRAWSTYLE_HIDDEN;
		S3.DrawStyle = DRAWSTYLE_DASH;
		PP.DrawStyle = DRAWSTYLE_DASH;
		R4.DrawStyle = DRAWSTYLE_HIDDEN;
		S4.DrawStyle = DRAWSTYLE_HIDDEN;
		R3_5.DrawStyle = DRAWSTYLE_HIDDEN;
		S3_5.DrawStyle = DRAWSTYLE_HIDDEN;
		R5.DrawStyle = DRAWSTYLE_HIDDEN;
		S5.DrawStyle = DRAWSTYLE_HIDDEN;

		
		R1.DrawZeros = false;
		R2.DrawZeros = false;
		S1.DrawZeros = false;
		S2.DrawZeros = false;
		R0_5.DrawZeros = false;
		R1_5.DrawZeros = false;
		R2_5.DrawZeros = false;
		R3.DrawZeros = false;
		S0_5.DrawZeros = false;
		S1_5.DrawZeros = false;
		S2_5.DrawZeros = false;
		S3.DrawZeros = false;
		PP.DrawZeros = false;
		R4.DrawZeros = false;
		S4.DrawZeros = false;
		R3_5.DrawZeros = false;
		S3_5.DrawZeros = false;
		R5.DrawZeros = false;
		S5.DrawZeros = false;
		

		PP.PrimaryColor = RGB(255,0,255);

		R1.PrimaryColor = RGB(255,0,0);
		R2.PrimaryColor = RGB(255,0,0);
		R0_5.PrimaryColor = RGB(255,0,0);
		R1_5.PrimaryColor = RGB(255,0,0);
		R2_5.PrimaryColor = RGB(255,0,0);
		R3.PrimaryColor = RGB(255,0,0);
		R4.PrimaryColor = RGB(255, 0, 0);
		R3_5.PrimaryColor = RGB(255, 0, 0);
		R5.PrimaryColor = RGB(255, 0, 0);

		S1.PrimaryColor = RGB(0,255,0);
		S2.PrimaryColor = RGB(0,255,0);
		S0_5.PrimaryColor = RGB(0,255,0);
		S1_5.PrimaryColor = RGB(0,255,0);
		S2_5.PrimaryColor = RGB(0,255,0);
		S3.PrimaryColor = RGB(0,255,0);
		S4.PrimaryColor = RGB(0,255,0);
		S3_5.PrimaryColor = RGB(0,255,0);
		S5.PrimaryColor = RGB(0,255,0);

		for (int i = 0; i <= 18; i++)
		{
			sc.Subgraph[i].LineLabel = LL_DISPLAY_NAME | LL_DISPLAY_VALUE | LL_NAME_ALIGN_ABOVE | LL_NAME_ALIGN_LEFT | LL_VALUE_ALIGN_CENTER | LL_VALUE_ALIGN_VALUES_SCALE;
		}

		sc.Input[3].SetChartNumber( 1);

		DailyChartNumber.Name = "Daily Chart #";
		DailyChartNumber.SetChartNumber(1);

		FormulaType.Name = "Formula Type";
		FormulaType.SetInt(0);

		NumberOfDays.Name = "Number of Days To Calculate";
		NumberOfDays.SetInt(50);
		NumberOfDays.SetIntLimits(1,MAX_STUDY_LENGTH);

		RoundToTickSize.Name = "Round to Tick Size";
		RoundToTickSize.SetYesNo(0);

		UseSaturdayData.Name = "Use Saturday Data";
		UseSaturdayData.SetYesNo(0);

		ReferenceDailyChartForData.Name = "Reference Daily Chart For Data";
		ReferenceDailyChartForData.SetYesNo(false);

		ForwardProjectLines.Name ="Forward Project Pivot Point Lines";
		ForwardProjectLines.SetYesNo(0);
		
		UseManualValues.Name = "Use User Entered OHLC Values";
		UseManualValues.SetYesNo(0);
		
		UserOpen.Name = "User Entered Open Value";
		UserOpen.SetFloat(0.0f);
		
		UserHigh.Name = "User Entered High Value";
		UserHigh.SetFloat(0.0f);
		
		UserLow.Name = "User Entered Low Value";
		UserLow.SetFloat(0.0f);
		
		UserClose.Name = "User Entered Close Value";
		UserClose.SetFloat(0.0f);

		UseDaySessionOnly.Name = "Use Day Session Only";
		UseDaySessionOnly.SetYesNo(false);

		Version.SetInt(2);
		
		return;
	}
	//Upgrade code
	if (Version.GetInt()<2)
	{
		Version.SetInt(2);
		DailyChartNumber.SetInt(sc.Input[3].GetInt() );
		ReferenceDailyChartForData .SetYesNo(!ReferenceDailyChartForData .GetYesNo());
	}

	float fPivotPoint= 0;

	float fR0_5 = 0, fR1 = 0, fR1_5 = 0, fR2 = 0, fR2_5 = 0, fR3 = 0, fR4 = 0, fR3_5 = 0,  fR5 = 0;
	float fS0_5 = 0, fS1 = 0, fS1_5 = 0, fS2 = 0, fS2_5 = 0, fS3 = 0, fS4 = 0, fS3_5 = 0,  fS5 = 0;
	
	int IntradayChartDate = 0;
	int ValidPivotPoint = 1;

	// we get chart data only once for speed
	SCGraphData DailyChartData;
	SCDateTimeArray DailyChartDateTime;
	if (ReferenceDailyChartForData.GetYesNo() )
	{
		sc.GetChartBaseData(DailyChartNumber.GetChartNumber(), DailyChartData);
		sc.GetChartDateTimeArray(DailyChartNumber.GetChartNumber(), DailyChartDateTime);
	}

	int NumberOfForwardBars = 0;

	if(ForwardProjectLines.GetYesNo())
	{
		NumberOfForwardBars = 20;

		if(sc.UpdateStartIndex == 0)
		{
			PP.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			R1.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			R2.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			S1.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			S2.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			R0_5.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			R1_5.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			R2_5.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			R3.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			S0_5.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			S1_5.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			S2_5.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			S3.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			R4.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			S4.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			R3_5.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			S3_5.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			R5.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			S5.ExtendedArrayElementsToGraph = NumberOfForwardBars;
		}

	}

	for (int Index = sc.UpdateStartIndex; Index < sc.ArraySize+NumberOfForwardBars; Index++ ) 
	{
		if (IntradayChartDate != sc.GetTradingDayDate(sc.BaseDateTimeIn[Index]))
		{
			IntradayChartDate = sc.GetTradingDayDate(sc.BaseDateTimeIn[Index]);

			ValidPivotPoint = 
				CalculateDailyPivotPoints(
					sc,
					IntradayChartDate,
					FormulaType.GetInt(),
					DailyChartNumber.GetChartNumber(),
					DailyChartData,
					DailyChartDateTime,
					NumberOfDays.GetInt(),
					UseSaturdayData.GetYesNo(),
					ReferenceDailyChartForData.GetYesNo() ? 0 : 1,
					UseManualValues.GetYesNo(),
					UserOpen.GetFloat(),
					UserHigh.GetFloat(),
					UserLow.GetFloat(),
					UserClose.GetFloat(),
					UseDaySessionOnly.GetYesNo(),
					fPivotPoint,
					fR0_5,
					fR1, fR1_5,
					fR2, fR2_5,
					fR3,
					fS0_5,
					fS1, fS1_5,
					fS2, fS2_5,
					fS3,
					fR3_5,
					fS3_5,
					fR4,
					fS4,
					fR5,
					fS5
				);
		}

		if (!ValidPivotPoint)
			continue;
		
		if (RoundToTickSize.GetYesNo() != 0)
		{
			R1[Index] = (float)sc.RoundToTickSize(fR1, sc.TickSize);
			R2[Index] = (float)sc.RoundToTickSize(fR2, sc.TickSize);
			S1[Index] = (float)sc.RoundToTickSize(fS1, sc.TickSize);
			S2[Index] = (float)sc.RoundToTickSize(fS2, sc.TickSize);

			R0_5[Index] = (float)sc.RoundToTickSize(fR0_5, sc.TickSize);
			R1_5[Index] = (float)sc.RoundToTickSize(fR1_5, sc.TickSize);
			R2_5[Index] = (float)sc.RoundToTickSize(fR2_5, sc.TickSize);
			R3[Index] = (float)sc.RoundToTickSize(fR3, sc.TickSize);
			S0_5[Index] = (float)sc.RoundToTickSize(fS0_5, sc.TickSize);
			S1_5[Index] = (float)sc.RoundToTickSize(fS1_5, sc.TickSize);
			S2_5[Index] = (float)sc.RoundToTickSize(fS2_5, sc.TickSize);
			S3[Index] = (float)sc.RoundToTickSize(fS3, sc.TickSize);
			PP[Index] = (float)sc.RoundToTickSize(fPivotPoint, sc.TickSize);
			R4[Index] = (float)sc.RoundToTickSize(fR4, sc.TickSize);
			S4[Index] = (float)sc.RoundToTickSize(fS4, sc.TickSize);
			R3_5[Index] = (float)sc.RoundToTickSize(fR3_5, sc.TickSize);
			S3_5[Index] = (float)sc.RoundToTickSize(fS3_5, sc.TickSize);
			R5[Index] = (float)sc.RoundToTickSize(fR5, sc.TickSize);
			S5[Index] = (float)sc.RoundToTickSize(fS5, sc.TickSize);
		}
		else 
		{
			R1[Index] = fR1;
			R2[Index] = fR2;
			S1[Index] = fS1;
			S2[Index] = fS2;

			R0_5[Index] = fR0_5;
			R1_5[Index] = fR1_5;
			R2_5[Index] = fR2_5;
			R3[Index] = fR3;
			S0_5[Index] = fS0_5;
			S1_5[Index] = fS1_5;
			S2_5[Index] = fS2_5;
			S3[Index] = fS3;
			PP[Index] = fPivotPoint;
			R4[Index] = fR4;
			S4[Index] = fS4;
			R3_5[Index] = fR3_5;
			S3_5[Index] = fS3_5;
			R5[Index] = fR5;
			S5[Index] = fS5;
		}
	} 
}

/*==========================================================================*/

SCSFExport scsf_PivotPointsVariablePeriod(SCStudyInterfaceRef sc)
{
	SCSubgraphRef R1 = sc.Subgraph[0];
	SCSubgraphRef R2 = sc.Subgraph[1];
	SCSubgraphRef S1 = sc.Subgraph[2];
	SCSubgraphRef S2 = sc.Subgraph[3];
	SCSubgraphRef R0_5 = sc.Subgraph[4];
	SCSubgraphRef R1_5 = sc.Subgraph[5];
	SCSubgraphRef R2_5 = sc.Subgraph[6];
	SCSubgraphRef R3 = sc.Subgraph[7];
	SCSubgraphRef S0_5 = sc.Subgraph[8];
	SCSubgraphRef S1_5 = sc.Subgraph[9];
	SCSubgraphRef S2_5 = sc.Subgraph[10];
	SCSubgraphRef S3 = sc.Subgraph[11];
	SCSubgraphRef PP = sc.Subgraph[12];
	SCSubgraphRef R4 = sc.Subgraph[13];
	SCSubgraphRef S4 = sc.Subgraph[14];
	SCSubgraphRef R3_5 = sc.Subgraph[15];
	SCSubgraphRef S3_5 = sc.Subgraph[16];
	SCSubgraphRef R5 = sc.Subgraph[17];
	SCSubgraphRef S5 = sc.Subgraph[18];

	SCInputRef TimePeriodType = sc.Input[0];
	SCInputRef TimePeriodLength = sc.Input[1];
	SCInputRef FormulaType = sc.Input[2];
	SCInputRef MinimumRequiredTP = sc.Input[3];
	SCInputRef RoundToTickSize = sc.Input[4];
	SCInputRef DisplayDebuggingOutput = sc.Input[5];
	SCInputRef ForwardProjectLines = sc.Input[6];
	SCInputRef NumberOfDaysToCalculate = sc.Input[7];
	SCInputRef AutoSkipPeriodOfNoTrading = sc.Input[8];
	SCInputRef NumberForwardProjectionBars = sc.Input[9];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Pivot Points-Variable Period";

		sc.ScaleRangeType = SCALE_SAMEASREGION;

		sc.GraphRegion = 0;
		sc.ValueFormat = VALUEFORMAT_INHERITED;
		sc.AutoLoop = 0;

		R1.Name = "R1";
		R2.Name = "R2";
		S1.Name = "S1";
		S2.Name = "S2";
		R0_5.Name = "R.5";
		R1_5.Name = "R1.5";
		R2_5.Name = "R2.5";
		R3.Name = "R3";
		S0_5.Name = "S.5";
		S1_5.Name = "S1.5";
		S2_5.Name = "S2.5";
		S3.Name = "S3";
		PP.Name = "PP";
		R4.Name = "R4";
		S4.Name = "S4";
		R3_5.Name = "R3.5";
		S3_5.Name = "S3.5";
		R5.Name = "R5";
		S5.Name = "S5";

		R1.DrawStyle = DRAWSTYLE_DASH;
		R2.DrawStyle = DRAWSTYLE_DASH;
		S1.DrawStyle = DRAWSTYLE_DASH;
		S2.DrawStyle = DRAWSTYLE_DASH;
		R0_5.DrawStyle = DRAWSTYLE_HIDDEN;
		R1_5.DrawStyle = DRAWSTYLE_HIDDEN;
		R2_5.DrawStyle = DRAWSTYLE_HIDDEN;
		R3.DrawStyle = DRAWSTYLE_DASH;
		S0_5.DrawStyle = DRAWSTYLE_HIDDEN;
		S1_5.DrawStyle = DRAWSTYLE_HIDDEN;
		S2_5.DrawStyle = DRAWSTYLE_HIDDEN;
		S3.DrawStyle = DRAWSTYLE_DASH;
		PP.DrawStyle = DRAWSTYLE_DASH;
		R4.DrawStyle = DRAWSTYLE_HIDDEN;
		S4.DrawStyle = DRAWSTYLE_HIDDEN;
		R3_5.DrawStyle = DRAWSTYLE_HIDDEN;
		S3_5.DrawStyle = DRAWSTYLE_HIDDEN;
		R5.DrawStyle = DRAWSTYLE_HIDDEN;
		S5.DrawStyle = DRAWSTYLE_HIDDEN;

		
		R1.DrawZeros = false;
		R2.DrawZeros = false;
		S1.DrawZeros = false;
		S2.DrawZeros = false;
		R0_5.DrawZeros = false;
		R1_5.DrawZeros = false;
		R2_5.DrawZeros = false;
		R3.DrawZeros = false;
		S0_5.DrawZeros = false;
		S1_5.DrawZeros = false;
		S2_5.DrawZeros = false;
		S3.DrawZeros = false;
		PP.DrawZeros = false;
		R4.DrawZeros = false;
		S4.DrawZeros = false;
		R3_5.DrawZeros = false;
		S3_5.DrawZeros = false;
		R5.DrawZeros = false;
		S5.DrawZeros = false;

		PP.PrimaryColor = RGB(255,0,255);

		R1.PrimaryColor = RGB(255,0,0);
		R2.PrimaryColor = RGB(255,0,0);
		R0_5.PrimaryColor = RGB(255,0,0);
		R1_5.PrimaryColor = RGB(255,0,0);
		R2_5.PrimaryColor = RGB(255,0,0);
		R3.PrimaryColor = RGB(255,0,0);
		R4.PrimaryColor = RGB(255, 0, 0);
		R3_5.PrimaryColor = RGB(255, 0, 0);
		R5.PrimaryColor = RGB(255, 0, 0);

		S1.PrimaryColor = RGB(0,255,0);
		S2.PrimaryColor = RGB(0,255,0);
		S0_5.PrimaryColor = RGB(0,255,0);
		S1_5.PrimaryColor = RGB(0,255,0);
		S2_5.PrimaryColor = RGB(0,255,0);
		S3.PrimaryColor = RGB(0,255,0);
		S4.PrimaryColor = RGB(0,255,0);
		S3_5.PrimaryColor = RGB(0,255,0);
		S5.PrimaryColor = RGB(0,255,0);

		for (int i = 0; i <= 18; i++)
		{
			sc.Subgraph[i].LineLabel = 
				LL_DISPLAY_NAME | LL_DISPLAY_VALUE | LL_NAME_ALIGN_ABOVE | LL_NAME_ALIGN_LEFT | 
				LL_VALUE_ALIGN_CENTER | LL_VALUE_ALIGN_VALUES_SCALE;
		}

		TimePeriodType.Name = "Time Period Type";
		TimePeriodType.SetTimePeriodType(TIME_PERIOD_LENGTH_UNIT_MINUTES);

		TimePeriodLength.Name = "Time Period Length";
		TimePeriodLength.SetInt(60);
		TimePeriodLength.SetIntLimits(1, 7*MINUTES_PER_DAY);

		FormulaType.Name = "Formula Type";
		FormulaType.SetInt(0);

		MinimumRequiredTP.Name = "Minimum Required Time Period as %";
		MinimumRequiredTP.SetFloat(25.0f);
		MinimumRequiredTP.SetFloatLimits(1.0f, 100.0f);

		RoundToTickSize.Name = "Round to Tick Size";
		RoundToTickSize.SetYesNo(0);

		DisplayDebuggingOutput.Name = "Display Debugging Output (slows study calculations)";
		DisplayDebuggingOutput.SetYesNo(0);

		ForwardProjectLines.Name ="Forward Project Pivot Point Lines";
		ForwardProjectLines.SetYesNo(0);

		NumberOfDaysToCalculate.Name = "Number Of Days To Calculate";
		NumberOfDaysToCalculate.SetInt(1000);

		AutoSkipPeriodOfNoTrading.Name = "Auto Skip Period Of No Trading";
		AutoSkipPeriodOfNoTrading.SetYesNo(false);

		NumberForwardProjectionBars.Name = "Number of Forward Project Bars";
		NumberForwardProjectionBars.SetInt(20);
		NumberForwardProjectionBars.SetIntLimits(1,200);
		return;
	}

	float fPivotPoint= 0;

	float fR0_5 = 0, fR1 = 0, fR1_5 = 0, fR2 = 0, fR2_5 = 0, fR3 = 0, fR4 = 0, fR3_5 = 0, fR5 = 0;
	float fS0_5 = 0, fS1 = 0, fS1_5 = 0, fS2 = 0, fS2_5 = 0, fS3 = 0, fS4 = 0, fS3_5 = 0, fS5 = 0;

	int ValidPivotPoint = 1;

	int PeriodLength = TimePeriodLength.GetInt();

	int NumberOfForwardBars = 0;

	if (ForwardProjectLines.GetYesNo())
	{
		NumberOfForwardBars = NumberForwardProjectionBars.GetInt();

		if(sc.UpdateStartIndex == 0)
		{
			PP.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			R1.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			R2.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			S1.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			S2.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			R0_5.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			R1_5.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			R2_5.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			R3.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			S0_5.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			S1_5.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			S2_5.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			S3.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			R4.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			S4.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			R3_5.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			S3_5.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			R5.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			S5.ExtendedArrayElementsToGraph = NumberOfForwardBars;

		}
	}

	if (NumberOfDaysToCalculate.GetInt() < 1)
		NumberOfDaysToCalculate.SetInt(1000);

	int LastDateInChart = sc.BaseDateTimeIn[sc.ArraySize - 1].GetDate();
	int FirstDateToCalculate = LastDateInChart - NumberOfDaysToCalculate.GetInt() + 1;

	float Open = 0, High = 0, Low = 0, Close = 0, NextOpen = 0;

	SCDateTime BeginOfRefDateTime,EndOfRefDateTime;
	SCDateTime CurrentPeriodBeginDateTime;
	SCDateTime CurrentPeriodEndDateTime;

	SCDateTime PriorCurrentPeriodStartDateTime;



	for (int index = sc.UpdateStartIndex; index < sc.ArraySize+NumberOfForwardBars; index++ )
	{

		SCDateTime CurrentBarDT = sc.BaseDateTimeIn[index];
		

		if (CurrentBarDT < FirstDateToCalculate)
			continue;

		bool GetReferenceData = true;

		CurrentPeriodBeginDateTime = sc.GetStartOfPeriodForDateTime(CurrentBarDT, TimePeriodType.GetTimePeriodType(),  PeriodLength, 0);

		if(	CurrentPeriodBeginDateTime == PriorCurrentPeriodStartDateTime)
			GetReferenceData  = false;


		PriorCurrentPeriodStartDateTime =	CurrentPeriodBeginDateTime;

		SCDateTime TimeIncrement = sc.TimePeriodSpan(TimePeriodType.GetTimePeriodType(),  PeriodLength);

		if(GetReferenceData)
		{

			BeginOfRefDateTime= sc.GetStartOfPeriodForDateTime( 	CurrentPeriodBeginDateTime, TimePeriodType.GetTimePeriodType(), PeriodLength, -1);
			EndOfRefDateTime=  	CurrentPeriodBeginDateTime - (1*SECONDS);
			
			CurrentPeriodEndDateTime=sc.GetStartOfPeriodForDateTime( 	CurrentPeriodBeginDateTime, TimePeriodType.GetTimePeriodType(),  PeriodLength, 1) - (1*SECONDS);

			if (DisplayDebuggingOutput.GetYesNo() != 0)
			{
				SCString Message;

				Message.Format("Current Bar: %s, BeginOfRefDateTime: %s, EndOfRefDateTime: %s, CPBeginDateTime: %s, CPEndDateTime: %s",
					sc.FormatDateTime(CurrentBarDT).GetChars(),sc.FormatDateTime(BeginOfRefDateTime).GetChars(),
					sc.FormatDateTime(EndOfRefDateTime).GetChars(),sc.FormatDateTime(CurrentPeriodBeginDateTime).GetChars(),
					sc.FormatDateTime(CurrentPeriodEndDateTime).GetChars());

				sc.AddMessageToLog(Message,0);

			}

			int MaxPeriodsToGoBack = 1;

			if(AutoSkipPeriodOfNoTrading.GetYesNo())
				MaxPeriodsToGoBack = 32;

			for (int WalkBack = 0; WalkBack < MaxPeriodsToGoBack; WalkBack++)
			{
				if (WalkBack >= 1) //Walk back 1 period.
				{

					SCDateTime  PriorBeginOfRefDateTime = BeginOfRefDateTime;

					BeginOfRefDateTime= sc.GetStartOfPeriodForDateTime(BeginOfRefDateTime, TimePeriodType.GetTimePeriodType(),  PeriodLength, -1);
					EndOfRefDateTime=  	 PriorBeginOfRefDateTime- (1*SECONDS);

					if (DisplayDebuggingOutput.GetYesNo() != 0)
					{
						SCString Message;

						Message.Format("Moving back 1 period. BeginOfRefDateTime: %s, EndOfRefDateTime: %s.",
							sc.FormatDateTime(BeginOfRefDateTime).GetChars(),sc.FormatDateTime(EndOfRefDateTime).GetChars());

						sc.AddMessageToLog(Message,0);
					}
				}

				int NumberOfBars = 0;
				SCDateTime TotalTimeSpan;
				int Result = sc.GetOHLCOfTimePeriod(BeginOfRefDateTime, EndOfRefDateTime, Open, High, Low, Close, NextOpen, NumberOfBars, TotalTimeSpan);
				if (!Result)
					continue;

				if (DisplayDebuggingOutput.GetYesNo() != 0)
				{
					SCString Message;
					Message.Format("Number of Bars: %d, Total Time Span In Minutes: %d",NumberOfBars,(int)(TotalTimeSpan/MINUTES+0.5));
					sc.AddMessageToLog(Message,0);

					Message.Format("RefOpen %f,RefHigh %f,RefLow %f,RefClose %f,RefNextOpen %f.",Open, High, Low, Close, NextOpen);
					sc.AddMessageToLog(Message,0);
				}

				SCDateTime MinimumTimeSpan = (TimeIncrement * MinimumRequiredTP.GetFloat()/100.0f);
				if (TotalTimeSpan >= MinimumTimeSpan)
					break;
			}
		}


		ValidPivotPoint = 
			CalculatePivotPoints(Open,High,Low,Close,NextOpen,
			fPivotPoint, fR0_5, fR1, fR1_5, fR2, fR2_5, fR3, fS0_5, 
			fS1, fS1_5, fS2, fS2_5, fS3, fR3_5, fS3_5, fR4, fS4, fR5, fS5, FormulaType.GetInt());				


		if (!ValidPivotPoint)
			continue;

		if (RoundToTickSize.GetYesNo() != 0)
		{
			R1[index] = (float)sc.RoundToTickSize(fR1, sc.TickSize);
			R2[index] = (float)sc.RoundToTickSize(fR2, sc.TickSize);
			S1[index] = (float)sc.RoundToTickSize(fS1, sc.TickSize);
			S2[index] = (float)sc.RoundToTickSize(fS2, sc.TickSize);

			R0_5[index] = (float)sc.RoundToTickSize(fR0_5, sc.TickSize);
			R1_5[index] = (float)sc.RoundToTickSize(fR1_5, sc.TickSize);
			R2_5[index] = (float)sc.RoundToTickSize(fR2_5, sc.TickSize);
			R3[index] = (float)sc.RoundToTickSize(fR3, sc.TickSize);
			S0_5[index] = (float)sc.RoundToTickSize(fS0_5, sc.TickSize);
			S1_5[index] = (float)sc.RoundToTickSize(fS1_5, sc.TickSize);
			S2_5[index] = (float)sc.RoundToTickSize(fS2_5, sc.TickSize);
			S3[index] = (float)sc.RoundToTickSize(fS3, sc.TickSize);
			PP[index] = (float)sc.RoundToTickSize(fPivotPoint, sc.TickSize);
			R4[index] = (float)sc.RoundToTickSize(fR4, sc.TickSize);
			S4[index] = (float)sc.RoundToTickSize(fS4, sc.TickSize);
			R3_5[index] = (float)sc.RoundToTickSize(fR3_5, sc.TickSize);
			S3_5[index] = (float)sc.RoundToTickSize(fS3_5, sc.TickSize);
			R5[index] = (float)sc.RoundToTickSize(fR5, sc.TickSize);
			S5[index] = (float)sc.RoundToTickSize(fS5, sc.TickSize);
		}
		else 
		{
			R1[index] = fR1;
			R2[index] = fR2;
			S1[index] = fS1;
			S2[index] = fS2;

			R0_5[index] = fR0_5;
			R1_5[index] = fR1_5;
			R2_5[index] = fR2_5;
			R3[index] = fR3;
			S0_5[index] = fS0_5;
			S1_5[index] = fS1_5;
			S2_5[index] = fS2_5;
			S3[index] = fS3;
			PP[index] = fPivotPoint;
			R4[index] = fR4;
			S4[index] = fS4;
			R3_5[index] = fR3_5;
			S3_5[index] = fS3_5;
			R5[index] = fR5;
			S5[index] = fS5;
		}
	} //for
}

/*==========================================================================*/

SCSFExport scsf_PivotRangeVariablePeriod(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Top = sc.Subgraph[0];
	SCSubgraphRef PPTop = sc.Subgraph[1];
	SCSubgraphRef PPBottom = sc.Subgraph[2];
	SCSubgraphRef Bottom = sc.Subgraph[3];
	

	SCInputRef TimePeriodType = sc.Input[0];
	SCInputRef TimePeriodLength = sc.Input[1];
	
	SCInputRef MinimumRequiredTP = sc.Input[3];
	SCInputRef RoundToTickSize = sc.Input[4];
	SCInputRef DisplayDebuggingOutput = sc.Input[5];
	SCInputRef ForwardProjectLines = sc.Input[6];
	SCInputRef NumberOfDaysToCalculate = sc.Input[7];
	SCInputRef AutoSkipPeriodOfNoTrading = sc.Input[8];


	if (sc.SetDefaults)
	{
		sc.GraphName = "Pivot Range-Variable Period";

		sc.ScaleRangeType = SCALE_SAMEASREGION;

		sc.GraphRegion = 0;
		sc.ValueFormat = VALUEFORMAT_INHERITED;
		sc.DrawStudyUnderneathMainPriceGraph= true;

		sc.AutoLoop = 0;

		Top.Name = "Top Range";
		PPTop.Name = "PP";
		PPBottom.Name = "PP";
		Bottom.Name = "Bottom Range";
		
		Top.DrawStyle = DRAWSTYLE_FILLRECTTOP;
		PPTop.DrawStyle = DRAWSTYLE_FILLRECTBOTTOM;
		PPBottom.DrawStyle = DRAWSTYLE_FILLRECTTOP;
		Bottom.DrawStyle = DRAWSTYLE_FILLRECTBOTTOM;

		Top.DrawZeros = false;
		PPTop.DrawZeros = false;
		PPBottom.DrawZeros = false;
		Bottom.DrawZeros = false;

		Top.PrimaryColor = RGB(128, 255, 255);
		PPTop.PrimaryColor = RGB(128, 255, 255);
		PPBottom.PrimaryColor = RGB(255, 255, 128);
		Bottom.PrimaryColor = RGB(255, 255, 128);
	

		for (int i = 0; i <= 4; i++)
		{
			sc.Subgraph[i].LineLabel = 
				LL_DISPLAY_NAME | LL_DISPLAY_VALUE | LL_NAME_ALIGN_ABOVE | LL_NAME_ALIGN_RIGHT| 
				LL_VALUE_ALIGN_CENTER | LL_VALUE_ALIGN_VALUES_SCALE;
		}

		TimePeriodType.Name = "Time Period Type";
		TimePeriodType.SetTimePeriodType(TIME_PERIOD_LENGTH_UNIT_MINUTES);

		TimePeriodLength.Name = "Time Period Length";
		TimePeriodLength.SetInt(60);
		TimePeriodLength.SetIntLimits(1, 7*MINUTES_PER_DAY);


		MinimumRequiredTP.Name = "Minimum Required Time Period as %";
		MinimumRequiredTP.SetFloat(25.0f);
		MinimumRequiredTP.SetFloatLimits(1.0f, 100.0f);

		RoundToTickSize.Name = "Round to Tick Size";
		RoundToTickSize.SetYesNo(0);

		DisplayDebuggingOutput.Name = "Display Debugging Output (slows study calculations)";
		DisplayDebuggingOutput.SetYesNo(0);

		ForwardProjectLines.Name ="Forward Project Lines";
		ForwardProjectLines.SetYesNo(0);

		NumberOfDaysToCalculate.Name = "Number Of Days To Calculate";
		NumberOfDaysToCalculate.SetInt(1000);

		AutoSkipPeriodOfNoTrading.Name = "Auto Skip Period Of No Trading";
		AutoSkipPeriodOfNoTrading.SetYesNo(false);

		return;
	}


	int PeriodLength = TimePeriodLength.GetInt();

	int NumberOfForwardBars = 0;

	if (ForwardProjectLines.GetYesNo())
	{
		NumberOfForwardBars = 20;

		if(sc.UpdateStartIndex == 0)
		{
			Top.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			PPTop.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			PPBottom.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			Bottom.ExtendedArrayElementsToGraph = NumberOfForwardBars;

		}
	}

	if (NumberOfDaysToCalculate.GetInt() < 1)
		NumberOfDaysToCalculate.SetInt(1000);

	int LastDateInChart = sc.BaseDateTimeIn[sc.ArraySize - 1].GetDate();
	int FirstDateToCalculate = LastDateInChart - NumberOfDaysToCalculate.GetInt() + 1;

	float Open = 0, High = 0, Low = 0, Close = 0, NextOpen = 0;

	SCDateTime BeginOfRefDateTime,EndOfRefDateTime;
	SCDateTime CurrentPeriodBeginDateTime;
	SCDateTime CurrentPeriodEndDateTime;

	SCDateTime PriorCurrentPeriodStartDateTime;


	for (int index = sc.UpdateStartIndex; index < sc.ArraySize+NumberOfForwardBars; index++ )
	{

		SCDateTime CurrentBarDT = sc.BaseDateTimeIn[index];


		if (CurrentBarDT < FirstDateToCalculate)
			continue;

		bool GetReferenceData = true;

		CurrentPeriodBeginDateTime = sc.GetStartOfPeriodForDateTime(CurrentBarDT, TimePeriodType.GetTimePeriodType(),  PeriodLength, 0);

		if(	CurrentPeriodBeginDateTime == PriorCurrentPeriodStartDateTime)
			GetReferenceData  = false;


		PriorCurrentPeriodStartDateTime =	CurrentPeriodBeginDateTime;

		SCDateTime TimeIncrement = sc.TimePeriodSpan(TimePeriodType.GetTimePeriodType(),  PeriodLength);

		if(GetReferenceData)
		{

			BeginOfRefDateTime= sc.GetStartOfPeriodForDateTime( 	CurrentPeriodBeginDateTime, TimePeriodType.GetTimePeriodType(), PeriodLength, -1);
			EndOfRefDateTime=  	CurrentPeriodBeginDateTime - (1*SECONDS);

			CurrentPeriodEndDateTime=sc.GetStartOfPeriodForDateTime( 	CurrentPeriodBeginDateTime, TimePeriodType.GetTimePeriodType(),  PeriodLength, 1) - (1*SECONDS);

			if (DisplayDebuggingOutput.GetYesNo() != 0)
			{
				SCString Message;

				Message.Format("Current Bar: %s, BeginOfRefDateTime: %s, EndOfRefDateTime: %s, CPBeginDateTime: %s, CPEndDateTime: %s",
					sc.FormatDateTime(CurrentBarDT).GetChars(),sc.FormatDateTime(BeginOfRefDateTime).GetChars(),
					sc.FormatDateTime(EndOfRefDateTime).GetChars(),sc.FormatDateTime(CurrentPeriodBeginDateTime).GetChars(),
					sc.FormatDateTime(CurrentPeriodEndDateTime).GetChars());

				sc.AddMessageToLog(Message,0);

			}

			int MaxPeriodsToGoBack = 1;

			if(AutoSkipPeriodOfNoTrading.GetYesNo())
				MaxPeriodsToGoBack = 32;

			for (int WalkBack = 0; WalkBack<MaxPeriodsToGoBack;WalkBack++)
			{
				if (WalkBack >= 1) //Walk back 1 period.
				{
					SCDateTime  PriorBeginOfRefDateTime=BeginOfRefDateTime;

					BeginOfRefDateTime= sc.GetStartOfPeriodForDateTime(BeginOfRefDateTime, TimePeriodType.GetTimePeriodType(),  PeriodLength, -1);
					EndOfRefDateTime=  	 PriorBeginOfRefDateTime- (1*SECONDS);

					if (DisplayDebuggingOutput.GetYesNo() != 0)
					{
						SCString Message;

						Message.Format("Moving back 1 period. BeginOfRefDateTime: %s, EndOfRefDateTime: %s.",
							sc.FormatDateTime(BeginOfRefDateTime).GetChars(),sc.FormatDateTime(EndOfRefDateTime).GetChars());

						sc.AddMessageToLog(Message,0);
					}
				}

				int NumberOfBars = 0;
				SCDateTime TotalTimeSpan;
				int Result = sc.GetOHLCOfTimePeriod(BeginOfRefDateTime, EndOfRefDateTime, Open, High, Low, Close, NextOpen, NumberOfBars, TotalTimeSpan);
				if (!Result)
					continue;

				if (DisplayDebuggingOutput.GetYesNo() != 0)
				{
					SCString Message;
					Message.Format("Number of Bars: %d, Total Time Span In Minutes: %d",NumberOfBars,(int)(TotalTimeSpan/MINUTES+0.5));
					sc.AddMessageToLog(Message,0);

					Message.Format("RefOpen %f,RefHigh %f,RefLow %f,RefClose %f,RefNextOpen %f.",Open, High, Low, Close, NextOpen);
					sc.AddMessageToLog(Message,0);
				}

				SCDateTime MinimumTimeSpan = (TimeIncrement * MinimumRequiredTP.GetFloat()/100.0f);
				if (TotalTimeSpan >= MinimumTimeSpan)
					break;
			}
		}
		


		PPTop[index] = (High + Low + Close)/3.0f;
		PPBottom[index] =PPTop[index] ;

		float Range = PPTop[index] - ((High + Low)/2.0f );
		Top [index]=PPTop[index] + Range;
		Bottom [index]=PPTop[index] - Range;

		if (RoundToTickSize.GetYesNo() != 0)
		{
			PPTop[index] = sc.RoundToTickSize(PPTop[index], sc.TickSize);
			PPBottom[index] =sc.RoundToTickSize(PPBottom[index], sc.TickSize);

			Top [index]=sc.RoundToTickSize(Top[index], sc.TickSize);
			Bottom [index]=sc.RoundToTickSize(Bottom[index], sc.TickSize);


		}

	} //for
}
/*==========================================================================*/
// This function is used internally by scsf_SwingHighAndLowCustom
int IsSwingLowAllowEqualBackwardsOnly(SCStudyInterfaceRef sc, int AllowEqual, int Index, int Length)
{
	for(int i = 1; i <= Length; i++)
	{
		if (AllowEqual)
		{
			if (sc.FormattedEvaluate(sc.BaseData [SC_LOW][Index] , sc.BaseGraphValueFormat, GREATER_OPERATOR, sc.BaseData [SC_LOW][Index-i], sc.BaseGraphValueFormat))
				return 0;
		}
		else
		{
			if (sc.FormattedEvaluate(sc.BaseData [SC_LOW][Index] , sc.BaseGraphValueFormat, GREATER_EQUAL_OPERATOR, sc.BaseData [SC_LOW][Index-i], sc.BaseGraphValueFormat))
				return 0;
		}
	}

	return 1;
}

/*==========================================================================*/
SCSFExport scsf_SwingHighAndLowCustom(SCStudyInterfaceRef sc)
{
	SCSubgraphRef SwingHigh = sc.Subgraph[0];
	SCSubgraphRef SwingLow = sc.Subgraph[1];

	SCSubgraphRef Former = sc.Subgraph[2];
	SCSubgraphRef ColorForInsideBar = sc.Subgraph[3];
	SCSubgraphRef ColorForPotentialSwingHigh = sc.Subgraph[4];
	SCSubgraphRef ColorForPotentialSwingLow = sc.Subgraph[5];

	SCInputRef ArrowOffsetValue = sc.Input[0];
	SCInputRef Length = sc.Input[1];
	SCInputRef AllowEqualBars = sc.Input[2];

	if ( sc.SetDefaults )
	{
		sc.GraphName = "Swing High And Low Hollow Bar";
		sc.StudyDescription = "Swing High And Low Hollow Bar";

		

		SwingHigh.LineWidth = 3;
		SwingLow.LineWidth = 3;


		sc.FreeDLL = 0;
		sc.GraphRegion = 0;

		SwingHigh.Name = "Swing High";
		SwingHigh.DrawStyle = DRAWSTYLE_ARROWDOWN;
		SwingHigh.PrimaryColor = RGB(255,0,0);
		SwingHigh.DrawZeros = false;

		SwingLow.Name = "Swing Low";
		SwingLow.DrawStyle = DRAWSTYLE_ARROWUP;
		SwingLow.PrimaryColor = RGB(0,255,0);
		SwingLow.DrawZeros = false;

		Former.Name = "Potential Former";
		Former.DrawStyle = DRAWSTYLE_COLORBARHOLLOW;
		Former.PrimaryColor = RGB(255,255,0);
		Former.DrawZeros = false;

		ColorForInsideBar.Name = "Color for Potential Inside Bar";
		ColorForInsideBar.DrawStyle = DRAWSTYLE_LINE;
		ColorForInsideBar.PrimaryColor = RGB(255,128,0);
		ColorForInsideBar.DrawZeros = false;

		ColorForPotentialSwingHigh.Name = "Color for Potential Swing High";
		ColorForPotentialSwingHigh.DrawStyle = DRAWSTYLE_LINE;
		ColorForPotentialSwingHigh.PrimaryColor = RGB(0,0,255);
		ColorForPotentialSwingHigh.DrawZeros = false;

		ColorForPotentialSwingLow.Name = "Color for Potential Swing Low";
		ColorForPotentialSwingLow.DrawStyle = DRAWSTYLE_LINE;
		ColorForPotentialSwingLow.PrimaryColor = RGB(255,255,255);
		ColorForPotentialSwingLow.DrawZeros = false;

		ArrowOffsetValue.Name = "Arrow Offset as Value";
		ArrowOffsetValue.SetFloat(0);

		Length.Name = "Length";
		Length.SetInt(1);
		Length.SetIntLimits(1,MAX_STUDY_LENGTH);

		AllowEqualBars.Name = "Allow Equal High/Low Bars";
		AllowEqualBars.SetYesNo(false);

		sc.AutoLoop = true;
		return;
	}

	SCFloatArrayRef High = sc.High;
	SCFloatArrayRef Low = sc.Low;

	int MiddleIndex = sc.Index - Length.GetInt();

	Former[sc.Index] = High[sc.Index]; 
	if (IsSwingHighAllowEqual_S(sc, AllowEqualBars.GetYesNo(), sc.Index, Length.GetInt()))
	{
		Former.Arrays[0][sc.Index] = 1;
		Former.DataColor[sc.Index] = ColorForPotentialSwingHigh.PrimaryColor;

		// erase all previous potential swing highs
		for (int i = sc.Index - 1; i >= MiddleIndex && i >= 0; i--)
		{
			if (Former.Arrays[0][i] == 1)
			{
				Former[i] = 0;
				Former.Arrays[0][i] = 0;
			}
		}
	}
	else if (IsSwingLowAllowEqualBackwardsOnly(sc, AllowEqualBars.GetYesNo(), sc.Index, Length.GetInt()))
	{
		Former.Arrays[0][sc.Index] = -1;
		Former.DataColor[sc.Index] = ColorForPotentialSwingLow.PrimaryColor;

		// erase all previous potential swing lows
		for (int i = sc.Index - 1; i >= MiddleIndex && i >= 0; i--)
		{
			if (Former.Arrays[0][i] == -1)
			{
				Former[i] = 0;
				Former.Arrays[0][i] = 0;
			}
		}
	}
	else
	{
		Former.Arrays[0][sc.Index] = 0;
		Former.DataColor[sc.Index] = ColorForInsideBar.PrimaryColor;
	}

	// erase all previous inside-bars in any case
	for (int i = sc.Index - 1; i >= MiddleIndex && i >= 0; i--)
	{
		if (Former.Arrays[0][i] == 0)
		{
			Former[i] = 0;
		}
	}

	if (sc.GetBarHasClosedStatus(sc.Index)!=BHCS_BAR_HAS_CLOSED)
		return;

	if(sc.Index < 2 * Length.GetInt())
	{
		SwingHigh[sc.Index] = 0;
		SwingLow[sc.Index] = 0;

		return;
	}

	if ( IsSwingHighAllowEqual_S( sc, AllowEqualBars.GetYesNo(), MiddleIndex,  Length.GetInt()))
		SwingHigh[MiddleIndex] = sc.High[MiddleIndex] + ArrowOffsetValue.GetFloat();

	if ( IsSwingLowAllowEqual_S( sc, AllowEqualBars.GetYesNo(), MiddleIndex,  Length.GetInt()))
		SwingLow[MiddleIndex] = sc.Low[MiddleIndex] - ArrowOffsetValue.GetFloat();

	// Former needs to be set to 0, as we have calculated real Swing High/Low
	Former[MiddleIndex] = 0;
}

/*==========================================================================*/
SCSFExport scsf_StochasticMomentumIndicator(SCStudyInterfaceRef sc)
{
	SCSubgraphRef SMI = sc.Subgraph[0];
	SCSubgraphRef AvgSMI = sc.Subgraph[1];
	SCSubgraphRef Overbought = sc.Subgraph[2];
	SCSubgraphRef Oversold = sc.Subgraph[3];

	SCInputRef OverboughtInput = sc.Input[0];
	SCInputRef OversoldInput = sc.Input[1];
	SCInputRef PercentDLength = sc.Input[2];
	SCInputRef PercentKLength = sc.Input[3];
	SCInputRef EmaLength = sc.Input[4];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Stochastic Momentum Indicator";
		sc.StudyDescription = "Stochastic Momentum Indicator";

		sc.FreeDLL = 0;
		sc.GraphRegion = 1;
		sc.AutoLoop = 1;

		SMI.Name = "SMI";
		SMI.DrawStyle = DRAWSTYLE_LINE;
		SMI.PrimaryColor = RGB(0,255,0);
		SMI.DrawZeros = false;

		AvgSMI.Name = "SMI Average";
		AvgSMI.DrawStyle = DRAWSTYLE_LINE;
		AvgSMI.PrimaryColor = RGB(255,0,255);
		AvgSMI.DrawZeros = false;

		Overbought.Name = "Overbought";
		Overbought.DrawStyle = DRAWSTYLE_LINE;
		Overbought.PrimaryColor = RGB(255,255,0);
		Overbought.DrawZeros = false;

		Oversold.Name = "Oversold";
		Oversold.DrawStyle = DRAWSTYLE_LINE;
		Oversold.PrimaryColor = RGB(255,127,0);
		Oversold.DrawZeros = false;

		OverboughtInput.Name = "Overbought";
		OverboughtInput.SetFloat(40.0f);

		OversoldInput.Name = "Oversold";
		OversoldInput.SetFloat(-40.0f);

		PercentDLength.Name = "%D Length";
		PercentDLength.SetInt(3);

		PercentKLength.Name = "%K Length";
		PercentKLength.SetInt(5);

		EmaLength.Name = "EMA Length";
		EmaLength.SetInt(3);

		return;
	}


	sc.DataStartIndex = max(PercentKLength.GetInt(), PercentDLength.GetInt());

	if (sc.Index < sc.DataStartIndex)
		return;

	SCFloatArrayRef TempLowest = SMI.Arrays[0];
	SCFloatArrayRef TempHighest = SMI.Arrays[1];
	SCFloatArrayRef TempRelDiff = SMI.Arrays[2];
	SCFloatArrayRef TempDiff = SMI.Arrays[3];
	SCFloatArrayRef TempAvgRelMA = SMI.Arrays[4];
	SCFloatArrayRef TempAvgRel = SMI.Arrays[5];
	SCFloatArrayRef TempAvgDiffMA = SMI.Arrays[6];
	SCFloatArrayRef TempAvgDiff = SMI.Arrays[7];

	sc.Lowest(sc.Low, TempLowest, PercentKLength.GetInt());
	sc.Highest(sc.High, TempHighest, PercentKLength.GetInt());

	TempRelDiff[sc.Index] = sc.Close[sc.Index] - (TempHighest[sc.Index] + TempLowest[sc.Index]) / 2.0f;
	TempDiff[sc.Index] = TempHighest[sc.Index] - TempLowest[sc.Index];

	sc.ExponentialMovAvg(TempRelDiff, TempAvgRelMA, PercentDLength.GetInt());
	sc.ExponentialMovAvg(TempAvgRelMA, TempAvgRel, PercentDLength.GetInt());
	sc.ExponentialMovAvg(TempDiff, TempAvgDiffMA, PercentDLength.GetInt());
	sc.ExponentialMovAvg(TempAvgDiffMA, TempAvgDiff, PercentDLength.GetInt());

	if (TempDiff[sc.Index] != 0.0f)
		SMI[sc.Index] = TempAvgRel[sc.Index] / (TempAvgDiff[sc.Index] / 2.0f) * 100.0f;
	else
		SMI[sc.Index] = 0.0f;

	sc.ExponentialMovAvg(SMI, AvgSMI, EmaLength.GetInt());

	Overbought[sc.Index] = OverboughtInput.GetFloat();
	Oversold[sc.Index] = OversoldInput.GetFloat();
}

/*==========================================================================*/
SCSFExport scsf_DEMA(SCStudyInterfaceRef sc)
{
	SCSubgraphRef DEMA = sc.Subgraph[0];

	SCInputRef InputData = sc.Input[0];
	SCInputRef Length = sc.Input[1];

	if (sc.SetDefaults)
	{
		sc.GraphName = "DEMA";
		sc.StudyDescription = "Double Exponential Moving Average";

		sc.FreeDLL = 0;
		sc.GraphRegion = 0;
		sc.AutoLoop = 1;

		DEMA.Name = "DEMA";
		DEMA.DrawStyle = DRAWSTYLE_LINE;
		DEMA.PrimaryColor = RGB(255, 0, 0);
		DEMA.DrawZeros = false;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		Length.Name = "Length";
		Length.SetIntLimits(1,MAX_STUDY_LENGTH);
		Length.SetInt(10);

		return;
	}

	sc.DataStartIndex = Length.GetInt();
	if (sc.Index < sc.DataStartIndex)
		return;

	SCFloatArrayRef EmaPrice = DEMA.Arrays[0];
	SCFloatArrayRef EmaEmaPrice = DEMA.Arrays[1];

	sc.ExponentialMovAvg(sc.BaseDataIn[InputData.GetInputDataIndex()], EmaPrice, Length.GetInt());
	sc.ExponentialMovAvg(EmaPrice, EmaEmaPrice, Length.GetInt());

	DEMA[sc.Index] = 2 * EmaPrice[sc.Index] - EmaEmaPrice[sc.Index];
}

/*==========================================================================*/
SCSFExport scsf_BidVolumeVersusAskVolume(SCStudyInterfaceRef sc)
{
	SCSubgraphRef AskVolume = sc.Subgraph[0];
	SCSubgraphRef BidVolume = sc.Subgraph[1];
	SCSubgraphRef ZeroLine = sc.Subgraph[3];
	SCSubgraphRef Difference = sc.Subgraph[4];

	SCInputRef InvertStudy = sc.Input[3];
	SCInputRef ShowBidVolumePositive = sc.Input[4];
	SCInputRef TryUseNonzeroVolumes = sc.Input[5];

	if (sc.SetDefaults)
	{
		sc.GraphName = "BidVolume vs. AskVolume";
		sc.ValueFormat= 0;

		AskVolume.Name = "Ask Volume";
		AskVolume.PrimaryColor = RGB(0, 255, 0);
		AskVolume.DrawStyle = DRAWSTYLE_BAR;
		AskVolume.DrawZeros = true;

		BidVolume.Name = "Bid Volume";
		BidVolume.PrimaryColor = RGB(255, 0, 0);
		BidVolume.DrawStyle = DRAWSTYLE_BAR;
		BidVolume.DrawZeros = true;

		ZeroLine.Name = "Zero Line";
		ZeroLine.PrimaryColor = RGB(255,127,0);
		ZeroLine.DrawStyle = DRAWSTYLE_LINE;
		ZeroLine.DrawZeros = true;

		Difference.Name = "Difference";
		Difference.PrimaryColor = RGB(255, 255, 0);
		Difference.DrawStyle = DRAWSTYLE_LINE;
		Difference.DrawZeros = true;

		InvertStudy.Name = "Invert Study";
		InvertStudy.SetYesNo(0);

		ShowBidVolumePositive.Name = "Show Bid Volume As Positive";
		ShowBidVolumePositive.SetYesNo(0);

		TryUseNonzeroVolumes.Name = "Try Use Non-Zero Bid/Ask Volumes for Difference";
		TryUseNonzeroVolumes.SetYesNo(0);

		return;
	}

	int Invert = 1;
	if(InvertStudy.GetYesNo() == 1)
		Invert = -1;


	// Check for the appropriate chart type
	if (sc.ChartDataType != INTRADAY_DATA)
	{
		if (sc.Index == 0)
		{
			sc.AddMessageToLog("This study can only be applied to an Intraday Chart", 1);
		}
		return;
	}

	int StartIndex = sc.UpdateStartIndex;


	
		// Loop through the new indexes to fill in
		for (int DestIndex = StartIndex; DestIndex < sc.ArraySize; ++DestIndex)
		{
			float BidVolumeVal = sc.BidVolume[DestIndex];
			float AskVolumeVal = sc.AskVolume[DestIndex];

			AskVolume[DestIndex] = AskVolumeVal * Invert;

			if (ShowBidVolumePositive.GetYesNo() == 1)
				BidVolume[DestIndex] = BidVolumeVal * Invert;
			else
				BidVolume[DestIndex] = - BidVolumeVal * Invert;

			ZeroLine[DestIndex] = 0;

			if(TryUseNonzeroVolumes.GetYesNo() == 1)
			{
				// if both volume are zero skip this iteration
				if(BidVolumeVal == 0 && AskVolumeVal == 0)
					continue;

				// if Bid Volume is zero try to find nonzero volume in previous elements
				if(BidVolumeVal == 0)
				{
					int Index = DestIndex - 1;
					while(Index >= 0 && BidVolumeVal == 0)
					{
						BidVolumeVal = sc.BidVolume[Index];
						Index--;
					}
				}

				// if Ask Volume is zero try to find nonzero volume in previous elements
				if(AskVolumeVal == 0)
				{
					int Index = DestIndex - 1;
					while(Index >= 0 && AskVolumeVal == 0)
					{
						AskVolumeVal = sc.AskVolume[Index];
						Index--;
					}
				}
			}
			Difference[DestIndex] = AskVolumeVal - BidVolumeVal;
		}
	
}

/*============================================================================
	Test function for HTTP request.
----------------------------------------------------------------------------*/
SCSFExport scsf_HTTPTest(SCStudyInterfaceRef sc)
{
	if (sc.SetDefaults)
	{
		// Set the configuration and defaults
		
		sc.GraphName = "HTTP Request Test";
		
		sc.StudyDescription = "This is a study function that demonstrates how to request data from a web server.";
		
		sc.AutoLoop = 1;
		
		// During development set this flag to 1, so the DLL can be modified. When development is done, set it to 0 to improve performance.
		sc.FreeDLL = 0;
		
		return;
	}
	
	
	enum {REQUEST_NOT_SENT = 0, REQUEST_MADE, REQUEST_RECEIVED};
	int& RequestState = sc.PersistVars->i1;
	
	
	// Do data processing
	
	if (sc.Index == 0)
	{
		if (RequestState == REQUEST_NOT_SENT)
		{
			// Make a request for a text file on the server. When the request is complete and all of the data has been downloaded, this study function will be called with the file placed into the sc.HTTPResponse character string array.
			if (!sc.MakeHTTPRequest("http://www.sierrachart.com/ACSILHTTPTest.txt"))
				sc.AddMessageToLog("Error making HTTP request.", 1);
			
			RequestState = REQUEST_MADE;
		}
	}
	
	if (RequestState == REQUEST_MADE && sc.HTTPResponse != "")
	{
		RequestState = REQUEST_RECEIVED;
		
		// Display the response from the Web server in the Message Log
		sc.AddMessageToLog(sc.HTTPResponse, 1); 
	}
	else if (RequestState == REQUEST_MADE && sc.HTTPResponse == "")
	{

		//The request has not completed, therefore there is nothing to do so we will return
		return;
	}
	
}

/*==========================================================================*/
SCSFExport scsf_TimeRangeHighlight(SCStudyInterfaceRef sc)
{
	SCSubgraphRef ColorBackground = sc.Subgraph[0];

	SCInputRef StartTime = sc.Input[0];
	SCInputRef EndTime = sc.Input[1];
	SCInputRef Version = sc.Input[2];

	SCInputRef ForwardProject = sc.Input[4];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Time Range Highlight";
		sc.GraphRegion = 0;
		sc.AutoLoop = 0;
		sc.ValueFormat = 0;
		sc.ScaleRangeType = SCALE_INDEPENDENT;


		ColorBackground.Name = "TR";
		ColorBackground.DrawStyle = DRAWSTYLE_BACKGROUND;
		ColorBackground.PrimaryColor = RGB(64, 64, 64);
		ColorBackground.DrawZeros = false;



		StartTime.Name = "Start Time";
		StartTime.SetTime(0);

		EndTime.Name = "End Time";
		EndTime.SetTime(SECONDS_PER_DAY - 1);

		Version.SetInt(1);

		ForwardProject.Name = "Display in Forward Projection Area";
		ForwardProject.SetYesNo(false);

		return;
	}


	if(Version.GetInt() < 1)
	{
		Version.SetInt(1);
		ColorBackground.DrawStyle = DRAWSTYLE_BACKGROUND;
	}

	int NumberForwardBars = 0;
	if(ForwardProject.GetYesNo())
	{
		NumberForwardBars = 200;
		ColorBackground.ExtendedArrayElementsToGraph = NumberForwardBars;
	}

	for (int Index = sc.UpdateStartIndex; Index < (sc.ArraySize + NumberForwardBars); Index++)
	{
		bool InputsReversed = (StartTime.GetTime() > EndTime.GetTime());

		SCDateTime& IndexDateTime =  sc.BaseDateTimeIn[Index];
		bool ShouldHighLight = false;

		if (InputsReversed)
			ShouldHighLight = (IndexDateTime.GetTime() >= StartTime.GetTime() || IndexDateTime.GetTime() <= EndTime.GetTime());
		else
			ShouldHighLight = (IndexDateTime.GetTime() >= StartTime.GetTime() && IndexDateTime.GetTime() <= EndTime.GetTime());

		int CloseIndex = Index;
		if(CloseIndex >= sc.ArraySize)
			CloseIndex = sc.ArraySize - 1;

		if (ShouldHighLight)
			ColorBackground[Index] =  sc.Close[CloseIndex];
		else
			ColorBackground[Index] = 0;
	}


}
/*==========================================================================*/
SCSFExport scsf_TimeRangeHighlightTransparent(SCStudyInterfaceRef sc)
{
	SCSubgraphRef TRTop = sc.Subgraph[0];
	SCSubgraphRef TRBottom = sc.Subgraph[1];

	SCInputRef StartTime = sc.Input[0];
	SCInputRef EndTime = sc.Input[1];

	SCInputRef ForwardProject = sc.Input[4];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Time Range Highlight - Transparent";
		sc.GraphRegion = 0;
		sc.AutoLoop = 0;//Manual looping
		sc.ValueFormat =0;
		sc.ScaleRangeType = SCALE_SAMEASREGION;
		sc.DrawStudyUnderneathMainPriceGraph = true;
		

		TRTop.Name = "Top";
		TRTop.DrawStyle = DRAWSTYLE_FILLRECTTOP_TRANSPARENT;
		TRTop.PrimaryColor = RGB(64, 64, 64);
		TRTop.DrawZeros = false;


		TRBottom.Name = "Bottom";
		TRBottom.DrawStyle = DRAWSTYLE_FILLRECTBOTTOM_TRANSPARENT;
		TRBottom.PrimaryColor = RGB(64, 64, 64);
		TRBottom.DrawZeros = false;

		StartTime.Name = "Start Time";
		StartTime.SetTime(0);

		EndTime.Name = "End Time";
		EndTime.SetTime(SECONDS_PER_DAY - 1);

		ForwardProject.Name = "Display in Forward Projection Area";
		ForwardProject.SetYesNo(false);

		return;
	}



	bool InputsReversed = (StartTime.GetTime() > EndTime.GetTime());


	int NumberForwardBars = 0;
	if(ForwardProject.GetYesNo())
	{
		NumberForwardBars = 200;
		TRTop.ExtendedArrayElementsToGraph = NumberForwardBars;
		TRBottom.ExtendedArrayElementsToGraph = NumberForwardBars;
	}

	for (int Index = sc.UpdateStartIndex; Index < (sc.ArraySize + NumberForwardBars); Index++)
	{
		SCDateTime& IndexDateTime =  sc.BaseDateTimeIn[Index];
		bool ShouldHighLight = false;

		if (InputsReversed)
			ShouldHighLight = (IndexDateTime.GetTime() >= StartTime.GetTime() || IndexDateTime.GetTime() <= EndTime.GetTime());
		else
			ShouldHighLight = (IndexDateTime.GetTime() >= StartTime.GetTime() && IndexDateTime.GetTime() <= EndTime.GetTime());

		if (ShouldHighLight)
		{
			TRTop[Index] = 1000000;
			TRBottom[Index] = -1000000;
		}
		else
		{
			TRTop[Index] = 0;
			TRBottom[Index] = 0;

		}
	}
}


/***************************************************************/
SCSFExport scsf_ColoredCCI(SCStudyInterfaceRef sc)
{
	SCSubgraphRef CCI = sc.Subgraph[0];
	SCSubgraphRef Line1 = sc.Subgraph[1];
	SCSubgraphRef Line2 = sc.Subgraph[2];
	SCSubgraphRef Line3 = sc.Subgraph[3];


	SCInputRef I_InputData = sc.Input[0];
	SCInputRef I_Length = sc.Input[3];
	SCInputRef I_Multiplier = sc.Input[4];
	SCInputRef I_Line2Value = sc.Input[5];
	SCInputRef I_Line3Value = sc.Input[6];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults
		sc.GraphName = "Colored CCI";


		sc.FreeDLL = 0;
		sc.AutoLoop = 1;  // true
		sc.GraphRegion = 1;
		sc.ValueFormat = 2;

		CCI.Name = "CCI";
		CCI.DrawStyle = DRAWSTYLE_BAR;
		CCI.DrawZeros= true;
		CCI.PrimaryColor = RGB(0, 255, 255);
		CCI.SecondaryColorUsed = 1;
		CCI.SecondaryColor= RGB(0, 255, 0);

		Line1.Name = "Line 1";
		Line1.DrawStyle = DRAWSTYLE_HIDDEN;
		Line1.PrimaryColor = RGB(255,0,255);
		Line1.DrawZeros= true;

		Line2.Name = "Line 2";
		Line2.DrawStyle = DRAWSTYLE_LINE;
		Line2.PrimaryColor = RGB(255,255,0);
		Line2.DrawZeros = true;

		Line3.Name = "Line 3";
		Line3.DrawStyle = DRAWSTYLE_LINE;
		Line3.PrimaryColor = RGB(255,127,0);
		Line3.DrawZeros = true;



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

		return;
	}

	float CCILow = -50.0f, CCHigh = 50.0f;

	sc.DataStartIndex = I_Length.GetInt();

	float Multiplier = I_Multiplier.GetFloat();

	if(Multiplier == 0.0f)
		Multiplier = 0.015f;

	sc.CCI(sc.BaseDataIn[I_InputData.GetInputDataIndex()], CCI, sc.Index, I_Length.GetInt(), Multiplier);

	Line2[sc.Index] = I_Line2Value.GetFloat();
	Line3[sc.Index] = I_Line3Value.GetFloat();

	if (CCI[sc.Index - 1] < CCILow && CCI[sc.Index] > CCHigh)
		CCI.DataColor[sc.Index] = CCI.SecondaryColor;
	else
		CCI.DataColor[sc.Index] = CCI.PrimaryColor;
}

/***************************************************************/
SCSFExport scsf_ColorBarsBasedOnMA(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Avg = sc.Subgraph[0];
	SCSubgraphRef ColorSMA = sc.Subgraph[1];
	SCSubgraphRef ArrowSMA = sc.Subgraph[2];

	SCFloatArrayRef Slope = Avg.Arrays[0];

	SCInputRef InputData = sc.Input[0];
	SCInputRef Length = sc.Input[3];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Color Bars Based on Moving Average";

		sc.GraphRegion = 0;
		sc.ValueFormat = 2;

		sc.AutoLoop = 1;

		Avg.Name = "Avg";

		ColorSMA.Name = "Color SMA";
		ColorSMA.DrawStyle = DRAWSTYLE_COLORBAR;
		ColorSMA.PrimaryColor = RGB(0, 255, 0);
		ColorSMA.DrawZeros = false;

		ArrowSMA.Name = "Arrow SMA";
		ArrowSMA.DrawStyle = DRAWSTYLE_ARROWUP;
		ArrowSMA.PrimaryColor = RGB(0, 255, 0);
		ArrowSMA.LineWidth = 3;
		ArrowSMA.DrawZeros = false;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		Length.Name = "Length";
		Length.SetInt(10);
		Length.SetIntLimits(1, MAX_STUDY_LENGTH);

		return;
	}

	sc.DataStartIndex = Length.GetInt() - 1;

	sc.SimpleMovAvg(sc.BaseDataIn[InputData.GetInputDataIndex()], Avg,  Length.GetInt());
	sc.Slope(Avg, Slope);

	if (sc.BaseDataIn[InputData.GetInputDataIndex()][sc.Index] > Avg[sc.Index] && Slope[sc.Index] > 0)
	{
		ColorSMA[sc.Index] = 1;
		ArrowSMA[sc.Index] = sc.Low[sc.Index];
	}
	else
	{
		ColorSMA[sc.Index] = 0;
		ArrowSMA[sc.Index] = 0;
	}
}

/*==========================================================================*/
SCSFExport scsf_SetAlertExample(SCStudyInterfaceRef sc)
{
	if (sc.SetDefaults)
	{
		// Set the configuration and defaults
		
		sc.GraphName = "Set Alert Example";
		
		sc.StudyDescription = "";
		
		sc.AutoLoop = true;
		
		// During development set this flag to 1, so the DLL can be modified. When development is done, set it to 0 to improve performance.
		sc.FreeDLL = 0;
		
		return;
	}
	
	
	// Do data processing
	
		int Index = sc.Index; 
	
		SCString AlertMessageText;
		AlertMessageText.Format("Alert at index %d", Index);
		sc.SetAlert(1, AlertMessageText);
	
}

/*==========================================================================*/
SCSFExport scsf_ClearMethodSwingLine(SCStudyInterfaceRef sc)
{
	SCSubgraphRef SwingLine = sc.Subgraph[0];

	SCFloatArrayRef HH = sc.Subgraph[0].Arrays[0];
	SCFloatArrayRef LL = sc.Subgraph[0].Arrays[1];
	SCFloatArrayRef LH = sc.Subgraph[0].Arrays[2];
	SCFloatArrayRef HL = sc.Subgraph[0].Arrays[3];
	SCFloatArrayRef UpSwing = sc.Subgraph[0].Arrays[4];
	
	if (sc.SetDefaults)
	{
		sc.GraphName = "Clear Method Swing Line";

		sc.AutoLoop = true;

		sc.GraphRegion = 0;

		SwingLine.Name = "SwingLine";
		SwingLine.PrimaryColor = COLOR_CYAN;
		SwingLine.SecondaryColor = COLOR_MAGENTA;
		SwingLine.SecondaryColorUsed = 1;

		// During development set this flag to 1, so the DLL can be modified. When development is done, set it to 0 to improve performance.
		sc.FreeDLL = 0;

		return;
	}

	// Do data processing


	//Carry forward prior values in the background data arrays
	HH[sc.Index] = HH[sc.Index-1];
	HL[sc.Index] = HL[sc.Index-1];
	LL[sc.Index] = LL[sc.Index-1];
	LH[sc.Index] = LH[sc.Index-1];
	UpSwing[sc.Index] = UpSwing[sc.Index-1];



	if (UpSwing[sc.Index] == 1)
	{
		if (sc.High[sc.Index] > HH[sc.Index])
			HH[sc.Index] = sc.High[sc.Index];

		if (sc.Low[sc.Index] > HL[sc.Index])
			HL[sc.Index] = sc.Low[sc.Index];

		if (sc.High[sc.Index] < HL[sc.Index])
		{
			UpSwing[sc.Index] = 0;
			LL[sc.Index] = sc.Low[sc.Index];
			LH[sc.Index] = sc.High[sc.Index];
		}
	}

	if (UpSwing[sc.Index] == 0)
	{
		if (sc.Low[sc.Index] < LL[sc.Index])
			LL[sc.Index] = sc.Low[sc.Index];

		if (sc.High[sc.Index] < LH[sc.Index])
			LH[sc.Index] = sc.High[sc.Index];

		if (sc.Low[sc.Index] > LH[sc.Index])
		{
			UpSwing[sc.Index] = 1;
			HH[sc.Index] = sc.High[sc.Index];
			HL[sc.Index] = sc.Low[sc.Index];
		}
	}

	if (UpSwing[sc.Index] == 1)
	{
		SwingLine[sc.Index] = HL[sc.Index];
		SwingLine.DataColor[sc.Index] = SwingLine.PrimaryColor;
	}
	else
	{
		SwingLine[sc.Index] = LH[sc.Index];
		SwingLine.DataColor[sc.Index] = SwingLine.SecondaryColor;
	}
}

/*==========================================================================*/
SCSFExport scsf_StudySubgraphsAdd(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Add = sc.Subgraph[0];

	SCInputRef StudySugbraph1 = sc.Input[0];
	SCInputRef StudySugbraph2 = sc.Input[1];
	SCInputRef Study2SubgraphOffset = sc.Input[2];

	SCInputRef DrawZeros = sc.Input[3];



	if (sc.SetDefaults)
	{
		sc.GraphName = "Study Subgraphs Add";

		sc.AutoLoop = 1;
		sc.CalculationPrecedence = LOW_PREC_LEVEL;

		sc.FreeDLL=0; 

		Add.Name = "Add";
		Add.DrawStyle = DRAWSTYLE_LINE;
		Add.LineWidth = 1;
		Add.PrimaryColor = RGB(0,255,0);

		StudySugbraph1.Name = "Input Study 1";
		StudySugbraph1.SetStudySubgraphValues(0, 0);

		StudySugbraph2.Name = "Input Study 2";
		StudySugbraph2.SetStudySubgraphValues(0, 0);

		Study2SubgraphOffset.Name = "Study 2 Subgraph Offset";
		Study2SubgraphOffset.SetInt(0);

		DrawZeros.Name = "Draw Zeros";
		DrawZeros.SetYesNo(false);

		return;
	}

	Add.DrawZeros= DrawZeros. GetYesNo();

	SCFloatArray Study1Array;
	sc.GetStudyArrayUsingID(StudySugbraph1.GetStudyID(),StudySugbraph1.GetSubgraphIndex(),Study1Array);

	SCFloatArray Study2Array;
	sc.GetStudyArrayUsingID(StudySugbraph2.GetStudyID(),StudySugbraph2.GetSubgraphIndex(),Study2Array);

	int i = sc.Index;
	Add[i] = Study1Array[i] + Study2Array[i-Study2SubgraphOffset.GetInt()];
}

/*==========================================================================*/
SCSFExport scsf_StudySubgraphsMultiply(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Multiply = sc.Subgraph[0];

	SCInputRef StudySugbraph1 = sc.Input[0];
	SCInputRef StudySugbraph2 = sc.Input[1];
	SCInputRef Study2SubgraphOffset = sc.Input[2];

	SCInputRef DrawZeros = sc.Input[3];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Study Subgraphs Multiply";


		sc.AutoLoop = 1;
		sc.CalculationPrecedence = LOW_PREC_LEVEL;

		sc.FreeDLL=0; 

		Multiply.Name = "Multiply";
		Multiply.DrawStyle = DRAWSTYLE_LINE;
		Multiply.PrimaryColor = RGB(0,255,0);
		Multiply.LineWidth = 1;

		StudySugbraph1.Name = "Input Study 1";
		StudySugbraph1.SetStudySubgraphValues(0, 0);

		StudySugbraph2.Name = "Input Study 2";
		StudySugbraph2.SetStudySubgraphValues(0, 0);

		Study2SubgraphOffset.Name = "Study 2 Subgraph Offset";
		Study2SubgraphOffset.SetInt(0);
		
		DrawZeros.Name = "Draw Zeros";
		DrawZeros.SetYesNo(false);

		return;
	}


	Multiply.DrawZeros= DrawZeros.GetYesNo();
	
	SCFloatArray Study1Array;
	sc.GetStudyArrayUsingID(StudySugbraph1.GetStudyID(),StudySugbraph1.GetSubgraphIndex(),Study1Array);

	SCFloatArray Study2Array;
	sc.GetStudyArrayUsingID(StudySugbraph2.GetStudyID(),StudySugbraph2.GetSubgraphIndex(),Study2Array);

	int i = sc.Index;
	Multiply[i] = Study1Array[i] * Study2Array[i-Study2SubgraphOffset.GetInt()];
}

/*==========================================================================*/
SCSFExport scsf_StudySubgraphsDivide(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Ratio = sc.Subgraph[0];

	SCInputRef StudySugbraph1 = sc.Input[0];
	SCInputRef StudySugbraph2 = sc.Input[1];
	SCInputRef Study2SubgraphOffset = sc.Input[2];
	SCInputRef DrawZeros = sc.Input[3];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Study Subgraphs Ratio";

		sc.AutoLoop = 1;
		sc.CalculationPrecedence = LOW_PREC_LEVEL;

		sc.FreeDLL=0; 

		Ratio.Name = "Ratio";
		Ratio.DrawStyle = DRAWSTYLE_LINE;
		Ratio.PrimaryColor = RGB(0,255,0);
		Ratio.LineWidth = 1;

		StudySugbraph1.Name = "Input Study 1";
		StudySugbraph1.SetStudySubgraphValues(0, 0);

		StudySugbraph2.Name = "Input Study 2";
		StudySugbraph2.SetStudySubgraphValues(0, 0);

		Study2SubgraphOffset.Name = "Study 2 Subgraph Offset";
		Study2SubgraphOffset.SetInt(0);


		DrawZeros.Name = "Draw Zeros";
		DrawZeros.SetYesNo(false);

		return;
	}


	Ratio.DrawZeros= DrawZeros. GetYesNo();

	SCFloatArray Study1Array;
	sc.GetStudyArrayUsingID(StudySugbraph1.GetStudyID(),StudySugbraph1.GetSubgraphIndex(),Study1Array);

	SCFloatArray Study2Array;
	sc.GetStudyArrayUsingID(StudySugbraph2.GetStudyID(),StudySugbraph2.GetSubgraphIndex(),Study2Array);

	int i = sc.Index;
	float Denominator = Study2Array[i-Study2SubgraphOffset.GetInt()];
	if (Denominator != 0.0f)
		Ratio[i] = Study1Array[i] / Denominator;
	else
		Ratio[i] = 0.0f;
}

/*==========================================================================*/
SCSFExport scsf_Inertia(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Inertia = sc.Subgraph[0];
	SCSubgraphRef RVI = sc.Subgraph[1];

	SCInputRef RVILength = sc.Input[0];
	SCInputRef LinearRegressionLength = sc.Input[1];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Inertia";

		sc.AutoLoop = 1;
		sc.GraphRegion = 1;

		sc.FreeDLL = 0; 

		Inertia.Name = "Inertia";
		Inertia.PrimaryColor = COLOR_GREEN;
		Inertia.DrawStyle = DRAWSTYLE_LINE;
		Inertia.PrimaryColor = RGB(0,255,0);
		Inertia.DrawZeros = true;
		Inertia.LineWidth = 1;

		RVILength.Name = "Relative Vigor Index Length";
		RVILength.SetInt(10);

		LinearRegressionLength.Name = "Linear Regression Length";
		LinearRegressionLength.SetInt(10);

		return;
	}

	sc.DataStartIndex = max(RVILength.GetInt(), LinearRegressionLength.GetInt());

	const int RviMinIndex = 3;
	if (sc.Index < RviMinIndex)
		return;

	// Calculate RVI
	SCFloatArrayRef Value1 = RVI.Arrays[0];
	SCFloatArrayRef Value2 = RVI.Arrays[1];
	SCFloatArrayRef RviNum = RVI.Arrays[2];
	SCFloatArrayRef RviDenom = RVI.Arrays[3];

	Value1[sc.Index] = ((sc.Close[sc.Index] - sc.Open[sc.Index])
		+ 2 * (sc.Close[sc.Index - 1] - sc.Open[sc.Index - 1])
		+ 2 * (sc.Close[sc.Index - 2] - sc.Open[sc.Index - 2])
		+ (sc.Close[sc.Index - 3] - sc.Open[sc.Index - 3])) / 6;

	Value2[sc.Index] = ((sc.High[sc.Index] - sc.Low[sc.Index])
		+ 2 * (sc.High[sc.Index - 1] - sc.Low[sc.Index - 1])
		+ 2 * (sc.High[sc.Index - 2] - sc.Low[sc.Index - 2])
		+ (sc.High[sc.Index - 3] - sc.Low[sc.Index - 3])) / 6;

	if (sc.Index < sc.DataStartIndex)
		return;

	sc.Summation(Value1, RviNum, RVILength.GetInt());
	sc.Summation(Value2, RviDenom, RVILength.GetInt());

	if (RviDenom[sc.Index] != 0.0f)
		RVI[sc.Index] = RviNum[sc.Index] / RviDenom[sc.Index];
	else
		RVI[sc.Index] = 0.0f;

	// Calculate Inertia
	sc.LinearRegressionIndicator(RVI, Inertia, LinearRegressionLength.GetInt());
}

/*==========================================================================*/
SCSFExport scsf_Inertia2(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Inertia = sc.Subgraph[0];
	SCSubgraphRef RVI = sc.Subgraph[1];

	SCInputRef StdDeviationLength = sc.Input[0];
	SCInputRef RviLength = sc.Input[1];
	SCInputRef LinearRegressionLength = sc.Input[2];
	
	if (sc.SetDefaults)
	{
		sc.GraphName = "Inertia 2";

		sc.AutoLoop = 1;
		sc.GraphRegion = 1;

		sc.FreeDLL = 0; 

		Inertia.Name = "Inertia";
		Inertia.PrimaryColor = COLOR_GREEN;
		Inertia.DrawStyle = DRAWSTYLE_LINE;
		Inertia.DrawZeros = true;
		Inertia.LineWidth = 1;

		StdDeviationLength.Name = "Standard Deviation Length";
		StdDeviationLength.SetInt(9);

		RviLength.Name = "Relative Volatility Index Length";
		RviLength.SetInt(14);

		LinearRegressionLength.Name = "Linear Regression Length";
		LinearRegressionLength.SetInt(10);

		return;
	}

	sc.DataStartIndex = max(StdDeviationLength.GetInt(), max(RviLength.GetInt(), LinearRegressionLength.GetInt()));
	if (sc.Index < sc.DataStartIndex)
		return;

	// Calculate RVI
	SCFloatArrayRef RviUp = RVI.Arrays[0];
	SCFloatArrayRef RviDown = RVI.Arrays[1];
	SCFloatArrayRef RviUpAvg = RVI.Arrays[2];
	SCFloatArrayRef RviDownAvg = RVI.Arrays[3];
	SCFloatArrayRef StdDeviation = RVI.Arrays[4];

	sc.StdDeviation(sc.Close, StdDeviation, StdDeviationLength.GetInt());

	if (sc.Close[sc.Index] > sc.Close[sc.Index - 1])
	{
		RviUp[sc.Index] = StdDeviation[sc.Index];
		RviDown[sc.Index] = 0.0f;
	}
	else
	{
		RviUp[sc.Index] = 0.0f;
		RviDown[sc.Index] = StdDeviation[sc.Index];
	}

	if (RviLength.GetInt() !=0)
	{
		RviUpAvg[sc.Index] = (RviUpAvg[sc.Index - 1] * (RviLength.GetInt() - 1) +  RviUp[sc.Index]) / RviLength.GetInt();
		RviDownAvg[sc.Index] = (RviDownAvg[sc.Index - 1] * (RviLength.GetInt() - 1) +  RviDown[sc.Index]) / RviLength.GetInt();
	}
	else
	{
		RviUpAvg[sc.Index] = 0.0f;
		RviDownAvg[sc.Index] = 0.0f;
	}

	if (RviUpAvg[sc.Index] + RviDownAvg[sc.Index] != 0.0f)
		RVI[sc.Index] = (RviUpAvg[sc.Index] / (RviUpAvg[sc.Index] + RviDownAvg[sc.Index])) * 100;
	else
		RVI[sc.Index] = 0.0f;

	// Calculate Inertia
	sc.LinearRegressionIndicator(RVI, Inertia, LinearRegressionLength.GetInt());
}

/*==========================================================================*/
SCSFExport scsf_AskBidVolumeDifferenceBars(SCStudyInterfaceRef sc)
{
	// reference the base data
	SCFloatArrayRef iNumTrades = sc.BaseDataIn[SC_NT];
	SCFloatArrayRef iBidVol    = sc.BaseDataIn[SC_BIDVOL];
	SCFloatArrayRef iAskVol    = sc.BaseDataIn[SC_ASKVOL];
	SCFloatArrayRef iDiffHigh  = sc.BaseDataIn[SC_ASKBID_DIFF_HIGH];
	SCFloatArrayRef iDiffLow   = sc.BaseDataIn[SC_ASKBID_DIFF_LOW];

	SCSubgraphRef Open = sc.Subgraph[0];
	SCSubgraphRef High = sc.Subgraph[1];
	SCSubgraphRef Low = sc.Subgraph[2];
	SCSubgraphRef Close = sc.Subgraph[3];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Ask/Bid Volume Difference Bars";

		sc.FreeDLL = 0;
		sc.MaintainAdditionalChartDataArrays = 1;

		sc.ValueFormat = 0;
		sc.GraphDrawType = GDT_CANDLESTICK;
		
		Open.Name = "Open";
		Open.PrimaryColor = RGB(0,255,0);
		Open.DrawStyle = DRAWSTYLE_LINE;
		Open.DrawZeros = true;

		High.Name = "High";
		High.PrimaryColor = RGB(0,128,0);
		High.DrawStyle = DRAWSTYLE_LINE;
		High.DrawZeros = true;

		Low.Name  = "Low";
		Low.PrimaryColor = RGB(255,0,0);
		Low.DrawStyle = DRAWSTYLE_LINE;
		Low.DrawZeros = true;

		Close.Name = "Close";
		Close.PrimaryColor = RGB(128,0,0);
		Close.DrawStyle = DRAWSTYLE_LINE;
		Close.DrawZeros = true;

		return;
	}

	for (int i = sc.UpdateStartIndex; i < sc.ArraySize; i++)
	{
		float diff = iAskVol[i] - iBidVol[i];
		Open[i]  = iNumTrades[i] == 1 ? diff : 0;
		High[i]  = iDiffHigh[i];
		Low[i]   = iDiffLow[i];
		Close[i] = diff;
		if (Open[i] > High[i]) Open[i] = High[i];
		if (Open[i] < Low[i])  Open[i] = Low[i];
	}
}

/*==========================================================================*/
SCSFExport scsf_CumulativeDeltaBarsVolume(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Open    = sc.Subgraph[SC_OPEN];
	SCSubgraphRef High    = sc.Subgraph[SC_HIGH];
	SCSubgraphRef Low     = sc.Subgraph[SC_LOW];
	SCSubgraphRef Close   = sc.Subgraph[SC_LAST];

	SCSubgraphRef OHLCAvg = sc.Subgraph[SC_OHLC];
	SCSubgraphRef HLCAvg  = sc.Subgraph[SC_HLC];
	SCSubgraphRef HLAvg   = sc.Subgraph[SC_HL];

	SCInputRef ResetAtSessionStart = sc.Input[2];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Cumulative Delta Bars - Volume";

		sc.FreeDLL = 0;
		sc.MaintainAdditionalChartDataArrays = 1;
		sc.AutoLoop = 0;

		sc.ValueFormat = 0;
		sc.GraphDrawType = GDT_CANDLESTICK;

		Open.Name = "Open";
		Open.DrawStyle = DRAWSTYLE_LINE;
		Open.PrimaryColor = RGB(0,255,0);
		Open.SecondaryColor = RGB(0,255,0);
		Open.SecondaryColorUsed = true;
		Open.DrawZeros = true;

		High.Name = "High";
		High.DrawStyle = DRAWSTYLE_LINE;
		High.PrimaryColor = RGB(0,128,0);
		High.DrawZeros = true;

		Low.Name  = "Low";
		Low.DrawStyle = DRAWSTYLE_LINE;
		Low.PrimaryColor = RGB(255,0,0);
		Low.SecondaryColor = RGB(255,0,0);
		Low.SecondaryColorUsed = true;
		Low.DrawZeros = true;

		Close.Name = "Close";
		Close.DrawStyle = DRAWSTYLE_LINE;
		Close.PrimaryColor = RGB(128,0,0);
		Close.DrawZeros = true;

		OHLCAvg.Name = "OHLC Avg";
		OHLCAvg.DrawStyle = DRAWSTYLE_IGNORE;
		OHLCAvg.PrimaryColor = COLOR_GREEN;
		OHLCAvg.DrawZeros = true;

		HLCAvg.Name = "HLC Avg";
		HLCAvg.DrawStyle = DRAWSTYLE_IGNORE;
		HLCAvg.PrimaryColor = COLOR_GREEN;
		HLCAvg.DrawZeros = true;

		HLAvg.Name = "HL Avg";
		HLAvg.DrawStyle = DRAWSTYLE_IGNORE;
		HLAvg.PrimaryColor = COLOR_GREEN;
		HLAvg.DrawZeros = true;

		ResetAtSessionStart.Name = "Reset at Start of Trading Day";
		ResetAtSessionStart.SetYesNo(1);

		return;
	}

	SCDateTime NextSessionStart = sc.GetTradingDayStartDateTimeOfBar(sc.BaseDateTimeIn[sc.UpdateStartIndex - 1]) + 1*DAYS;

	for (int Index = sc.UpdateStartIndex; Index < sc.ArraySize; Index++)
	{
		bool FirstBar = Index == 0;

		if (ResetAtSessionStart.GetYesNo() != 0)
		{
			SCDateTime IndexDateTime = sc.BaseDateTimeIn[Index];

			if (IndexDateTime >= NextSessionStart)
			{
				FirstBar = true;
				NextSessionStart = sc.GetTradingDayStartDateTimeOfBar(IndexDateTime) + 1*DAYS;
			}
		}

		sc.CumulativeDeltaVolume(sc.BaseDataIn, Close, Index, FirstBar);
		Open[Index] = Close.Arrays[0] [Index];
		High[Index] = Close.Arrays[1] [Index];
		 Low[Index] = Close.Arrays[2] [Index];
	

		sc.CalculateOHLCAverages(Index);
	}
}

/*==========================================================================*/
SCSFExport scsf_CumulativeDeltaBarsTicks(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Open    = sc.Subgraph[SC_OPEN];
	SCSubgraphRef High    = sc.Subgraph[SC_HIGH];
	SCSubgraphRef Low     = sc.Subgraph[SC_LOW];
	SCSubgraphRef Close   = sc.Subgraph[SC_LAST];
	SCSubgraphRef OHLCAvg = sc.Subgraph[SC_OHLC];
	SCSubgraphRef HLCAvg  = sc.Subgraph[SC_HLC];
	SCSubgraphRef HLAvg   = sc.Subgraph[SC_HL];

	SCInputRef ResetAtSessionStart = sc.Input[2];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Cumulative Delta Bars - Trades";

		sc.FreeDLL = 0;
		sc.MaintainAdditionalChartDataArrays = 1;
		sc.AutoLoop = 0;

		sc.ValueFormat = 0;
		sc.GraphDrawType = GDT_CANDLESTICK;

		Open.Name = "Open";
		Open.DrawStyle = DRAWSTYLE_LINE;
		Open.PrimaryColor = RGB(0,255,0);
		Open.SecondaryColor = RGB(0,255,0);
		Open.SecondaryColorUsed = true;
		Open.DrawZeros = true;

		High.Name = "High";
		High.DrawStyle = DRAWSTYLE_LINE;
		High.PrimaryColor = RGB(0,128,0);
		High.DrawZeros = true;

		Low.Name  = "Low";
		Low.DrawStyle = DRAWSTYLE_LINE;
		Low.PrimaryColor = RGB(255,0,0);
		Low.SecondaryColor = RGB(255,0,0);
		Low.SecondaryColorUsed = true;
		Low.DrawZeros = true;

		Close.Name = "Close";
		Close.DrawStyle = DRAWSTYLE_LINE;
		Close.PrimaryColor = RGB(128,0,0);
		Close.DrawZeros = true;

		OHLCAvg.Name = "OHLC Avg";
		OHLCAvg.DrawStyle = DRAWSTYLE_IGNORE;
		OHLCAvg.PrimaryColor = COLOR_GREEN;
		OHLCAvg.DrawZeros = true;

		HLCAvg.Name = "HLC Avg";
		HLCAvg.DrawStyle = DRAWSTYLE_IGNORE;
		HLCAvg.PrimaryColor = COLOR_GREEN;
		HLCAvg.DrawZeros = true;

		HLAvg.Name = "HL Avg";
		HLAvg.DrawStyle = DRAWSTYLE_IGNORE;
		HLAvg.PrimaryColor = COLOR_GREEN;
		HLAvg.DrawZeros = true;

		ResetAtSessionStart.Name = "Reset at Start of Trading Day";
		ResetAtSessionStart.SetYesNo(1);

		return;
	}

	SCDateTime NextSessionStart = sc.GetTradingDayStartDateTimeOfBar(sc.BaseDateTimeIn[sc.UpdateStartIndex - 1]) + 1*DAYS;

	for (int i = sc.UpdateStartIndex; i < sc.ArraySize; i++)
	{
		bool FirstBar = i == 0;

		if (ResetAtSessionStart.GetYesNo() != 0)
		{
			SCDateTime IndexDateTime = sc.BaseDateTimeIn[i];

			if (IndexDateTime >= NextSessionStart)
			{
				FirstBar = true;
				NextSessionStart = sc.GetTradingDayStartDateTimeOfBar(IndexDateTime) + 1*DAYS;
			}
		}

		CumulativeDeltaTicks_S(sc.BaseDataIn, Open, High, Low, Close, i, FirstBar);

		sc.CalculateOHLCAverages(i);
	}
}

/*==========================================================================*/
SCSFExport scsf_CumulativeDeltaBarsTickVolume(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Open    = sc.Subgraph[SC_OPEN];
	SCSubgraphRef High    = sc.Subgraph[SC_HIGH];
	SCSubgraphRef Low     = sc.Subgraph[SC_LOW];
	SCSubgraphRef Close   = sc.Subgraph[SC_LAST];
	SCSubgraphRef OHLCAvg = sc.Subgraph[SC_OHLC];
	SCSubgraphRef HLCAvg  = sc.Subgraph[SC_HLC];
	SCSubgraphRef HLAvg   = sc.Subgraph[SC_HL];

	SCInputRef ResetAtSessionStart = sc.Input[2];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Cumulative Delta Bars - Up/Down Tick Volume";

		sc.FreeDLL = 0;
		sc.MaintainAdditionalChartDataArrays = 1;
		sc.AutoLoop = 0;

		sc.ValueFormat = 0;
		sc.GraphDrawType = GDT_CANDLESTICK;

		Open.Name = "Open";
		Open.DrawStyle = DRAWSTYLE_LINE;
		Open.PrimaryColor = RGB(0,255,0);
		Open.SecondaryColor = RGB(0,255,0);
		Open.SecondaryColorUsed = true;
		Open.DrawZeros = true;

		High.Name = "High";
		High.DrawStyle = DRAWSTYLE_LINE;
		High.PrimaryColor = RGB(0,128,0);
		High.DrawZeros = true;

		Low.Name  = "Low";
		Low.DrawStyle = DRAWSTYLE_LINE;
		Low.PrimaryColor = RGB(255,0,0);
		Low.SecondaryColor = RGB(255,0,0);
		Low.SecondaryColorUsed = true;
		Low.DrawZeros = true;

		Close.Name = "Close";
		Close.DrawStyle = DRAWSTYLE_LINE;
		Close.PrimaryColor = RGB(128,0,0);
		Close.DrawZeros = true;

		OHLCAvg.Name = "OHLC Avg";
		OHLCAvg.DrawStyle = DRAWSTYLE_IGNORE;
		OHLCAvg.PrimaryColor = COLOR_GREEN;
		OHLCAvg.DrawZeros = true;

		HLCAvg.Name = "HLC Avg";
		HLCAvg.DrawStyle = DRAWSTYLE_IGNORE;
		HLCAvg.PrimaryColor = COLOR_GREEN;
		HLCAvg.DrawZeros = true;

		HLAvg.Name = "HL Avg";
		HLAvg.DrawStyle = DRAWSTYLE_IGNORE;
		HLAvg.PrimaryColor = COLOR_GREEN;
		HLAvg.DrawZeros = true;

		ResetAtSessionStart.Name = "Reset at Start of Trading Day";
		ResetAtSessionStart.SetYesNo(1);

		return;
	}

	SCDateTime NextSessionStart = sc.GetTradingDayStartDateTimeOfBar(sc.BaseDateTimeIn[sc.UpdateStartIndex - 1]) + 1*DAYS;

	for (int Index = sc.UpdateStartIndex; Index < sc.ArraySize; Index++)
	{
		bool FirstBar = Index == 0;

		if (ResetAtSessionStart.GetYesNo() != 0)
		{
			SCDateTime IndexDateTime = sc.BaseDateTimeIn[Index];

			if (IndexDateTime >= NextSessionStart)
			{
				FirstBar = true;
				NextSessionStart = sc.GetTradingDayStartDateTimeOfBar(IndexDateTime) + 1*DAYS;
			}
		}

		CumulativeDeltaTickVolume_S(sc.BaseDataIn, Open, High, Low, Close, Index, FirstBar);

		sc.CalculateOHLCAverages(Index);
	}
}

/*==========================================================================*/
SCSFExport scsf_BackgroundDrawStyleExample(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Background   = sc.Subgraph[0];
	SCSubgraphRef BackgroundDC = sc.Subgraph[1];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults
		sc.GraphName = "Background DrawStyle Example";
		sc.StudyDescription = "Background DrawStyle Example";

		sc.AutoLoop = true;
		sc.FreeDLL = 0;

		sc.GraphRegion = 0;
		sc.DrawStudyUnderneathMainPriceGraph = 1;    // not required in studies, but want color behind price for this study

		Background.Name = "Background";
		Background.DrawStyle = DRAWSTYLE_BACKGROUND;
		Background.PrimaryColor = COLOR_LIGHTGREEN;
		Background.SecondaryColor = COLOR_LIGHTPINK;
		Background.SecondaryColorUsed = true;        // turn on if both colors are to be used
		Background.AutoColoring = AUTOCOLOR_POSNEG;  // use positive/negative values to signify colors

		BackgroundDC.Name = "Background DataColor";
		BackgroundDC.DrawStyle = DRAWSTYLE_BACKGROUND;
		BackgroundDC.PrimaryColor = RGB(255,0,255);

		return;
	}

	// Do data processing
	int min = sc.BaseDateTimeIn[sc.Index].GetMinute();

	if (min > 0 && min < 15)
		Background[sc.Index] = 0;    // do not color background
	else if (min >= 15 && min < 30)
		Background[sc.Index] = 1;    // use primary color
	else if (min >= 30 && min < 45)
		Background[sc.Index] = -1;   // use secondary color
	else if (min >= 45 && min < 60)
	{
		BackgroundDC[sc.Index] = 1;
		BackgroundDC.DataColor[sc.Index] = RGB(0, 0, 17*(60-min));
	}
}


/*==========================================================================*/
SCSFExport scsf_NumericInformationGraphDrawTypeExample(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Grid   = sc.Subgraph[0];
	SCSubgraphRef Open   = sc.Subgraph[1];
	SCSubgraphRef High   = sc.Subgraph[2];
	SCSubgraphRef Low    = sc.Subgraph[3];
	SCSubgraphRef Close  = sc.Subgraph[4];
	SCSubgraphRef Volume = sc.Subgraph[5];

	SCInputRef FontSize = sc.Input[0];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults
		sc.GraphName = "Numeric Information Graph Draw Type Example";
		sc.StudyDescription = "Numeric Information Graph Draw Type Example";

		sc.AutoLoop = true;
		sc.FreeDLL = 0;

		sc.GraphDrawType = GDT_NUMERIC_INFORMATION;

		FontSize.Name = "Font Size";
		FontSize.SetInt(8);
		FontSize.SetIntLimits(0,100);


		Grid.Name = "Grid Style";
		Grid.DrawStyle = DRAWSTYLE_LINE;
		Grid.PrimaryColor = COLOR_GRAY;

		Open.Name = "Open";
		Open.DrawStyle = DRAWSTYLE_LINE;
		Open.PrimaryColor = COLOR_CYAN;

		High.Name = "High";
		High.DrawStyle = DRAWSTYLE_LINE;
		High.PrimaryColor = COLOR_GREEN;

		Low.Name = "Low";
		Low.DrawStyle = DRAWSTYLE_LINE;
		Low.PrimaryColor = COLOR_RED;

		Close.Name = "Close";
		Close.DrawStyle = DRAWSTYLE_LINE;
		Close.PrimaryColor = COLOR_PURPLE;

		Volume.Name = "Volume";
		Volume.DrawStyle = DRAWSTYLE_LINE;
		Volume.PrimaryColor = COLOR_WHITE;

		return;
	}

	if (sc.Index == 0)
	{
		sc.ValueFormat = sc.BaseGraphValueFormat;

		// set up the information graph draw type
		s_NumericInformationGraphDrawTypeConfig NumericInformationGraphDrawTypeConfig;

		// only need to override defaults (see s_NumericInformationGraphDrawTypeConfig for defaults and other options to override)
		NumericInformationGraphDrawTypeConfig.GridlineStyleSubgraphIndex = 0;  // allow sg 1 to specify grid line style
		NumericInformationGraphDrawTypeConfig.FontSize = FontSize.GetInt();
		NumericInformationGraphDrawTypeConfig.ValueFormat[5] = 0;  // set value format for volume, others are inherited

		sc.SetNumericInformationGraphDrawTypeConfig(NumericInformationGraphDrawTypeConfig);

		// display volume first
		sc.SetNumericInformationDisplayOrderFromString("6,2,3,4,5");
	}

	// Do data processing
	Open[sc.Index]   = sc.Open[sc.Index];
	High[sc.Index]   = sc.High[sc.Index];
	Low[sc.Index]    = sc.Low[sc.Index];
	Close[sc.Index]  = sc.Close[sc.Index];
	Volume[sc.Index] = sc.Volume[sc.Index];

	// set cell foreground/background color
	if (Close[sc.Index] > Close[sc.Index-1])
		Close.DataColor[sc.Index] = sc.CombinedForegroundBackgroundColorRef(COLOR_BLACK, COLOR_GREEN);
	else
		Close.DataColor[sc.Index] = sc.CombinedForegroundBackgroundColorRef(COLOR_WHITE, COLOR_RED);
}




/*==========================================================================*/
SCSFExport scsf_StudySubgraphAbsoluteValue(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Result = sc.Subgraph[0];

	SCInputRef InputData = sc.Input[0];

	if (sc.SetDefaults)
	{

		sc.GraphName = "Study Subgraph Absolute Value";
		sc.StudyDescription = "This study calculates and displays the absolute value of the selected study Subgraph. To select the study to use, set the Based On study setting to that study and set the Input Data input to the specific study Subgraph in that study selected.";

		sc.AutoLoop = 1;

		sc.FreeDLL = 0; 

		Result.Name = "Abs";
		Result.DrawStyle = DRAWSTYLE_LINE;
		Result.PrimaryColor = RGB(0,255,0);
		Result.DrawZeros = true;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(0);


		return;
	}


	Result[sc.Index]= fabs(sc.BaseData[InputData.GetInputDataIndex()][sc.Index] );
}



/*==========================================================================*/
SCSFExport scsf_VolumeBitcoin(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Volume = sc.Subgraph[0];
	if (sc.SetDefaults)
	{
		sc.GraphName = "Volume - Bitcoin";

		sc.ValueFormat = sc.VolumeValueFormat;
		sc.AutoLoop = 1;

		Volume.Name = "Vol";
		Volume.DrawStyle = DRAWSTYLE_BAR;
		Volume.LineWidth= 2;
		Volume.PrimaryColor = RGB(0,255,0);
		Volume.SecondaryColor = RGB(255,0,0);
		Volume.SecondaryColorUsed = true;
		Volume.DrawZeros = true;
		Volume.AutoColoring= AUTOCOLOR_BASEGRAPH;

		sc.ScaleRangeType=SCALE_ZEROBASED;

		return;
	}

	Volume[sc.Index] = (float)sc.Volume[sc.Index]*sc.MultiplierFromVolumeValueFormat();
}



/*==========================================================================*/
SCSFExport scsf_VolumeZoneOscillator(SCStudyInterfaceRef sc)
{
	SCInputRef Period           = sc.Input[1];
	SCInputRef OverBoughtLevel3 = sc.Input[2];
	SCInputRef OverBoughtLevel2 = sc.Input[3];
	SCInputRef OverBoughtLevel1 = sc.Input[4];
	SCInputRef OverSoldLevel1   = sc.Input[5];
	SCInputRef OverSoldLevel2   = sc.Input[6];
	SCInputRef OverSoldLevel3   = sc.Input[7];


	SCSubgraphRef OverBought3 = sc.Subgraph[0];
	SCSubgraphRef OverBought2 = sc.Subgraph[1];
	SCSubgraphRef OverBought1 = sc.Subgraph[2];
	SCSubgraphRef Zero        = sc.Subgraph[3];
	SCSubgraphRef OverSold1   = sc.Subgraph[4];
	SCSubgraphRef OverSold2   = sc.Subgraph[5];
	SCSubgraphRef OverSold3   = sc.Subgraph[6];
	SCSubgraphRef VZO         = sc.Subgraph[7];

	SCFloatArrayRef R  = VZO.Arrays[0];
	SCFloatArrayRef VP = VZO.Arrays[1];
	SCFloatArrayRef TV = VZO.Arrays[2];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Volume Zone Oscillator";

		sc.StudyDescription = "TASC Article, May, 2011, In The Volume Zone";

		sc.GraphRegion = 1;
		sc.ValueFormat = 2;
		sc.AutoLoop = 1;
		sc.DrawZeros = 1;

		// inputs
		Period.Name = "Period";
		Period.SetInt(14);
		Period.SetIntLimits(1, MAX_STUDY_LENGTH);

		OverBoughtLevel3.Name = "Overbought Line3 Value";
		OverBoughtLevel3.SetFloat(60.0f);

		OverBoughtLevel2.Name = "Overbought Line2 Value";
		OverBoughtLevel2.SetFloat(40.0f);

		OverBoughtLevel1.Name = "Overbought Line1 Value";
		OverBoughtLevel1.SetFloat(15.0f);

		OverSoldLevel1.Name = "Oversold Line1 Value";
		OverSoldLevel1.SetFloat(-5.0f);

		OverSoldLevel2.Name = "Oversold Line2 Value";
		OverSoldLevel2.SetFloat(-40.0f);

		OverSoldLevel3.Name = "Oversold Line3 Value";
		OverSoldLevel3.SetFloat(-60.0f);

		// subgraphs
		VZO.Name = "VZO";
		VZO.DrawStyle = DRAWSTYLE_LINE;
		VZO.PrimaryColor = RGB(0,255,0);
		VZO.DrawZeros = true;

		OverBought3.Name = "Over Bought Level 3";
		OverBought3.DrawStyle = DRAWSTYLE_LINE;
		OverBought3.PrimaryColor = RGB(255,0,255);
		OverBought3.DrawZeros = true;
		OverBought3.DisplayNameValueInWindowsFlags = 0;

		OverBought2.Name = "Over Bought Level 2";
		OverBought2.DrawStyle = DRAWSTYLE_LINE;
		OverBought2.PrimaryColor = RGB(255,0,255);
		OverBought2.DrawZeros = true;
		OverBought2.DisplayNameValueInWindowsFlags = 0;

		OverBought1.Name = "Over Bought Level 1";
		OverBought1.DrawStyle = DRAWSTYLE_LINE;
		OverBought1.PrimaryColor = RGB(255,0,255);
		OverBought1.DrawZeros = true;
		OverBought1.DisplayNameValueInWindowsFlags = 0;

		Zero.Name = "Zero";
		Zero.DrawStyle = DRAWSTYLE_LINE;
		Zero.PrimaryColor = RGB(255,255,255);
		Zero.DrawZeros = true;
		Zero.DisplayNameValueInWindowsFlags = 0;

		OverSold1.Name = "Over Sold Level 1";
		OverSold1.DrawStyle = DRAWSTYLE_LINE;
		OverSold1.PrimaryColor = RGB(255,255,0);
		OverSold1.DrawZeros = true;
		OverSold1.DisplayNameValueInWindowsFlags = 0;

		OverSold2.Name = "Over Sold Level 2";
		OverSold2.DrawStyle = DRAWSTYLE_LINE;
		OverSold2.PrimaryColor = RGB(255,255,0);
		OverSold2.DrawZeros = true;
		OverSold2.DisplayNameValueInWindowsFlags = 0;

		OverSold3.Name = "Over Sold Level 3";
		OverSold3.DrawStyle = DRAWSTYLE_LINE;
		OverSold3.PrimaryColor = RGB(255,255,0);
		OverSold3.DrawZeros = true;
		OverSold3.DisplayNameValueInWindowsFlags = 0;

		return;
	}

	OverBought3[sc.Index] = OverBoughtLevel3.GetFloat();
	OverBought2[sc.Index] = OverBoughtLevel2.GetFloat();
	OverBought1[sc.Index] = OverBoughtLevel1.GetFloat();
	Zero[sc.Index] = 0;
	OverSold1[sc.Index] = OverSoldLevel1.GetFloat();
	OverSold2[sc.Index] = OverSoldLevel2.GetFloat();
	OverSold3[sc.Index] = OverSoldLevel3.GetFloat();

	if (sc.Index == 0)
	{
		R[sc.Index] = sc.Volume[sc.Index];
		return;
	}
	else
	{
		if (sc.Close[sc.Index] > sc.Close[sc.Index-1])
			R[sc.Index] = sc.Volume[sc.Index];
		else
			R[sc.Index] = -sc.Volume[sc.Index];
	}

	sc.MovingAverage(R, VP, MOVAVGTYPE_EXPONENTIAL, Period.GetInt());
	sc.MovingAverage(sc.Volume, TV, MOVAVGTYPE_EXPONENTIAL, Period.GetInt());

	if (TV[sc.Index] != 0)
	{
		VZO[sc.Index] = VP[sc.Index] / TV[sc.Index] * 100.0f;
	}
}

/*==========================================================================*/
SCSFExport scsf_FisherTransform(SCStudyInterfaceRef sc)
{
	SCInputRef iPrice  = sc.Input[0];
	SCInputRef iPeriod = sc.Input[1];

	SCSubgraphRef Zero              = sc.Subgraph[0];
	SCSubgraphRef FisherTransform   = sc.Subgraph[1];
	SCSubgraphRef FisherTransformM1 = sc.Subgraph[2];

	SCFloatArrayRef Value = Zero.Arrays[0];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Fisher Transform";

		sc.StudyDescription = "Fisher Transform";

		sc.GraphRegion = 1;
		sc.ValueFormat = 2;
		sc.AutoLoop = 1;
		sc.DrawZeros = 1;

		// inputs
		iPrice.Name = "Price";
		iPrice.SetInputDataIndex(SC_HL);

		iPeriod.Name = "Period";
		iPeriod.SetInt(10);
		iPeriod.SetIntLimits(1, MAX_STUDY_LENGTH);

		// subgraphs
		FisherTransform.Name = "FisherTransform";
		FisherTransform.DrawStyle = DRAWSTYLE_LINE;
		FisherTransform.PrimaryColor = RGB(255,0,0);
		FisherTransform.DrawZeros = true;

		FisherTransformM1.Name = "FisherTransformOffset";
		FisherTransformM1.DrawStyle = DRAWSTYLE_LINE;
		FisherTransformM1.PrimaryColor = RGB(0,0,255);
		FisherTransformM1.DrawZeros = true;

		Zero.Name = "Zero";
		Zero.DrawStyle = DRAWSTYLE_LINE;
		Zero.PrimaryColor = RGB(255,255,255);
		Zero.DrawZeros = true;
		Zero.DisplayNameValueInWindowsFlags = 0;

		return;
	}

	Zero[sc.Index] = 0;

	if (sc.Index != 0)
	{
		SCFloatArrayRef Price = sc.BaseData[iPrice.GetInputDataIndex()];

		float Highest = sc.GetHighest(Price, iPeriod.GetInt());
		float Lowest  = sc.GetLowest(Price, iPeriod.GetInt());
		float Range   = Highest - Lowest;

		if (Range == 0)
			Value[sc.Index] = 0;
		else
			Value[sc.Index] = .66f * ((Price[sc.Index] - Lowest) / Range - 0.5f) + 0.67f * Value[sc.Index-1];

		float TruncValue = Value[sc.Index];

		if (TruncValue > .99f)
			TruncValue = .999f;
		else if (TruncValue < -.99f)
			TruncValue = -.999f;

		float Fisher = .5f * (log((1 + TruncValue) / (1 - TruncValue)) + FisherTransform[sc.Index-1]);

		FisherTransformM1[sc.Index] = FisherTransform[sc.Index-1];
		FisherTransform[sc.Index] = Fisher;
	}
}


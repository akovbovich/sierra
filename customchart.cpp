/*****************************************************************************

This file contains the scsf_CopyOfBaseGraph function which is an advanced
custom study function that creates a copy of the existing base graph in a
chart. You can modify this or use it as an example to create your own custom
chart.


*****************************************************************************/

#include "sierrachart.h"

/*==========================================================================*/
SCSFExport scsf_CopyOfBaseGraph(SCStudyInterfaceRef sc)
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
	//SCSubgraphRef BidVol = sc.Subgraph[9];
//	SCSubgraphRef AskVol = sc.Subgraph[10];

	// Set configuration variables
	
	if (sc.SetDefaults)
	{
		// Set the defaults
		
		sc.GraphName = "Copy of BaseGraph Example";
		
		sc.StudyDescription = "This is an example of how to create a custom chart. This one is a copy of the BaseGraph.";
		
		// This member indicates that this is a custom chart study. 
		//	This can only be set when SetDefaults is true.
		sc.IsCustomChart = 1;  // true		
		sc.GraphRegion = 0;  // First region		
		sc.DrawZeros = 0;  // false		
		sc.GraphDrawType = GDT_OHLCBAR;
		sc.StandardChartHeader = 1; // true
		
		// Subgraphs

		Open.Name = "Open";
		Open.DrawStyle = DRAWSTYLE_LINE;

		High.Name = "High";
		High.DrawStyle = DRAWSTYLE_LINE;

		Low.Name = "Low";
		Low.DrawStyle = DRAWSTYLE_LINE;

		Last.Name = "Last";
		Last.DrawStyle = DRAWSTYLE_LINE;

		Volume.Name = "Volume";
		Volume.DrawStyle = DRAWSTYLE_IGNORE;

		OpenInterest.Name = "# of Trades / OI";
		OpenInterest.DrawStyle = DRAWSTYLE_IGNORE;

		OHLCAvg.Name = "OHLC Avg";
		OHLCAvg.DrawStyle = DRAWSTYLE_IGNORE;

		HLCAvg.Name = "HLC Avg";
		HLCAvg.DrawStyle = DRAWSTYLE_IGNORE;

		HLAvg.Name = "HL Avg";
		HLAvg.DrawStyle = DRAWSTYLE_IGNORE;

	/*	BidVol.Name = "Bid Vol";
		BidVol.DrawStyle = DRAWSTYLE_IGNORE;

		AskVol.Name = "Ask Vol";
		AskVol.DrawStyle = DRAWSTYLE_IGNORE;*/

		return;
	}
	
	// If our start index is zero, then we want to clear our existing chart
	// data, if any, so we start clean to avoid any possible problems.  This
	// is accomplished by using sc.ResizeArrays.
	if (sc.UpdateStartIndex == 0)
	{
		sc.ResizeArrays(0);
	}
	
	// Do data processing
	for (int i = sc.UpdateStartIndex; i < sc.ArraySize; i++)
	{
		if (sc.OutArraySize - 1 < i)
			sc.AddElements(1);
		
		sc.DateTimeOut[i] = sc.BaseDateTimeIn[i];
		
		for (int SubGraph = 0; SubGraph <=8; SubGraph++)
		{
			sc.Subgraph[SubGraph][i] = sc.BaseDataIn[SubGraph][i];

			//For testing coloring bars
			//sc.Subgraph[SubGraph].DataColor[i] = RGB(255,255,255);
		}
	}
}



#include "sierrachart.h"

/*==========================================================================*/


SCSFExport scsf_TradingExample(SCStudyInterfaceRef sc)
{
	//Define references to the Subgraphs and Inputs for easy reference
	SCSubgraphRef BuyEntrySubgraph = sc.Subgraph[0];
	SCSubgraphRef BuyExitSubgraph = sc.Subgraph[1];
	SCSubgraphRef SellEntrySubgraph = sc.Subgraph[2];
	SCSubgraphRef SellExitSubgraph = sc.Subgraph[3];

	SCSubgraphRef SimpMovAvgSubgraph = sc.Subgraph[4];

	SCInputRef Enabled = sc.Input[0];
	SCInputRef TargetValue = sc.Input[1];
	SCInputRef StopValue = sc.Input[2];


	if (sc.SetDefaults)
	{
		// Set the study configuration and defaults.

		sc.GraphName = "Trading Example";

		BuyEntrySubgraph.Name = "Buy Entry";
		BuyEntrySubgraph.DrawStyle = DRAWSTYLE_ARROWUP;
		BuyEntrySubgraph.PrimaryColor = RGB(0, 255, 0);
		BuyEntrySubgraph.LineWidth = 2;
		BuyEntrySubgraph.DrawZeros = false;

		BuyExitSubgraph.Name = "Buy Exit";
		BuyExitSubgraph.DrawStyle = DRAWSTYLE_ARROWDOWN;
		BuyExitSubgraph.PrimaryColor = RGB(255, 128, 128);
		BuyExitSubgraph.LineWidth = 2;
		BuyExitSubgraph.DrawZeros = false;

		SellEntrySubgraph.Name = "Sell Entry";
		SellEntrySubgraph.DrawStyle = DRAWSTYLE_ARROWDOWN;
		SellEntrySubgraph.PrimaryColor = RGB(255, 0, 0);
		SellEntrySubgraph.LineWidth = 2;
		SellEntrySubgraph.DrawZeros = false;

		SellExitSubgraph.Name = "Sell Exit";
		SellExitSubgraph.DrawStyle = DRAWSTYLE_ARROWUP;
		SellExitSubgraph.PrimaryColor = RGB(128, 255, 128);
		SellExitSubgraph.LineWidth = 2;
		SellExitSubgraph.DrawZeros = false;

		SimpMovAvgSubgraph.Name = "Simple Moving Average";
		SimpMovAvgSubgraph.DrawStyle = DRAWSTYLE_LINE;
		SimpMovAvgSubgraph.DrawZeros = false;

		Enabled.Name = "Enabled";
		Enabled.SetYesNo(0); 

		TargetValue.Name = "Target Value";
		TargetValue.SetFloat(2.0f);

		StopValue.Name = "Stop Value";
		StopValue.SetFloat(1.0f);

		sc.StudyDescription = "This study function is an example of how to use the ACSIL Trading Functions. This function will display a simple moving average and perform a Buy Entry when the Last price crosses the moving average from below and a Sell Entry when the Last price crosses the moving average from above. A new entry cannot occur until the Target or Stop has been hit. When an order is sent, a corresponding arrow will appear on the chart to show that an order was sent. This study will do nothing until the Enabled Input is set to Yes.";

		sc.AllowMultipleEntriesInSameDirection = false; 
		sc.MaximumPositionAllowed = 10;
		sc.SupportReversals = false;

		// This is false by default. Orders will go to the simulation system always.
		sc.SendOrdersToTradeService = false;

		sc.AllowOppositeEntryWithOpposingPositionOrOrders = false;
		sc.SupportAttachedOrdersForTrading = false;
		sc.CancelAllOrdersOnEntriesAndReversals= true;
		sc.AllowEntryWithWorkingOrders = false;
		sc.CancelAllWorkingOrdersOnExit = false;

		// Only 1 trade for each Order Action type is allowed per bar.
		sc.AllowOnlyOneTradePerBar = true; 

		//This needs to be set to true when a trading study uses trading functions.
		sc.MaintainTradeStatisticsAndTradesData = true;

		sc.AutoLoop = 1;
		sc.GraphRegion = 0;

		// During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;

		return;
	}




	if (!Enabled.GetYesNo())
		return;
	
	SCFloatArrayRef Last = sc.Close;
	
	// Calculate the moving average
	sc.SimpleMovAvg(Last, SimpMovAvgSubgraph,  sc.Index, 10);
	

	// Get the Trade Position data to be used for position exit processing.
	s_SCPositionData PositionData;
	sc.GetTradePosition(PositionData) ;


	float LastTradePrice = sc.Close[sc.Index];

	// Create an s_SCNewOrder object. 
	s_SCNewOrder NewOrder;
	NewOrder.OrderQuantity = 1;
	NewOrder.OrderType = SCT_MARKET;
	int Result;

	// Buy when the last price crosses the moving average from below.
	if (sc.CrossOver(Last, SimpMovAvgSubgraph) == CROSS_FROM_BOTTOM && sc.GetBarHasClosedStatus() == BHCS_BAR_HAS_CLOSED)
	{
			Result = sc.BuyEntry(NewOrder);
			if (Result > 0) //If there has been a successful order entry, then draw an arrow at the low of the bar.
				BuyEntrySubgraph[sc.Index] = sc.Low[sc.Index];
	}
	
	
	// When there is a long position AND the Last price is less than the price the Buy Entry was filled at minus Stop Value, OR there is a long position AND the Last price is greater than the price the Buy Entry was filled at plus the Target Value.
	else if (PositionData.PositionQuantity > 0
		&& (LastTradePrice <= PositionData.AveragePrice - StopValue.GetFloat() || 
		LastTradePrice >= PositionData.AveragePrice + TargetValue.GetFloat())) 
	{
		Result = sc.BuyExit(NewOrder);
		if(Result>0) //If there has been a successful order entry, then draw an arrow at the high of the bar.
			BuyExitSubgraph[sc.Index] = sc.High[sc.Index];
	}
	
	
	// Sell when the last price crosses the moving average from above.
	else if (sc.CrossOver(Last, SimpMovAvgSubgraph) == CROSS_FROM_TOP && sc.GetBarHasClosedStatus() == BHCS_BAR_HAS_CLOSED)
	{
		Result = sc.SellEntry(NewOrder);
		if(Result>0) //If there has been a successful order entry, then draw an arrow at the high of the bar.
			SellEntrySubgraph[sc.Index] = sc.High[sc.Index];
	}
	
	// When there is a short position AND the Last price is greater than the price the Sell Entry was filled at plus the Stop Value, OR there is a short position AND the Last price is less than the price the Sell Entry was filled at minus the Target Value.
	else if (PositionData.PositionQuantity < 0 
		&& (LastTradePrice >= PositionData.AveragePrice + StopValue.GetFloat() ||
		LastTradePrice <= PositionData.AveragePrice - TargetValue.GetFloat())) 
	{
		Result = sc.SellExit(NewOrder);
		if(Result>0) //If there has been a successful order entry, then draw an arrow at the low of the bar.
			SellExitSubgraph[sc.Index] = sc.Low[sc.Index];
	}

}

/*==========================================================================*/ 
SCSFExport scsf_TradingExampleWithAttachedOrders(SCStudyInterfaceRef sc)
{
	//Define references to the Subgraphs and Inputs for easy reference
	SCSubgraphRef BuyEntrySubgraph = sc.Subgraph[0];
	SCSubgraphRef SellEntrySubgraph = sc.Subgraph[2];
	SCSubgraphRef SimpMovAvgSubgraph = sc.Subgraph[4];

	SCInputRef Enabled = sc.Input[0];


	if (sc.SetDefaults)
	{
		// Set the study configuration and defaults.

		sc.GraphName = "Trading Example With Attached Orders";

		BuyEntrySubgraph.Name = "Buy Entry";
		BuyEntrySubgraph.DrawStyle = DRAWSTYLE_ARROWUP;
		BuyEntrySubgraph.PrimaryColor = RGB(0, 255, 0);
		BuyEntrySubgraph.LineWidth = 2;
		BuyEntrySubgraph.DrawZeros = false;

		SellEntrySubgraph.Name = "Sell Entry";
		SellEntrySubgraph.DrawStyle = DRAWSTYLE_ARROWDOWN;
		SellEntrySubgraph.PrimaryColor = RGB(255, 0, 0);
		SellEntrySubgraph.LineWidth = 2;
		SellEntrySubgraph.DrawZeros = false;


		SimpMovAvgSubgraph.Name = "Simple Moving Average";
		SimpMovAvgSubgraph.DrawStyle = DRAWSTYLE_LINE;
		SimpMovAvgSubgraph.DrawZeros = false;

		Enabled.Name = "Enabled";
		Enabled.SetYesNo(0); 

		sc.StudyDescription = "This study function is an example of how to use the ACSIL Trading Functions.  This example uses the Attached Orders defined on the chart Trade Window. This function will display a simple moving average and perform a Buy Entry when the Last price crosses the moving average from below and a Sell Entry when the Last price crosses the moving average from above. A new entry cannot occur until the Target or Stop has been hit. When an order is sent, a corresponding arrow will appear on the chart to show that an order was sent. This study will do nothing until the Enabled Input is set to Yes.";

		sc.AllowMultipleEntriesInSameDirection = false; 
		sc.MaximumPositionAllowed = 10;
		sc.SupportReversals = false;

		// This is false by default. Orders will go to the simulation system always.
		sc.SendOrdersToTradeService = false;

		sc.AllowOppositeEntryWithOpposingPositionOrOrders = false;
		sc.SupportAttachedOrdersForTrading = true;

		// This line can be within sc.SetDefaults or outside of it.
		sc.TradeWindowConfigFileName = "SimpleBracket.twconfig";

		sc.CancelAllOrdersOnEntriesAndReversals= true;
		sc.AllowEntryWithWorkingOrders = false;
		sc.CancelAllWorkingOrdersOnExit = true;

		// Only 1 trade for each Order Action type is allowed per bar.
		sc.AllowOnlyOneTradePerBar = true; 

		//This needs to be set to true when a trading study uses trading functions.
		sc.MaintainTradeStatisticsAndTradesData = true;

		sc.AutoLoop = 1;
		sc.GraphRegion = 0;

		// During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;

		return;
	}




	if (!Enabled.GetYesNo())
		return;


	SCFloatArrayRef Last = sc.Close;

	// Calculate the moving average
	sc.SimpleMovAvg(Last, SimpMovAvgSubgraph, sc.Index, 10);


	// Create an s_SCNewOrder object. 
	s_SCNewOrder NewOrder;
	NewOrder.OrderQuantity = 1;
	NewOrder.OrderType = SCT_MARKET;


	// Buy when the last price crosses the moving average from below.
	if (sc.CrossOver(Last, SimpMovAvgSubgraph) == CROSS_FROM_BOTTOM && sc.GetBarHasClosedStatus() == BHCS_BAR_HAS_CLOSED)
	{
		int Result = sc.BuyEntry(NewOrder);
		if (Result > 0) //If there has been a successful order entry, then draw an arrow at the low of the bar.
			BuyEntrySubgraph[sc.Index] = sc.Low[sc.Index];
	}


	// Sell when the last price crosses the moving average from above.
	else if (sc.CrossOver(Last, SimpMovAvgSubgraph) == CROSS_FROM_TOP && sc.GetBarHasClosedStatus() == BHCS_BAR_HAS_CLOSED)
	{
		int Result = sc.SellEntry(NewOrder);
		if (Result > 0) //If there has been a successful order entry, then draw an arrow at the high of the bar.
			SellEntrySubgraph[sc.Index] = sc.High[sc.Index];
	}
}

/*==========================================================================*/
SCSFExport scsf_TradingExampleWithAttachedOrdersDirectlyDefined(SCStudyInterfaceRef sc)
{
	// Define references to the Subgraphs and Inputs for easy reference
	SCSubgraphRef BuyEntrySubgraph = sc.Subgraph[0];
	SCSubgraphRef SellEntrySubgraph = sc.Subgraph[2];
	SCSubgraphRef SimpMovAvgSubgraph = sc.Subgraph[4];

	SCInputRef Enabled = sc.Input[0];


	if (sc.SetDefaults)
	{
		// Set the study configuration and defaults.

		sc.GraphName = "Trading Example With Hardcoded Attached Orders";

		BuyEntrySubgraph.Name = "Buy Entry";
		BuyEntrySubgraph.DrawStyle = DRAWSTYLE_ARROWUP;
		BuyEntrySubgraph.PrimaryColor = RGB(0, 255, 0);
		BuyEntrySubgraph.LineWidth = 2;
		BuyEntrySubgraph.DrawZeros = false;

		SellEntrySubgraph.Name = "Sell Entry";
		SellEntrySubgraph.DrawStyle = DRAWSTYLE_ARROWDOWN;
		SellEntrySubgraph.PrimaryColor = RGB(255, 0, 0);
		SellEntrySubgraph.LineWidth = 2;
		SellEntrySubgraph.DrawZeros = false;

		SimpMovAvgSubgraph.Name = "Simple Moving Average";
		SimpMovAvgSubgraph.DrawStyle = DRAWSTYLE_LINE;
		SimpMovAvgSubgraph.DrawZeros = false;

		Enabled.Name = "Enabled";
		Enabled.SetYesNo(0); 

		sc.StudyDescription = "This study function is an example of how to use the ACSIL Trading Functions.  This example uses Attached Orders defined directly within this function. This function will display a simple moving average and perform a Buy Entry when the Last price crosses the moving average from below and a Sell Entry when the Last price crosses the moving average from above. A new entry cannot occur until the Target or Stop has been hit. When an order is sent, a corresponding arrow will appear on the chart to show that an order was sent. This study will do nothing until the Enabled Input is set to Yes.";

		sc.AllowMultipleEntriesInSameDirection = false; 
		sc.MaximumPositionAllowed = 5;
		sc.SupportReversals = false;

		// This is false by default. Orders will go to the simulation system always.
		sc.SendOrdersToTradeService = false;

		sc.AllowOppositeEntryWithOpposingPositionOrOrders = false;

		// This can be false in this function because we specify Attached Orders directly with the order which causes this to be considered true when submitting an order.
		sc.SupportAttachedOrdersForTrading = false;  

		sc.CancelAllOrdersOnEntriesAndReversals= true;
		sc.AllowEntryWithWorkingOrders = false;
		sc.CancelAllWorkingOrdersOnExit = true;

		// Only 1 trade for each Order Action type is allowed per bar.
		sc.AllowOnlyOneTradePerBar = true; 

		//This needs to be set to true when a trading study uses trading functions.
		sc.MaintainTradeStatisticsAndTradesData = true;

		sc.AutoLoop = 1;
		sc.GraphRegion = 0;

		// During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;

		return;
	}

	if (!Enabled.GetYesNo())
		return;


	SCFloatArrayRef Last = sc.Close;

	// Use persistent variables to remember attached order IDs so they can be modified or canceled. 
	int& Target1OrderID = sc.PersistVars->i1;
	int& Stop1OrderID = sc.PersistVars->i2;

	// Calculate the moving average
	sc.SimpleMovAvg(Last, SimpMovAvgSubgraph,  sc.Index, 10);


	// Create an s_SCNewOrder object. 
	s_SCNewOrder NewOrder;
	NewOrder.OrderQuantity = 1;
	NewOrder.OrderType = SCT_MARKET;
	NewOrder.TimeInForce = SCT_TIF_GTC;

	//Specify a Target and a Stop with 8 tick offsets. We are specifying one Target and one Stop, additional targets and stops can be specified as well. 
	NewOrder.Target1Offset = 8*sc.TickSize;
	NewOrder.Stop1Offset = 8*sc.TickSize;
	NewOrder.OCOGroup1Quantity = 1; // If this is left at the default of 0, then it will be automatically set.

	// Buy when the last price crosses the moving average from below.
	if (sc.CrossOver(Last, SimpMovAvgSubgraph) == CROSS_FROM_BOTTOM && sc.GetBarHasClosedStatus() == BHCS_BAR_HAS_CLOSED)
	{
		int Result = sc.BuyEntry(NewOrder);
		if (Result > 0) //If there has been a successful order entry, then draw an arrow at the low of the bar.
		{
			BuyEntrySubgraph[sc.Index] = sc.Low[sc.Index];

			// Remember the order IDs for subsequent modification and cancellation
			Target1OrderID = NewOrder.Target1InternalOrderID;
			Stop1OrderID = NewOrder.Stop1InternalOrderID;
		}
	}


	// Sell when the last price crosses the moving average from above.
	else if (sc.CrossOver(Last, SimpMovAvgSubgraph) == CROSS_FROM_TOP && sc.GetBarHasClosedStatus() == BHCS_BAR_HAS_CLOSED)
	{
		int Result = sc.SellEntry(NewOrder);
		if (Result > 0) //If there has been a successful order entry, then draw an arrow at the high of the bar.
		{
			SellEntrySubgraph[sc.Index] = sc.High[sc.Index];

			// Remember the order IDs for subsequent modification and cancellation
			Target1OrderID = NewOrder.Target1InternalOrderID;
			Stop1OrderID = NewOrder.Stop1InternalOrderID;
		}
	}

	
	// This code block serves as an example of how to modify an attached order.  In this example it is set to false and never runs.
	bool ExecuteModifyOrder = false;
	if (ExecuteModifyOrder && (sc.Index == sc.ArraySize - 1))//Only  do a modify on the most recent bar
	{
		double NewLimit = 0.0;
		
		// Get the existing target order
		s_SCTradeOrder ExistingOrder;
		if (sc.GetOrderByOrderID(Target1OrderID, ExistingOrder) != SCTRADING_ORDER_ERROR)
		{
			if (ExistingOrder.BuySell == BSE_BUY)
				NewLimit = sc.Close[sc.Index] - 5*sc.TickSize;
			else if (ExistingOrder.BuySell == BSE_SELL)
				NewLimit = sc.Close[sc.Index] + 5*sc.TickSize;
			
			// We can only modify price and/or quantity
			
			s_SCNewOrder ModifyOrder;
			ModifyOrder.InternalOrderID = Target1OrderID;
			ModifyOrder.Price1 = NewLimit;

			sc.ModifyOrder(ModifyOrder);
		}
	}


	////Target 1 
	//NewOrder.Target1Offset =10*sc.TickSize; 
	//NewOrder.AttachedOrderTarget1Type = SCT_ORDERTYPE_LIMIT; 

	////Target 2 
	//NewOrder.Target2Offset = 10*sc.TickSize; 
	//NewOrder.AttachedOrderTarget2Type = SCT_ORDERTYPE_LIMIT; 

	////Common Stop 
	//NewOrder.StopAllOffset = 10* sc.TickSize; 
	//NewOrder.AttachedOrderStopAllType = SCT_ORDERTYPE_STOP; 
}

/*==========================================================================*/
//This function is used for ACSIL trading testing. It does NOT serve as an example.

SCSFExport scsf_ACSILTradingTest(SCStudyInterfaceRef sc)
{
	//Define references to the Subgraphs and Inputs for easy reference
	SCSubgraphRef BuyEntrySubgraph = sc.Subgraph[0];
	SCSubgraphRef BuyExitSubgraph = sc.Subgraph[1];
	SCSubgraphRef SellEntrySubgraph = sc.Subgraph[2];
	SCSubgraphRef SellExitSubgraph = sc.Subgraph[3];

	SCInputRef Enabled = sc.Input[0];


	if (sc.SetDefaults)
	{
		// Set the study configuration and defaults.

		sc.GraphName = "Trading Test";

		BuyEntrySubgraph.Name = "Buy Entry";
		BuyEntrySubgraph.DrawStyle = DRAWSTYLE_ARROWUP;
		BuyEntrySubgraph.PrimaryColor = RGB(0, 255, 0);
		BuyEntrySubgraph.LineWidth = 2;
		BuyEntrySubgraph.DrawZeros = false;

		BuyExitSubgraph.Name = "Buy Exit";
		BuyExitSubgraph.DrawStyle = DRAWSTYLE_ARROWDOWN;
		BuyExitSubgraph.PrimaryColor = RGB(255, 128, 128);
		BuyExitSubgraph.LineWidth = 2;
		BuyExitSubgraph.DrawZeros = false;

		SellEntrySubgraph.Name = "Sell Entry";
		SellEntrySubgraph.DrawStyle = DRAWSTYLE_ARROWDOWN;
		SellEntrySubgraph.PrimaryColor = RGB(255, 0, 0);
		SellEntrySubgraph.LineWidth = 2;
		SellEntrySubgraph.DrawZeros = false;

		SellExitSubgraph.Name = "Sell Exit";
		SellExitSubgraph.DrawStyle = DRAWSTYLE_ARROWUP;
		SellExitSubgraph.PrimaryColor = RGB(128, 255, 128);
		SellExitSubgraph.LineWidth = 2;
		SellExitSubgraph.DrawZeros = false;

		Enabled.Name = "Enabled";
		Enabled.SetYesNo(0); 
		Enabled.  SetDescription("This input enables the study and allows it to function. Otherwise, it does nothing.");


		// This is false by default. Orders will be simulated.
		sc.SendOrdersToTradeService = false;

		sc.AllowMultipleEntriesInSameDirection = true;

		//This must be equal to or greater than the order quantities you will be  submitting orders for.
		sc.MaximumPositionAllowed = 10000;

		sc.SupportReversals = false;
		sc.AllowOppositeEntryWithOpposingPositionOrOrders = true;

		//  This variable controls whether to use or not use attached orders that are configured on the Trade Window for the chart.
		sc.SupportAttachedOrdersForTrading = false;

		//This variable controls whether to use the "Use Attached Orders" setting on the Trade Window for the chart
		sc.UseGUIAttachedOrderSetting = false;

		sc.CancelAllOrdersOnEntriesAndReversals= false;
		sc.AllowEntryWithWorkingOrders = true;
		sc.CancelAllWorkingOrdersOnExit = true;
		sc.AllowOnlyOneTradePerBar = false;

		//This needs to be set to true when a trading study uses trading functions.
		sc.MaintainTradeStatisticsAndTradesData = true;

		sc.AutoLoop = true;
		sc.GraphRegion = 0;

		// During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;

		return;
	}

	//return;

	//These must be outside of the sc.SetDefaults code block
	sc.SupportTradingScaleIn = 1;
	sc.SupportTradingScaleOut = 0;


	if (!Enabled.GetYesNo())
		return;

	if ( sc.LastCallToFunction )
		return;//nothing to do


	// Run the below code only on the last bar
	if (sc.Index < sc.ArraySize-1)
		return;


	s_SCPositionData PositionData;
	sc.GetTradePosition(  PositionData);
	if (PositionData.PositionQuantity== 0)
	{
		// Create an s_SCNewOrder object. 
		s_SCNewOrder NewOrder;
		NewOrder.OrderQuantity = 1;
		NewOrder.OrderType = SCT_MARKET;
		NewOrder.TimeInForce = SCT_TIF_DAY;

		//Specify a Target and a Stop with 8 tick offsets. We are specifying one Target and one Stop, additional targets and stops can be specified as well. 
		NewOrder.Target1Offset = 8*sc.TickSize;
		NewOrder.Stop1Offset = 8*sc.TickSize;
		NewOrder.OCOGroup1Quantity = 1; // If this is left at the default of 0, then it will be automatically set.
		sc.BuyEntry(NewOrder);
	}
	//else
	//{
	//	s_SCNewOrder NewOrder;
	//	NewOrder.OrderQuantity = 0; //Flatten
	//	NewOrder.OrderType = SCT_MARKET;
	//	sc.BuyExit(NewOrder);
	//}
	

	//int &OrderCount = sc.PersistVars->i1;

	//if (OrderCount > 0)
	//	return;
	//

	//s_SCNewOrder EntryOrder; 
	//EntryOrder.OrderQuantity = 10000; // full position size 
	//EntryOrder.OrderType = SCT_MARKET; 

	//int Result = sc.BuyEntry(EntryOrder);
	//if(Result > 0)
	//{
	//	

	//	s_SCNewOrder ExitOrder; 
	//	ExitOrder.OrderQuantity = 10000; // full position size 
	//	ExitOrder.OrderType = SCT_MARKET; 

	//	int quantity = sc.BuyExit(ExitOrder); 
	//	OrderCount++;
	//}


// 
// 	sc.MaximumPositionAllowed += 1;
// 
// 	int &RememberedOrderID= sc.PersistVars->Integers[0];
// 	bool CancelOrder = false;
// 
// 	 // Create a s_SCNewOrder object.  
// 	s_SCNewOrder NewOrder; 
// 	NewOrder.OrderQuantity = 1;
// 	NewOrder.OrderType = SCT_MARKET; 
// 
// 
// 	int Result =sc.BuyEntry(NewOrder);
// 	if (Result > 0) //If there has been a successful order entry
// 	{
// 		//Remember the order ID for subsequent order modification or cancellation.
// 		RememberedOrderID = NewOrder.InternalOrderID;
// 
// 		//Put an arrow on the chart to indicate the order entry
// 		BuyEntrySubgraph[sc.Index] = sc.Low[sc.Index];
// 	}
// 
// 	if(CancelOrder)
// 	{
// 		int Result = sc.CancelOrder(RememberedOrderID);
// 	}
}

/*==========================================================================*/
SCSFExport scsf_TradingExampleUsingReversals(SCStudyInterfaceRef sc)
{
	// Define references to the Subgraphs and Inputs for easy reference
	SCSubgraphRef BuyEntrySubgraph = sc.Subgraph[0];
	
	SCSubgraphRef SellEntrySubgraph = sc.Subgraph[2];
	

	SCSubgraphRef SimpMovAvgSubgraph = sc.Subgraph[4];

	SCInputRef Enabled = sc.Input[0];


	if (sc.SetDefaults)
	{
		// Set the study configuration and defaults.

		sc.GraphName = "Trading Example Using Reversals";

		BuyEntrySubgraph.Name = "Buy Entry";
		BuyEntrySubgraph.DrawStyle = DRAWSTYLE_ARROWUP;
		BuyEntrySubgraph.PrimaryColor = RGB(0, 255, 0);
		BuyEntrySubgraph.LineWidth = 2;
		BuyEntrySubgraph.DrawZeros = false;

		SellEntrySubgraph.Name = "Sell Entry";
		SellEntrySubgraph.DrawStyle = DRAWSTYLE_ARROWDOWN;
		SellEntrySubgraph.PrimaryColor = RGB(255, 0, 0);
		SellEntrySubgraph.LineWidth = 2;
		SellEntrySubgraph.DrawZeros = false;

		SimpMovAvgSubgraph.Name = "Simple Moving Average";
		SimpMovAvgSubgraph.DrawStyle = DRAWSTYLE_LINE;
		SimpMovAvgSubgraph.DrawZeros = false;

		Enabled.Name = "Enabled";
		Enabled.SetYesNo(0); 


		sc.StudyDescription = "This study function is an example of how to use the ACSIL Trading Functions and demonstrates using reversals. This study will do nothing until the Enabled Input is set to Yes.";

		sc.AllowMultipleEntriesInSameDirection = false; 

		//This must be equal to or greater than the order quantities you will be  submitting orders for.
		sc.MaximumPositionAllowed = 2;

		sc.SupportReversals = true;

		// This is false by default. Orders will go to the simulation system always.
		sc.SendOrdersToTradeService = false;

		sc.AllowOppositeEntryWithOpposingPositionOrOrders = true;
		sc.SupportAttachedOrdersForTrading = false;
		sc.CancelAllOrdersOnEntriesAndReversals= true;
		sc.AllowEntryWithWorkingOrders = false;
		sc.CancelAllWorkingOrdersOnExit = true;
		sc.AllowOnlyOneTradePerBar = true; 

		//This needs to be set to true when a trading study uses trading functions.
		sc.MaintainTradeStatisticsAndTradesData = true;

		sc.AutoLoop = 1;
		sc.GraphRegion = 0;

		// During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;

		return;
	}


	if (!Enabled.GetYesNo())
		return;

	SCFloatArrayRef BarLastArray = sc.Close;

	// Calculate the moving average
	sc.SimpleMovAvg(BarLastArray, SimpMovAvgSubgraph, sc.Index, 10);


	// Create an s_SCNewOrder object. 
	s_SCNewOrder NewOrder;
	NewOrder.OrderQuantity = 1;
	NewOrder.OrderType = SCT_MARKET;

	// Buy when the last price of the current bar crosses the moving average from below. This will reverse the position if a short position exists.
	if (sc.CrossOver(BarLastArray, SimpMovAvgSubgraph) == CROSS_FROM_BOTTOM && sc.GetBarHasClosedStatus() == BHCS_BAR_HAS_CLOSED)
	{
		int Result = sc.BuyEntry(NewOrder);
		if (Result > 0) //If there has been a successful order entry, then draw an arrow at the low of the bar.
			BuyEntrySubgraph[sc.Index] = sc.Low[sc.Index];
	}

	// Sell when the last price of the current bar crosses the moving average from above. This will reverse the position if a long position exists.
	else if (sc.CrossOver(BarLastArray, SimpMovAvgSubgraph) == CROSS_FROM_TOP && sc.GetBarHasClosedStatus() == BHCS_BAR_HAS_CLOSED)
	{
		int Result = sc.SellEntry(NewOrder);
		if (Result > 0) //If there has been a successful order entry, then draw an arrow at the high of the bar.
			SellEntrySubgraph[sc.Index] = sc.High[sc.Index];
	}
}

/*==========================================================================*/

SCSFExport scsf_TradingExampleWithAttachedOrdersUsingActualPrices (SCStudyInterfaceRef sc)
{
	// Define references to the Subgraphs and Inputs for easy reference
	SCSubgraphRef BuyEntrySubgraph = sc.Subgraph[0];
	SCSubgraphRef SellEntrySubgraph = sc.Subgraph[2];
	SCSubgraphRef SimpMovAvgSubgraph = sc.Subgraph[4];
	SCInputRef Enabled = sc.Input[0];


	if (sc.SetDefaults)
	{
		// Set the study configuration and defaults.

		sc.GraphName = "Trading Example With Attached Orders Using Actual Prices";

		BuyEntrySubgraph.Name = "Buy Entry";
		BuyEntrySubgraph.DrawStyle = DRAWSTYLE_ARROWUP;
		BuyEntrySubgraph.PrimaryColor = RGB(0, 255, 0);
		BuyEntrySubgraph.LineWidth = 2;
		BuyEntrySubgraph.DrawZeros = false;


		SellEntrySubgraph.Name = "Sell Entry";
		SellEntrySubgraph.DrawStyle = DRAWSTYLE_ARROWDOWN;
		SellEntrySubgraph.PrimaryColor = RGB(255, 0, 0);
		SellEntrySubgraph.LineWidth = 2;
		SellEntrySubgraph.DrawZeros = false;


		SimpMovAvgSubgraph.Name = "Simple Moving Average";
		SimpMovAvgSubgraph.DrawStyle = DRAWSTYLE_LINE;
		SimpMovAvgSubgraph.DrawZeros = false;

		Enabled.Name = "Enabled";
		Enabled.SetYesNo(0); 

		sc.StudyDescription = "This study function is an example of how to use the ACSIL Trading Functions.  This example uses Attached Orders defined directly within this function. This function will display a simple moving average and perform a Buy Entry when the Last price crosses the moving average from below and a Sell Entry when the Last price crosses the moving average from above. A new entry cannot occur until the Target or Stop has been filled. When an order is sent, a corresponding arrow will appear on the chart to show that an order was sent. This study will do nothing until the Enabled Input is set to Yes.";

		sc.AllowMultipleEntriesInSameDirection = false; 

		//This must be equal to or greater than the order quantities you will be submitting orders for.
		sc.MaximumPositionAllowed = 5;

		sc.SupportReversals = false;

		// This is false by default. Orders will go to the simulation system always.
		sc.SendOrdersToTradeService = false;

		sc.AllowOppositeEntryWithOpposingPositionOrOrders = false;

		// This can be false in this function because we specify Attached Orders directly with the order which causes this to be considered true when submitting an order.
		sc.SupportAttachedOrdersForTrading = false;  

		sc.CancelAllOrdersOnEntriesAndReversals= true;
		sc.AllowEntryWithWorkingOrders = false;
		sc.CancelAllWorkingOrdersOnExit = true;

		// Only 1 trade for each Order Action type is allowed per bar.
		sc.AllowOnlyOneTradePerBar = true; 

		//This needs to be set to true when a trading study uses trading functions.
		sc.MaintainTradeStatisticsAndTradesData = true;

		sc.AutoLoop = 1;
		sc.GraphRegion = 0;

		// During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;

		return;
	}

	if (!Enabled.GetYesNo())
		return;


	SCFloatArrayRef ClosePriceArray = sc.Close;

	// Use persistent variables to remember attached order IDs so they can be modified or canceled. 
	int& Target1OrderID = sc.PersistVars->i1;
	int& Stop1OrderID = sc.PersistVars->i2;

	// Calculate the moving average
	sc.SimpleMovAvg(ClosePriceArray, SimpMovAvgSubgraph, sc.Index, 10);


	// Create an s_SCNewOrder object. 
	s_SCNewOrder NewOrder;
	NewOrder.OrderQuantity = 1;
	NewOrder.OrderType = SCT_MARKET;

	NewOrder.OCOGroup1Quantity = 1; // If this is left at the default of 0, then it will be automatically set.

	// Buy when the last price crosses the moving average from below.
	if (sc.CrossOver(ClosePriceArray, SimpMovAvgSubgraph) == CROSS_FROM_BOTTOM && sc.GetBarHasClosedStatus() == BHCS_BAR_HAS_CLOSED)
	{
		NewOrder.Target1Price =sc.Close[sc.Index] + 8*sc.TickSize ;
		NewOrder.Stop1Price = sc.Close[sc.Index]- 8*sc.TickSize;

		int Result = sc.BuyEntry(NewOrder);
		if (Result > 0) //If there has been a successful order entry, then draw an arrow at the low of the bar.
		{
			BuyEntrySubgraph[sc.Index] = sc.Low[sc.Index];

			// Remember the order IDs for subsequent modification and cancellation
			Target1OrderID = NewOrder.Target1InternalOrderID;
			Stop1OrderID = NewOrder.Stop1InternalOrderID;
		}
	}


	// Sell when the last price crosses the moving average from above.
	else if (sc.CrossOver(ClosePriceArray, SimpMovAvgSubgraph) == CROSS_FROM_TOP && sc.GetBarHasClosedStatus() == BHCS_BAR_HAS_CLOSED)
	{

		NewOrder.Target1Price =sc.Close[sc.Index] -  8*sc.TickSize;
		NewOrder.Stop1Price =sc.Close[sc.Index] + 8*sc.TickSize;

		int Result = sc.SellEntry(NewOrder);
		if (Result > 0) //If there has been a successful order entry, then draw an arrow at the high of the bar.
		{
			SellEntrySubgraph[sc.Index] = sc.High[sc.Index];

			// Remember the order IDs for subsequent modification and cancellation
			Target1OrderID = NewOrder.Target1InternalOrderID;
			Stop1OrderID = NewOrder.Stop1InternalOrderID;
		}
	}


	// This code block serves as an example of how to modify an attached order.  In this example it is set to false and never runs.
	bool ExecuteModifyOrder = false;
	if (ExecuteModifyOrder && (sc.Index == sc.ArraySize - 1))//Only  do a modify on the most recent bar
	{
		double NewLimit = 0.0;

		// Get the existing target order
		s_SCTradeOrder ExistingOrder;
		if (sc.GetOrderByOrderID(Target1OrderID, ExistingOrder) != SCTRADING_ORDER_ERROR)
		{
			if (ExistingOrder.BuySell == BSE_BUY)
				NewLimit = sc.Close[sc.Index] - 5*sc.TickSize;
			else if (ExistingOrder.BuySell == BSE_SELL)
				NewLimit = sc.Close[sc.Index] + 5*sc.TickSize;

			// We can only modify price and/or quantity

			s_SCNewOrder ModifyOrder;
			ModifyOrder.InternalOrderID = Target1OrderID;
			ModifyOrder.Price1 = NewLimit;

			sc.ModifyOrder(ModifyOrder);
		}
	}
}



/*==========================================================================*/
SCSFExport scsf_TradingExampleWithStopAllAttachedOrdersDirectlyDefined(SCStudyInterfaceRef sc)
{
	// Define references to the Subgraphs and Inputs for easy reference
	SCSubgraphRef BuyEntrySubgraph = sc.Subgraph[0];
	SCSubgraphRef SellEntrySubgraph = sc.Subgraph[2];
	SCSubgraphRef SimpMovAvgSubgraph = sc.Subgraph[4];
	SCInputRef Enabled = sc.Input[0];


	if (sc.SetDefaults)
	{
		// Set the study configuration and defaults.

		sc.GraphName = "Trading Example With Hardcoded Attached Orders (uses Stop All)";

		BuyEntrySubgraph.Name = "Buy Entry";
		BuyEntrySubgraph.DrawStyle = DRAWSTYLE_ARROWUP;
		BuyEntrySubgraph.PrimaryColor = RGB(0, 255, 0);
		BuyEntrySubgraph.LineWidth = 2;
		BuyEntrySubgraph.DrawZeros = false;


		SellEntrySubgraph.Name = "Sell Entry";
		SellEntrySubgraph.DrawStyle = DRAWSTYLE_ARROWDOWN;
		SellEntrySubgraph.PrimaryColor = RGB(255, 0, 0);
		SellEntrySubgraph.LineWidth = 2;
		SellEntrySubgraph.DrawZeros = false;


		SimpMovAvgSubgraph.Name = "Simple Moving Average";
		SimpMovAvgSubgraph.DrawStyle = DRAWSTYLE_LINE;
		SimpMovAvgSubgraph.DrawZeros = false;

		Enabled.Name = "Enabled";
		Enabled.SetYesNo(0); 

		sc.StudyDescription = "This study function is an example of how to use the ACSIL Trading Functions.  This example uses Attached Orders defined directly within this function. This function will display a simple moving average and perform a Buy Entry when the Last price crosses the moving average from below and a Sell Entry when the Last price crosses the moving average from above. A new entry cannot occur until the Target or Stop has been hit. When an order is sent, a corresponding arrow will appear on the chart to show that an order was sent. This study will do nothing until the Enabled Input is set to Yes.";

		sc.AllowMultipleEntriesInSameDirection = false; 
		sc.MaximumPositionAllowed = 5;
		sc.SupportReversals = false;

		// This is false by default. Orders will go to the simulation system always.
		sc.SendOrdersToTradeService = false;

		sc.AllowOppositeEntryWithOpposingPositionOrOrders = false;

		sc.SupportAttachedOrdersForTrading = false;  // This can be false in this function because we specify Attached Orders directly with the order which causes this to be considered true when submitting an order.

		sc.CancelAllOrdersOnEntriesAndReversals= true;
		sc.AllowEntryWithWorkingOrders = false;
		sc.CancelAllWorkingOrdersOnExit = true;

		// Only 1 trade for each Order Action type is allowed per bar.
		sc.AllowOnlyOneTradePerBar = true; 

		//This needs to be set to true when a trading study uses trading functions.
		sc.MaintainTradeStatisticsAndTradesData = true;

		sc.AutoLoop = 1;
		sc.GraphRegion = 0;

		// During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;

		return;
	}

	if (!Enabled.GetYesNo())
		return;


	SCFloatArrayRef Last = sc.Close;

	// Use persistent variables to remember attached order IDs so they can be modified or canceled. 
	int& Target1OrderID = sc.PersistVars->Integers[0];
	int& Target2OrderID = sc.PersistVars->Integers[1];
	int& StopAllOrderID = sc.PersistVars->Integers[2];

	int & ExecuteModifyOrder = sc.PersistVars->Integers[3];

	// Calculate the moving average
	sc.SimpleMovAvg(Last, SimpMovAvgSubgraph, sc.Index, 10);


	// Create an s_SCNewOrder object. 
	s_SCNewOrder NewOrder;
	NewOrder.OrderQuantity = 2;
	NewOrder.OrderType = SCT_MARKET;

	//Specify Targets and Stops
	NewOrder.Target1Offset = 8*sc.TickSize;
	NewOrder.Target2Offset = 12*sc.TickSize;
	NewOrder.StopAllOffset = 5*sc.TickSize;
	
	
	// Buy when the last price crosses the moving average from below.
	if (sc.CrossOver(Last, SimpMovAvgSubgraph) == CROSS_FROM_BOTTOM && sc.GetBarHasClosedStatus() == BHCS_BAR_HAS_CLOSED)
	{
		//NewOrder.Price1 = Last[sc.Index]+ 2*sc.TickSize;
		int Result = sc.BuyEntry(NewOrder);
		if (Result > 0) //If there has been a successful order entry, then draw an arrow at the low of the bar.
		{
			BuyEntrySubgraph[sc.Index] = sc.Low[sc.Index];
			ExecuteModifyOrder =1;

			// Remember the order IDs for subsequent modification and cancellation
			Target1OrderID = NewOrder.Target1InternalOrderID;
			Target2OrderID = NewOrder.Target2InternalOrderID;
			StopAllOrderID = NewOrder.StopAllInternalOrderID;
		}


	}


	// Sell when the last price crosses the moving average from above.
	else if (sc.CrossOver(Last, SimpMovAvgSubgraph) == CROSS_FROM_TOP && sc.GetBarHasClosedStatus() == BHCS_BAR_HAS_CLOSED)
	{
		//NewOrder.Price1 = Last[sc.Index] - 2*sc.TickSize;
		int Result = sc.SellEntry(NewOrder);
		if (Result > 0) //If there has been a successful order entry, then draw an arrow at the high of the bar.
		{
			SellEntrySubgraph[sc.Index] = sc.High[sc.Index];
			ExecuteModifyOrder =1;

			// Remember the order IDs for subsequent modification and cancellation
			Target1OrderID = NewOrder.Target1InternalOrderID;
			Target2OrderID = NewOrder.Target2InternalOrderID;
			StopAllOrderID = NewOrder.StopAllInternalOrderID;
		}



	}


	// This code block serves as an example of how to modify an Attached Order.
	if (ExecuteModifyOrder == 1&& (sc.Index == sc.ArraySize - 1))//Only  do a modify on the most recent bar
	{
		ExecuteModifyOrder = 0;

		double NewPrice = 0.0;

		// Get the existing target order
		s_SCTradeOrder ExistingOrder;
		if (sc.GetOrderByOrderID(StopAllOrderID, ExistingOrder) != SCTRADING_ORDER_ERROR)
		{
			if (IsWorkingOrderStatus(ExistingOrder.OrderStatusCode))
			{
				if (ExistingOrder.BuySell == BSE_BUY)
					NewPrice = ExistingOrder.Price1 + 10*sc.TickSize;
				else if (ExistingOrder.BuySell == BSE_SELL)
					NewPrice = ExistingOrder.Price1 - 10*sc.TickSize;

				// We can only modify price and/or quantity

				s_SCNewOrder ModifyOrder;
				ModifyOrder.InternalOrderID = StopAllOrderID;
				ModifyOrder.Price1 = NewPrice;

				sc.ModifyOrder(ModifyOrder);
			}
		}
	}
}

/*==========================================================================*/


SCSFExport scsf_ACSILTradingOCOExample(SCStudyInterfaceRef sc)
{
	//Define references to the Subgraphs and Inputs for easy reference

	SCInputRef Enabled = sc.Input[0];


	if (sc.SetDefaults)
	{
		// Set the study configuration and defaults.

		sc.GraphName = "Trading OCO Order Example";

		Enabled.Name = "Enabled";
		Enabled.SetYesNo(0); 
		Enabled.  SetDescription("This input enables the study and allows it to function. Otherwise, it does nothing.");


		// This is false by default. Orders will go to the simulation system always.
		sc.SendOrdersToTradeService = false;
		sc.AllowMultipleEntriesInSameDirection = false;
		sc.MaximumPositionAllowed = 10;
		sc.SupportReversals = false;
		sc.AllowOppositeEntryWithOpposingPositionOrOrders = false;
		sc.SupportAttachedOrdersForTrading = false;
		sc.CancelAllOrdersOnEntriesAndReversals= false;
		sc.AllowEntryWithWorkingOrders = false;
		sc.CancelAllWorkingOrdersOnExit = true;
		sc.AllowOnlyOneTradePerBar = false; 

		//This needs to be set to true when a trading study uses trading functions.
		sc.MaintainTradeStatisticsAndTradesData = true;

		sc.AutoLoop = false;
		sc.GraphRegion = 0;

		// During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;

		return;
	}

	//These must be outside of the sc.SetDefaults code block
	sc.SupportTradingScaleIn = 0;
	sc.SupportTradingScaleOut = 0;


	if (!Enabled.GetYesNo())
		return;


	// Do not run on a full calculation.
	if (sc.UpdateStartIndex == 0)
		return;

	float Last = sc.BaseDataIn[SC_LAST][sc.Index];

	s_SCPositionData PositionData;
	sc.GetTradePosition(PositionData);

	if(PositionData.PositionQuantity != 0 || PositionData.WorkingOrdersExist)
		return;

	s_SCNewOrder NewOrder;
	NewOrder.OrderQuantity = 4;
	NewOrder.OrderType = SCT_ORDERTYPE_OCO_BUY_STOP_SELL_STOP;

	NewOrder.Price1 = Last + 10 * sc.TickSize;
	NewOrder.Price2 = Last - 10 * sc.TickSize;

	// Optional: Add target and stop Attached Orders to each stop in the OCO pair.
	NewOrder.AttachedOrderTarget1Type = SCT_ORDERTYPE_LIMIT;
	NewOrder.Target1Offset = 4 * sc.TickSize;
	NewOrder.AttachedOrderStop1Type = SCT_ORDERTYPE_STOP;
	NewOrder.Stop1Offset = 4 * sc.TickSize;

	// Optional: Use different Attached Orders for the 2nd order in the OCO pair.
	
	NewOrder.Target1Offset_2 = 8 * sc.TickSize;
	NewOrder.Stop1Offset_2 = 8 * sc.TickSize;

	if ( sc.SubmitOCOOrder(NewOrder) >0)
	{

		int BuyStopOrder1ID = NewOrder.InternalOrderID;
		int SellStopOrder2ID = NewOrder.InternalOrderID2;

	}
}
	

/*==========================================================================*/
SCSFExport scsf_TradingExample1WithAdvancedAttachedOrders(SCStudyInterfaceRef sc)
{
	// Define references to the Subgraphs and Inputs for easy reference
	SCSubgraphRef BuyEntrySubgraph = sc.Subgraph[0];
	SCSubgraphRef SellEntrySubgraph = sc.Subgraph[2];
	SCSubgraphRef SimpMovAvgSubgraph = sc.Subgraph[4];

	SCInputRef Enabled = sc.Input[0];


	if (sc.SetDefaults)
	{
		// Set the study configuration and defaults.

		sc.GraphName = "Trading Example 1 With Advanced Attached Orders";

		BuyEntrySubgraph.Name = "Buy Entry";
		BuyEntrySubgraph.DrawStyle = DRAWSTYLE_ARROWUP;
		BuyEntrySubgraph.PrimaryColor = RGB(0, 255, 0);
		BuyEntrySubgraph.LineWidth = 2;
		BuyEntrySubgraph.DrawZeros = false;

		SellEntrySubgraph.Name = "Sell Entry";
		SellEntrySubgraph.DrawStyle = DRAWSTYLE_ARROWDOWN;
		SellEntrySubgraph.PrimaryColor = RGB(255, 0, 0);
		SellEntrySubgraph.LineWidth = 2;
		SellEntrySubgraph.DrawZeros = false;

		SimpMovAvgSubgraph.Name = "Simple Moving Average";
		SimpMovAvgSubgraph.DrawStyle = DRAWSTYLE_LINE;
		SimpMovAvgSubgraph.PrimaryColor = RGB(255,255,0);
		SimpMovAvgSubgraph.LineWidth = 2;
		SimpMovAvgSubgraph.DrawZeros = false;

		Enabled.Name = "Enabled";
		Enabled.SetYesNo(0); 

		sc.StudyDescription = "This study function is an example of how to use the ACSIL Trading functions.  This example uses Attached Orders defined directly within this function. It demonstrates more advanced use of Attached Orders. A new entry cannot occur until the Target or Stop has been filled or canceled. When an order is sent, a corresponding arrow will appear on the chart to show that an order was sent. This study will do nothing until the 'Enabled' Input is set to Yes.";

		sc.AllowMultipleEntriesInSameDirection = false; 
		sc.MaximumPositionAllowed = 5;
		sc.SupportReversals = false;

		// This is false by default. Orders will go to the simulation system always.
		sc.SendOrdersToTradeService = false;

		sc.AllowOppositeEntryWithOpposingPositionOrOrders = false;

		// This can be false in this function because we specify Attached Orders directly with the order which causes this to be considered true when submitting an order.
		sc.SupportAttachedOrdersForTrading = false;  

		sc.CancelAllOrdersOnEntriesAndReversals= true;
		sc.AllowEntryWithWorkingOrders = false;
		sc.CancelAllWorkingOrdersOnExit = true;

		// Only 1 trade for each Order Action type is allowed per bar.
		sc.AllowOnlyOneTradePerBar = true; 

		//This needs to be set to true when a trading study uses trading functions.
		sc.MaintainTradeStatisticsAndTradesData = true;

		sc.AutoLoop = 1;
		sc.GraphRegion = 0;

		// During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;

		return;
	}

	if (!Enabled.GetYesNo())
		return;


	SCFloatArrayRef Last = sc.Close;


	// Calculate the moving average
	sc.SimpleMovAvg(Last, SimpMovAvgSubgraph, sc.Index, 10);


	// Create an s_SCNewOrder object. 
	s_SCNewOrder NewOrder;
	NewOrder.OrderQuantity = 2;
	NewOrder.OrderType = SCT_ORDERTYPE_MARKET;

	//Define the Attached Orders to be attached to the main Market order
	//Target 1
	NewOrder.Target1Offset = 4*sc.TickSize;
	NewOrder.AttachedOrderTarget1Type = SCT_ORDERTYPE_LIMIT;

	//Target 2
	NewOrder.Target2Offset = 8*sc.TickSize;
	NewOrder.AttachedOrderTarget2Type = SCT_ORDERTYPE_LIMIT;

	// Common Step Trailing Stop
	NewOrder.StopAllOffset = 8*sc.TickSize;
	NewOrder.AttachedOrderStopAllType = SCT_ORDERTYPE_STEP_TRAILING_STOP_LIMIT;
	NewOrder.TrailStopStepPriceAmount = 4 * sc.TickSize;

	// Common Trailing Stop. Comment the section above and uncomment this section to use a simple trailing stop.
	//NewOrder.StopAllOffset = 8*sc.TickSize;
	//NewOrder.AttachedOrderStopAllType = SCT_ORDERTYPE_TRAILING_STOP;
	
	// Buy when the last price crosses the moving average from below.
	if (sc.CrossOver(Last, SimpMovAvgSubgraph) == CROSS_FROM_BOTTOM && sc.GetBarHasClosedStatus() == BHCS_BAR_HAS_CLOSED)
	{
		int Result = sc.BuyEntry(NewOrder);
		if (Result > 0) //If there has been a successful order entry, then draw an arrow at the low of the bar.
		{
			BuyEntrySubgraph[sc.Index] = sc.Low[sc.Index];
		}
	}


	// Sell when the last price crosses the moving average from above.
	else if (sc.CrossOver(Last, SimpMovAvgSubgraph) == CROSS_FROM_TOP && sc.GetBarHasClosedStatus() == BHCS_BAR_HAS_CLOSED)
	{
		int Result = sc.SellEntry(NewOrder);
		if (Result > 0) //If there has been a successful order entry, then draw an arrow at the high of the bar.
		{
			SellEntrySubgraph[sc.Index] = sc.High[sc.Index];

		}
	}
}


/*==========================================================================*/
SCSFExport scsf_TradingExample2WithAdvancedAttachedOrders(SCStudyInterfaceRef sc)
{
	// Define references to the Subgraphs and Inputs for easy reference
	SCSubgraphRef BuyEntrySubgraph = sc.Subgraph[0];
	SCSubgraphRef SellEntrySubgraph = sc.Subgraph[2];
	SCSubgraphRef SimpMovAvgSubgraph = sc.Subgraph[4];

	SCInputRef Enabled = sc.Input[0];


	if (sc.SetDefaults)
	{
		// Set the study configuration and defaults.

		sc.GraphName = "Trading Example 2 With Advanced Attached Orders";

		BuyEntrySubgraph.Name = "Buy Entry";
		BuyEntrySubgraph.DrawStyle = DRAWSTYLE_ARROWUP;
		BuyEntrySubgraph.PrimaryColor = RGB(0, 255, 0);
		BuyEntrySubgraph.LineWidth = 2;
		BuyEntrySubgraph.DrawZeros = false;

		SellEntrySubgraph.Name = "Sell Entry";
		SellEntrySubgraph.DrawStyle = DRAWSTYLE_ARROWDOWN;
		SellEntrySubgraph.PrimaryColor = RGB(255, 0, 0);
		SellEntrySubgraph.LineWidth = 2;
		SellEntrySubgraph.DrawZeros = false;

		SimpMovAvgSubgraph.Name = "Simple Moving Average";
		SimpMovAvgSubgraph.DrawStyle = DRAWSTYLE_LINE;
		SimpMovAvgSubgraph.PrimaryColor = RGB(255,255,0);
		SimpMovAvgSubgraph.LineWidth = 2;
		SimpMovAvgSubgraph.DrawZeros = false;

		Enabled.Name = "Enabled";
		Enabled.SetYesNo(0); 

		sc.StudyDescription = "This study function is an example of how to use the ACSIL Trading functions.  This example uses Attached Orders defined directly within this function. It demonstrates more advanced use of Attached Orders. A new entry cannot occur until the Targets and Stops have been filled or canceled. When an order is sent, a corresponding arrow will appear on the chart to show that an order was sent. This study will do nothing until the Enabled Input is set to Yes.";

		sc.AllowMultipleEntriesInSameDirection = false; 
		sc.MaximumPositionAllowed = 5;
		sc.SupportReversals = false;

		// This is false by default. Orders will go to the simulation system always.
		sc.SendOrdersToTradeService = false;

		sc.AllowOppositeEntryWithOpposingPositionOrOrders = false;

		// This can be false in this function because we specify Attached Orders directly with the order which causes this to be considered true when submitting an order.
		sc.SupportAttachedOrdersForTrading = false;  

		sc.CancelAllOrdersOnEntriesAndReversals= true;
		sc.AllowEntryWithWorkingOrders = false;
		sc.CancelAllWorkingOrdersOnExit = true;

		// Only 1 trade for each Order Action type is allowed per bar.
		sc.AllowOnlyOneTradePerBar = true; 

		//This needs to be set to true when a trading study uses trading functions.
		sc.MaintainTradeStatisticsAndTradesData = true;

		sc.AutoLoop = 1;
		sc.GraphRegion = 0;

		// During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;

		return;
	}

	if (!Enabled.GetYesNo())
		return;


	SCFloatArrayRef Last = sc.Close;


	// Calculate the moving average
	sc.SimpleMovAvg(Last, SimpMovAvgSubgraph, sc.Index, 10);


	// Create an s_SCNewOrder object. 
	s_SCNewOrder NewOrder;
	NewOrder.OrderQuantity = 2;
	NewOrder.OrderType = SCT_ORDERTYPE_MARKET;

	//Define the Attached Orders to be attached to the main Market order
	//Target 1
	NewOrder.Target1Offset = 4*sc.TickSize;
	NewOrder.AttachedOrderTarget1Type = SCT_ORDERTYPE_LIMIT;

	//Target 2
	NewOrder.Target2Offset = 12*sc.TickSize;
	NewOrder.AttachedOrderTarget2Type = SCT_ORDERTYPE_LIMIT;

	//Common Stop
	NewOrder.StopAllOffset = 8*sc.TickSize;
	NewOrder.AttachedOrderStopAllType = SCT_ORDERTYPE_TRIGGERED_TRAILING_STOP_LIMIT_3_OFFSETS;
	NewOrder.TriggeredTrailStopTriggerPriceOffset=8*sc.TickSize;
	NewOrder.TriggeredTrailStopTrailPriceOffset=4*sc.TickSize;

	//Set up a move to breakeven action for the common stop

	NewOrder.MoveToBreakEven.Type=MOVETO_BE_ACTION_TYPE_OCO_GROUP_TRIGGERED;
	NewOrder.MoveToBreakEven.BreakEvenLevelOffsetInTicks= 1;
	//When Target 1 is filled, the order will be moved to breakeven +1
	NewOrder.MoveToBreakEven.TriggerOCOGroup= OCO_GROUP_1;


	// Buy when the last price crosses the moving average from below.
	if (sc.CrossOver(Last, SimpMovAvgSubgraph) == CROSS_FROM_BOTTOM && sc.GetBarHasClosedStatus() == BHCS_BAR_HAS_CLOSED)
	{
		int Result = sc.BuyEntry(NewOrder);
		if (Result > 0) //If there has been a successful order entry, then draw an arrow at the low of the bar.
		{
			BuyEntrySubgraph[sc.Index] = sc.Low[sc.Index];
		}
	}


	// Sell when the last price crosses the moving average from above.
	else if (sc.CrossOver(Last, SimpMovAvgSubgraph) == CROSS_FROM_TOP && sc.GetBarHasClosedStatus() == BHCS_BAR_HAS_CLOSED)
	{
		int Result = sc.SellEntry(NewOrder);
		if (Result > 0) //If there has been a successful order entry, then draw an arrow at the high of the bar.
		{
			SellEntrySubgraph[sc.Index] = sc.High[sc.Index];

		}
	}
}

/*==========================================================================*/

SCSFExport scsf_SC_TradingCrossOverExample(SCStudyGraphRef sc) 
{

	SCInputRef Line1Ref = sc.Input[0];
	SCInputRef Line2Ref = sc.Input[1];

	SCSubgraphRef BuyEntry = sc.Subgraph[0];
	SCSubgraphRef SellEntry = sc.Subgraph[1];

	if (sc.SetDefaults)
	{

		// Set the configuration and defaults

		sc.GraphName = "Trading CrossOver Example";

		sc.StudyDescription = "An example of a trading system that enters a new position or \
							  reverses an existing one on the crossover of two study Subgraphs. \
							  When line1 crosses line2 from below, the system will look to be long. \
							  When line1 crosses line2 from above, the system will look to be short.";

		sc.AutoLoop = 1;  // true
		sc.GraphRegion = 0;
		sc.FreeDLL = 0;
		sc.CalculationPrecedence = LOW_PREC_LEVEL;

		Line1Ref.Name = "Line1";
		Line1Ref.SetStudySubgraphValues(1, 0);

		Line2Ref.Name = "Line2";
		Line2Ref.SetStudySubgraphValues(1, 0);

		BuyEntry.Name = "Buy";
		BuyEntry.DrawStyle = DRAWSTYLE_POINTHIGH;
		BuyEntry.LineWidth = 3;

		SellEntry.Name = "Sell";
		SellEntry.DrawStyle = DRAWSTYLE_POINTLOW;
		SellEntry.LineWidth = 3;

		sc.AllowMultipleEntriesInSameDirection = false; 
		sc.MaximumPositionAllowed = 5;
		sc.SupportReversals = true;

		// This is false by default. Orders will not go to the external connected trading service.
		sc.SendOrdersToTradeService = false;

		sc.AllowOppositeEntryWithOpposingPositionOrOrders = false;

		// This can be false in this function because we specify Attached Orders directly with the order which causes this to be considered true when submitting an order.
		sc.SupportAttachedOrdersForTrading = false;  

		sc.CancelAllOrdersOnEntriesAndReversals= true;
		sc.AllowEntryWithWorkingOrders = false;
		sc.CancelAllWorkingOrdersOnExit = true;

		// Only 1 trade for each Order Action type is allowed per bar.
		sc.AllowOnlyOneTradePerBar = true; 

		//This needs to be set to true when a trading study uses trading functions.
		sc.MaintainTradeStatisticsAndTradesData = true;


		return;
	}

	// only process at the close of the bar; if it has not closed don't do anything
	if (sc.GetBarHasClosedStatus() == BHCS_BAR_HAS_NOT_CLOSED) 
	{
		return;
	}


	// using the line1Ref and line2Ref input variables, retrieve the subgraph arrays into line1, line2 arrays respectively
	SCFloatArray line1;
	sc.GetStudyArrayUsingID(Line1Ref.GetStudyID(), Line1Ref.GetSubgraphIndex(), line1);

	SCFloatArray line2;
	sc.GetStudyArrayUsingID(Line2Ref.GetStudyID(), Line2Ref.GetSubgraphIndex(), line2);

	// code below is where we check for crossovers and take action accordingly

	if (sc.CrossOver(line1, line2) == CROSS_FROM_BOTTOM) 
	{

		// mark the crossover on the chart
		BuyEntry[sc.Index] = 1;

		// Create a market order and enter long.
		s_SCNewOrder order;
		order.OrderQuantity = 1;
		order.OrderType = SCT_ORDERTYPE_MARKET;

		sc.BuyEntry(order);
	}

	if (sc.CrossOver(line1, line2) == CROSS_FROM_TOP)
	{


		// mark the crossover on the chart
		SellEntry[sc.Index] = 1;

		// create a market order and enter short
		s_SCNewOrder order;
		order.OrderQuantity = 1;
		order.OrderType = SCT_ORDERTYPE_MARKET;

		sc.SellEntry(order);
	}
}


/*==========================================================================*/

SCSFExport scsf_TradingOrderEntryExamples(SCStudyInterfaceRef sc)
{
	// Subgraph references
	// None

	// Input references
	SCInputRef Enabled = sc.Input[0];


	if (sc.SetDefaults)
	{
		// Set the study configuration and defaults.

		sc.GraphName = "Trading Order Entry Examples";

		Enabled.Name = "Enabled";
		Enabled.SetYesNo(0); 

		sc.StudyDescription = "This study function is an example of submitting various types of orders. It does not actually submit any orders, it only contains code examples for submitting  orders.";

		sc.AllowMultipleEntriesInSameDirection = false; 
		sc.MaximumPositionAllowed = 10;//Quantity 10
		sc.SupportReversals = false;

		// This is false by default. Orders will go to the simulation system always.
		sc.SendOrdersToTradeService = false;

		sc.AllowOppositeEntryWithOpposingPositionOrOrders = false;
		sc.SupportAttachedOrdersForTrading = false;
		sc.CancelAllOrdersOnEntriesAndReversals= true;
		sc.AllowEntryWithWorkingOrders = false;
		sc.CancelAllWorkingOrdersOnExit = false;

		// Only 1 trade for each Order Action type is allowed per bar.
		sc.AllowOnlyOneTradePerBar = true; 

		//This needs to be set to true when a trading study uses trading functions.
		sc.MaintainTradeStatisticsAndTradesData = true;

		sc.AutoLoop = 1;
		sc.GraphRegion = 0;

		// During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;

		return;
	}


	if (!Enabled.GetYesNo())
		return;

	if (0)
	{
		//Order entry examples
		{


			//Move to breakeven on Stop Attached order

			// Create an s_SCNewOrder object. 
			s_SCNewOrder NewOrder;
			NewOrder.OrderQuantity = 2;
			NewOrder.OrderType = SCT_ORDERTYPE_MARKET;

			//Define the Attached Orders to be attached to the main Market order
			//Target 1
			NewOrder.Target1Offset = 10*sc.TickSize;
			NewOrder.AttachedOrderTarget1Type = SCT_ORDERTYPE_LIMIT;

			//Common Stop
			NewOrder.StopAllOffset = 10*sc.TickSize;
			NewOrder.AttachedOrderStopAllType = SCT_ORDERTYPE_STOP;


			// Set up a move to breakeven action for the common stop. 

			// This is a common setting and applies to all Stop Attached Orders set on the main order.
			NewOrder.MoveToBreakEven.Type=MOVETO_BE_ACTION_TYPE_OFFSET_TRIGGERED;

			//After 5 ticks of profit, the stop order will be moved to breakeven
			NewOrder.MoveToBreakEven.TriggerOffsetInTicks= 5;
			NewOrder.MoveToBreakEven.BreakEvenLevelOffsetInTicks= 0; 

			sc.BuyOrder(NewOrder);

		}

		{

			// Trailing stop order. This has no Attached Orders. 
			// The trailing offset is the difference between the current market price for the Symbol of the chart the trading study is applied to
			// and the price set by NewOrder.Price1

			// Create an s_SCNewOrder object. 
			s_SCNewOrder NewOrder;
			NewOrder.OrderQuantity = 2;
			NewOrder.OrderType = SCT_ORDERTYPE_TRAILING_STOP; 
			NewOrder.Price1 = sc.BaseData[SC_LAST][sc.Index] - 10*sc.TickSize;
			sc.SellOrder(NewOrder);
		}

		{

			// Trailing Stop Attached Order
			// The trailing offset is set by NewOrder.Stop1Offset 

			// Create an s_SCNewOrder object. 
			s_SCNewOrder NewOrder;
			NewOrder.OrderQuantity = 2;
			NewOrder.OrderType = SCT_ORDERTYPE_MARKET;
			NewOrder.AttachedOrderStop1Type = SCT_ORDERTYPE_TRAILING_STOP;
			NewOrder.Stop1Offset = 10 * sc.TickSize;
			sc.BuyOrder(NewOrder);
		}
	}

}


/*==========================================================================*/
SCSFExport scsf_TradingExampleMACrossOverWithAttachedOrders(SCStudyInterfaceRef sc)
{
	// Define references to the Subgraphs and Inputs for easy reference
	SCSubgraphRef BuyEntrySubgraph = sc.Subgraph[0];
	SCSubgraphRef SellEntrySubgraph = sc.Subgraph[1];
	SCSubgraphRef FastSimpMovAvgSubgraph = sc.Subgraph[4];
	SCSubgraphRef SlowSimpMovAvgSubgraph = sc.Subgraph[5];

	SCInputRef Enabled = sc.Input[0];


	if (sc.SetDefaults)
	{
		// Set the study configuration and defaults.

		sc.GraphName = "Trading Example: Moving Average Crossover with Attached Orders";

		BuyEntrySubgraph.Name = "Buy Entry";
		BuyEntrySubgraph.DrawStyle = DRAWSTYLE_ARROWUP;
		BuyEntrySubgraph.PrimaryColor = RGB(0, 255, 0);
		BuyEntrySubgraph.LineWidth = 2;
		BuyEntrySubgraph.DrawZeros = false;

		SellEntrySubgraph.Name = "Sell Entry";
		SellEntrySubgraph.DrawStyle = DRAWSTYLE_ARROWDOWN;
		SellEntrySubgraph.PrimaryColor = RGB(255, 0, 0);
		SellEntrySubgraph.LineWidth = 2;
		SellEntrySubgraph.DrawZeros = false;

		FastSimpMovAvgSubgraph.Name = "Fast Moving Average";
		FastSimpMovAvgSubgraph.DrawStyle = DRAWSTYLE_LINE;
		FastSimpMovAvgSubgraph.PrimaryColor = RGB(255, 255, 255);
		FastSimpMovAvgSubgraph.DrawZeros = false;
		FastSimpMovAvgSubgraph.LineWidth = 2;

		SlowSimpMovAvgSubgraph.Name = "Slow Moving Average";
		SlowSimpMovAvgSubgraph.DrawStyle = DRAWSTYLE_LINE;
		SlowSimpMovAvgSubgraph.PrimaryColor = RGB(0, 255, 0);
		SlowSimpMovAvgSubgraph.DrawZeros = false;
		SlowSimpMovAvgSubgraph.LineWidth = 2;

		Enabled.Name = "Enabled";
		Enabled.SetYesNo(0); 


		sc.AllowMultipleEntriesInSameDirection = false; 
		sc.MaximumPositionAllowed = 2;
		sc.SupportReversals = false;

		// This is false by default. Orders will go to the simulation system always.
		sc.SendOrdersToTradeService = false;

		sc.AllowOppositeEntryWithOpposingPositionOrOrders = false;

		// This can be false in this function because we specify Attached Orders directly with the order which causes this to be considered true when submitting an order.
		sc.SupportAttachedOrdersForTrading = false;  

		sc.CancelAllOrdersOnEntriesAndReversals= false;
		sc.AllowEntryWithWorkingOrders = false;
		sc.CancelAllWorkingOrdersOnExit = true;

		// Only 1 trade for each Order Action type is allowed per bar.
		sc.AllowOnlyOneTradePerBar = true; 

		//This needs to be set to true when a trading study uses trading functions.
		sc.MaintainTradeStatisticsAndTradesData = true;

		sc.AutoLoop = 1;
		sc.GraphRegion = 0;

		// During development set this flag to 1, so the DLL can be modified. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;

		return;
	}

	if (!Enabled.GetYesNo())
		return;
	

	// Use persistent variables to remember attached order IDs so they can be modified or canceled. 
	int& Target1OrderID = sc.PersistVars->i1;
	int& Stop1OrderID = sc.PersistVars->i2;

	// Calculate the moving average
	sc.SimpleMovAvg(sc.Close, FastSimpMovAvgSubgraph, 10);
	sc.SimpleMovAvg(sc.Close, SlowSimpMovAvgSubgraph, 20);

	s_SCPositionData PositionData;
	sc.GetTradePosition(PositionData);
	if(PositionData.PositionQuantity != 0)
		return;

	// Create an s_SCNewOrder object. 
	s_SCNewOrder NewOrder;
	NewOrder.OrderQuantity = 1;
	NewOrder.OrderType = SCT_MARKET;
	NewOrder.TimeInForce = SCT_TIF_GTC;

	//Specify a Target and a Stop with 8 tick offsets. We are specifying one Target and one Stop, additional targets and stops can be specified as well. 
	NewOrder.Target1Offset = 8*sc.TickSize;
	NewOrder.Stop1Offset = 8*sc.TickSize;
	NewOrder.OCOGroup1Quantity = 1; // If this is left at the default of 0, then it will be automatically set.

	// Buy when the last price crosses the moving average from below.
	if (sc.CrossOver(FastSimpMovAvgSubgraph, SlowSimpMovAvgSubgraph) == CROSS_FROM_BOTTOM && sc.GetBarHasClosedStatus() == BHCS_BAR_HAS_CLOSED)
	{
		int Result = sc.BuyEntry(NewOrder);
		if (Result > 0) //If there has been a successful order entry, then draw an arrow at the low of the bar.
		{
			BuyEntrySubgraph[sc.Index] = sc.Low[sc.Index];

			// Remember the order IDs for subsequent modification and cancellation
			Target1OrderID = NewOrder.Target1InternalOrderID;
			Stop1OrderID = NewOrder.Stop1InternalOrderID;
		}
	}


	// Sell when the last price crosses the moving average from above.
	else if (sc.CrossOver(FastSimpMovAvgSubgraph, SlowSimpMovAvgSubgraph) == CROSS_FROM_TOP && sc.GetBarHasClosedStatus() == BHCS_BAR_HAS_CLOSED)
	{
		int Result = sc.SellEntry(NewOrder);
		if (Result > 0) //If there has been a successful order entry, then draw an arrow at the high of the bar.
		{
			SellEntrySubgraph[sc.Index] = sc.High[sc.Index];

			// Remember the order IDs for subsequent modification and cancellation
			Target1OrderID = NewOrder.Target1InternalOrderID;
			Stop1OrderID = NewOrder.Stop1InternalOrderID;
		}
	}


}

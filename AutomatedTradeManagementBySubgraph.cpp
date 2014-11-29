#include "sierrachart.h"


SCSFExport scsf_trade_management_based_on_subgraph(SCStudyGraphRef sc)
{

	// Setup References

	SCInputRef Enabled = sc.Input[0];
	SCInputRef OrderType = sc.Input[1];
	SCInputRef ControlSubgraphRef = sc.Input[2];
	SCInputRef AdjustmentTiming = sc.Input[3]; 
	SCInputRef AdjustmentFrequency = sc.Input[4];
	SCInputRef OrderOffset = sc.Input[5];
	SCInputRef ToolId = sc.Input[6];
	SCInputRef PositionType = sc.Input[7];
	SCInputRef Logging = sc.Input[8];

	if (sc.SetDefaults)	
	{
		sc.GraphName = "Trade Management by Study";
		sc.AutoLoop = 0;//We are using Manual looping for efficiency since historical data is not relevant with this study
		sc.GraphRegion = 0;
		sc.FreeDLL =0;
		sc.CalculationPrecedence = LOW_PREC_LEVEL;
		
		Enabled.Name = "Enabled";
		Enabled.SetYesNo(0);

		OrderType.Name = "Order Type To Manage";
		OrderType.SetCustomInputStrings("Stops;Targets");
		OrderType.SetCustomInputIndex(0);

		ControlSubgraphRef.Name = "Controlling Subgraph Reference";
		ControlSubgraphRef.SetStudySubgraphValues(0, 0);

		AdjustmentTiming.Name = "Order Adjustment Timing";
		AdjustmentTiming.SetCustomInputStrings("Every N Seconds;On Bar Close");
		AdjustmentTiming.SetCustomInputIndex(1);

		AdjustmentFrequency.Name = "Adjustment Frequency (in Seconds)";
		AdjustmentFrequency.SetInt(30);

		OrderOffset.Name = "Trailing Offset (in Ticks)";
		OrderOffset.SetInt(2);

		ToolId.Name = "ACS Tool # ToolBar button for Enable/Disable";
		ToolId.SetInt(1);
		ToolId.SetIntLimits(1,50);

		PositionType.  Name = "Position Type";
		PositionType.SetCustomInputStrings("All Positions;Long Only;Short Only");
		PositionType.SetCustomInputIndex(0);

		Logging.Name = "Detailed Logging (for debugging)";
		Logging.SetYesNo(0);

		// Do not receive mouse pointer events when ACS Tool ToolBar button is pressed in/enabled
		sc.ReceivePointerEvents = false;

		return;
	}



	//References to persistent variables
	SCDateTime& LastAdjustmentDateTime = sc.PersistVars->scdt1;

	int& MenuID = sc.PersistVars->i1;

	double & LastModifyPrice = sc.PersistVars->d1;

	int & LastLogMessageIdentifier =sc.PersistVars->i2;

	/****************************************
	* First, handle mouse events
	*****************************************/

	if (sc.UpdateStartIndex == 0)
	{
		// set ACS Tool toolbar tool tip
		sc.SetACSToolToolTip(ToolId.GetInt(), "Enable/Disable Trade Management by Study");

		// set ACS Tool toolbar button text
		sc.SetACSToolButtonText(ToolId.GetInt(), "TMS");

		sc.SetACSToolEnable(ToolId.GetInt(), Enabled.GetBoolean() != 0 );

		LastModifyPrice = 0.0;

		LastLogMessageIdentifier = 0;
	}

	// Wait for an event to enable study if not already enabled
	if (sc.MenuEventID == ToolId.GetInt())
	{
		int state = (sc.PointerEventType == SC_ACS_TOOL_ON) ? 1 : 0;
		Enabled.SetYesNo(state);
	}

	/****************************************
	* next, execute the order management
	*****************************************/

	// If study is not enabled, exit without doing anything
	if (Enabled.GetYesNo() == 0)
	{
		if (Logging.GetYesNo() && LastLogMessageIdentifier != 1)
		{
			LastLogMessageIdentifier = 1;
			sc.AddMessageToLog("Study not enabled.", 0);
		}

		return;
	}
	

	//For safety we must never do any order management while historical data is being downloaded.
	if (sc.ChartIsDownloadingHistoricalData(sc.ChartNumber))
	{
		if (Logging.GetYesNo() && LastLogMessageIdentifier != 2)
		{
			LastLogMessageIdentifier = 2;
			sc.AddMessageToLog("No order adjustments performed because historical data is being downloaded for chart.", 0);
		}
		return;
	}

	sc.SendOrdersToTradeService = !sc.GlobalTradeSimulationIsOn;

	// Flat state is a good time to reset LastAdjustmentDateTime
	s_SCPositionData PositionData;
	sc.GetTradePosition (PositionData);

	if (PositionData.PositionQuantity == 0)
	{
		LastAdjustmentDateTime = 0.0;
		if (Logging.GetYesNo() && LastLogMessageIdentifier !=  3)
		{
			LastLogMessageIdentifier = 3;
			sc.AddMessageToLog("Position is flat.", 0);
		}
		return;
	}

	if (PositionType.GetIndex()== 1 && PositionData.PositionQuantity <0 )
	{
		if (Logging.GetYesNo() && LastLogMessageIdentifier != 4)
		{
			LastLogMessageIdentifier = 4;
			sc.AddMessageToLog("Position Type is set to Long Only and position is Short.", 0);
		}

		return;
	}

	if (PositionType.GetIndex()== 2 && PositionData.PositionQuantity >0 )
	{
		if (Logging.GetYesNo() && LastLogMessageIdentifier != 5)
		{
			LastLogMessageIdentifier = 5;
			sc.AddMessageToLog("Position Type is set to Short Only and position is Long.", 0);
		}

		return;
	}

	// If the timing is every N seconds, check that we have surpassed the set frequency
	if (AdjustmentTiming.GetInt() == 0 && (sc.LatestDateTimeForLastBar - LastAdjustmentDateTime) < AdjustmentFrequency.GetFloat() * SECONDS)
	{
		if (Logging.GetYesNo() && LastLogMessageIdentifier != 6)
		{
			LastLogMessageIdentifier = 6;
			sc.AddMessageToLog("Time interval has not elapsed yet for 'Every N Seconds'.", 0);
		}
		return;		
	}


	//  Update the LastAdjustmentDateTime

	LastAdjustmentDateTime = sc.LatestDateTimeForLastBar;

	// Retrieve the controlling subgraph array
	SCFloatArray ControllingSubgraph;
	sc.GetStudyArrayUsingID(ControlSubgraphRef.GetStudyID(), ControlSubgraphRef.GetSubgraphIndex(), ControllingSubgraph);

	// Based on user settings, retrieve nearest stop/target order
	s_SCTradeOrder Order;
	int Result;

	if (OrderType.IntValue == 0)
		Result = sc.GetNearestStopOrder(Order);
	else
		Result = sc.GetNearestTargetOrder(Order);

	if( Result == 0)
	{
		if (Logging.GetYesNo() && LastLogMessageIdentifier != 7)
		{
			LastLogMessageIdentifier = 7;
			sc.AddMessageToLog("There is no Stop/Target order found.", 0);
		}
		return;
	}
	
	// only process open orders
	if ( Order.OrderStatusCode != SCT_OSC_OPEN)
	{	
		if (Logging.GetYesNo() && LastLogMessageIdentifier != 8)
		{
			LastLogMessageIdentifier = 8;
			sc.AddMessageToLog("The found Stop/Target order is not in an Open state.", 0);
		}
		return;
	}
	
	// Modify the order
	int BarIndex = sc.ArraySize - 1;
	
	if(AdjustmentTiming.GetIndex() == 1 )
		BarIndex = sc.ArraySize - 2;

	if ( ControllingSubgraph[BarIndex]== 0.0)//0.0 not considered valid
	{	
		if (Logging.GetYesNo() && LastLogMessageIdentifier != 9)
		{
			LastLogMessageIdentifier = 9;
			sc.AddMessageToLog("The Controlling Subgraph Reference is 0 at the last bar.", 0);
		}
		return;
	}

	double NewPrice;
	
	if (Order.BuySell == BSE_SELL)
	{
		NewPrice = ControllingSubgraph[BarIndex] - OrderOffset.GetInt() * sc.TickSize;
	}
	else
	{
		NewPrice = ControllingSubgraph[BarIndex] + OrderOffset.GetInt() * sc.TickSize;
	}

	//Round the price to an actual order price.
	NewPrice= sc.RoundToTickSize(NewPrice);

	//If the price has not changed, then do not modify.
	if(NewPrice == LastModifyPrice ||  sc.FormattedEvaluateUsingDoubles(Order.Price1,sc.ValueFormat, EQUAL_OPERATOR,NewPrice, sc.ValueFormat ) )
	{	
		if (Logging.GetYesNo() && LastLogMessageIdentifier != 10)
		{
			LastLogMessageIdentifier = 10;
			sc.AddMessageToLog("There is not a new price to modify the order to.", 0);
		}
		return;
	}

	LastModifyPrice = NewPrice;
	
	s_SCNewOrder ModifyOrder;
	ModifyOrder.InternalOrderID = Order.InternalOrderID;
	ModifyOrder.Price1 = NewPrice;

	Result = sc.ModifyOrder(ModifyOrder);
	if (Logging.GetYesNo() && LastLogMessageIdentifier != 11)
	{
		LastLogMessageIdentifier = 11;
		sc.AddMessageToLog("The Stop/Target order has been modified.", 0);
	}

}



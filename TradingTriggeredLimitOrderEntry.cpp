#include "sierrachart.h"

// Defining all references as one define for  convenience
// This is so that each method/function does not have 
// to redefine these references

#define TRADING_TRIGGERED_LIMIT_ORDER_ENTRY_REFS(sc)\
	SCSubgraphRef TriggerLine = sc.Subgraph[0];\
	SCSubgraphRef OrderLine = sc.Subgraph[1];\
	SCSubgraphRef TextDisplay = sc.Subgraph[2];\
	SCInputRef TextHorizontalPosition = sc.Input[0];\
	SCInputRef TextVerticalPosition = sc.Input[1];\
	SCInputRef NumberOfBarsBack = sc.Input[3];\
	int& SetTriggerPriceMenuID = sc.PersistVars->i1;\
	int& SetSellOrderPriceMenuID = sc.PersistVars->i2;\
	int& SetBuyOrderPriceMenuID = sc.PersistVars->i3;\
	int& CancelOrderMenuID = sc.PersistVars->i4;\
	int& OrderType = sc.PersistVars->i5;\
	int& LastSubgraphUpdateIndex = sc.PersistVars->i6;\
	double& TriggerPrice = sc.PersistVars->d1;\
	double& OrderPrice = sc.PersistVars->d2;\

/*==========================================================================*/

void TradingTriggeredLimitOrderEntry_UpdateTriggerAndOrderPriceTextDisplay(SCStudyGraphRef sc)
{
	TRADING_TRIGGERED_LIMIT_ORDER_ENTRY_REFS(sc)

		s_UseTool Tool;

	long LineNumber = 79212343 + sc.StudyGraphInstanceID;

	Tool.Clear(); // Reset tool structure for our next use. Unnecessary in this case, but good practice.
	Tool.ChartNumber = sc.ChartNumber;
	Tool.DrawingType = DRAWING_TEXT;

	Tool.LineNumber = LineNumber;
	Tool.BeginDateTime = TextHorizontalPosition.GetInt();

	Tool.Region = sc.GraphRegion;
	Tool.BeginValue = (float)TextVerticalPosition.GetInt();
	Tool.UseRelativeValue = true;
	Tool.Color = TextDisplay.PrimaryColor;
	Tool.FontBackColor = TextDisplay.SecondaryColor;
	Tool.FontBold = true;

	SCString Text;

	Text.Format("Trigger Price: ");
	if (TriggerPrice != 0.0) {
		Text += sc.FormatGraphValue(TriggerPrice, sc.GetValueFormat());
	} 
	else
	{
		Text += "Not Set";
	}

	Text += "\n";

	Text += "Order Price: ";
	if (OrderPrice != 0.0) 
	{
		Text += sc.FormatGraphValue(OrderPrice, sc.GetValueFormat());
	}
	else
	{
		Text += "Not Set";
	}

	Tool.Text = Text;
	Tool.FontSize = TextDisplay.LineWidth;
	Tool.AddMethod = UTAM_ADD_OR_ADJUST;
	Tool.ReverseTextColor = false;

	sc.UseTool(Tool);
}

/*==========================================================================*/

void TradingTriggeredLimitOrderEntry_ResetAll(SCStudyGraphRef sc)
{

	TRADING_TRIGGERED_LIMIT_ORDER_ENTRY_REFS(sc)

		sc.RemoveACSChartShortcutMenuItem(sc.ChartNumber, SetBuyOrderPriceMenuID);
	sc.RemoveACSChartShortcutMenuItem(sc.ChartNumber, SetSellOrderPriceMenuID);
	sc.RemoveACSChartShortcutMenuItem(sc.ChartNumber, CancelOrderMenuID);

	SetSellOrderPriceMenuID = 0;
	SetBuyOrderPriceMenuID = 0;
	CancelOrderMenuID = 0;

	OrderPrice = 0.0;
	TriggerPrice = 0.0;
	OrderType = 0;

	for (int i = LastSubgraphUpdateIndex; i < sc.ArraySize; i++) 
	{
		TriggerLine[i] =0;
		OrderLine[i] = 0;
	}

	TradingTriggeredLimitOrderEntry_UpdateTriggerAndOrderPriceTextDisplay(sc);
}

/*==========================================================================*/

void TradingTriggeredLimitOrderEntry_HandleMenuEvents(SCStudyGraphRef sc) 
{

	TRADING_TRIGGERED_LIMIT_ORDER_ENTRY_REFS(sc)

		if (sc.MenuEventID != 0)	
		{
			if (!sc.ChartTradeModeEnabled)
			{
				sc.AddMessageToLog("Chart Trade Mode is not active. No action performed.", 1);
				return;
			}

		}

		// handle set trigger price menu clicks
		if (sc.MenuEventID != 0 && sc.MenuEventID == SetTriggerPriceMenuID)	
		{

			// Get price that the user selected.  This is already rounded to the nearest price tick
			TriggerPrice =sc.ChartTradingOrderPrice;

			// Add Sell/Buy Order Price menu items
			if (SetSellOrderPriceMenuID <= 0)
				SetSellOrderPriceMenuID = sc.AddACSChartShortcutMenuItem(sc.ChartNumber, "Set Sell Order Price");

			if (SetBuyOrderPriceMenuID <= 0)
				SetBuyOrderPriceMenuID = sc.AddACSChartShortcutMenuItem(sc.ChartNumber, "Set Buy Order Price");

			if (CancelOrderMenuID <=0)
				CancelOrderMenuID = sc.AddACSChartShortcutMenuItem(sc.ChartNumber, "Cancel Order");

		}

		// handle set SELL order menu clicks
		if (sc.MenuEventID != 0 && sc.MenuEventID == SetSellOrderPriceMenuID)
		{

			//Get the order price
			double NewOrderPrice = sc.ChartTradingOrderPrice;

			// order price must be greater than trigger price for sell orders; less than trigger price for buy orders
			if (NewOrderPrice < TriggerPrice) 
				sc.AddMessageToLog("Sell Order Price must be greater than the Trigger Price", 1);
			else
			{
				OrderPrice = NewOrderPrice;
				OrderType = -1;
			}
		}

		// handle set BUY order menu clicks
		if (sc.MenuEventID != 0 && sc.MenuEventID == SetBuyOrderPriceMenuID)	
		{

			//Get the order price
			double NewOrderPrice = sc.ChartTradingOrderPrice;

			// order price must be greater than trigger price for sell orders; less than trigger price for buy orders
			if (NewOrderPrice > TriggerPrice)
				sc.AddMessageToLog("Buy Order price must be less than the Trigger Price", 1);
			else
			{
				OrderPrice = NewOrderPrice;
				OrderType = 1;
			}
		}

		// handle cancel order menu clicks
		if (sc.MenuEventID != 0 && sc.MenuEventID == CancelOrderMenuID)
		{

			OrderType = 0;
			OrderPrice = 0.0;
			TriggerPrice= 0.0;

			sc.RemoveACSChartShortcutMenuItem(sc.ChartNumber, CancelOrderMenuID) 	;
			CancelOrderMenuID = 0;

			if (SetBuyOrderPriceMenuID)
			{
				sc.RemoveACSChartShortcutMenuItem(sc.ChartNumber, SetBuyOrderPriceMenuID);
				SetBuyOrderPriceMenuID = 0;
			}

			if (SetSellOrderPriceMenuID)
			{
				sc.RemoveACSChartShortcutMenuItem(sc.ChartNumber, SetSellOrderPriceMenuID);
				SetSellOrderPriceMenuID = 0;
			}

		}

		// if there was an event, update the on chart text display
		if (sc.MenuEventID != 0)
			TradingTriggeredLimitOrderEntry_UpdateTriggerAndOrderPriceTextDisplay(sc);
}

/*==========================================================================*/

void TradingTriggeredLimitOrderEntry_UpdateSubgraphs(SCStudyGraphRef sc) 
{

	TRADING_TRIGGERED_LIMIT_ORDER_ENTRY_REFS(sc)


		int StartingOutputIndex = sc.ArraySize - NumberOfBarsBack.GetInt();

	for (int i = LastSubgraphUpdateIndex; i < StartingOutputIndex; i++)
	{
		TriggerLine[i] = 0.0f;
		OrderLine[i] = 0.0f;
	}
	LastSubgraphUpdateIndex = StartingOutputIndex;



	for (int i = StartingOutputIndex; i < sc.ArraySize; i++)
	{
		TriggerLine[i] = (float) TriggerPrice;
		OrderLine[i] =  (float) OrderPrice;
	}


}

/*==========================================================================*/

SCSFExport scsf_TradingTriggeredLimitOrderEntry(SCStudyGraphRef sc)
{


	TRADING_TRIGGERED_LIMIT_ORDER_ENTRY_REFS(sc)

		if (sc.SetDefaults)
		{
			sc.GraphName = "Trading: Triggered Limit Order Entry";
			sc.StudyDescription = "This study provides an example of how to implement custom order management based on price action within Sierra Chart. The study uses the menu API to provide user interaction.";
			sc.AutoLoop = 0;//Manual looping
			sc.GraphRegion = 0;
			sc.FreeDLL = 0;
			sc.ScaleRangeType= SCALE_SAMEASREGION;

			TextDisplay.Name = "Text Display";
			TextDisplay.LineWidth = 12;
			TextDisplay.DrawStyle = DRAWSTYLE_CUSTOM_TEXT;
			TextDisplay.PrimaryColor = RGB(0, 0, 0); //black
			TextDisplay.SecondaryColor = RGB(128, 255, 255);
			TextDisplay.SecondaryColorUsed = true;
			TextDisplay.DisplayNameValueInWindowsFlags = 0;

			TextHorizontalPosition.Name = "Text - Horizontal Position From Left (1-150)";
			TextHorizontalPosition.SetInt(20);
			TextHorizontalPosition.SetIntLimits(1,150);

			TextVerticalPosition.Name = "Text - Vertical Position From Bottom (1-100)";
			TextVerticalPosition.SetInt(90);
			TextVerticalPosition.SetIntLimits(1,100);

			NumberOfBarsBack.Name = "Subgraphs - # of bars back to display";
			NumberOfBarsBack.SetInt(20);

			OrderLine.Name = "Order Line";
			OrderLine.PrimaryColor = COLOR_GREEN;
			OrderLine.LineLabel = LL_DISPLAY_VALUE | LL_VALUE_ALIGN_LEFT_EDGE | LL_VALUE_ALIGN_BELOW | LL_DISPLAY_NAME | LL_NAME_ALIGN_LEFT_EDGE | LL_NAME_ALIGN_ABOVE;
			OrderLine.DrawZeros = false;

			TriggerLine.Name = "Trigger Line";
			TriggerLine.PrimaryColor = COLOR_CYAN;
			TriggerLine.LineLabel = LL_DISPLAY_VALUE | LL_VALUE_ALIGN_LEFT_EDGE | LL_VALUE_ALIGN_BELOW | LL_DISPLAY_NAME | LL_NAME_ALIGN_LEFT_EDGE | LL_NAME_ALIGN_ABOVE;
			TriggerLine.DrawZeros = false;

			sc.ReceivePointerEvents = false;

			sc.AllowMultipleEntriesInSameDirection = 1;
			sc.AllowOppositeEntryWithOpposingPositionOrOrders = 1;
			sc.CancelAllOrdersOnEntriesAndReversals = 0;
			sc.AllowOnlyOneTradePerBar = 0;
			sc.SupportReversals = 0;
			sc.AllowEntryWithWorkingOrders = 1;
			sc.SupportAttachedOrdersForTrading = 0;
			sc.UseGUIAttachedOrderSetting = 1;

			return;
		}


		/*#########################################
		inits and teardowns
		##########################################*/

		if (sc.UpdateStartIndex == 0)
		{

			if (SetTriggerPriceMenuID <= 0)
			{
				SetTriggerPriceMenuID = sc.AddACSChartShortcutMenuItem(sc.ChartNumber, "Set Trigger Price");
			}

			LastSubgraphUpdateIndex = 0;
			TradingTriggeredLimitOrderEntry_UpdateTriggerAndOrderPriceTextDisplay(sc);
		}


		if (sc.LastCallToFunction)
		{
			// be sure to remove the menu command when study is removed
			sc.RemoveACSChartShortcutMenuItem(sc.ChartNumber, SetTriggerPriceMenuID);
			sc.RemoveACSChartShortcutMenuItem(sc.ChartNumber, SetBuyOrderPriceMenuID);
			sc.RemoveACSChartShortcutMenuItem(sc.ChartNumber, SetSellOrderPriceMenuID);
			sc.RemoveACSChartShortcutMenuItem(sc.ChartNumber, CancelOrderMenuID);
		}

		/*#########################################
		first, handle mouse events from the user
		##########################################*/

		TradingTriggeredLimitOrderEntry_HandleMenuEvents(sc);

		/*#########################################
		next, update the subgraphs
		##########################################*/

		TradingTriggeredLimitOrderEntry_UpdateSubgraphs(sc);

		/*#########################################
		finally, handle triggers and orders
		##########################################*/

		//For safety we must never do any order management while historical data is being downloaded.
		if (sc.ChartIsDownloadingHistoricalData(sc.ChartNumber))
			return;



		// If the order is zero, we have nothing to do
		if (OrderPrice == 0.0) 
		{
			return;
		}


		sc.SendOrdersToTradeService = !sc.GlobalTradeSimulationIsOn;

		// check for sell order trigger conditions
		if (OrderType == -1 &&
			sc.FormattedEvaluate(sc.BaseDataIn[SC_LOW][sc.ArraySize-1], sc.BaseGraphValueFormat, LESS_OPERATOR, (float) TriggerPrice, sc.BaseGraphValueFormat)) 
		{

			s_SCNewOrder Order;
			Order.Price1 = OrderPrice;
			Order.OrderType = SCT_LIMIT;
			Order.OrderQuantity = sc.TradeWindowOrderQuantity;

			int ret = sc.SellOrder(Order);
			if (ret > 0)
			{
				
				TradingTriggeredLimitOrderEntry_ResetAll(sc);
			}
		}

		// check for buy order trigger conditions
		if (OrderType == 1 &&
			sc.FormattedEvaluate(sc.BaseDataIn[SC_HIGH][sc.ArraySize-1], sc.BaseGraphValueFormat, GREATER_OPERATOR, (float) TriggerPrice, sc.BaseGraphValueFormat)) 
		{

			s_SCNewOrder Order;
			Order.Price1 = OrderPrice;
			Order.OrderType = SCT_LIMIT;
			Order.OrderQuantity = sc.TradeWindowOrderQuantity;

			int ret = sc.BuyOrder(Order);
			if (ret > 0)
			{
				
				TradingTriggeredLimitOrderEntry_ResetAll(sc);
			}
		}
} 

/*==========================================================================*/
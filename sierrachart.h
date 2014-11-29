// File Name: sierrachart.h

#ifndef _SIERRACHART_H_
#define _SIERRACHART_H_

// SC_DLL_VERSION gets updated only when there have been changes to the DLL
// study interface.  This should be set to the Sierra Chart version number
// that includes the changes made to the interface.
#define SC_DLL_VERSION 1200

#ifndef _CRT_SECURE_NO_WARNINGS
#define  _CRT_SECURE_NO_WARNINGS
#endif

#include <windows.h>  //Main Windows SDK header file
#include <stdio.h>  //This header is for input/output including File input/output
#include <stdarg.h>
#include <float.h>
#include <io.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <map>
#include <vector>
#include <algorithm>



#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif
/****************************************************************************/

#include "scconstants.h"
#include "scdatetime.h"
#include "scsymboldata.h"
#include "sccolors.h"
#include "VAPContainer.h"
#include "SCString.h"

struct s_sc;

typedef s_sc& SCStudyInterfaceRef;

const int CURRENT_SC_NEW_ORDER_VERSION = 12;
const int CURRENT_SC_ORDER_FILL_DATA_VERSION = 1;


#define SCDLLEXPORT extern "C" __declspec(dllexport)
#define SCDLLCALL __cdecl

#define SCSFExport SCDLLEXPORT void SCDLLCALL  // Use SCSFExport when defining a study function

#define SCDLLName(DLLName) \
	SCDLLEXPORT int SCDLLCALL scdll_DLLVersion() { return SC_DLL_VERSION; } \
	SCDLLEXPORT const char* SCDLLCALL scdll_DLLName() { return DLLName; }


#define SPECIAL_FEATURES "You need to be on a software service package that includes Additional Features to use this study."

#define UserDrawnLineExists UserDrawnChartDrawingExists
#define LineExists ChartDrawingExists

typedef void (SCDLLCALL* fp_ACSGDIFunction)(HWND WindowHandle, HDC DeviceContext, SCStudyInterfaceRef);

#pragma pack(push, 8)


/****************************************************************************/

struct s_TimeAndSales
{
	SCDateTimeMS DateTime;
	float				Price;
	unsigned int		Volume;
	float				Bid;
	float				Ask;
	unsigned int BidSize;
	unsigned int AskSize; 
	//unsigned char		Hour;
	//unsigned char		Minute;
	//unsigned char		Second;

	//This will always be a value >= 1.  It is unlikely to wrap around, but it could.  It will never be 0.
	unsigned int		Sequence;

	short				Type;
	//unsigned short		Date;
	unsigned int TotalBidDepth;
	unsigned int TotalAskDepth;
	
	s_TimeAndSales()
	{
		Clear();
	}

	explicit s_TimeAndSales(int i)
	{
		Clear();
	}
	
	s_TimeAndSales& operator *= (float Multiplier)
	{
		Price *= Multiplier;
		Bid *= Multiplier;
		Ask *= Multiplier;
		
		return *this;
	}

	void Clear() 
	{
		memset(this, 0, sizeof(s_TimeAndSales));
	}

};

class SCTimeAndSalesArray
{
	protected:
		const int m_Version;
		
		s_TimeAndSales* m_Data;
		int m_MaxSize;
		int m_NextIndex;
		int m_StartAtNext;  // boolean
		
	public:
		/*==================================================================*/
		SCTimeAndSalesArray()
			: m_Version(1)
			
			, m_Data(NULL)
			, m_MaxSize(0)
			, m_NextIndex(0)
			, m_StartAtNext(0)  // false
		{
		}
		
		/*==================================================================*/
		~SCTimeAndSalesArray()
		{
		}
		
		/*==================================================================*/
		//Thread safe as long as an instance of this object is only used by one thread at a time.
		int IsCreated() const
		{
			return (m_Data != NULL)?1:0;
		}
		
		/*==================================================================*/
		int MaxSize() const
		{
			return m_MaxSize;
		}
		
		/*==================================================================*/
		int Size() const
		{
			if (m_Data == NULL)
				return 0;
			
			if (m_StartAtNext)
				return m_MaxSize;  // Using the full buffer
			else
				return m_NextIndex;  // Using only up to m_NextIndex
		}
		
		/*====================================================================
			This function is for backwards compatibility.
		--------------------------------------------------------------------*/
		int GetArraySize() const
		{
			return Size();
		}
		
		/*====================================================================
			The oldest element is at index 0, and the newest element is at
			index Size()-1.
		--------------------------------------------------------------------*/
		const s_TimeAndSales& operator [] (int Index) const
		{
			static s_TimeAndSales DefaultElement;
			
			if (m_Data == NULL)
				return DefaultElement;
			
			if (Index < 0 || Index >= Size())
				return DefaultElement;
			
			// Translate the index
			if (m_StartAtNext)
			{
				Index += m_NextIndex;
				
				if (Index >= m_MaxSize)
					Index -= m_MaxSize;
			}
			
			return m_Data[Index];
		}
		
		/*====================================================================
			This does not copy the data, it just copies the pointer from the
			given Time and Sales array.
		--------------------------------------------------------------------*/
		SCTimeAndSalesArray& operator = (const SCTimeAndSalesArray& Rhs)
		{
			if (&Rhs == this)
				return *this;
			
			m_Data = Rhs.m_Data;
			m_MaxSize = Rhs.m_MaxSize;
			m_NextIndex = Rhs.m_NextIndex;
			m_StartAtNext = Rhs.m_StartAtNext;
			
			return *this;
		}

		bool IsRecordIndexTrade (int Index)
		{
			return (operator[]( Index). Type == SC_TS_ASK  || operator[](Index). Type  == SC_TS_BID );
		}

		SCDateTimeMS GetDateTimeOfLastTradeRecord()
		{
			int ArraySize = Size();
			if(ArraySize== 0)
				return 0.0;

			int RecordIndex=ArraySize-1;
			while(true)
			{
				if (RecordIndex <0)
					break;

				if (!IsRecordIndexTrade( RecordIndex) && RecordIndex >=0)
				{
					RecordIndex--;
					continue;
				}
				else
					break;
			}

			if (RecordIndex>= 0)
				return	operator[]( RecordIndex).DateTime;
			else
				return 0.0;


		}
};



/****************************************************************************/

class s_UseTool
{		
	private:
		unsigned int Size;
	public:
		int ChartNumber;
		int Tool;
		int LineNumber;
		int ExtendLeft;  
		int ExtendRight;  
		int TransparencyLevel;  
		int Unused3;  
		float BeginValue;
		float EndValue;
		int Region;
		COLORREF Color;
		char* Old_Text;
		int FontSize;
		int Unused4;
		float RetracementLevels[SC_DRAWING_MAX_LEVELS];
		SCDateTime BeginDateTime;
		SCDateTime EndDateTime;
		SCString Text;
		
		DrawingTypeEnum DrawingType;
		unsigned short LineWidth;
		unsigned short LineStyle;  // Same line styles as subgraphs use
		
		int AddMethod;  // UseToolAddMethodEnum

	private:
		unsigned int Version;
	public:
		int ReverseTextColor;

		unsigned int FontBackColor;
		SCString FontFace;
		int FontBold;
		int FontItalic;
		int TextAlignment;
		int UseRelativeValue;

		COLORREF SecondaryColor; // For Highlight tools
		int SquareEdge;

		int DisplayHorizontalLineValue;

// Added in 779
		SCDateTime  ThirdDateTime;
		float       ThirdValue;

		int		BeginIndex;
		int		EndIndex;
		int		ThirdIndex;

		COLORREF LevelColor[SC_DRAWING_MAX_LEVELS];
		int      LevelStyle[SC_DRAWING_MAX_LEVELS];
		int      LevelWidth[SC_DRAWING_MAX_LEVELS];

		int ShowTickDifference;		// calc
		int ShowCurrencyValue;		// calc
		int ShowPriceDifference;	// calc
		int ShowPercentChange;		// calc
		int ShowTimeDifference;		// calc
		int ShowNumberOfBars;		// calc
		int ShowAngle;				// calc
		int ShowEndPointPrice;		// calc
		int MultiLineLabel;			// calc

 		int ShowEndPointDateTime;	// vert line, calc
 		int ShowEndPointDate;		// vert line, calc
 		int ShowEndPointTime;		// vert line, calc

		int ShowPercent;			// retrace
		int ShowPrice;				// retrace
		int RoundToTickSize;		// retrace

		int   FixedSizeArrowHead;
		float ArrowHeadSize;

		int MarkerType;
		int MarkerSize;
		int UseBarSpacingForMarkerSize;

		int TimeExpVerticals;
		int TimeExpTopLabel1;
		int TimeExpTopLabel2;
		int TimeExpBottomLabel1;
		int TimeExpBottomLabel2;
		int TimeExpBasedOnTime;

		int TransparentLabelBackground;

		int AddAsUserDrawnDrawing;
// End 779 Adds

		int ShowVolume;				// calc
		int DrawFromEnd;
		int DrawUnderneathMainGraph;

		// 1087
		int ShowLabels;
		int ShowLabelsAtEnd;
		int AllLevelsSameColorStyle;

		// 1108
		int UseToolCfgNum;

		// Note: When adding new members, remember to update CopyAndUpdateUseToolStruct function.

		s_UseTool()
		{
			Clear();
		}
		
		void Clear()
		{
			Text.Initialize();

			Size = sizeof(s_UseTool);

			Version = SC_DLL_VERSION;
			Tool = TOOL_UNDEFINED;
			AddMethod = UTAM_ADD_OR_ADJUST;
			BeginValue = FLT_MIN;
			EndValue = FLT_MIN;
			ThirdValue = FLT_MIN;

			ExtendLeft = -1;  
			ExtendRight = -1;  

			Region = -1;
			FontSize = -1;
			ReverseTextColor = -1;
			Color = 0xff000000;
			LineWidth = 0xffff;
			LineStyle = 0xffff;
			FontBackColor = 0xff000000;
			FontFace.Initialize();
			FontBold = -1;
			FontItalic = -1;
			TextAlignment = -1;
			UseRelativeValue = false;
			SecondaryColor = 0xff000000;

			DisplayHorizontalLineValue = -1;
			TransparencyLevel = -1;

			for (int i=0; i<SC_DRAWING_MAX_LEVELS; i++)
			{
				RetracementLevels[i] = -FLT_MAX;
				LevelColor[i] = 0xff000000;
				LevelStyle[i] = -1;
				LevelWidth[i] = -1;
			}

			ShowTickDifference = -1;
			ShowCurrencyValue = -1;
			ShowPriceDifference = -1;
			ShowPercentChange = -1;
			ShowTimeDifference = -1;
			ShowNumberOfBars = -1;
			ShowAngle = -1;
			ShowVolume = -1;
			ShowEndPointPrice = -1;
			DrawFromEnd = -1;
			MultiLineLabel = -1;
			DrawUnderneathMainGraph = -1;

			ShowLabels = -1;
			ShowLabelsAtEnd = -1;
			AllLevelsSameColorStyle = -1;

			ShowEndPointDateTime = -1;
			ShowEndPointDate = -1;
			ShowEndPointTime = -1;

			ShowPercent = -1;
			ShowPrice = -1;
			RoundToTickSize = -1;

			FixedSizeArrowHead = -1;
			ArrowHeadSize = -FLT_MAX;

			MarkerType = -1;
			MarkerSize = -1;
			UseBarSpacingForMarkerSize = -1;

			TimeExpVerticals = -1;
			TimeExpTopLabel1 = -1;
			TimeExpTopLabel2 = -1;
			TimeExpBottomLabel1 = -1;
			TimeExpBottomLabel2 = -1;
			TimeExpBasedOnTime = -1;

			TransparentLabelBackground = -1;

			BeginIndex = -1;
			EndIndex = -1;
			ThirdIndex = -1;

			AddAsUserDrawnDrawing = 0;

			UseToolCfgNum = -1;

			LineNumber = -1;

			//set to 0
			ChartNumber = 0;

			Unused3 = 0;  

			Old_Text = NULL;

			Unused4 = 0;
			BeginDateTime = 0;
			EndDateTime = 0;

			DrawingType = DRAWING_UNKNOWN;

			SquareEdge=0;
			ThirdDateTime = 0;

		}
		
		unsigned int GetSize() const
		{
			return Size;
		}
		unsigned int GetVersion() const
		{
			return Version;
		}
};


/****************************************************************************/
struct s_ACSTradeStatistics
{
	double  ClosedProfitLoss() { return ClosedProfit + ClosedLoss; }
	double  ClosedProfit;
	double  ClosedLoss;
	double  ProfitFactor() { return ClosedLoss == 0 ? 0 : fabs(ClosedProfit / ClosedLoss); }
	double  TotalCommission;

	double  MaximumRunup;
	double  MaximumDrawdown;

	double  MaximumTradeRunup;
	double  MaximumTradeDrawdown;
	double  MaximumOpenPositionProfit;
	double  MaximumOpenPositionLoss;

	int		TotalTrades() { return TotalWinningTrades + TotalLosingTrades; }
	int		TotalWinningTrades;
	int		TotalLosingTrades;
	double  TotalPercentProfitable() { return TotalTrades() == 0 ? 0 : (double)TotalWinningTrades / (double)TotalTrades(); }
	int		TotalLongTrades;
	int		TotalShortTrades;

	int		TotalClosedQuantity() { return TotalWinningQuantity + TotalLosingQuantity; }
	int		TotalWinningQuantity;
	int		TotalLosingQuantity;
	int     TotalFilledQuantity;

	int		LargestTradeQuantity;

	double  AvgQuantityPerTrade() { return TotalTrades() == 0 ? 0 : (double)TotalClosedQuantity() / (double)TotalTrades(); }
	double  AvgQuantityPerWinningTrade() { return TotalWinningTrades == 0 ? 0 : (double)TotalWinningQuantity / (double)TotalWinningTrades; }
	double  AvgQuantityPerLosingTrade() { return TotalLosingTrades == 0 ? 0 : (double)TotalLosingQuantity / (double)TotalLosingTrades; }

	double  AvgProfitLoss() { return TotalTrades() == 0 ? 0 : ClosedProfitLoss() / TotalTrades(); }
	double  AvgProfit() { return TotalWinningTrades == 0 ? 0 : ClosedProfit / TotalWinningTrades; }
	double  AvgLoss() { return TotalLosingTrades == 0 ? 0 : ClosedLoss / TotalLosingTrades; }
	double  AvgProfitFactor() { return AvgLoss() == 0 ? 0 : fabs(AvgProfit() / AvgLoss()); }

	double  LargestWinningTrade;
	double  LargestLosingTrade;
	double  LargestWinnerPercentOfProfit() { return ClosedProfit == 0 ? 0 : LargestWinningTrade / ClosedProfit; }
	double  LargestLoserPercentOfLoss() { return ClosedLoss == 0 ? 0 : fabs(LargestLosingTrade / ClosedLoss); }

	int		TimeInTrades() { return TimeInWinningTrades + TimeInLosingTrades; }
	int		TimeInWinningTrades;
	int		TimeInLosingTrades;

	int     AvgTimeInTrade() { return TotalTrades() == 0 ? 0 : TimeInTrades() / TotalTrades(); }
	int     AvgTimeInWinningTrade() { return TotalWinningTrades == 0 ? 0 : TimeInWinningTrades / TotalWinningTrades; }
	int     AvgTimeInLosingTrade() { return TotalLosingTrades == 0 ? 0 : TimeInLosingTrades / TotalLosingTrades; }
	
	int		MaxConsecutiveWinners;
	int		MaxConsecutiveLosers;

	s_ACSTradeStatistics()
	{
		Clear();
	}

	void Clear()
	{
		ClosedProfit = 0;
		ClosedLoss = 0;
		TotalCommission = 0;
		MaximumRunup = 0;
		MaximumDrawdown = 0;
		MaximumTradeRunup = 0;
		MaximumTradeDrawdown = 0;
		MaximumOpenPositionProfit = 0;
		MaximumOpenPositionLoss = 0;
		TotalWinningTrades = 0;
		TotalLosingTrades = 0;
		TotalLongTrades = 0;
		TotalShortTrades = 0;
		TotalWinningQuantity = 0;
		TotalLosingQuantity = 0;
		TotalFilledQuantity = 0;
		LargestTradeQuantity = 0;
		LargestWinningTrade = 0;
		LargestLosingTrade = 0;
		TimeInWinningTrades = 0;
		TimeInLosingTrades = 0;
		MaxConsecutiveWinners = 0;
		MaxConsecutiveLosers = 0;
	}
};


/****************************************************************************/
struct s_ACSTrade
{
	SCDateTime	OpenDateTime;
	SCDateTime	CloseDateTime;
	int			TradeType;			// +1=long, -1=short
	int			EntryQuantity;
	int			ExitQuantity;
	int			MaxOpenQuantity;
	double		AverageEntryPrice;
	double		AverageExitPrice;
	double		OpenProfitLoss;
	double		ClosedProfitLoss;
	double		MaximumOpenPositionLoss;
	double		MaximumOpenPositionProfit;
	double		MaximumRunup;
	double		MaximumDrawdown;
	double		Commission;

	s_ACSTrade()
	{
		Clear();
	}

	void Clear()
	{
		OpenDateTime = 0;
		CloseDateTime = 0;
		TradeType = 0;
		EntryQuantity = 0;
		ExitQuantity = 0;
		MaxOpenQuantity = 0;
		AverageEntryPrice = 0;
		AverageExitPrice = 0;
		OpenProfitLoss = 0;
		ClosedProfitLoss = 0;
		MaximumOpenPositionLoss = 0;
		MaximumOpenPositionProfit = 0;
		MaximumRunup = 0;
		MaximumDrawdown = 0;
		Commission = 0;
	}
};

/****************************************************************************/
struct s_SCOrderFillData
{
	int				Version;

	SCString		Symbol;
	SCString		TradeAccount;
	int				InternalOrderID;

	SCDateTime		FillDateTime;	// adjusted to chart time zone
	BuySellEnum		BuySell;
	unsigned int	Quantity;
	double			AverageFillPrice;

	s_SCOrderFillData()
	{
		Clear();
	}

	void Clear()
	{
		memset(this, 0, sizeof(s_SCOrderFillData));
		Version = CURRENT_SC_ORDER_FILL_DATA_VERSION;
	}
};

/****************************************************************************/
struct s_SCNewOrder
{
	int Version;
	
	// Version 1 is no longer supported
	
	// Version 2
	unsigned int OrderQuantity;
	int OrderType; //SCOrderTypeEnum 
	double Unused_LimitPrice;
	double Unused_StopPrice;
	int InternalOrderID;

	// Version 3
	SCString TextTag;

	// Version 4
	double Target1Offset;
	int Target1InternalOrderID;
	double Stop1Offset;
	int Stop1InternalOrderID;
	unsigned int OCOGroup1Quantity;
	
	double Target2Offset;
	int Target2InternalOrderID;
	double Stop2Offset;
	int Stop2InternalOrderID;
	unsigned int OCOGroup2Quantity;
	
	double Target3Offset;
	int Target3InternalOrderID;
	double Stop3Offset;
	int Stop3InternalOrderID;
	unsigned int OCOGroup3Quantity;
	
	// Version 5
	SCTimeInForceEnum TimeInForce;

	// Version 6
	double Target4Offset;
	int Target4InternalOrderID;
	double Stop4Offset;
	int Stop4InternalOrderID;
	unsigned int OCOGroup4Quantity;
	
	// Version 7
	double StopAllOffset;
	int StopAllInternalOrderID;

	// Version 8
	double Target1Price;
	double Stop1Price;
	double Target2Price;
	double Stop2Price;
	double Target3Price;
	double Stop3Price;
	double Target4Price;
	double Stop4Price;
	double StopAllPrice;
	
	// Version 9
	double Price1;
	double Price2;
	double MaximumChaseAsPrice;
	int InternalOrderID2;
	
	
	//Version 10
	double Target5Offset;
	double Target5Price;
	int Target5InternalOrderID;
	double Stop5Offset;
	double Stop5Price;
	int Stop5InternalOrderID;
	unsigned int OCOGroup5Quantity;


	char AttachedOrderTarget1Type;
	char AttachedOrderTarget2Type;
	char AttachedOrderTarget3Type;
	char AttachedOrderTarget4Type;
	char AttachedOrderTarget5Type;


	char AttachedOrderStop1Type;
	char AttachedOrderStop2Type;
	char AttachedOrderStop3Type;
	char AttachedOrderStop4Type;
	char AttachedOrderStop5Type;
	char AttachedOrderStopAllType;

	double AttachedOrderMaximumChase;
	double TrailStopStepPriceAmount;
	double TriggeredTrailStopTriggerPriceOffset;
	double TriggeredTrailStopTrailPriceOffset;

	
	struct s_MoveToBreakEven
	{
		int Type;
		int BreakEvenLevelOffsetInTicks;
		int TriggerOffsetInTicks;
		int TriggerOCOGroup;
	};
	//The move to breakeven settings will apply to all stop attached orders specified
	 s_MoveToBreakEven MoveToBreakEven;


	 int Target1InternalOrderID_2;
	 int Stop1InternalOrderID_2;
	 int Target2InternalOrderID_2;
	 int Stop2InternalOrderID_2;
	 int Target3InternalOrderID_2;
	 int Stop3InternalOrderID_2;
	 int Target4InternalOrderID_2;
	 int Stop4InternalOrderID_2;
	 int Target5InternalOrderID_2;
	 int Stop5InternalOrderID_2;
	 int StopAllInternalOrderID_2;
	 double  StopLimitOrderLimitOffset;

	 //Version 12
	 double Target1Offset_2;
	 double Target1Price_2;
	 double Stop1Offset_2;
	 double Stop1Price_2;

	 double Target2Offset_2;
	 double Target2Price_2;
	 double Stop2Offset_2;
	 double Stop2Price_2;

	 double Target3Offset_2;
	 double Target3Price_2;
	 double Stop3Offset_2;
	 double Stop3Price_2;

	 double Target4Offset_2;
	 double Target4Price_2;
	 double Stop4Offset_2;
	 double Stop4Price_2;

	 double Target5Offset_2;
	 double Target5Price_2;
	 double Stop5Offset_2;
	 double Stop5Price_2;

	 double StopAllOffset_2;
	 double StopAllPrice_2;

	s_SCNewOrder()		
	{
		Reset();
	}

	void Reset()
	{
		memset(this,0,sizeof(s_SCNewOrder));
		Version = CURRENT_SC_NEW_ORDER_VERSION;
		OrderType = SCT_MARKET;
		Unused_LimitPrice = DBL_MAX;
		Unused_StopPrice = DBL_MAX;

		
		TimeInForce = SCT_TIF_DAY;
		
		Price1 = DBL_MAX;
		Price2 = DBL_MAX;

		/* Can be one of the following: 
		SCT_ORDERTYPE_LIMIT, 
		SCT_ORDERTYPE_LIMIT_CHASE, 
		SCT_ORDERTYPE_LIMIT_TOUCH_CHASE
		*/
		AttachedOrderTarget1Type = SCT_ORDERTYPE_LIMIT;
		AttachedOrderTarget2Type  = SCT_ORDERTYPE_LIMIT;
		AttachedOrderTarget3Type = SCT_ORDERTYPE_LIMIT;
		AttachedOrderTarget4Type = SCT_ORDERTYPE_LIMIT;
		AttachedOrderTarget5Type = SCT_ORDERTYPE_LIMIT;


		/*Can be one of the following: 
		SCT_ORDERTYPE_STOP, 
		SCT_ORDERTYPE_STOP_LIMIT, 
		SCT_ORDERTYPE_TRAILING_STOP, 
		SCT_ORDERTYPE_TRAILING_STOP_LIMIT, 
		SCT_ORDERTYPE_TRIGGERED_TRAILING_STOP_3_OFFSETS, 
		SCT_ORDERTYPE_TRIGGERED_TRAILING_STOP_LIMIT_3_OFFSETS, 
		SCT_ORDERTYPE_STEP_TRAILING_STOP, 
		SCT_ORDERTYPE_STEP_TRAILING_STOP_LIMIT,
		SCT_ORDERTYPE_TRIGGERED_STEP_TRAILING_STOP, 
		SCT_ORDERTYPE_TRIGGERED_STEP_TRAILING_STOP_LIMIT
		*/
		AttachedOrderStop1Type = SCT_ORDERTYPE_STOP;
		AttachedOrderStop2Type  = SCT_ORDERTYPE_STOP;
		AttachedOrderStop3Type = SCT_ORDERTYPE_STOP;
		AttachedOrderStop4Type = SCT_ORDERTYPE_STOP;
		AttachedOrderStop5Type = SCT_ORDERTYPE_STOP;
		AttachedOrderStopAllType = SCT_ORDERTYPE_STOP;
		
	
		MoveToBreakEven.Type = MOVETO_BE_ACTION_TYPE_NONE;


		StopLimitOrderLimitOffset=DBL_MAX;


	}
};

/****************************************************************************/

struct s_SCTradeOrder
{
	int Version;
	
	// Version 1 is no longer supported
	
	// Version 2
	int InternalOrderID;
	SCString Symbol;
	SCString OrderType;
	unsigned int OrderQuantity;
	SCString Old_OpenClose;
	SCString Old_BuySell;
	double Price1;
	double Price2;
	double AvgFillPrice;
	int Unused;
	
	// Version 3 (also changed in version 7)
	SCOrderStatusCodeEnum OrderStatusCode;
	
	// Version 4
	unsigned int FilledQuantity;
	
	// Version 5
	int ParentInternalOrderID;
	int LinkID;
	
	// Version 6
	SCDateTime LastActivityTime;
	
	// Version 7 - nothing new
	
	// Version 8
	OpenCloseEnum OpenClose;
	BuySellEnum BuySell;

	// Version 9
	SCDateTime EntryDateTime;
	
	// Version 10
	int OrderTypeAsInt;
	
	s_SCTradeOrder()
		: Version(10)
		
		, InternalOrderID(0)
		, OrderQuantity(0)
		, Price1(0.0)
		, Price2(0.0)
		, AvgFillPrice(0.0)
		, Unused(0)
		
		, OrderStatusCode(SCT_OSC_UNSPECIFIED)

		, FilledQuantity(0)
		
		,ParentInternalOrderID(0)
		,LinkID(0)
		
		, OpenClose(OCE_UNDEFINED)
		, BuySell(BSE_UNDEFINED)
		, OrderTypeAsInt(0)
	{
	}

	bool IsWorking()
	{
		return IsWorkingOrderStatus(OrderStatusCode);
	}

	bool IsLimitOrder()
	{
		return (OrderTypeAsInt ==  SCT_ORDERTYPE_LIMIT 
			|| OrderTypeAsInt == SCT_ORDERTYPE_MARKET_IF_TOUCHED 
			|| OrderTypeAsInt == SCT_ORDERTYPE_LIMIT_CHASE 
			|| OrderTypeAsInt == SCT_ORDERTYPE_LIMIT_TOUCH_CHASE);
	}

	bool IsAttachedOrder()
	{
		return ParentInternalOrderID != 0;
	}
};

/****************************************************************************/

struct s_SCPositionData
{
	int Version;
	
	// Version 1 is no longer supported

	// Version 2
	SCString Symbol;
	int PositionQuantity;
	double AveragePrice;
	double OpenProfitLoss;
	double CumulativeProfitLoss;
	int WinTrades;
	int LoseTrades;
	int TotalTrades;
	double MaximumOpenPositionLoss;
	double MaximumOpenPositionProfit;
	double LastTradeProfitLoss;
	
	// Version 3
	int PositionQuantityWithAllWorkingOrders;
	int PositionQuantityWithExitWorkingOrders;
	int WorkingOrdersExist;

	// Version 4
	double DailyProfitLoss;

	// Version 5
	SCDateTime LastFillDateTime;
	SCDateTime LastEntryDateTime;
	SCDateTime LastExitDateTime;

	// Version 6
	double Unused_1;
	int DailyTotalQuantityFilled;
	double Unused_2;
	int Unused_3;
	int PositionQuantityWithExitMarketOrders;

	//Version 7
	int TotalQuantityFilled;

	//Version 8
	int LastTradeQuantity;

	//Version 9
	double TradeStatsDailyProfitLoss;
	int    TradeStatsDailyClosedQuantity;
	double TradeStatsOpenProfitLoss;
	int    TradeStatsOpenQuantity;


	s_SCPositionData()
		: Version(9)
		
		, PositionQuantity(0)
		, AveragePrice(0.0)
		, OpenProfitLoss(0.0)
		, CumulativeProfitLoss(0.0)
		, WinTrades(0)
		, LoseTrades(0)
		, TotalTrades(0)
		,  TotalQuantityFilled (0)
		, MaximumOpenPositionLoss(0.0)
		, MaximumOpenPositionProfit(0.0)
		, LastTradeProfitLoss(0.0)

		, PositionQuantityWithAllWorkingOrders(0)
		, PositionQuantityWithExitWorkingOrders(0)
		, WorkingOrdersExist(0)
		
		, DailyProfitLoss(0.0)
		
		, Unused_1(0.0)
		, DailyTotalQuantityFilled(0)
		, Unused_2(0.0)
		, Unused_3(0)
		, PositionQuantityWithExitMarketOrders(0)
		, LastTradeQuantity(0)
		, TradeStatsDailyProfitLoss(0.0)
		, TradeStatsDailyClosedQuantity(0)
		, TradeStatsOpenProfitLoss(0.0)
		, TradeStatsOpenQuantity(0)
	{
	}
};

/****************************************************************************/

struct s_GetOHLCOfTimePeriod
{
	s_GetOHLCOfTimePeriod(SCDateTime vStartDateTime, SCDateTime vEndDateTime, float& vOpen, float& vHigh, float& vLow, float& vClose, float& vNextOpen, int& vNumberOfBars, SCDateTime& vTotalTimeSpan)
	:	Open(vOpen), 
		High(vHigh), 
		Low(vLow), 
		Close(vClose), 
		NextOpen(vNextOpen), 
		NumberOfBars(vNumberOfBars), 
		TotalTimeSpan(vTotalTimeSpan)
	{
		StartDateTime = vStartDateTime;
		EndDateTime = vEndDateTime;
	}

	SCDateTime StartDateTime;
	SCDateTime EndDateTime;
	float& Open;
	float& High;
	float& Low;
	float& Close;
	float& NextOpen;
	int& NumberOfBars;
	SCDateTime& TotalTimeSpan;
};

/****************************************************************************/



typedef void (SCDLLCALL* fp_SCDLLOnArrayUsed)(int);

template <typename T> class c_ArrayWrapper;
template <typename T> class c_ConstArrayWrapper;

template <typename T>
class c_ArrayWrapper
{
	private:
		T* m_Data;
		int m_NumElements;
		
		fp_SCDLLOnArrayUsed m_fp_OnArrayUsed;
		int m_OnArrayUsedParam;
		
		int m_NumExtendedElements;
		
		char Reserved[44]; 
		
		T m_DefaultElement;
		
	public:
		c_ArrayWrapper()
		{
			ResetMembers();
		}
		explicit c_ArrayWrapper(int i)
		{
			ResetMembers();
		}
		c_ArrayWrapper(const c_ArrayWrapper& Src)
			: m_Data(Src.m_Data)
			, m_NumElements(Src.m_NumElements)
			, m_NumExtendedElements(Src.m_NumExtendedElements)
			, m_fp_OnArrayUsed(Src.m_fp_OnArrayUsed)
			, m_OnArrayUsedParam(Src.m_OnArrayUsedParam)
			, m_DefaultElement(0)
		{
			memset(Reserved, 0, sizeof(Reserved));
		}
		
		const c_ArrayWrapper& operator = (const c_ArrayWrapper& Src)
		{
			m_Data = Src.m_Data;
			m_NumElements = Src.m_NumElements;
			m_NumExtendedElements = Src.m_NumExtendedElements;
			m_fp_OnArrayUsed = Src.m_fp_OnArrayUsed;
			m_OnArrayUsedParam = Src.m_OnArrayUsedParam;
			m_DefaultElement = Src.m_DefaultElement;
			
			return *this;
		}

		void ResetMembers()
		{
			m_Data = NULL;
			m_NumElements = 0;
			m_NumExtendedElements = 0;
			m_fp_OnArrayUsed = NULL;
			m_OnArrayUsedParam = 0;
			m_DefaultElement = T(0);
			
			memset(Reserved, 0, sizeof(Reserved));
		}
		
		T& operator [] (int Index)
		{
			return ElementAt(Index);
		}
		
		const T& operator [] (int Index) const
		{
			return ElementAt(Index);
		}
		
		T& ElementAt(int Index)
		{
			if (m_Data == NULL && m_fp_OnArrayUsed != NULL)
				m_fp_OnArrayUsed(m_OnArrayUsedParam);  // This will allocate and set up the array if it can
			
			if (m_Data == NULL)
				return m_DefaultElement;
			
			int ExtendedArraySize = GetExtendedArraySize();
			if (ExtendedArraySize == 0)
				return m_DefaultElement;
			
			if (Index < 0)
				Index = 0;
			if (Index >= ExtendedArraySize)
				Index = ExtendedArraySize - 1;
			
			return m_Data[Index];
		}
		
		const T& ElementAt(int Index) const
		{
			if (m_Data == NULL && m_fp_OnArrayUsed != NULL)
				m_fp_OnArrayUsed(m_OnArrayUsedParam);  // This will allocate and set up the array if it can
			
			if (m_Data == NULL)
				return m_DefaultElement;
			
			int ExtendedArraySize = GetExtendedArraySize();
			if (ExtendedArraySize == 0)
				return m_DefaultElement;
			
			if (Index < 0)
				Index = 0;
			if (Index >= ExtendedArraySize)
				Index = ExtendedArraySize - 1;
			
			return m_Data[Index];
		}
		
		// For Sierra Chart internal use only
		void InternalSetArray(T* DataPointer, int Size, int NumExtendedElements = 0)
		{
			m_NumElements = Size;
			m_NumExtendedElements = NumExtendedElements;
			m_Data = DataPointer;
		}
		
		// For Sierra Chart internal use only
		void InternalSetOnUsed(fp_SCDLLOnArrayUsed OnArrayUsed, int Param)
		{
			m_fp_OnArrayUsed = OnArrayUsed;
			m_OnArrayUsedParam = Param;
		}
		
		int GetArraySize() const
		{
			return m_NumElements;
		}
		
		int GetExtendedArraySize() const
		{
			if (m_NumElements == 0)
				return 0;
			
			return m_NumElements + m_NumExtendedElements;
		}
		
		// This should only be used when absolutely necessary and you know what
		// you are doing with it.
		T* GetPointer()
		{
			return m_Data;
		}
		const T* GetPointer() const
		{
			return m_Data;
		}
	
	friend class c_ConstArrayWrapper<T>;
};

template <typename T>
class c_ConstArrayWrapper
{
	private:
		const T* m_Data;
		int m_NumElements;
		
		fp_SCDLLOnArrayUsed m_OnArrayUsed;
		int m_OnArrayUsedParam;
		
		int m_NumExtendedElements;
		
		char Reserved[44];
		
		T m_DefaultElement;
		
	public:
		c_ConstArrayWrapper()
			: m_Data(NULL)
			, m_NumElements(0)
			, m_NumExtendedElements(0)
			, m_OnArrayUsed(NULL)
			, m_OnArrayUsedParam(0)
			, m_DefaultElement(0)
		{
			memset(Reserved, 0, sizeof(Reserved));
		}
		explicit c_ConstArrayWrapper(int i)
			: m_Data(NULL)
			, m_NumElements(0)
			, m_NumExtendedElements(0)
			, m_OnArrayUsed(NULL)
			, m_OnArrayUsedParam(0)
			, m_DefaultElement(0)
		{
			memset(Reserved, 0, sizeof(Reserved));
		}
		c_ConstArrayWrapper(const c_ConstArrayWrapper& Src)
			: m_Data(Src.m_Data)
			, m_NumElements(Src.m_NumElements)
			, m_NumExtendedElements(Src.m_NumExtendedElements)
			, m_OnArrayUsed(Src.m_OnArrayUsed)
			, m_OnArrayUsedParam(Src.m_OnArrayUsedParam)
			, m_DefaultElement(0)
		{
			memset(Reserved, 0, sizeof(Reserved));
		}
		c_ConstArrayWrapper(const c_ArrayWrapper<T>& Src)
			: m_Data(Src.m_Data)
			, m_NumElements(Src.m_NumElements)
			, m_NumExtendedElements(Src.m_NumExtendedElements)
			, m_OnArrayUsed(Src.m_OnArrayUsed)
			, m_OnArrayUsedParam(Src.m_OnArrayUsedParam)
			, m_DefaultElement(0)
		{
			memset(Reserved, 0, sizeof(Reserved));
		}
		
		~c_ConstArrayWrapper()
		{
		}
		
		const c_ConstArrayWrapper& operator = (const c_ConstArrayWrapper& Src)
		{
			m_Data = Src.m_Data;
			m_NumElements = Src.m_NumElements;
			m_NumExtendedElements = Src.m_NumExtendedElements;
			m_OnArrayUsed = Src.m_OnArrayUsed;
			m_OnArrayUsedParam = Src.m_OnArrayUsedParam;
			m_DefaultElement = Src.m_DefaultElement;
			
			return *this;
		}
		const c_ConstArrayWrapper& operator = (const c_ArrayWrapper<T>& Src)
		{
			m_Data = Src.m_Data;
			m_NumElements = Src.m_NumElements;
			m_NumExtendedElements = Src.m_NumExtendedElements;
			m_OnArrayUsed = Src.m_OnArrayUsed;
			m_OnArrayUsedParam = Src.m_OnArrayUsedParam;
			m_DefaultElement = Src.m_DefaultElement;
			
			return *this;
		}
		
		const T& operator [] (int Index) const
		{
			return ElementAt(Index);
		}
		
		const T& ElementAt(int Index) const
		{
			if (m_Data == NULL && m_OnArrayUsed != NULL)
				m_OnArrayUsed(m_OnArrayUsedParam);  // This will allocate and set up the array if it can
			
			if (m_Data == NULL)
				return m_DefaultElement;

			int ExtendedArraySize = GetExtendedArraySize();
			
			if (Index < 0)
				Index = 0;
			if (Index >= ExtendedArraySize)
				Index = ExtendedArraySize - 1;
			
			return m_Data[Index];
		}
		
		// For Sierra Chart internal use only
		void InternalSetArray(const T* DataPointer, int Size, int NumExtendedElements = 0)
		{
			m_NumElements = Size;
			m_NumExtendedElements = NumExtendedElements;
			m_Data = DataPointer;
		}
		
		// For Sierra Chart internal use only
		void InternalSetOnUsed(fp_SCDLLOnArrayUsed OnArrayUsed, int Param)
		{
			m_OnArrayUsed = OnArrayUsed;
			m_OnArrayUsedParam = Param;
		}
		
		int GetArraySize() const
		{
			return m_NumElements;
		}

		int GetExtendedArraySize() const
		{
			if (m_NumElements == 0)
				return 0;

			return m_NumElements + m_NumExtendedElements;
		}
		
		// This should only be used when absolutely necessary and you know what
		// you are doing with it.
		const T* GetPointer() const
		{
			return m_Data;
		}

		friend class c_ArrayWrapper<T>;
};

/****************************************************************************/

class SCDateTimeArray
	: public c_ArrayWrapper<SCDateTime>
{
	public:
		/*====================================================================
			Days since 1899-12-30.
		--------------------------------------------------------------------*/
		int DateAt(int Index) const
		{
			return DATE_PART(ElementAt(Index));
		}
		/*====================================================================
			Seconds since midnight.
		--------------------------------------------------------------------*/
		int TimeAt(int Index) const
		{
			return TIME_PART(ElementAt(Index));
		}
		
		/*====================================================================
			Returns the date that was set.
		--------------------------------------------------------------------*/
		int SetDateAt(int Index, int Date)
		{
			ElementAt(Index) = COMBINE_DATE_TIME(Date, TimeAt(Index));
			return Date;
		}
		/*====================================================================
			Returns the time that was set.
		--------------------------------------------------------------------*/
		int SetTimeAt(int Index, int Time)
		{
			ElementAt(Index) = COMBINE_DATE_TIME(DateAt(Index), Time);
			return Time;
		}
};

class SCConstDateTimeArray
	: public c_ConstArrayWrapper<SCDateTime>
{
	public:
		/*====================================================================
			Days since 1899-12-30.
		--------------------------------------------------------------------*/
		int DateAt(int Index) const
		{
			return DATE_PART(ElementAt(Index));
		}
		/*====================================================================
			Seconds since midnight.
		--------------------------------------------------------------------*/
		int TimeAt(int Index) const
		{
			return TIME_PART(ElementAt(Index));
		}
};

typedef SCDateTimeArray &SCDateTimeArrayRef;
typedef c_ArrayWrapper<unsigned short> SCUShortArray;
typedef c_ArrayWrapper<unsigned int> SCUIntArray;
typedef c_ArrayWrapper<unsigned int>& SCUIntArrayRef;
typedef c_ArrayWrapper<SCUIntArray> SCUIntArrayArray;
typedef c_ArrayWrapper<float> SCFloatArray;
typedef c_ArrayWrapper<COLORREF> SCColorArray;
typedef c_ArrayWrapper<COLORREF>& SCColorArrayRef;
typedef c_ConstArrayWrapper<unsigned short> SCConstUShortArray;
typedef c_ConstArrayWrapper<char> SCConstCharArray;

typedef c_ArrayWrapper<float> SCConstFloatArray;

typedef c_ArrayWrapper<SCFloatArray>& SCBaseDataRef;
typedef c_ArrayWrapper<float>& SCFloatArrayRef;
typedef c_ArrayWrapper<float>& SCConstFloatArrayRef;
typedef const c_ArrayWrapper<float>& SCFloatArrayInRef;

typedef c_ArrayWrapper<SCConstFloatArray> SCConstFloatArrayArray;
typedef c_ArrayWrapper<SCFloatArray> SCFloatArrayArray;

typedef c_ArrayWrapper<SCString> SCStringArray;

class SCGraphData
	: public SCFloatArrayArray
{
	private:
		SCFloatArray BaseData[SC_SUBGRAPHS_AVAILABLE];
		
	public:
		SCGraphData()
		{
			InternalSetArray(BaseData, SC_SUBGRAPHS_AVAILABLE);
		}
		
		SCFloatArrayRef InternalAccessBaseDataArray(int Index)
		{
			if (Index < 0)
				return BaseData[0];
			
			if (Index >= SC_SUBGRAPHS_AVAILABLE)
				return BaseData[SC_SUBGRAPHS_AVAILABLE - 1];
			
			return BaseData[Index];
		}
};

/****************************************************************************/
struct s_SCSubgraph_260
{
	SCString Name;
	
	unsigned int PrimaryColor;
	unsigned int SecondaryColor;
	unsigned int SecondaryColorUsed;  // boolean
	
	unsigned short DrawStyle;
	unsigned short LineStyle;
	unsigned short LineWidth;
	unsigned short OldLineLabel;  // Line label Name/Value
	
	SCFloatArray Data;  // Array of data values
	SCColorArray DataColor;  // Array of colors for each of the data elements
	
	SCFloatArrayArray	Arrays;  // Array of extra arrays
	
	unsigned char Unused;
	unsigned char Unused2;
	unsigned char AutoColoring;  // boolean
	
	int GraphicalDisplacement;
	
	int DrawZeros;  // boolean
	unsigned short DisplayNameValueInWindowsFlags; // This controls whether the Subgraph name and value are displayed in the Values windows and on the Region Data Line.
	
	unsigned int LineLabel;  // Line label Name/Value
	int ExtendedArrayElementsToGraph;
	
	SCString TextDrawStyleText;
	
	float GradientAngleUnit;
	float GradientAngleMax;

	SCString ShortName;

	char Reserve[74];
	
	s_SCSubgraph_260()
		: Name()
		, PrimaryColor(0)
		, SecondaryColor(0)
		, SecondaryColorUsed(0)
		, DrawStyle(0)
		, LineStyle(0)
		, LineWidth(0)
		, OldLineLabel(0)
		, LineLabel(0)
		, Data()
		, DataColor()
		, Unused(0)
		, Unused2(0)
		, AutoColoring(0)
		, GraphicalDisplacement(0)
		, DrawZeros(0)
		, DisplayNameValueInWindowsFlags(0)
		, ExtendedArrayElementsToGraph(0)
		, TextDrawStyleText()
		, GradientAngleUnit(0)
		, GradientAngleMax(0)
		, ShortName()
	{
		memset(Reserve, 0, sizeof(Reserve));
	}

	// This constructor is so this struct can work as an c_ArrayWrapper element
	explicit s_SCSubgraph_260(int i)
		: Name()
		, PrimaryColor(0)
		, SecondaryColor(0)
		, SecondaryColorUsed(0)
		, DrawStyle(0)
		, LineStyle(0)
		, LineWidth(0)
		, OldLineLabel(0)
		, LineLabel(0)
		, Data()
		, DataColor()
		, Unused(0)
		, Unused2(0)
		, AutoColoring(0)
		, GraphicalDisplacement(0)
		, DrawZeros(0)
		, DisplayNameValueInWindowsFlags(0)
		, ExtendedArrayElementsToGraph(0)
		, TextDrawStyleText()
		, GradientAngleUnit(0)
		, GradientAngleMax(0)
		, ShortName()
	{
		memset(Reserve, 0, sizeof(Reserve));
	}

	float& operator [] (int Index)
	{
		return Data[Index];
	}

	operator SCFloatArray& ()
	{
		return Data;
	}
};

typedef c_ArrayWrapper<s_SCSubgraph_260> SCSubgraph260Array;
typedef s_SCSubgraph_260& SCSubgraph260Ref;
typedef s_SCSubgraph_260& SCSubgraphRef;

/****************************************************************************/
struct s_ChartStudySubgraphValues
{
	int ChartNumber;
	int StudyID;
	int SubgraphIndex;
};

struct s_SCInput_145
{
	SCString Name;
	
	unsigned char ValueType;
	unsigned char Unused1;
	unsigned short DisplayOrder;
	#pragma pack(push, 4)  // This is necessary otherwise data items larger that 4 bytes will throw off the alignment and size of the union
	union
	{
		unsigned int IndexValue;
		float FloatValue;
		unsigned int BooleanValue;
		struct
		{
			double DateTimeValue;  // SCDateTime
			unsigned int TimeZoneValue;
		};
		int IntValue;
		unsigned int ColorValue;
		s_ChartStudySubgraphValues ChartStudySubgraphValues;
		double DoubleValue;
		struct  
		{
			bool        StringModified;
			const char* StringValue;
		};

		unsigned char ValueBytes[16];
	};
	
	union
	{
		float FloatValueMin;
		double DateTimeValueMin;  // SCDateTime
		int IntValueMin;
		double DoubleValueMin;
		
		unsigned char ValueBytesMin[16];
	};
	union
	{
		float FloatValueMax;
		double DateTimeValueMax;  // SCDateTime
		int IntValueMax;
		double DoubleValueMax;
		
		unsigned char ValueBytesMax[16];
	};
	#pragma pack(pop)
	
	// Do not use or modify these.  For internal use only
	int InputIndex;
	void (SCDLLCALL* InternalSetCustomStrings)(int InputIndex, const char* CustomStrings);
	const char* SelectedCustomInputString;
	
	void (SCDLLCALL* InternalSetDescription)(int InputIndex, const char* Description);
	
	char Reserve[48];
	
	s_SCInput_145()
	{
		Clear();
	}
	
	explicit s_SCInput_145(int i)
	{
		Clear();
	}
	
	void Clear()
	{
		memset(this, 0, sizeof(s_SCInput_145));
		Name.Initialize();
	}
	
	unsigned int GetIndex() const
	{
		switch (ValueType)
		{
			case OHLC_VALUE:  // IndexValue
			case STUDYINDEX_VALUE:  // IndexValue
			case SUBGRAPHINDEX_VALUE:  // IndexValue
			case MOVAVGTYPE_VALUE:  // IndexValue
			case TIME_PERIOD_LENGTH_UNIT_VALUE:
			case STUDYID_VALUE:  // IndexValue
			case ALERT_SOUND_NUMBER_VALUE:
			case CANDLESTICK_PATTERNS_VALUE:
			case CUSTOM_STRING_VALUE:
			case TIMEZONE_VALUE:  // IndexValue
			return IndexValue;
			
			case FLOAT_VALUE:  // FloatValue
			return (unsigned int)((FloatValue < 0.0f) ? (FloatValue - 0.5f) : (FloatValue + 0.5f));
			
			case YESNO_VALUE:  // BooleanValue
			return (BooleanValue != 0) ? 1 : 0;
			
			case DATE_VALUE:  // DateTimeValue
			case TIME_VALUE:  // DateTimeValue
			case DATETIME_VALUE:  // DateTimeValue
			return 0;
			
			case INT_VALUE:  // IntValue
			case CHART_NUMBER:
			return (unsigned int)IntValue;
			
			case COLOR_VALUE:  // ColorValue
			return (unsigned int)ColorValue;
			
			case CHART_STUDY_SUBGRAPH_VALUES:
			case STUDY_SUBGRAPH_VALUES:
			case CHART_STUDY_VALUES:
			return 0;
			
			case DOUBLE_VALUE:  // DoubleValue
			return (unsigned int)((DoubleValue < 0.0) ? (DoubleValue - 0.5f) : (DoubleValue + 0.5f));
			
			case TIME_WITH_TIMEZONE_VALUE:
			return TimeZoneValue;
			
			case STRING_VALUE:
				return StringValue == NULL ? 0 : strlen(StringValue);

			case NO_VALUE:
			default:
			return 0;
		}
	}
	
	float GetFloat() const
	{
		switch (ValueType)
		{
			case OHLC_VALUE:  // IndexValue
			case STUDYINDEX_VALUE:  // IndexValue
			case SUBGRAPHINDEX_VALUE:  // IndexValue
			case MOVAVGTYPE_VALUE:  // IndexValue
			case TIME_PERIOD_LENGTH_UNIT_VALUE:
			case STUDYID_VALUE:  // IndexValue
			case ALERT_SOUND_NUMBER_VALUE:
			case CANDLESTICK_PATTERNS_VALUE:
			case CUSTOM_STRING_VALUE:
			case TIMEZONE_VALUE:  // IndexValue
			return (float)IndexValue;
			
			case FLOAT_VALUE:  // FloatValue
			return FloatValue;
			
			case YESNO_VALUE:  // BooleanValue
			return (BooleanValue != 0) ? 1.0f : 0.0f;
			
			case DATE_VALUE:  // DateTimeValue
			case TIME_VALUE:  // DateTimeValue
			case DATETIME_VALUE:  // DateTimeValue
			return 0.0f;
			
			case INT_VALUE:  // IntValue
			case CHART_NUMBER:
			return (float)IntValue;
			
			case COLOR_VALUE:  // ColorValue
			return 0.0f;
			
			case CHART_STUDY_SUBGRAPH_VALUES:
			case STUDY_SUBGRAPH_VALUES:
			case CHART_STUDY_VALUES:
			return 0.0f;
			
			case DOUBLE_VALUE:  // DoubleValue
			return (float)DoubleValue;
			
			case TIME_WITH_TIMEZONE_VALUE:
			return 0.0f;
			
			case STRING_VALUE:
				return StringValue == NULL ? 0 : (float)strlen(StringValue);

			case NO_VALUE:
			default:
			return 0.0f;
		}
	}
	
	unsigned int GetBoolean() const
	{
		switch (ValueType)
		{
			case OHLC_VALUE:  // IndexValue
			case STUDYINDEX_VALUE:  // IndexValue
			case SUBGRAPHINDEX_VALUE:  // IndexValue
			case MOVAVGTYPE_VALUE:  // IndexValue
			case TIME_PERIOD_LENGTH_UNIT_VALUE:
			case STUDYID_VALUE:  // IndexValue
			case ALERT_SOUND_NUMBER_VALUE:
			case CANDLESTICK_PATTERNS_VALUE:
			case CUSTOM_STRING_VALUE:
			case TIMEZONE_VALUE:  // IndexValue
			return (IndexValue != 0) ? 1 : 0;
			
			case FLOAT_VALUE:  // FloatValue
			return (FloatValue != 0.0f) ? 1 : 0;
			
			case YESNO_VALUE:  // BooleanValue
			return (BooleanValue != 0) ? 1 : 0;
			
			case DATE_VALUE:  // DateTimeValue
			case TIME_VALUE:  // DateTimeValue
			case DATETIME_VALUE:  // DateTimeValue
			return (DateTimeValue != 0) ? 1 : 0;
			
			case INT_VALUE:  // IntValue
			case CHART_NUMBER:
			return (IntValue != 0) ? 1 : 0;
			
			case COLOR_VALUE:  // ColorValue
			return (ColorValue != 0) ? 1 : 0;
			
			case CHART_STUDY_SUBGRAPH_VALUES:
			case STUDY_SUBGRAPH_VALUES:
			case CHART_STUDY_VALUES:
			return 0;
			
			case DOUBLE_VALUE:  // DoubleValue
			return (DoubleValue != 0.0) ? 1 : 0;
			
			case TIME_WITH_TIMEZONE_VALUE:
			return (DateTimeValue != 0) ? 1 : 0;
			
			case STRING_VALUE:
			return StringValue == NULL ? 0 : strlen(StringValue) > 0 ? 1 : 0;

			case NO_VALUE:
			default:
			return 0;
		}
	}
	
	SCDateTime GetDateTime() const
	{
		switch (ValueType)
		{
			case OHLC_VALUE:  // IndexValue
			case STUDYINDEX_VALUE:  // IndexValue
			case SUBGRAPHINDEX_VALUE:  // IndexValue
			case MOVAVGTYPE_VALUE:  // IndexValue
			case TIME_PERIOD_LENGTH_UNIT_VALUE:
			case STUDYID_VALUE:  // IndexValue
			case ALERT_SOUND_NUMBER_VALUE:
			case CANDLESTICK_PATTERNS_VALUE:
			case CUSTOM_STRING_VALUE:
			case TIMEZONE_VALUE:  // IndexValue
			return 0.0;
			
			case FLOAT_VALUE:  // FloatValue
			return 0.0;
			
			case YESNO_VALUE:  // BooleanValue
			return 0.0;
			
			case DATE_VALUE:  // DateTimeValue
			case TIME_VALUE:  // DateTimeValue
			case DATETIME_VALUE:  // DateTimeValue
			return DateTimeValue;
			
			case INT_VALUE:  // IntValue
			case CHART_NUMBER:
			return 0.0;
			
			case COLOR_VALUE:  // ColorValue
			return 0.0;
			
			case CHART_STUDY_SUBGRAPH_VALUES:
			case STUDY_SUBGRAPH_VALUES:
			case CHART_STUDY_VALUES:
			return 0.0;
			
			case DOUBLE_VALUE:  // DoubleValue
			return 0.0;
			
			case TIME_WITH_TIMEZONE_VALUE:
			return DateTimeValue;
			
			case STRING_VALUE:
			return 0;

			case NO_VALUE:
			default:
			return 0;
		}
	}
	
	int GetInt() const
	{
		switch (ValueType)
		{
			case OHLC_VALUE:  // IndexValue
			case STUDYINDEX_VALUE:  // IndexValue
			case SUBGRAPHINDEX_VALUE:  // IndexValue
			case MOVAVGTYPE_VALUE:  // IndexValue
			case TIME_PERIOD_LENGTH_UNIT_VALUE:
			case STUDYID_VALUE:  // IndexValue
			case ALERT_SOUND_NUMBER_VALUE:
			case CANDLESTICK_PATTERNS_VALUE:
			case CUSTOM_STRING_VALUE:
			case TIMEZONE_VALUE:  // IndexValue
			return (int)IndexValue;
			
			case FLOAT_VALUE:  // FloatValue
			return (int)((FloatValue < 0.0f) ? (FloatValue - 0.5f) : (FloatValue + 0.5f));
			
			case YESNO_VALUE:  // BooleanValue
			return (BooleanValue != 0) ? 1 : 0;
			
			case DATE_VALUE:  // DateTimeValue
			return DATE_PART(DateTimeValue);
			
			case TIME_VALUE:  // DateTimeValue
			return TIME_PART(DATETIME_VALUE);
			
			case DATETIME_VALUE:  // DateTimeValue
			return 0;
			
			case INT_VALUE:  // IntValue
			case CHART_NUMBER:
			return IntValue;
			
			case COLOR_VALUE:  // ColorValue
			return (int)ColorValue;
			
			case CHART_STUDY_SUBGRAPH_VALUES:
			case STUDY_SUBGRAPH_VALUES:
			case CHART_STUDY_VALUES:
			return 0;
			
			case DOUBLE_VALUE:  // DoubleValue
			return (int)((DoubleValue < 0.0) ? (DoubleValue - 0.5) : (DoubleValue + 0.5));
			
			case TIME_WITH_TIMEZONE_VALUE:
			return TIME_PART(DateTimeValue);
			
			case STRING_VALUE:
			return StringValue == NULL ? 0 : strlen(StringValue);

			case NO_VALUE:
			default:
			return 0;
		}
	}
	
	unsigned int GetColor() const
	{
		switch (ValueType)
		{
			case OHLC_VALUE:  // IndexValue
			case STUDYINDEX_VALUE:  // IndexValue
			case SUBGRAPHINDEX_VALUE:  // IndexValue
			case MOVAVGTYPE_VALUE:  // IndexValue
			case TIME_PERIOD_LENGTH_UNIT_VALUE:
			case STUDYID_VALUE:  // IndexValue
			case ALERT_SOUND_NUMBER_VALUE:
			case CANDLESTICK_PATTERNS_VALUE:
			case CUSTOM_STRING_VALUE:
			case TIMEZONE_VALUE:  // IndexValue
			return (unsigned int)IndexValue;
			
			case FLOAT_VALUE:  // FloatValue
			return 0;
			
			case YESNO_VALUE:  // BooleanValue
			return (BooleanValue != 0) ? RGB(255,255,255) : 0;
			
			case DATE_VALUE:  // DateTimeValue
			case TIME_VALUE:  // DateTimeValue
			case DATETIME_VALUE:  // DateTimeValue
			return 0;
			
			case INT_VALUE:  // IntValue
			case CHART_NUMBER:
			return (unsigned int)IntValue;
			
			case COLOR_VALUE:  // ColorValue
			return ColorValue;
			
			case CHART_STUDY_SUBGRAPH_VALUES:
			case STUDY_SUBGRAPH_VALUES:
			case CHART_STUDY_VALUES:
			return 0;
			
			case DOUBLE_VALUE:  // DoubleValue
			return 0;
			
			case TIME_WITH_TIMEZONE_VALUE:
			return 0;
			
			case STRING_VALUE:
			return 0;

			case NO_VALUE:
			default:
			return 0;
		}
	}
	
	double GetDouble() const
	{
		switch (ValueType)
		{
			case OHLC_VALUE:  // IndexValue
			case STUDYINDEX_VALUE:  // IndexValue
			case SUBGRAPHINDEX_VALUE:  // IndexValue
			case MOVAVGTYPE_VALUE:  // IndexValue
			case TIME_PERIOD_LENGTH_UNIT_VALUE:
			case STUDYID_VALUE:  // IndexValue
			case ALERT_SOUND_NUMBER_VALUE:
			case CANDLESTICK_PATTERNS_VALUE:
			case CUSTOM_STRING_VALUE:
			case TIMEZONE_VALUE:  // IndexValue
			return (double)IndexValue;
			
			case FLOAT_VALUE:  // FloatValue
			return (double)FloatValue;
			
			case YESNO_VALUE:  // BooleanValue
			return (BooleanValue != 0) ? 1.0 : 0.0;
			
			case DATE_VALUE:  // DateTimeValue
			case TIME_VALUE:  // DateTimeValue
			case DATETIME_VALUE:  // DateTimeValue
			return DateTimeValue;
			
			case INT_VALUE:  // IntValue
			case CHART_NUMBER:
			return (double)IntValue;
			
			case COLOR_VALUE:  // ColorValue
			return 0.0;
			
			case CHART_STUDY_SUBGRAPH_VALUES:
			case STUDY_SUBGRAPH_VALUES:
			case CHART_STUDY_VALUES:
			return 0.0;
			
			case DOUBLE_VALUE:  // DoubleValue
			return DoubleValue;
			
			case TIME_WITH_TIMEZONE_VALUE:
			return DateTimeValue;
			
			case STRING_VALUE:
			return StringValue == NULL ? 0 : (double)strlen(StringValue);

			case NO_VALUE:
			default:
			return 0.0;
		}
	}
	
	unsigned int GetInputDataIndex() const
	{
		unsigned int InputDataIndex = GetIndex();
		
		if (InputDataIndex > SC_SUBGRAPHS_AVAILABLE - 1)
			InputDataIndex = SC_SUBGRAPHS_AVAILABLE - 1;
		
		return InputDataIndex;
	}
	
	unsigned int GetStudyIndex() const
	{
		return GetIndex();
	}
	
	unsigned int GetSubgraphIndex() const
	{
		unsigned int SubgraphIndex;
		if (ValueType == CHART_STUDY_SUBGRAPH_VALUES || ValueType == STUDY_SUBGRAPH_VALUES)
			SubgraphIndex = (unsigned int)ChartStudySubgraphValues.SubgraphIndex;
		else
			SubgraphIndex = GetIndex();
		
		if (SubgraphIndex > SC_SUBGRAPHS_AVAILABLE - 1)
			SubgraphIndex = SC_SUBGRAPHS_AVAILABLE - 1;
		
		return SubgraphIndex;
	}
	
	unsigned int GetMovAvgType() const
	{
		unsigned int MovAvgTypeValue = GetIndex();
		if (MovAvgTypeValue >= MOVAVGTYPE_NUMBER_OF_AVERAGES)
			MovAvgTypeValue = 0;
		return MovAvgTypeValue;
	}
	
	unsigned int GetTimePeriodType() const
	{
		return GetIndex();
	}
	
	unsigned int GetAlertSoundNumber() const
	{
		return GetIndex();
	}
	
	unsigned int GetCandleStickPatternIndex() const
	{
		return GetIndex();
	}
	
	unsigned int GetStudyID() const
	{
		if (ValueType == CHART_STUDY_SUBGRAPH_VALUES || ValueType == STUDY_SUBGRAPH_VALUES || ValueType == CHART_STUDY_VALUES)
			return (unsigned int)ChartStudySubgraphValues.StudyID;
		else
			return GetIndex();
	}
	
	unsigned int GetYesNo() const
	{
		return GetBoolean();
	}
	
	int GetDate() const
	{
		return DATE_PART(GetDateTime());
	}
	
	int GetTime() const
	{
		return TIME_PART(GetDateTime());
	}
	
	void GetChartStudySubgraphValues(int& ChartNumber, int& StudyID, int& SubgraphIndex) const
	{
		ChartNumber = ChartStudySubgraphValues.ChartNumber;
		StudyID = ChartStudySubgraphValues.StudyID;
		SubgraphIndex = ChartStudySubgraphValues.SubgraphIndex;
	}
	
	s_ChartStudySubgraphValues GetChartStudySubgraphValues() const
	{
		return ChartStudySubgraphValues;
	}
	
	int GetChartNumber()
	{
		if (ValueType == CHART_STUDY_SUBGRAPH_VALUES || ValueType == CHART_STUDY_VALUES)
			return ChartStudySubgraphValues.ChartNumber;
		else if (ValueType == CHART_NUMBER)
			return IntValue;
		else
			return GetInt();
	}
	
	const SCString GetSelectedCustomString()
	{
		SCString TempString;
		if(SelectedCustomInputString != NULL)
		 TempString = SelectedCustomInputString;

		return TempString;
	}
	
	TimeZonesEnum GetTimeZone()
	{
		unsigned int TimeZone = TIMEZONE_NOT_SPECIFIED;
		
		if (ValueType == TIMEZONE_VALUE)
			TimeZone = GetIndex();
		else if (ValueType == TIME_WITH_TIMEZONE_VALUE)
			TimeZone = TimeZoneValue;
		
		if (TimeZone >= NUM_TIME_ZONES)
			TimeZone = TIMEZONE_NOT_SPECIFIED;
		
		return (TimeZonesEnum)TimeZone;
	}
	
	const char* GetString()
	{
		if (ValueType == STRING_VALUE && StringValue != NULL)
			return StringValue;

		return "INVALID";
	}

	s_SCInput_145& SetInputDataIndex(unsigned int Value)
	{
		ValueType = OHLC_VALUE;
		IndexValue = Value;
		return *this;
	}
	
	s_SCInput_145& SetStudyIndex(unsigned int Value)
	{
		ValueType = STUDYINDEX_VALUE;
		IndexValue = Value;
		return *this;
	}
	
	s_SCInput_145& SetSubgraphIndex(unsigned int Value)
	{
		ValueType = SUBGRAPHINDEX_VALUE;
		IndexValue = Value;
		return *this;
	}
	
	s_SCInput_145& SetMovAvgType(unsigned int Value)
	{
		ValueType = MOVAVGTYPE_VALUE;
		IndexValue = Value;
		return *this;
	}
	
	s_SCInput_145& SetTimePeriodType(unsigned int Value)
	{
		ValueType = TIME_PERIOD_LENGTH_UNIT_VALUE;
		IndexValue = Value;
		return *this;
	}
	
	s_SCInput_145& SetFloat(float Value)
	{
		ValueType = FLOAT_VALUE;
		FloatValue = Value;
		return *this;
	}
	
	s_SCInput_145& SetYesNo(unsigned int Value)
	{
		ValueType = YESNO_VALUE;
		BooleanValue = (Value != 0)?1:0;
		return *this;
	}
	
	s_SCInput_145& SetDate(int Value)
	{
		ValueType = DATE_VALUE;
		DateTimeValue = COMBINE_DATE_TIME(Value, 0);
		return *this;
	}
	
	s_SCInput_145& SetTime(int Value)
	{
		ValueType = TIME_VALUE;
		DateTimeValue = COMBINE_DATE_TIME(0, Value);
		return *this;
	}
	
	s_SCInput_145& SetDateTime(SCDateTime Value)
	{
		ValueType = DATETIME_VALUE;
		DateTimeValue = Value;
		return *this;
	}
	
	s_SCInput_145& SetInt(int Value)
	{
		ValueType = INT_VALUE;
		IntValue = Value;
		return *this;
	}
	
	s_SCInput_145& SetStudyID(unsigned int Value)
	{
		ValueType = STUDYID_VALUE;
		IndexValue = Value;
		return *this;
	}
	
	s_SCInput_145& SetColor(unsigned int Color)
	{
		ValueType = COLOR_VALUE;
		ColorValue = Color;
		return *this;
	}
	
	s_SCInput_145& SetColor(unsigned char Red, unsigned char Green, unsigned char Blue)
	{
		return SetColor(RGB(Red, Green, Blue));
	}
	
	s_SCInput_145& SetAlertSoundNumber(unsigned int Value)
	{
		ValueType = ALERT_SOUND_NUMBER_VALUE;
		IndexValue = Value;
		return *this;
	}
	
	s_SCInput_145& SetCandleStickPatternIndex(unsigned int Value)
	{
		ValueType = CANDLESTICK_PATTERNS_VALUE;
		IndexValue = Value;
		return *this;
	}
	
	s_SCInput_145& SetChartStudySubgraphValues(int ChartNumber, int StudyID, int SubgraphIndex)
	{
		ValueType = CHART_STUDY_SUBGRAPH_VALUES;
		ChartStudySubgraphValues.ChartNumber = ChartNumber;
		ChartStudySubgraphValues.StudyID = StudyID;
		ChartStudySubgraphValues.SubgraphIndex = SubgraphIndex;
		return *this;
	}
	
	s_SCInput_145& SetChartNumber(int ChartNumber)
	{
		ValueType = CHART_NUMBER;
		IntValue = ChartNumber;
		return *this;
	}
	
	s_SCInput_145& SetStudySubgraphValues(int StudyID, int SubgraphIndex)
	{
		ValueType = STUDY_SUBGRAPH_VALUES;
		ChartStudySubgraphValues.StudyID = StudyID;
		ChartStudySubgraphValues.SubgraphIndex = SubgraphIndex;
		return *this;
	}
	
	s_SCInput_145& SetChartStudyValues(int ChartNumber, int StudyID)
	{
		ValueType = CHART_STUDY_VALUES;
		ChartStudySubgraphValues.ChartNumber = ChartNumber;
		ChartStudySubgraphValues.StudyID = StudyID;
		return *this;
	}
	
	s_SCInput_145& SetCustomInputIndex(unsigned int Value)
	{
		ValueType = CUSTOM_STRING_VALUE;
		IndexValue = Value;
		return *this;
	}
	
	s_SCInput_145& SetDouble(double Value)
	{
		ValueType = DOUBLE_VALUE;
		DoubleValue = Value;
		return *this;
	}
	
	s_SCInput_145& SetTimeZone(TimeZonesEnum Value)
	{
		ValueType = TIMEZONE_VALUE;
		IndexValue = Value;
		return *this;
	}
	
	s_SCInput_145& SetTimeWithTimeZone(int ValueTime, TimeZonesEnum ValueTimeZone)
	{
		ValueType = TIME_WITH_TIMEZONE_VALUE;
		DateTimeValue = COMBINE_DATE_TIME(0, ValueTime);
		TimeZoneValue = ValueTimeZone;
		return *this;
	}

	s_SCInput_145& SetString(const char* Value)
	{
		if (ValueType == STRING_VALUE && StringModified && StringValue != NULL)
			HeapFree(GetProcessHeap(), 0, (char*)StringValue);

		StringValue = NULL;

 		size_t StringLength = 0;
 		if (Value != NULL)
 			StringLength = strlen(Value);
 
 		if (StringLength != 0)
		{
			size_t NumBytes = StringLength + 1;

 			char* NewString = (char*)HeapAlloc(GetProcessHeap(), 8 /*HEAP_ZERO_MEMORY*/, NumBytes);

 			if (NewString != NULL)
			{
				#if __STDC_WANT_SECURE_LIB__
				strcpy_s(NewString, NumBytes, Value);
				#else
				strcpy(NewString, Value);
				#endif
				StringValue = NewString;
			}
		}

		ValueType = STRING_VALUE;
		StringModified = true;
		return *this;
	}

	void SetFloatLimits(float Min, float Max)
	{
		FloatValueMin = Min;
		FloatValueMax = Max;
	}
	
	void SetDateTimeLimits(SCDateTime Min, SCDateTime Max)
	{
		DateTimeValueMin = Min;
		DateTimeValueMax = Max;
	}
	
	void SetIntLimits(int Min, int Max)
	{
		IntValueMin = Min;
		IntValueMax = Max;
	}
	
	void SetDoubleLimits(double Min, double Max)
	{
		DoubleValueMin = Min;
		DoubleValueMax = Max;
	}
	
	void SetCustomInputStrings(const char* p_CustomStrings)
	{
		if(InternalSetCustomStrings)
			InternalSetCustomStrings(InputIndex, p_CustomStrings);
	}
	
	void SetDescription(const char* Description)
	{
		if(InternalSetDescription != NULL && Description != NULL)
			InternalSetDescription(InputIndex, Description);
	}
};

typedef c_ArrayWrapper<s_SCInput_145> SCInput145Array;
typedef s_SCInput_145& SCInputRef;

/****************************************************************************/

struct s_Parabolic
{ 
	s_Parabolic(SCBaseDataRef BaseDataIn, SCSubgraphRef Out, SCDateTimeArrayRef BaseDateTimeIn, int Index, float InStartAccelFactor, float InAccelIncrement, float InMaxAccelFactor, unsigned int InAdjustForGap, int InputDataHighIndex = SC_HIGH, int InputDataLowIndex = SC_LOW)
		:	 _BaseDataIn(BaseDataIn),
		_Out(Out),
		_BaseDateTimeIn(BaseDateTimeIn),
		_Index(Index),
		_InStartAccelFactor(InStartAccelFactor),
		_InAccelIncrement(InAccelIncrement),
		_InMaxAccelFactor(InMaxAccelFactor),
		_InAdjustForGap(InAdjustForGap),
		_InputDataHighIndex(InputDataHighIndex),
		_InputDataLowIndex(InputDataLowIndex)
	{
	}

	SCBaseDataRef _BaseDataIn;
	SCSubgraphRef _Out;
	SCDateTimeArrayRef _BaseDateTimeIn;
	int _Index;
	float _InStartAccelFactor;
	float _InAccelIncrement;
	float _InMaxAccelFactor;
	unsigned int _InAdjustForGap;
	int _InputDataHighIndex;
	int _InputDataLowIndex;

	inline float BaseDataHigh(int Index) 
	{ 
		//return max(_BaseDataIn[_InputDataHighIndex][Index], _BaseDataIn[_InputDataLowIndex][Index]); 
		return _BaseDataIn[_InputDataHighIndex][Index];

	}
	
	inline float BaseDataLow(int Index)  
	{ 
		//return min(_BaseDataIn[_InputDataHighIndex][Index], _BaseDataIn[_InputDataLowIndex][Index]); 
		return _BaseDataIn[_InputDataLowIndex][Index];
	}
};

/****************************************************************************/

struct s_NumericInformationGraphDrawTypeConfig
{ 
	COLORREF	TextBackgroundColor;
	bool		TransparentTextBackground;
	bool		LabelsOnRight;
	bool		AllowLabelOverlap;
	bool		DrawGridlines;
	int			GridlineStyleSubgraphIndex;
	int			FontSize;
	bool		ShowPullback;
	bool		IncludePriorBarCommonPriceVolume;
	bool		HideLabels;
	char		Reserved;
	int			ValueFormat[SC_SUBGRAPHS_AVAILABLE];
	int			SubgraphOrder[SC_SUBGRAPHS_AVAILABLE];

	s_NumericInformationGraphDrawTypeConfig()
	{
		TextBackgroundColor = COLOR_BLACK;
		TransparentTextBackground = true;
		LabelsOnRight = false;
		AllowLabelOverlap = false;
		DrawGridlines = true;
		GridlineStyleSubgraphIndex = -1;
		FontSize = 0;
		ShowPullback = false;
		IncludePriorBarCommonPriceVolume = false;
		HideLabels = false;
		Reserved = 0;
		for (int i=0; i<SC_SUBGRAPHS_AVAILABLE; i++)
		{
			ValueFormat[i] = VALUEFORMAT_INHERITED;
			SubgraphOrder[i] = i;
		}
	}
};

/****************************************************************************/

struct s_ACSOpenChartParameters
{
	int Version;
	int PriorChartNumber;  // This gets checked first, 0 is unknown
	
	ChartDataTypeEnum ChartDataType;
	SCString Symbol;
	
	IntradayBarPeriodTypeEnum IntradayBarPeriodType;
	int IntradayBarPeriodLength;
	
	SCDateTime SessionStartTime;
	SCDateTime SessionEndTime;
	SCDateTime EveningSessionStartTime;
	SCDateTime EveningSessionEndTime;
	
	// 864
	int DaysToLoad;

	// 916
	int IntradayBarPeriodParm2;  // used for IBPT_FLEX_RENKO_IN_TICKS - trend offset
	int IntradayBarPeriodParm3;  // used for IBPT_FLEX_RENKO_IN_TICKS - reversal offset

	// 933

	int IntradayBarPeriodParm4;  // used for IBPT_FLEX_RENKO_IN_TICKS - new bar when exceeded

	//1155
	int HistoricalChartBarPeriod;//default is  DAILY_DATA_PERIOD_DAYS


	//Version 3
	int ChartLinkNumber;

	s_ACSOpenChartParameters()
	{
		Reset();
	}

	void Reset()
	{
		Version = 3;
		PriorChartNumber = 0;
		ChartDataType = NO_DATA_TYPE;
		Symbol = "";
		IntradayBarPeriodType = IBPT_DAYS_MINS_SECS;
		IntradayBarPeriodLength = 0;
		SessionStartTime = -.1; //Flag indicating unset value
		SessionEndTime = -.1;
		EveningSessionStartTime = -.1;
		EveningSessionEndTime = -.1;
		DaysToLoad = 0;
		IntradayBarPeriodParm2 = 0;
		IntradayBarPeriodParm3 = 0;
		IntradayBarPeriodParm4 = 0;
		HistoricalChartBarPeriod = HISTORICAL_CHART_PERIOD_DAYS;
		ChartLinkNumber = 0;
	}
};

/****************************************************************************/

// Structure passed to custom DLL study functions.
// s_sc = structure _ sierra chart
struct s_sc
{
	/************************************************************************/
	// Functions (These do not expand the size of the structure)

	s_sc();//Only implemented internally within Sierra Chart

	//Non static functions
	int CompareOpenToHighLow(float Open, float High, float Low, int ValueFormat)
	{
		float OpenToHighDiff = High - Open;
		float OpenToLowDiff = Open - Low;

		if(FormattedEvaluate(OpenToHighDiff, ValueFormat, LESS_OPERATOR, OpenToLowDiff, ValueFormat))
			return 1; //Open closer to High
		else if(FormattedEvaluate(OpenToHighDiff, ValueFormat, GREATER_OPERATOR, OpenToLowDiff, ValueFormat))
			return -1; //Open closer to Low

		return 0; //must be equal distance

	}
	void AddMessageToLog(const char* Message, int ShowLog)
	{
		pAddMessageToLog(Message, ShowLog);
	}
	void AddMessageToLog(const SCString& Message, int ShowLog)
	{
		pAddMessageToLog(Message.GetChars(), ShowLog);
	}
	
	
	/*========================================================================
	
	BHCS_BAR_HAS_CLOSED: Element at BarIndex has closed. 

	BHCS_BAR_HAS_NOT_CLOSED: Element at BarIndex has not closed. 

	BHCS_SET_DEFAULTS: Configuration and defaults are being set.  Allow your SetDefaults code block to run.
	------------------------------------------------------------------------*/

	int GetBarHasClosedStatus()
	{
		return GetBarHasClosedStatus(Index);
	}

	int GetBarHasClosedStatus(int BarIndex)
	{
		if (SetDefaults)
			return BHCS_SET_DEFAULTS;

		if (BarIndex != ArraySize - 1)
			return BHCS_BAR_HAS_CLOSED;
		else
			return BHCS_BAR_HAS_NOT_CLOSED;
	}

	float GetHighest(SCFloatArrayRef In, int Index, int Length)
	{
		float High = -FLT_MAX;
		
		// Get the high starting at Index going back by Length in the In array
		for (int SrcIndex = Index; SrcIndex > Index - Length; --SrcIndex)
		{
			if (SrcIndex < 0 || SrcIndex >= In.GetArraySize())
				continue;
			
			if (In[SrcIndex] > High)
				High = In[SrcIndex];
		}
		
		return High;
	}

	float GetHighest(SCFloatArrayRef In, int Length)
	{
		return GetHighest( In, Index,  Length);
	}

	float GetLowest(SCFloatArrayRef In, int Index, int Length)
	{
		float Low = FLT_MAX;
		
		// Get the low starting at Index going back by Length in the In array
		for (int SrcIndex = Index; SrcIndex > Index - Length; --SrcIndex)
		{
			if (SrcIndex < 0 || SrcIndex >= In.GetArraySize())
				continue;
			
			if (In[SrcIndex] < Low)
				Low = In[SrcIndex];
		}
		
		return Low;
	}
	float GetLowest(SCFloatArrayRef In,int Length)
	{
		return GetLowest( In, Index, Length);
	}

	int CrossOver(SCFloatArrayRef First, SCFloatArrayRef Second)
	{
		return CrossOver( First,  Second,  Index);
	}
	
	int CrossOver(SCFloatArrayRef First, SCFloatArrayRef Second, int Index)
	{
		float X1 = First[Index-1];
		float X2 = First[Index];
		float Y1 = Second[Index-1];
		float Y2 = Second[Index];

		if (X2 != Y2)  // The following is not useful if X2 and Y2 are equal
		{
			// Find non-equal values for prior values
			int PriorIndex = Index - 1;
			while (X1 == Y1 && PriorIndex > 0 && PriorIndex > Index - 100)
			{
				--PriorIndex;
				X1 = First[PriorIndex];
				Y1 = Second[PriorIndex];
			}
		}

		if (X1 > Y1 && X2 < Y2)
			return CROSS_FROM_TOP;
		else if (X1 < Y1 && X2 > Y2)
			return CROSS_FROM_BOTTOM;
		else
			return NO_CROSS;
	}
	
	int ResizeArrays(int NewSize)
	{
		return p_ResizeArrays(NewSize);
	}
	

	int AddElements(int NumElements)
	{
		return p_AddElements(NumElements);
	}

	int FormattedEvaluate(float Value1, unsigned int Value1Format,
		OperatorEnum Operator,
		float Value2, unsigned int Value2Format,
		float PrevValue1 = 0.0f, float PrevValue2 = 0.0f,
		int* CrossDirection = NULL)
	{
		return InternalFormattedEvaluate(Value1, Value1Format, Operator, Value2, Value2Format, PrevValue1, PrevValue2, CrossDirection)?1:0;
	}

	int FormattedEvaluateUsingDoubles(double Value1, unsigned int Value1Format,
		OperatorEnum Operator,
		double Value2, unsigned int Value2Format,
		double PrevValue1 = 0.0f, double PrevValue2 = 0.0f,
		int* CrossDirection = NULL)
	{
		return InternalFormattedEvaluateUsingDoubles(Value1, Value1Format, Operator, Value2, Value2Format, PrevValue1, PrevValue2, CrossDirection)?1:0;
	}

	int PlaySound(int AlertNumber)
	{
		return InternalPlaySound(AlertNumber, "", 0 );
	}
	
	int PlaySound(int AlertNumber, const SCString& AlertMessage, int ShowAlertLog = 0)
	{	
		return InternalPlaySound(AlertNumber, AlertMessage.GetChars(), ShowAlertLog);
	}
	
	int PlaySound(int AlertNumber, const char* AlertMessage, int ShowAlertLog = 0)
	{	
		return InternalPlaySound(AlertNumber, AlertMessage, ShowAlertLog);
	}
	
	int PlaySound(const char* AlertPathAndFileName, int NumberTimesPlayAlert = 1)
	{
		return InternalPlaySoundPath(AlertPathAndFileName, NumberTimesPlayAlert, "", 0);
	}

	int PlaySound(const SCString& AlertPathAndFileName, int NumberTimesPlayAlert = 1)
	{
		return InternalPlaySoundPath(AlertPathAndFileName.GetChars(), NumberTimesPlayAlert, "", 0);
	}

	int PlaySound( SCString& AlertPathAndFileName, int NumberTimesPlayAlert, SCString& AlertMessage, int ShowAlertLog = 0)
	{
		return InternalPlaySoundPath(AlertPathAndFileName.GetChars(), NumberTimesPlayAlert, AlertMessage.GetChars(), ShowAlertLog);
	}

	int PlaySound(const char* AlertPathAndFileName, int NumberTimesPlayAlert, const char* AlertMessage, int ShowAlertLog = 0)
	{
		return InternalPlaySoundPath(AlertPathAndFileName, NumberTimesPlayAlert, AlertMessage, ShowAlertLog);
	}

	void AddAlertLine(const SCString& Message, int ShowAlertLog = 0)
	{
		InternalAddAlertLine(Message.GetChars(), ShowAlertLog);
	}

	void AddAlertLine(const char* Message, int ShowAlertLog = 0)
	{
		InternalAddAlertLine(Message, ShowAlertLog);
	}

	SCString GetStudyName(int StudyIndex)
	{
		SCString Temp;
		Temp = InternalGetStudyName(StudyIndex);
		return Temp;		
	}
	
	SCFloatArrayRef CCI(SCFloatArrayRef In, SCFloatArrayRef SMAOut, SCFloatArrayRef CCIOut, int Index, int Length, float Multiplier, unsigned int MovingAverageType = MOVAVGTYPE_SIMPLE)
	{
		return InternalCCI(In, SMAOut, CCIOut, Index, Length, Multiplier, MovingAverageType);
	}
	
	SCFloatArrayRef CCI(SCFloatArrayRef In, SCFloatArrayRef SMAOut, SCFloatArrayRef CCIOut, int Length, float Multiplier, unsigned int MovingAverageType = MOVAVGTYPE_SIMPLE)
	{
		return InternalCCI(In, SMAOut, CCIOut, Index, Length, Multiplier, MovingAverageType);
	}

	SCFloatArrayRef CCI(SCFloatArrayRef In, SCSubgraphRef Out, int Index, int Length, float Multiplier, unsigned int MovingAverageType = MOVAVGTYPE_SIMPLE)
	{
		return InternalCCI(In, Out.Arrays[0], Out, Index, Length, Multiplier, MovingAverageType);
	}

	SCFloatArrayRef CCI(SCFloatArrayRef In, SCSubgraphRef Out, int Length, float Multiplier, unsigned int MovingAverageType = MOVAVGTYPE_SIMPLE)
	{
		return InternalCCI(In, Out.Arrays[0], Out, Index, Length, Multiplier, MovingAverageType);
	}

    SCFloatArrayRef RSI(SCFloatArrayRef In, SCSubgraphRef RsiOut, int Index, unsigned int MovingAverageType, int Length)
    {      
        SCFloatArrayRef UpSumsTemp = RsiOut.Arrays[0];
        SCFloatArrayRef DownSumsTemp = RsiOut.Arrays[1];
        SCFloatArrayRef SmoothedUpSumsTemp = RsiOut.Arrays[2];
        SCFloatArrayRef SmoothedDownSumsTemp = RsiOut.Arrays[3];

        return InternalRSI(In, RsiOut, 
                           UpSumsTemp, DownSumsTemp, SmoothedUpSumsTemp, SmoothedDownSumsTemp, 
                           Index, MovingAverageType, Length);
    };


    SCFloatArrayRef RSI(SCFloatArrayRef In, SCSubgraphRef RsiOut, unsigned int MovingAverageType, int Length)
    {      
        SCFloatArrayRef UpSumsTemp = RsiOut.Arrays[0];
        SCFloatArrayRef DownSumsTemp = RsiOut.Arrays[1];
        SCFloatArrayRef SmoothedUpSumsTemp = RsiOut.Arrays[2];
        SCFloatArrayRef SmoothedDownSumsTemp = RsiOut.Arrays[3];

        return InternalRSI(In, RsiOut, 
                           UpSumsTemp, DownSumsTemp, SmoothedUpSumsTemp, SmoothedDownSumsTemp, 
                           Index, MovingAverageType, Length);
    };


	SCFloatArrayRef RSI(SCFloatArrayRef In, SCFloatArrayRef RsiOut, 
				         SCFloatArrayRef UpSumsTemp, SCFloatArrayRef DownSumsTemp, 
						  SCFloatArrayRef SmoothedUpSumsTemp, SCFloatArrayRef SmoothedDownSumsTemp, 
						  int Index, unsigned int MovingAverageType, int Length)
	{
        return InternalRSI(In, RsiOut, 
                           UpSumsTemp, DownSumsTemp, SmoothedUpSumsTemp, SmoothedDownSumsTemp, 
                           Index, MovingAverageType, Length);
	}


	SCFloatArrayRef RSI(SCFloatArrayRef In, SCFloatArrayRef RsiOut, 
				         SCFloatArrayRef UpSumsTemp, SCFloatArrayRef DownSumsTemp, 
						  SCFloatArrayRef SmoothedUpSumsTemp, SCFloatArrayRef SmoothedDownSumsTemp, 
						  unsigned int MovingAverageType, int Length)
	{
        return InternalRSI(In, RsiOut, 
                           UpSumsTemp, DownSumsTemp, SmoothedUpSumsTemp, SmoothedDownSumsTemp, 
                           Index, MovingAverageType, Length);
	}

    void DMI(SCBaseDataRef BaseDataIn, SCSubgraphRef PosDMIOut, SCSubgraphRef NegDMIOut, int Index, int Length)
    {
		SCFloatArrayRef InternalPosDM = PosDMIOut.Arrays[0];
        SCFloatArrayRef InternalNegDM = PosDMIOut.Arrays[1];
        SCFloatArrayRef InternalTrueRangeSummation = PosDMIOut.Arrays[2];
		 SCFloatArrayRef DiffDMIOut = PosDMIOut.Arrays[3];

        InternalDMI(BaseDataIn,Index, Length, 1 /* rounding disabled */,
                           PosDMIOut, NegDMIOut, DiffDMIOut, 
                           InternalTrueRangeSummation, InternalPosDM, InternalNegDM);
		return;
    }

	void DMI(SCBaseDataRef BaseDataIn, SCSubgraphRef PosDMIOut, SCSubgraphRef NegDMIOut, int Length)
    {
		SCFloatArrayRef InternalPosDM = PosDMIOut.Arrays[0];
        SCFloatArrayRef InternalNegDM = PosDMIOut.Arrays[1];
        SCFloatArrayRef InternalTrueRangeSummation = PosDMIOut.Arrays[2];
		 SCFloatArrayRef DiffDMIOut = PosDMIOut.Arrays[3];

        InternalDMI(BaseDataIn,Index, Length, 1 /* rounding disabled */,
                           PosDMIOut, NegDMIOut, DiffDMIOut, 
                           InternalTrueRangeSummation, InternalPosDM, InternalNegDM);
		return;
    }

	SCSubgraphRef DMI(SCBaseDataRef BaseDataIn, SCSubgraphRef Out, int Index, int Length)
    {
        SCFloatArrayRef InternalPosDM = Out.Arrays[1];
        SCFloatArrayRef InternalNegDM = Out.Arrays[2];
        SCFloatArrayRef InternalTrueRangeSummation = Out.Arrays[3];
		 SCFloatArrayRef DiffDMIOut = Out.Arrays[4];

        InternalDMI(BaseDataIn,Index, Length, 1 /* rounding disabled */,
			Out, Out.Arrays[0], DiffDMIOut, 
                           InternalTrueRangeSummation, InternalPosDM, InternalNegDM);
		return Out;
    }

	SCSubgraphRef DMI(SCBaseDataRef BaseDataIn, SCSubgraphRef Out, int Length)
    {
        SCFloatArrayRef InternalPosDM = Out.Arrays[1];
        SCFloatArrayRef InternalNegDM = Out.Arrays[2];
        SCFloatArrayRef InternalTrueRangeSummation = Out.Arrays[3];
		 SCFloatArrayRef DiffDMIOut = Out.Arrays[4];

        InternalDMI(BaseDataIn,Index, Length, 1 /* rounding disabled */,
			Out, Out.Arrays[0], DiffDMIOut, 
                           InternalTrueRangeSummation, InternalPosDM, InternalNegDM);
		return Out;
    }



    SCFloatArrayRef DMIDiff(SCBaseDataRef BaseDataIn, SCSubgraphRef Out, int Index, int Length)
    {        
        SCFloatArrayRef InternalTrueRangeSummation = Out.Arrays[0];
        SCFloatArrayRef InternalPosDM = Out.Arrays[1];
        SCFloatArrayRef InternalNegDM = Out.Arrays[2];        

        return InternalDMIDiff(BaseDataIn, Index, Length, Out, 
                               InternalTrueRangeSummation, InternalPosDM, InternalNegDM);
    }

   
    SCFloatArrayRef DMIDiff(SCBaseDataRef BaseDataIn, SCSubgraphRef Out, int Length)
    {        
        SCFloatArrayRef InternalTrueRangeSummation = Out.Arrays[0];
        SCFloatArrayRef InternalPosDM = Out.Arrays[1];
        SCFloatArrayRef InternalNegDM = Out.Arrays[2];        

        return InternalDMIDiff(BaseDataIn, Index, Length, Out, 
                               InternalTrueRangeSummation, InternalPosDM, InternalNegDM);
    }

    SCFloatArrayRef ADX(SCBaseDataRef BaseDataIn, SCSubgraphRef Out, int Index, int DXLength, int DXMovAvgLength)
    {
        SCFloatArrayRef InternalTrueRangeSummation = Out.Arrays[0];
        SCFloatArrayRef InternalPosDM = Out.Arrays[1];
        SCFloatArrayRef InternalNegDM  = Out.Arrays[2];
        SCFloatArrayRef InternalDX  = Out.Arrays[3];

        return InternalADX(BaseDataIn, Index, DXLength, DXMovAvgLength, Out, 
                           InternalTrueRangeSummation, InternalPosDM, InternalNegDM, InternalDX);
    }

    
    SCFloatArrayRef ADX(SCBaseDataRef BaseDataIn, SCSubgraphRef Out, int DXLength, int DXMovAvgLength)
    {
        SCFloatArrayRef InternalTrueRangeSummation = Out.Arrays[0];
        SCFloatArrayRef InternalPosDM = Out.Arrays[1];
        SCFloatArrayRef InternalNegDM  = Out.Arrays[2];
        SCFloatArrayRef InternalDX  = Out.Arrays[3];

        return InternalADX(BaseDataIn, Index, DXLength, DXMovAvgLength, Out, 
                           InternalTrueRangeSummation, InternalPosDM, InternalNegDM, InternalDX);
    }

    SCFloatArrayRef ADXR(SCBaseDataRef BaseDataIn, SCSubgraphRef Out, int Index,
                         int DXLength, int DXMovAvgLength, int ADXRInterval)
    {
        SCFloatArrayRef InternalTrueRangeSummation = Out.Arrays[0];
        SCFloatArrayRef InternalPosDM  = Out.Arrays[1];
        SCFloatArrayRef InternalNegDM  = Out.Arrays[2];
        SCFloatArrayRef InternalDX = Out.Arrays[3];
        SCFloatArrayRef InternalADX = Out.Arrays[4];

        return InternalADXR(BaseDataIn, Index, DXLength, DXMovAvgLength, ADXRInterval, Out, 
                            InternalTrueRangeSummation, InternalPosDM, InternalNegDM, InternalDX, InternalADX);
    }

    SCFloatArrayRef ADXR(SCBaseDataRef BaseDataIn, SCSubgraphRef Out,
                         int DXLength, int DXMovAvgLength, int ADXRInterval)
    {
        SCFloatArrayRef InternalTrueRangeSummation = Out.Arrays[0];
        SCFloatArrayRef InternalPosDM  = Out.Arrays[1];
        SCFloatArrayRef InternalNegDM  = Out.Arrays[2];
        SCFloatArrayRef InternalDX = Out.Arrays[3];
        SCFloatArrayRef InternalADX = Out.Arrays[4];

        return InternalADXR(BaseDataIn, Index, DXLength, DXMovAvgLength, ADXRInterval, Out, 
                            InternalTrueRangeSummation, InternalPosDM, InternalNegDM, InternalDX, InternalADX);
    }

	SCFloatArrayRef Ergodic(SCFloatArrayRef In, SCSubgraphRef Out, int Index, int LongEMALength, int ShortEMALength, float Multiplier)
	{
		return Internal_Ergodic( In,  Out,  Index,  LongEMALength,  ShortEMALength,  Multiplier,
			Out.Arrays[0],  Out.Arrays[1],  Out.Arrays[2],  Out.Arrays[3],  Out.Arrays[4],  Out.Arrays[5]);
	}

	SCFloatArrayRef Ergodic(SCFloatArrayRef In, SCSubgraphRef Out, int LongEMALength, int ShortEMALength, float Multiplier)
	{
		return Internal_Ergodic( In,  Out,  Index,  LongEMALength,  ShortEMALength,  Multiplier,
			Out.Arrays[0],  Out.Arrays[1],  Out.Arrays[2],  Out.Arrays[3],  Out.Arrays[4],  Out.Arrays[5]);
	}

	
	SCFloatArrayRef Keltner(SCBaseDataRef BaseDataIn, SCFloatArrayRef In, SCSubgraphRef Out, int Index, int KeltnerMALength, unsigned int KeltnerMAType, int TrueRangeMALength, unsigned int TrueRangeMAType, float TopBandMultiplier, float BottomBandMultiplier)
	{
		return Internal_Keltner( BaseDataIn,  In,  Out, Out.Arrays[0], Out.Arrays[1], Index,  KeltnerMALength, KeltnerMAType,  TrueRangeMALength, TrueRangeMAType,  TopBandMultiplier, BottomBandMultiplier,  Out.Arrays[2], Out.Arrays[3]);
	}

	SCFloatArrayRef Keltner(SCBaseDataRef BaseDataIn, SCFloatArrayRef In, SCSubgraphRef Out, int KeltnerMALength, unsigned int KeltnerMAType, int TrueRangeMALength, unsigned int TrueRangeMAType, float TopBandMultiplier, float BottomBandMultiplier)
	{
		return Internal_Keltner( BaseDataIn,  In,  Out, Out.Arrays[0], Out.Arrays[1], Index,  KeltnerMALength, KeltnerMAType,  TrueRangeMALength, TrueRangeMAType,  TopBandMultiplier, BottomBandMultiplier,  Out.Arrays[2], Out.Arrays[3]);
	}

	
	SCFloatArrayRef  SmoothedMovingAverage(SCFloatArrayRef In, SCFloatArrayRef Out, int Index, int Length)
	{
		return Internal_SmoothedMovingAverage( In,  Out,  Index,  Length, 0);
	}
	
	SCFloatArrayRef  SmoothedMovingAverage(SCFloatArrayRef In, SCFloatArrayRef Out, int Length)
	{
		return Internal_SmoothedMovingAverage( In,  Out,  Index,  Length, 0);
	}

	SCFloatArrayRef SmoothedMovingAverage(SCFloatArrayRef In, SCFloatArrayRef Out, int Index, int Length, int Offset)
	{
		return Internal_SmoothedMovingAverage( In,  Out,  Index,  Length, Offset);
	}

	
	SCFloatArrayRef ATR(SCBaseDataRef BaseDataIn, SCSubgraphRef Out, int Index, int Length, unsigned int MovingAverageType)
	{
		return InternalATR( BaseDataIn,  Out.Arrays[0],  Out,  Index,  Length, MovingAverageType);
	}

	SCFloatArrayRef ATR(SCBaseDataRef BaseDataIn, SCSubgraphRef Out, int Length, unsigned int MovingAverageType)
	{
		return InternalATR( BaseDataIn,  Out.Arrays[0],  Out,  Index,  Length, MovingAverageType);
	}

	SCFloatArrayRef ATR(SCBaseDataRef BaseDataIn, SCFloatArrayRef TROut, SCFloatArrayRef ATROut, int Index, int Length, unsigned int MovingAverageType)
	{
		return InternalATR( BaseDataIn,  TROut,  ATROut,  Index,  Length, MovingAverageType);
	}

	SCFloatArrayRef ATR(SCBaseDataRef BaseDataIn, SCFloatArrayRef TROut, SCFloatArrayRef ATROut, int Length, unsigned int MovingAverageType)
	{
		return InternalATR( BaseDataIn,  TROut,  ATROut,  Index,  Length, MovingAverageType);
	}

	SCFloatArrayRef Stochastic(SCBaseDataRef BaseDataIn, SCSubgraphRef Out, int Index, int FastKLength, int FastDLength, int SlowDLength, unsigned int MovingAverageType)
	{
		InternalStochastic( BaseDataIn,  Out,  Out.Arrays[0],  Out.Arrays[1],  Index,  FastKLength,  FastDLength,  SlowDLength, MovingAverageType);
		return Out;
	}
	
	SCFloatArrayRef Stochastic(SCBaseDataRef BaseDataIn, SCSubgraphRef Out, int FastKLength, int FastDLength, int SlowDLength, unsigned int MovingAverageType)
	{
		InternalStochastic( BaseDataIn,  Out,  Out.Arrays[0],  Out.Arrays[1],  Index,  FastKLength,  FastDLength,  SlowDLength, MovingAverageType);
		return Out;
	}

	//Three Separate Input Arrays
	SCFloatArrayRef Stochastic(SCFloatArrayRef InputDataHigh, SCFloatArrayRef InputDataLow, SCFloatArrayRef InputDataLast, SCSubgraphRef Out, int Index, int FastKLength, int FastDLength, int SlowDLength, unsigned int MovingAverageType)
	{
		InternalStochastic2( InputDataHigh, InputDataLow, InputDataLast,  Out,  Out.Arrays[0],  Out.Arrays[1],  Index,  FastKLength,  FastDLength,  SlowDLength, MovingAverageType);
		return Out;
	}

	//Three Separate Input Arrays
	SCFloatArrayRef Stochastic(SCFloatArrayRef InputDataHigh, SCFloatArrayRef InputDataLow, SCFloatArrayRef InputDataLast, SCSubgraphRef Out, int FastKLength, int FastDLength, int SlowDLength, unsigned int MovingAverageType)
	{
		InternalStochastic2( InputDataHigh, InputDataLow, InputDataLast,  Out,  Out.Arrays[0],  Out.Arrays[1],  Index,  FastKLength,  FastDLength,  SlowDLength, MovingAverageType);
		return Out;
	}

	SCFloatArrayRef MACD(SCFloatArrayRef In, SCSubgraphRef Out, int Index, int FastMALength, int SlowMALength, int MACDMALength, int MovAvgType)
	{
		return InternalMACD( In, Out.Arrays[0], Out.Arrays[1], Out.Data, Out.Arrays[2], Out.Arrays[3], Index, FastMALength, SlowMALength, MACDMALength, MovAvgType);
	}

	SCFloatArrayRef MACD(SCFloatArrayRef In, SCSubgraphRef Out, int FastMALength, int SlowMALength, int MACDMALength, int MovAvgType)
	{
		return InternalMACD( In, Out.Arrays[0], Out.Arrays[1], Out.Data, Out.Arrays[2], Out.Arrays[3], Index, FastMALength, SlowMALength, MACDMALength, MovAvgType);
	}
	

	SCFloatArrayRef HullMovingAverage(SCFloatArrayRef In, SCSubgraphRef Out, int Index, int Length)
	{
		return InternalHullMovingAverage( In,  Out,  Out.Arrays[0], Out.Arrays[1], Out.Arrays[2],  Index,  Length);
	}

	SCFloatArrayRef HullMovingAverage(SCFloatArrayRef In, SCSubgraphRef Out, int Length)
	{
		return InternalHullMovingAverage( In,  Out,  Out.Arrays[0], Out.Arrays[1], Out.Arrays[2],  Index,  Length);
	}

	SCFloatArrayRef TriangularMovingAverage(SCFloatArrayRef In, SCSubgraphRef Out, int Index, int Length)
	{
		return InternalTriangularMovingAverage( In, Out, Out.Arrays[0], Index, Length);
	}

	SCFloatArrayRef TriangularMovingAverage(SCFloatArrayRef In, SCSubgraphRef Out, int Length)
	{
		return InternalTriangularMovingAverage( In, Out, Out.Arrays[0], Index, Length);
	}

	SCFloatArrayRef TEMA(SCFloatArrayRef In, SCSubgraphRef Out, int Index, int  Length)
	{
		return InternalTEMA( In,  Out, Out.Arrays[0],Out.Arrays[1],Out.Arrays[2],  Index,   Length);
	}
	
	SCFloatArrayRef TEMA(SCFloatArrayRef In, SCSubgraphRef Out, int  Length)
	{
		return InternalTEMA( In,  Out, Out.Arrays[0],Out.Arrays[1],Out.Arrays[2],  Index,   Length);
	}

	SCFloatArrayRef BollingerBands(SCFloatArrayRef In, SCSubgraphRef Out, int Index, int  Length, float Multiplier, int MovingAverageType)
	{
		InternalBollingerBands(In, Out,  Out.Arrays[0], Out.Arrays[1], Out.Arrays[2], Index, Length, Multiplier, MovingAverageType);
		return Out;
	}

	SCFloatArrayRef BollingerBands(SCFloatArrayRef In, SCSubgraphRef Out, int  Length, float Multiplier, int MovingAverageType)
	{
		InternalBollingerBands(In, Out,  Out.Arrays[0], Out.Arrays[1], Out.Arrays[2], Index, Length, Multiplier, MovingAverageType);
		return Out;
	}


	SCFloatArrayRef WeightedMovingAverage(SCFloatArrayRef In, SCFloatArrayRef Out, int Index, int Length)
	{
		return InternalWeightedMovingAverage( In,  Out,  Index,  Length);
	}
	
	SCFloatArrayRef WeightedMovingAverage(SCFloatArrayRef In, SCFloatArrayRef Out, int Length)
	{
		return InternalWeightedMovingAverage( In,  Out,  Index,  Length);
	}

	SCFloatArrayRef Momentum(SCFloatArrayRef In, SCFloatArrayRef Out, int Index, int Length)
	{
		return InternalMomentum(In,Out,Index,Length);
	}

	SCFloatArrayRef Momentum(SCFloatArrayRef In, SCFloatArrayRef Out, int Length)
	{
		return InternalMomentum(In,Out,Index,Length);
	}
	
	SCFloatArrayRef TRIX(SCFloatArrayRef In, SCSubgraphRef Out, int Index, int Length)
	{
		return InternalTRIX(In, Out, Out.Arrays[0], Out.Arrays[1], Out.Arrays[2], Index, Length);
	}

	SCFloatArrayRef TRIX(SCFloatArrayRef In, SCSubgraphRef Out, int Length)
	{
		return InternalTRIX(In, Out, Out.Arrays[0], Out.Arrays[1], Out.Arrays[2], Index, Length);
	}

	SCFloatArrayRef AroonIndicator(SCFloatArrayRef FloatArrayInHigh, SCFloatArrayRef FloatArrayInLow, SCSubgraphRef Out, int Index, int Length)
	{
		return Internal_AroonIndicator( FloatArrayInHigh, FloatArrayInLow,  Out.Data, Out.Arrays[0], Out.Arrays[1], Index,  Length);
	}

	SCFloatArrayRef AroonIndicator(SCFloatArrayRef FloatArrayInHigh, SCFloatArrayRef FloatArrayInLow, SCSubgraphRef Out, int Length)
	{
			return Internal_AroonIndicator( FloatArrayInHigh, FloatArrayInLow,  Out.Data, Out.Arrays[0], Out.Arrays[1], Index,  Length);
	}

	SCFloatArrayRef Demarker(SCBaseDataRef BaseDataIn, SCSubgraphRef Out, int Index, int Length)
	{
		return Internal_Demarker( BaseDataIn,  Out, Out.Arrays[1], Out.Arrays[3], Out.Arrays[2], Out.Arrays[4], Index,  Length);
	}

	SCFloatArrayRef Demarker(SCBaseDataRef BaseDataIn, SCSubgraphRef Out, int Length)
	{
		return Internal_Demarker( BaseDataIn,  Out, Out.Arrays[1], Out.Arrays[3], Out.Arrays[2], Out.Arrays[4], Index,  Length);
	}

	
	int GetOHLCOfTimePeriod(SCDateTime StartDateTime, SCDateTime EndDateTime, float& Open, float& High, float& Low, float& Close, float& NextOpen, int& NumberOfBars,SCDateTime& TotalTimeSpan)
	{	
		s_GetOHLCOfTimePeriod Data(StartDateTime, EndDateTime, Open, High, Low, Close, NextOpen, NumberOfBars, TotalTimeSpan);

		return InternalGetOHLCOfTimePeriod( Data );

	}

	int GetOHLCOfTimePeriod(SCDateTime StartDateTime, SCDateTime EndDateTime, float& Open, float& High, float& Low, float& Close, float& NextOpen)
	{
		int NumberOfBars;
		SCDateTime TotalTimeSpan;

		s_GetOHLCOfTimePeriod Data(StartDateTime, EndDateTime, Open, High, Low, Close, NextOpen, NumberOfBars, TotalTimeSpan);

		return InternalGetOHLCOfTimePeriod( Data );

	}

	int GetOHLCForDate(SCDateTime Date, float& Open, float& High, float& Low, float& Close)
	{
		return Internal_GetOHLCForDate(Date, Open, High, Low, Close);
	}

	int AreTimeSpecificBars()
	{
		if(NumberOfTradesPerBar == 0 && VolumePerBar == 0 && RangeBarValue == 0 && RenkoTicksPerBar == 0 &&  ReversalTicksPerBar==0)
			return 1;
		
		return 0;
	}
	int AreRangeBars()
	{
		if(RangeBarValue)
			return 1;
		
		return 0;
	}
	int AreNumberOfTradesBars()
	{
		if(NumberOfTradesPerBar)
			return 1;
		
		return 0;
	}
	int AreVolumeBars()
	{
		if(VolumePerBar)
			return 1;
		
		return 0;
	}
	int AreRenkoBars()
	{
		if (RenkoTicksPerBar)
			return 1;

		return 0;
	}
	int AreReversalBars()
	{
		if (ReversalTicksPerBar)
			return 1;

		return 0;
	}
	int IsReplayRunning()
	{
		if(ReplayStatus)
			return 1;
	
		return 0;
	}
	
	int GetExactMatchForSCDateTime(int ChartNumber, const SCDateTime& DateTime)
	{
		int Index = GetContainingIndexForSCDateTime(ChartNumber, DateTime);
		
		SCDateTimeArray ChartDateTimeArray;
		GetChartDateTimeArray(ChartNumber, ChartDateTimeArray);
		
		if (ChartDateTimeArray[Index] == DateTime)
			return Index;
		else
			return -1;
	}
	
	SCFloatArrayRef Highest(SCFloatArrayRef In, SCFloatArrayRef Out, int Index, int Length)
	{
		return InternalHighest( In,  Out,  Index,  Length);
	}
	
	SCFloatArrayRef Highest(SCFloatArrayRef In, SCFloatArrayRef Out, int Length)
	{
		return InternalHighest( In,  Out,  Index,  Length);
	}


	SCFloatArrayRef Lowest(SCFloatArrayRef In, SCFloatArrayRef Out, int Index, int Length)
	{
		return InternalLowest( In,  Out,  Index,  Length);
	}

	SCFloatArrayRef Lowest(SCFloatArrayRef In, SCFloatArrayRef Out, int Length)
	{
		return InternalLowest( In,  Out,  Index,  Length);
	}
	
	SCFloatArrayRef ChaikinMoneyFlow(SCBaseDataRef BaseDataIn, SCSubgraphRef Out, int Index, int Length)
	{
		return InternalChaikinMoneyFlow(BaseDataIn, Out, Out.Arrays[0], Index, Length);
	}
	
	SCFloatArrayRef ChaikinMoneyFlow(SCBaseDataRef BaseDataIn, SCSubgraphRef Out, int Length)
	{
		return InternalChaikinMoneyFlow(BaseDataIn, Out, Out.Arrays[0], Index, Length);
	}
	
	SCFloatArrayRef EnvelopePct(SCFloatArrayRef In, SCSubgraphRef Out, float pct, int Index)
	{
		return InternalEnvelopePercent(In, Out, Out.Arrays[0], pct, Index);
	}
	
	SCFloatArrayRef EnvelopePct(SCFloatArrayRef In, SCSubgraphRef Out, float pct)
	{
		return InternalEnvelopePercent(In, Out, Out.Arrays[0], pct, Index);
	}
	
	SCFloatArrayRef EnvelopeFixed(SCFloatArrayRef In, SCSubgraphRef Out, float FixedValue, int Index)
	{
		return InternalEnvelopeFixed(In, Out, Out.Arrays[0], FixedValue, Index);
	}
	
	SCFloatArrayRef EnvelopeFixed(SCFloatArrayRef In, SCSubgraphRef Out, float FixedValue)
	{
		return InternalEnvelopeFixed(In, Out, Out.Arrays[0], FixedValue, Index);
	}

	SCFloatArrayRef RandomWalkIndicator(SCBaseDataRef In, SCSubgraphRef Out, int Index, int Length)
	{
		return InternalRWI(In, Out, Out.Arrays[0], Out.Arrays[1], Out.Arrays[2], Out.Arrays[3], Index, Length);
	}
	
	SCFloatArrayRef RandomWalkIndicator(SCBaseDataRef In, SCSubgraphRef Out, int Length)
	{
		return InternalRWI(In, Out, Out.Arrays[0], Out.Arrays[1], Out.Arrays[2], Out.Arrays[3], Index, Length);
	}
	
	SCFloatArrayRef UltimateOscillator(SCBaseDataRef In, SCSubgraphRef Out1, SCSubgraphRef Out2,int Index, int Length1, int Length2, int Length3)
	{
		return InternalUltimateOscillator( In,  
			Out1, 
			Out1.Arrays[0], 
			Out1.Arrays[1], 
			Out1.Arrays[2], 
			Out1.Arrays[3],  
			Out1.Arrays[4],  
			Out1.Arrays[5],  
			Out1.Arrays[6],  
			Out1.Arrays[7],  
			Out1.Arrays[8],  
			Out2.Arrays[0], 
			Out2.Arrays[1], 
			Out2.Arrays[2], 
			Out2.Arrays[3], 
			Index, Length1, Length2, Length3);
	}
	
	SCFloatArrayRef UltimateOscillator(SCBaseDataRef In, SCSubgraphRef Out1, SCSubgraphRef Out2, int Length1, int Length2, int Length3)
	{
		return InternalUltimateOscillator( In,  
			Out1, 
			Out1.Arrays[0], 
			Out1.Arrays[1], 
			Out1.Arrays[2], 
			Out1.Arrays[3],  
			Out1.Arrays[4],  
			Out1.Arrays[5],  
			Out1.Arrays[6],  
			Out1.Arrays[7],  
			Out1.Arrays[8],  
			Out2.Arrays[0], 
			Out2.Arrays[1], 
			Out2.Arrays[2], 
			Out2.Arrays[3], 
			Index, Length1, Length2, Length3);
	}
	
	SCFloatArrayRef Oscillator(SCFloatArrayRef In1, SCFloatArrayRef In2, SCFloatArrayRef Out, int Index)
	{
		return InternalOscillator(In1, In2, Out, Index);
	}
	
	SCFloatArrayRef Oscillator(SCFloatArrayRef In1, SCFloatArrayRef In2, SCFloatArrayRef Out)
	{
		return InternalOscillator(In1, In2, Out, Index);
	}
	
	int UseTool(s_UseTool& ToolDetails)
	{
		return Internal_UseToolEx(ToolDetails);
	}
	
	SCFloatArrayRef LinearRegressionIndicatorAndStdErr(SCFloatArrayRef In, SCFloatArrayRef Out, SCFloatArrayRef StdErr, int Index, int Length)
	{
		return  InternalLinearRegressionIndicatorAndStdErr( In,  Out,  StdErr, Index, Length);
	}
	
	SCFloatArrayRef LinearRegressionIndicatorAndStdErr(SCFloatArrayRef In, SCFloatArrayRef Out, SCFloatArrayRef StdErr, int Length)
	{
		return InternalLinearRegressionIndicatorAndStdErr( In,  Out,  StdErr, Index, Length);
	}
	
	SCFloatArrayRef PriceVolumeTrend(SCBaseDataRef BaseDataIn, SCFloatArrayRef Out, int Index)
	{
		return InternalPriceVolumeTrend( BaseDataIn,  Out,  Index); 
	}
	
	SCFloatArrayRef PriceVolumeTrend(SCBaseDataRef BaseDataIn, SCFloatArrayRef Out)
	{
		return InternalPriceVolumeTrend( BaseDataIn,  Out,  Index); 
	}
	
	int NumberOfBarsSinceLowestValue(SCFloatArrayRef in, int Index, int Length)
	{
		return InternalNumberOfBarsSinceLowestValue( in,  Index,  Length);
	}
	int NumberOfBarsSinceLowestValue(SCFloatArrayRef in, int Length)
	{
		return InternalNumberOfBarsSinceLowestValue( in,  Index,  Length);
	}
	
	int NumberOfBarsSinceHighestValue(SCFloatArrayRef in, int Index, int Length)
	{
		return InternalNumberOfBarsSinceHighestValue( in,  Index,  Length);
	}
	
	int NumberOfBarsSinceHighestValue(SCFloatArrayRef in, int Length)
	{
		return InternalNumberOfBarsSinceHighestValue( in,  Index,  Length);
	}
	
	float GetCorrelationCoefficient(SCFloatArrayRef In1, SCFloatArrayRef In2, int Index, int Length)
	{
		float value;
		value = InternalGetCorrelationCoefficient( In1,  In2,  Index,  Length);
		return value;
	}
	float GetCorrelationCoefficient(SCFloatArrayRef In1, SCFloatArrayRef In2, int Length)
	{
		float value;
		value = InternalGetCorrelationCoefficient( In1,  In2,  Index,  Length);
		return value;
	}
	
	float GetTrueRange(SCBaseDataRef BaseDataIn, int Index)
	{
		return InternalGetTrueRange( BaseDataIn,  Index);
	}
	float GetTrueRange(SCBaseDataRef BaseDataIn)
	{
		return InternalGetTrueRange( BaseDataIn,  Index);
	}
	
	float GetTrueLow(SCBaseDataRef BaseDataIn, int Index)
	{
		return InternalGetTrueLow( BaseDataIn,  Index);
	}
	float GetTrueLow(SCBaseDataRef BaseDataIn)
	{
		return InternalGetTrueLow( BaseDataIn,  Index);
	}
	
	float GetTrueHigh(SCBaseDataRef BaseDataIn, int Index)
	{
		return InternalGetTrueHigh( BaseDataIn,  Index);
	}
	float GetTrueHigh(SCBaseDataRef BaseDataIn)
	{
		return InternalGetTrueHigh( BaseDataIn,  Index);
	}
	
	int GetIslandReversal(SCBaseDataRef BaseDataIn, int Index)
	{
		return InternalGetIslandReversal( BaseDataIn,  Index);
	}

	int GetIslandReversal(SCBaseDataRef BaseDataIn)
	{
		return InternalGetIslandReversal( BaseDataIn,  Index);
	}


	SCFloatArrayRef Parabolic(SCBaseDataRef BaseDataIn, SCDateTimeArrayRef BaseDateTimeIn, SCSubgraphRef Out,  float InStartAccelFactor, float InAccelIncrement, float InMaxAccelFactor, unsigned int InAdjustForGap, int InputDataHighIndex = SC_HIGH, int InputDataLowIndex = SC_LOW)
	{
		s_Parabolic ParabolicData( BaseDataIn,  Out, BaseDateTimeIn, Index, InStartAccelFactor, InAccelIncrement, InMaxAccelFactor, InAdjustForGap, InputDataHighIndex, InputDataLowIndex);

		return InternalParabolic(ParabolicData);
	}

	SCFloatArrayRef Parabolic(SCBaseDataRef BaseDataIn, SCDateTimeArrayRef BaseDateTimeIn, SCSubgraphRef Out, int Index, float InStartAccelFactor, float InAccelIncrement, float InMaxAccelFactor, unsigned int InAdjustForGap, int InputDataHighIndex = SC_HIGH, int InputDataLowIndex = SC_LOW)
	{
		s_Parabolic ParabolicData( BaseDataIn,  Out, BaseDateTimeIn, Index, InStartAccelFactor, InAccelIncrement, InMaxAccelFactor, InAdjustForGap, InputDataHighIndex, InputDataLowIndex);

		return InternalParabolic(ParabolicData);
	}
	
	SCFloatArrayRef ZigZag(SCFloatArrayRef InputDataHigh, SCFloatArrayRef InputDataLow, SCSubgraphRef Out, int Index, float ReversalPercent, int StartIndex = 0)
	{
		return InternalResettableZigZag(InputDataHigh, InputDataLow, Out, Out.Arrays[0], Out.Arrays[1], StartIndex, Index, ReversalPercent, 0.0f);
	}
	
	SCFloatArrayRef ZigZag(SCFloatArrayRef InputDataHigh, SCFloatArrayRef InputDataLow, SCSubgraphRef Out, float ReversalPercent, int StartIndex = 0)
	{
		return InternalResettableZigZag(InputDataHigh, InputDataLow, Out, Out.Arrays[0], Out.Arrays[1],  StartIndex, Index, ReversalPercent, 0.0f);
	}

	SCFloatArrayRef ZigZag(SCFloatArrayRef InputDataHigh, SCFloatArrayRef InputDataLow, SCSubgraphRef Out, int Index, float ReversalPercent, float ReversalAmount, int StartIndex = 0)
	{
		return InternalResettableZigZag(InputDataHigh, InputDataLow, Out, Out.Arrays[0], Out.Arrays[1],  StartIndex, Index, ReversalPercent, ReversalAmount);
	}

	SCFloatArrayRef ZigZag(SCFloatArrayRef InputDataHigh, SCFloatArrayRef InputDataLow, SCSubgraphRef Out, float ReversalPercent, float ReversalAmount, int StartIndex = 0)
	{
		return InternalResettableZigZag(InputDataHigh, InputDataLow, Out, Out.Arrays[0], Out.Arrays[1],  StartIndex, Index, ReversalPercent, ReversalAmount);
	}

	SCFloatArrayRef ZigZag2(SCFloatArrayRef InputDataHigh, SCFloatArrayRef InputDataLow, SCSubgraphRef Out, int Index, int NumberOfBars, float ReversalAmount, int StartIndex = 0)
	{
		return InternalResettableZigZag2(InputDataHigh, InputDataLow, Out, Out.Arrays[0], Out.Arrays[1], StartIndex, Index, NumberOfBars, ReversalAmount);
	}

	SCFloatArrayRef ZigZag2(SCFloatArrayRef InputDataHigh, SCFloatArrayRef InputDataLow, SCSubgraphRef Out, int NumberOfBars, float ReversalAmount, int StartIndex = 0)
	{
		return InternalResettableZigZag2(InputDataHigh, InputDataLow, Out, Out.Arrays[0], Out.Arrays[1], StartIndex, Index, NumberOfBars, ReversalAmount);
	}

	SCFloatArrayRef WilliamsR(SCBaseDataRef BaseDataIn, SCFloatArrayRef Out, int Index, int Length)
	{
		return InternalWilliamsR( BaseDataIn,  Out,  Index,  Length);
	}
	SCFloatArrayRef WilliamsR(SCBaseDataRef BaseDataIn, SCFloatArrayRef Out, int Length)
	{
		return InternalWilliamsR( BaseDataIn,  Out,  Index,  Length);
	}


	//Three Separate Input Arrays
	SCFloatArrayRef WilliamsR(SCFloatArrayRef InputDataHigh, SCFloatArrayRef InputDataLow, SCFloatArrayRef InputDataLast, SCFloatArrayRef Out, int Index, int Length)
	{
		return InternalWilliamsR2( InputDataHigh, InputDataLow, InputDataLast,  Out,  Index, Length);
	}
	
	//Three Separate Input Arrays without Index
	SCFloatArrayRef WilliamsR(SCFloatArrayRef InputDataHigh, SCFloatArrayRef InputDataLow, SCFloatArrayRef InputDataLast, SCFloatArrayRef Out, int Length)
	{
		return InternalWilliamsR2( InputDataHigh, InputDataLow, InputDataLast,  Out,  Index, Length);
	}

	SCFloatArrayRef WilliamsAD(SCBaseDataRef BaseDataIn, SCFloatArrayRef Out, int Index)
	{
		return InternalWilliamsAD( BaseDataIn,  Out,  Index);
	}
	SCFloatArrayRef WilliamsAD(SCBaseDataRef BaseDataIn, SCFloatArrayRef Out)
	{
		return InternalWilliamsAD( BaseDataIn,  Out,  Index);
	}
	
	SCFloatArrayRef VHF(SCFloatArrayRef In, SCFloatArrayRef Out, int Index, int Length)
	{
		return InternalVHF( In,  Out, Index,  Length);
	}
	SCFloatArrayRef VHF(SCFloatArrayRef In, SCFloatArrayRef Out, int Length)
	{
		return InternalVHF( In,  Out,  Index,  Length);
	}
	
	float GetDispersion(SCFloatArrayRef In, int Index, int Length)
	{
		return InternalGetDispersion( In,  Index,  Length);
	}
	float GetDispersion(SCFloatArrayRef In, int Length)
	{
		return InternalGetDispersion( In,  Index,  Length);
	}
	
	SCFloatArrayRef Dispersion(SCFloatArrayRef In, SCFloatArrayRef Out, int Index, int Length)
	{
		return InternalDispersion( In,  Out,  Index,  Length);
	}
	SCFloatArrayRef Dispersion(SCFloatArrayRef In, SCFloatArrayRef Out, int Length)
	{
		return InternalDispersion( In,  Out,  Index, Length);
	}
	
	SCFloatArrayRef Summation(SCFloatArrayRef In, SCFloatArrayRef Out, int Index, int Length)
	{
		return InternalSummation( In,  Out, Index, Length);
	}
	SCFloatArrayRef Summation(SCFloatArrayRef In, SCFloatArrayRef Out, int Length)
	{
		return InternalSummation( In,  Out, Index, Length);
	}
	
	SCFloatArrayRef ArmsEMV(SCBaseDataRef BaseDataIn, SCFloatArrayRef Out, int volinc, int Index)
	{
		return InternalArmsEMV( BaseDataIn,  Out,  volinc,  Index);
	}
	SCFloatArrayRef ArmsEMV(SCBaseDataRef BaseDataIn, SCFloatArrayRef Out, int volinc)
	{
		return InternalArmsEMV( BaseDataIn,  Out,  volinc,  Index);
	}
	
	SCFloatArrayRef CumulativeSummation(SCFloatArrayRef In, SCFloatArrayRef Out, int Index)
	{
		return InternalCumulativeSummation( In,  Out,  Index);
	}
	SCFloatArrayRef CumulativeSummation(SCFloatArrayRef In, SCFloatArrayRef Out)
	{
		return InternalCumulativeSummation( In,  Out,  Index);
	}
	
	float GetSummation(SCFloatArrayRef In,int Index,int Length)
	{
		return InternalGetSummation( In, Index, Length);
	}
	float GetSummation(SCFloatArrayRef In,int Length)
	{
		return InternalGetSummation( In, Index, Length);
	}
	
	SCFloatArrayRef VolumeWeightedMovingAverage(SCFloatArrayRef InData, SCFloatArrayRef InVolume, SCFloatArrayRef Out, int Index, int Length)
	{
		return InternalVolumeWeightedMovingAverage( InData,  InVolume,  Out,  Index,  Length);
	}
	SCFloatArrayRef VolumeWeightedMovingAverage(SCFloatArrayRef InData, SCFloatArrayRef InVolume, SCFloatArrayRef Out, int Length)
	{
		return InternalVolumeWeightedMovingAverage( InData,  InVolume,  Out,  Index,  Length);
	}
	
	SCFloatArrayRef WellesSum(SCFloatArrayRef In ,SCFloatArrayRef Out , int Index, int Length)
	{
		return InternalWellesSum( In , Out ,  Index, Length);
	}
	SCFloatArrayRef WellesSum(SCFloatArrayRef In ,SCFloatArrayRef Out , int Length)
	{
		return InternalWellesSum( In , Out ,  Index, Length);
	}
	
	SCFloatArrayRef TrueRange(SCBaseDataRef BaseDataIn, SCFloatArrayRef Out, int Index)
	{
		return InternalTrueRange( BaseDataIn,  Out,  Index);
	}
	SCFloatArrayRef TrueRange(SCBaseDataRef BaseDataIn, SCFloatArrayRef Out)
	{
		return InternalTrueRange( BaseDataIn,  Out,  Index);
	}
	
	
	int CalculateOHLCAverages(int Index)
	{
		return InternalCalculateOHLCAverages(Index);
	}
	int CalculateOHLCAverages()
	{
		return InternalCalculateOHLCAverages(Index);
	}
	
	SCFloatArrayRef StdError(SCFloatArrayRef In, SCFloatArrayRef Out, int Index, int Length)
	{
		return InternalStdError( In,  Out,  Index,  Length);
	}
	SCFloatArrayRef StdError(SCFloatArrayRef In, SCFloatArrayRef Out, int Length)
	{
		return InternalStdError( In,  Out,  Index,  Length);
	}
	
	SCFloatArrayRef MovingAverage(SCFloatArrayRef In, SCFloatArrayRef Out, unsigned int MovingAverageType, int Index, int Length)
	{
		return InternalMovingAverage( In,  Out,  MovingAverageType,  Index,  Length);
	}
	SCFloatArrayRef MovingAverage(SCFloatArrayRef In, SCFloatArrayRef Out, unsigned int MovingAverageType, int Length)
	{
		return InternalMovingAverage( In,  Out, MovingAverageType,  Index,  Length);
	}
	
	SCFloatArrayRef MovingMedian(SCFloatArrayRef In, SCSubgraphRef Out, int Index, int Length)
	{
		return InternalMovingMedian( In,  Out, Out.Arrays[0],  Index,  Length);
	}
	SCFloatArrayRef MovingMedian(SCFloatArrayRef In, SCSubgraphRef Out, int Length)
	{
		return InternalMovingMedian( In,  Out, Out.Arrays[0],  Index,  Length);
	}

	SCDateTime GetStartOfPeriodForDateTime(SCDateTime DateTime, unsigned int TimePeriodType, int TimePeriodLength, int PeriodOffset, int NewPeriodAtBothSessionStarts )
	{
		return InternalGetStartOfPeriodForDateTime( DateTime,   TimePeriodType,  TimePeriodLength,  PeriodOffset,  NewPeriodAtBothSessionStarts );
	}

	SCDateTime GetStartOfPeriodForDateTime(SCDateTime DateTime, unsigned int TimePeriodType, int TimePeriodLength, int PeriodOffset)
	{
		return InternalGetStartOfPeriodForDateTime( DateTime,   TimePeriodType,  TimePeriodLength,  PeriodOffset,  false );
	}

	SCFloatArrayRef OnBalanceVolumeShortTerm(SCBaseDataRef BaseDataIn, SCSubgraphRef Out,  int Index, int Length)
	{
		return InternalOnBalanceVolumeShortTerm( BaseDataIn,  Out,  Out.Arrays[0], Index,  Length);
	}
	SCFloatArrayRef OnBalanceVolumeShortTerm(SCBaseDataRef BaseDataIn, SCSubgraphRef Out, int Length)
	{
		return InternalOnBalanceVolumeShortTerm( BaseDataIn,  Out,  Out.Arrays[0],  Index,  Length);
	}
	
	SCFloatArrayRef OnBalanceVolume(SCBaseDataRef BaseDataIn, SCFloatArrayRef Out, int Index)
	{
		return InternalOnBalanceVolume( BaseDataIn,  Out,  Index);
	}
	SCFloatArrayRef OnBalanceVolume(SCBaseDataRef BaseDataIn, SCFloatArrayRef Out)
	{
		return InternalOnBalanceVolume( BaseDataIn,  Out,  Index);
	}
	
	SCFloatArrayRef StdDeviation(SCFloatArrayRef In, SCFloatArrayRef Out, int Index, int Length)
	{
		return InternalStdDeviation( In,  Out,  Index,  Length);
	}
	SCFloatArrayRef StdDeviation(SCFloatArrayRef In, SCFloatArrayRef Out, int Length)
	{
		return InternalStdDeviation( In,  Out,  Index, Length);
	}
	
	SCFloatArrayRef WildersMovingAverage(SCFloatArrayRef In, SCFloatArrayRef Out, int Index, int Length)
	{
		return InternalWildersMovingAverage( In,  Out,  Index,  Length);
	}
	SCFloatArrayRef WildersMovingAverage(SCFloatArrayRef In, SCFloatArrayRef Out, int Length)
	{
		return InternalWildersMovingAverage( In,  Out,  Index, Length);
	}
	
	SCFloatArrayRef SimpleMovAvg(SCFloatArrayRef In, SCFloatArrayRef Out, int Index, int Length)
	{
		return InternalSimpleMovAvg( In,  Out,  Index,  Length);
	}
	SCFloatArrayRef SimpleMovAvg(SCFloatArrayRef In, SCFloatArrayRef Out, int Length)
	{
		return InternalSimpleMovAvg( In,  Out,  Index, Length);
	}
	
	SCFloatArrayRef AdaptiveMovAvg(SCFloatArrayRef In, SCFloatArrayRef Out, int Index, int Length, float FastSmoothConst, float SlowSmoothConst)
	{
		return InternalAdaptiveMovAvg( In,  Out,  Index, Length,  FastSmoothConst,  SlowSmoothConst);
	}
	SCFloatArrayRef AdaptiveMovAvg(SCFloatArrayRef In, SCFloatArrayRef Out, int Length, float FastSmoothConst, float SlowSmoothConst)
	{
		return InternalAdaptiveMovAvg( In,  Out,  Index,  Length,  FastSmoothConst,  SlowSmoothConst);
	}
	
	SCFloatArrayRef LinearRegressionIndicator(SCFloatArrayRef In, SCFloatArrayRef Out, int Index, int Length)
	{
		return InternalLinearRegressionIndicator( In,  Out,  Index,  Length);
	}
	SCFloatArrayRef LinearRegressionIndicator(SCFloatArrayRef In, SCFloatArrayRef Out, int Length)
	{
		return InternalLinearRegressionIndicator( In,  Out,  Index,  Length);
	}
	
	SCFloatArrayRef ExponentialMovAvg(SCFloatArrayRef In, SCFloatArrayRef Out, int Index, int Length)
	{
		return InternalExponentialMovAvg( In,  Out,  Index,  Length);
	}
	SCFloatArrayRef ExponentialMovAvg(SCFloatArrayRef In, SCFloatArrayRef Out, int Length)
	{
		return InternalExponentialMovAvg( In,  Out,  Index,  Length);
	}

	float ArrayValueAtNthOccurrence (SCFloatArrayRef TrueFalseIn, SCFloatArrayRef ValueArrayIn, int Index, int NthOccurrence )
	{
		return InternalArrayValueAtNthOccurrence (TrueFalseIn, ValueArrayIn, Index, NthOccurrence );
	}

	float ArrayValueAtNthOccurrence (SCFloatArrayRef TrueFalseIn, SCFloatArrayRef ValueArrayIn, int NthOccurrence )
	{
		return InternalArrayValueAtNthOccurrence (TrueFalseIn, ValueArrayIn, Index, NthOccurrence );
	}


	int BuyEntry(s_SCNewOrder& NewOrder, int DataArrayIndex)
	{
		return InternalBuyEntry(NewOrder, DataArrayIndex);
	}

	int BuyEntry(s_SCNewOrder& NewOrder)
	{
		return InternalBuyEntry(NewOrder, Index);
	}

	int BuyOrder(s_SCNewOrder& NewOrder)
	{
		return InternalBuyEntry(NewOrder, Index);
	}
	
	int SubmitOrder(s_SCNewOrder& NewOrder)
	{
		if(NewOrder.OrderType == SCT_ORDERTYPE_OCO_BUY_STOP_SELL_STOP 
			|| NewOrder.OrderType == SCT_ORDERTYPE_OCO_BUY_STOP_LIMIT_SELL_STOP_LIMIT 
			|| NewOrder.OrderType == SCT_ORDERTYPE_OCO_BUY_LIMIT_SELL_LIMIT
			)
			return InternalBuyEntry(NewOrder, Index);
		else
			return SCTRADING_NOT_OCO_ORDER_TYPE;//only allow this function to be used with OCO order types
	}	

	int BuyExit(s_SCNewOrder& NewOrder, int DataArrayIndex)
	{
		return InternalBuyExit(NewOrder, DataArrayIndex);
	}
	int BuyExit(s_SCNewOrder& NewOrder)
	{
		return InternalBuyExit(NewOrder, Index);
	}

	int SellEntry(s_SCNewOrder& NewOrder, int DataArrayIndex)
	{
		return InternalSellEntry(NewOrder, DataArrayIndex);
	}
	int SellEntry(s_SCNewOrder& NewOrder)
	{
		return InternalSellEntry(NewOrder, Index);
	}

	int SellOrder(s_SCNewOrder& NewOrder)
	{
		return InternalSellEntry(NewOrder, Index);
	}
	
	int SellExit(s_SCNewOrder& NewOrder, int DataArrayIndex)
	{
		return InternalSellExit(NewOrder, DataArrayIndex);
	}
	int SellExit(s_SCNewOrder& NewOrder)
	{
		return InternalSellExit(NewOrder, Index);
	}
	
	int SubmitOCOOrder(s_SCNewOrder& NewOrder)
	{
		if(NewOrder.OrderType == SCT_ORDERTYPE_OCO_BUY_STOP_SELL_STOP 
			|| NewOrder.OrderType == SCT_ORDERTYPE_OCO_BUY_STOP_LIMIT_SELL_STOP_LIMIT 
			|| NewOrder.OrderType == SCT_ORDERTYPE_OCO_BUY_LIMIT_SELL_LIMIT
			)
			return InternalSubmitOCOOrder(NewOrder, Index);
		else
			return SCTRADING_NOT_OCO_ORDER_TYPE;//only allow this function to be used with OCO order types
		
	}


	int  GetTradePosition(s_SCPositionData& PositionData)
	{
		return InternalGetPosition(PositionData);
	}
	
	
	int IsSwingHigh(SCFloatArrayRef In, int Index, int Length)
	{
		for(int IndexOffset = 1; IndexOffset <= Length; IndexOffset++)
		{
			
			if( FormattedEvaluate(In[Index],  BasedOnGraphValueFormat, LESS_EQUAL_OPERATOR,In[Index - IndexOffset], BasedOnGraphValueFormat)
				|| FormattedEvaluate(In[Index],  BasedOnGraphValueFormat, LESS_EQUAL_OPERATOR,In[Index + IndexOffset], BasedOnGraphValueFormat) )
			{
				return 0;
			}
		}

		return 1;
	}

	int IsSwingHigh(SCFloatArrayRef In, int Length)
	{
		return IsSwingHigh(In, Index, Length);
	}

	int IsSwingLow(SCFloatArrayRef In, int Index, int Length)
	{
		for(int IndexOffset = 1; IndexOffset <= Length; IndexOffset++)
		{

			if( FormattedEvaluate(In[Index],  BasedOnGraphValueFormat, GREATER_EQUAL_OPERATOR,In[Index - IndexOffset], BasedOnGraphValueFormat)
				|| FormattedEvaluate(In[Index],  BasedOnGraphValueFormat, GREATER_EQUAL_OPERATOR,In[Index + IndexOffset], BasedOnGraphValueFormat) )
			{
				return 0;
			}
		}

		return 1;
	}



	int IsSwingLow(SCFloatArrayRef In, int Length)
	{
		return IsSwingLow(In, Index, Length);
	}

	SCFloatArrayRef AwesomeOscillator(SCFloatArrayRef In, SCSubgraphRef Out, int Index, int Length1, int Length2)
	{
		SCFloatArrayRef TempMA1 = Out.Arrays[0];
		SCFloatArrayRef TempMA2 = Out.Arrays[1];

		return InternalAwesomeOscillator(In, Out, TempMA1, TempMA2, Index, Length1, Length2);
	}

	SCFloatArrayRef AwesomeOscillator(SCFloatArrayRef In, SCSubgraphRef Out, int Length1, int Length2)
	{
		SCFloatArrayRef TempMA1 = Out.Arrays[0];
		SCFloatArrayRef TempMA2 = Out.Arrays[1];

		return InternalAwesomeOscillator(In, Out, TempMA1, TempMA2, Index, Length1, Length2);
	} 

	SCFloatArrayRef Slope(SCFloatArrayRef In, SCFloatArrayRef Out)
	{
		return InternalSlope(In, Out, Index);
	}

	SCFloatArrayRef Slope(SCFloatArrayRef In, SCFloatArrayRef Out, int Index)
	{
		return InternalSlope(In, Out, Index);
	}

	SCFloatArrayRef CumulativeDeltaVolume(SCBaseDataRef BaseDataIn, SCSubgraphRef Out, int Index, int ResetCumulativeCalculation)
	{
		return InternalCumulativeDeltaVolume(BaseDataIn, Out.Arrays[0], Out.Arrays[1], Out.Arrays[2], Out, Index, ResetCumulativeCalculation);
	}

	SCFloatArrayRef CumulativeDeltaVolume(SCBaseDataRef BaseDataIn, SCSubgraphRef Out, int ResetCumulativeCalculation)
	{
		return InternalCumulativeDeltaVolume(BaseDataIn, Out.Arrays[0], Out.Arrays[1], Out.Arrays[2], Out, Index, ResetCumulativeCalculation);
	}

	SCFloatArrayRef CumulativeDeltaTicks(SCBaseDataRef BaseDataIn, SCSubgraphRef Out, int Index, int ResetCumulativeCalculation)
	{
		return InternalCumulativeDeltaTicks(BaseDataIn, Out.Arrays[0], Out.Arrays[1], Out.Arrays[2], Out, Index, ResetCumulativeCalculation);
	}

	SCFloatArrayRef CumulativeDeltaTicks(SCBaseDataRef BaseDataIn, SCSubgraphRef Out, int ResetCumulativeCalculation)
	{
		return InternalCumulativeDeltaTicks(BaseDataIn, Out.Arrays[0], Out.Arrays[1], Out.Arrays[2], Out, Index, ResetCumulativeCalculation);
	}

	int Round(float Number)
	{
		int integer=(int)Number;


		if ( (Number>0.0)&&( (Number-integer) >= .5) )
			return ++integer;
		else if ( (Number<0.0)&&( (Number-integer) <= -.5) )
			return --integer;

		return integer;

	}

	//rounds to the nearest TickSize
	float RoundToTickSize(float Value)
	{
		return (float)RoundToTickSize((double)Value, (double)TickSize);
	}

	double RoundToTickSize(double Value)
	{
		return RoundToTickSize(Value, (double)TickSize);
	}

	float RoundToTickSize(float Value, float TickSize)
	{
		return (float)RoundToTickSize((double)Value, (double)TickSize);
	}

	double RoundToTickSize(double Value, float TickSize)
	{
		return RoundToTickSize(Value, (double)TickSize);
	}

	double RoundToIncrement(double Value, float TickSize)
	{
		return RoundToTickSize(Value, (double)TickSize);
	}

	double RoundToTickSize(double Value, double TickSize)
	{
		if (TickSize == 0)
			return Value;  // cannot round
		
		double ClosestMult = (int)(Value / TickSize) * TickSize;
		double Diff = Value - ClosestMult;
		
		double DifferenceFromIncrement = TickSize*0.5 - Diff;
		
		double Result;
		if (Value > 0.0 && DifferenceFromIncrement <= TickSize * 0.001)
			Result = ClosestMult + TickSize;
		else if (Value < 0.0 && DifferenceFromIncrement <= TickSize * 0.001)
			Result = ClosestMult - TickSize;
		else
			Result = ClosestMult;
		
		return Result;
	}

	unsigned int PriceValueToTicks(float PriceValue)
	{

		return Round(PriceValue/TickSize);
	}	
	
	float TicksToPriceValue(unsigned int Ticks)
	{

		return Ticks*TickSize;
	}

	// This uses the Volume Value Format chart setting to determine the multiplier.  This is used when volumes have fractional values and are stored as an integer.
	float MultiplierFromVolumeValueFormat() const
	{
		float Multiplier;
		switch (VolumeValueFormat)
		{
			default:
			case 0: Multiplier = 1.0f; break;
			case 1: Multiplier = 0.1f; break;
			case 2: Multiplier = 0.01f; break;
			case 3: Multiplier = 0.001f; break;
			case 4: Multiplier = 0.0001f; break;
			case 5: Multiplier = 0.00001f; break;
			case 6: Multiplier = 0.000001f; break;
			case 7: Multiplier = 0.0000001f; break;
			case 8: Multiplier = 0.00000001f; break;
		}
		return Multiplier;
	}
	
	void SetAlert(int AlertNumber, int ArrayIndex, const SCString& Message = "")
	{

		InternalSetAlert(AlertNumber, ArrayIndex, Message);
	}
	void SetAlert(int AlertNumber, const SCString& Message = "")
	{
		SetAlert(AlertNumber, Index, Message);
	}
	
	int GetNumberOfBaseGraphArrays() const
	{
		return BaseDataIn.GetArraySize();
	}

	SCString &GetRealTimeSymbol()
	{
		if (TradeAndCurrentQuoteSymbol.GetLength() > 0)
			return TradeAndCurrentQuoteSymbol;
		else
			return Symbol;

	}
	
	/*========================================================================
		Returns the color at the RGB distance between Color1 and Color2,
		where ColorDistance is a value between 0 and 1.  If ColorDistance is 0, then Color1 is returned.
		If ColorDistance is 1, then Color2 is returned.  If ColorDistance is 0.5f, then the color half
		way between Color1 and Color2 is returned.
	------------------------------------------------------------------------*/
	COLORREF RGBInterpolate(const COLORREF& Color1, const COLORREF& Color2, float ColorDistance)
	{
		return RGB((int)(GetRValue(Color1)*(1.0f-(ColorDistance)) + GetRValue(Color2)*(ColorDistance) + 0.5f),
			(int)(GetGValue(Color1)*(1.0f-(ColorDistance)) + GetGValue(Color2)*(ColorDistance) + 0.5f),
			(int)(GetBValue(Color1)*(1.0f-(ColorDistance)) + GetBValue(Color2)*(ColorDistance) + 0.5f));
	}
	
	SCDateTime ConvertToSCTimeZone(const SCDateTime& DateTime, const char* TimeZonePOSIXString)
	{
		return InternalConvertToSCTimeZone(DateTime, TimeZonePOSIXString);
	}
	
	SCDateTime ConvertToSCTimeZone(const SCDateTime& DateTime, TimeZonesEnum TimeZone)
	{
		// The reason we are passing a string instead of the enumeration
		// value is because the enumeration value could change if the list of
		// time zones ever changes, but the string will not change.
		return InternalConvertToSCTimeZone(DateTime, GetPosixStringForTimeZone(TimeZone));
	}
	
	SCFloatArrayRef DoubleStochastic(SCBaseDataRef BaseDataIn, SCSubgraphRef Out, int Length, int MovAvgLength, int MovingAverageType)
	{
		InternalDoubleStochastic(BaseDataIn, Out, Out.Arrays[0], Out.Arrays[1], Out.Arrays[2], Out.Arrays[3], Index, Length, MovAvgLength, MovingAverageType);
		return Out;
	}

	SCFloatArrayRef DoubleStochastic(SCBaseDataRef BaseDataIn, SCSubgraphRef Out, int Index, int Length, int MovAvgLength, int MovingAverageType)
	{
		InternalDoubleStochastic(BaseDataIn, Out, Out.Arrays[0], Out.Arrays[1], Out.Arrays[2], Out.Arrays[3], Index, Length, MovAvgLength, MovingAverageType);
		return Out;
	}


	double GetStandardError(SCFloatArrayRef In, int Index, int Length)
	{
		return InternalGetStandardError(In, Index, Length);
	}

	double GetStandardError(SCFloatArrayRef In, int Length)
	{
		return InternalGetStandardError(In, Index, Length);
	}

	SCFloatArrayRef AccumulationDistribution ( SCBaseDataRef BaseDataIn, SCFloatArrayRef Out, int Index)
	{
		return InternalAccumulationDistribution(BaseDataIn, Out, Index);
	}
	
	SCFloatArrayRef AccumulationDistribution ( SCBaseDataRef BaseDataIn, SCFloatArrayRef Out)
	{
		return InternalAccumulationDistribution(BaseDataIn, Out, Index);
	}

	int GetValueFormat()
	{	
		if(ValueFormat == VALUEFORMAT_INHERITED)
		{
			return BasedOnGraphValueFormat;
		}
		else
			return ValueFormat;
	}

	void* AllocateMemory(int Size)
	{
		char* Pointer = (char*)HeapAlloc(GetProcessHeap(), 0, (SIZE_T)Size);
		memset(Pointer, 0, Size);
		return Pointer;
	}

	void FreeMemory(void* Pointer)
	{
		if (Pointer != NULL)
			HeapFree(GetProcessHeap(), 0, Pointer);
	}

	//This function is for Intraday charts and determines if the given BarIndex is the beginning of a new trading day according to the Session Times for the chart.
	bool IsNewTradingDay(int BarIndex)
	{
		SCDateTime CurrentBarTradingDayStartDateTime = GetTradingDayStartDateTimeOfBar(BaseDateTimeIn[BarIndex]);
		SCDateTime PriorBarTradingDayStartDateTime = GetTradingDayStartDateTimeOfBar(BaseDateTimeIn[BarIndex -1]);

		return PriorBarTradingDayStartDateTime != CurrentBarTradingDayStartDateTime;
		

	}

	/************************************************************************/
	// Data Members
	
	int ArraySize;
	
	void* Unused49;
	void* Unused48;
	void* Unused47[9];
	void* Unused46[9];
	void* Unused45;
	void* Unused44[4];
	void* Unused43[8];

	
	int DrawZeros;
	int DataStartIndex;
	int UpdateStartIndex;
	
	char Unused42[128];
	
	void* Unused41;
	void* Unused40;
	
	int GraphRegion;
	void* Unused39;
	int Unused_ReferencesOtherChart;
	
	
	void* Unused38;

	int FreeDLL; //set to 0 to not free the DLL after your function is called.
	float Bid;
	float Ask;
	const char* Unused_UserName;
	int Unused_UsePriceGraphStyle;
	
	void* Unused37;

	unsigned int DailyVolume;
	char Unused_Symbol[128];
	int ChartNumber;
	int ChartDataType;  // 1 = historical daily data, 2 = intraday data
	int Unused_TwoSecondsPerBar;
	int NumberOfTradesPerBar;
	
	int DailyDataBarPeriod;  // 1 = days, 2 = weekly, 3 = monthly
	int DaysPerBar;
	int BidSize;
	int AskSize;
	int LastSize;
	char* Unused_TextInput;
	int VolumePerBar;  // This is the volume per bar setting
    
	int unused_CurrentDate;
	int unused_CurrentTime;
	
	void* Unused36;
	int (SCDLLCALL* GetNearestMatchForDateTimeIndex)(int ChartNumber, int CallingChartIndex);
	
	
	void* Unused53;
	double Unused_FibLevels[16];
	
	int (SCDLLCALL* ChartDrawingExists)(int ChartNumber, int LineNumber);
	
	const char* VersionNumber;
	
	void (SCDLLCALL* pAddMessageToLog)(const char* Message, int showlog);
	

	int Unused_PlayAlerts;
	
	void* Unused30;
	void* Unused31;
	void* Unused32;
	void* Unused33;
	void* Unused34;
	void* Unused35;
	
	void* Unused_GetToolOnChart;
	
	int (SCDLLCALL* DeleteLineOrText)(int ChartNumber, int Tool, int LineNumber);
	
	void (SCDLLCALL* Unused_pAddAlertLine)(const char* Message);
	
	int Unused_StartTime;  // Old -- Use StartTime1
	int Unused_EndTime;  // Old -- Use EndTime1
	
	HWND ChartWindowHandle;
	unsigned int ProcId;
	char Unused_DataFile[512];
	int LastCallToFunction;
	
	int Unused51;
	
	void* StorageBlock;  // This is 512 bytes of permanent storage for any data type
	
	void* Unused29;
	
	// 73.0
	
	void* Unused59;
	
	
	// 73.4
	
	float DailyHigh;
	float DailyLow;
	
	void* Unused25;
	
	void* Unused26;
	
	
	// 73.7
	
	void* Unused27;
	
	float RangeBarValue;
	unsigned int RangeBarType;
	
	
	// 73.8
	
	unsigned int NumFillSpaceBars;
	unsigned int PreserveFillSpace;  // Boolean
	SCDateTime TimeScaleAdjustment;
	
	
	// 74.4
	
	int (SCDLLCALL* Unused_GetNearestDateTimeIndex)(int ChartNumber, unsigned short Date, unsigned short Time);  // Time in 2-second units
	int (SCDLLCALL* GetNearestMatchForSCDateTime)(int ChartNumber, const SCDateTime& DateTime);
	int (SCDLLCALL* GetContainingIndexForDateTimeIndex)(int ChartNumber, int CallingChartIndex);
	int (SCDLLCALL* Unused_GetIndexForDateTime)(int ChartNumber, unsigned short Date, unsigned short Time);  // Time in 2-second units
	int (SCDLLCALL* GetContainingIndexForSCDateTime)(int ChartNumber, const SCDateTime& DateTime);
	
	short GraphicalDisplacement;
	short Unused_Reserved1;
	
	
	// 75.1
	
	int SetDefaults;  // Boolean
	
	
	// 78
	
	int UpdateAlways;  // Boolean
	unsigned int ActiveToolIndex;
	int ActiveToolYPosition;
	
	
	
	int IndexOfLastVisibleBar; 
	
	unsigned int ChartBackgroundColor;
	const char* ChartTextFont;
	int StudyRegionTopCoordinate;
	int StudyRegionBottomCoordinate;
	int Unused_FillSpaceLeftCoordinate;
	
	
	// 113
	
	int Unused_DateOfLastFileRecord;  // Use DateTimeOfLastFileRecord
	int Unused_TimeOfLastFileRecord;  // Use DateTimeOfLastFileRecord
	
	
	// 119
	
	int IsCustomChart;  // Boolean
	int NumberOfArrays;
	
	int OutArraySize;
	void * Unused1;  
	void * Unused2;  
	
	const char* (SCDLLCALL* InternalGetStudyName)(int StudyIndex);
	
	void* Unused57;
	void* Unused58;
	
	void* Unused24;
	
	
	// 123
	

	int (SCDLLCALL* Internal_UseToolEx)(s_UseTool& ToolDetails);
	
	
	// 124
	
	unsigned short ScaleType;  // ScaleTypeEnum
	unsigned short ScaleRangeType;  // ScaleRangeTypeEnum
	float ScaleRangeTop;
	float ScaleRangeBottom;
	float ScaleIncrement; 
	float ScaleConstRange;
	
	
	// 132
	
	// You can use these variables for anything required by a study function and they will be
	// preserved between function calls.  These are all initialized to 0 when the study function
	// runs for the first time.  The values will not be saved when the chart is closed.
	// Example usage: sc.PersistVars->i1 = 10;  sc.PersistVars->f1 = 10.5f;
	// See the scsf_PersistentVariablesExample function for more examples.
	struct s_PersistentVariables
	{
		int i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13, i14, i15, i16;
		float f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16;
		double d1, d2, d3, d4, d5, d6, d7, d8, d9, d10, d11, d12, d13, d14, d15, d16,d17,d18,d19,d20,d21,d22,d23,d24,d25,d26,d27,d28,d29,d30,d31,d32;
		SCDateTime scdt1,scdt2,scdt3,scdt4,scdt5,scdt6,scdt7,scdt8;
		int Integers[64];
		double Doubles[64];

		void Clear ()
		{
			memset(this, 0, sizeof(s_PersistentVariables));
		}
	};
	s_PersistentVariables* PersistVars;
	
	
	// 135
	
	float BaseGraphScaleIncrement;
	float BaseGraphHorizontalGridLineIncrement ; // Added in version 1186

	void* Unused_YearMonthDayToDate;
	void* Unused54;
	void* Unused_TimeToHourMinuteSecond;
	void* Unused56;
	void* Unused_HourMinuteSecondToTime;
	
	void* Unused21;
	
	
	// 145
	
	SCString GraphName;
	SCString TextInput;
	SCString TextInputName;
	
	SCUShortArray Unused22;  //  no longer set
	SCUShortArray Unused23;  // no longer set
	SCDateTimeArray BaseDateTimeIn;  // From the base graph
	SCFloatArrayArray BaseDataIn;  // From the base graph
	SCDateTimeArray DateTimeOut;
	char Unused_Subgraph145[424];
	SCInput145Array Input;
	
	SCDateTime Unused_CurrentDateTime;
	SCDateTime DateTimeOfLastFileRecord;  // This is the DateTime of the last file record added to the BaseDataIn array
	int SecondsPerBar;
	SCString Symbol;
	SCString DataFile;  // The path + filename for the chart data file
	
	void (SCDLLCALL* GetChartArray)(int ChartNumber, int DataItem, SCFloatArrayRef PriceArray);
	void (SCDLLCALL* GetChartDateTimeArray)(int ChartNumber, SCDateTimeArrayRef DateTimeArray);
	int (SCDLLCALL* GetStudyArray)(int StudyNumber, int StudySubgraphNumber, SCFloatArrayRef SubgraphArray);
	void* Unused_GetTimeAndSales;
	void* Unused6;
	void* Unused7;
	void* Unused8;
	void* Unused9;
	void* Unused10;
	void* Unused11;
	
	void (SCDLLCALL* GetStudyArrayFromChart)(int ChartNumber, int StudyNumber, int StudySubgraphNumber, SCFloatArrayRef SubgraphArray);
	
	void* Unused12;
	void* Unused13;
	void* Unused14;
	void* Unused15;
	void* Unused16;
	
	int ValueFormat;
	SCString StudyDescription;
	
	int StartTime1;
	int EndTime1;
	int StartTime2;
	int EndTime2;
	int UseSecondStartEndTimes;  

	// 148
	
	int IndexOfFirstVisibleBar; 

	
	// 152
	
	void* Unused50;
	
	// 157
	
	SCString& (SCDLLCALL* FormatString)(SCString& Out, const char* Format, ...);
	
	
	// 158
	
	const s_SCBasicSymbolData* SymbolData;
	
	void* Unused17;
	void* Unused18;
	void* Unused19;
	void* Unused20;

	int CalculationPrecedence;
	
	
	// 162
	
	int AutoLoop;
	int CurrentIndex;
	int Index;
	int StandardChartHeader;
	int DisplayAsMainPriceGraph;
	
	void* Unused4;
	void* Unused5;
	void* Unused3;
	
	int (SCDLLCALL* Unused_GetChartDrawing)(int ChartNumber, DrawingTypeEnum DrawingType, void* ChartDrawing, int DrawingIndex);
	
	
	// 165
	
	SCFloatArrayRef (SCDLLCALL* InternalExponentialMovAvg)(SCFloatArrayRef In, SCFloatArrayRef Out, int Index, int Length);

	SCFloatArrayRef (SCDLLCALL* InternalLinearRegressionIndicator)(SCFloatArrayRef In, SCFloatArrayRef Out, int Index, int Length);

	SCFloatArrayRef (SCDLLCALL* InternalAdaptiveMovAvg)(SCFloatArrayRef In, SCFloatArrayRef Out, int Index, int Length, float FastSmoothConst, float SlowSmoothConst);

	SCFloatArrayRef (SCDLLCALL* InternalSimpleMovAvg)(SCFloatArrayRef In, SCFloatArrayRef Out, int Index, int Length);

	SCFloatArrayRef (SCDLLCALL* InternalWildersMovingAverage)(SCFloatArrayRef In, SCFloatArrayRef Out, int Index, int Length);

	SCFloatArrayRef (SCDLLCALL* InternalWeightedMovingAverage)(SCFloatArrayRef In, SCFloatArrayRef Out, int Index, int Length);

	SCFloatArrayRef (SCDLLCALL* InternalStdDeviation)(SCFloatArrayRef In, SCFloatArrayRef Out, int Index, int Length);
	
	SCFloatArrayRef (SCDLLCALL* InternalCCISMA)(SCFloatArrayRef In, SCFloatArrayRef SMAOut, SCFloatArrayRef CCIOut, int Index, int Length, float Multiplier);

	SCFloatArrayRef (SCDLLCALL* InternalHighest)(SCFloatArrayRef In, SCFloatArrayRef Out, int Index, int Length);

	SCFloatArrayRef (SCDLLCALL* InternalLowest)(SCFloatArrayRef In, SCFloatArrayRef Out, int Index, int Length);

	SCFloatArrayRef (SCDLLCALL* InternalATR)(SCBaseDataRef BaseDataIn, SCFloatArrayRef TROut, SCFloatArrayRef ATROut, int Index, int Length, unsigned int MovingAverageType);

	SCFloatArrayRef (SCDLLCALL* InternalOnBalanceVolume)(SCBaseDataRef BaseDataIn, SCFloatArrayRef Out, int Index);

	SCFloatArrayRef (SCDLLCALL* InternalOnBalanceVolumeShortTerm)(SCBaseDataRef BaseDataIn, SCFloatArrayRef Out, SCFloatArray& OBVTemp, int Index, int Length);

	SCFloatArrayRef (SCDLLCALL* InternalMovingAverage)(SCFloatArrayRef In, SCFloatArrayRef Out, unsigned int MovingAverageType, int Index, int Length);

	void (SCDLLCALL* InternalStochastic)(SCBaseDataRef BaseDataIn, SCFloatArrayRef FastKOut, SCFloatArrayRef FastDOut, SCFloatArrayRef SlowDOut, int Index, int FastKLength, int FastDLength, int SlowDLength, unsigned int MovingAverageType);
	
	
	// 178
	
	SCString UserName;
	
	SCFloatArrayRef (SCDLLCALL* InternalStdError)(SCFloatArrayRef In, SCFloatArrayRef Out, int Index, int Length);
	int (SCDLLCALL* InternalCalculateOHLCAverages)(int Index);
	
	
	// 191
	
	int (SCDLLCALL* Unused_PlaySound)(int AlertNumber);
	
	
	// 199
	
	void* Unused60;
	int (SCDLLCALL* p_ResizeArrays)(int NewSize);
	int (SCDLLCALL* p_AddElements)(int NumElements);
	
	
	// 206
	
	int (SCDLLCALL* GetChartDrawing)(int ChartNumber, int DrawingType, s_UseTool& ChartDrawing, int DrawingIndex);
	
	SCFloatArrayRef (SCDLLCALL* Internal_Ergodic)(SCFloatArrayRef In, SCFloatArrayRef Out, int Index, int LongEMALength, int ShortEMALength, float Multiplier,
		SCFloatArrayRef InternalArray1, SCFloatArrayRef InternalArray2, SCFloatArrayRef InternalArray3, SCFloatArrayRef InternalArray4, SCFloatArrayRef InternalArray5, SCFloatArrayRef InternalArray6);

	void* Unused62;

	SCFloatArrayRef (SCDLLCALL* Internal_Keltner)(SCBaseDataRef BaseDataIn, SCFloatArrayRef In, SCFloatArrayRef KeltnerAverageOut, SCFloatArrayRef TopBandOut, SCFloatArrayRef BottomBandOut,	int Index, int KeltnerMALength, unsigned int KeltnerMAType, int TrueRangeMALength, unsigned int TrueRangeMAType, float TopBandMultiplier, float BottomBandMultiplier,SCFloatArrayRef InternalArray1, SCFloatArrayRef InternalArray2);

	void* unused;
	
	
	// 210
	
	SCFloatArrayArray BaseData;  // From the base graph
	
	
	// 222
	
	SCFloatArrayRef (SCDLLCALL* InternalTrueRange)(SCBaseDataRef BaseDataIn, SCFloatArrayRef Out, int Index);
	
	SCFloatArrayRef (SCDLLCALL* InternalWellesSum)(SCFloatArrayRef In ,SCFloatArrayRef Out , int Index, int Length);
	
	void (SCDLLCALL* InternalDMI)(SCBaseDataRef BaseDataIn,
		int Index,
		int Length, unsigned int DisableRounding,
		SCFloatArrayRef PosDMIOut, SCFloatArrayRef NegDMIOut, SCFloatArrayRef DiffDMIOut,
		SCFloatArrayRef InternalTrueRangeSummation, SCFloatArrayRef InternalPosDM, SCFloatArrayRef InternalNegDM);
	
	SCFloatArrayRef (SCDLLCALL* InternalDMIDiff)(SCBaseDataRef BaseDataIn,
		int Index,
		int Length,
		SCFloatArrayRef Out,
		SCFloatArrayRef InternalTrueRangeSummation, SCFloatArrayRef InternalPosDM, SCFloatArrayRef InternalNegDM);
	
	SCFloatArrayRef (SCDLLCALL* InternalADX)(SCBaseDataRef BaseDataIn,
		int Index,
		int DXLength, int DXMovAvgLength,
		SCFloatArrayRef Out,
		SCFloatArrayRef InternalTrueRangeSummation, SCFloatArrayRef InternalPosDM, SCFloatArrayRef InternalNegDM, SCFloatArrayRef InternalDX);
	
    SCFloatArrayRef (SCDLLCALL* InternalADXR)(SCBaseDataRef BaseDataIn,
        int Index,
        int DXLength, int DXMovAvgLength, int ADXRInterval,
        SCFloatArrayRef Out,
        SCFloatArrayRef InternalTrueRangeSummation, SCFloatArrayRef InternalPosDM, SCFloatArrayRef InternalNegDM, SCFloatArrayRef InternalDX, SCFloatArrayRef InternalADX);
	
	// 235
	
	int FileRecordIndexOfLastDataRecordInChart;
	
	
	// 241
	
	//The return value for this function must be implemented as a four byte integer, because a Boolean type may not be implemented the same way on all compilers and can cause corruptions.
	int (SCDLLCALL* InternalFormattedEvaluate)(float Value1, unsigned int Value1Format,
		OperatorEnum Operator,
		float Value2, unsigned int Value2Format,
		float PrevValue1, float PrevValue2,
		int* CrossDirection);
	
	
	// 245
	
	int (SCDLLCALL* InternalPlaySound)(int AlertNumber, const char* AlertMessage, int ShowAlertLog);
	
	void (SCDLLCALL* InternalAddAlertLine)(const char* Message, int ShowAlertLog);
	
	
	// 247
	
	SCFloatArrayRef (SCDLLCALL *InternalRSI)(SCFloatArrayRef In,  SCFloatArrayRef RsiOut, SCFloatArrayRef UpSumsTemp, SCFloatArrayRef DownSumsTemp, SCFloatArrayRef SmoothedUpSumsTemp, SCFloatArrayRef SmoothedDownSumsTemp, int Index, unsigned int AveragingType, int Length);
	
	SCDateTime CurrentSystemDateTime;
	
	
	// 248
	
	int (SCDLLCALL* InternalPlaySoundPath)(const char* AlertPathAndFileName, int NumberTimesPlayAlert, const char* AlertMessage, int ShowLog);
	
	
	// 260
	
	SCSubgraph260Array Subgraph;
	
	
	// 263
	
	
	// 264
	
	float TickSize;
	
	
	// 270
	
	SCFloatArrayRef (SCDLLCALL* InternalHullMovingAverage)(SCFloatArrayRef In, SCFloatArrayRef Out, SCFloatArrayRef InternalArray1, SCFloatArrayRef InternalArray2, SCFloatArrayRef InternalArray3, int Index, int Length);

	SCFloatArrayRef (SCDLLCALL* InternalTriangularMovingAverage)(SCFloatArrayRef In, SCFloatArrayRef Out, SCFloatArrayRef InternalArray1, int Index, int Length);

	SCFloatArrayRef (SCDLLCALL* InternalVolumeWeightedMovingAverage)(SCFloatArrayRef InData, SCFloatArrayRef InVolume, SCFloatArrayRef Out, int Index, int Length);

	SCFloatArrayRef (SCDLLCALL* Internal_SmoothedMovingAverage)(SCFloatArrayRef In, SCFloatArrayRef Out, int Index, int Length, int Offset);
	
	
	// 273
	
	SCString Unused_CountdownText;
	
	float (SCDLLCALL* InternalGetSummation)(SCFloatArrayRef In,int Index,int Length);
	
	
	// 275
	
	SCFloatArrayRef (SCDLLCALL* InternalMACD)(SCFloatArrayRef In, SCFloatArrayRef FastMAOut, SCFloatArrayRef SlowMAOut, SCFloatArrayRef MACDOut, SCFloatArrayRef MACDMAOut, SCFloatArrayRef MACDDiffOut, int Index, int FastMALength, int SlowMALength, int MACDMALength, int MovAvgType);
	
	void (SCDLLCALL* GetMainGraphVisibleHighAndLow)(float& High, float& Low);
	
	
	// 276
	
	SCFloatArrayRef (SCDLLCALL* InternalTEMA)(SCFloatArrayRef In, SCFloatArrayRef Out, SCFloatArrayRef InternalArray1,SCFloatArrayRef InternalArray2,SCFloatArrayRef InternalArray3, int Index, int  Length);
	
	SCFloatArrayRef (SCDLLCALL* InternalBollingerBands)(SCFloatArrayRef In, SCFloatArrayRef Avg, SCFloatArrayRef TopBand,SCFloatArrayRef BottomBand,SCFloatArrayRef StdDev, int Index, int Length,float Multiplier,int MovAvgType);
	
	
	// 278
	
	int (SCDLLCALL* Internal_GetOHLCForDate)(double Date, float& Open, float& High, float& Low, float& Close);
	
	
	// 280
	
	int ReplayStatus;
	
	
	// 281
	
	int AdvancedFeatures;
	float PreviousClose;
	
	int BaseGraphValueFormat;
	
	SCString ((SCDLLCALL* FormatGraphValue)(double Value, int ValueFormat));
	
	int UseGlobalChartColors;
	
	unsigned int ScaleBorderColor;
	
	int (SCDLLCALL* GetStudyArrayUsingID)(unsigned int StudyID, unsigned int StudySubgraphIndex, SCFloatArrayRef SubgraphArray);
	
	int SelectedAlertSound;
	
	
	// 297
	
	SCFloatArrayRef (SCDLLCALL* InternalCumulativeSummation)(SCFloatArrayRef In, SCFloatArrayRef Out, int Index);
	
	SCFloatArrayRef (SCDLLCALL* InternalArmsEMV)(SCBaseDataRef BaseDataIn, SCFloatArrayRef Out, int volinc, int Index);
	
	SCFloatArrayRef (SCDLLCALL* InternalChaikinMoneyFlow)(SCBaseDataRef BaseDataIn, SCFloatArrayRef Out, SCFloatArrayRef temp, int Index, int Length);
	
	SCFloatArrayRef (SCDLLCALL* InternalSummation)(SCFloatArrayRef In, SCFloatArrayRef Out, int Index, int Length);
	
	SCFloatArrayRef (SCDLLCALL* InternalDispersion)(SCFloatArrayRef In, SCFloatArrayRef Out, int Index, int Length);
	
	float (SCDLLCALL* InternalGetDispersion)(SCFloatArrayRef In, int Index, int Length);
	
	SCFloatArrayRef (SCDLLCALL* InternalEnvelopePercent)(SCFloatArrayRef In, SCFloatArrayRef Out1, SCFloatArrayRef Out2,  float pct, int Index);
	SCFloatArrayRef (SCDLLCALL* InternalVHF)(SCFloatArrayRef In, SCFloatArrayRef Out, int Index, int Length);
	SCFloatArrayRef (SCDLLCALL* InternalRWI)(SCBaseDataRef BaseDataIn, SCFloatArrayRef Out1, SCFloatArrayRef Out2, 
		SCFloatArrayRef TrueRangeArray, SCFloatArrayRef LookBackLowArray, SCFloatArrayRef LookBackHighArray, int Index, int Length);
	
	SCFloatArrayRef (SCDLLCALL* InternalUltimateOscillator)(SCBaseDataRef BaseDataIn, 
		SCFloatArrayRef Out, 
		SCFloatArrayRef CalcE,
		SCFloatArrayRef CalcF,
		SCFloatArrayRef CalcG,
		SCFloatArrayRef CalcH,
		SCFloatArrayRef CalcI,
		SCFloatArrayRef CalcJ,
		SCFloatArrayRef CalcK,
		SCFloatArrayRef CalcL,
		SCFloatArrayRef CalcM,
		SCFloatArrayRef CalcN,
		SCFloatArrayRef CalcO,
		SCFloatArrayRef CalcP,
		SCFloatArrayRef CalcQ,
		int Index, int Length1, int Length2, int Length3);
	
	SCFloatArrayRef (SCDLLCALL* InternalWilliamsAD)(SCBaseDataRef BaseDataIn, SCFloatArrayRef Out, int Index);
	SCFloatArrayRef (SCDLLCALL* InternalWilliamsR)(SCBaseDataRef BaseDataIn, SCFloatArrayRef Out, int Index, int Length);
	int (SCDLLCALL* InternalGetIslandReversal)(SCBaseDataRef BaseDataIn, int Index);
	SCFloatArrayRef (SCDLLCALL* InternalOscillator)(SCFloatArrayRef In1, SCFloatArrayRef In2, SCFloatArrayRef Out, int Index);
	float (SCDLLCALL* InternalGetTrueHigh)(SCBaseDataRef BaseDataIn, int Index);
	float (SCDLLCALL* InternalGetTrueLow)(SCBaseDataRef BaseDataIn, int Index);
	float (SCDLLCALL* InternalGetTrueRange)(SCBaseDataRef BaseDataIn, int Index);
	float (SCDLLCALL* InternalGetCorrelationCoefficient)(SCFloatArrayRef In1, SCFloatArrayRef In2, int Index, int Length);
	
	int (SCDLLCALL* InternalNumberOfBarsSinceHighestValue)(SCFloatArrayRef in, int Index, int Length);
	int (SCDLLCALL* InternalNumberOfBarsSinceLowestValue)(SCFloatArrayRef in, int Index, int Length);
	
	SCFloatArrayRef(SCDLLCALL* InternalPriceVolumeTrend)(SCBaseDataRef BaseDataIn, SCFloatArrayRef Out, int Index); 
	
	
	// 298
	
	SCString ((SCDLLCALL* GetStudyNameUsingID)(unsigned int StudyID));
	int (SCDLLCALL* GetStudyDataStartIndexUsingID)(unsigned int StudyID);
	
	
	// 307
	
	SCFloatArrayRef (SCDLLCALL* InternalLinearRegressionIndicatorAndStdErr)(SCFloatArrayRef In, SCFloatArrayRef Out, SCFloatArrayRef StdErr, int Index, int Length);
	
	
	// 315
	
	float ValuePerPoint;
	SCString ((SCDLLCALL* GetCountDownText)());
	
	
	// 318
	
	int GraphDrawType;
	
	
	// 335
	
	SCFloatArrayRef (SCDLLCALL* InternalMomentum)(SCFloatArrayRef In, SCFloatArrayRef Out, int Index, int Length);
	
	
	// 338
	
	int (SCDLLCALL* InternalGetOHLCOfTimePeriod)(s_GetOHLCOfTimePeriod& Data);
	
	void* Unused61;
	
	
	// 340
	
	int AlertOnlyOncePerBar;
	int ResetAlertOnNewBar;
	
	
	// 346
	
	int AllowMultipleEntriesInSameDirection; 
	int SupportReversals;
	int SendOrdersToTradeService;
	int AllowOppositeEntryWithOpposingPositionOrOrders;
	int (SCDLLCALL* InternalBuyEntry)(s_SCNewOrder& NewOrder, int Index);
	int (SCDLLCALL* InternalBuyExit)(s_SCNewOrder& NewOrder, int Index);
	int (SCDLLCALL* InternalSellEntry)(s_SCNewOrder& NewOrder, int Index);
	int (SCDLLCALL* InternalSellExit)(s_SCNewOrder& NewOrder, int Index);
	int (SCDLLCALL* CancelOrder)(int InternalOrderID);
	int (SCDLLCALL* GetOrderByOrderID)(int InternalOrderID, s_SCTradeOrder& OrderDetails);
	int (SCDLLCALL* GetOrderByIndex)(int OrderIndex, s_SCTradeOrder& OrderDetails);
	void * Unused65;
	void * Unused66;
	
	int Unused67;
	
	
	// 351
	
	SCFloatArrayRef (SCDLLCALL* InternalTRIX)(SCFloatArrayRef In, SCFloatArrayRef Out, SCFloatArrayRef InternalEma_1, SCFloatArrayRef InternalEma_2, SCFloatArrayRef InternalEma_3, int Index, int Length);
	
	
	// 359
	
	int DownloadingHistoricalData;
	SCString CustomAffiliateCode;
	
	
	// 363
	
	int AllowOnlyOneTradePerBar;
	
	void (SCDLLCALL* InternalStochastic2)(SCFloatArrayRef InputDataHigh, SCFloatArrayRef InputDataLow, SCFloatArrayRef InputDataLast, SCFloatArrayRef FastKOut, SCFloatArrayRef FastDOut, SCFloatArrayRef SlowDOut, int Index, int FastKLength, int FastDLength, int SlowDLength, unsigned int MovingAverageType);
	
	SCFloatArrayRef (SCDLLCALL* InternalWilliamsR2)(SCFloatArrayRef InputDataHigh, SCFloatArrayRef InputDataLow, SCFloatArrayRef InputDataLast,  SCFloatArrayRef Out, int Index, int Length);
	
	SCFloatArrayRef (SCDLLCALL* InternalParabolic)(s_Parabolic& ParabolicData);
	
	void* Unused63;
	
	// 373
	
	int (SCDLLCALL* CancelAllOrders)();
	
	
	// 380
	
	int StartTimeOfDay;
	
	
	// 388
	
	float (SCDLLCALL* InternalArrayValueAtNthOccurrence)(SCFloatArrayRef TrueFalseIn, SCFloatArrayRef ValueArrayIn, int Index, int NthOccurrence );
	
	
	// 408
	
	SCFloatArrayRef (SCDLLCALL* Internal_Demarker)(SCBaseDataRef BaseDataIn, SCFloatArrayRef Out, SCFloatArrayRef DemMax, SCFloatArrayRef DemMin, SCFloatArrayRef SmaDemMax, SCFloatArrayRef SmaDemMin, int Index, int Length);

	SCFloatArrayRef (SCDLLCALL* Old_Internal_AroonIndicator)(SCFloatArrayRef In, SCFloatArrayRef OutUp, SCFloatArrayRef OutDown, int Index, int Length);
	
	
	// 409
	
	int SupportAttachedOrdersForTrading;
	
	
	// 412
	
	int GlobalTradeSimulationIsOn;
	int ChartTradeModeEnabled;//Version 946
	
	
	// 419
	
	int CancelAllOrdersOnEntriesAndReversals;
	
	
	// 421
	
	SCFloatArrayRef (SCDLLCALL* InternalEnvelopeFixed)(SCFloatArrayRef In, SCFloatArrayRef Out1, SCFloatArrayRef Out2,  float FixedValue, int Index);
	void* Unused52;
	
	
	// 429
	
	float CurrencyValuePerTick;
	
	
	// 434
	
	SCString ((SCDLLCALL* GetStudyNameFromChart)(int ChartNumber, int StudyID));
	
	
	// 436
	
	int MaximumPositionAllowed;
	
	
	// 450
	
	short ChartBarSpacing;
	
	
	// 453
	
	int (SCDLLCALL* InternalIsSwingHigh)(SCFloatArrayRef In, int Index, int Length);
	int (SCDLLCALL* InternalIsSwingLow)(SCFloatArrayRef In, int Index, int Length);
	
	SCFloatArrayRef (SCDLLCALL* InternalZigZag2)(SCFloatArrayRef InputDataHigh, SCFloatArrayRef InputDataLow, SCFloatArrayRef ZigZagValues, SCFloatArrayRef ZigZagPeakType, SCFloatArrayRef ZigZagPeakIndex, int Index, int NumberOfBars, float ReversalAmount);
	
	// 475
	
	int (SCDLLCALL* ModifyOrder)( s_SCNewOrder& OrderModifications);
	
	
	// 493
	
	SCFloatArrayRef (SCDLLCALL* InternalZigZag)(SCFloatArrayRef InputDataHigh, SCFloatArrayRef InputDataLow, SCFloatArrayRef  ZigZagValues,SCFloatArrayRef ZigZagPeakType, SCFloatArrayRef ZigZagPeakIndex , int Index, float ReversalPercent, float ReversalAmount);
	
	
	// 507
	
	int AllowEntryWithWorkingOrders;
	
	
	// 508
	
	SCFloatArrayRef (SCDLLCALL* InternalAwesomeOscillator)(SCFloatArrayRef In, SCFloatArrayRef Out, SCFloatArrayRef TempMA1, SCFloatArrayRef TempMA2, int Index, int Length1, int Length2);
	
	
	// 510
	
	int IsUserAllowedForSCDLLName;
	
	int (SCDLLCALL* FlattenAndCancelAllOrders)();
	
	
	// 519
	
	int CancelAllWorkingOrdersOnExit;
	
	
	// 521
	
	int (SCDLLCALL* SessionStartTime)();
	int (SCDLLCALL* IsDateTimeInSession)(const SCDateTime& DateTime);
	int (SCDLLCALL* TradingDayStartsInPreviousDate)();
	int (SCDLLCALL* GetTradingDayDate)(const SCDateTime& DateTime);
	double (SCDLLCALL* GetStartDateTimeForTradingDate)(int TradingDate);
	double (SCDLLCALL* GetTradingDayStartDateTimeOfBar)(const SCDateTime& BarDateTime);
	int (SCDLLCALL* SecondsSinceStartTime)(const SCDateTime& BarDateTime);
	
	
	// 530
	
	int HideStudy;
	
	
	// 538
	
	float ScaleValueOffset;
	float AutoScalePaddingPercentage;
	
	
	// 539
	
	SCString ((SCDLLCALL* GetChartName)(int ChartNumber));


	// 543

	int (SCDLLCALL* Unused_InternalGetServicePosition)(s_SCPositionData& PositionData);
	int (SCDLLCALL* InternalGetPosition)(s_SCPositionData& PositionData);
	
	
	// 544
	
	SCString Unused_SetAlertMessage;


	// 552
	
	void (SCDLLCALL* GetChartBaseData)(int ChartNumber, SCGraphData& BaseData);
	
	
	// 553
	
	void (SCDLLCALL* GetStudyArraysFromChart)(int ChartNumber, int StudyNumber, SCGraphData& GraphData);
	
	
	// 557
	
	void (SCDLLCALL* GetTimeAndSales)(SCTimeAndSalesArray& TSArray);
	
	
	// 560
	
	void * Old_VolumeAtPriceForBars;
	
	
	// 562
	
	int (SCDLLCALL* InternalFormattedEvaluateUsingDoubles)(double Value1, unsigned int Value1Format,
		OperatorEnum Operator,
		double Value2, unsigned int Value2Format,
		double PrevValue1, double PrevValue2,
		int* CrossDirection);
	
	
	// 569
	
	SCString TradeWindowConfigFileName;
	
	
	//594
	
	int StudyGraphInstanceID;
	
	
	//601
	
	int (SCDLLCALL* GetTradingDayDateForChartNumber)(int ChartNumber, const SCDateTime& DateTime);
	int (SCDLLCALL* GetFirstIndexForDate)(int ChartNumber, int Date);
	
	
	// 602
	
	int MaintainVolumeAtPriceData;
	
	SCString ((SCDLLCALL* FormatDateTime)(const SCDateTime& DateTime));
	
	
	// 613
	
	int DrawBaseGraphOverStudies;

	
	// 614
	unsigned int IntradayDataStorageTimeUnit;
	
	// 617
	int (SCDLLCALL* MakeHTTPRequest)(const SCString& URL);
	SCString HTTPResponse;
	
	void (SCDLLCALL* SetStudySubgraphDrawStyle)(int ChartNumber, int StudyID, int StudySubgraphNumber, int DrawStyle);
	
	// 627
	void (SCDLLCALL* GetStudyArrayFromChartUsingID)(const s_ChartStudySubgraphValues& ChartStudySubgraphValues, SCFloatArrayRef SubgraphArray);
	
	// 630
	SCFloatArrayRef (SCDLLCALL* InternalSlope)(SCFloatArrayRef In, SCFloatArrayRef Out, int Index);
	
	// 632
	void (SCDLLCALL* InternalSetAlert)(int AlertNumber, int Index, const SCString& Message);

	// 634
	int HideDLLAndFunctionNames;
	
	// 636
	SCFloatArray Open;
	SCFloatArray High;
	SCFloatArray Low;
	SCFloatArray Close;
	SCFloatArray Volume;
	SCFloatArray OpenInterest;
	SCFloatArray NumberOfTrades;
	SCFloatArray OHLCAvg;
	SCFloatArray HLCAvg;
	SCFloatArray HLAvg;
	SCFloatArray BidVolume;
	SCFloatArray AskVolume;
	SCFloatArray UpTickVolume;
	SCFloatArray DownTickVolume;
	SCFloatArray NumberOfBidTrades;
	SCFloatArray NumberOfAskTrades;



	// 641
	int DrawStudyUnderneathMainPriceGraph;

	// 649
	SCDateTime LatestDateTimeForLastBar;

	// 650
	int  GlobalDisplayStudyNameSubgraphNamesAndValues ;
	int DisplayStudyName;
	int DisplayStudyInputValues;
	
	// 652
	double TradeServiceAccountBalance;
	
	float ActiveToolYValue;
	
	// 657
	SCString ExternalServiceUsername;
	
	// 659
	int NewBarAtSessionStart;
	
	// 673
	double (SCDLLCALL* InternalConvertToSCTimeZone)(const SCDateTime& DateTime, const char* TimeZonePosixString);
	float LastTradePrice;
	
	// 676
	SCDateTime ScrollToDateTime;
	
	SCFloatArrayRef (SCDLLCALL* InternalDoubleStochastic)(SCBaseDataRef BaseDataIn, SCFloatArrayRef Out, SCFloatArrayRef MovAvgIn, SCFloatArrayRef MovAvgOut, 		SCFloatArrayRef MovAvgIn2, SCFloatArrayRef Unused, int Index, int Length, int EmaLength, int MovAvgType);
	
	// 690
	int SupportTradingScaleIn;
	int SupportTradingScaleOut;
	
	// 691
	int TradeWindowOrderQuantity;
	
	// 692
	int IsAutoTradingEnabled;
	
	// 693
	int CurrentlySelectedDrawingTool;
	int CurrentlySelectedDrawingToolState;
	
	// 703
	int ServerConnectionState;  // ServerConnectionStateEnum
	
	// 711
	int MaintainAdditionalChartDataArrays;
	
	// 712
	SCString GraphShortName;
	
	// 716
	void (SCDLLCALL* GetStudyArraysFromChartUsingID)(int ChartNumber, int StudyID, SCGraphData& GraphData);
	int (SCDLLCALL* Internal_GetStudyIDByIndex)(int ChartNumber, int StudyNumber);
	
	// 725
	double (SCDLLCALL* DateStringToSCDateTime)(const SCString& DateString);
	
	// 745
	int BaseGraphScaleRangeType;
	
	// 749
	double (SCDLLCALL* InternalGetStandardError)(SCFloatArrayRef In, int Index, int Length);
	
	// 754
	SCString DocumentationImageURL;

	// 766
	int BaseGraphGraphDrawType;
	
	// 786
	int (SCDLLCALL* UserDrawnChartDrawingExists)(int ChartNumber, int LineNumber);
	
	// 787
	void (SCDLLCALL* SetACSToolToolTip)(int ToolBarButtonNum, const char* ToolTip);
	void (SCDLLCALL* SetACSToolEnable)(int ToolBarButtonNum, bool Enable);
	int  (SCDLLCALL* AddACSChartShortcutMenuItem)(int ChartNumber, const char* MenuText);
	int (SCDLLCALL* RemoveACSChartShortcutMenuItem)(int ChartNumber, int MenuID);
	
	int MenuEventID;
	int PointerEventType;
	
	// 792
	int (SCDLLCALL* GetUserDrawingByLineNumber)(int ChartNumber, int LineNumber, s_UseTool& ChartDrawing, int DrawingType, int DrawingIndex);
	
	// 794
	SCString TradeAndCurrentQuoteSymbol;
	
	int GraphUsesChartColors;
	
	void (SCDLLCALL* GetBasicSymbolData)(const char* Symbol, s_SCBasicSymbolData& BasicSymbolData, bool Subscribe);
	
	// 798
	void (SCDLLCALL* SetACSToolButtonText)(int ToolBarButtonNum, const char* ButtonText);
	
	// 799
	double (SCDLLCALL* AdjustDateTimeToGMT)(const SCDateTime& DateTime);
	
	// 805
	int (SCDLLCALL* DeleteUserDrawnACSDrawing)(int ChartNumber, int LineNumber);
	
	// 806
	int (SCDLLCALL* UploadChartImage)();
	
	SCString ChartbookName;

	//808
	void (SCDLLCALL* RefreshTradeData)();

	//810
	SCFloatArrayRef (SCDLLCALL* InternalCumulativeDeltaVolume)(SCBaseDataRef BaseDataIn, SCFloatArrayRef Open, SCFloatArrayRef High, SCFloatArrayRef Low, SCFloatArrayRef Close, int Index, int ResetCumulativeCalculation);
	SCFloatArrayRef (SCDLLCALL* InternalCumulativeDeltaTicks)(SCBaseDataRef BaseDataIn, SCFloatArrayRef Open, SCFloatArrayRef High, SCFloatArrayRef Low, SCFloatArrayRef Close, int Index, int ResetCumulativeCalculation);

	// 813
	void (SCDLLCALL* GetStudyDataColorArrayFromChartUsingID)(int ChartNumber, int StudyID, int SubgraphIndex , SCColorArrayRef DataColorArray);

	// 817
	SCFloatArrayRef (SCDLLCALL* Internal_AroonIndicator)(SCFloatArrayRef FloatArrayInHigh, SCFloatArrayRef FloatArrayInLow, SCFloatArrayRef OutUp, SCFloatArrayRef OutDown, SCFloatArrayRef OutOscillator, int Index, int Length);


	// 821
	int  (SCDLLCALL* CreateRelayServer)(unsigned short Port, const char* pServiceCode);
	int  (SCDLLCALL* RelayNewSymbol)(SCString Symbol, int ValueFormat, float TickSize);
	int  (SCDLLCALL* RelayTradeUpdate)(SCString Symbol, double DateTime, float TradeValue, unsigned int TradeVolume);
	void (SCDLLCALL* RelayDataFeedAvailable)();
	void (SCDLLCALL* RelayDataFeedUnavailable)();

	// 822
	int  MaintainTradeStatisticsAndTradesData;

	// 823
	SCFloatArrayRef (SCDLLCALL* InternalMovingMedian)(SCFloatArrayRef In, SCFloatArrayRef Out, SCFloatArrayRef Temp, int Index, int Length);

	// 826
	s_PersistentVariables const* (SCDLLCALL* GetStudyPersistentVariablesFromChartUsingID)(int ChartNumber, int StudyID);
	int (SCDLLCALL* ChartIsDownloadingHistoricalData)(int ChartNumber);
	int (SCDLLCALL* RelayServerConnected)();

	// 828
	void (SCDLLCALL* CreateProfitLossDisplayString)(double ProfitLoss, int Quantity, PositionProfitLossDisplayEnum ProfitLossFormat, SCString& Result);
	
	// 829
	int FlagFullRecalculate;
	
	// 830
	double (SCDLLCALL* InternalGetStartOfPeriodForDateTime)(double DateTime, unsigned int TimePeriodType, int TimePeriodLength, int PeriodOffset, int NewPeriodAtBothSessionStarts );
	
	// 834
	c_VAPContainer *VolumeAtPriceForBars;
	
	// 841
	double (SCDLLCALL* TimePeriodSpan)(unsigned int TimePeriodType, int TimePeriodLength);
	
	// 860
	int (SCDLLCALL* OpenChartOrGetChartReference)(s_ACSOpenChartParameters& Parameters);
	
	// 868
	int ProtectStudy;
	
	// 874
	int RenkoTicksPerBar;
	
	// 875
	float RealTimePriceMultiplier;
	float HistoricalPriceMultiplier;
	
	// 878
	int (SCDLLCALL* GetTradeStatisticsForSymbol)(int Simulated, int DailyStats, s_ACSTradeStatistics& TradeStats);
	
	// 879
	int ConnectToExternalServiceServer;
	int ReconnectToExternalServiceServer;
	int DisconnectFromExternalServiceServer;

	// 892
	int (SCDLLCALL* IsDateTimeInDaySession) ( const SCDateTime &DateTime);
	
	// 895
	int DeltaVolumePerBar;

	// 896
	int  AlertConditionFlags; 
	
	// 896
	bool (SCDLLCALL* GetTimeAndSalesForSymbol)(const SCString& Symbol, SCTimeAndSalesArray& TSArray);

	// 905
	unsigned int StudyVersion;

	// 909
	int AdvancedFeaturesLevel2;
	void (SCDLLCALL* SetNumericInformationGraphDrawTypeConfig)(const s_NumericInformationGraphDrawTypeConfig& NumericInformationGraphDrawTypeConfig);

	// 913
	double ChartTradingOrderPrice;
	int UseGUIAttachedOrderSetting;
	int BasedOnGraphValueFormat;

	int  RenkoTrendOpenOffsetInTicks;
	int  RenkoReversalOpenOffsetInTicks;

	// 915
	SCFloatArrayRef (SCDLLCALL* InternalCumulativeDeltaTickVolume)(SCBaseDataRef BaseDataIn, SCFloatArrayRef Open, SCFloatArrayRef High, SCFloatArrayRef Low, SCFloatArrayRef Close, int Index, int ResetCumulativeCalculation);

	// 917
	__int64 DLLNameUserServiceLevel;

	// 918
	unsigned int (SCDLLCALL* CombinedForegroundBackgroundColorRef)(COLORREF ForegroundColor, COLORREF BackgroundColor);
	
	// 921
	SCFloatArrayRef (SCDLLCALL* InternalAccumulationDistribution)(SCBaseDataRef BaseDataIn, SCFloatArrayRef Out, int Index);

	// 922
	int SaveChartImageToFile;

	// 926
	int (SCDLLCALL* ACSToolBlocksChartValuesTool)(int Block);

	// 930
	int (SCDLLCALL* SetStudyVisibilityState)(int StudyID, int Visible);

	c_VAPContainer *HistoricalHighPullbackVolumeAtPriceForBars;

	// 931
	int ReversalTicksPerBar;

	// 933
	int  RenkoNewBarWhenExceeded;

	// 934
	c_VAPContainer *HistoricalLowPullbackVolumeAtPriceForBars;

	// 937
	void (SCDLLCALL* Unused_GetTradeList)(std::vector<s_ACSTrade>& TradeList);

	int  ReceivePointerEvents;

	// 938
	c_VAPContainer *PullbackVolumeAtPrice;

	//939
	int (SCDLLCALL* GetNearestTargetOrder)(s_SCTradeOrder& OrderDetails);
	int (SCDLLCALL* GetNearestStopOrder)(s_SCTradeOrder& OrderDetails);

	// 943
	int (SCDLLCALL* FlattenPosition)();

	// 945
	int (SCDLLCALL* BarIndexToXPixelCoordinate)(int Index);

	// 952
	int (SCDLLCALL* RegionValueToYPixelCoordinate)(float RegionValue, int ChartRegionNumber);
	
	// 962
	int (SCDLLCALL* SetACSChartShortcutMenuItemDisplayed)(int ChartNumber, int MenuItemID, bool DisplayItem);
	int (SCDLLCALL* SetACSChartShortcutMenuItemEnabled) (int ChartNumber, int MenuItemID, bool Enabled);
	int (SCDLLCALL* SetACSChartShortcutMenuItemChecked)(int ChartNumber, int MenuItemID, bool Checked);

	// 964 
	fp_ACSGDIFunction p_GDIFunction;

	// 968
	unsigned int VolumeValueFormat;

	int (SCDLLCALL* GetStudyVisibilityState)(int StudyID);

	// 970
	int (SCDLLCALL* InternalSubmitOCOOrder)(s_SCNewOrder& NewOrder, int Index);

	float BaseGraphScaleConstRange;

	// 983
	int ACSVersion;  // This is set to the header version the custom study was compiled with

	int (SCDLLCALL* MakeHTTPBinaryRequest)(const SCString& URL);
	SCConstCharArray HTTPBinaryResponse;

	// 985
	int (SCDLLCALL* GetTradeListEntry)(unsigned int TradeIndex, s_ACSTrade& TradeEntry);

	// 1000
	void (SCDLLCALL* GetStudyExtraArrayFromChartUsingID)(int ChartNumber, int StudyID, int SubgraphIndex, int ExtraArrayIndex, SCFloatArrayRef ExtraArrayRef);

	// 1005
	int (SCDLLCALL* GetTradeListSize)();
	
	// 1014
	int OnExternalDataImmediateStudyCall;
	double (SCDLLCALL* YPixelCoordinateToGraphValue)(int YPixelCoordinate);

	// 1037
	int ContinuousFuturesContractLoading;

	// 1044
	bool PlaceACSChartShortcutMenuItemsAtTopOfMenu;
	int (SCDLLCALL* AddACSChartShortcutMenuSeparator)(int ChartNumber);

	// 1055
	int UsesMarketDepthData;

	float BaseGraphScaleRangeTop;
	float BaseGraphScaleRangeBottom;
	float BaseGraphScaleValueOffset;
	float BaseGraphAutoScalePaddingPercentage;

	// 1076
	int StudyRegionLeftCoordinate;
	int StudyRegionRightCoordinate;

	// 1086
	void (SCDLLCALL* SetNumericInformationDisplayOrderFromString)(const SCString& CommaSeparatedDisplayOrder);

	//1091
	SCString CustomChartTitleBarName;

	// 1129
	SCFloatArrayRef (SCDLLCALL* InternalCCI)(SCFloatArrayRef In, SCFloatArrayRef SMAOut, SCFloatArrayRef CCIOut, int Index, int Length, float Multiplier, unsigned int MovingAverageType);
	int (SCDLLCALL* GetOrderFillSize)();
	int (SCDLLCALL* GetOrderFillEntry)(unsigned int FillIndex, s_SCOrderFillData& FillData);

	//1142
	int CancelAllOrdersOnReversals;

	//1147
	SCFloatArrayRef (SCDLLCALL* InternalResettableZigZag2)(SCFloatArrayRef InputDataHigh, SCFloatArrayRef InputDataLow, SCFloatArrayRef ZigZagValues, SCFloatArrayRef ZigZagPeakType, SCFloatArrayRef ZigZagPeakIndex, int StartIndex, int Index, int NumberOfBars, float ReversalAmount);
	SCFloatArrayRef (SCDLLCALL* InternalResettableZigZag)(SCFloatArrayRef InputDataHigh, SCFloatArrayRef InputDataLow, SCFloatArrayRef  ZigZagValues,SCFloatArrayRef ZigZagPeakType, SCFloatArrayRef ZigZagPeakIndex , int StartIndex, int Index, float ReversalPercent, float ReversalAmount);

	//1148
	SCString ((SCDLLCALL * DateTimeToString)(const double &DateTime,int Flags));

	//1149
	SCDateTimeMS CurrentSystemDateTimeMS;

	//1166
	float FilterChartVolumeGreaterThanEqualTo;
	float FilterChartVolumeLessThanEqualTo;

	//1174
	SCString TitleBarName;

	//1177
	int PriceChangesPerBar;

	//1190
	int ResetAllScales;

	//1193
	SCString ((SCDLLCALL* FormatVolumeValue)(__int64 Volume, int VolumeValueFormat, bool UseLargeNumberSuffix));
	bool FilterChartVolumeTradeCompletely;

	//1197

	int ChartRegion1TopCoordinate ;
	int ChartRegion1BottomCoordinate ;
	int ChartRegion1LeftCoordinate;
	int ChartRegion1RightCoordinate;
	
	

	// When adding new functions that use arrays as parameters, always use
	// SCFloatArrayRef and not SCSubgraphRef.  All required
	// internal arrays must be passed in as parameters.  Write a wrapper
	// function that takes a SCSubgraphRef and pass into the Internal function the
	// necessary internal arrays.  This ensures back compatibility if there are changes to the
	// Subgraph structure and it allows our functions to also be used
	// internally in Sierra Chart where Subgraph internal arrays are not automatically
	// supported.
	
	// Always update SC_DLL_VERSION when updating the interface
};

/*************************************************************/


#define SCStudyGraphRef SCStudyInterfaceRef

#pragma pack(pop)

#endif

#ifndef _SCCONSTANTS_H_
#define _SCCONSTANTS_H_

const int SC_OPEN		= 0;
const int SC_HIGH		= 1;
const int SC_LOW		= 2;
const int SC_LAST		= 3;  // Close
const int SC_VOLUME		= 4;
const int SC_OI			= 5;  // Open Interest
const int SC_NT			= 5;  // Number of Trades
const int SC_OHLC		= 6;  // Average of Open, High, Low, Last
const int SC_HLC		= 7;  // Average of High, Low, Last
const int SC_HL			= 8;  // Average of High, Low
const int SC_BIDVOL		= 9;
const int SC_ASKVOL		= 10;
const int SC_UPVOL		= 11;  // UpTick Volume
const int SC_DOWNVOL	= 12;  // DownTick Volume
const int SC_BIDNT		= 13;
const int SC_ASKNT		= 14;
const int SC_ASKBID_DIFF_HIGH		= 15;//Maximum difference of ask volume and bid volume for a bar
const int SC_ASKBID_DIFF_LOW		= 16;//Minimum difference of ask volume and bid volume for a bar
const int SC_ASKBID_NT_DIFF_HIGH	= 17; //maximum difference of ask number of trades and bid number of trades for a bar
const int SC_ASKBID_NT_DIFF_LOW		= 18; //minimum difference of ask number of trades and bid number of trades for a bar

const int SC_UPDOWN_VOL_DIFF_HIGH	= 19;//Maximum difference of uptick volume and downtick volume for a bar
const int SC_UPDOWN_VOL_DIFF_LOW	= 20;//Minimum difference of uptick volume and downtick volume for a bar

const int SC_RENKO_OPEN				= 21;
const int SC_RENKO_CLOSE			= 22;

const int NUM_BASE_GRAPH_ARRAYS = 11;
const int NUM_BASE_GRAPH_ARRAYS_WITH_VOLUME_ARRAYS = 21;
const int NUM_BASE_GRAPH_ARRAYS_WITH_ADDITIONAL_ARRAYS = 23;

const int NUM_BASE_GRAPHS_FOR_HISTORICAL_CHART = 11;

const int PF_NUM_BOXES_ARRAY = NUM_BASE_GRAPH_ARRAYS_WITH_ADDITIONAL_ARRAYS + 0;
const int PF_DIRECTION_ARRAY = NUM_BASE_GRAPH_ARRAYS_WITH_ADDITIONAL_ARRAYS + 1;
const int PF_TRUELAST_ARRAY  = NUM_BASE_GRAPH_ARRAYS_WITH_ADDITIONAL_ARRAYS + 2;

const int TLB_LBOPEN_ARRAY   = NUM_BASE_GRAPH_ARRAYS_WITH_ADDITIONAL_ARRAYS + 0;
const int TLB_REVERSE_ARRAY  = NUM_BASE_GRAPH_ARRAYS_WITH_ADDITIONAL_ARRAYS + 1;
const int TLB_CONTINUE_ARRAY = NUM_BASE_GRAPH_ARRAYS_WITH_ADDITIONAL_ARRAYS + 2;

const int SC_NO = 0;
const int SC_YES = 1;

const int MAX_STUDY_LENGTH = 1000000;

// Used in s_ChartDrawing
// These are written to disk
enum DrawingTypeEnum
{ DRAWING_UNKNOWN = 0
, DRAWING_LINE = 1
, DRAWING_RAY = 2
, DRAWING_HORIZONTALLINE = 3
, DRAWING_VERTICALLINE = 4
, DRAWING_ARROW = 5
, DRAWING_TEXT = 6
, DRAWING_CALCULATOR = 7
, DRAWING_RETRACEMENT = 8
, DRAWING_UNUSED = 9           // old fan drawing type
, DRAWING_PROJECTION = 11
, DRAWING_RECTANGLEHIGHLIGHT  = 12
, DRAWING_ELLIPSEHIGHLIGHT = 13
, DRAWING_FAN_GANN = 14
, DRAWING_PITCHFORK = 15
, DRAWING_UNUSED16 = 16      // was DRAWING_FLOWELLIPSEHIGHLIGHT
, DRAWING_WORKING_ORDER = 17
, DRAWING_POSITION_LINE = 18
, DRAWING_CYCLE = 19
, DRAWING_TIME_EXPANSION = 20
, DRAWING_GANNGRID = 21
, DRAWING_UNUSED_22 = 22   
, DRAWING_UNUSED_23 = 23 
, DRAWING_ORDER_FILL = 24
, DRAWING_ENTRYEXIT_CONNECTLINE = 25
, DRAWING_RECTANGLE_EXT_HIGHLIGHT = 26
, DRAWING_FAN_FIBONACCI = 27
, DRAWING_PARALLEL_LINES = 28
, DRAWING_PARALLEL_RAYS = 29
, DRAWING_LINEAR_REGRESSION = 30
, DRAWING_RAFF_REGRESSION_CHANNEL = 31
, DRAWING_EXTENDED_LINE = 32
, DRAWING_PITCHFORK_SCHIFF = 33
, DRAWING_PITCHFORK_MODIFIED_SCHIFF = 34
, DRAWING_EXPANSION = 35
, DRAWING_VOLUME_PROFILE = 36
, DRAWING_STATIONARY_TEXT = 37
, DRAWING_TIME_PROJECTION = 38
, DRAWING_MARKER = 39
, DRAWING_HORIZONTAL_RAY = 40
, DRAWING_REWARD_RISK = 41
, DRAWING_SWING_MARKER = 42
, DRAWING_DATE_MARKER = 43
, DRAWING_OHLC_RAY = 44
, DRAWING_HORIZONTAL_LEVEL = 45

, SC_NUM_DRAWING_TYPE
};



// Constants for s_UseTool - Chart Tools
const int TOOL_DELETE_ALL				= -1;
const int TOOL_DELETE_CHARTDRAWING		= 0;

const int TOOL_TRENDLINE				= 1;  // depreciated, use TOOL_LINE instead
const int TOOL_EXTENDING_LINE			= 2;  // depreciated, use TOOL_RAY  instead
const int TOOL_RETRACE_EXTENSION		= 3;  // depreciated, use TOOL_RETRACEMENT instead

const int TOOL_CHART_CALC				= 5;
const int TOOL_HORIZONTAL				= 6;
const int TOOL_TEXT						= 7;
const int TOOL_ADJUST					= 8;
const int TOOL_VERTICAL					= 9;
const int TOOL_ARROW					= 10;
const int TOOL_UNDEFINED				= 11;
const int TOOL_RECTANGLE_HIGHLIGHT		= 13;
const int TOOL_EXT_RECTANGLE_HIGHLIGHT	= 14;
const int TOOL_ELLIPSE_HIGHLIGHT		= 15;
const int TOOL_LINE						= TOOL_TRENDLINE;
const int TOOL_RAY						= TOOL_EXTENDING_LINE;
const int TOOL_EXTENDED_LINE			= 16;
const int TOOL_HORIZONTAL_RAY			= 17;
const int TOOL_RETRACEMENT				= TOOL_RETRACE_EXTENSION;
const int TOOL_EXPANSION				= 18;
const int TOOL_PROJECTION				= 19;
const int TOOL_PITCHFORK				= 20;
const int TOOL_PITCHFORK_SCHIFF			= 21;
const int TOOL_PITCHFORK_MODIFIED_SCHIFF= 22;
const int TOOL_TIME_EXPANSION			= 23;
const int TOOL_TIME_PROJECTION			= 24;
const int TOOL_PARALLEL_LINES			= 25;
const int TOOL_PARALLEL_RAYS			= 26;
const int TOOL_LINEAR_REGRESSION		= 27;
const int TOOL_RAFF_REGRESSION_CHANNEL	= 28;
const int TOOL_MARKER					= 29;
const int TOOL_HORIZONTAL_LEVEL			= 30;

const int MARKER_POINT			= 0;
const int MARKER_DASH			= 1;
const int MARKER_SQUARE			= 2;
const int MARKER_STAR			= 3;
const int MARKER_PLUS			= 4;
const int MARKER_X				= 5;
const int MARKER_ARROWUP		= 6;
const int MARKER_ARROWDOWN		= 7;
const int MARKER_ARROWRIGHT		= 8;
const int MARKER_ARROWLEFT		= 9;
const int MARKER_TRIANGLEUP		= 10;
const int MARKER_TRIANGLEDOWN	= 11;
const int MARKER_TRIANGLERIGHT	= 12;
const int MARKER_TRIANGLELEFT	= 13;
const int MARKER_DIAMOND		= 14;

const int TIME_EXP_LABEL_NONE			= 0;
const int TIME_EXP_LABEL_PERCENT		= 1;
const int TIME_EXP_LABEL_BARS			= 2;
const int TIME_EXP_LABEL_PERCENTBARS	= 3;
const int TIME_EXP_LABEL_BARSPERCENT	= 4;
const int TIME_EXP_LABEL_DATE			= 5;
const int TIME_EXP_LABEL_TIME			= 6;
const int TIME_EXP_LABEL_DATETIME		= 7;

const int TIME_EXP_EXTENDED		= 0;
const int TIME_EXP_STANDARD		= 1;
const int TIME_EXP_COMPRESSED	= 2;

// Constants for ACS ToolBar buttons and Pointer interaction
const int SC_ACS_TOOL1  = 1;
const int SC_ACS_TOOL2  = 2;
const int SC_ACS_TOOL3  = 3;
const int SC_ACS_TOOL4  = 4;
const int SC_ACS_TOOL5  = 5;
const int SC_ACS_TOOL6  = 6;
const int SC_ACS_TOOL7  = 7;
const int SC_ACS_TOOL8  = 8;
const int SC_ACS_TOOL9  = 9;
const int SC_ACS_TOOL10 = 10;
const int SC_ACS_TOOL11 = 11;
const int SC_ACS_TOOL12 = 12;
const int SC_ACS_TOOL13 = 13;
const int SC_ACS_TOOL14 = 14;
const int SC_ACS_TOOL15 = 15;
const int SC_ACS_TOOL16 = 16;
const int SC_ACS_TOOL17 = 17;
const int SC_ACS_TOOL18 = 18;
const int SC_ACS_TOOL19 = 19;
const int SC_ACS_TOOL20 = 20;
const int SC_ACS_TOOL21 = 21;
const int SC_ACS_TOOL22 = 22;
const int SC_ACS_TOOL23 = 23;
const int SC_ACS_TOOL24 = 24;
const int SC_ACS_TOOL25 = 25;
const int SC_ACS_TOOL26 = 26;
const int SC_ACS_TOOL27 = 27;
const int SC_ACS_TOOL28 = 28;
const int SC_ACS_TOOL29 = 29;
const int SC_ACS_TOOL30 = 30;
const int SC_ACS_TOOL31 = 31;
const int SC_ACS_TOOL32 = 32;
const int SC_ACS_TOOL33 = 33;
const int SC_ACS_TOOL34 = 34;
const int SC_ACS_TOOL35 = 35;
const int SC_ACS_TOOL36 = 36;
const int SC_ACS_TOOL37 = 37;
const int SC_ACS_TOOL38 = 38;
const int SC_ACS_TOOL39 = 39;
const int SC_ACS_TOOL40 = 40;
const int SC_ACS_TOOL41 = 41;
const int SC_ACS_TOOL42 = 42;
const int SC_ACS_TOOL43 = 43;
const int SC_ACS_TOOL44 = 44;
const int SC_ACS_TOOL45 = 45;
const int SC_ACS_TOOL46 = 46;
const int SC_ACS_TOOL47 = 47;
const int SC_ACS_TOOL48 = 48;
const int SC_ACS_TOOL49 = 49;
const int SC_ACS_TOOL50 = 50;

const int SC_POINTER_BUTTON_DOWN   = 1;
const int SC_POINTER_MOVE   = 2;
const int SC_POINTER_BUTTON_UP     = 3;
const int SC_ACS_TOOL_ON  = 4;
const int SC_ACS_TOOL_OFF = 5;


// These constants for sc.Subgraph[i].AutoColoring
enum AutoColoringEnum
{ AUTOCOLOR_NONE = 0
, AUTOCOLOR_SLOPE
, AUTOCOLOR_POSNEG
, AUTOCOLOR_BASEGRAPH
, AUTOCOLOR_GRADIENT

, NUM_AUTOCOLOR_OPTIONS
};


enum SubgraphDrawStyles
{ DRAWSTYLE_LINE
, DRAWSTYLE_BAR
, DRAWSTYLE_POINT
, DRAWSTYLE_DASH
, DRAWSTYLE_HIDDEN
, DRAWSTYLE_IGNORE
, DRAWSTYLE_STAIR
, DRAWSTYLE_SQUARE
, DRAWSTYLE_STAR
, DRAWSTYLE_PLUS
, DRAWSTYLE_ARROWUP
, DRAWSTYLE_ARROWDOWN
, DRAWSTYLE_ARROWLEFT
, DRAWSTYLE_ARROWRIGHT
, DRAWSTYLE_FILLTOP
, DRAWSTYLE_FILLBOTTOM
, DRAWSTYLE_FILLRECTTOP
, DRAWSTYLE_FILLRECTBOTTOM
, DRAWSTYLE_COLORBAR
, DRAWSTYLE_BOXTOP
, DRAWSTYLE_BOXBOTTOM
, DRAWSTYLE_COLORBARHOLLOW
, DRAWSTYLE_COLORBAR_CANDLEFILL
, DRAWSTYLE_CUSTOM_TEXT
, DRAWSTYLE_BARTOP
, DRAWSTYLE_BARBOTTOM
, DRAWSTYLE_LINE_SKIPZEROS
, DRAWSTYLE_FILLTOP_TRANSPARENT
, DRAWSTYLE_FILLBOTTOM_TRANSPARENT
, DRAWSTYLE_TEXT
, DRAWSTYLE_POINTLOW
, DRAWSTYLE_POINTHIGH
, DRAWSTYLE_TRIANGLEUP
, DRAWSTYLE_TRIANGLEDOWN
, DRAWSTYLE_FILLRECTTOP_TRANSPARENT
, DRAWSTYLE_FILLRECTBOTTOM_TRANSPARENT
, DRAWSTYLE_BACKGROUND
, DRAWSTYLE_DIAMOND
, DRAWSTYLE_LEFTHASH
, DRAWSTYLE_RIGHTHASH
, DRAWSTYLE_TRIANGLELEFT
, DRAWSTYLE_TRIANGLERIGHT
, DRAWSTYLE_TRIANGLERIGHTOFFSET
, DRAWSTYLE_TRIANGLERIGHTOFFSETB
, DRAWSTYLE_CANDLE_BODYOPEN
, DRAWSTYLE_CANDLE_BODYCLOSE
, DRAWSTYLE_FILLTOZERO
, DRAWSTYLE_FILLTOZERO_TRANSPARENT
, DRAWSTYLE_SQUAREOFFSETLEFT
, DRAWSTYLE_SQUAREOFFSETLEFTB
, DRAWSTYLE_VALUE_ON_HIGH
, DRAWSTYLE_VALUE_ON_LOW
, DRAWSTYLE_VALUE_OF_SUBGRAPH
, DRAWSTYLE_SUBGRAPH_NAME_AND_VALUES_ONLY
, NUM_SUBGRAPH_STYLES
};

enum SubgraphLineStyles
{ LINESTYLE_SOLID
, LINESTYLE_DASH
, LINESTYLE_DOT
, LINESTYLE_DASHDOT
, LINESTYLE_DASHDOTDOT

, NUM_LINESTYLES
};

enum SubgraphLineLabelFlags
{ LL_DISPLAY_NAME				= 0x00000001
, LL_NAME_ALIGN_CENTER			= 0x00000002
, LL_NAME_ALIGN_FAR_RIGHT		= 0x00000004
, LL_NAME_ALIGN_ABOVE			= 0x00000008
, LL_NAME_ALIGN_BELOW			= 0x00000010
, LL_NAME_ALIGN_RIGHT			= 0x00000020
, LL_NAME_ALIGN_VALUES_SCALE	= 0x00000040
, LL_NAME_ALIGN_LEFT_EDGE		= 0x00000080
, LL_DISPLAY_VALUE				= 0x00000100
, LL_VALUE_ALIGN_CENTER			= 0x00000200
, LL_VALUE_ALIGN_FAR_RIGHT		= 0x00000400
, LL_VALUE_ALIGN_ABOVE			= 0x00000800
, LL_VALUE_ALIGN_BELOW			= 0x00001000
, LL_VALUE_ALIGN_RIGHT			= 0x00002000
, LL_VALUE_ALIGN_VALUES_SCALE	= 0x00004000
, LL_VALUE_ALIGN_LEFT_EDGE		= 0x00008000
, LL_NAME_ALIGN_LEFT			= 0x00010000
, LL_VALUE_ALIGN_LEFT			= 0x00020000
};

enum SubgraphNameAndValueDisplayFlags
{ SNV_DISPLAY_IN_WINDOWS	= 0x00000001
, SNV_DISPLAY_IN_DATA_LINE	= 0x00000002
};


enum ChartDataTypeEnum
{ NO_DATA_TYPE = 0
, DAILY_DATA = 1
, INTRADAY_DATA = 2
};

enum ScaleTypeEnum
{ SCALE_LINEAR
, SCALE_LOGARITHMIC
};

enum ScaleRangeTypeEnum
{ SCALE_AUTO				// 0
, SCALE_USERDEFINED			// 1
, SCALE_INDEPENDENT			// 2
, SCALE_SAMEASREGION		// 3
, SCALE_CONSTRANGE			// 4
, SCALE_CONSTRANGECENTER	// 5
, SCALE_ZEROBASED			// 6
};

enum MovAvgTypeEnum
{ MOVAVGTYPE_EXPONENTIAL
, MOVAVGTYPE_LINEARREGRESSION
, MOVAVGTYPE_SIMPLE
, MOVAVGTYPE_WEIGHTED
, MOVAVGTYPE_WILDERS
, MOVAVGTYPE_SIMPLE_SKIP_ZEROS
, MOVAVGTYPE_SMOOTHED
, MOVAVGTYPE_NUMBER_OF_AVERAGES
};

enum
{ BHCS_BAR_HAS_CLOSED = 2
, BHCS_BAR_HAS_NOT_CLOSED = 3
, BHCS_SET_DEFAULTS = 4
};

// These constants are used to set the calculation precedence through the
// sc.CalculationPrecedence member variable.
enum PrecedenceLevelEnum
{ STD_PREC_LEVEL = 0
, LOW_PREC_LEVEL = 1
, VERY_LOW_PREC_LEVEL = 2
};

enum UseToolAddMethodEnum
{ UTAM_NOT_SET  // AddMethod is not used. Instead AddIfExists is used (for backwards compatibility).
, UTAM_ADD_ALWAYS  // Adds a new drawing, even if another drawing with the same line number already exists.
, UTAM_ADD_ONLY_IF_NEW  // Only adds the drawing if it does not already exist on the chart
, UTAM_ADD_OR_ADJUST  // If the drawing exists, then it will be adjusted, otherwise it will be added
};



// Constants for s_TimeAndSales record Level:
const int SC_TS_MARKER			= 0;  // Indicates a gap in the time and sales data
const int SC_TS_BID				= 1;  // Trade is considered to have occurred at Bid price or lower
const int SC_TS_ASK				= 2;  // Trade is considered to have occurred at Ask price or higher
const int SC_TS_INSIDE			= 3;  // Unused
const int SC_TS_BIDASKVALUES	= 6;  // Bid and Ask quote data update

const int  COUNTDOWN_TIMER_CHART_DRAWINGNUMBER = 40338335;


enum InputValueTypes
{ NO_VALUE
, OHLC_VALUE                // IndexValue
, FLOAT_VALUE                // FloatValue
, STUDYINDEX_VALUE            // IndexValue
, SUBGRAPHINDEX_VALUE        // IndexValue
, YESNO_VALUE                // BooleanValue
, MOVAVGTYPE_VALUE            // IndexValue
, UNUSED7
, DATE_VALUE                // DateTimeValue
, TIME_VALUE                // DateTimeValue
, DATETIME_VALUE            // DateTimeValue
, INT_VALUE                    // IntValue
, UNUSED12
, STUDYID_VALUE                // IndexValue
, COLOR_VALUE                // ColorValue
, ALERT_SOUND_NUMBER_VALUE    // IndexValue (0 = No Alert, 1 = Alert 1, 2 = Alert 2, ...)
, CANDLESTICK_PATTERNS_VALUE    // IndexValue
, TIME_PERIOD_LENGTH_UNIT_VALUE    // IndexValue
, CHART_STUDY_SUBGRAPH_VALUES    // ChartNum, StudyID, SubgraphIndex
, CHART_NUMBER                // IntValue
, STUDY_SUBGRAPH_VALUES        // StudyID, SubgraphIndex
, CHART_STUDY_VALUES        // ChartNum, StudyID
, CUSTOM_STRING_VALUE        // IndexValue
, DOUBLE_VALUE                // DoubleValue
, TIMEZONE_VALUE            // IndexValue
, TIME_WITH_TIMEZONE_VALUE    // DateTimeValue, TimeZoneValue
, STRING_VALUE                // StringValue
};


enum EnumTimePeriodLengthUnit
{ TIME_PERIOD_LENGTH_UNIT_MINUTES
, TIME_PERIOD_LENGTH_UNIT_DAYS
, TIME_PERIOD_LENGTH_UNIT_WEEKS
, TIME_PERIOD_LENGTH_UNIT_MONTHS
, TIME_PERIOD_LENGTH_UNIT_YEARS
, TIME_PERIOD_LENGTH_UNIT_NUM_BARS

, TIME_PERIOD_LENGTH_UNIT_COUNT
};

/*==========================================================================*/
inline const char* TimePeriodTypeToString(int TimePeriodType)
{
	switch (TimePeriodType)
	{
		case TIME_PERIOD_LENGTH_UNIT_MINUTES:
		return "Minutes";
		
		case TIME_PERIOD_LENGTH_UNIT_DAYS:
		return "Days";
		
		case TIME_PERIOD_LENGTH_UNIT_WEEKS:
		return "Weeks";
		
		case TIME_PERIOD_LENGTH_UNIT_MONTHS:
		return "Months";
		
		case TIME_PERIOD_LENGTH_UNIT_YEARS:
		return "Years";
		
		default:
		return "";
	};
}


// These constants are for the CrossOver function
const int CROSS_FROM_TOP = 1;
const int CROSS_FROM_BOTTOM = -1;
const int NO_CROSS = 0;


enum OperatorEnum
{ NOT_EQUAL_OPERATOR
, LESS_EQUAL_OPERATOR
, GREATER_EQUAL_OPERATOR
, CROSS_EQUAL_OPERATOR
, CROSS_OPERATOR
, EQUAL_OPERATOR
, LESS_OPERATOR
, GREATER_OPERATOR

, NUM_OPERATORS
};


const int SC_SUBGRAPHS_AVAILABLE = 60;
const int SC_INPUTS_AVAILABLE = 128;
const int SC_DRAWING_MAX_LEVELS = 32;  // Don't change this, used in s_ChartDrawing and s_ConfigData

const int NUM_LINREG_TOOL_STD_DEVIATIONS = 12;


// constants for GraphDrawType
enum GraphDrawTypeEnum
{ GDT_CUSTOM = 0
, GDT_OHLCBAR
, GDT_CANDLESTICK
, GDT_CANDLESTICK_BODY_ONLY
, GDT_LINEONCLOSE
, GDT_MOUNTAIN
, GDT_HLCBAR
, GDT_LINEONOPEN
, GDT_LINEONHLAVG
, GDT_STAIRSTEPONCLOSE
, GDT_HLBAR
, GDT_TPO_PROFILE
, GDT_KAGI
, GDT_BOX_UNUSED
, GDT_POINT_AND_FIGURE_BARS
, GDT_VOLUME_BY_PRICE
, GDT_POINT_AND_FIGURE_XO
, GDT_BID_ASK_BAR
, GDT_PRICE_VOLUME
, GDT_CANDLE_VOLUME_BAR
, GDT_ASK_BID_DIFF_NUMBER_BAR  // rem
, GDT_VOLUME_NUMBER_BAR  //
, GDT_TRADES_NUMBER_BAR  //
, GDT_BID_ASK_NUMBER_BAR  //
, GDT_ASK_BID_VOL_DIFF_BAR  //
, GDT_BLANK
, GDT_NUMBERS_BARS
, GDT_NUMERIC_INFORMATION
, GDT_RENKO
, GDT_RENKO_WICK
, GDT_CANDLESTICK_HOLLOW

// if new types are added, be sure to update GRAPHTYPE_STRING
, NUM_GRAPH_DRAW_TYPES
};

const int VALUEFORMAT_MAXDIGITS = 19;
const int VALUEFORMAT_TIME = 20;
const int VALUEFORMAT_DATE = 21;
const int VALUEFORMAT_DATETIME = 22;
const int VALUEFORMAT_CURRENCY = 23;
const int VALUEFORMAT_PERCENT = 24;
const int VALUEFORMAT_SCIENTIFIC = 25;
const int VALUEFORMAT_INHERITED = 40;
const int VALUEFORMAT_2_DENOMINATOR = 102;
	
//Server connection state constants.   These relate to File >> Connect to Data Feed.
enum ServerConnectionStateEnum
{ SCS_DISCONNECTED = 0
, SCS_CONNECTING = 1
, SCS_CONNECTED = 2
, SCS_CONNECTION_LOST = 3
, SCS_DISCONNECTING = 4
, SCS_RECONNECTING =5
};

/****************************************************************************/
enum PositionProfitLossDisplayEnum
{ PPLD_DO_NOT_DISPLAY =0
, PPLD_CURRENCY_VALUE
, PPLD_POINTS
, PPLD_POINTS_IGNORE_QUANTITY
, PPLD_TICKS
, PPLD_TICKS_IGNORE_QUANTITY
, PPLD_CV_AND_POINTS
, PPLD_CV_AND_POINTS_IGNORE_QUANTITY
, PPLD_CV_AND_TICKS
, PPLD_CV_AND_TICKS_IGNORE_QUANTITY
};

/****************************************************************************/
// Time Zones

enum TimeZonesEnum
{ TIMEZONE_NOT_SPECIFIED = 0
, TIMEZONE_HONOLULU  // Hawaii
, TIMEZONE_ANCHORAGE  // Alaska
, TIMEZONE_LOS_ANGELES  // US Pacific
, TIMEZONE_PHOENIX
, TIMEZONE_DENVER  // US Mountain
, TIMEZONE_CHICAGO  // US Central
, TIMEZONE_NEW_YORK  // US Eastern
, TIMEZONE_HALIFAX
, TIMEZONE_UTC
, TIMEZONE_LONDON
, TIMEZONE_BRUSSELS
, TIMEZONE_ATHENS
, TIMEZONE_BAGHDAD
, TIMEZONE_MOSCOW
, TIMEZONE_DUBAI
, TIMEZONE_ISLAMABAD
, TIMEZONE_NEW_DELHI
, TIMEZONE_DHAKA
, TIMEZONE_JAKARTA
, TIMEZONE_HONG_KONG
, TIMEZONE_TOKYO
, TIMEZONE_BRISBANE
, TIMEZONE_SYDNEY
, TIMEZONE_AUCKLAND
, TIMEZONE_CUSTOM

, NUM_TIME_ZONES
};

static const char* TIME_ZONE_POSIX_STRINGS[NUM_TIME_ZONES] =
{ ""  // Not specified
, "HST-10"  // Honolulu
, "AKST-09AKDT+01,M3.2.0/02:00,M11.1.0/02:00"  // Anchorage - DST rule for 2007+
, "PST-08PDT+01,M3.2.0/02:00,M11.1.0/02:00"  // Los Angeles - DST rule for 2007+
, "MST-07"  // Phoenix
, "MST-07MDT+01,M3.2.0/02:00,M11.1.0/02:00"  // Denver - DST rule for 2007+
, "CST-06CDT+01,M3.2.0/02:00,M11.1.0/02:00"  // Chicago - DST rule for 2007+
, "EST-05EDT+01,M3.2.0/02:00,M11.1.0/02:00"  // New York - DST rule for 2007+
, "AST-04ADT+01,M3.2.0/02:00,M11.1.0/02:00"  // Halifax - DST rule for 2007+
, "UTC+00"  // UTC (Reykjavik)
, "GMT+00BST+01,M3.5.0/01:00,M10.5.0/02:00"  // London
, "CET+01CEST+01,M3.5.0/02:00,M10.5.0/03:00"  // Brussels
, "EET+02EEST+01,M3.5.0/03:00,M10.5.0/04:00"  // Athens
, "AST+03"  // Baghdad
, "MSK+04"  // Moscow (before summer 2011: MSK+03MSD+01,M3.5.0/02:00,M10.5.0/02:00)
, "GST+04"  // Dubai
, "PKT+05"  // Islamabad
, "IST+05:30"  // New Delhi
, "BDT+06"  // Dhaka
, "WIT+07"  // Jakarta
, "HKT+08"  // Hong Kong
, "JST+09"  // Tokyo
, "AEST+10"  // Brisbane
, "AEST+10AEDT+01,M10.1.0/02:00,M4.1.0/03:00"  // Sydney
, "NZST+12NZDT+01,M9.5.0/02:00,M4.1.0/03:00"  // Auckland - DST rule for Sep. 2007+
, ""  // Other
};


static const char* TIME_ZONE_NAME_STRINGS[NUM_TIME_ZONES] =
{ "Not Specified"
, "Honolulu (-10 HST)"
, "Anchorage (-9 AKST/-8 AKDT)"
, "Los Angeles (-8 PST/-7 PDT)"
, "Phoenix (-7 MST)"
, "Denver (-7 MST/-6 MDT)"
, "Chicago (-6 CST/-5 CDT)"
, "New York (-5 EST/-4 EDT)"
, "Halifax (-4 AST/-3 ADT)"
, "UTC (+0 UTC)"
, "London (+0 GMT/+1 BST)"
, "Brussels (+1 CET/+2 CEST)"
, "Athens (+2 EET/+3 EEST)"
, "Baghdad (+3 AST)"
, "Moscow (+4 MSK)"
, "Dubai (+4 GST)"
, "Islamabad (+5 PKT)"
, "New Delhi (+5:30 IST)"
, "Dhaka (+6 BDT)"
, "Jakarta (+7 WIT)"
, "Hong Kong (+8 HKT)"
, "Tokyo (+9 JST)"
, "Brisbane (+10 AEST)"
, "Sydney (+10 AEST/+11 AEDT)"
, "Auckland (+12 NZST/+13 NZDT)"
, "Other/Custom DST..."
};
static const char* TIME_ZONE_SHORT_NAME_STRINGS[NUM_TIME_ZONES] =
{ "N/A"
, "HST"
, "AKST/AKDT"
, "PST/PDT"
, "MST"
, "MST/MDT"
, "CST/CDT"
, "EST/EDT"
, "AST/ADT"
, "UTC"
, "GMT/BST"
, "CET/CEST"
, "EET/EEST"
, "AST"
, "MSK"
, "GST"
, "PKT"
, "IST"
, "BDT"
, "WIT"
, "HKT"
, "JST"
, "AEST"
, "AEST/AEDT"
, "NZST/NZDT"
, "Custom"
};

/*==========================================================================*/
inline const char* GetPosixStringForTimeZone(TimeZonesEnum TimeZone)
{
	if (TimeZone < TIMEZONE_NOT_SPECIFIED || TimeZone >= NUM_TIME_ZONES)
		return TIME_ZONE_POSIX_STRINGS[TIMEZONE_NOT_SPECIFIED];
	
	return TIME_ZONE_POSIX_STRINGS[TimeZone];
}

/*==========================================================================*/
inline TimeZonesEnum GetTimeZoneFromPosixString(const char* PosixString)
{
	if (PosixString == NULL)
		return TIMEZONE_NOT_SPECIFIED;
	
	for (int TimeZoneIndex = TIMEZONE_NOT_SPECIFIED; TimeZoneIndex < NUM_TIME_ZONES; ++TimeZoneIndex)
	{
		#if _MSC_VER >= 1400
		if (_stricmp(PosixString, TIME_ZONE_POSIX_STRINGS[TimeZoneIndex]) == 0)
		#else
		if (stricmp(PosixString, TIME_ZONE_POSIX_STRINGS[TimeZoneIndex]) == 0)
		#endif
			return (TimeZonesEnum)TimeZoneIndex;
	}
	
	return TIMEZONE_NOT_SPECIFIED;
}

/****************************************************************************/

enum IntradayBarPeriodTypeEnum
{ IBPT_DAYS_MINS_SECS = 0
, IBPT_VOLUME_PER_BAR
, IBPT_NUM_TRADES_PER_BAR
, IBPT_RANGE_IN_TICKS_STANDARD
, IBPT_RANGE_IN_TICKS_NEWBAR_ON_RANGEMET
, IBPT_RANGE_IN_TICKS_TRUE
, IBPT_RANGE_IN_TICKS_FILL_GAPS
, IBPT_REVERSAL_IN_TICKS
, IBPT_RENKO_IN_TICKS
, IBPT_DELTA_VOLUME_PER_BAR
, IBPT_FLEX_RENKO_IN_TICKS
, IBPT_RANGE_IN_TICKS_OPEN_EQUAL_CLOSE
, IBPT_PRICE_CHANGES_PER_BAR
};

// Useful constants for HTTP requests
const int HTTP_REQUEST_ERROR = 0;
const int HTTP_REQUEST_NOT_SENT = 1;
const int HTTP_REQUEST_MADE = 2;
const int HTTP_REQUEST_RECEIVED = 3;

// Trading related declarations

const int SCTRADING_ORDER_ERROR = -1;
const int SCTRADING_NOT_OCO_ORDER_TYPE = -2;

const int SCT_SKIPPED_DOWNLOADING_HISTORICAL_DATA = -8999;
const int SCT_SKIPPED_FULL_RECALC = -8998;
const int SCT_SKIPPED_ONLY_ONE_TRADE_PER_BAR = -8997;
const int SCT_SKIPPED_INVALID_INDEX_SPECIFIED = -8996;
const int SCT_SKIPPED_TOO_MANY_NEW_BARS_DURING_UPDATE = -8995;

enum SCOrderTypeEnum  // Old
{ SCT_MARKET = 0
, SCT_LIMIT
, SCT_STOP
, SCT_STOPLIMIT
, SCT_MIT
};


const int SCT_ORDERTYPE_MARKET = 0;
const int SCT_ORDERTYPE_LIMIT = 1;
const int SCT_ORDERTYPE_STOP = 2;
const int SCT_ORDERTYPE_STOP_LIMIT = 3;
const int SCT_ORDERTYPE_MARKET_IF_TOUCHED = 4;
const int SCT_ORDERTYPE_LIMIT_CHASE = 5;
const int SCT_ORDERTYPE_LIMIT_TOUCH_CHASE = 6;
const int SCT_ORDERTYPE_TRAILING_STOP = 7;
const int SCT_ORDERTYPE_TRAILING_STOP_LIMIT = 8;
const int SCT_ORDERTYPE_TRIGGERED_TRAILING_STOP_3_OFFSETS = 9;
const int SCT_ORDERTYPE_TRIGGERED_TRAILING_STOP_LIMIT_3_OFFSETS  = 10;
const int SCT_ORDERTYPE_STEP_TRAILING_STOP = 11;
const int SCT_ORDERTYPE_STEP_TRAILING_STOP_LIMIT = 12;
const int SCT_ORDERTYPE_TRIGGERED_STEP_TRAILING_STOP = 13;
const int SCT_ORDERTYPE_TRIGGERED_STEP_TRAILING_STOP_LIMIT = 14;
const int SCT_ORDERTYPE_OCO_LIMIT_STOP = 15;
const int SCT_ORDERTYPE_OCO_LIMIT_STOP_LIMIT = 16;
const int SCT_ORDERTYPE_OCO_BUY_STOP_SELL_STOP = 17;
const int SCT_ORDERTYPE_OCO_BUY_STOP_LIMIT_SELL_STOP_LIMIT = 18;
const int SCT_ORDERTYPE_OCO_BUY_LIMIT_SELL_LIMIT = 19;
const int SCT_ORDERTYPE_NUM_ORDERS = 20;


enum SCTimeInForceEnum
{ SCT_TIF_DAY
, SCT_TIF_GTC
};


// Trading order status codes. SCT_ = Sierra Chart Trading. OSC_ = OrderStatusCode.
enum SCOrderStatusCodeEnum
{ SCT_OSC_UNSPECIFIED = 0 //OrderStatusCode is undefined. You will never get this. It is internally used.

, SCT_OSC_ORDERSENT
, SCT_OSC_PENDINGOPEN  // This is only used for external trading services, and it should be used when the service indicates the order is pending and it is clear that the order cannot be modified or it is unclear if the order can be modified in the pending state. Sierra Chart will queue and delay an order modification if the state is Pending Open.
, SCT_OSC_PENDINGCHILD//Only used internally. In DTC map SCT_OSC_PENDINGCHILD to SCT_OSC_OPEN
, SCT_OSC_OPEN
, SCT_OSC_PENDINGMODIFY
, SCT_OSC_PENDINGCANCEL
, SCT_OSC_FILLED // Order has been completely filled or partially filled, and is no longer considered working.
, SCT_OSC_CANCELED // Order has been canceled. The order could have been partially filled before it was canceled.
, SCT_OSC_ERROR // Error with order.
, SCT_OSC_PENDING_CANCEL_FOR_REPLACE//To be used with trading services that do not support modifying orders.
, NUM_ORDER_STATUS_CODES
};

/*============================================================================
	This function returns true when the given order status code is considered
	to be in a "working" order state.  This means the order can potentially
	be filled or be canceled by the user, or canceled by the trading service.
----------------------------------------------------------------------------*/
inline bool IsWorkingOrderStatus(SCOrderStatusCodeEnum OrderStatusCode)
{
	switch (OrderStatusCode)
	{
		case SCT_OSC_ORDERSENT:
		case SCT_OSC_PENDINGOPEN:
		case SCT_OSC_OPEN:
		case SCT_OSC_PENDINGCANCEL:
		case SCT_OSC_PENDINGMODIFY:
		case SCT_OSC_PENDINGCHILD:
		case SCT_OSC_PENDING_CANCEL_FOR_REPLACE:
		return true;
		
		default:
		return false;
	}
}

/*============================================================================
	This function returns true when the given order status code is considered
	to be in a "working" order state.  This means the order can potentially
	be filled or be canceled by the user, or canceled by the trading service.
----------------------------------------------------------------------------*/
inline bool IsWorkingOrderStatusNoChild(SCOrderStatusCodeEnum OrderStatusCode)
{
	switch (OrderStatusCode)
	{
		case SCT_OSC_ORDERSENT:
		case SCT_OSC_PENDINGOPEN:
		case SCT_OSC_OPEN:
		case SCT_OSC_PENDINGCANCEL:
		case SCT_OSC_PENDINGMODIFY:
		case SCT_OSC_PENDING_CANCEL_FOR_REPLACE:
		return true;
		
		default:
		return false;
	}
}

/*==========================================================================*/
inline const char* OrderStatusToString(SCOrderStatusCodeEnum OrderStatusCode, bool  Abbreviated=false)
{
	switch (OrderStatusCode)
	{
		case SCT_OSC_UNSPECIFIED:
		return "Unspecified";
		
		case SCT_OSC_FILLED:
		return "Filled";
		
		case SCT_OSC_CANCELED:
		return "Canceled";
		
		case SCT_OSC_ERROR:
		return "Error";
		
		case SCT_OSC_ORDERSENT:
		return "Order Sent";
		
		case SCT_OSC_PENDINGOPEN:
		return "Pending Open";
		
		case SCT_OSC_OPEN:
		return "Open";
		
		case SCT_OSC_PENDINGCANCEL:
		return "Pending Cancel";
		
		case SCT_OSC_PENDINGMODIFY:
			return "Pending Modify";

		case SCT_OSC_PENDINGCHILD:
			if(Abbreviated)
				return "Child";
			else
				return "Pending Child";

		case SCT_OSC_PENDING_CANCEL_FOR_REPLACE:
			return "Cancel/Replace";

		default:
		return "";
	}
}

enum BuySellEnum
{ BSE_UNDEFINED
, BSE_BUY
, BSE_SELL
};

/*==========================================================================*/
inline const char* BuySellToString(BuySellEnum BuySell)
{
	switch (BuySell)
	{
		case BSE_BUY:
		return "Buy";
		
		case BSE_SELL:
		return "Sell";
		
		default:
		return "";
	}
}

/*==========================================================================*/
inline char BuySellToChar(BuySellEnum BuySell)
{
	switch (BuySell)
	{
		case BSE_BUY:
		return 'B';
		
		case BSE_SELL:
		return 'S';
		
		default:
		return 0;
	}
}

/*==========================================================================*/
inline BuySellEnum BuySellInverse(BuySellEnum BuySell)
{
	switch (BuySell)
	{
		case BSE_BUY:
		return BSE_SELL;
		
		case BSE_SELL:
		return BSE_BUY;
		
		default:
		return BSE_UNDEFINED;
	}
}

enum OpenCloseEnum
{ OCE_UNDEFINED
, OCE_OPEN
, OCE_CLOSE
};

/*==========================================================================*/
inline const char* OpenCloseToString(OpenCloseEnum OpenClose)
{
	switch (OpenClose)
	{
		case OCE_OPEN:
		return "Open";
		
		case OCE_CLOSE:
		return "Close";
		
		default:
		return "";
	}
}

/*==========================================================================*/
inline OpenCloseEnum OpenCloseInverse(OpenCloseEnum OpenClose)
{
	switch (OpenClose)
	{
		case OCE_OPEN:
		return OCE_CLOSE;
		
		case OCE_CLOSE:
		return OCE_OPEN;
		
		default:
		return OCE_UNDEFINED;
	}
}

enum MoveToBreakEvenActionTypeEnum
{ MOVETO_BE_ACTION_TYPE_NONE = 0
, MOVETO_BE_ACTION_TYPE_OFFSET_TRIGGERED = 1
, MOVETO_BE_ACTION_TYPE_OCO_GROUP_TRIGGERED = 2
, MOVETO_BE_ACTION_TYPE_TRAIL_TO_BREAKEVEN = 3
, MOVETO_BE_ACTION_TYPE_OFFSET_TRIGGERED_TRAIL_TO_BREAKEVEN = 4
};

//OCO group constants
const int OCO_GROUP_1 = 0;
const int OCO_GROUP_2 = 1;
const int OCO_GROUP_3 = 2;
const int OCO_GROUP_4 = 3;
const int OCO_GROUP_5 = 4;
const int NUM_OCO_GROUPS = 5;

const int MAX_ACS_TOOL_BUTTONS= 50;


enum ReplayStatus {REPLAY_STOPPED, REPLAY_RUNNING, REPLAY_PAUSED};


//Date-Time to string constants
static const int FLAG_DT_YEAR	= 0x0001;
static const int FLAG_DT_MONTH	= 0x0002;
static const int FLAG_DT_DAY	= 0x0004;
static const int FLAG_DT_HOUR	= 0x0008;
static const int FLAG_DT_MINUTE	= 0x0010;
static const int FLAG_DT_SECOND	= 0x0020;
static const int FLAG_DT_PLUS_WITH_TIME				= 0x0040;
static const int FLAG_DT_NO_ZERO_PADDING_FOR_DATE	= 0x0080;
static const int FLAG_DT_HIDE_SECONDS_IF_ZERO		= 0x0100;
static const int FLAG_DT_NO_HOUR_PADDING			= 0x0200;
static const int FLAG_DT_MILLISECOND				= 0x0400;
static const int FLAG_DT_COMPACT_DATE				= 0x0800;  // DDMonYY
static const int FLAG_DT_COMPLETE_DATE = FLAG_DT_YEAR | FLAG_DT_MONTH | FLAG_DT_DAY;
static const int FLAG_DT_COMPLETE_TIME = FLAG_DT_HOUR  | FLAG_DT_MINUTE |  FLAG_DT_SECOND;
static const int FLAG_DT_COMPLETE_DATETIME = FLAG_DT_YEAR | FLAG_DT_MONTH | FLAG_DT_DAY | FLAG_DT_HOUR | FLAG_DT_MINUTE | FLAG_DT_SECOND;
static const int FLAG_DT_COMPLETE_DATETIME_MS = FLAG_DT_YEAR | FLAG_DT_MONTH | FLAG_DT_DAY | FLAG_DT_HOUR | FLAG_DT_MINUTE | FLAG_DT_SECOND | FLAG_DT_MILLISECOND;



enum HistoricalChartBarPeriodEnum
{ HISTORICAL_CHART_PERIOD_DAYS = 1 
, HISTORICAL_CHART_PERIOD_WEEKLY = 2
, HISTORICAL_CHART_PERIOD_MONTHLY = 3
, HISTORICAL_CHART_PERIOD_QUARTERLY = 4
, HISTORICAL_CHART_PERIOD_YEARLY = 5
};

#endif

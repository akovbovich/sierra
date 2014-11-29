#ifndef _SCSYMBOLDATA_H_
#define _SCSYMBOLDATA_H_

typedef unsigned int t_Volume;

#pragma pack(push, 8)

struct s_SCDomStructure
{
	float Price;
	t_Volume Volume;//The quantity of shares/contracts at the price
	
	s_SCDomStructure()
		: Price(0.0f)
		, Volume(0)
	{
	}
	
	void Clear()
	{
		Price = 0.0f;
		Volume = 0;
	}
	
	bool IsEmpty() const
	{
		return (Price == 0.0f && Volume == 0);
	}
	
	static bool BidDOMSortPred(const s_SCDomStructure& First, const s_SCDomStructure& Second)
	{
		return (First.Price > Second.Price);
	}
	
	static bool AskDOMSortPred(const s_SCDomStructure& First, const s_SCDomStructure& Second)
	{
		return (First.Price < Second.Price);
	}
	
	typedef bool (*t_SortPred)(const s_SCDomStructure& First, const s_SCDomStructure& Second);
};

enum TickDirectionEnum
{ DOWN_TICK = -1
, NO_TICK_DIRECTION = 0
, UP_TICK = 1
};

static const int MAX_NUM_DOM_LEVELS = 50;
static const float DEPTH_PRICE_COMPARISON_TOLERANCE = 0.0000001f;

// This structure is used in DLL studies.  Therefore back compatibility needs to be maintained.
struct s_SCBasicSymbolData
{
	int NumberOfTrades;
	float High;
	float Low;
	int LastTradeSize;
	int AskSize;
	int BidSize;
	float Bid;
	float Ask;
	float LastTradePrice;
	float Open;
	float CurrencyValuePerTick;
	float SettlementPrice; 
	unsigned int OpenInterest;
	unsigned int Volume;
	unsigned int SharesOutstanding;
	float EarningsPerShare;
	int TickDirection;  // TickDirectionEnum
	int LastTradeAtSamePrice;  
	float StrikePrice;
	char Description[256];
	SCDateTime LastDateTimePriorDataModified;  // The last date-time that prior data was modified, requiring a reload of the intraday data file
	char Symbol[128];
	float SellRolloverInterest;

	// 0 <= PriceFormat < 20: number of decimal places
	// PriceFormat = 20: date/time
	// PriceFormat > 20: denominator + 100
	int PriceFormat;	

	s_SCDomStructure BidDOM[MAX_NUM_DOM_LEVELS];
	s_SCDomStructure AskDOM[MAX_NUM_DOM_LEVELS];
	float BuyRolloverInterest;
	char TradeIndicator;
	char LastTradeAtBidAsk;

	// This variable indicates intraday data is being download for symbol and should not be read by remote instance.  It is only set when doing a multi-stage download.
	char IsMultiStageIntradayDownloadActive;

	char Reserved2;
	float TickSize;
	SCDateTime LastTradeDateTime;
	int AccumulatedLastTradeVolume; 
	SCDateTime LastTradingDateForFutures;
	
	/*======================================================================*/
	s_SCBasicSymbolData()
	{
		Clear();
	};

	/*======================================================================*/
	void Clear()
	{
		memset(this, 0, sizeof(s_SCBasicSymbolData));
		PriceFormat = -1;
		TickDirection=NO_TICK_DIRECTION;		
	}

	/*======================================================================*/
	s_SCBasicSymbolData(const s_SCBasicSymbolData& Src)
	{
		Copy(Src);
	}
	
	/*======================================================================*/
	s_SCBasicSymbolData& operator = (const s_SCBasicSymbolData& Src)
	{
		Copy(Src);
		return *this;
	}
	
	/*======================================================================*/
	void Copy(const s_SCBasicSymbolData& Src)
	{
		if (&Src == this)
			return;
		
		memcpy(this, &Src, sizeof(s_SCBasicSymbolData));
	}
	
	/*======================================================================*/
	bool operator == (const s_SCBasicSymbolData& That)
	{
		return (memcmp(this, &That, sizeof(s_SCBasicSymbolData)) == 0);
	}

	/*======================================================================*/
	void MultiplyData(float Multiplier)
	{
		if (Multiplier != 1.0)
		{
			Ask *= Multiplier;
			Bid *= Multiplier;
			High *= Multiplier;
			LastTradePrice *= Multiplier;
			Low *= Multiplier;
			Open *= Multiplier;
			SettlementPrice *= Multiplier;

			for (int Level = 0; Level < MAX_NUM_DOM_LEVELS; ++Level)
			{
				BidDOM[Level].Price *= Multiplier;
				AskDOM[Level].Price *= Multiplier;
			}
		}
	}
};

#pragma pack(pop)

#endif

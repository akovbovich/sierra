#ifndef _SCDATETIME_H_
#define _SCDATETIME_H_

class SCDateTime;
class SCDateTimeMS;

#include <time.h>
#include <float.h>
#include <math.h>

enum MonthEnum
{ JANUARY = 1
, FEBRUARY = 2
, MARCH = 3
, APRIL = 4
, MAY = 5
, JUNE = 6
, JULY = 7
, AUGUST = 8
, SEPTEMBER = 9
, OCTOBER = 10
, NOVEMBER = 11
, DECEMBER = 12
};
static const char* FULL_NAME_FOR_MONTH[13] = {"", "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
static const char* SHORT_NAME_FOR_MONTH[13] = {"", "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
static const int DAYS_IN_MONTH[13] = {0, 31, 28, 31,  30,  31,  30,  31,  31,  30,  31,  30,  31};
static const int DAYS_IN_YEAR_AT_MONTH_END[13]   = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};
static const char FUTURES_CODE_FOR_MONTH[13] = {'\0', 'F', 'G', 'H', 'J', 'K', 'M', 'N', 'Q', 'U', 'V', 'X', 'Z'};

#ifdef _DEBUG
const int MAXIMUM_INTRADAY_DATA_FILE_MILLISECONDS = 999;

#else
const int MAXIMUM_INTRADAY_DATA_FILE_MILLISECONDS = 999;//400

#endif


enum DayOfWeekEnum
{ SUNDAY = 0
, MONDAY = 1
, TUESDAY = 2
, WEDNESDAY = 3
, THURSDAY = 4
, FRIDAY = 5
, SATURDAY = 6
};
static const char* FULL_NAME_FOR_DAY_OF_WEEK[7] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
static const char* SHORT_NAME_FOR_DAY_OF_WEEK[7] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
static const char* TWO_CHAR_NAME_FOR_DAY_OF_WEEK[7] = {"Su", "Mo", "Tu", "We", "Th", "Fr", "Sa"};

static const int MONTHS_PER_YEAR = 12;
static const int DAYS_PER_WEEK = 7;  // number of days in a week
static const int HOURS_PER_DAY = 24;  // number of hours in a day
static const int MINUTES_PER_HOUR = 60;  // number of minutes in an hour
static const int SECONDS_PER_MINUTE = 60;  // number of seconds in a minute
static const int MILLISECONDS_PER_SECOND = 1000;  // number of milliseconds in a second
static const __int64 MICROSECONDS_PER_MILLISECOND = 1000;  // number of microseconds in a millisecond
static const int MINUTES_PER_DAY = MINUTES_PER_HOUR*HOURS_PER_DAY;  // number of minutes in a day = 1 440
static const int SECONDS_PER_DAY = SECONDS_PER_MINUTE*MINUTES_PER_DAY;  // number of seconds in a day = 86 400
static const int SECONDS_PER_HOUR = SECONDS_PER_MINUTE*MINUTES_PER_HOUR;  // number of seconds in an hour = 3 600
static const int MILLISECONDS_PER_DAY = MILLISECONDS_PER_SECOND*SECONDS_PER_DAY;  // number of milliseconds in a day = 86 400 000
static const int MILLISECONDS_PER_HOUR = MILLISECONDS_PER_SECOND*SECONDS_PER_HOUR;  // number of milliseconds in an hour = 3 600 000
static const int MILLISECONDS_PER_MINUTE = MILLISECONDS_PER_SECOND*SECONDS_PER_MINUTE;  // number of milliseconds in a minute = 60 000
static const __int64 MICROSECONDS_PER_DAY = MICROSECONDS_PER_MILLISECOND*MILLISECONDS_PER_DAY;  // number of microseconds in a day = 86 400 000 000
static const __int64 MICROSECONDS_PER_HOUR = MICROSECONDS_PER_MILLISECOND*MILLISECONDS_PER_HOUR;  // number of microseconds in an hour = 3 600 000 000
static const __int64 MICROSECONDS_PER_MINUTE = MICROSECONDS_PER_MILLISECOND*MILLISECONDS_PER_MINUTE;  // number of microseconds in a minute = 60 000 000
static const __int64 MICROSECONDS_PER_SECOND = MICROSECONDS_PER_MILLISECOND*MILLISECONDS_PER_SECOND;  // number of microseconds in a second = 1 000 000

static const double YEARS = 365.25;  // 1 year in days (approx.)
static const double WEEKS = 7.0;  // 1 week in days
static const double DAYS = 1.0;  // 1 day in days
static const double HOURS = (1.0/HOURS_PER_DAY)*DAYS;  // 1 hour in days
static const double MINUTES = (1.0/MINUTES_PER_HOUR)*HOURS;  // 1 minute in days
static const double SECONDS = (1.0/SECONDS_PER_MINUTE)*MINUTES;  // 1 second in days
static const double MILLISECONDS = (1.0/MILLISECONDS_PER_SECOND)*SECONDS;  // 1 millisecond in days
static const double MICROSECONDS = (1.0/MICROSECONDS_PER_MILLISECOND)*MILLISECONDS; // 1 microsecond in days

static const double HALF_MILLISECOND_IN_SECONDS = (1.0/MILLISECONDS_PER_SECOND) * 0.5;  // = 0.0005

// The epsilon should always be half of the smallest unit that is used
static const double SCDATETIME_EPSILON = 0.5*SECONDS;  // Half a second
static const double SCDATETIME_EPSILON_MS = 0.5*MILLISECONDS;  // Half a millisecond
static const double SCDATETIME_EPSILON_MCS = 0.5*MICROSECONDS;  // Half a millisecond

static const double HALF_SECOND = 0.5*SECONDS;  // Half a second
static const double HALF_MILLISECOND = 0.5*MILLISECONDS;  // Half a millisecond
static const double HALF_MICROSECOND = 0.5*MICROSECONDS;  // Half a microsecond

static const int MIN_DATE = -657434;  // Year 100
static const int MAX_DATE = 2958465;  // Year 9999

static const int EPOCH_YEAR = 1899;
static const int EPOCH_MONTH = 12;
static const int EPOCH_DAY = 30;

/*============================================================================
	Gets the date part of a SCDateTime.  Returns the number of days since
	1899-12-30.
----------------------------------------------------------------------------*/
inline int DATE_PART(double DateTime)
{
	if (DateTime < 0)
		return (int)(DateTime - SCDATETIME_EPSILON);
	else
		return (int)(DateTime + SCDATETIME_EPSILON);
}

/*============================================================================
	Returns only the time part (ignoring the date) of the given DateTime
	value as the number of seconds since midnight.  The return value will be
	in the range [0..86399].  The given value is rounded to the nearest
	second.
----------------------------------------------------------------------------*/
inline int TIME_PART(double DateTime)
{
	int TimeInSeconds = 0;
	if (DateTime < 0)  // Special handling for negative numbers
	{
		TimeInSeconds = (int)(
			(DateTime
				- ceil(DateTime - SCDATETIME_EPSILON)  // remove the date part
			)  // this is the fraction (time) part
			* SECONDS_PER_DAY  // convert to seconds
			- 0.5  // subtract half a second before truncating to round to the nearest second
		);
	}
	else  // Typical handling
	{
		TimeInSeconds = (int)(
			(DateTime
				- floor(DateTime + SCDATETIME_EPSILON)  // remove the date part
			)  // this is the fraction (time) part
			* SECONDS_PER_DAY  // convert to seconds
			+ 0.5  // add half a second before truncating to round to the nearest second
		);
	}

	if(TimeInSeconds >= SECONDS_PER_DAY)
		 TimeInSeconds = 0;

	return TimeInSeconds;
}

/*============================================================================
	Returns only the time part (ignoring the date) of the given DateTime
	value as the number of seconds since midnight.  The return value will be
	in the range 0 to 86399. The given value is rounded to the nearest
	millisecond, and then the millisecond component is discarded so that the
	value is truncated to the containing second.
----------------------------------------------------------------------------*/
inline int TIME_PART_MS(double DateTime)
{
	if (DateTime < 0)  // Special handling for negative numbers
	{
		return (int)(
			(DateTime
				- ceil(DateTime - SCDATETIME_EPSILON_MS)  // remove the date part
			)  // this is the fraction (time) part
			* SECONDS_PER_DAY  // convert to seconds
			- HALF_MILLISECOND_IN_SECONDS  // round down by no more than half a millisecond for (negative) integer conversion
		);
	}
	else  // Typical handling
	{
		return (int)(
			(DateTime
				- floor(DateTime + SCDATETIME_EPSILON_MS)  // remove the date part
			)  // this is the fraction (time) part
			* SECONDS_PER_DAY  // convert to seconds
			+ HALF_MILLISECOND_IN_SECONDS  // round up by no more than half a millisecond for integer conversion
		);
	}
}

/*============================================================================
	Returns only the milliseconds part of the given DateTime, ignoring any
	date part or time part that is 1 second or greater.  The return value
	will be in the range [0..999].  The given value is rounded to the nearest
	millisecond.
----------------------------------------------------------------------------*/
inline int MS_PART(double DateTime)
{
	// Convert to seconds
	DateTime *= SECONDS_PER_DAY;

	if (DateTime < 0)  // Special handling for negative numbers
	{
		// Remove the seconds part, leaving only the fractional milliseconds
		DateTime -= ceil(DateTime + HALF_MILLISECOND_IN_SECONDS);

		// Convert to milliseconds, rounding to the nearest millisecond
		return (int)(DateTime * MILLISECONDS_PER_SECOND - 0.5);
	}
	else  // Typical handling
	{
		// Remove the seconds part, leaving only the fractional milliseconds
		DateTime -= floor(DateTime + HALF_MILLISECOND_IN_SECONDS);

		// Convert to milliseconds, rounding to the nearest millisecond
		return (int)(DateTime * MILLISECONDS_PER_SECOND + 0.5);
	}
}

/*============================================================================
	Combines a date (days since 1899-12-30) and time (seconds since midnight)
	into a SCDateTime/OLE Date Time value.
----------------------------------------------------------------------------*/
inline double COMBINE_DATE_TIME(int Date, int Time)
{
	return Date + Time*SECONDS;
}

/*==========================================================================*/
inline DayOfWeekEnum DAY_OF_WEEK(int Date)
{
	return (DayOfWeekEnum)((Date - MONDAY) % DAYS_PER_WEEK);
}

/*==========================================================================*/
inline DayOfWeekEnum DAY_OF_WEEK(double DateTime)
{
	return DAY_OF_WEEK(DATE_PART(DateTime));
}

/*==========================================================================*/
inline const char* GetFullNameForMonth(int Month)
{
	if (Month < JANUARY || Month > DECEMBER)
		return FULL_NAME_FOR_MONTH[0];
	
	return FULL_NAME_FOR_MONTH[Month];
}

/*==========================================================================*/
inline const char* GetShortNameForMonth(int Month)
{
	if (Month < JANUARY || Month > DECEMBER)
		return SHORT_NAME_FOR_MONTH[0];
	
	return SHORT_NAME_FOR_MONTH[Month];
}

/*==========================================================================*/
inline int GetMonthFromShortName(const char* ShortName)
{
	if (ShortName == NULL)
		return 0;
	
	for (int Month = JANUARY; Month <= DECEMBER; ++Month)
	{
		#if _MSC_VER >= 1400
		if (_stricmp(ShortName, SHORT_NAME_FOR_MONTH[Month]) == 0)
		#else
		if (stricmp(ShortName, SHORT_NAME_FOR_MONTH[Month]) == 0)
		#endif
			return Month;
	}
	
	return 0;
}

/*==========================================================================*/
inline int GetMonthFromFuturesCode(char MonthCode)
{
	// Make upper-case
	if (MonthCode >= 'a' && MonthCode <= 'z')
		MonthCode += 'A' - 'a';
	
	for (int Month = JANUARY; Month <= DECEMBER; ++Month)
	{
		if (MonthCode == FUTURES_CODE_FOR_MONTH[Month])
			return Month;
	}
	
	return 0;
}

/*==========================================================================*/
inline const char* GetFullNameForDayOfWeek(int DayOfWeek)
{
	if (DayOfWeek < SUNDAY || DayOfWeek > SATURDAY)
		return "";
	
	return FULL_NAME_FOR_DAY_OF_WEEK[DayOfWeek];
}

/*==========================================================================*/
inline const char* GetShortNameForDayOfWeek(int DayOfWeek)
{
	if (DayOfWeek < SUNDAY || DayOfWeek > SATURDAY)
		return "";
	
	return SHORT_NAME_FOR_DAY_OF_WEEK[DayOfWeek];
}

/*==========================================================================*/
inline const char* GetTwoCharNameForDayOfWeek(int DayOfWeek)
{
	if (DayOfWeek < SUNDAY || DayOfWeek > SATURDAY)
		return "";

	return TWO_CHAR_NAME_FOR_DAY_OF_WEEK[DayOfWeek];
}

/*==========================================================================*/
inline int IsWeekend(int Date)
{
	int DayOfWeek = DAY_OF_WEEK(Date);
	return (DayOfWeek == SUNDAY || DayOfWeek == SATURDAY);
}

/*==========================================================================*/
inline int IsWeekend(double DateTime)
{
	int DayOfWeek = DAY_OF_WEEK(DateTime);
	return (DayOfWeek == SUNDAY || DayOfWeek == SATURDAY);
}

/*==========================================================================*/
inline int IsWeekend(int Date, bool UseSaturdayData)
{
	int DayOfWeek = DAY_OF_WEEK(Date);
	return (DayOfWeek == SUNDAY || (!UseSaturdayData && DayOfWeek == SATURDAY));
}

/*==========================================================================*/
inline int IsWeekend(double DateTime, bool UseSaturdayData)
{
	int DayOfWeek = DAY_OF_WEEK(DateTime);
	return (DayOfWeek == SUNDAY || (!UseSaturdayData && DayOfWeek == SATURDAY));
}

/*==========================================================================*/
inline int IsBusinessDay(int Date)
{
	return !IsWeekend(Date);
}

/*==========================================================================*/
inline bool IsLeapYear(int Year)
{
	return ((Year & 3) == 0) && ((Year % 100) != 0 || (Year % 400) == 0);
}

/*==========================================================================*/
inline int GetDaysInMonth(int Month, int Year)
{
	if (Month < JANUARY || Month > DECEMBER)
		return 0;
	
	if (Month == FEBRUARY && IsLeapYear(Year))
		return DAYS_IN_MONTH[Month] + 1;
	else
		return DAYS_IN_MONTH[Month];
}

/*============================================================================
	This function returns the number of weekdays within the given date span,
	including both the FirstDate and the LastDate.  Neither Saturdays nor
	Sundays are counted.
----------------------------------------------------------------------------*/
inline int DaysInDateSpanNotIncludingWeekends(int FirstDate, int LastDate)
{
	if (FirstDate > LastDate)
	{
		// Swap the dates
		int EarlierDate = LastDate;
		LastDate = FirstDate;
		FirstDate = EarlierDate;
	}
	
	int SpanDifference = LastDate - FirstDate;
	int FirstDayOfWeek = DAY_OF_WEEK(FirstDate);
	int SundayBasedSpan = SpanDifference + FirstDayOfWeek;
	
	int NumSaturdays;
	if (SundayBasedSpan - SATURDAY < 0)
		NumSaturdays = 0;
	else
		NumSaturdays = (SundayBasedSpan - SATURDAY) / DAYS_PER_WEEK + 1;
	
	int NumSundays = SundayBasedSpan / DAYS_PER_WEEK;
	if (FirstDayOfWeek == SUNDAY)
		++NumSundays;
	
	return SpanDifference + 1 - NumSaturdays - NumSundays;
}

/*============================================================================
	SCDateTime values are the same as Excel DateTime values.  These are
	stored as a floating point value (double) representing the number of days
	since 1899-12-30 00:00:00.  (Time is represented as a fraction of a day.)
	Unix-style timestamps are stored as integer values representing the
	number of seconds since 1970-01-01 00:00:00.  These two functions are
	used to convert from one to the other.
----------------------------------------------------------------------------*/
inline time_t SCDateTimeToUNIXTime(double DateTime)
{
	return (time_t)( (DateTime - 25569.0) * SECONDS_PER_DAY + 0.5);
}
/*==========================================================================*/
inline double SCDateTimeToFloatUNIXTime(double DateTime)
{
	return ((DateTime - 25569.0) * SECONDS_PER_DAY);
}


/*==========================================================================*/
inline double UNIXTimeToSCDateTime(time_t UnixTime) 
{
	if (UnixTime != 0)
		return UnixTime*SECONDS + 25569.0;
	else
		 return 0.0;
}

/*==========================================================================*/
inline double UNIXTimeWithMillisecondsToSCDateTime(double UnixTimeWithMilliseconds) 
{
	if (UnixTimeWithMilliseconds != 0)
		return UnixTimeWithMilliseconds*SECONDS + 25569.0;
	else
		return 0.0;
}
/*============================================================================
	This function expects one based values for Month and Day.
----------------------------------------------------------------------------*/
inline double YMDHMS_MS_DATETIME(int Year, int Month, int Day, int Hour, int Minute, int Second, int MilliSecond)
{
	// Validate year and month (ignore day of week)
	if (Year > 9999 || Month < 1 || Month > 12)
		return 0.0;
	
	//  Check for leap year and set the number of days in the month
	bool bLeapYear = IsLeapYear(Year);
	
	int nDaysInMonth
		= DAYS_IN_YEAR_AT_MONTH_END[Month] - DAYS_IN_YEAR_AT_MONTH_END[Month-1]
		+ ((bLeapYear && Day == 29 && Month == 2) ? 1 : 0);
	
	// Finish validating the date
	if (Day < 1 || Day > nDaysInMonth ||
		Hour > 23 || Minute > 59 ||
		Second > 59 || MilliSecond > 999)
	{
		return 0.0;
	}
	
	// Cache the date in days and time in fractional days
	int nDate;
	double dblTime;
	
	//It is a valid date; make Jan 1, 1AD be 1
	nDate = Year*365 + Year/4 - Year/100 + Year/400 +
		DAYS_IN_YEAR_AT_MONTH_END[Month-1] + Day;
	
	//  If leap year and it's before March, subtract 1:
	if (Month <= 2 && bLeapYear)
		--nDate;
	
	//  Offset so that 12/30/1899 is 0
	nDate -= 693959;
	
	dblTime = ((Hour * 3600) +  // hrs in seconds
		(Minute * 60) +  // mins in seconds
		(Second) +
		+ (MilliSecond/1000.)) / 86400.
		;
	
	return (double)nDate + ((nDate >= 0) ? dblTime : -dblTime);
}

/*==========================================================================*/
inline double YMDHMS_DATETIME(int Year, int Month, int Day, int Hour, int Minute, int Second)
{
	return YMDHMS_MS_DATETIME(Year, Month, Day, Hour, Minute, Second, 0);
}

/*============================================================================
	This function is thread safe.
----------------------------------------------------------------------------*/
inline void DATETIME_TO_YMDHMS_MS(double DateTime, int& Year, int& Month, int& Day, int& Hour, int& Minute, int& Second, int& MilliSecond)
{
	Year = Month = Day = Hour = Minute = Second = 0;

	int WeekDay = 0;
	int DayOfYear = 0;
	
	
	// The legal range does not actually span year 0 to 9999.
	if (DateTime > MAX_DATE || DateTime < MIN_DATE) // about year 100 to about 9999
		return;
	
	int nDays;				// Number of days since Dec. 30, 1899
	int nDaysAbsolute;		// Number of days since 1/1/0
	int MillisecondsInDayForGivenTime;		

	int n400Years;			// Number of 400 year increments since 1/1/0
	int n400Century;		// Century within 400 year block (0,1,2 or 3)
	int n4Years;			// Number of 4 year increments since 1/1/0
	int n4Day;				// Day within 4 year block
							//  (0 is 1/1/yr1, 1460 is 12/31/yr4)
	int n4Yr;				// Year within 4 year block (0,1,2 or 3)
	int bLeap4 = 1;			// TRUE if 4 year block includes leap year
	
	
	double TempDateTime = DateTime; // temporary serial date
	
	
	
	if(DateTime != 0)
	{
	//If resolving for Milliseconds
		if(MilliSecond != -1) 
			// Round to the millisecond.  
			TempDateTime += ((DateTime > 0.0) ? HALF_MILLISECOND  : - (HALF_MILLISECOND));
		else
			TempDateTime += ((DateTime > 0.0) ?( HALF_SECOND) : -(HALF_SECOND) );//Round to the second
	}
	MilliSecond = 0;
	
	// If a valid date, then this conversion should not overflow
	nDays = (int)TempDateTime;
	
	nDaysAbsolute = (int)TempDateTime + 693959; // Add days from 1/1/0 to 12/30/1899
	
	TempDateTime = fabs(TempDateTime);
	MillisecondsInDayForGivenTime = (int)((TempDateTime - floor(TempDateTime)) * MILLISECONDS_PER_DAY);
	
	// Calculate the day of week (sun=1, mon=2...)
	//   -1 because 1/1/0 is Sat.  +1 because we want 1-based
	WeekDay = ((nDaysAbsolute - 1) % 7) + 1;
	
	// Leap years every 4 yrs except centuries not multiples of 400.
	n400Years = nDaysAbsolute / 146097;
	
	// Set nDaysAbsolute to day within 400-year block
	nDaysAbsolute %= 146097;
	
	// -1 because first century has extra day
	n400Century = ((nDaysAbsolute - 1) / 36524);
	
	// Non-leap century
	if (n400Century != 0)
	{
		// Set nDaysAbsolute to day within century
		nDaysAbsolute = (nDaysAbsolute - 1) % 36524;
		
		// +1 because 1st 4 year increment has 1460 days
		n4Years = ((nDaysAbsolute + 1) / 1461);
		
		if (n4Years != 0)
			n4Day = ((nDaysAbsolute + 1) % 1461);
		else
		{
			bLeap4 = 0;
			n4Day = nDaysAbsolute;
		}
	}
	else
	{
		// Leap century - not special case!
		n4Years = nDaysAbsolute / 1461;
		n4Day = nDaysAbsolute % 1461;
	}
	
	if (bLeap4)
	{
		// -1 because first year has 366 days
		n4Yr = (n4Day - 1) / 365;
		
		if (n4Yr != 0)
			n4Day = (n4Day - 1) % 365;
	}
	else
	{
		n4Yr = n4Day / 365;
		n4Day %= 365;
	}
	
	// n4Day is now 0-based day of year. Save 1-based day of year, year number
	DayOfYear = n4Day + 1;
	Year = n400Years * 400 + n400Century * 100 + n4Years * 4 + n4Yr;
	
	// Handle leap year: before, on, and after Feb. 29.
	if (n4Yr == 0 && bLeap4)
	{
		// Leap Year
		if (n4Day == 59)
		{
			/* Feb. 29 */
			Month = 2;
			Day = 29;
			goto DoTime;
		}

		// Pretend it's not a leap year for month/day comp.
		if (n4Day >= 60)
			--n4Day;
	}
	
	// Make n4DaY a 1-based day of non-leap year and compute
	//  month/day for everything but Feb. 29.
	++n4Day;
	
	// Month number always >= n/32, so save some loop time */
	for (Month = (n4Day >> 5) + 1;
		n4Day > DAYS_IN_YEAR_AT_MONTH_END[Month]; Month++);
	
	Day = (n4Day - DAYS_IN_YEAR_AT_MONTH_END[Month-1]);
	
DoTime:
	
	if (MillisecondsInDayForGivenTime != 0)
	{
		Hour = MillisecondsInDayForGivenTime / MILLISECONDS_PER_HOUR;
		MillisecondsInDayForGivenTime -= Hour * MILLISECONDS_PER_HOUR;
		Minute = MillisecondsInDayForGivenTime / MILLISECONDS_PER_MINUTE;
		MillisecondsInDayForGivenTime -= Minute * MILLISECONDS_PER_MINUTE;
		Second = MillisecondsInDayForGivenTime / MILLISECONDS_PER_SECOND;
		MillisecondsInDayForGivenTime -= Second * MILLISECONDS_PER_SECOND;
		MilliSecond = MillisecondsInDayForGivenTime;
	}
}
/*============================================================================
	This version supports microseconds. Needs testing.
----------------------------------------------------------------------------*/
inline void DATETIME_TO_YMDHMS_MCS(double DateTime, int& Year, int& Month, int& Day, int& Hour, int& Minute, int& Second, int& MilliSecond, int & Microsecond)
{
	Year = Month = Day = Hour = Minute = Second = 0;

	int WeekDay = 0;
	int DayOfYear = 0;
	
	
	// The legal range does not actually span year 0 to 9999.
	if (DateTime > MAX_DATE || DateTime < MIN_DATE) // about year 100 to about 9999
		return;
	
	int nDays;				// Number of days since Dec. 30, 1899
	int nDaysAbsolute;		// Number of days since 1/1/0
	__int64 MicrosecondsInDayForGivenTime;		

	int n400Years;			// Number of 400 year increments since 1/1/0
	int n400Century;		// Century within 400 year block (0,1,2 or 3)
	int n4Years;			// Number of 4 year increments since 1/1/0
	int n4Day;				// Day within 4 year block
							//  (0 is 1/1/yr1, 1460 is 12/31/yr4)
	int n4Yr;				// Year within 4 year block (0,1,2 or 3)
	int bLeap4 = 1;			// TRUE if 4 year block includes leap year
	
	
	double TempDateTime = DateTime; // temporary serial date
	
	

	if(DateTime != 0)
	{
		if(Microsecond != -1)//If resolving to the microsecond
			TempDateTime += ((DateTime > 0.0) ? HALF_MICROSECOND  : - (HALF_MICROSECOND)); 	// Round to the microsecond.  
		else if(MilliSecond != -1) //If resolving for Milliseconds
			TempDateTime += ((DateTime > 0.0) ? HALF_MILLISECOND  : - (HALF_MILLISECOND)); 	// Round to the millisecond.  
		else
			TempDateTime += ((DateTime > 0.0) ?( HALF_SECOND) : -(HALF_SECOND) );//Round to the second
	}

	MilliSecond = 0;
	Microsecond = 0;

	// If a valid date, then this conversion should not overflow
	nDays = (int)TempDateTime;
	
	nDaysAbsolute = (int)TempDateTime + 693959; // Add days from 1/1/0 to 12/30/1899
	
	TempDateTime = fabs(TempDateTime);
	MicrosecondsInDayForGivenTime = (__int64)((TempDateTime - floor(TempDateTime)) * MICROSECONDS_PER_DAY);
	
	// Calculate the day of week (sun=1, mon=2...)
	//   -1 because 1/1/0 is Sat.  +1 because we want 1-based
	WeekDay = ((nDaysAbsolute - 1) % 7) + 1;
	
	// Leap years every 4 yrs except centuries not multiples of 400.
	n400Years = nDaysAbsolute / 146097;
	
	// Set nDaysAbsolute to day within 400-year block
	nDaysAbsolute %= 146097;
	
	// -1 because first century has extra day
	n400Century = ((nDaysAbsolute - 1) / 36524);
	
	// Non-leap century
	if (n400Century != 0)
	{
		// Set nDaysAbsolute to day within century
		nDaysAbsolute = (nDaysAbsolute - 1) % 36524;
		
		// +1 because 1st 4 year increment has 1460 days
		n4Years = ((nDaysAbsolute + 1) / 1461);
		
		if (n4Years != 0)
			n4Day = ((nDaysAbsolute + 1) % 1461);
		else
		{
			bLeap4 = 0;
			n4Day = nDaysAbsolute;
		}
	}
	else
	{
		// Leap century - not special case!
		n4Years = nDaysAbsolute / 1461;
		n4Day = nDaysAbsolute % 1461;
	}
	
	if (bLeap4)
	{
		// -1 because first year has 366 days
		n4Yr = (n4Day - 1) / 365;
		
		if (n4Yr != 0)
			n4Day = (n4Day - 1) % 365;
	}
	else
	{
		n4Yr = n4Day / 365;
		n4Day %= 365;
	}
	
	// n4Day is now 0-based day of year. Save 1-based day of year, year number
	DayOfYear = n4Day + 1;
	Year = n400Years * 400 + n400Century * 100 + n4Years * 4 + n4Yr;
	
	// Handle leap year: before, on, and after Feb. 29.
	if (n4Yr == 0 && bLeap4)
	{
		// Leap Year
		if (n4Day == 59)
		{
			/* Feb. 29 */
			Month = 2;
			Day = 29;
			goto DoTime;
		}

		// Pretend it's not a leap year for month/day comp.
		if (n4Day >= 60)
			--n4Day;
	}
	
	// Make n4DaY a 1-based day of non-leap year and compute
	//  month/day for everything but Feb. 29.
	++n4Day;
	
	// Month number always >= n/32, so save some loop time */
	for (Month = (n4Day >> 5) + 1;
		n4Day > DAYS_IN_YEAR_AT_MONTH_END[Month]; Month++);
	
	Day = n4Day - DAYS_IN_YEAR_AT_MONTH_END[Month-1];
	
DoTime:
	
	if (MicrosecondsInDayForGivenTime != 0)
	{
		Hour = (int)(MicrosecondsInDayForGivenTime / MICROSECONDS_PER_HOUR);
		MicrosecondsInDayForGivenTime -= Hour * MICROSECONDS_PER_HOUR;

		Minute = (int)(MicrosecondsInDayForGivenTime / MICROSECONDS_PER_MINUTE);
		MicrosecondsInDayForGivenTime -= Minute * MICROSECONDS_PER_MINUTE;

		Second = (int)(MicrosecondsInDayForGivenTime / MICROSECONDS_PER_SECOND);
		MicrosecondsInDayForGivenTime -= Second * MICROSECONDS_PER_SECOND;

		MilliSecond = (int)(MicrosecondsInDayForGivenTime / 1000);
		MicrosecondsInDayForGivenTime -= MilliSecond * 1000;

		Microsecond = (int)MicrosecondsInDayForGivenTime;
	}
}

/*============================================================================
	This function is thread safe.
----------------------------------------------------------------------------*/
inline void DATETIME_TO_YMDHMS(double DateTime, int& Year, int& Month, int& Day, int& Hour, int& Minute, int& Second)
{
	int MilliSecond = -1;
	DATETIME_TO_YMDHMS_MS(DateTime, Year, Month, Day,  Hour, Minute, Second, MilliSecond);
}

/*==========================================================================*/
inline int YMD_DATE(int Year, int Month, int Day)
{
	return DATE_PART(YMDHMS_DATETIME(Year, Month, Day, 0, 0, 0));
}

/*==========================================================================*/
inline void DATE_TO_YMD(int Date, int& Year, int& Month, int& Day)
{
	int Hour, Minute, Second;
	DATETIME_TO_YMDHMS(Date, Year, Month, Day, Hour, Minute, Second);
}

/*============================================================================
	Converts hours, minutes, seconds to an integer time.  This is seconds
	since midnight.
----------------------------------------------------------------------------*/
inline int HMS_TIME(int Hour, int Minute, int Second)
{
	return (Hour*SECONDS_PER_HOUR + Minute*SECONDS_PER_MINUTE + Second);
}

/*==========================================================================*/
inline int HMS_TIME_WITH_VALIDATION(int Hour, int Minute, int Second)
{
	if (Hour < 0 || Hour > 23 || Minute < 0 || Minute > 59 || Second < 0 || Second > 59)
		return 0;

	return (Hour*SECONDS_PER_HOUR + Minute*SECONDS_PER_MINUTE + Second);
}

/*==========================================================================*/
inline int HOUR_OF_TIME(int Time)
{
	return ((Time / SECONDS_PER_HOUR) % HOURS_PER_DAY);
}

/*==========================================================================*/
inline int MINUTE_OF_TIME(int Time)
{
	return ((Time / SECONDS_PER_MINUTE) % MINUTES_PER_HOUR);
}

/*==========================================================================*/
inline int SECOND_OF_TIME(int Time)
{
	return (Time % SECONDS_PER_MINUTE);
}

/*==========================================================================*/
inline void TIME_TO_HMS(int Time, int& Hour, int& Minute, int& Second)
{
	Hour = HOUR_OF_TIME(Time);
	Minute = MINUTE_OF_TIME(Time);
	Second = SECOND_OF_TIME(Time);
}

/*****************************************************************************
// SCDateTime

The SCDateTime object wraps a single 8-byte floating-point (double) value
representing a complete date and time.

Comparison operators on this class are designed to compare date-time values
(including doubles) within half a second.  For best results, at least one of
the date-time values should be as close to the exact second as possible,
meaning avoid any milliseconds.
----------------------------------------------------------------------------*/

class SCDateTime
{
	public:
		double m_dt;

	public:
		SCDateTime();
		SCDateTime(double DateTime);
		SCDateTime(const SCDateTime& DateTime);
		SCDateTime(int Date, int Time);
		SCDateTime(int Year, int Month, int Day, int Hour, int Minute, int Second);

		operator double() const;

		const SCDateTime& operator = (double dt);
		const SCDateTime& operator = (const SCDateTime& DateTime);
		const SCDateTime& operator += (double dt);
		const SCDateTime& operator += (const SCDateTime& DateTime);
		const SCDateTime& operator -= (double dt);
		const SCDateTime& operator -= (const SCDateTime& DateTime);

		bool operator == (int Date) const;
		bool operator != (int Date) const;
		bool operator < (int Date) const;
		bool operator <= (int Date) const;
		bool operator > (int Date) const;
		bool operator >= (int Date) const;

		bool operator == (double dt) const;
		bool operator != (double dt) const;
		bool operator < (double dt) const;
		bool operator <= (double dt) const;
		bool operator > (double dt) const;
		bool operator >= (double dt) const;

		bool operator == (const SCDateTime& DateTime) const;
		bool operator != (const SCDateTime& DateTime) const;
		bool operator < (const SCDateTime& DateTime) const;
		bool operator <= (const SCDateTime& DateTime) const;
		bool operator > (const SCDateTime& DateTime) const;
		bool operator >= (const SCDateTime& DateTime) const;

		bool operator == (const SCDateTimeMS& DateTimeMS) const;
		bool operator != (const SCDateTimeMS& DateTimeMS) const;
		bool operator < (const SCDateTimeMS& DateTimeMS) const;
		bool operator <= (const SCDateTimeMS& DateTimeMS) const;
		bool operator > (const SCDateTimeMS& DateTimeMS) const;
		bool operator >= (const SCDateTimeMS& DateTimeMS) const;

		int GetDate() const;
		int GetTime() const;
		int GetTimeWithoutMilliseconds() const;
		SCDateTime GetTimeAsSCDateTime() const;
		void GetDateYMD(int& Year, int& Month, int& Day) const;
		void GetTimeHMS(int& Hour, int& Minute, int& Second) const;
		void GetDateTimeYMDHMS(int& Year, int& Month, int& Day, int& Hour, int& Minute, int& Second) const;
		void GetDateTimeYMDHMS_MS(int& Year, int& Month, int& Day, int& Hour, int& Minute, int& Second, int& MilliSecond) const;
		int GetYear() const;
		int GetMonth() const;
		int GetDay() const;
		int GetHour() const;
		int GetMinute() const;
		int GetSecond() const;
		int GetMilliSecond() const;
		int GetDayOfWeek() const;

		bool IsWeekend() const;
		bool IsSaturday() const;
		bool IsSunday() const;

		void RoundDateTimeDownToSecond();
		void RoundToNearestSecond();
		SCDateTime& SetDateTime(double DateTime);
		SCDateTime& SetDateTime(int Date, int Time);
		SCDateTime& SetDate(int Date);
		SCDateTime& SetTime(int Time);
		SCDateTime& SetDateYMD(int Year, int Month, int Day);
		SCDateTime& SetTimeHMS(int Hour, int Minute, int Second);
		SCDateTime& SetTimeHMS_MS(int Hour, int Minute, int Second, int MilliSecond);
		SCDateTime& SetDateTimeYMDHMS(int Year, int Month, int Day, int Hour, int Minute, int Second);
		SCDateTime& SetDateTimeYMDHMS_MS(int Year, int Month, int Day, int Hour, int Minute, int Second, int MilliSecond);
};

/*****************************************************************************
// SCDateTimeMS

The SCDateTime object wraps a single 8-byte floating-point (double) value
representing a complete date and time.

Comparison operators on this class are designed to compare date-time values
(including doubles) within half a millisecond.  For best results, at least
one of the date-time values should be as close to the exact millisecond as
possible, meaning avoid any fractional milliseconds.

When comparing the value of a SCDateTime object with the value of a
SCDateTimeMS object, the comparison will be done at the milliseconds level.
If the SCDateTime object's value contains a millisecond value other than 0,
that millisecond value will be considered, so be careful about that.
----------------------------------------------------------------------------*/

class SCDateTimeMS
	: public SCDateTime
{
	public:
		SCDateTimeMS();
		SCDateTimeMS(double DateTime);
		SCDateTimeMS(const SCDateTime& DateTime);
		SCDateTimeMS(const SCDateTimeMS& DateTime);
		SCDateTimeMS(int Date, int Time);

		const SCDateTimeMS& operator = (double dt);
		const SCDateTimeMS& operator = (const SCDateTime& DateTime);

		bool operator == (int Date) const;
		bool operator != (int Date) const;
		bool operator < (int Date) const;
		bool operator <= (int Date) const;
		bool operator > (int Date) const;
		bool operator >= (int Date) const;

		bool operator == (double dt) const;
		bool operator != (double dt) const;
		bool operator < (double dt) const;
		bool operator <= (double dt) const;
		bool operator > (double dt) const;
		bool operator >= (double dt) const;

		bool operator == (const SCDateTime& DateTime) const;
		bool operator != (const SCDateTime& DateTime) const;
		bool operator < (const SCDateTime& DateTime) const;
		bool operator <= (const SCDateTime& DateTime) const;
		bool operator > (const SCDateTime& DateTime) const;
		bool operator >= (const SCDateTime& DateTime) const;

		bool operator == (const SCDateTimeMS& DateTimeMS) const;
		bool operator != (const SCDateTimeMS& DateTimeMS) const;
		bool operator < (const SCDateTimeMS& DateTimeMS) const;
		bool operator <= (const SCDateTimeMS& DateTimeMS) const;
		bool operator > (const SCDateTimeMS& DateTimeMS) const;
		bool operator >= (const SCDateTimeMS& DateTimeMS) const;

		//void GetDateTimeYMDHMS_MS(int& Year, int& Month, int& Day, int& Hour, int& Minute, int& Second,int&  Millisecond) const;
		bool IsSameTimeToSecond(const SCDateTime& DateTime) const;
};

/****************************************************************************/
// c_SCDateTime

/*==========================================================================*/
inline SCDateTime::SCDateTime()
	: m_dt(0.0)
{
}

/*==========================================================================*/
inline SCDateTime::SCDateTime(double DateTime)
	: m_dt(DateTime)
{
}

/*==========================================================================*/
inline SCDateTime::SCDateTime(const SCDateTime& DateTime)
	: m_dt(DateTime.m_dt)
{
}

/*==========================================================================*/
inline SCDateTime::SCDateTime(int Date, int Time)
	: m_dt(COMBINE_DATE_TIME(Date, Time))
{
}

/*==========================================================================*/
inline SCDateTime::SCDateTime(int Year, int Month, int Day, int Hour, int Minute, int Second)
	: m_dt(YMDHMS_DATETIME(Year, Month, Day, Hour, Minute, Second))
{
}

/*==========================================================================*/
inline SCDateTime::operator double() const
{
	return m_dt;
}

/*==========================================================================*/
inline const SCDateTime& SCDateTime::operator = (double dt)
{
	m_dt = dt;
	return *this;
}

/*==========================================================================*/
inline const SCDateTime& SCDateTime::operator = (const SCDateTime& DateTime)
{
	m_dt = DateTime.m_dt;
	return *this;
}

/*==========================================================================*/
inline const SCDateTime& SCDateTime::operator += (double dt)
{
	m_dt += dt;
	return *this;
}

/*==========================================================================*/
inline const SCDateTime& SCDateTime::operator += (const SCDateTime& DateTime)
{
	m_dt += DateTime.m_dt;
	return *this;
}

/*==========================================================================*/
inline const SCDateTime& SCDateTime::operator -= (double dt)
{
	m_dt -= dt;
	return *this;
}

/*==========================================================================*/
inline const SCDateTime& SCDateTime::operator -= (const SCDateTime& DateTime)
{
	m_dt -= DateTime.m_dt;
	return *this;
}

/*==========================================================================*/
inline bool SCDateTime::operator == (int Date) const
{
	return (fabs(m_dt - Date) < SCDATETIME_EPSILON);
}

/*==========================================================================*/
inline bool SCDateTime::operator != (int Date) const
{
	return !(fabs(m_dt - Date) < SCDATETIME_EPSILON);
}

/*==========================================================================*/
inline bool SCDateTime::operator < (int Date) const
{
	return (m_dt - Date < -SCDATETIME_EPSILON);
}

/*==========================================================================*/
inline bool SCDateTime::operator <= (int Date) const
{
	return (m_dt - Date < SCDATETIME_EPSILON);
}

/*==========================================================================*/
inline bool SCDateTime::operator > (int Date) const
{
	return (m_dt - Date > SCDATETIME_EPSILON);
}

/*==========================================================================*/
inline bool SCDateTime::operator >= (int Date) const
{
	return (m_dt - Date > -SCDATETIME_EPSILON);
}

/*==========================================================================*/
inline bool SCDateTime::operator == (double dt) const
{
	return (fabs(m_dt - dt) < SCDATETIME_EPSILON);
}

/*==========================================================================*/
inline bool SCDateTime::operator != (double dt) const
{
	return !(fabs(m_dt - dt) < SCDATETIME_EPSILON);
}

/*==========================================================================*/
inline bool SCDateTime::operator < (double dt) const
{
	return (m_dt - dt < -SCDATETIME_EPSILON);
}

/*==========================================================================*/
inline bool SCDateTime::operator <= (double dt) const
{
	return (m_dt - dt < SCDATETIME_EPSILON);
}

/*==========================================================================*/
inline bool SCDateTime::operator > (double dt) const
{
	return (m_dt - dt > SCDATETIME_EPSILON);
}

/*==========================================================================*/
inline bool SCDateTime::operator >= (double dt) const
{
	return (m_dt - dt > -SCDATETIME_EPSILON);
}

/*==========================================================================*/
inline bool SCDateTime::operator == (const SCDateTime& DateTime) const
{
	return (fabs(m_dt - DateTime.m_dt) < SCDATETIME_EPSILON);
}

/*==========================================================================*/
inline bool SCDateTime::operator != (const SCDateTime& DateTime) const
{
	return !(fabs(m_dt - DateTime.m_dt) < SCDATETIME_EPSILON);
}

/*==========================================================================*/
inline bool SCDateTime::operator < (const SCDateTime& DateTime) const
{
	return (m_dt - DateTime.m_dt < -SCDATETIME_EPSILON);
}

/*==========================================================================*/
inline bool SCDateTime::operator <= (const SCDateTime& DateTime) const
{
	return (m_dt - DateTime.m_dt < SCDATETIME_EPSILON);
}

/*==========================================================================*/
inline bool SCDateTime::operator > (const SCDateTime& DateTime) const
{
	return (m_dt - DateTime.m_dt > SCDATETIME_EPSILON);
}

/*==========================================================================*/
inline bool SCDateTime::operator >= (const SCDateTime& DateTime) const
{
	return (m_dt - DateTime.m_dt > -SCDATETIME_EPSILON);
}

/*==========================================================================*/
inline bool SCDateTime::operator == (const SCDateTimeMS& DateTimeMS) const
{
	return (fabs(m_dt - DateTimeMS.m_dt) < SCDATETIME_EPSILON_MS);
}

/*==========================================================================*/
inline bool SCDateTime::operator != (const SCDateTimeMS& DateTimeMS) const
{
	return (fabs(m_dt - DateTimeMS.m_dt) >= SCDATETIME_EPSILON_MS);
}

/*==========================================================================*/
inline bool SCDateTime::operator < (const SCDateTimeMS& DateTimeMS) const
{
	return (m_dt - DateTimeMS.m_dt < -SCDATETIME_EPSILON_MS);
}

/*==========================================================================*/
inline bool SCDateTime::operator <= (const SCDateTimeMS& DateTimeMS) const
{
	return (m_dt - DateTimeMS.m_dt < SCDATETIME_EPSILON_MS);
}

/*==========================================================================*/
inline bool SCDateTime::operator > (const SCDateTimeMS& DateTimeMS) const
{
	return (m_dt - DateTimeMS.m_dt > SCDATETIME_EPSILON_MS);
}

/*==========================================================================*/
inline bool SCDateTime::operator >= (const SCDateTimeMS& DateTimeMS) const
{
	return (m_dt - DateTimeMS.m_dt > -SCDATETIME_EPSILON_MS);
}

/*==========================================================================*/
inline int SCDateTime::GetDate() const
{
	return DATE_PART(m_dt);
}

/*==========================================================================*/
inline int SCDateTime::GetTime() const
{
	return TIME_PART(m_dt);
}

/*==========================================================================*/
inline int SCDateTime::GetTimeWithoutMilliseconds() const
{
	return TIME_PART_MS(m_dt);
}

/*==========================================================================*/
inline SCDateTime SCDateTime::GetTimeAsSCDateTime() const
{
	return m_dt - DATE_PART(m_dt);
}

/*==========================================================================*/
inline void SCDateTime::GetDateYMD(int& Year, int& Month, int& Day) const
{
	DATE_TO_YMD(DATE_PART(m_dt), Year, Month, Day);
}

/*==========================================================================*/
inline void SCDateTime::GetTimeHMS(int& Hour, int& Minute, int& Second) const
{
	TIME_TO_HMS(TIME_PART(m_dt), Hour, Minute, Second);
}

/*==========================================================================*/
inline void SCDateTime::GetDateTimeYMDHMS(int& Year, int& Month, int& Day, int& Hour, int& Minute, int& Second) const
{
	DATETIME_TO_YMDHMS(m_dt, Year, Month, Day, Hour, Minute, Second);
}

/*==========================================================================*/
inline void SCDateTime::GetDateTimeYMDHMS_MS(int& Year, int& Month, int& Day, int& Hour, int& Minute, int& Second, int& MilliSecond) const
{
	DATETIME_TO_YMDHMS_MS(m_dt, Year, Month, Day, Hour, Minute, Second, MilliSecond);
}

/*==========================================================================*/
inline int SCDateTime::GetYear() const
{
	int Year, Month, Day;
	GetDateYMD(Year, Month, Day);
	return Year;
}

/*==========================================================================*/
inline int SCDateTime::GetMonth() const
{
	int Year, Month, Day;
	GetDateYMD(Year, Month, Day);
	return Month;
}

/*==========================================================================*/
inline int SCDateTime::GetDay() const
{
	int Year, Month, Day;
	GetDateYMD(Year, Month, Day);
	return Day;
}

/*==========================================================================*/
inline int SCDateTime::GetHour() const
{
	return HOUR_OF_TIME(TIME_PART(m_dt));
}

/*==========================================================================*/
inline int SCDateTime::GetMinute() const
{
	return MINUTE_OF_TIME(TIME_PART(m_dt));
}

/*==========================================================================*/
inline int SCDateTime::GetSecond() const
{
	return SECOND_OF_TIME(TIME_PART(m_dt));
}

/*==========================================================================*/
inline int SCDateTime::GetMilliSecond() const
{
	return MS_PART(m_dt);
}

/*==========================================================================*/
inline int SCDateTime::GetDayOfWeek() const
{
	return DAY_OF_WEEK(m_dt);
}

/*==========================================================================*/
inline bool SCDateTime::IsWeekend() const
{
	int DayOfWeek = GetDayOfWeek();
	return (DayOfWeek == SATURDAY || DayOfWeek == SUNDAY);
}

/*==========================================================================*/
inline bool SCDateTime::IsSaturday() const
{
	int DayOfWeek = GetDayOfWeek();
	return (DayOfWeek == SATURDAY);
}

/*==========================================================================*/
inline bool SCDateTime::IsSunday() const
{
	int DayOfWeek = GetDayOfWeek();
	return (DayOfWeek == SUNDAY);
}
/*==========================================================================*/
inline void SCDateTime::RoundDateTimeDownToSecond()
{
	m_dt -= MS_PART(m_dt)*MILLISECONDS;
}


/*==========================================================================*/
inline void SCDateTime::RoundToNearestSecond()
{
	__int64 TimeAsSeconds =(__int64) ( (m_dt + HALF_SECOND) * SECONDS_PER_DAY);
	m_dt = TimeAsSeconds * SECONDS+(0.25 * MILLISECONDS);

}

/*==========================================================================*/
inline SCDateTime& SCDateTime::SetDateTime(double DateTime)
{
	m_dt = DateTime;
	return *this;
}

/*==========================================================================*/
inline SCDateTime& SCDateTime::SetDateTime(int Date, int Time)
{
	m_dt = COMBINE_DATE_TIME(Date, Time);
	return *this;
}

/*==========================================================================*/
inline SCDateTime& SCDateTime::SetDate(int Date)
{
	m_dt = m_dt - (int)m_dt + Date;
	return *this;
}

/*==========================================================================*/
inline SCDateTime& SCDateTime::SetTime(int Time)
{
	m_dt = (int)m_dt + Time*SECONDS;
	return *this;
}

/*==========================================================================*/
inline SCDateTime& SCDateTime::SetDateYMD(int Year, int Month, int Day)
{
	return SetDate(YMD_DATE(Year, Month, Day));
}

/*==========================================================================*/
inline SCDateTime& SCDateTime::SetTimeHMS(int Hour, int Minute, int Second)
{
	return SetTime(HMS_TIME(Hour, Minute, Second));
}

/*==========================================================================*/
inline SCDateTime& SCDateTime::SetTimeHMS_MS(int Hour, int Minute, int Second, int MilliSecond)
{
	SetTime(HMS_TIME(Hour, Minute, Second));
	m_dt += (MilliSecond*MILLISECONDS);
	return *this;
}

/*==========================================================================*/
inline SCDateTime& SCDateTime::SetDateTimeYMDHMS(int Year, int Month, int Day, int Hour, int Minute, int Second)
{
	return SetDateTime(YMDHMS_DATETIME(Year, Month, Day, Hour, Minute, Second));
}

/*==========================================================================*/
inline SCDateTime& SCDateTime::SetDateTimeYMDHMS_MS(int Year, int Month, int Day, int Hour, int Minute, int Second, int MilliSecond)
{
	return SetDateTime(YMDHMS_MS_DATETIME(Year, Month, Day, Hour, Minute, Second, MilliSecond));
}

/****************************************************************************/
// SCDateTimeMS

/*==========================================================================*/
inline SCDateTimeMS::SCDateTimeMS()
	:SCDateTime(0.0)
{
}

/*==========================================================================*/
inline SCDateTimeMS::SCDateTimeMS(double DateTime)
	: SCDateTime(DateTime)
{
}

/*==========================================================================*/
inline SCDateTimeMS::SCDateTimeMS(const SCDateTime& DateTime)
	: SCDateTime(DateTime)
{
}

/*==========================================================================*/
inline SCDateTimeMS::SCDateTimeMS(const SCDateTimeMS& DateTime)
	: SCDateTime(DateTime.m_dt)
{
}

/*==========================================================================*/
inline SCDateTimeMS::SCDateTimeMS(int Date, int Time)
	: SCDateTime(COMBINE_DATE_TIME(Date, Time))
{
}

/*==========================================================================*/
inline const SCDateTimeMS& SCDateTimeMS::operator = (double dt)
{
	m_dt = dt;
	return *this;
}

/*==========================================================================*/
inline const SCDateTimeMS& SCDateTimeMS::operator = (const SCDateTime& DateTime)
{
	m_dt = DateTime.m_dt;
	return *this;
}

/*==========================================================================*/
inline bool SCDateTimeMS::operator == (int Date) const
{
	return (fabs(m_dt - Date) < SCDATETIME_EPSILON_MS);
}

/*==========================================================================*/
inline bool SCDateTimeMS::operator != (int Date) const
{
	return !(fabs(m_dt - Date) < SCDATETIME_EPSILON_MS);
}

/*==========================================================================*/
inline bool SCDateTimeMS::operator < (int Date) const
{
	return (m_dt - Date < -SCDATETIME_EPSILON_MS);
}

/*==========================================================================*/
inline bool SCDateTimeMS::operator <= (int Date) const
{
	return (m_dt - Date < SCDATETIME_EPSILON_MS);
}

/*==========================================================================*/
inline bool SCDateTimeMS::operator > (int Date) const
{
	return (m_dt - Date > SCDATETIME_EPSILON_MS);
}

/*==========================================================================*/
inline bool SCDateTimeMS::operator >= (int Date) const
{
	return (m_dt - Date > -SCDATETIME_EPSILON_MS);
}

/*==========================================================================*/
inline bool SCDateTimeMS::operator == (double dt) const
{
	return (fabs(m_dt - dt) < SCDATETIME_EPSILON_MS);
}

/*==========================================================================*/
inline bool SCDateTimeMS::operator != (double dt) const
{
	return !(fabs(m_dt - dt) < SCDATETIME_EPSILON_MS);
}

/*==========================================================================*/
inline bool SCDateTimeMS::operator < (double dt) const
{
	return (m_dt - dt < -SCDATETIME_EPSILON_MS);
}

/*==========================================================================*/
inline bool SCDateTimeMS::operator <= (double dt) const
{
	return (m_dt - dt < SCDATETIME_EPSILON_MS);
}

/*==========================================================================*/
inline bool SCDateTimeMS::operator > (double dt) const
{
	return (m_dt - dt > SCDATETIME_EPSILON_MS);
}

/*==========================================================================*/
inline bool SCDateTimeMS::operator >= (double dt) const
{
	return (m_dt - dt > -SCDATETIME_EPSILON_MS);
}

/*==========================================================================*/
inline bool SCDateTimeMS::operator == (const SCDateTime& DateTime) const
{
	return (fabs(m_dt - DateTime.m_dt) < SCDATETIME_EPSILON_MS);
}

/*==========================================================================*/
inline bool SCDateTimeMS::operator != (const SCDateTime& DateTime) const
{
	return !(fabs(m_dt - DateTime.m_dt) < SCDATETIME_EPSILON_MS);
}

/*==========================================================================*/
inline bool SCDateTimeMS::operator < (const SCDateTime& DateTime) const
{
	return (m_dt - DateTime.m_dt < -SCDATETIME_EPSILON_MS);
}

/*==========================================================================*/
inline bool SCDateTimeMS::operator <= (const SCDateTime& DateTime) const
{
	return (m_dt - DateTime.m_dt < SCDATETIME_EPSILON_MS);
}

/*==========================================================================*/
inline bool SCDateTimeMS::operator > (const SCDateTime& DateTime) const
{
	return (m_dt - DateTime.m_dt > SCDATETIME_EPSILON_MS);
}

/*==========================================================================*/
inline bool SCDateTimeMS::operator >= (const SCDateTime& DateTime) const
{
	return (m_dt - DateTime.m_dt > -SCDATETIME_EPSILON_MS);
}

/*==========================================================================*/
inline bool SCDateTimeMS::operator == (const SCDateTimeMS& DateTimeMS) const
{
	return (fabs(m_dt - DateTimeMS.m_dt) < SCDATETIME_EPSILON_MS);
}

/*==========================================================================*/
inline bool SCDateTimeMS::operator != (const SCDateTimeMS& DateTimeMS) const
{
	return (fabs(m_dt - DateTimeMS.m_dt) >= SCDATETIME_EPSILON_MS);
}

/*==========================================================================*/
inline bool SCDateTimeMS::operator < (const SCDateTimeMS& DateTimeMS) const
{
	return (m_dt - DateTimeMS.m_dt < -SCDATETIME_EPSILON_MS);
}

/*==========================================================================*/
inline bool SCDateTimeMS::operator <= (const SCDateTimeMS& DateTimeMS) const
{
	return (m_dt - DateTimeMS.m_dt < SCDATETIME_EPSILON_MS);
}

/*==========================================================================*/
inline bool SCDateTimeMS::operator > (const SCDateTimeMS& DateTimeMS) const
{
	return (m_dt - DateTimeMS.m_dt > SCDATETIME_EPSILON_MS);
}

/*==========================================================================*/
inline bool SCDateTimeMS::operator >= (const SCDateTimeMS& DateTimeMS) const
{
	return (m_dt - DateTimeMS.m_dt > -SCDATETIME_EPSILON_MS);
}

/*==========================================================================*/
//Same function exists in the base class, so this does not seem necessary and is commented out
//inline void SCDateTimeMS::GetDateTimeYMDHMS_MS(int& Year, int& Month, int& Day, int& Hour, int& Minute, int& Second,int&  Millisecond) const
//{
//	DATETIME_TO_YMDHMS_MS(m_dt, Year, Month, Day, Hour, Minute, Second, Millisecond);
//}

/*============================================================================
	Compares the given DateTime to the member date-time without regard to the milliseconds.
----------------------------------------------------------------------------*/
inline bool SCDateTimeMS::IsSameTimeToSecond(const SCDateTime& DateTime) const
{
	//Optimization
	static const  double Limit = 1002*MILLISECONDS;
	double TimeDifference = m_dt - DateTime;
	if (TimeDifference > Limit  || TimeDifference < -Limit )
		return false;

	//Need to remove the millisecond component for the comparison
	 TimeDifference = (m_dt - GetMilliSecond()*MILLISECONDS) - (DateTime- DateTime.GetMilliSecond() * MILLISECONDS);
	return (TimeDifference < MILLISECONDS && TimeDifference >  -MILLISECONDS);
}

/*==========================================================================*/
inline __int64 SCDateTimeToUNIXTimeRemoveMilliseconds(SCDateTime DateTime)
{
	DateTime.RoundDateTimeDownToSecond();
	return( __int64) ( (DateTime - 25569.0) * SECONDS_PER_DAY + 0.5);
}

/*==========================================================================*/

/****************************************************************************/
// Integer comparison operators

/*==========================================================================*/
inline bool operator == (int IntDateTime, const SCDateTime& DateTime)
{
	return (fabs(IntDateTime - DateTime) < SCDATETIME_EPSILON);
}

/*==========================================================================*/
inline bool operator != (int dt, const SCDateTime& DateTime)
{
	return !(fabs(dt - DateTime) < SCDATETIME_EPSILON);
}

/*==========================================================================*/
inline bool operator < (int dt, const SCDateTime& DateTime)
{
	return (dt - DateTime < -SCDATETIME_EPSILON);
}

/*==========================================================================*/
inline bool operator <= (int dt, const SCDateTime& DateTime)
{
	return (dt - DateTime < SCDATETIME_EPSILON);
}

/*==========================================================================*/
inline bool operator > (int dt, const SCDateTime& DateTime)
{
	return (dt - DateTime > SCDATETIME_EPSILON);
}

/*==========================================================================*/
inline bool operator >= (int dt, const SCDateTime& DateTime)
{
	return (dt - DateTime > -SCDATETIME_EPSILON);
}

/****************************************************************************/
// Double comparison operators

/*==========================================================================*/
inline bool operator == (double dt, const SCDateTime& DateTime)
{
	return (fabs(dt - DateTime) < SCDATETIME_EPSILON);
}

/*==========================================================================*/
inline bool operator != (double dt, const SCDateTime& DateTime)
{
	return !(fabs(dt - DateTime) < SCDATETIME_EPSILON);
}

/*==========================================================================*/
inline bool operator < (double dt, const SCDateTime& DateTime)
{
	return (dt - DateTime < -SCDATETIME_EPSILON);
}

/*==========================================================================*/
inline bool operator <= (double dt, const SCDateTime& DateTime)
{
	return (dt - DateTime < SCDATETIME_EPSILON);
}

/*==========================================================================*/
inline bool operator > (double dt, const SCDateTime& DateTime)
{
	return (dt - DateTime > SCDATETIME_EPSILON);
}

/*==========================================================================*/
inline bool operator >= (double dt, const SCDateTime& DateTime)
{
	return (dt - DateTime > -SCDATETIME_EPSILON);
}

/*==========================================================================*/
inline double GetNextMillisecondForSameSecondDateTime(double PreviousDateTime, double NextDateTime)
{
	//Purpose: We need to make a new record timestamps unique by using milliseconds.


	SCDateTimeMS PreviousDateTimeCopy(PreviousDateTime);

	if(PreviousDateTimeCopy.IsSameTimeToSecond(NextDateTime) )//Timestamp to the second is the same.  
	{
		int Milliseconds = PreviousDateTimeCopy.GetMilliSecond();

		if (Milliseconds >= MAXIMUM_INTRADAY_DATA_FILE_MILLISECONDS)
			NextDateTime = PreviousDateTimeCopy;
		else
			NextDateTime = PreviousDateTimeCopy + 1 * MILLISECONDS;
	}

	return NextDateTime;
}


/*==========================================================================*/

//This function is thread safe.
inline  SCDateTime GetCurrentDateTimeInGMT( )
{
	SYSTEMTIME st;


	::GetSystemTime(&st);

	//return YMDHMS_MS_DATETIME(st.wYear, st.wMonth, st.wDay,st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

	return YMDHMS_DATETIME(st.wYear, st.wMonth, st.wDay,st.wHour, st.wMinute, st.wSecond);

}


/****************************************************************************/
// Special CLR code

#ifdef _MANAGED
#pragma managed

/*==========================================================================*/
inline System::DateTime UnmanagedDateTimeToManaged(const SCDateTime& DateTime)
{
	int Year, Month, Day, Hour, Minute, Second;
	DateTime.GetDateTimeYMDHMS(Year, Month, Day, Hour, Minute, Second);
	return System::DateTime(Year, Month, Day, Hour, Minute, Second);
}

/*==========================================================================*/
inline SCDateTime ManagedDateTimeToUnmanaged(System::DateTime% DateTime)
{
	return SCDateTime(DateTime.Year, DateTime.Month, DateTime.Day, DateTime.Hour, DateTime.Minute, DateTime.Second);
}

#pragma unmanaged
#endif

#endif

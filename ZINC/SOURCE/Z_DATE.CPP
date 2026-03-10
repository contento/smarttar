//	Zinc Interface Library - Z_DATE.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#define INCL_DOSDATETIME	// OS/2 date and time functions.
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include "ui_gen.hpp"
#if defined(ZIL_MSDOS) || defined(ZIL_MSWINDOWS) || (defined(ZIL_OS2) && !defined(__IBMCPP__)) || defined(__DVX__)
#include <dos.h>
#endif
#if defined(ZIL_MSWINDOWS) || defined(ZIL_UNIX)
#include <time.h>
#endif
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

static int _daysInMonth[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

static int LeapYear(int year)
{
	return ((year % 400 == 0) || ((year % 4 == 0) && (year % 100 != 0)));
}

int UI_DATE::DayOfWeek(void)
{
#if defined(ZIL_LINKBUG)
	extern void z_date_dummy(void);			// Bug fix for broken linkers.
	z_date_dummy();
#endif

	return ((int)(((value + 1) % 7) + 1));
}

int UI_DATE::DaysInMonth(void)
{
	int year, month;
	Export(&year, &month, NULL);
	if (month == 2 && LeapYear(year))
		return (29);
	return (_daysInMonth[month - 1]);
}

int UI_DATE::DaysInYear(void)
{
	int year;
	Export(&year, NULL, NULL);
	return (LeapYear(year) ? 366 : 365);
}

void UI_DATE::Export(int *pYear, int *pMonth, int *pDay, int *pDayOfWeek)
{
	long year = 0, day = 0, month = 0;
	if (value >= 0)
	{
		// Compute the separate date values using algorithm 199 of
		// the CALGO of the ACM.
		long tValue = value - 1721119L;
		year = (tValue * 4 - 1) / 146097L;
		tValue = tValue * 4 - 1 - 146097L * year;
		day = tValue / 4;
		tValue = (day * 4 + 3) / 1461L;
		day = day * 4 + 3 - 1461L * tValue;
		day = (day + 4) / 4;
		month = (5 * day - 3) / 153;
		day = 5 * day - 3 - 153 * month;
		day = (day + 5) / 5;
		year = 100 * year + tValue;
		if (month < 10)
			month += 3;
		else
		{
			month -= 9;
			year++;
		}
	}
	// Set the integer variables.
	if (pYear)
		*pYear = (int)year;
	if (pMonth)
		*pMonth = (int)month;
	if (pDay)
		*pDay = (int)day;
	if (pDayOfWeek)
		*pDayOfWeek = DayOfWeek();
}

static void setString(char *str, int val, int flags)
{
	::sprintf(str, (FlagSet(flags, DTF_ZERO_FILL) && val < 10 ? "%02d" : "%d"), val);
}

void UI_DATE::Export(char *string, DTF_FLAGS dtFlags)
{
	if (value < 0)
	{
		strcpy(string, "");
		return;
	}
	// Make sure the country information is initialized.
	UI_INTERNATIONAL::Initialize();
	char *tString = string;

	// Determine the separator character.
	char *yearSeparator, *monthSeparator, *daySeparator;
	if (FlagSet(dtFlags, DTF_MILITARY_FORMAT))
	{
		dtFlags &= (DTF_MILITARY_FORMAT | DTF_UPPER_CASE | DTF_ZERO_FILL |
			DTF_SHORT_YEAR | DTF_SHORT_MONTH | DTF_SYSTEM);
		dtFlags |= DTF_ALPHA_MONTH;
		if (FlagSet(dtFlags, DTF_SHORT_YEAR | DTF_SHORT_MONTH))
			dtFlags |= DTF_SHORT_YEAR | DTF_SHORT_MONTH;
		yearSeparator = monthSeparator = daySeparator = " ";
	}
	else if (FlagSet(dtFlags, DTF_DASH))
		yearSeparator = monthSeparator = daySeparator = "-";
	else if (FlagSet(dtFlags, DTF_SLASH))
		yearSeparator = monthSeparator = daySeparator = "/";
	else
		yearSeparator = monthSeparator = daySeparator = dateSeparator;

	// Get the integer date values.
	char year[32], month[32], day[32];
	int yearValue, monthValue, dayValue, dayOfWeekValue;
	Export(&yearValue, &monthValue, &dayValue, &dayOfWeekValue);

	// Determine the year value.
	if (FlagSet(dtFlags, DTF_SHORT_YEAR))
		::sprintf(year, "%02d", yearValue % 100);
	else
		setString(year, yearValue, dtFlags);

	// Determine the month value.
	if (FlagSet(dtFlags, DTF_ALPHA_MONTH | DTF_SHORT_MONTH))
	{
		int value = FlagSet(dtFlags, DTF_SHORT_MONTH) ? 0 : 1;
		strcpy(month, monthTable[--monthValue * 2 + value]);
		if (FlagSet(dtFlags, DTF_MILITARY_FORMAT))
		{
			daySeparator = " ";
			if (FlagSet(dtFlags, DTF_SHORT_MONTH))
				month[3] = '\0';
		}
		else
			daySeparator = ", ";
		monthSeparator = " ";
	}
	else setString(month, monthValue, dtFlags);

	// Determine the day value.
	setString(day, dayValue, dtFlags);

	// Add the day of week value to the date.
	if (FlagSet(dtFlags, DTF_DAY_OF_WEEK | DTF_SHORT_DAY))
	{
		int value = FlagSet(dtFlags, DTF_SHORT_DAY) ? 0 : 1;
		::sprintf(tString, "%s ", dayTable[(dayOfWeekValue - 1) * 2 + value]);// BUG.SunOS4
		tString += strlen(tString);
	}

	// Format the date according to the country information.
	char *s1 = day,
	     *s2 = daySeparator,
	     *s3 = month,
	     *s4 = monthSeparator,
	     *s5 = year;
	if (FlagSet(dtFlags, DTF_JAPANESE_FORMAT))
		s1 = year, s2 = yearSeparator, s5 = day;
	else if (FlagSet(dtFlags, DTF_EUROPEAN_FORMAT | DTF_MILITARY_FORMAT))
		; // already done.
	else if (FlagSet(dtFlags, DTF_US_FORMAT))
		s1 = month, s2 = monthSeparator, s3 = day, s4 = daySeparator;
	else if (dateFormat == 2)
		s1 = year, s2 = yearSeparator, s5 = day;
	else if (dateFormat == 1)
		; // already done.
	else
		s1 = month, s2 = monthSeparator, s3 = day, s4 = daySeparator;
	::sprintf(tString, "%s%s%s%s%s", s1, s2, s3, s4, s5);
	if (FlagSet(dtFlags, DTF_UPPER_CASE))
		ui_strupr(string);
}

void UI_DATE::Export(int *packedDate)
{
	int year, month, day;

	if (value < 0)
		*packedDate = 0;
	else
	{
		Export(&year, &month, &day);
		*packedDate = ((year - 1980) << 9) | (month << 5) | day;
	}
}

DTI_RESULT UI_DATE::Import(void)
{
	// Set the date values according to the system date.
#if defined(ZIL_UNIX) || (defined(ZIL_MSWINDOWS) && defined(WIN32)) || (defined(__GNUC__) && defined(__DVX__))
	time_t t;
	(void)time(&t);
	struct tm *localTime = localtime(&t);
	// Date years are from 1900, don't change this for years past 2000
	Import(1900 + localTime->tm_year, localTime->tm_mon + 1, localTime->tm_mday);
#elif defined(__ZTC__)
	struct dos_date_t info;
	dos_getdate(&info);
	Import(info.year, info.month, info.day);
#elif defined(_MSC_VER)
	struct _dosdate_t info;
	_dos_getdate(&info);
	Import(info.year, info.month, info.day);
#elif defined(__WATCOMC__) || defined(__HIGHC__)
	struct dosdate_t info;
	_dos_getdate(&info);
	Import(info.year, info.month, info.day);
#elif defined(__BCPLUSPLUS__) | defined(__TCPLUSPLUS__)
	struct date info;
	getdate(&info);
	Import(info.da_year, info.da_mon, info.da_day);
#elif defined(__IBMCPP__)
	DATETIME info;
	DosGetDateTime(&info);
	Import(info.year, info.month, info.day);
#else 
	??? // Undetected compiler.
#endif
	return (DTI_OK);
}

DTI_RESULT UI_DATE::Import(const UI_DATE &date)
{
	// Set the date value according to the passed date.
	value = date.value;
	return (DTI_OK);
}

DTI_RESULT UI_DATE::Import(int year, int month, int day)
{
	// Make sure year, month and day are valid.
//	if (!year || !month || !day)						// BUG.internal
//	{
//		UI_DATE date;
//		date.Export(year ? NULL : &year, month ? NULL : &month, day ? NULL : &day);
//	}
	if (year < 100)
	{
		// year += 1900;
		// Assume two digit years are offsets in the current century
		UI_DATE current;						// BUG.internal
		int y1;
		current.Export(&y1, NULL, NULL);
		year += y1 - (y1 % 100);
	}
	// Compute the julian date from three integer values using
	// algorithm 199 of CALGO of the ACM.
	if (year <= 0 || month < 1 || month > 12 || day < 1 ||			// BUG.internal
		(month == 2 && day == 29 && !LeapYear(year)) ||
		((month != 2 || day != 29) && day > _daysInMonth[month-1]))
		return (DTI_INVALID);
	if (month > 2)	      
		month -= 3;
	else
	{
		month += 9;
		year--;
	}
	value = ((146097L * (year / 100)) / 4) + ((1461L * (year % 100)) / 4) + (153L * month + 2) / 5 + day + 1721119L;

	return (DTI_OK);
}

DTI_RESULT UI_DATE::Import(const char *string, DTF_FLAGS dtFlags)
{
	// Check for a system date.
	if (!string || !string[0])
		if (FlagSet(dtFlags, DTF_SYSTEM))
			return (UI_DATE::Import());
		else
		{
			value = -1L;
			return DTI_OK;
		}

	// Make sure the country information is initialized.
	UI_INTERNATIONAL::Initialize();

	// Set up the date variables and determine their proper order.
	int date[3];
	date[0] = date[1] = date[2] = 0;
	int year, month, day;
	day = 0; month = 1; year = 2;
	if (FlagSet(dtFlags, DTF_JAPANESE_FORMAT))
		day = 2, year = 0;
	else if (FlagSet(dtFlags, DTF_EUROPEAN_FORMAT | DTF_MILITARY_FORMAT))
		;// already set
	else if (FlagSet(dtFlags, DTF_US_FORMAT))
		day = 1, month = 0;
	else if (dateFormat == 2)
		day = 2, year = 0;
	else if (dateFormat == 1)
		;// already set
	else
		day = 1, month = 0;

	// Parse out the string date.
	for (int i = 0; i < 3 && *string; i++)
	{
		while (*string && !isdigit(*string) && !isalpha(*string))
			string++;
		if (isdigit(*string))
		{
			for (int value = 0; *string && isdigit(*string); *string++)
				value = value * 10 + *string - '0';
			date[i] = value;
		}
		else if (isalpha(*string))
		{
			int found = FALSE;
			const char *tString = string;
			while (*string && isalpha(*string))
				string++;
//			for (int j = 0; j < 24 && !found; j++)			// BUG.1508
			for (int j = 0; j < 24; j += 2)
				if (ui_strnicmp(tString, monthTable[j+1], (int)(string - tString)) == 0 ||
				    ui_stricmp(tString, monthTable[j]) == 0)
				{
					if (found)				// BUG.1508
					{
						value = -1L;
						return DTI_AMBIGUOUS;
					}
					date[i] = (j / 2) + 1;
					if (year == i)
						year = month;
					else if (day == i)
						day = month;
					month = i;
					found = TRUE;
				}
			if (!found)
				i--;
		}
	}
	if (i != 3)	// We didn't parse out a complete date...
		return DTI_INVALID;

	// Set the date according to the parsed values.
	return (Import(date[year], date[month], date[day]));
}

DTI_RESULT UI_DATE::Import(int packedDate)
{
	if (packedDate == -1)
	{
		value = -1;		// This can work only because -1 isn't
		return (DTI_OK);	// a valid packed date.
	}
	return(Import(1980 + (((int)(packedDate & 0xFE00)) >> 9),
		      ((int)(packedDate & 0x01E0)) >> 5, packedDate & 0x001F));
}



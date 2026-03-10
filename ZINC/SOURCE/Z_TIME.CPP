//	Zinc Interface Library - Z_TIME.CPP
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
#if defined(ZIL_MSWINDOWS) || defined(ZIL_POSIX)
#include <time.h>
#endif
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif
#if defined(sun) && !defined(_SUNOS4)
	extern "C" int gettimeofday(struct timeval *tp, struct timezone *tzp);
#elif defined(_IBM_RS6000)
#	include <sys/time.h>
#endif

// ----- UI_TIME ------------------------------------------------------------

void UI_TIME::Export(int *hour, int *minute, int *second, int *hundredth)
{
	long value1 = value;
	if (value < 0)
		value1 = 0L;
	// Set the integer variables.
	long value2 = value1 / 360000L;
	if (hour)
		*hour = (int)value2;
	value1 -= value2 * 360000L;
	value2 = value1 / 6000L;
	if (minute)
		*minute = (int)value2;
	value1 -= value2 * 6000L;
	value2 = value1 / 100L;
	if (second)
		*second = (int)value2;
	value1 -= value2 * 100L;
	if (hundredth)
		*hundredth = (int)value1;
}

void UI_TIME::Export(char *string, TMF_FLAGS tmFlags)
{
#if defined(ZIL_LINKBUG)
	extern void z_time_dummy(void);			// Bug fix for broken linkers.
	z_time_dummy();
#endif

	if (value < 0)
	{
		strcpy(string, "");
		return;
	}
	// Make sure the country information is initialized.
	UI_INTERNATIONAL::Initialize();

	// Determine the seperator character.
	char *separator = timeSeparator;
	if (FlagSet(tmFlags, TMF_COLON_SEPARATOR))
		separator = ":";
	else if (FlagSet(tmFlags, TMF_NO_SEPARATOR))
		separator = "";

	// Determine the time format.
	long hourValue = -1;
	char *format = FlagSet(tmFlags, TMF_ZERO_FILL | TMF_NO_SEPARATOR) ? "%02ld" : "%ld";

	// Format the string.
	strcpy(string, "");
	long value1 = value;
	long value2;
	long ratio = 360000L;
	for (int i = 0; i < 4; i++)
	{
		// Strip off the relavant value.
		value2 = value1 / ratio;
		value1 -= value2 * ratio;
		if (ratio == 360000L)
			ratio = 6000L;
		else if (ratio == 6000L)
			ratio = 100L;
		else
			ratio = 1L;

		// See if the string should contain the value.
		if ((i == 0 && !FlagSet(tmFlags, TMF_NO_HOURS)) ||
			(i == 1 && !FlagSet(tmFlags, TMF_NO_MINUTES)) ||
			(i == 2 && FlagSet(tmFlags, TMF_SECONDS)) ||
			(i == 3 && FlagSet(tmFlags, TMF_HUNDREDTHS)))
		{
			// Append the number separator.
			if (*string)
				strcat(string, (i == 3) ? decimalSeparator : separator);
			// Check for the 12 hour timing.
			if (i == 0 && (FlagSet(tmFlags, TMF_TWELVE_HOUR) ||
				(timeFormat == 0 && !FlagSet(tmFlags, TMF_TWENTY_FOUR_HOUR))))
			{
				hourValue = value2;
				value2 = (value2 == 0 || value2 == 12) ? 12 : value2 % 12;
			}
			// Append the time value.
			char sValue[8];
			::sprintf(sValue, *string ? "%02ld" : format, value2);
			strcat(string, sValue);
		}
	}

	// Set the am/pm information.
	if (hourValue >= 0)
	{
		// Append the am/pm string.
		strcat(string, " ");
		strcat(string, (hourValue >= 12) ? pmPtr : amPtr);
		// Convert to upper or lower case.
		if (FlagSet(tmFlags, TMF_UPPER_CASE))
			ui_strupr(string);
		else if (FlagSet(tmFlags, TMF_LOWER_CASE))
			ui_strlwr(string);
	}
}

void UI_TIME::Export(int *packedTime)
{
	int hour, minute, second;
	
	if (value < 0)
		*packedTime = 0;
	else
	{
		Export(&hour, &minute, &second);
		*packedTime = (hour << 11) | (minute << 5) | second / 2;
	}
}

TMI_RESULT UI_TIME::Import(void)
{
	// Set the time values according to the system time.
#if defined(ZIL_POSIX)
#define USE_GETTIMEOFDAY		// Must use this for usec stuff.
#if defined(USE_GETTIMEOFDAY)
	// Use GREAT Berkeley function!!!
	struct timeval tv;
	struct timezone tz;
	gettimeofday(&tv, &tz);
	// Fix the time for the right timezone/daylight savings first.
#if defined(sun) && !defined(_SUNOS4)
	tzset();
	tv.tv_sec -= altzone;
#else
	tv.tv_sec -= (tz.tz_minuteswest * 60);
	tv.tv_sec += (tz.tz_dsttime * 60 * 60);
#endif
	tv.tv_sec %= (60 * 60 * 24);
	Import((int)(tv.tv_sec / (60 * 60)), (int)((tv.tv_sec / 60) % 60),
	       (int)(tv.tv_sec % 60), (int)(tv.tv_usec / (1000 * 10)));
#else
	// Do boring POSIX stuff...
	time_t t;
	time(&t);
	struct tm *localTime = localtime(&t);
	Import(localTime->tm_hour, localTime->tm_min, localTime->tm_sec, 0);
#endif
#elif defined(ZIL_MSWINDOWS) && defined(WIN32)
	time_t t;
	time(&t);		// This function MUST be called before GetSystemTime().
	SYSTEMTIME gmtTime;
	GetSystemTime(&gmtTime);
	struct tm *localTime = localtime(&t);

#	if defined(_MSC_VER) && defined(WIN32)

	// Bug with Microsoft's localtime function.  Adjust for Daylight savings.
	TIME_ZONE_INFORMATION tZInfo;
	GetTimeZoneInformation(&tZInfo);
	int cYear, cMonth, cDay;
	UI_DATE cDate;
	cDate.Export(&cYear, &cMonth, &cDay);
	if ((cMonth > tZInfo.DaylightDate.wMonth ||
		cMonth == tZInfo.DaylightDate.wMonth && cDay >= tZInfo.DaylightDate.wDay) &&
		(cMonth < tZInfo.StandardDate.wMonth ||
		cMonth == tZInfo.StandardDate.wMonth && cDay < tZInfo.StandardDate.wDay))
			localTime->tm_hour += -tZInfo.DaylightBias / 60;
	else
			localTime->tm_hour += -tZInfo.StandardBias / 60;

	localTime->tm_hour %= 24; 

#	elif defined(__BCPLUSPLUS__) && defined(WIN32)

	// Bug with Borland's localtime function.  Adjust for GMT.
	TIME_ZONE_INFORMATION tZInfo;
	GetTimeZoneInformation(&tZInfo);
	int cYear, cMonth, cDay;
	UI_DATE cDate;
	cDate.Export(&cYear, &cMonth, &cDay);
	if ((cMonth > tZInfo.DaylightDate.wMonth ||
		cMonth == tZInfo.DaylightDate.wMonth && cDay >= tZInfo.DaylightDate.wDay) &&
		(cMonth < tZInfo.StandardDate.wMonth ||
		cMonth == tZInfo.StandardDate.wMonth && cDay < tZInfo.StandardDate.wDay))
			tZInfo.Bias += tZInfo.DaylightBias;
	else
			tZInfo.Bias += tZInfo.StandardBias;

	localTime->tm_min + tZInfo.Bias % 60;
	if (localTime->tm_min < 0)
	{
		localTime->tm_min + 60;
		localTime->tm_hour - 1; 
	}
	if (localTime->tm_min > 60)
	{
		localTime->tm_min - 60;
		localTime->tm_hour + 1; 
	}

	localTime->tm_hour += tZInfo.Bias / 60;
	localTime->tm_hour %= 24; 
#	endif

	Import(localTime->tm_hour, localTime->tm_min, localTime->tm_sec,
		(localTime->tm_sec == gmtTime.wSecond) ? gmtTime.wMilliseconds / 10 : 0);
#elif defined(__ZTC__)
	struct dos_time_t info;
	dos_gettime(&info);
	Import(info.hour, info.minute, info.second, info.hsecond);
#elif defined(_MSC_VER)
	struct dostime_t info;
	_dos_gettime(&info);
	Import(info.hour, info.minute, info.second, info.hsecond);
#elif defined(__WATCOMC__) || defined(__HIGHC__)
	struct dostime_t info;
	_dos_gettime(&info);
	Import(info.hour, info.minute, info.second, info.hsecond);
#elif defined(__BCPLUSPLUS__) | defined(__TCPLUSPLUS__)
	struct time info;
	gettime(&info);
	Import(info.ti_hour, info.ti_min, info.ti_sec, info.ti_hund);
#elif defined(__IBMCPP__)
	DATETIME info;
	DosGetDateTime(&info);
	Import(info.hours, info.minutes, info.seconds, info.hundredths);
#elif defined(__GNUC__) && defined(__DVX__)
	time_t t;
	time(&t);
	struct tm *localTime = localtime(&t);
	Import(localTime->tm_hour, localTime->tm_min, localTime->tm_sec, 0);
#else 
	??? // Undetected compiler.
#endif
	return (TMI_OK);
}

TMI_RESULT UI_TIME::Import(const UI_TIME &time)
{
	// Set the time value according to the passed time.
	value = time.value;
	return (TMI_OK);
}

TMI_RESULT UI_TIME::Import(int hour, int minute, int second, int hundredth)
{
	// Set the time values according to the specified integers.
	if (hour < 0 || hour > 23 || minute < 0 || minute > 59 ||
		second < 0 || second > 59 || hundredth < 0 || hundredth > 99)
		return (TMI_INVALID);
	value = (3600L * hour + 60L * minute + second) * 100 + hundredth;
	return (TMI_OK);
}

TMI_RESULT UI_TIME::Import(const char *string, TMF_FLAGS tmFlags)
{
	// Check for a system time.
	if (!string || !string[0])
		if (FlagSet(tmFlags, TMF_SYSTEM))
			return (UI_TIME::Import());
		else
		{
			value = -1L;
			return TMI_OK;
		}

	UI_INTERNATIONAL::Initialize();

	// Parse out the string time.
//	const char *saveString = string;
	int time[4];
	time[0] = time[1] = time[2] = time[3] = 0;
	static int maxTime[4] = { 23, 59, 59, 99 };
	char *Separators[4];
	Separators[0] = Separators[1] = timeSeparator;
	Separators[2] = decimalSeparator;
	Separators[3] = "";
	// The following breaks if there needs to be non-digit stuff before
	// a time (i.e. am 9:00)
	while (*string && !isdigit(*string))
		string++;
	for (int i = 0; i < 4 && isdigit(*string); i++)
	{
		int j = 0;
		int value = 0;
		while (isdigit(*string) &&
		       (j < 2 || !FlagSet(tmFlags, TMF_NO_SEPARATOR)))
		{
			value = value * 10 + *string - '0';
			if (value > maxTime[i])
				return (TMI_INVALID);
			string++;
			j++;
		}
		if ((i == 0 && !FlagSet(tmFlags, TMF_NO_HOURS)) ||
		    (i == 1 && !FlagSet(tmFlags, TMF_NO_MINUTES)) ||
		    (i == 2 && FlagSet(tmFlags, TMF_SECONDS)) ||
		    (i == 3 && FlagSet(tmFlags, TMF_HUNDREDTHS)))
			time[i] = value;
		if (!FlagSet(tmFlags, TMF_NO_SEPARATOR))
		{
			char *separator = Separators[i];
			if (*string == ':')
				string++;
			else while (*string == *separator)
				string++, separator++;
		}
	}
	// Find the am/pm string (if any).
//	string = saveString;							// BUG.internal
	while (*string && !isalpha(*string))
		string++;
	if (isalpha(*string))							// BUG.internal
	{
		if (ui_strnicmp(string, pmPtr, 1) == 0)
		{
			if (time[0] > 12)					// BUG.2034
				return (TMI_INVALID);
			if (time[0] < 12)					// BUG.2034
				time[0] += 12;
		}
		else if (ui_strnicmp(string, amPtr, 1) == 0)
		{
			if (time[0] > 12)
				return (TMI_INVALID);
			if (time[0] == 12)
				time[0] -= 12;
		}
	}
	// Set the time according to the parsed values.
	return (Import(time[0], time[1], time[2], time[3]));			// BUG.internal
}

TMI_RESULT UI_TIME::Import(int packedTime)
{
	if (packedTime == -1)
	{
		value = -1;		// This can work only because -1 isn't
		return (TMI_OK);	// a valid packed time.
	}
	return(Import((packedTime & 0xF800) >> 11, (packedTime & 0x07E0) >> 5, 2 * (packedTime & 0x001F)));
}


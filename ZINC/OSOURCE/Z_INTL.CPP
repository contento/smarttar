//	Zinc Interface Library - Z_INTL.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#define INCL_DOSNLS				// OS/2 Dos calls.
#define INCL_WINSHELLDATA		// OS/2 Dos calls.
#include <stdio.h>
#include <stdarg.h>
#if !(defined(__GNUC__) && defined(__DVX__))
#include <locale.h>
#endif
#include <limits.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "ui_gen.hpp"
#if defined(ZIL_MSDOS) || defined(ZIL_MSWINDOWS) || (defined(ZIL_OS2) && !defined(__IBMCPP__))
#	include <dos.h>
#endif
#if defined(_MSC_VER)
#	if defined(DOSX286)
#		define	_int86x	int86x
#		define	_REGS	REGS
#		define	_SREGS	SREGS
#	endif
#	pragma hdrstop			// Microsoft pre-compiled header pragma.
#endif

#define LENGTHOF(x)	(sizeof(x)/sizeof((x)[0]))

int UI_INTERNATIONAL::initialized = FALSE;
#if defined(ZIL_MSDOS) || defined(ZIL_MSWINDOWS) || defined(ZIL_OS2)
int UI_INTERNATIONAL::countryCode = 1;
#endif

char UI_INTERNATIONAL::languageName[64] = "English";
char UI_INTERNATIONAL::localeName[64] = "United States";
char UI_INTERNATIONAL::decimalSeparator[4] = ".";
char UI_INTERNATIONAL::monDecimalSeparator[4] = ".";
char UI_INTERNATIONAL::thousandsSeparator[4] = ",";
char UI_INTERNATIONAL::monThousandsSeparator[4] = ",";
char UI_INTERNATIONAL::grouping[10] = "\3";
char UI_INTERNATIONAL::monGrouping[10] = "\3";
char UI_INTERNATIONAL::currencySymbol[8] = "$";
char UI_INTERNATIONAL::intCurrencySymbol[8] = "$";
int UI_INTERNATIONAL::posCurrencyPrecedes = TRUE;
int UI_INTERNATIONAL::negCurrencyPrecedes = TRUE;
int UI_INTERNATIONAL::fractionDigits = 2;
int UI_INTERNATIONAL::intFractionDigits = 2;
char UI_INTERNATIONAL::positiveSign[4] = "";
int UI_INTERNATIONAL::posSignPrecedes = TRUE;
int UI_INTERNATIONAL::posSpaceSeparation = 0;
char UI_INTERNATIONAL::negativeSign[4] = "-";
int UI_INTERNATIONAL::negSignPrecedes = TRUE;
int UI_INTERNATIONAL::negSpaceSeparation = 0;

int UI_INTERNATIONAL::dateFormat = 0;
char UI_INTERNATIONAL::dateSeparator[4] = "/";

int UI_INTERNATIONAL::timeFormat = 0;
char UI_INTERNATIONAL::timeSeparator[4] = ":";

struct {
	int code;
	char *language;
	char *country;
} codeMap[] = {
	{   1, "English", "United States" },
	{   2, "French", "Quebec" },
	{   3, "Spanish", "Latin America" },
	{  31, "Dutch", "Netherlands" },
	{  32, "Flemish", "Belgium" },
	{  33, "French", "France" },
	{  34, "Spanish", "Spain" },
	{  36, "Hungarian", "Hungary" },
	{  38, "Serbo-Croat", "Yugoslavia" },
	{  39, "Italian", "Italy" },
	{  41, "Swiss", "Switzerland" },
	{  42, "Czech", "Czechoslavakia" },
	{  44, "English", "United Kingdom" },
	{  45, "Danish", "Denmark" },
	{  46, "Swedish", "Sweden" },
	{  47, "Norwegian", "Norway" },
	{  48, "Polish", "Poland" },
	{  49, "German", "Germany" },
	{  55, "Portuguese", "Brazil" },
	{  61, "English", "Australia" },
	{  81, "Japanese", "Japan" },
	{  82, "Korean", "Korea" },
	{  86, "Mandarin", "PROC" },
	{  88, "Mandarin", "ROC" },
	{  90, "Turk", "Turkey" },
	{ 351, "Portuguese",  "Portugal" },
	{ 354, "Icelandic",  "Iceland" },
	{ 358, "Finnish",  "Finland" },
	{ 785, "Arabic",  "Arabia" },
	{ 972, "Hebrew",  "Israel" },
	{  0,	NULL,	NULL}
};

enum type
{
	Z_INT, Z_LONG, Z_DOUBLE, Z_CHAR_STAR
};

typedef struct
{
	enum type type;
	int newPos, oldPos;
} rdata;

#if defined(__ZTC__) && (__ZTC__ >= 0x310)
	static int __CLIB rdatacmp(const void *a, const void *b)
#elif defined(__ZTC__)
	extern "C" {
	static int rdatacmp(const void *a, const void *b)
#elif defined(__IBMCPP__)
	extern "C" {
	int rdatacmp(const void *a, const void *b)
#else
	static int rdatacmp(const void *a, const void *b)
#endif
	{
		return ((rdata *)a)->newPos - ((rdata *)b)->newPos;		// BUG.internal
	}
#if (defined(__ZTC__) && (__ZTC__ < 0x310)) || defined(__IBMCPP__)
	}
#endif

void UI_INTERNATIONAL::rearrangeArgs(const char *format, const void *args,
	char *newFormat, void *newArgs, void *toRet)
{
	int i, j, islonger;
	rdata *tbl = new rdata[UII_MAXPARAM];
	int orgpos, first;
	const char *fmtStart;

	orgpos = 0;
	i = 0;
	first = 0;
	while (*format)
	{
		if (*format == '%')
		{
			fmtStart = format;
			format++;
			/* scan out the position */
			j = 0;
			while (isdigit(*format))
			{
				j *= 10;
				j += *format++ - '0';
			}
			j--;	// The position starts at 1
			if (j < 0 || j >= UII_MAXPARAM || *format != '%')
			{
				// no (valid) positional information
				j = first++;
				format = fmtStart;
			}
			newFormat[i++] = *format++;
			while (*format == '-' || *format == '+' ||
			       *format == '#' || *format == ' ')
				newFormat[i++] = *format++;
			if (*format == '*')
			{
				newFormat[i++] = *format++;
				tbl[orgpos].type = Z_INT;
				tbl[orgpos].newPos = j-1;
				tbl[orgpos].oldPos = orgpos;
				orgpos++;
			}
			else if (isdigit(*format))
			{
				while (isdigit(*format))
					newFormat[i++] = *format++;
			}
			if (*format == '.')
			{
				newFormat[i++] = *format++;
#if defined(TESTING)
				if (!isdigit(*format))
					abort();	/*raise(badformat)*/
#endif
				while (isdigit(*format))
					newFormat[i++] = *format++;
			}
			islonger = 0;
			switch (*format)
			{
#if defined(ZIL_MSDOS)
			case 'N':
				newFormat[i++] = *format++;
				break;
			case 'F':
				newFormat[i++] = *format++;
				break;
#endif
			case 'h':
				newFormat[i++] = *format++;
				break;
			case 'l':
				newFormat[i++] = *format++;
				islonger = 1;
				break;
			case 'L':
				newFormat[i++] = *format++;
				break;
			}
			/*???? I think I only care about 'l' and 'L' */
			/* and either F or N depending on memory model */
			switch (*format)
			{
			case 'd': case 'i': case 'o': case 'u': case 'x':
			case 'X': case 'c':
				tbl[orgpos].type = (islonger ? Z_LONG : Z_INT);
				tbl[orgpos].newPos = j;
				tbl[orgpos].oldPos = orgpos;
				orgpos++;
				break;
			case 'f': case 'e': case 'g': case 'E': case 'G':
				tbl[orgpos].type = Z_DOUBLE;
				tbl[orgpos].newPos = j;
				tbl[orgpos].oldPos = orgpos;
				orgpos++;
				break;
			case 's': case 'n': case 'p':
				tbl[orgpos].type = Z_CHAR_STAR;
				tbl[orgpos].newPos = j;
				tbl[orgpos].oldPos = orgpos;
				orgpos++;
				break;
			case '%':
			default:
				break;
			}
		}
		newFormat[i++] = *format++;
	}
	newFormat[i] = '\0';
#if defined(__ZTC__) && (__ZTC__ >= 0x0310)
	extern void __CLIB qsort(void *,size_t,size_t, int (__CLIB *)(const void *,const void *));
#endif
	qsort(tbl, orgpos, sizeof(tbl[0]), rdatacmp);
#if defined(TESTING)
	for (i=1; i < orgpos; i++)
		if (tbl[i].newPos == tbl[i-1].newPos-1)
			abort();	/*raise(badformat)*/
#endif
	va_list toVlist;
#if defined(__WATCOMC__)
	memmove(toRet, &newArgs, sizeof(newArgs));
	memmove(&toVlist, &newArgs, sizeof(toVlist));
//	memmove(&toVlist, toRet, sizeof(toVlist));
#else
	toVlist = *(va_list *)toRet = (va_list)newArgs;

	// This handles those machines (like the PA-RISC) that runs varargs
	// from high to low memory rather than from low to high memory.
	(void) va_arg(toVlist, int);
	if (toVlist < *(va_list *)toRet)
	{
		*(va_list *)toRet = (va_list)((char *)newArgs + UII_MAXPARAM);
	}
	toVlist = *(va_list *)toRet;
#endif
	for (i=0; i < orgpos; i++)
	{
#if defined(__WATCOMC__)
		va_list fromVlist;
//		memmove(&fromVlist, toRet, sizeof(fromVlist));
		memmove(&fromVlist, args, sizeof(fromVlist));
#else
		va_list fromVlist = (va_list)args;
#endif
		for (j=0; j < orgpos && tbl[j].oldPos != i; j++)		// BUG.internal
		{
			switch (tbl[j].type)
			{
			case Z_INT:
				(void) va_arg(fromVlist, int);
				break;
			case Z_LONG:
				(void) va_arg(fromVlist, long);
				break;
			case Z_DOUBLE:
				(void) va_arg(fromVlist, double);
				break;
			case Z_CHAR_STAR:
				(void) va_arg(fromVlist, char *);
				break;
			};
		}
		switch (tbl[j].type)
		{
		case Z_INT:
			va_arg(toVlist, int) = va_arg(fromVlist, int);
			break;
		case Z_LONG:
			va_arg(toVlist, long) = va_arg(fromVlist, long);
			break;
		case Z_DOUBLE:
#if defined(ZIL_MSDOS)
			memcpy(toVlist, fromVlist, sizeof(double));
			(void)va_arg(toVlist, double);
			(void)va_arg(fromVlist, double);
#else
			va_arg(toVlist, double) = va_arg(fromVlist, double);
#endif
			break;
		case Z_CHAR_STAR:
			{
				long ptr = (long) va_arg(fromVlist, char *);
				va_arg(toVlist, char *) = (char *)ptr;
			}
//			va_arg(toVlist, char *) = va_arg(fromVlist, char *);

			break;
		};
	}
#if defined(ZIL_DELETE)
	delete []tbl;
#else
	delete [UII_MAXPARAM]tbl;
#endif
}

#if 0
int UI_INTERNATIONAL::fprintf(FILE *f, char *format, ...)
{
	int i;
	va_list args;
	char *newFormat = new char[UII_MAXPARAM];
	char newArgs[UII_MAXPARAM];
	va_list buff;

	va_start(args, format);
	rearrangeArgs(format, args, newFormat, newArgs, &buff);
	i = ::vfprintf(f, newFormat, buff);
	va_end(args);
	delete newFormat;
	return i;
}

int UI_INTERNATIONAL::printf(char *format, ...)
{
	int i;
	va_list args;
	char *newFormat = new char[UII_MAXPARAM];
	char newArgs[UII_MAXPARAM];
	va_list buff;

	va_start(args, format);
	rearrangeArgs(format, args, newFormat, newArgs, &buff);
	i = ::vprintf(newFormat, buff);
	va_end(args);
	delete newFormat;
	return i;
}
#endif

int UI_INTERNATIONAL::sprintf(char *dst, const char *format, ...)
{
	int i;
	va_list args;
	char *newFormat = new char[UII_MAXPARAM];
	char newArgs[UII_MAXPARAM];	// Borland expects this in the stack.
	va_list buff;

	va_start(args, format);
	rearrangeArgs(format, args, newFormat, newArgs, &buff);
	i = ::vsprintf(dst, newFormat, buff);
	va_end(args);
	delete newFormat;
	return i;
}

int UI_INTERNATIONAL::vsprintf(char *dst, const char *format, void *args)
{
	int i;
	char *newFormat = new char[UII_MAXPARAM];
	char newArgs[UII_MAXPARAM];	// Borland expects this in the stack.
	va_list buff;

	rearrangeArgs(format, args, newFormat, newArgs, &buff);
	i = ::vsprintf(dst, newFormat, buff);
	delete newFormat;
	return i;
}

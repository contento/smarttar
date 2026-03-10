//	Zinc Interface Library - D_INTL.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include <stdio.h>
#include <stdarg.h>
#include <locale.h>
#include <limits.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "ui_gen.hpp"
#include <dos.h>
#if defined(_MSC_VER)
#	if defined(DOSX286)
#		define	_int86x	int86x
#		define	_REGS	REGS
#		define	_SREGS	SREGS
#	endif
#	pragma hdrstop			// Microsoft pre-compiled header pragma.
#elif defined(__WATCOMC__)
#	include <i86.h>
#endif

#if defined(__ZTC__) && defined(DOS386)						// BUG.1721
//	Zortech 32 bit mode doesn't support int 21h, function 38h right.
//	Remove these lines when it works.
#	define ZIL_LOCALE
#	define DISABLE_INT21
#endif

extern struct {
	int code;
	char *language;
	char *country;
} codeMap[];

struct DOS2_COUNTRY
{
	short dateFormat;
	char currencySymbol[2];
	char thousandsSeparator[2];
	char decimalSeparator[2];
	char filler[24];
};

struct DOS3_COUNTRY
{
	short dateFormat;
	char currencySymbol[5];
	char thousandsSeparator[2];
	char decimalSeparator[2];
	char dateSeparator[2];
	char timeSeparator[2];
	unsigned char currencyStyle;
	unsigned char significantDigits;
	unsigned char timeFormat;
	long caseMap;
	char dataSeparator[2];
	char filler[10];
};

static void ui_copy(char *dst, char *src)
{
	if (*src)
		::strcpy(dst, src);
}

void UI_INTERNATIONAL::Initialize(int force)
{
	// Make sure the country information has not already been initialized.
	if (initialized && !force)
		return;
	initialized = TRUE;

#if defined (ZIL_LOCALE)
	char *tmp = getenv("LOCALE");
	if (tmp)
		strcpy(localeName, tmp);
	tmp = setlocale(LC_ALL, localeName);

	// Get the locale information.  (Probably brain-damaged.)
	struct lconv *conv = localeconv();
	ui_copy(decimalSeparator, conv->decimal_point);
	ui_copy(monDecimalSeparator, conv->mon_decimal_point);
	ui_copy(thousandsSeparator, conv->thousands_sep);
	ui_copy(monThousandsSeparator, conv->mon_thousands_sep);
	ui_copy(intCurrencySymbol, conv->int_curr_symbol);
	if (conv->p_cs_precedes != CHAR_MAX)
		posCurrencyPrecedes = conv->p_cs_precedes;
	if (conv->frac_digits != CHAR_MAX)
		fractionDigits = conv->frac_digits;
	if (conv->p_sep_by_space != CHAR_MAX)
		posSpaceSeparation = conv->p_sep_by_space;
	ui_copy(negativeSign, conv->negative_sign);
	if (conv->n_sign_posn != CHAR_MAX)
		negSignPrecedes = conv->n_sign_posn;
	if (conv->n_sep_by_space != CHAR_MAX)
		negSpaceSeparation = conv->n_sep_by_space;

#if defined(__ZTC__) && __ZTC__ < 0x310
	if (conv->n_sc_precedes != CHAR_MAX)
		negCurrencyPrecedes = conv->n_sc_precedes;
#else	// !ZTC 3.1
	ui_copy(grouping, conv->grouping);
	ui_copy(monGrouping, conv->mon_grouping);
	if (conv->n_cs_precedes != CHAR_MAX)
		negCurrencyPrecedes = conv->n_cs_precedes;
	if (conv->int_frac_digits != CHAR_MAX)
		intFractionDigits = conv->int_frac_digits;
	ui_copy(positiveSign, conv->positive_sign);
	if (conv->p_sign_posn != CHAR_MAX)
		posSignPrecedes = conv->p_sign_posn;
#endif	// !ZTC 3.1

#elif !defined(DISABLE_INT21)
	// Override it with the DOS country information
	static DOS2_COUNTRY dos2Info;
	static DOS3_COUNTRY dos3Info;

#if defined(_MSC_VER)
	union _REGS regs;
	struct _SREGS segregs;

	// Get the proper operating system version.
	void far* ptr = (_osmajor == 2 ? (void *)&dos2Info:(void *)&dos3Info);
	regs.x.dx = _FP_OFF(ptr);
	segregs.ds = _FP_SEG(ptr);
	segregs.es = 0;

	// Determine the country code.
	regs.x.ax = 0x3800;
	_int86x(0x21, &regs, &regs, &segregs);
	countryCode = (regs.h.al == 0xFF) ? regs.x.bx : regs.h.al;
#elif defined(__WATCOMC__)
	union REGS regs;
	extern unsigned char _osmajor;
	// Get the proper operating system version.
	regs.x.edx = (_osmajor == 2) ? (unsigned int)&dos2Info : (unsigned int)&dos3Info;
	// Determine the country code.
	regs.w.ax = 0x3800;
	int386(0x21, &regs, &regs);
	countryCode = (regs.h.al == 0xFF) ? regs.w.bx : regs.h.al;
#else	// WATCOM || MSC
	union REGS regs;
	extern unsigned char _osmajor;
	// Get the proper operating system version.
	regs.x.dx = (_osmajor == 2 ? (USHORT)&dos2Info : (USHORT)&dos3Info);
	// Determine the country code.
	regs.x.ax = 0x3800;
	int86(0x21, &regs, &regs);
	countryCode = (regs.h.al == 0xFF) ? regs.x.bx : regs.h.al;
#endif	// WATCOM || MSC

	for (int i = 0; codeMap[i].code && countryCode != codeMap[i].code; i++)
		;
	if (codeMap[i].code)
	{
		ui_copy(languageName, codeMap[i].language);
		ui_copy(localeName, codeMap[i].country);
	}

	if (_osmajor == 2)
	{
		dateFormat = dos2Info.dateFormat;
		::strcpy(currencySymbol, dos2Info.currencySymbol);
		::strcpy(intCurrencySymbol, dos2Info.currencySymbol);
		::strcpy(thousandsSeparator, dos2Info.thousandsSeparator);
		::strcpy(monThousandsSeparator, dos2Info.thousandsSeparator);
		::strcpy(decimalSeparator, dos2Info.decimalSeparator);
		::strcpy(monDecimalSeparator, dos2Info.decimalSeparator);
	}
	else
	{
		dateFormat = dos3Info.dateFormat;
		::strcpy(currencySymbol, dos3Info.currencySymbol);
		::strcpy(intCurrencySymbol, dos3Info.currencySymbol);
		::strcpy(thousandsSeparator, dos3Info.thousandsSeparator);
		::strcpy(monThousandsSeparator, dos3Info.thousandsSeparator);
		::strcpy(decimalSeparator, dos3Info.decimalSeparator);
		::strcpy(monDecimalSeparator, dos3Info.decimalSeparator);
		::strcpy(dateSeparator, dos3Info.dateSeparator);
		::strcpy(timeSeparator, dos3Info.timeSeparator);
		posSignPrecedes = negSignPrecedes = 1;

		static struct {
			int spce;
			int curr;
		} cstable[] = {
			{ 0, 1 },
			{ 0, 0 },
			{ 1, 1 },
			{ 1, 0 },
			{ 0, 1 }
		};
		if (dos3Info.currencyStyle <= 4) {
			posSpaceSeparation = negSpaceSeparation =
				cstable[dos3Info.currencyStyle].spce;
			posCurrencyPrecedes = negCurrencyPrecedes =
				cstable[dos3Info.currencyStyle].curr;
			if (dos3Info.currencyStyle == 4) {
				::strcpy(monDecimalSeparator, currencySymbol);
				currencySymbol[0] = 0;
			}
		}
		intFractionDigits = fractionDigits =
			dos3Info.significantDigits;
		timeFormat = dos3Info.timeFormat;
	}
#endif	// !DISABLE_INT21
}

//	Zinc Interface Library - W_INTL.CPP
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
#if defined(DOSX286)
#define	_int86x	int86x
#define	_REGS	REGS
#define	_SREGS	SREGS
#endif
#pragma hdrstop			// Microsoft pre-compiled header pragma.
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

#if defined(ZIL_LOCALE)
	// Get the locale information.  (Probably brain-damaged.)
	struct lconv *conv = localeconv();
	ui_copy(intCurrencySymbol, conv->int_curr_symbol);
	if (conv->p_cs_precedes != CHAR_MAX)
		posCurrencyPrecedes = conv->p_cs_precedes;
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
#else
	ui_copy(grouping, conv->grouping);
	ui_copy(monGrouping, conv->mon_grouping);
	if (conv->n_cs_precedes != CHAR_MAX)
		negCurrencyPrecedes = conv->n_cs_precedes;
	if (conv->int_frac_digits != CHAR_MAX)
		intFractionDigits = conv->int_frac_digits;
	ui_copy(positiveSign, conv->positive_sign);
	if (conv->p_sign_posn != CHAR_MAX)
		posSignPrecedes = conv->p_sign_posn;
#endif

#else
#	if !defined(DISABLE_INT21)
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
#else
	union REGS regs;
	extern unsigned char _osmajor;
	// Get the proper operating system version.
	regs.x.dx = (_osmajor == 2 ? (USHORT)&dos2Info : (USHORT)&dos3Info);

	// Determine the country code.
	regs.x.ax = 0x3800;
	int86(0x21, &regs, &regs);
#endif

	countryCode = (regs.h.al == 0xFF) ? regs.x.bx : regs.h.al;
	countryCode = GetProfileInt("intl", "iCountry", countryCode);			// BUG.2017

	for (int i = 0; codeMap[i].code && countryCode != codeMap[i].code; i++)
		;
	if (codeMap[i].code)
	{
		ui_copy(languageName, codeMap[i].language);
		ui_copy(localeName, codeMap[i].country);
	}

	if (_osmajor == 2)
		ui_copy(intCurrencySymbol, dos2Info.currencySymbol);
	else
	{
		::strcpy(intCurrencySymbol, dos3Info.currencySymbol);
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
	}
#	endif

	char *intlString = "Intl";

	GetProfileString(intlString, "sCountry", localeName, 
			 localeName, sizeof(localeName));
	GetProfileString(intlString, "sLanguage", languageName, 
			 languageName, sizeof(languageName));
	GetProfileString(intlString, "sDecimal", decimalSeparator,
			 decimalSeparator, sizeof(decimalSeparator));
	GetProfileString(intlString, "sDecimal", monDecimalSeparator,
			 monDecimalSeparator, sizeof(monDecimalSeparator));
	GetProfileString(intlString, "sThousand", thousandsSeparator,
			 thousandsSeparator, sizeof(thousandsSeparator));
	GetProfileString(intlString, "sThousand", monThousandsSeparator,
			 monThousandsSeparator, sizeof(monThousandsSeparator));
	GetProfileString(intlString, "sCurrency", currencySymbol,
			 currencySymbol, sizeof(currencySymbol));
	// ???? intCurrencySymbol
	WORD tmpval;
	static struct {
		int spce;
		int curr;
	} pcstable[] = {
		{ 0, TRUE },
		{ 0, FALSE },
		{ 1, TRUE },
		{ 1, FALSE }
	};
	tmpval = GetProfileInt(intlString, "iCurrency", -1);
	if (tmpval < 4) {
		posSpaceSeparation = pcstable[tmpval].spce;
		posCurrencyPrecedes = pcstable[tmpval].curr;
	}
	// ???? posSignPrecedes = 1;
	// ???? positiveSign
	// ???? negativeSign
	static struct {
		int spce;
		int curr;
		int sign;
	} ncstable[] = {
		{ 0, 1, 0 },	// 0
		{ 0, 1, 1 },	// 1
		{ 0, 1, 3 },	// 2
		{ 0, 1, 2 },	// 3
		{ 0, 0, 0 },	// 4
		{ 0, 0, 1 },	// 5
		{ 0, 0, 4 },	// 6
		{ 0, 0, 2 },	// 7
		{ 1, 0, 1 },	// 8
		{ 1, 1, 1 },	// 9
		{ 1, 0, 2 }		// 10
	};
	tmpval = GetProfileInt(intlString, "iNegCurr", -1);
	if (tmpval < 11) {
		negSpaceSeparation = ncstable[tmpval].spce;
		negCurrencyPrecedes = ncstable[tmpval].curr;
		negSignPrecedes = ncstable[tmpval].sign;
	}
	fractionDigits =
		GetProfileInt(intlString, "iCurrDigits", fractionDigits);
	// ???? intFractionDigits
	intFractionDigits = fractionDigits;
	// iDate and sDate should be replaced with sShortDate
	dateFormat = GetProfileInt(intlString, "iDate", dateFormat);
	GetProfileString(intlString, "sDate", dateSeparator, dateSeparator,
			 sizeof(dateSeparator));
	timeFormat = GetProfileInt(intlString, "iTime", timeFormat);
	GetProfileString(intlString, "sTime", timeSeparator, timeSeparator,
			 sizeof(timeSeparator));
#endif
}

//	Zinc Interface Library - Z_BNUM.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include <limits.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#if defined(__BCPLUSPLUS__) | defined(__TCPLUSPLUS__)
#include <mem.h>
#endif
#include "ui_gen.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

#define STACKSIZE	10

// WARNING: This must change if DIGITS changes
#if (DIGITS == 8)
	// WHOLE_WORD is 10^DIGITS
#	define WHOLE_WORD	100000000U
	// MOD_WORD is sqrt(WHOLE_WORD)
#	define MOD_WORD	(10000U)
#elif (DIGITS == 4)
	// WHOLE_WORD is 10^DIGITS
#	define WHOLE_WORD	10000U
	// MOD_WORD is sqrt(WHOLE_WORD)
#	define MOD_WORD	(100U)
#endif

#define MAXSTORED	(WHOLE_WORD-1)
#define LENGTHOF(x)	(sizeof(x)/sizeof((x)[0]))
#define INCSTACK(s, e)	stackpos++; if (stackpos >= STACKSIZE) ui_numerror(s, e)
#define TRUE		1
#define FALSE		0

// TO DO
// div
// mod
// sqrt

// DONE
// add		*
// sub		*
// neg		-
// abs		-
// zero		-
// ceil		-
// floor	-
// truncate	-
// round	-
// >		*
// >=		*
// <		*
// <=		*
// ==		*
// !=		*
// mul		*
// ascii to ui_num
// ui_num to ascii
// underflow/overflow exceptions

static int stackpos = -1;
static UI_BIGNUM stack[STACKSIZE];

static nm_number poten[DIGITS+1] = {
	1,		// 0
#if (DIGITS > 0)
	10,		// 1
#endif
#if (DIGITS > 1)
	100,		// 2
#endif
#if (DIGITS > 2)
	1000,		// 3
#endif
#if (DIGITS > 3)
	10000,		// 4
#endif
#if (DIGITS > 4)
	100000,		// 5
#endif
#if (DIGITS > 5)
	1000000,	// 6
#endif
#if (DIGITS > 6)
	10000000,	// 7
#endif
#if (DIGITS > 7)
	100000000,	// 8
#endif
#if (DIGITS > 8)
	1000000000,	// 9
#endif
#if (DIGITS > 9)
	10000000000,	// 10
#endif
};

//--------------------------------------------------------------------------
// Helper routines
int UI_BIGNUM::do_add_loop(const nm_number *aw, int carry)
{
	int i;
	nm_number *r;

	if (aw) aw += LENGTHOF(num)-1;
	r = &num[LENGTHOF(num)-1];
	for (i=0; i < LENGTHOF(num); i++)
	{
		*r += carry;
		if (aw) *r += *aw--;
		carry = (*r > MAXSTORED);
		if (carry) *r -= MAXSTORED+1;
		r--;
	}
	return carry;
}

void UI_BIGNUM::nines_complement(void)
{
	int i;
	nm_number *r;

	r = num;
	for (i=0; i < LENGTHOF(num); i++, r++)
		*r = MAXSTORED - *r;
}

int UI_BIGNUM::add_same_sign(const UI_BIGNUM &b, UI_BIGNUM &result)
{
	memcpy(&result, this, sizeof(UI_BIGNUM));
	return result.do_add_loop(b.num, 0);
}

void UI_BIGNUM::zero(void)
{
	int i;
	sign = 0;
	int j = LENGTHOF(num);
	for (i=0; i < j; i++)
		num[i] = 0;
}

int UI_BIGNUM::add_diff_sign(const UI_BIGNUM &b, UI_BIGNUM &result)
{
	int i;
	const nm_number *aw;
	nm_number tmp = 0;

	for (i=0; i < LENGTHOF(num); i++)
		if ((tmp = num[i] - b.num[i]) != 0) break;
	if (i >= LENGTHOF(num))
	{
		result.zero();
		return 0;
	}
	if (tmp > 0)
	{
		memcpy(&result, &b, sizeof(UI_BIGNUM));
		aw = num;
		result.sign = sign;
	}
	else
	{
		memcpy(&result, this, sizeof(UI_BIGNUM));
		aw = b.num;
		result.sign = b.sign;
	}
	// set 9's compliment
	result.nines_complement();
	i = result.do_add_loop(aw, 1);
//???? Not done
	if (i == 0)
	{
		result.nines_complement();
		i = result.do_add_loop(NULL, 1);
		result.sign = 1-result.sign;
		i = 1-i;
	}
	return 1-i;
}

static void ui_numerror(char *s, int error)
{
#if defined(ZIL_MSDOS) || defined(ZIL_OS2) || defined(ZIL_MOTIF)
	errno = error;
	perror(s);
	abort();
#endif
}

UI_BIGNUM &UI_BIGNUM::floor_ceil(int nsign) const
{
	int i, j, k;
	nm_number tmp;
	nm_number n[LENGTHOF(num)];

	INCSTACK("UI_BIGNUM::floor/ceil", ENOMEM);
	memcpy(&stack[stackpos], this, sizeof(UI_BIGNUM));
	j = LENGTHOF(num) - (NUMBER_DECIMAL+DIGITS) / DIGITS;
	k = NUMBER_DECIMAL % DIGITS;
	tmp = stack[stackpos].num[j] % poten[k];
	stack[stackpos].num[j] -= tmp;
	if (k == 0)
		tmp = (j+1 < LENGTHOF(num) ? stack[stackpos].num[j+1] : 0);
	for (i=j+1; i < LENGTHOF(num); i++)
		stack[stackpos].num[i] = 0;
	if ((stack[stackpos].sign == nsign) && tmp != 0)
	{
		for (i=0; i < LENGTHOF(n); i++)
			n[i] = 0;
		n[j] = poten[k];
		(void) stack[stackpos].do_add_loop(n, 0);
	}
	return stack[stackpos];
}

int UI_BIGNUM::compare(const UI_BIGNUM &b, int gt, int ls, int eq)
{
	int i;

	if (sign < b.sign)
		return gt;
	if (sign > b.sign)
		return ls;
	for (i=0; i < LENGTHOF(num); i++)
	{
		if (num[i] > b.num[i])
			return (sign ? ls : gt);
		if (num[i] < b.num[i])
			return (sign ? gt : ls);
	}
	return eq;
}

void UI_BIGNUM::checkZero(void)
{
	// Don't allow "-0"
	for (int i=0; i < LENGTHOF(num); i++)
		if (num[i] != 0)
			return;
	sign = 0;
}	

//--------------------------------------------------------------------------
// Regular UI_BIGNUM op UI_BIGNUM operations
UI_BIGNUM &UI_BIGNUM::operator=(const UI_BIGNUM &number)
{
	memcpy(this, &number, sizeof(*this));
	// set stack to empty
	stackpos = -1;
	return *this;
}

UI_BIGNUM &UI_BIGNUM::operator+(const UI_BIGNUM &number)
{
	INCSTACK("UI_BIGNUM::+", ENOMEM);
	if (sign == number.sign)
	{
		if (add_same_sign(number, stack[stackpos]))
			ui_numerror("UI_BIGNUM::+", ERANGE);
	}
	else
	{
		if (add_diff_sign(number, stack[stackpos]))
			ui_numerror("UI_BIGNUM::+", ERANGE);
	}
	return stack[stackpos];
}

UI_BIGNUM &UI_BIGNUM::operator-(const UI_BIGNUM &number)
{
	INCSTACK("UI_BIGNUM::-", ENOMEM);
	if (sign != number.sign)
	{
		if (add_same_sign(number, stack[stackpos]))
			ui_numerror("UI_BIGNUM::-", ERANGE);
	}
	else
	{
		if (add_diff_sign(number, stack[stackpos]))
			ui_numerror("UI_BIGNUM::-", ERANGE);
	}
	return stack[stackpos];
}

UI_BIGNUM &UI_BIGNUM::operator++(void)
{
	return *this += UI_BIGNUM(1L);
}

UI_BIGNUM &UI_BIGNUM::operator--(void)
{
	return *this -= UI_BIGNUM(1L);
}

UI_BIGNUM &abs(const UI_BIGNUM &number)
{
	INCSTACK("UI_BIGNUM::abs", ENOMEM);
	memcpy(&stack[stackpos], &number, sizeof(UI_BIGNUM));
	stack[stackpos].sign = 0;
	return stack[stackpos];
}

UI_BIGNUM &truncate(const UI_BIGNUM &number, int places)
{
	int i, j;

	INCSTACK("UI_BIGNUM::truncate", ENOMEM);
	memcpy(&stack[stackpos], &number, sizeof(UI_BIGNUM));
	places = NUMBER_DECIMAL - places;
	if (places <= 0)
		return stack[stackpos];
	if (places > NUMBER_DECIMAL + NUMBER_WHOLE)
	{
		stack[stackpos].zero();
		return stack[stackpos];
	}
	j = LENGTHOF(number.num) - (places+DIGITS) / DIGITS;
	for (i=j+1; i < LENGTHOF(number.num); i++)
		stack[stackpos].num[i] = 0;
#if defined(__ZTC__) && defined(DOS386)
// This seems to prevent the optimizer from not compiling the other line right.
// I wish I knew another way around this (without spending another day or two).
	int tmp = poten[places % DIGITS];
	stack[stackpos].num[j] -= (stack[stackpos].num[j] % tmp);
#else
	stack[stackpos].num[j] -= (stack[stackpos].num[j] % poten[places % DIGITS]);
#endif
	stack[stackpos].checkZero();
	return stack[stackpos];
}

UI_BIGNUM &round(const UI_BIGNUM &number, int places)
{
	int i, j, k;
	nm_number tmp;
	nm_number n[LENGTHOF(number.num)];

	INCSTACK("UI_BIGNUM::round", ENOMEM);
	memcpy(&stack[stackpos], &number, sizeof(UI_BIGNUM));
	places = NUMBER_DECIMAL - places;
	if (places <= 0)
		return stack[stackpos];
	if (places > NUMBER_DECIMAL + NUMBER_WHOLE)
	{
		stack[stackpos].zero();
		return stack[stackpos];
	}
	j = LENGTHOF(number.num) - (places+DIGITS) / DIGITS;
	k = (places+DIGITS) % DIGITS;
	if (k == 0)
	{
		tmp = stack[stackpos].num[j+1] % poten[DIGITS];
		k = DIGITS;
	}
	else
	{
		tmp = stack[stackpos].num[j] % poten[k];
		stack[stackpos].num[j] -= tmp;
	}
	for (i=j+1; i < LENGTHOF(number.num); i++)
		stack[stackpos].num[i] = 0;
	if (tmp >= poten[k] / 2)
	{
		for (i=0; i < LENGTHOF(n); i++)
			n[i] = 0;
		n[j] = (k == DIGITS ? poten[0] : poten[k]);
		(void) stack[stackpos].do_add_loop(n, 0);
	}
	stack[stackpos].checkZero();
	return stack[stackpos];
}

// Support for mixed mode (number op int) operations
void UI_BIGNUM::itob(ibignum value)
{
	zero();
	if (value < 0)
	{
		sign = 1;
		value = -value;
	}
	nm_number *numbase = &num[LENGTHOF(num)-1-(NUMBER_DECIMAL/DIGITS)];
#if DIGITS == 4
#if (NUMBER_DECIMAL % DIGITS == 0)
	numbase[0]  = (nm_number)(value % WHOLE_WORD);
	numbase[-1] = (nm_number)(value / WHOLE_WORD);
#else
	ibignum potbase = poten[NUMBER_DECIMAL%DIGITS];
	ibignum potpowr = poten[DIGITS-NUMBER_DECIMAL%DIGITS];
	numbase[0]  = (value % potpowr) * potbase;
	numbase[-1] = (value / potpowr) % WHOLE_WORD;
	numbase[-2] = (value / potpowr) / WHOLE_WORD;
#endif
#endif
#if DIGITS == 8
#if (NUMBER_DECIMAL % DIGITS == 0)
	numbase[0] = (unsigned)value;
#else
	ibignum potbase = poten[NUMBER_DECIMAL%DIGITS];
	ibignum potpowr = poten[DIGITS-NUMBER_DECIMAL%DIGITS];
	numbase[0]  = (value % potpowr) * potbase;
	numbase[-1] = (value / potpowr);
#endif
#endif
}

//--------------------------------------------------------------------------
// Conversion routines
static char *copy(char *to, const char *from)
{
	if (! *from) return to;
	const char *_from = from + strlen(from);

	do {
		*--to = *--_from;
	} while (_from != from);
	return to;
}

void UI_BIGNUM::Export(char *string, NMF_FLAGS nmFlags)
{
	int i, dgt, j, comma;
	nm_number tmp = 0;
	char *t;
	char *ps1, *pc, *ps, *ps2;
	char *ss2, *ss, *sc, *ss1;
	char *pt, *cm;
	char *gp; int oldgp;

	int digits = NMF_DIGITS(nmFlags);
	// bigger than the output string???
	char *buff = new char[2*(NUMBER_WHOLE+digits)];
	char *cpos = &buff[2*(NUMBER_WHOLE+digits)-1];

	UI_INTERNATIONAL::Initialize();
	ps1 = pc = ps = ps2 = ss1 = sc = ss = ss2 = pt = cm = "";
	if (FlagSet(nmFlags, NMF_CURRENCY))
	{
		pt = UI_INTERNATIONAL::monDecimalSeparator;
		cm = UI_INTERNATIONAL::monThousandsSeparator;
		gp = UI_INTERNATIONAL::monGrouping;
		if (sign)
		{
			if (UI_INTERNATIONAL::negCurrencyPrecedes)
			{
				pc = UI_INTERNATIONAL::currencySymbol;
				if (UI_INTERNATIONAL::negSpaceSeparation)
					ps = " ";
			}
			else
			{
				sc = UI_INTERNATIONAL::currencySymbol;
				if (UI_INTERNATIONAL::negSpaceSeparation)
					ss = " ";
			}
		}
		else
		{
			if (UI_INTERNATIONAL::posCurrencyPrecedes)
			{
				pc = UI_INTERNATIONAL::currencySymbol;
				if (UI_INTERNATIONAL::posSpaceSeparation)
					ps = " ";
			}
			else
			{
				sc = UI_INTERNATIONAL::currencySymbol;
				if (UI_INTERNATIONAL::posSpaceSeparation)
					ss = " ";
			}
		}
		if (digits < 0)
			digits = UI_INTERNATIONAL::fractionDigits;
	}
	else
	{
		pt = UI_INTERNATIONAL::decimalSeparator;
		cm = UI_INTERNATIONAL::thousandsSeparator;
		gp = UI_INTERNATIONAL::grouping;
		if (digits < 0)
			digits = NUMBER_DECIMAL;
	}
	if (sign)
	{
		i = UI_INTERNATIONAL::negSignPrecedes;
		t = UI_INTERNATIONAL::negativeSign;
		if (FlagSet(nmFlags, NMF_CREDIT))
			i = 0;
	}
	else
	{
		i = UI_INTERNATIONAL::posSignPrecedes;
		t = UI_INTERNATIONAL::positiveSign;
	}
	switch (i)
	{
		case 0: ps1 = "("; ss1 = ")"; break;
		case 1: ps1 = t; break;
		case 2: ss1 = t; break;
		case 3: ps2 = t; break;
		case 4: ss2 = t; break;
		default:
			ui_numerror("UI_BIGNUM::Export", EINVAL);
	}
	*cpos = '\0';
	cpos = copy(cpos, ss1);
	if (FlagSet(nmFlags, NMF_PERCENT))
		*--cpos = '%';
	cpos = copy(cpos, sc);
	cpos = copy(cpos, ss);
	cpos = copy(cpos, ss2);

	t = cpos;		// to see if anything is added
	while (digits > NUMBER_DECIMAL)
	{
		*--cpos = '0';
		digits--;
	}
	dgt = NUMBER_DECIMAL;
	i = LENGTHOF(num);
	while (dgt > digits)
	{
		if (dgt % DIGITS == 0)
		{
			i--;
			tmp = num[i];
		}
		tmp /= 10;
		dgt--;
	}
	while (dgt > 0)
	{
		if (dgt % DIGITS == 0)
		{
			i--;
			tmp = num[i];
		}
		*--cpos = (char)(tmp % 10) + '0';
		tmp /= 10;
		dgt--;
	}
	if (cpos != t)
		cpos = copy(cpos, pt);

	t = cpos;		// to see if anything is added
	j = 0;
	while (j < LENGTHOF(num) && num[j] == 0) j++;
	oldgp = (unsigned)*gp;
	if (oldgp == 0 || !FlagSet(nmFlags, NMF_COMMAS))
		oldgp = CHAR_MAX;
	else
		 gp++;
	comma = 0;
	while (i >= j)
	{
		if (dgt % DIGITS == 0)
		{
			i--;
			tmp = num[i];
		}
		*--cpos = (char)(tmp % 10) + '0';
		tmp /= 10;
	if (i == j && tmp == 0) break;
		comma++;
		if (oldgp != CHAR_MAX && comma == oldgp)
		{
			cpos = copy(cpos, cm);
			comma = 0;
			if (*gp)
				oldgp = (unsigned)*gp++;
		}
		dgt--;
	}
	if (cpos == t)
		*--cpos = '0';	// need a leading zero
	cpos = copy(cpos, ps2);
	cpos = copy(cpos, ps);
	cpos = copy(cpos, pc);
	cpos = copy(cpos, ps1);
	(void) strcpy(string, cpos);
#if defined(ZIL_DELETE)
	delete []buff;
#else
	digits = NMF_DIGITS(nmFlags);
	delete [2*(NUMBER_WHOLE+digits)]buff;
#endif
}

void UI_BIGNUM::Export(ibignum *value)
{
	ibignum l;

	nm_number *numbase = &num[LENGTHOF(num)-1-(NUMBER_DECIMAL/DIGITS)];
#if DIGITS == 4
#if (NUMBER_DECIMAL % DIGITS == 0)
	l = numbase[0] + (long)numbase[-1] * WHOLE_WORD;
#else
	ibignum potbase = poten[NUMBER_DECIMAL%DIGITS];
	ibignum potpowr = poten[DIGITS-NUMBER_DECIMAL%DIGITS];
	l = (numbase[0] / potbase) + ((long)numbase[-1] * potpowr) + 
		(long)(numbase[-2] % potbase) * potpowr * WHOLE_WORD;
#endif
#endif
#if DIGITS == 8
#if (NUMBER_DECIMAL % DIGITS == 0)
	l = numbase[0];
#else
	ibignum potbase = poten[NUMBER_DECIMAL%DIGITS];
	ibignum potpowr = poten[DIGITS-NUMBER_DECIMAL%DIGITS];
	l = (numbase[0] / potbase) + (numbase[-1] % potbase) * potpowr;
#endif
#endif
	*value = sign ? -l : l;
}

NMI_RESULT UI_BIGNUM::Import(const UI_BIGNUM &number)
{
	sign = number.sign;
	memcpy(num, number.num, sizeof(num));
	return NMI_OK;
}

NMI_RESULT UI_BIGNUM::Import(ibignum value)
{
	this->itob(value);
	return NMI_OK;
}

NMI_RESULT UI_BIGNUM::Import(const char *string, const char *decimalstr, const char *signstr)
{
	int i, j, decimal;
	const char *tmp;
	const char *monstr;

	zero();
	UI_INTERNATIONAL::Initialize();
	if (decimalstr == 0)
	{
		decimalstr = UI_INTERNATIONAL::decimalSeparator;
		monstr = UI_INTERNATIONAL::monDecimalSeparator;
	}
	else
		monstr = decimalstr;
	if (signstr == 0)
		signstr = UI_INTERNATIONAL::negativeSign;
	tmp = string;
	sign = (*string == '(');
	while (*tmp && !sign)
	{
		if (strncmp(tmp, signstr, ui_strlen(signstr)) == 0)
			sign = 1;
		tmp++;
	}
	tmp = string;
	decimal = 0;
	while (*tmp && strncmp(tmp, monstr, ui_strlen(monstr)) != 0 &&
	       strncmp(tmp, decimalstr, ui_strlen(decimalstr)) != 0)
	{
		if (isascii(*tmp) && isdigit(*tmp))
			decimal++;
		tmp++;
	}
	if (decimal > NUMBER_WHOLE)
		return NMI_OUT_OF_RANGE;
		// ui_numerror("UI_BIGNUM::Import", ERANGE);
	i = LENGTHOF(num) - (decimal + NUMBER_DECIMAL + DIGITS-1) / DIGITS;
	j = (decimal + NUMBER_DECIMAL - 1) % DIGITS;
	while (*string && i < LENGTHOF(num))
	{
		if (isascii(*string) && isdigit(*string))
			num[i] += poten[j] * (*string - '0');
		else
			j++;
		string++;
		j--;
		if (j < 0)
		{
			j = DIGITS-1;
			i++;
		}
	}
	checkZero();
	return NMI_OK;
}

UI_BIGNUM &UI_BIGNUM::operator*(const UI_BIGNUM &number)
// If we have two 2-word bignum, A and B, we can break the multiplication
// problem down into the following:
//    A = K^3*A0 + K^2*A1 + K*A2 + A3 and
//    B = K^3*B0 + K^2*B1 + K*B2 + B3,
// where K is a half word modulus (sqrt(WHOLE_WORD)).  The solution for
// (A*B) is now:
//    K^6*(A0*B0) +
//    K^5*(A0*B1 + A1*B0) + 
//    K^4*(A0*B2 + A1*B1 + A2*B0) + 
//    K^3*(A0*B3 + A1*B2 + A2*B1 + A3*B0) +
//    K^2*(A1*B3 + A2*B2 + A3*B1) +
//    K^1*(A2*B3 + A3*B2) +
//    K^0*(A3*B3)
// We can also optimize this by checking for overflow before multiplying
// and not working on the truncated digits.
{
#define XTENDED	(2*LENGTHOF(num))
#define RESULT	(2*XTENDED)
	int i, j, carry;
	nm_number ax[XTENDED], bx[XTENDED], rx[RESULT], *ap, *rp;
	const nm_number *bp;

	stackpos++;
	if (stackpos >= STACKSIZE)
		ui_numerror("UI_BIGNUM::*", ENOMEM);
	memcpy(&stack[stackpos], this, sizeof(UI_BIGNUM));
	// split number, initialize arrays
	for (i=0, ap = &num[0], bp = &number.num[0];
	     i < LENGTHOF(num); i++, ap++, bp++)
	{
		ax[2*i+1] = *ap % MOD_WORD;
		ax[2*i+0] = *ap / MOD_WORD;
		bx[2*i+1] = *bp % MOD_WORD;
		bx[2*i+0] = *bp / MOD_WORD;
	}

	// init for summation
	for (i=0, rp = rx; i < LENGTHOF(rx); i++, rp++)
		*rp = 0;

	// cross multiply
	for (i=0, ap = ax; i < LENGTHOF(ax); i++, ap++)
		for (j=0, bp = bx; j < LENGTHOF(bx); j++, bp++)
			rx[i+j+1] += *ap * *bp;

	// combine terms (doing this first makes propogate-carry shorter)
	for (i=LENGTHOF(rx)-2; i > 0; i -= 2)
	{
		ax[i >> 1] = rx[i+1] + (rx[i] % MOD_WORD) * MOD_WORD;
		rx[i-1] += (rx[i] / MOD_WORD);
	}

	// propogate carry
	for (i=LENGTHOF(ax)-1, ap = ax + i; i > 0; i--, ap--)
		if (*ap >= WHOLE_WORD)
		{
			ap[-1] += *ap / WHOLE_WORD;
			*ap %= WHOLE_WORD;
		}
	if (ax[0] >= MOD_WORD)
		ui_numerror("UI_BIGNUM::*", ERANGE);

	// cut NUMBER_DECIMAL / DIGITS words out of result
	// cut NUMBER_DECIMAL % DIGITS digits out of that result
	// round the whole thing
#if (NUMBER_DECIMAL % DIGITS == 0)
	carry = ax[LENGTHOF(ax)-(NUMBER_DECIMAL/DIGITS)] % poten[DIGITS-1] >= 5;
#else
	carry = ax[LENGTHOF(ax)-(NUMBER_DECIMAL/DIGITS)] % poten[NUMBER_DECIMAL % DIGITS - 1] >= (poten[NUMBER_DECIMAL % DIGITS] >> 1);
#endif
	for (j=LENGTHOF(num)-1,
	     i=LENGTHOF(ax)-1-(NUMBER_DECIMAL/DIGITS), ap = ax + i;
	     j >= 0; j--, i--, ap--)
	{
		*ap /= poten[NUMBER_DECIMAL % DIGITS];
		*ap += (ap[1] % poten[NUMBER_DECIMAL % DIGITS]) * poten[DIGITS - NUMBER_DECIMAL % DIGITS] + carry;
		stack[stackpos].num[j] = *ap;
		carry = 0;
	}
	stack[stackpos].sign = (sign != number.sign);
	return stack[stackpos];
}



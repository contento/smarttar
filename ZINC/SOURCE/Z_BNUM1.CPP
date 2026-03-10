//	Zinc Interface Library - Z_BNUM1.HPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include <stdio.h>
#include "ui_gen.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

/*--------------------------------------------------------------------------*/
/* Support for mixed mode (UI_BIGNUM op double) operations */
void UI_BIGNUM::Export(rbignum *value)
{
	rbignum r;
	char tmp[NUMBER_WHOLE+NUMBER_DECIMAL+4];

	Export(tmp, NMF_DECIMAL(NUMBER_DECIMAL));
	UI_INTERNATIONAL::Initialize();
	for (int i=0; tmp[i]; i++)
		if (tmp[i] == UI_INTERNATIONAL::monDecimalSeparator[0])
			tmp[i] = '.';
	(void) sscanf(tmp, "%lf", &r);
	*value = r;
}

void UI_BIGNUM::rtob(rbignum value)
{
	int i, _sign = 0;
	char tmp[NUMBER_WHOLE+NUMBER_DECIMAL+4];

	if (value < 0)
	{
		_sign = 1;
		value = -value;
	}
	::sprintf(tmp, "%-.*f", NUMBER_DECIMAL, value);
	UI_INTERNATIONAL::Initialize();
	for (i=0; tmp[i]; i++)
		if (tmp[i] == '.')
			tmp[i] = UI_INTERNATIONAL::monDecimalSeparator[0];
	Import(tmp);
	sign = _sign;
}

NMI_RESULT UI_BIGNUM::Import(rbignum value)
{
	this->rtob(value);
	return NMI_OK;
}


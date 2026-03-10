//	Zinc Interface Library - LIBMAIN.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "ui_gen.hpp"
#pragma hdrstop

int FAR PASCAL LibMain(HANDLE hInstance, WORD wDataSegment, WORD wHeapSize, LPSTR lpszCmdLine)
{
	if (wHeapSize != 0)
		UnlockData(0);
	return (1);
}

int FAR PASCAL WEP(int bSystemExit)
{
	return (1);
}

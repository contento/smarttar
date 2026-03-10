/*= Archive Library v2.12 ================================================
 |
 |  H\WINUTIL.H
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  WINUTIL.H contains the declaration for utility functions used under
 |  the Windows User Interface.
 |
 +=======================================================================*/

#ifndef __WINUTIL_H__
#define __WINUTIL_H__

#ifdef __cplusplus
extern "C"
#endif

AL_LINKAGE void AL_CFUNCTION EditDisplay(HWND hDlg, int id, char AL_DLL_FAR *fmt, ...);

#endif // #ifndef __WINUTIL_H__

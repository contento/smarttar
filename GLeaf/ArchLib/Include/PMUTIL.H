/*= Archive Library v2.12 ================================================
 |
 |  H\PMUTIL.H
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  PMUTIL.H contains the declaration for utility functions used under
 |  the Presentation Manager Interface.
 |
 +=======================================================================*/

#ifndef __PMUTIL_H__
#define __PMUTIL_H__

#ifdef __cplusplus
extern "C" {
#endif

AL_LINKAGE void AL_FUNCTION CenterWindow(HWND hwnd);
AL_LINKAGE void AL_FUNCTION TrimSystemMenu(HWND hwnd);
AL_LINKAGE int AL_FUNCTION SetupSlider(HWND hDlg, int slider_id, int draw_ticks);
AL_LINKAGE void AL_CFUNCTION EditDisplay(HWND hDlg, int id, char AL_DLL_FAR *fmt, ...);

#ifdef __cplusplus
};
#endif

#endif // #ifndef __PMUTIL_H__

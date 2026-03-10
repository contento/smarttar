/*= Archive Library v2.12 ================================================
 |
 |  EXAMPLES\EX17PM.CPP
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  EX17PM.CPP demonstrates a few of the ALEntryList manipulation
 |  functions.  It lets you create a list, delete duplicates, and
 |  delete a list.
 |
 +- Functions ------------------------------------------------------------
 |  ALEntryList::FillListBox()
 |  ALEntryList::UnmarkDuplicates()
 |
 +=======================================================================*/

#define INCL_WIN
#define INCL_WINDIALOGS
#define INCL_WINPOINTERS

#include <os2.h>
#include <stdio.h>
#include <string.h>

#include "ex17pm.h"
#include "arclib.h"
#include "pmmon.h"
#include "glarc.h"
#include "glengn.h"
#include "pkarc.h"
#include "pkengn.h"
#include "copyengn.h"
#include "filestor.h"
#include "memstore.h"
#include "wildcard.h"

ALEntryList *pList = 0;

void DeleteList(HWND hDlg)
{
 if (pList)
  delete pList;
 pList = 0;
 WinSendDlgItemMsg(hDlg, AL_DIRECTORY, LM_DELETEALL, 0, 0);
}

// Here we delete any duplicate entries, then update the list box with the
// new, non-duplicated list.

void DeleteDuplicates(HWND hDlg)
{
 if (pList) {
  pList->UnmarkDuplicates(*pList);
  pList->DeleteUnmarked();
  pList->FillListBox(hDlg, AL_DIRECTORY);
 }
}

void MakeList(HWND hDlg)
{
 char dir_mask[128];

 if (pList)
  delete pList;

#if defined (ZIP)
 pList = new ALEntryList(0, PkTools());
#else
 pList = new ALEntryList(0, GlTools());
#endif
 WinQueryDlgItemText(hDlg, AL_DIR_MASK, 128, dir_mask);
 pList->AddWildCardFiles(dir_mask);
 pList->FillListBox(hDlg, AL_DIRECTORY);
}

MRESULT EXPENTRY AboutBoxDialogProc(HWND hwndDlg,
                                    ULONG msg,
                                    MPARAM mp1,
                                    MPARAM mp2)
{
 switch (msg) {
 case WM_INITDLG :
  TrimSystemMenu(hwndDlg);
  CenterWindow(hwndDlg);
  break;
 default :
  return WinDefDlgProc(hwndDlg, msg, mp1, mp2);
 }
 return (MRESULT) FALSE;
}

MRESULT EXPENTRY MainDialogProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
 switch (msg) {
 case WM_INITDLG :
  WinSetDlgItemText(hwnd, AL_DIR_MASK, "*.CPP");
  break;
 case WM_COMMAND :
  switch (LOUSHORT(mp1)) {
  case AL_ABOUT :
   WinDlgBox(HWND_DESKTOP,
             hwnd,
             AboutBoxDialogProc,
             NULLHANDLE,
             AL_ABOUT_DIALOG,
             0);
   break;
  case AL_CREATE_LIST :
   MakeList(hwnd);
   break;
  case AL_DELETE_LIST :
   DeleteList(hwnd);
   break;
  case AL_DELETE_DUPLICATES :
   DeleteDuplicates(hwnd);
   break;
  case AL_EXIT :
   WinPostMsg(hwnd, WM_QUIT, 0, 0);
   return FALSE;
  }
  break;
 case WM_CLOSE :
  WinPostMsg(hwnd, WM_QUIT, 0, 0);
  return FALSE;
 default :
  return WinDefDlgProc(hwnd, msg, mp1, mp2);
 }
 return (MRESULT) FALSE;
}

int main()
{
 HAB hab;
 HMQ hmq;
 HWND hwndDlg;
 QMSG qmsg;
 HPOINTER hIcon;

 hab = WinInitialize(0);
 hmq = WinCreateMsgQueue(hab, 0);
 hIcon = WinLoadPointer(HWND_DESKTOP, 0, AL_ICON);
 hwndDlg = WinLoadDlg(HWND_DESKTOP, // parent
                      HWND_DESKTOP, // owner
                      MainDialogProc, // dialog window proc
                      0, // module handle
                      AL_MAIN_DIALOG, // dialog template ID
                      0); // No application data
 WinSendMsg(hwndDlg, WM_SETICON, (MPARAM) hIcon, 0);
 CenterWindow(hwndDlg);
 if (hwndDlg)
  while (WinGetMsg(hab, &qmsg, NULLHANDLE, 0, 0))
   WinDispatchMsg(hab, &qmsg);
 WinDestroyWindow(hwndDlg);
 WinDestroyMsgQueue(hmq);
 WinTerminate(hab);
 return 0;
}

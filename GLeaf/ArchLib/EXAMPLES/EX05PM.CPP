/*= Archive Library v2.12 ================================================
 |
 |  EXAMPLES\EX05PM.CPP
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  EX05PM.CPP demonstrates the capabilities of the debug libraries by
 |  doing a few bad things.  Most of the bad things should cause an
 |  assertion error, with a good explanation of what happened.  However,
 |  this will only work if you link with the debug versions of the
 |  libraries!
 |
 |  A lot of the other demo programs in the library use a dummy framing
 |  window that surrounds the dialog box.  This guy skips all that and
 |  just uses the dialog as the main window, period.  This makes it
 |  nice and simple.  In exchange for that, we give up accelerator keys
 |  and a few other goodies.
 |
 +- Functions ------------------------------------------------------------
 |
 +=======================================================================*/

#define INCL_WIN
#define INCL_WINDIALOGS
#define INCL_WINPOINTERS

#include <os2.h>
#include <stdio.h>

#include "ex05pm.h"
#include "arclib.h"
#include "glarc.h"
#include "pmmon.h"
#include "filestor.h"

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
  break;
 case WM_COMMAND :
  switch (LOUSHORT(mp1)) {
  case AL_DESTROY_TWICE : {
    ALFile *f = new ALFile;
    delete f;
    delete f;
    break;
  }
 case AL_WRITE_PAST_END : {
   char *p = new char[100];
   p[100] = 0;
   delete p;
   break;
 }
 case AL_UNDERSHOOT : {
  ALGlArchive *p = new ALGlArchive("test.gal");
  ((char *) p)[- 1] = 0;
  delete p;
  break;
 }
 case AL_DELETE_BAD_PTR : {
  char *p = new char[100];
  p[50] = 1;
  p++;
  delete p;
  break;
 }
 case AL_ABOUT :
  WinDlgBox(HWND_DESKTOP,
            hwnd,
            AboutBoxDialogProc,
            NULLHANDLE,
            AL_ABOUT_DIALOG,
            0);
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
                      MainDialogProc,  // dialog window proc
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

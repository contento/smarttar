/*= Archive Library v2.12 ================================================
 |
 |  EXAMPLES\EX20PM.CPP
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  EX20PM.CPP demonstrates some of the timedate functions in ALStorage.
 |  It is a "read-only" program that doesn't modify any of the files it
 |  messes with.  It just lets you double click on a file, then see the
 |  time date stamps for that file.
 |
 +- Functions ------------------------------------------------------------
 |  ALTimeDate::GetTimeDate()
 |  ALTimeDate::GetUnixTime()
 |
 +=======================================================================*/

#define INCL_WIN
#define INCL_WINDIALOGS
#define INCL_WINPOINTERS

#include <os2.h>
#include <stdio.h>
#include <string.h>

#include "ex20pm.h"
#include "arclib.h"
#include "pmmon.h"
#include "wildcard.h"
#include "filestor.h"

void ReadDir(HWND hDlg)
{
 ALWildCardExpander files("*.*");
 char *p;
 while ((p = files.GetNextFile()) != 0)
  WinSendDlgItemMsg(hDlg,
                    AL_FILES,
                    LM_INSERTITEM,
                    MPFROMSHORT(LIT_SORTASCENDING),
                    MPFROMP((PSZ) p));

}

void UpdateData(HWND hDlg)
{
 char buf[128];
 long stamp;

 int i = (int) WinSendDlgItemMsg(hDlg, AL_FILES, LM_QUERYSELECTION, 0, 0);
 if (i == LIT_NONE)
  return;
 WinSendDlgItemMsg(hDlg,
                   AL_FILES,
                   LM_QUERYITEMTEXT,
                   MPFROM2SHORT((short) i, 128),
                   MPFROMP((PSZ) buf));
 WinSetDlgItemText(hDlg, AL_FILE_NAME, buf);
 ALFile file(buf);
 file.Open();
 file.Close();
 if (file.mStatus < AL_SUCCESS) {
  WinSetDlgItemText(hDlg, AL_UNIX_TIME_STAMP, "<Error>");
  return;
 }
 sprintf(buf, "%ld", stamp = file.mTimeDate.GetUnixTime());
 WinSetDlgItemText(hDlg, AL_UNIX_TIME_STAMP, buf);
 WinSetDlgItemShort(hDlg,
                    AL_YEARS_SINCE_1970, (short) (stamp / 365L / 24L / 60L / 60L), 0);
 struct tm tblock;
 file.mTimeDate.GetTimeDate(&tblock);
 char *p = asctime(&tblock);
 strcpy(buf, p);
 buf[24] = '\0';
 WinSetDlgItemText(hDlg, AL_DATE_AND_TIME, buf);
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
  ReadDir(hwnd);
  break;
 case WM_CONTROL :
  if (SHORT2FROMMP(mp1) == LN_ENTER && SHORT1FROMMP(mp1) == AL_FILES)
   UpdateData(hwnd);
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

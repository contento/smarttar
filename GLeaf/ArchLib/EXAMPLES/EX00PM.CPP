/*= Archive Library v2.12 ================================================
 |
 |  EXAMPLES\EX00PM.CPP
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  EX00PM.CPP creates an archive and adds files to it, while using an
 |  ALBarGraph monitor.  By default it uses *.CPP as its input files, but
 |  you can modify this by entering new data in the AL_INPUT_FILES text
 |  box.
 |
 |  EX00PM.CPP uses a dialog as its main window.  Unlike most of the other
 |  PM examples, this guy doesn't have a dummy frame window around the
 |  dialog.
 |
 +- Notes ----------------------------------------------------------------
 |  #define ZIP if you wish to create PM00.ZIP.
 |
 +- Functions ------------------------------------------------------------
 |  ALArchiveBase::Create()
 |  ALOs2Message::ALOs2Message()
 |
 +=======================================================================*/

#define INCL_WIN
#define INCL_WINDIALOGS
#define INCL_WINPOINTERS

#include <os2.h>
#include "ex00pm.h"
#include "arclib.h"
#include "pkarc.h"
#include "glarc.h"
#include "pmmon.h"

ALArchive *pArchive = 0;

void Compress(HWND hwnd)
{
 char archive_name[128];
 char input_name[128];

 WinEnableWindow(WinWindowFromID(hwnd, AL_COMPRESS), FALSE);
 ALOs2Message monitor(AL_MONITOR_JOB,
                      WinWindowFromID(hwnd, AL_OPERATION),
                      AL_SEND_BYTE_COUNT,
                      WinWindowFromID(hwnd, AL_BYTE_COUNT));
 WinQueryDlgItemText(hwnd, AL_ARCHIVE, 128, archive_name);
#if defined (ZIP)
 pArchive = new ALPkArchive(archive_name);
 ALEntryList list(&monitor, PkCompressTools());
#else
 pArchive = new ALGlArchive(archive_name);
 ALEntryList list(&monitor, GlCompressTools());
#endif
 WinQueryDlgItemText(hwnd, AL_FILES, 128, input_name);
 list.AddWildCardFiles(input_name);
 pArchive->Create(list);
 WinSetDlgItemText(hwnd, AL_STATUS, pArchive->mStatus.GetStatusString());
 delete pArchive;
 pArchive = 0;
 WinEnableWindow(WinWindowFromID(hwnd, AL_COMPRESS), TRUE);
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
#if defined (ZIP)
  WinSetDlgItemText(hwnd, AL_ARCHIVE, "PM00.ZIP");
#else
  WinSetDlgItemText(hwnd, AL_ARCHIVE, "PM00.GAL");
#endif
  WinSetDlgItemText(hwnd, AL_FILES, "*.CPP");
  WinSetDlgItemText(hwnd, AL_OPERATION, "");
  WinSetDlgItemText(hwnd, AL_BYTE_COUNT, "");
  break;
 case WM_COMMAND :
  switch (LOUSHORT(mp1)) {
  case AL_COMPRESS :
   Compress(hwnd);
   break;
  case AL_ABORT :
   if (pArchive)
    pArchive->GetStorageObject()->mStatus.SetError(AL_USER_ABORT,
                                                   "User pressed abort key");
  break;
 case AL_ABOUT :
  WinDlgBox(HWND_DESKTOP, hwnd, AboutBoxDialogProc, NULLHANDLE, AL_ABOUT_DIALOG, 0);
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
 SWP swp;
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
 WinQueryTaskSizePos(hab, 0, &swp);
 WinSetWindowPos(hwndDlg, // Change the window's position
                 NULLHANDLE, // ignfored, SWP_ZORDER not set
                 swp.x, swp.y, // position of ll corner
                 0, 0, // ignored, SWP_SIZE not set
                 SWP_SHOW | // Show the dialog box
                 SWP_MOVE); // Set new position
 if (hwndDlg)
  while (WinGetMsg(hab, &qmsg, NULLHANDLE, 0, 0))
   WinDispatchMsg(hab, &qmsg);
 WinDestroyWindow(hwndDlg);
 WinDestroyMsgQueue(hmq);
 WinTerminate(hab);
 return 0;
}

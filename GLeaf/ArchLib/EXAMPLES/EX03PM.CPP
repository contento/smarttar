/*= Archive Library v2.12 ================================================
 |
 |  EXAMPLES\EX03PM.CPP
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  EX03PM.CPP demonstrates ALArchiveBase::Delete().  It lets you
 |  select a list of files to delete from an archive, then performs
 |  the deletion.  The list boxes are updated right afterwards so you
 |  can see the results.
 |
 |  This is a very simple Windows program.  It works by creating a modeless
 |  dialog box, then wrapping a framing window around it.  By using a framing
 |  window as the main window we are able to handle accelerator keys, menus,
 |  and icons a bit easier. Because of this approach, WinMain() and
 |  MainWndProc() are not very exciting, and have been moved down to the
 |  bottom of the file.  MainWndProc() does handle menus and accelerator
 |  keys, so it comes first.
 |
 +- Notes ----------------------------------------------------------------
 |  #define ZIP if you wish to create PM00.ZIP.
 |
 +- Functions ------------------------------------------------------------
 |  ALArchiveBase::Delete()
 |
 +=======================================================================*/

#define INCL_WIN
#define INCL_WINDIALOGS
#define INCL_WINPOINTERS

#include <os2.h>
#include "ex03pm.h"
#include "arclib.h"
#include "pkarc.h"
#include "glarc.h"
#include "pmmon.h"
#include "wildcard.h"

ALArchive *pArchive = 0;

void ReadArchive(HWND hDlg, int name_id, int list_id)
{
 char name[128];

 WinQueryDlgItemText(hDlg, name_id, 128, name);
#if defined (ZIP)
 ALPkArchive archive(name);
#else
 ALGlArchive archive(name);
#endif
 archive.FillListBox(hDlg, list_id);
 return;
}

void Delete (HWND hDlg)
{
 char input_name[128];

 ALOs2Message monitor(AL_MONITOR_JOB,
                      WinWindowFromID(hDlg, AL_PROGRESS_TEXT),
                      AL_SEND_RATIO,
                      WinWindowFromID(hDlg, AL_PROGRESS_BAR),
                      SLM_SETSLIDERINFO);
 WinQueryDlgItemText(hDlg, AL_ARCHIVE_NAME, 128, input_name);
#if defined (ZIP)
 pArchive = new ALPkArchive(input_name);
 ALEntryList list(&monitor, PkTools());
#else
 pArchive = new ALGlArchive(input_name);
 ALEntryList list(&monitor, GlTools());
#endif
 if (!pArchive)
  return;
 pArchive->ReadDirectory(list);
 list.ClearMarks();
 int count = list.SetMarksFromListBox(hDlg, AL_ARCHIVE);
 EditDisplay(hDlg,
             AL_MESSAGE,
             "\n%d file%s selected",
             count,
             (count != 1 ? "s are" : " is"));
 // The new archive has a blank name.  This tells the delete function
 // to rename it after the deletion is done.

#if defined (ZIP)
 ALPkArchive archive("");
#else
 ALGlArchive archive("");
#endif
 pArchive->Delete (list, archive);
 WinSetDlgItemText(hDlg,
                   AL_BACKUP_ARCHIVE_NAME,
                   pArchive->GetStorageObject()->mName.GetSafeName());
 ReadArchive(hDlg, AL_ARCHIVE_NAME, AL_ARCHIVE);
 ReadArchive(hDlg, AL_BACKUP_ARCHIVE_NAME, AL_BACKUP_ARCHIVE);
 delete pArchive;
 EditDisplay(hDlg, AL_MESSAGE, "\n%s", pArchive->mStatus.GetStatusDetail());
 pArchive = 0;
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
  WinSetDlgItemText(hwnd, AL_ARCHIVE_NAME, "PM00.ZIP");
  WinSetDlgItemText(hwnd, AL_BACKUP_ARCHIVE_NAME, "");
#else
  WinSetDlgItemText(hwnd, AL_ARCHIVE_NAME, "PM00.GAL");
  WinSetDlgItemText(hwnd, AL_BACKUP_ARCHIVE_NAME, "");
#endif
  WinSetDlgItemText(hwnd, AL_PROGRESS_TEXT, "");
  ReadArchive(hwnd, AL_ARCHIVE_NAME, AL_ARCHIVE);
  ReadArchive(hwnd, AL_BACKUP_ARCHIVE_NAME, AL_BACKUP_ARCHIVE);
  SetupSlider(hwnd, AL_PROGRESS_BAR, 1);
  break;
 case WM_COMMAND :
  switch (LOUSHORT(mp1)) {
  case AL_DELETE :
   Delete (hwnd);
   break;
  case AL_ABORT :
   if (pArchive)
    pArchive->GetStorageObject()->mStatus.SetError(AL_USER_ABORT,
                                                   "User pressed abort key");
  break;
 case AL_ABOUT :
  WinDlgBox(HWND_DESKTOP,
            hwnd,
            AboutBoxDialogProc,
            NULLHANDLE,
            AL_ABOUT_DIALOG,
            0);
  break;

  // This is really lame, but sorry, I can't figure out
  // how to process the enter key in a text box...

 case AL_DEF :
 {
  ReadArchive(hwnd, AL_ARCHIVE_NAME, AL_ARCHIVE);
  ReadArchive(hwnd, AL_BACKUP_ARCHIVE_NAME, AL_BACKUP_ARCHIVE);
 }
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

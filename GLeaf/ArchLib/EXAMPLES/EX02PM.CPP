/*= Archive Library v2.12 ================================================
 |
 |  EXAMPLES\EX02PM.CPP
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  EX02PM.CPP demonstrates ALArchiveBase::Create() and
 |  ALArchiveBase::Append().  Both of these functions have overloaded
 |  versions so that you can use an archive or a list of storage objects
 |  as your input.  In this case, the input comes from an archive.  The
 |  program lets you select a list of files from the input archive, and
 |  then gives you the choice of either copying or appending to a new
 |  archive.
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
 |  #define ZIP if you wish to use ZIP compression
 |
 +- Functions ------------------------------------------------------------
 |  ALArchive::Append()
 |  ALArchiveBase::Create()
 |  ALArchiveBase::ReadDirectory()
 |  ALEntryList::ClearMarks()
 |  ALEntryList::SetMarksFromListBox()
 |
 +=======================================================================*/

#define INCL_WIN
#define INCL_WINDIALOGS
#define INCL_WINPOINTERS

#include <os2.h>
#include "ex02pm.h"
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

void Copy(HWND hDlg)
{
 char input_name[128];

 WinQueryDlgItemText(hDlg, AL_INPUT_ARCHIVE_NAME, 128, input_name);
 ALOs2Message monitor(AL_MONITOR_JOB,
                      WinWindowFromID(hDlg, AL_PROGRESS_TEXT),
                      AL_SEND_RATIO,
                      WinWindowFromID(hDlg, AL_PROGRESS_BAR),
                      SLM_SETSLIDERINFO);
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
 int count = list.SetMarksFromListBox(hDlg, AL_INPUT_ARCHIVE);
 EditDisplay(hDlg,
             AL_MESSAGE,
             "\n%d file%s selected",
             count,
             (count != 1 ? "s are" : " is"));
 char archive_name[128];
 WinQueryDlgItemText(hDlg, AL_OUTPUT_ARCHIVE_NAME, 128, archive_name);
#if defined (ZIP)
 ALPkArchive archive(archive_name);
#else
 ALGlArchive archive(archive_name);
#endif
 archive.Create(*pArchive, list);
 ReadArchive(hDlg, AL_INPUT_ARCHIVE_NAME, AL_INPUT_ARCHIVE);
 ReadArchive(hDlg, AL_OUTPUT_ARCHIVE_NAME, AL_OUTPUT_ARCHIVE);
 EditDisplay(hDlg,
             AL_MESSAGE,
             "\n%s",
             archive.mStatus.GetStatusDetail());
}

void Append(HWND hDlg)
{
 char input_name[128];

 ALOs2Message monitor(AL_MONITOR_JOB,
                      WinWindowFromID(hDlg, AL_PROGRESS_TEXT),
                      AL_SEND_RATIO,
                      WinWindowFromID(hDlg, AL_PROGRESS_BAR),
                      SLM_SETSLIDERINFO);
 WinQueryDlgItemText(hDlg, AL_INPUT_ARCHIVE_NAME, 128, input_name);
#if defined (ZIP)
 if ((pArchive = new ALPkArchive(input_name)) == 0)
  return;
  ALEntryList list(&monitor, PkTools());
#else
 if ((pArchive = new ALGlArchive(input_name)) == 0)
  return;
 ALEntryList list(&monitor, GlTools());
#endif

 pArchive->ReadDirectory(list);
 list.ClearMarks();
 int count = list.SetMarksFromListBox(hDlg, AL_INPUT_ARCHIVE);
 EditDisplay(hDlg,
             AL_MESSAGE,
             "\n%d file%s selected",
             count,
             (count != 1 ? "s are" : " is"));
 char archive_name[128];
 WinQueryDlgItemText(hDlg, AL_OUTPUT_ARCHIVE_NAME, 128, archive_name);
#if defined (ZIP)
 ALPkArchive archive(archive_name);
#else
 ALGlArchive archive(archive_name);
#endif
 archive.Append(*pArchive, list);
 ReadArchive(hDlg, AL_INPUT_ARCHIVE_NAME, AL_INPUT_ARCHIVE);
 ReadArchive(hDlg, AL_OUTPUT_ARCHIVE_NAME, AL_OUTPUT_ARCHIVE);
 EditDisplay(hDlg,
             AL_MESSAGE,
             "\n%s",
             archive.mStatus.GetStatusDetail());
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
  WinSetDlgItemText(hwnd, AL_INPUT_ARCHIVE_NAME, "PM00.ZIP");
  WinSetDlgItemText(hwnd, AL_OUTPUT_ARCHIVE_NAME, "OUT00.ZIP");
#else
  WinSetDlgItemText(hwnd, AL_INPUT_ARCHIVE_NAME, "PM00.GAL");
  WinSetDlgItemText(hwnd, AL_OUTPUT_ARCHIVE_NAME, "OUT00.GAL");
#endif
  WinSetDlgItemText(hwnd, AL_PROGRESS_TEXT, "");
  ReadArchive(hwnd, AL_INPUT_ARCHIVE_NAME, AL_INPUT_ARCHIVE);
  ReadArchive(hwnd, AL_OUTPUT_ARCHIVE_NAME, AL_OUTPUT_ARCHIVE);
  SetupSlider(hwnd, AL_PROGRESS_BAR, 1);
  break;
 case WM_COMMAND :
  switch (LOUSHORT(mp1)) {
  case AL_COPY :
   Copy(hwnd);
   break;
  case AL_APPEND :
   Append(hwnd);
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
  ReadArchive(hwnd, AL_INPUT_ARCHIVE_NAME, AL_INPUT_ARCHIVE);
  ReadArchive(hwnd, AL_OUTPUT_ARCHIVE_NAME, AL_OUTPUT_ARCHIVE);
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

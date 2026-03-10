/*= Archive Library v2.12 ================================================
 |
 |  EXAMPLES\EX01PM.CPP
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  EX01PM.CPP demonstrates the Append() function, using the overloaded
 |  version that appends files to an archive.  It has three command
 |  handlers.  One reads the contents of the current directory, so you
 |  can pick files to append.  The next command actually performs the
 |  append.  And finally, there is a function to read the contents
 |  of the archive, so you know that the append did something useful.
 |
 +- Notes ----------------------------------------------------------------
 |  #define ZIP if you wish to use ZIP compression.
 |
 +- Functions ------------------------------------------------------------
 |  ALArchiveBase::~ALArchiveBase()
 |  ALArchiveBase::Append()
 |  ALArchiveBase::FillListBox()
 |  ALArchive::MakeEntriesFromListBox()
 |  ALEntryList::~ALEntryList()
 |  ALEditControlStream::ALEditControlStream()
 |  ALEditControlStream::~ALEditControlStream()
 |  ALWildCardExpander::ALWildCardExpander()
 |  ALWildCardExpander::~ALWildCardExpander()
 |  ALWildCardExpander::GetNextFile()
 |
 +=======================================================================*/
#define INCL_WIN
#define INCL_WINDIALOGS
#define INCL_WINPOINTERS

#include <os2.h>
#include "ex01pm.h"
#include "arclib.h"
#include "pkarc.h"
#include "glarc.h"
#include "pmmon.h"
#include "wildcard.h"

ALArchive *pArchive = 0;

void ReadDir(HWND hDlg)
{
 char dir_mask[128];

 WinQueryDlgItemText(hDlg, AL_DIR_MASK, 128, dir_mask);
 WinSendDlgItemMsg(hDlg, AL_INPUT_FILES, LM_DELETEALL, 0, 0);
 ALWildCardExpander files(dir_mask);
 char AL_DLL_FAR *p;
 while ((p = files.GetNextFile()) != 0)
  WinSendDlgItemMsg(hDlg,
                    AL_INPUT_FILES,
                    LM_INSERTITEM,
                    MPFROMSHORT(LIT_END),
                    MPFROMP((PSZ) p));
}

void ReadArchive(HWND hDlg)
{
 char input_name[128];

 WinQueryDlgItemText(hDlg, AL_ARCHIVE_NAME, 128, input_name);
#if defined (ZIP)
 ALPkArchive archive(input_name);
#else
 ALGlArchive archive(input_name);
#endif
 archive.FillListBox(hDlg, AL_ARCHIVE_CONTENTS);
 return;
}

void Append(HWND hDlg)
{
 char input_name[128];

 WinQueryDlgItemText(hDlg, AL_ARCHIVE_NAME, 128, input_name);
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
 if (!pArchive) {
  WinSendDlgItemMsg(hDlg,
                    AL_ARCHIVE_CONTENTS,
                    LM_INSERTITEM,
                    0,
                    MPFROMP((PSZ) "Error!"));
  return;
 }
 list.MakeEntriesFromListBox(hDlg, AL_INPUT_FILES);
 pArchive->Append(list);
 ReadArchive(hDlg);
 WinSetDlgItemText(hDlg, AL_PROGRESS_TEXT, pArchive->mStatus.GetStatusDetail());
 delete pArchive;
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

int foo = 0;
HWND h;

MRESULT EXPENTRY MainDialogProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
 switch (msg) {
 case WM_INITDLG :
#if defined (ZIP)
  WinSetDlgItemText(hwnd, AL_ARCHIVE_NAME, "PM00.ZIP");
#else
  WinSetDlgItemText(hwnd, AL_ARCHIVE_NAME, "PM00.GAL");
#endif
  WinSetDlgItemText(hwnd, AL_PROGRESS_TEXT, "");
  WinSetDlgItemText(hwnd, AL_DIR_MASK, "*.RC");
  ReadDir(hwnd);
  ReadArchive(hwnd);
  SetupSlider(hwnd, AL_PROGRESS_BAR, 1);
  break;
 case WM_COMMAND :
  switch (LOUSHORT(mp1)) {
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
   h = WinQueryFocus(HWND_DESKTOP);
   ReadDir(hwnd);
   ReadArchive(hwnd);
  }
  break;
 default :
  foo++;
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

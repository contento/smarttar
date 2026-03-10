/*= Archive Library v2.12 ================================================
 |
 |  EXAMPLES\EX15PM.CPP
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  EX15PM.CPP is a grab bag that uses several of the archive status and
 |  informational functions.  You open an archive by clicking on it from a
 |  list box, then see all the information regarding that archive.
 |
 |  EX15PM.CPP checks for duplicate entries in an archive.  You might
 |  have to fiddle with the examples here in order to get one with a
 |  duplicate entry, because we try to avoid that.  I created a duplicate
 |  entry by using a binary file editor and just changing the name of an
 |  entry stored at the end of the archive.
 |
 +- Notes ----------------------------------------------------------------
 |  #define ZIP if you wish to use ZIP compression.
 |
 +- Functions ------------------------------------------------------------
 |  ALArchiveBase::FillListBox()
 |  ALArchiveBase::ReadDirectory()
 |  ALEntry::Duplicate()
 |
 +=======================================================================*/

#define INCL_WIN
#define INCL_WINDIALOGS
#define INCL_WINPOINTERS

#include <os2.h>
#include <stdio.h>
#include <string.h>

#include "ex15pm.h"
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

void ReadArchive(HWND hDlg)
{
 char buf[128];

 int i = (int) WinSendDlgItemMsg(hDlg,
                                 AL_DIRECTORY,
                                 LM_QUERYSELECTION,
                                 0,
                                 0);
 if (i == LIT_NONE)
  return;
 WinSendDlgItemMsg(hDlg,
                   AL_DIRECTORY,
                   LM_QUERYITEMTEXT,
                   MPFROM2SHORT((short) i, 128),
                   MPFROMP((PSZ) buf));
#if defined (ZIP)
 ALPkArchive archive(buf);
#else
 ALGlArchive archive(buf);
#endif
 archive.FillListBox(hDlg, AL_CONTENTS);
 WinSetDlgItemText(hDlg, AL_ARCHIVE_NAME, buf);
 sprintf(buf, "0x%04x  (%d)", archive.GetVersion(), archive.GetVersion());
 WinSetDlgItemText(hDlg, AL_ARCHIVE_VERSION, buf);
 sprintf(buf, "%d", archive.mStatus.GetStatusCode());

 ALEntryList list(0, FullTools());
 archive.ReadDirectory(list);
 ALEntry *entry = list.GetFirstEntry();
 while (entry) {
  if (entry->Duplicate(list))
   break;
  entry = entry->GetNextEntry();
 }
 if (entry == 0)
  WinSetDlgItemText(hDlg, AL_FIRST_DUPLICATE_ENTRY, "None");
 else {
  WinSetDlgItemText(hDlg,
                    AL_FIRST_DUPLICATE_ENTRY,
                    (PSZ) entry->mpStorageObject->mName);
 archive.mStatus.SetError(AL_DUPLICATE_ENTRY, "Duplicate entry in archive!");
 }
 WinSetDlgItemText(hDlg, AL_ARCHIVE_STATUS_CODE, buf);
 WinSetDlgItemText(hDlg, AL_ARCHIVE_STATUS_STRING, archive.mStatus.GetStatusString());
 WinSetDlgItemText(hDlg, AL_ARCHIVE_STATUS_DETAIL, archive.mStatus.GetStatusDetail());
}

void ReadDir(HWND hDlg)
{
 char dir_mask[128];

 WinQueryDlgItemText(hDlg, AL_DIR_MASK, 128, dir_mask);
 WinSendDlgItemMsg(hDlg, AL_DIRECTORY, LM_DELETEALL, 0, 0);
 ALWildCardExpander files(dir_mask);
 char *p;
 while ((p = files.GetNextFile()) != 0)
  WinSendDlgItemMsg(hDlg,
                    AL_DIRECTORY,
                    LM_INSERTITEM,
                    MPFROMSHORT(LIT_SORTASCENDING),
                    MPFROMP((PSZ) p));

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
  WinSendDlgItemMsg(hwnd,
                    AL_DIRECTORY,
                    LM_INSERTITEM,
                    MPFROMSHORT(LIT_END),
                    MPFROMP((PSZ) "Hey"));
#if defined (ZIP)
  WinSetDlgItemText(hwnd, AL_DIR_MASK, "*.ZIP");
#else
  WinSetDlgItemText(hwnd, AL_DIR_MASK, "*.GAL");
#endif
  ReadDir(hwnd);
  break;
 case WM_CONTROL :
  if (SHORT2FROMMP(mp1) == LN_ENTER && SHORT1FROMMP(mp1) == AL_DIRECTORY)
   ReadArchive(hwnd);
  else if (SHORT2FROMMP(mp1) == LN_SELECT && SHORT1FROMMP(mp1) == AL_CONTENTS)
   WinSendDlgItemMsg(hwnd, AL_CONTENTS, LM_SELECTITEM, MPFROMSHORT(LIT_NONE), 0);
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
  case AL_READ_DIR :
   ReadDir(hwnd);
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

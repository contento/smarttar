/*= Archive Library v2.12 ================================================
 |
 |  EXAMPLES\EX04PM.CPP
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  EX04PM.CPP demonstrates ALMemory::ALMemory().  It lets you
 |  compress selected input files to a memory archive.  It then turns around
 |  and decompresses the objects to a new memory files, then compares the
 |  contents of the two.
 |
 |  This is a very simple PM program.  It works by creating a modeless
 |  dialog box, then wrapping a framing window around it.  By using a framing
 |  window as the main window we are able to handle accelerator keys, menus,
 |  and icons a bit easier.
 |
 +- Functions ------------------------------------------------------------
 |  ALStorage::~ALStorage()
 |  ALStorage::Compare()
 |  ALStorage::Delete()
 |  ALFile::ALFile()
 |  ALFile::~ALFile()
 |  ALMemory::ALMemory()
 |  ALArchiveBase::Extract()
 |  ALArchive::ALArchive()
 |  ALName::GetSafeName()
 |
 +=======================================================================*/

#define INCL_WIN
#define INCL_WINDIALOGS
#define INCL_WINPOINTERS

#include <os2.h>
#include <stdio.h>

#include "ex04pm.h"
#include "arclib.h"
#include "pkarc.h"
#include "glarc.h"
#include "pmmon.h"
#include "wildcard.h"
#include "memstore.h"
#include "filestor.h"

ALStorage *pObject = 0;

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

void ReadDir(HWND hwnd, int id)
{
 ALWildCardExpander e("*.*");
 char *p = e.GetNextFile();
 while (p) {
  WinSendDlgItemMsg(hwnd,
                    id,
                    LM_INSERTITEM,
                    MPFROMSHORT(LIT_SORTASCENDING),
                    MPFROMP((PSZ) p));
  p = e.GetNextFile();
 }
}

void CompressFiles(HWND hDlg, int list_box)
{
 // The first section of this routine is pretty similar to lots of other
 // example programs, both under Windows and DOS.  We create a new archive
 // using an ALMemory object as the base, then create a list with elements
 // picked out from a list box, then insert them into an archive.

 ALOs2Message monitor(AL_MONITOR_OBJECTS,
                      WinWindowFromID(hDlg, AL_PROGRESS_TEXT),
                      AL_SEND_RATIO,
                      WinWindowFromID(hDlg, AL_PROGRESS_NUMBER));
 pObject = new ALMemory("archive in RAM");
 if (!pObject)
  return;
#if defined (ZIP)
 ALPkArchive archive(*pObject);
 ALEntryList list(&monitor, PkTools());                                       //Deliberately use default engine
#else
 ALGlArchive archive(*pObject);
 ALEntryList list(&monitor, GlTools());
#endif
 list.MakeEntriesFromListBox(hDlg, list_box);
 archive.Create(list);
 if (pObject->mStatus < 0) {
  delete pObject;
  pObject = 0;
  WinMessageBox(HWND_DESKTOP,
                hDlg,
                "Error creating archive",
                "EX04PM",
                0,
                MB_ICONEXCLAMATION);
  return;
 }

 // Here I modify the job list so that the archive data records will
 // be extracted to memory objects instead of file objects.  Then I
 // execute the extract function, creating a scad of new ALMemory
 // objects.  At this point, the only way I have of getting to those
 // memory objects is by way of the list, so I have to hang on to it.
 // But I'm done with the archive, so I can delete it.

 for (ALEntry *job = list.GetFirstEntry(); job != 0; job = job->GetNextEntry()) {
  ALName temp = job->mpStorageObject->mName;
  delete job->mpStorageObject;
  job->mpStorageObject = new ALMemory(temp);
 }
 archive.Extract(list);
 if (archive.mStatus < 0) {
  delete pObject;
  pObject = 0;
  WinMessageBox(HWND_DESKTOP,
                hDlg,
                archive.mStatus.GetStatusDetail(),
                "Error extracting from archive",
                0,
                MB_ICONEXCLAMATION);
  return;
 }
 pObject->Delete ();
 delete pObject;
 pObject = 0;

 //  Now I iterate through the list.  For every entry in the list, I compare
 //  the memory object with the original ALFile object.  note that I am
 //  using a monitor object to track the progress of the comparison.  This
 //  is kind of a tricky bit.  Anyway, I do the compare and print the
 //  results.

 for (job = list.GetFirstEntry(); job != 0; job = job->GetNextEntry()) {
  pObject = new ALFile(job->mpStorageObject->mName);
  char buf[128];
  sprintf(buf, "Comparing %s", job->mpStorageObject->mName.GetSafeName());
  WinSetDlgItemText(hDlg, AL_PROGRESS_TEXT, buf);
  pObject->mpMonitor = &monitor;

  // Since I am monitoring objects, I need to set up these two data members in
  // the monitor if I expect it to work properly.

  pObject->mpMonitor->mlObjectSize = -1;
  pObject->mpMonitor->mlObjectStart = 0;
  int result = pObject->Compare(*job->mpStorageObject);
  if (result != AL_SUCCESS)
   WinSetDlgItemText(hDlg, AL_PROGRESS_TEXT, "Failure!");
  if (pObject->mStatus < 0) {
   delete pObject;
   pObject = 0;
   return;
  } else {
   delete pObject;
   pObject = 0;
   WinSetDlgItemText(hDlg, AL_PROGRESS_TEXT, "Success!");
  }
 }
}

MRESULT EXPENTRY MainDialogProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
 switch (msg) {
 case WM_INITDLG :
  WinSetDlgItemText(hwnd, AL_PROGRESS_TEXT, "");
  ReadDir(hwnd, AL_FILES);
  break;
 case WM_COMMAND :
  switch (LOUSHORT(mp1)) {
  case AL_ABORT :
   if (pObject)
    pObject->mStatus.SetError(AL_USER_ABORT, "User pressed abort key");
  break;
 case AL_CYCLE :
  CompressFiles(hwnd, AL_FILES);
  break;
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

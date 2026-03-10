/*= Archive Library v2.12 ================================================
 |
 |  EXAMPLES\EX21WIN.C
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  EX21WIN.C is used to demo the functions that set the job size and job
 |  so far data members of the monitor class.  This is something you need
 |  to know how to do if you are ever going to try to use a monitor for
 |  something of your own, instead of our archive or compressed object
 |  classes.
 |
 +- Notes ----------------------------------------------------------------
 |  #define AL_USING_DLL if your are linking to an Archive Library 16-bit
 |  import library.
 |  #define ZIP if you wish to create WIN00.ZIP.
 |
 +- Functions ------------------------------------------------------------
 |  ALMonitorSetJobSize()
 |  ALMonitorSetJobSoFar()
 |
 +=======================================================================*/

#define STRICT
#include <windows.h>
#include <stdarg.h>

#define AL_USING_DLL
#include "arclib.h"
#include "winmon.h"
#include "ex21win.h"

#if defined (AL_BORLAND) || !defined (AL_FLAT_MODEL)
 #define AL_3D
 #include "ctl3d.h"
#else
 #define Ctl3dColorChange()
 #define Ctl3dRegister(a)
 #define Ctl3dAutoSubclass(a)
 #define Ctl3dUnregister(a)
#endif

int iInstanceNumber;
HINSTANCE hInstance;
hALStorage hFile = 0;

void Process(HWND hDlg);

BOOL AL_EXPORT CALLBACK AboutDialogProc(HWND, UINT, WPARAM, LPARAM);

/*
  In this program, we have a main window, but it is just a dummy framing
  window for the dialog box.  This dialog box is where all the action
  takes place.  It handles input keystrokes and button presses from the
  dialog.
*/
BOOL AL_EXPORT CALLBACK MainDialogProc(HWND hDlg,
                                       UINT message,
                                       WPARAM wParam,
                                       LPARAM lParam)
{
 RECT rc;

 AL_UNUSED_PARAMETER(lParam);
 switch (message) {
 /*
   The first time into the dialog, we have to set up the default
   contents of a bunch of text boxes.  We also set up the title
   of the main window. (the framing window)
 */
 case WM_INITDIALOG :
  GetWindowRect(hDlg, &rc);
  SetWindowPos(hDlg,
               0,
               ((GetSystemMetrics(SM_CXSCREEN) - (rc.right - rc.left)) / 2),
               ((GetSystemMetrics(SM_CYSCREEN) - (rc.bottom - rc.top)) / 2),
               0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
  SetWindowText(hDlg, "Windows example 21");
  SetDlgItemText(hDlg, AL_FILE_NAME, "");
  SetDlgItemText(hDlg, AL_INPUT_FILES, "*.CPP");
  SetDlgItemText(hDlg, AL_PROGRESS_TEXT, "");
  SetDlgItemText(hDlg, AL_PROGRESS_NUMBER, "");
  return (TRUE);
 case WM_SYSCOLORCHANGE :
  Ctl3dColorChange();
  break;

 /*
   Some of our examples process a bunch of different WM_COMMAND messages.
   This one handles the process() button, which exercises the monitor,
   and the exit buttons, and that's it.
 */
 case WM_COMMAND :
  switch (wParam) {
  /*
    We don't want to process the file if a process is already running.
    If that is the case, we skip it.
  */
  case AL_PROCESS :
   if (!hFile)
    Process(hDlg);
   return TRUE;
   /*
     If a processing step is in progress, we try to set the error message
     for the file.  I think we have a high potential for a GPF here anyway,
     because we go ahead with the exit code.  When the processing
     is finished, what happens when the main dialog is gone?
   */
  case AL_EXIT :
  case WM_QUIT :
  case WM_DESTROY :
   if (hFile)
    ALStorageSetError(hFile, AL_USER_ABORT, "User pressed abort key");
  EndDialog(hDlg, TRUE);
  return TRUE;
  /*
    The abort key accomplishes its function by just setting an error
    flag on the file object.
  */
  case AL_ABORT :
   if (hFile)
    ALStorageSetError(hFile, AL_USER_ABORT, "User pressed abort key");
  return TRUE;

  case AL_ABOUT :
   DialogBox(hInstance, "ALAboutDialog", 0, (DLGPROC) AboutDialogProc);
   return TRUE;
  }
  break;
 }
 return FALSE;
}

/*
  Process is a bogus function.  It just opens each file in order,then
  reads in all the bytes in the file.  This simulates some sort of
  processing you might do on your own.  The important thing to note
  here is how we manage to do some processing on our own and still
  incorporate the same monitor that other archiving functions use.
*/
void Process(HWND hDlg)
{
 char input_name[128];
 char temp[21];
 hALEntryList hList;
 hALMonitor hMonitor;
 hALEntry hEntry;
 long JobSize;
 long JobSoFar;

 hMonitor = newALWindowsMessage(AL_MONITOR_JOB,
                                0, /* Not sending archive text msgs */
                                AL_SEND_RATIO,
                                GetDlgItem(hDlg, AL_PROGRESS_NUMBER),
                                0);
 GetDlgItemText(hDlg, AL_INPUT_FILES, input_name, 128);
#if defined (ZIP)
 hList = newALListPkTools(hMonitor, AL_DEFAULT, AL_DEFAULT, AL_DEFAULT);
#else
 hList = newALListGlTools(hMonitor, AL_DEFAULT);
#endif
 ALEntryListAddWildCardFiles(hList, input_name, 0);
 /*
   At this point, I have a list of files that I am going to process.  But
   I need to know the total size of the job in order for my monitor to
   work properly.  Unfortunately, I don't have a shortcut function
   to do this.  Instead, I have to open each file, get the size, and add
   it to my total.  After I have gone through the entire list, I should
   have an accurate total for the number of bytes that are going to
   be processed for the entire list.
 */
 JobSize = 0;
 for (hEntry = ALEntryListGetFirstEntry(hList);
  hEntry;
  hEntry = ALEntryGetNextEntry(hEntry)) {
  hALStorage hFile;
  long ObjectSize;

  hFile = ALEntryGetStorage(hEntry);
  ALStorageOpen(hFile);                                                        /* I have to open the file to get its size */
  ALStorageClose(hFile);
  ObjectSize = ALStorageGetSize(hFile);
  if (ObjectSize > 0)
   JobSize += ObjectSize;
 }
 /*
   During the processing itself, I have to update the monitor after each
   and every file is completed with a new JobSoFar total.  This loop
   processes each file, then updates the stats.
 */
 wsprintf(temp, "%ld", JobSize);
 SetDlgItemText(hDlg, AL_PROGRESS_TEXT, temp);
 JobSoFar = 0;
 ALMonitorSetJobSize(hMonitor, JobSize);
 ALMonitorSetJobSoFar(hMonitor, JobSoFar);
 for (hEntry = ALEntryListGetFirstEntry(hList);
  hEntry;
  hEntry = ALEntryGetNextEntry(hEntry)) {
  long ObjectSize;
  int c;

  hFile = ALEntryGetStorage(hEntry);
  ObjectSize = ALStorageGetSize(hFile);
  if (ObjectSize > 0) {
   ALMonitorSetObjectSize(hMonitor, ObjectSize);
   ALMonitorSetObjectStart(hMonitor, 0);
   ALStorageSetMonitor(hFile, hMonitor);
   SetDlgItemText(hDlg, AL_FILE_NAME, ALStorageGetName(hFile));
   ALStorageOpen(hFile);                                                       /* I have to open the file to get its size */
   for (c = ALStorageReadChar(hFile);
    c >= 0;
    c = ALStorageReadChar(hFile))
    if (ALStorageGetStatusCode(hFile) < 0)
     break;
    ;
    ALStorageClose(hFile);
    if (ALStorageGetStatusCode(hFile) < 0)
     break;
    ALStorageSetMonitor(hFile, 0);
    hFile = 0;
    JobSoFar += ObjectSize;
    ALMonitorSetJobSoFar(hMonitor, JobSoFar);
  }
 }
 hFile = 0;
 SetDlgItemText(hDlg, AL_FILE_NAME, "<Done>");
 if (hList)
  deleteALEntryList(hList);
 if (hMonitor)
  deleteALMonitor(hMonitor);
}

/* The about box. */

BOOL AL_EXPORT CALLBACK AboutDialogProc(HWND hDlg,
                                        UINT message,
                                        WPARAM wParam,
                                        LPARAM lParam)
{
 RECT rc;

 AL_UNUSED_PARAMETER(lParam);
 switch (message) {
 case WM_INITDIALOG :
  GetWindowRect(hDlg, &rc);
  SetWindowPos(hDlg,
               0,
               ((GetSystemMetrics(SM_CXSCREEN) - (rc.right - rc.left)) / 2),
               ((GetSystemMetrics(SM_CYSCREEN) - (rc.bottom - rc.top)) / 2),
               0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
  break;
 case WM_COMMAND :
  switch (wParam) {
  case IDOK :
  case AL_EXIT :
  case WM_QUIT :
  case WM_DESTROY :
   EndDialog(hDlg, TRUE);
   return TRUE;

  }
  break;
 }
 return FALSE;
}

/*
  WinMain first registers our class.  It then creates the dialog, and
  starts the message pump.
*/
int AL_WIN_MAIN_FAR PASCAL WinMain(HINSTANCE instance,
                                   HINSTANCE previous_instance,
                                   LPSTR cmd_line,
                                   int cmd_show)
{
 AL_UNUSED_PARAMETER(cmd_line);
 AL_UNUSED_PARAMETER(cmd_show);
 hInstance = instance;
 if (previous_instance == 0)
  iInstanceNumber = 0;
#if !defined (AL_FLAT_MODEL)
 else {
  GetInstanceData(previous_instance,
                  (PBYTE) (void _near *) &iInstanceNumber,
                  sizeof iInstanceNumber);
 iInstanceNumber++;
}
#endif

 Ctl3dRegister(instance);
 Ctl3dAutoSubclass(instance);
 DialogBox(instance, "ALMainDialog", 0, (DLGPROC) MainDialogProc);
 Ctl3dUnregister(instance);
 return 0;
}

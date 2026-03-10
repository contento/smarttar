/*= Archive Library v2.12 ================================================
 |
 |  EXAMPLES\EX01WIN.C
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  EX01WIN.C demonstrates the Append() function, using the overloaded
 |  version that appends files to an archive.  It has three command
 |  handlers.  One reads the contents of the current directory, so you
 |  can pick files to append.  The next command actually performs the
 |  append.  And finally, there is a function to read the contents
 |  of the archive, so you know that the append did something useful.
 |
 |  EX01WIN.C is functionally equivalent to EX01WIN.CPP.  The C version
 |  uses the translation layer functions to get the job done, but if
 |  you put them side by side, you won't see too much difference.
 |
 +- Notes ----------------------------------------------------------------
 |  #define AL_USING_DLL if your are linking to an Archive Library 16-bit
 |  import library.
 |  #define ZIP if you wish to create WIN00.ZIP.
 |
 +- Functions ------------------------------------------------------------
 |  ALArchiveAppend()
 |  ALArchiveFillListBoxDialog()
 |  ALEntryListAddFromDialog()
 |  EditDisplay()
 |  newALExpander()
 |  deleteALExpander()
 |  ALExpanderGetNextFile()
 |
 +=======================================================================*/

#define STRICT
#include <windows.h>
#include <stdarg.h>

#define AL_USING_DLL
#include "arclib.h"
#include "algauge.h"
#include "wildcard.h"
#include "winmon.h"
#include "pkarc.h"
#include "glarc.h"
#include "ex01win.h"


int iInstanceNumber;
HINSTANCE hInstance;
HWND hDlgMain;
hALArchive hArchive = 0;

BOOL AL_EXPORT CALLBACK AboutDialogProc(HWND hDlg,
                                        UINT message,
                                        WPARAM wParam,
                                        LPARAM);
void SizeFramingWindow(HWND hDlg);
void ReadArchive(HWND hDlg);
void Append(HWND hDlg);

/*
  This function uses a wild card expander to dump the contents of the
  directory into a window, based on the wild cards found in the
  AL_INPUT_FILES text box.
*/

void ReadDir(HWND hDlg)
{
 char dir_mask[128];
 hALExpander hExpander;
 char AL_DLL_FAR *p;

 GetDlgItemText(hDlg, AL_DIR_MASK, dir_mask, 128);
 SendDlgItemMessage(hDlg, AL_INPUT_FILES, LB_RESETCONTENT, 0, 0);
 hExpander = newALExpander(dir_mask, 0, 0);
 while ((p = ALExpanderGetNextFile(hExpander)) != 0)
  SendDlgItemMessage(hDlg,
                     AL_INPUT_FILES,
                     LB_ADDSTRING,
                     0,
                     (LPARAM) ((LPSTR) p));
 deleteALExpander(hExpander);
}

/*
  This is the routine that appends the marked objects from the
  directory window to the archive.  We have to first create a monitor,
  then a list.  We make the entries from the list box, then append them
  to the archive.  Finally, we call the ReadArchive() function to update
  the display with the new archive information.
*/

void Append(HWND hDlg)
{
 char input_name[128];
 hALMonitor hMonitor;
 hALEntryList hList;
 WORD count;

 GetDlgItemText(hDlg, AL_ARCHIVE_NAME, input_name, 128);
 hMonitor = newALWindowsMessage(AL_MONITOR_JOB,
                                GetDlgItem(hDlg, AL_PROGRESS_TEXT),
                                AL_SEND_RATIO,
                                GetDlgItem(hDlg, AL_PROGRESS_BAR),
                                ALGaugeSetPosition);
#if defined (ZIP)
 hArchive = newALPkArchive(input_name);
 hList = newALListPkCompressFileTools(hMonitor, 9, 15, 8);
#else
 hArchive = newALGlArchive(input_name);
 hList = newALListFullTools(hMonitor);
#endif
 if (!hArchive || !hList)
  SendDlgItemMessage(hDlg,
                     AL_ARCHIVE_CONTENTS,
                     LB_ADDSTRING,
                     0,
                     (LPARAM) ((LPSTR) "Error!"));
 else {
  count = (WORD) ALEntryListMakeEntriesFromListBox(hList, hDlg, AL_INPUT_FILES);
  EditDisplay(hDlg,
              AL_DEBUG,
              "%d file%s selected\r\n",
              count,
              (LPSTR) ((count == 1) ? " is" : "s are"));
  ALArchiveAppend(hArchive, hList);
  ReadArchive(hDlg);
 }
 deleteALMonitor(hMonitor);
 if (hList)
  deleteALEntryList(hList);
 if (hArchive)
  deleteALArchive(hArchive);
 hArchive = 0;
}

/*
  This function reads the contents of the archive into a list box.  Since
  we have a dedicated function to do this, it is a pretty easy piece of
  work.
*/

void ReadArchive(HWND hDlg)
{
 char input_name[128];
 hALArchive archive;

 GetDlgItemText(hDlg, AL_ARCHIVE_NAME, input_name, 128);
#if defined (ZIP)
 archive = newALPkArchive(input_name);
#else
 archive = newALGlArchive(input_name);
#endif
 if (archive) {
  ALArchiveFillListBox(archive, hDlg, AL_ARCHIVE_CONTENTS);
  deleteALArchive(archive);
 }
 return;
}

/*
  In this program, we have a main window, but it is just a dummy framing
  window for the dialog box.  This dialog box proc is where all the action
  takes place.  It handles input keystrokes and button presses from the
  dialog.
*/

BOOL AL_EXPORT CALLBACK MainDialogProc(HWND hDlg,
                                       UINT message,
                                       WPARAM wParam,
                                       LPARAM lParam)
{
 char buf[81];

 AL_UNUSED_PARAMETER(lParam);

 switch (message) {

/*
  Upon initialization, we set the window title, the archive name, and
  the wild cards that will be used to read the directory.
*/
 case WM_INITDIALOG :
  SizeFramingWindow(hDlg);
  wsprintf(buf, "Windows Example 01 <%d>", iInstanceNumber);
  SetWindowText(hDlg, buf);
#if defined (ZIP)
  wsprintf(buf, "WIN%02d.ZIP", iInstanceNumber);
#else
  wsprintf(buf, "WIN%02d.GAL", iInstanceNumber);
#endif
  SetDlgItemText(hDlg, AL_ARCHIVE_NAME, buf);
  SetDlgItemText(hDlg, AL_DIR_MASK, "*.CPP");
  return (TRUE);

/*
  The commands this guy process are mostly related to button presses.
  Each of the handlers winds up being called from somewhere in here.
*/
 case WM_COMMAND :
  switch (wParam) {

/*
  We get the IDOK message when someone presses the enter key while in the
  dialog.  Depending on where the focus is, we either read the directory
  or read the archive contents into a list box.
*/
  case IDOK :                                                                  /*User has pressed enter*/
   if (GetFocus() == GetDlgItem(hDlg, AL_DIR_MASK))
    ReadDir(hDlg);
   else if (GetFocus() == GetDlgItem(hDlg, AL_ARCHIVE_NAME))
    ReadArchive(hDlg);
   break;
  case AL_APPEND :

/*
  The append button makes the append happen.  Note the tricky bit here,
  we have to make sure the archive isn't already in the process of having
  an append done.  If it is in progress, we just blow off this request.
*/
   if (!hArchive)
    Append(hDlg);
   return TRUE;

/* Pressing this button reads the archive contents into a list box. */

  case AL_READ_ARCHIVE :
   ReadArchive(hDlg);
   return TRUE;

/*
  Pressing this button expands the wild cards in the text box into
  a list of file names, ready for selection.
*/
  case AL_READ_DIR :
   ReadDir(hDlg);
   return TRUE;

/*
  If the user wants to quit, we first check to see if an append() is in
  progress.  If it is, we do our best to abort the current append,
  but we don't let the user exit just yet.

  If no append is already in progress, we just initiate the shutdown
  process, by sending the destroy command to the framing window and
  destroying this dialog.
*/
  case AL_EXIT :
  case WM_QUIT :
  case WM_DESTROY :
   if (hArchive) {
    hALStorage hFile = ALArchiveGetStorage(hArchive);
    ALStorageSetError(hFile, AL_USER_ABORT, "User pressed abort key");
    FlashWindow(GetParent(hDlg), 1);
    FlashWindow(GetParent(hDlg), 0);
    return FALSE;
   }
   PostMessage(GetParent(hDlg), WM_DESTROY, 0, 0);
   DestroyWindow(hDlgMain);
   hDlgMain = 0;
   return TRUE;

/*
  Aborting this job is simply a matter of setting the error status
  for the archive object.  Next time he comes up for air, he will
  abort.
*/
  case AL_ABORT :
   if (hArchive) {
    hALStorage hFile = ALArchiveGetStorage(hArchive);
    ALStorageSetError(hFile, AL_USER_ABORT, "User pressed abort key");
   }
   return TRUE;
  }
  break;
 }
 return FALSE;
}

/* The about box. */

BOOL AL_EXPORT CALLBACK
AboutDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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
               0,
               0,
               SWP_NOSIZE | SWP_NOACTIVATE);
  break;
 case WM_COMMAND :
  switch (wParam) {
  case IDOK :
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
  The main window is nothing more than a shell that exists to manage
  the dialog box, take menu commands, and process accelerator keys.
*/
LONG AL_EXPORT CALLBACK MainWndProc(HWND hWnd,
                                    UINT message,
                                    WPARAM wParam,
                                    LPARAM lParam)
{
 switch (message) {
 case WM_COMMAND :
  switch (wParam) {
  case AL_GOTO_INPUT :
   if (hDlgMain)
    SetFocus(GetDlgItem(hDlgMain, AL_DIR_MASK));
   break;
  case AL_GOTO_OUTPUT :
   if (hDlgMain)
    SetFocus(GetDlgItem(hDlgMain, AL_ARCHIVE_NAME));
   break;
  case AL_ABOUT :
   DialogBox(hInstance, "ALAboutDialog", 0, (DLGPROC) AboutDialogProc);
   return TRUE;
  case AL_EXIT :
   PostQuitMessage(0);
  }
  break;
 case WM_SETFOCUS :
  SetFocus(hDlgMain);                                                          /* insure that the Dialog Box has the focus */
  break;
 case WM_DESTROY :
  PostQuitMessage(0);
  break;
 }
 return DefWindowProc(hWnd, message, wParam, lParam);
}

/*
  WinMain calls the initialization routine for the progress gauge and
  registers our class  It then creates the dialog, and starts the
  message pump.
*/

int AL_WIN_MAIN_FAR PASCAL WinMain(HINSTANCE instance,
                                   HINSTANCE previous_instance,
                                   LPSTR cmd_line,
                                   int nCmdShow)
{
 HWND hWnd;
 FARPROC lpfn;
 MSG msg;
 HACCEL hAccel;

 AL_UNUSED_PARAMETER(cmd_line);

 hInstance = instance;
 if (!ALGaugeInit(instance, previous_instance))
  return (FALSE);
 if (previous_instance == 0) {
  WNDCLASS wc;
  wc.style = 0;
  wc.lpfnWndProc = (WNDPROC) MainWndProc;
  wc.cbClsExtra = 0;
  wc.cbWndExtra = 0;
  wc.hInstance = hInstance;
  wc.hIcon = LoadIcon(hInstance, "ALIcon");
  wc.hCursor = LoadCursor(0, IDC_ARROW);
  wc.hbrBackground =(HBRUSH) GetStockObject(WHITE_BRUSH);
  wc.lpszMenuName = "ALMenu";
  wc.lpszClassName = "ALClass";
  if (!RegisterClass(&wc))
   return FALSE;
  iInstanceNumber = 0;
 }
#if !defined (AL_FLAT_MODEL)
 else {
  GetInstanceData(previous_instance,
                  (PBYTE) (void _near *) &iInstanceNumber,
                  sizeof iInstanceNumber);
 iInstanceNumber++;
}
#endif
 hWnd = CreateWindow("ALClass",
                     "Windows Example 01",
                     WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
                     CW_USEDEFAULT,
                     0, 0, 0, 0, 0, hInstance, 0);
 lpfn = MakeProcInstance((FARPROC) MainDialogProc, hInstance);
 hDlgMain = CreateDialog(hInstance, "ALMainDialog", hWnd, (DLGPROC) lpfn);
 ShowWindow(hWnd, nCmdShow);
 ShowWindow(hDlgMain, SW_SHOW);
 hAccel = LoadAccelerators(hInstance, "ALAccelerator");
 while (GetMessage(&msg, 0, 0, 0)) {
  HWND hWndAccel = GetActiveWindow();
  if (!(hWndAccel && TranslateAccelerator(hWndAccel, hAccel, &msg))) {
  if (hDlgMain == NULL || !IsDialogMessage(hDlgMain, &msg)) {
   TranslateMessage(&msg);
   DispatchMessage(&msg);
  }
 }
}
return (msg.wParam);
}

/*
  I need this routine for one purpose only.  After the dialog is loaded,
  it calls this routine, which adjusts the size of the framing window to
  fit exactly around the dialog box.  It also moves it to the center of
  the screen.
*/

void SizeFramingWindow(HWND hDlg)
{
 RECT rect;
 int y;
 int x;

 GetWindowRect(hDlg, &rect);
 y = GetSystemMetrics(SM_CYSCREEN);
 y -= (rect.bottom - rect.top);
 y -= GetSystemMetrics(SM_CYMENU) + GetSystemMetrics(SM_CYCAPTION);
 y /= 2;
 x = GetSystemMetrics(SM_CXSCREEN);
 x -= rect.right - rect.left;
 x /= 2;
 SetWindowPos(GetParent(hDlg), 0, x, y,
              rect.right - rect.left,
              rect.bottom - rect.top + GetSystemMetrics(SM_CYMENU) + GetSystemMetrics(SM_CYCAPTION),
              SWP_NOZORDER);
}

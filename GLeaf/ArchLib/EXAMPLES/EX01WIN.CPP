/*= Archive Library v2.12 ================================================
 |
 |  EXAMPLES\EX01WIN.CPP
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  EX01WIN.CPP demonstrates the Append() function, using the overloaded
 |  version that appends files to an archive.  It has three command
 |  handlers.  One reads the contents of the current directory, so you
 |  can pick files to append.  The next command actually performs the
 |  append.  And finally, there is a function to read the contents
 |  of the archive, so you know that the append did something useful.
 |
 +- Notes ----------------------------------------------------------------
 |  #define AL_USING_DLL if your are linking to an Archive Library 16-bit
 |  import library.
 |  #define ZIP if you wish to create WIN00.ZIP.
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

#define STRICT
#include <windows.h>
#include <stdarg.h>

#define AL_USING_DLL
#include "al.h"
#include "ex01win.h"

int NEAR iInstanceNumber;
HINSTANCE hInstance;
HWND hDlgMain;
ALArchive *pArchive = 0;

BOOL AL_EXPORT CALLBACK AboutDialogProc(HWND, UINT, WPARAM, LPARAM);
void SizeFramingWindow(HWND hDlg);
void ReadArchive(HWND hDlg);

// This function uses a wild card expander to dump the contents of the
// directory into a window, based on the wild cards found in the
// AL_INPUT_FILES text box.

void ReadDir(HWND hDlg)
{
 char dir_mask[128];

 GetDlgItemText(hDlg, AL_DIR_MASK, dir_mask, 128);
 SendDlgItemMessage(hDlg, AL_INPUT_FILES, LB_RESETCONTENT, 0, 0);
 ALWildCardExpander files(dir_mask);
 char AL_DLL_FAR *p;
 while ((p = files.GetNextFile()) != 0)
  SendDlgItemMessage(hDlg,
                     AL_INPUT_FILES,
                     LB_ADDSTRING,
                     0,
                     (LPARAM) ((LPSTR) p));
}

// This is the routine that appends the marked objects from the
// directory window to the archive.  We have to first create a monitor,
// then a list.  We make the entries from the list box, then append them
// to the archive.  Finally, we call the ReadArchive() function to update
// the display with the new archive information.

void Append(HWND hDlg)
{
 char input_name[128];

 GetDlgItemText(hDlg, AL_ARCHIVE_NAME, input_name, 128);
 ALWindowsMessage monitor(AL_MONITOR_JOB,
                          GetDlgItem(hDlg, AL_PROGRESS_TEXT),
                          AL_SEND_RATIO,
                          GetDlgItem(hDlg, AL_PROGRESS_BAR),
                          ALGaugeSetPosition);
#if defined (ZIP)
 pArchive = new ALPkArchive(input_name);
 ALEntryList list(&monitor, PkTools());
#else
 pArchive = new ALGlArchive(input_name);
 ALEntryList list(&monitor, GlTools());
#endif
 if (!pArchive) {
  SendDlgItemMessage(hDlg,
                     AL_ARCHIVE_CONTENTS,
                     LB_ADDSTRING,
                     0,
                     (LPARAM) ((LPSTR) "Error!"));
  return;
 }
 WORD count = (WORD) list.MakeEntriesFromListBox(hDlg,
                                                 AL_INPUT_FILES);
                                                 EditDisplay(hDlg,
                                                 AL_DEBUG,
                                                 "%d file%s selected\r\n",
                                                 count,
                                                 (count == 1 ? " is" : "s are"));
 pArchive->Append(list);
 EditDisplay(hDlg, AL_DEBUG, "%s\r\n", pArchive->mStatus.GetStatusDetail());
 ReadArchive(hDlg);
 delete pArchive;
 pArchive = 0;
}

// This function reads the contents of the archive into a list box.  Since
// we have a dedicated function to do this, it is a pretty easy piece of
// work.

void ReadArchive(HWND hDlg)
{
 char input_name[128];

 GetDlgItemText(hDlg, AL_ARCHIVE_NAME, input_name, 128);
#if defined (ZIP)
 ALPkArchive archive(input_name);
#else
 ALGlArchive archive(input_name);
#endif
 archive.FillListBox(hDlg, AL_ARCHIVE_CONTENTS);
 return;
}

// In this program, we have a main window, but it is just a dummy framing
// window for the dialog box.  This dialog box proc is where all the action
// takes place.  It handles input keystrokes and button presses from the
// dialog.

BOOL AL_EXPORT CALLBACK MainDialogProc(HWND hDlg,
                                       UINT message,
                                       WPARAM wParam,
                                       LPARAM)
{
 switch (message) {

// Upon initialization, we set the window title, the archive name, and
// the wild cards that will be used to read the directory.

 case WM_INITDIALOG :
  SizeFramingWindow(hDlg);
  char buf[81];
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

// The commands this guy process are mostly related to button presses.
// Each of the handlers winds up being called from somewhere in here.

 case WM_COMMAND :
  switch (wParam) {

   // We get the IDOK message when someone presses the enter key while in the
   // dialog.  Depending on where the focus is, we either read the directory
   // or read the archive contents into a list box.

  case IDOK :                                                                  //User has pressed enter
   if (GetFocus() == GetDlgItem(hDlg, AL_DIR_MASK))
    ReadDir(hDlg);
   else if (GetFocus() == GetDlgItem(hDlg, AL_ARCHIVE_NAME))
    ReadArchive(hDlg);
   break;

   // The append button makes the append happen.  Note the tricky bit here,
   // we have to make sure the archive isn't already in the process of having
   // an append done.  If it is in progress, we just blow off this request.

  case AL_APPEND :
   if (!pArchive)
    Append(hDlg);
   return TRUE;

   // Pressing this button reads the archive contents into a list box.

  case AL_READ_ARCHIVE :
   ReadArchive(hDlg);
   return TRUE;

   // Pressing this button expands the wild cards in the text box into
   // a list of file names, ready for selection.

  case AL_READ_DIR :
   ReadDir(hDlg);
   return TRUE;

   // If the user wants to quit, we first check to see if an append() is in
   // progress.  If it is, we do our best to abort the current append,
   // but we don't let the user exit just yet.

   // If no append is already in progress, we just initiate the shutdown
   // process, by sending the destroy command to the framing window and
   // destroying this dialog.

  case AL_EXIT :
  case WM_QUIT :
  case WM_DESTROY :
   if (pArchive) {
    pArchive->GetStorageObject()->mStatus.SetError(AL_USER_ABORT, "User pressed abort key");
    FlashWindow(GetParent(hDlg), 1);
    FlashWindow(GetParent(hDlg), 0);
    return FALSE;
   }
   PostMessage(GetParent(hDlg), WM_DESTROY, 0, 0);
   DestroyWindow(hDlgMain);
   hDlgMain = 0;
   return TRUE;

   // Aborting this job is simply a matter of setting the error status
   // for the archive object.  Next time he comes up for air, he will
   // abort.

  case AL_ABORT :
   if (pArchive)
    pArchive->GetStorageObject()->mStatus.SetError(AL_USER_ABORT, "User pressed abort key");
  return TRUE;
  }
  break;
 }
 return FALSE;
}

// The about box.

BOOL AL_EXPORT CALLBACK
AboutDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM)
{
 switch (message) {
 case WM_INITDIALOG :
  RECT rc;
  GetWindowRect(hDlg, &rc);
  SetWindowPos(hDlg,
               NULL,
               (short int) ((GetSystemMetrics(SM_CXSCREEN) - (rc.right - rc.left)) / 2),
               (short int) ((GetSystemMetrics(SM_CYSCREEN) - (rc.bottom - rc.top)) / 2),
               0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
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

// The main window is nothing more than a shell that exists to manage
// the dialog box, take menu commands, and process accelerator keys.

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
  case AL_ABOUT : {
   FARPROC lpfnAboutDlgProc = MakeProcInstance((FARPROC) AboutDialogProc, hInstance);
   DialogBox(hInstance, "ALAboutDialog", 0, (DLGPROC) lpfnAboutDlgProc);
   (void) FreeProcInstance(lpfnAboutDlgProc);
  }
  return TRUE;
 case AL_EXIT :
  PostQuitMessage(0);
 }
  break;
 case WM_SETFOCUS :
  SetFocus(hDlgMain);                                                          // insure that the Dialog Box has the focus
  break;
 case WM_DESTROY :
  PostQuitMessage(0);
  break;
 }
 return DefWindowProc(hWnd, message, wParam, lParam);
}

// WinMain calls the initialization routine for the progress gauge and
// registers our class.  It then creates the dialog, and starts the
// message pump.

int AL_WIN_MAIN_FAR PASCAL WinMain(HINSTANCE instance,
                                   HINSTANCE previous_instance,
                                   LPSTR,
                                   int nCmdShow)
{
 hInstance = instance;
 if (!ALGaugeInit(instance, previous_instance))
  return (FALSE);
 if (previous_instance == 0) {
  WNDCLASS wc;
  wc.style = 0;
  wc.lpfnWndProc = MainWndProc;
  wc.cbClsExtra = 0;
  wc.cbWndExtra = 0;
  wc.hInstance = instance;
  wc.hIcon = LoadIcon(hInstance, "ALIcon");
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground =(HBRUSH) GetStockObject(WHITE_BRUSH);
  wc.lpszMenuName = "ALMenu";
  wc.lpszClassName = "ALClass";
  if (!RegisterClass(&wc))
   return FALSE;
  iInstanceNumber = 0;
 } else {
#ifndef AL_FLAT_MODEL
  GetInstanceData(previous_instance,
                  (PBYTE) (void _near *) &iInstanceNumber,
                  sizeof iInstanceNumber);
#endif
  iInstanceNumber++;
 }
 HWND hWnd = CreateWindow("ALClass",
                          "Windows Example 01",
                          WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
                          CW_USEDEFAULT, 0, 0, 0, NULL, NULL, hInstance, NULL);

 FARPROC lpfn = MakeProcInstance((FARPROC) MainDialogProc, hInstance);
 hDlgMain = CreateDialog(hInstance, "ALMainDialog", hWnd, (DLGPROC) lpfn);
 ShowWindow(hWnd, (short) nCmdShow);
 ShowWindow(hDlgMain, SW_SHOW);
 MSG msg;
 HACCEL hAccel = LoadAccelerators(hInstance, "ALAccelerator");
 while (GetMessage(&msg, NULL, NULL, NULL)) {
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

// I need this routine for one purpose only.  After the dialog is loaded,
// it calls this routine, which adjusts the size of the framing window to
// fit exactly around the dialog box.  It also moves it to the center of
// the screen.

void SizeFramingWindow(HWND hDlg)
{
 RECT rect;

 GetWindowRect(hDlg, &rect);
 int y = GetSystemMetrics(SM_CYSCREEN);
 y -= (rect.bottom - rect.top);
 y -= GetSystemMetrics(SM_CYMENU) + GetSystemMetrics(SM_CYCAPTION);
 y /= 2;
 int x = GetSystemMetrics(SM_CXSCREEN);
 x -= rect.right - rect.left;
 x /= 2;
 SetWindowPos(GetParent(hDlg), NULL, (short) x, (short) y,
              (short int) (rect.right - rect.left),
              (short int) (rect.bottom - rect.top +
              GetSystemMetrics(SM_CYMENU) + GetSystemMetrics(SM_CYCAPTION)),
              SWP_NOZORDER);
}

/*= Archive Library v2.12 ================================================
 |
 |  EXAMPLES\EX15WIN.CPP
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  EX15WIN.CPP is a grab bag that uses several of the archive status and
 |  informational functions.  You open an archive by clicking on it from a
 |  list box, then see all the information regarding that archive.
 |
 |  EX15WIN.CPP checks for duplicate entries in an archive.  You might
 |  have to fiddle with the examples here in order to get one with a
 |  duplicate entry, because we try to avoid that.  I created a duplicate
 |  entry by using a binary file editor and just changing the name of an
 |  entry stored at the end of the archive.
 |
 +- Notes ----------------------------------------------------------------
 |  #define AL_USING_DLL if your are linking to an Archive Library 16-bit
 |  import library.
 |  #define ZIP if you wish to create WIN00.ZIP.
 |
 +- Functions ------------------------------------------------------------
 |  ALArchiveBase::FillListBox()
 |  ALArchiveBase::ReadDirectory()
 |  ALEntry::Duplicate()
 |
 +=======================================================================*/

#define STRICT
#include <windows.h>
#include <stdarg.h>

#define AL_USING_DLL
#include "al.h"
#include "ex15win.h"

HINSTANCE hInstance;
HWND hDlgMain;

void SizeFramingWindow(HWND hDlg);
void ReadDir(HWND hDlg);
void UpdateArchiveInfo(HWND HDlg);

// Like most of the other examples, this program uses a dialog box as
// a main window.  This is the dialog procedure for that window, and
// it handles all of the user interface, such as buttons, text boxes,
// and keystrokes.  There is a framing window around this dialog, but
// it doesn't do much of anything.

BOOL AL_EXPORT CALLBACK MainDialogProc(HWND hDlg,
                                       UINT message,
                                       WPARAM wParam,
                                       LPARAM lParam)
{
 switch (message) {

 // The initialization function fills the list box with the names of all the
 // archives in this directory.

 case WM_INITDIALOG :
  SizeFramingWindow(hDlg);
  SetWindowText(hDlg, "Windows Example 15");
#if defined (ZIP)
  SetDlgItemText(hDlg, AL_DIR_MASK, "*.ZIP");
#else
  SetDlgItemText(hDlg, AL_DIR_MASK, "*.GAL");
#endif
  ReadDir(hDlg);
  return (TRUE);

  // WM_COMMAND handles all the button presses and keyboard commands
  // that come in on the dialog box.

 case WM_COMMAND :
  switch (LOWORD(wParam)) {

   // If you double click on a file name in the list box, we treat
   // it the same as if you pressed enter.  It goes out and gets the
   // archive info.

  case AL_INPUT_FILES : {
#if defined (AL_FLAT_MODEL)
    AL_UNUSED_PARAMETER(lParam);
    switch (HIWORD(wParam)) {
#else
    switch (HIWORD(lParam)) {
#endif
     case LBN_DBLCLK :
      UpdateArchiveInfo(hDlg);
      break;
      }
    }
    break;

     // If you press enter, one of two things happen.  If the focus is on
     // the list box, we get all the archive info.  If you are on the
     // text box that contains the wild card mask, pressing enter causes
     // a new directory to be read into the list box.

    case IDOK :                                                                //User has pressed enter
     if (GetFocus() == GetDlgItem(hDlg, AL_DIR_MASK))
      ReadDir(hDlg);
     else if (GetFocus() == GetDlgItem(hDlg, AL_INPUT_FILES))
      UpdateArchiveInfo(hDlg);
     break;

     // Routine exit.

    case AL_EXIT :
    case WM_QUIT :
    case WM_DESTROY :
     PostMessage(GetParent(hDlg), WM_DESTROY, 0, 0);
     DestroyWindow(hDlgMain);
     hDlgMain = 0;
     return TRUE;
  }
  break;
  }
  return FALSE;
}

 // This is the function that does most of the interesting stuff in this
 // example.  It gets the file name from the list box, opens the
 // corresponding archive, then prints out a few status items in
 // text boxes.
 //
 // After that, it goes through the archive and looks for any duplicate
 // entries.  If it finds one, it prints it out in the text box.

void UpdateArchiveInfo(HWND hDlg)
{
 char buf[128];
 int i;
 int len;

 i = (int) SendDlgItemMessage(hDlg, AL_INPUT_FILES, LB_GETCURSEL, 0, 0);
 if (i == LB_ERR)
  return;
 len = (int) SendDlgItemMessage(hDlg, AL_INPUT_FILES, LB_GETTEXTLEN, i, 0);
 if (i == LB_ERR || len < 0 || len > 127)
  return;
 SendDlgItemMessage(hDlg, AL_INPUT_FILES, LB_GETTEXT, i, (LPARAM) (LPSTR) buf);
 SendDlgItemMessage(hDlg, AL_INPUT_FILES, LB_SETSEL, 0, i);
 SetDlgItemText(hDlg, AL_ARCHIVE_NAME, buf);
#if defined (ZIP)
 ALPkArchive archive(buf);
#else
 ALGlArchive archive(buf);
#endif
 archive.FillListBox(hDlg, AL_ARCHIVE_LISTING);
 char version[10];
 wsprintf(version, "%04x", archive.GetVersion());
 SetDlgItemText(hDlg, AL_ARCHIVE_VERSION, version);
 ALEntryList list;
 archive.ReadDirectory(list);
 ALEntry *entry = list.GetFirstEntry();
 while (entry) {
  if (entry->Duplicate(list))
   break;
  entry = entry->GetNextEntry();
 }
 if (entry == 0)
  SetDlgItemText(hDlg, AL_DUPLICATE_ENTRY_NAME, "None");
 else {
  SetDlgItemText(hDlg,
   AL_DUPLICATE_ENTRY_NAME,
   (LPSTR) entry->mpStorageObject->mName);
  archive.mStatus.SetError(AL_DUPLICATE_ENTRY, "Duplicate entry in archive!");
 }
 SetDlgItemInt(hDlg, AL_ARCHIVE_STATUS_CODE, (int) archive.mStatus, 1);
 SetDlgItemText(hDlg, AL_ARCHIVE_STATUS_STRING, archive.mStatus.
   GetStatusString());
 SetDlgItemText(hDlg, AL_ARCHIVE_STATUS_DETAIL, archive.mStatus.
   GetStatusDetail());
 SetFocus(GetDlgItem(hDlg, AL_EXIT));
}

// This function reads a list of wild card files into a list box.
// Usually this will be an expansion of "*.GAL", so that it will
// show us a list of archives in the current directory.

void ReadDir(HWND hDlg)
{
 char buf[128];

 GetDlgItemText(hDlg, AL_DIR_MASK, buf, 128);
 SendDlgItemMessage(hDlg, AL_INPUT_FILES, LB_RESETCONTENT, 0, 0);
 ALWildCardExpander files(buf);
 char AL_DLL_FAR *p;
 while ((p = files.GetNextFile()) != 0)
  SendDlgItemMessage(hDlg,
                     AL_INPUT_FILES,
                     LB_ADDSTRING,
                     0,
                     (LPARAM) ((LPSTR) p));
 SetFocus(GetDlgItem(hDlg, AL_INPUT_FILES));
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
  case AL_EXIT :
   PostQuitMessage(0);
  }
  break;
 case WM_SETFOCUS :
  SetFocus(hDlgMain);                                                         // insure that the Dialog Box has the focus
  break;
 case WM_DESTROY :
  PostQuitMessage(0);
  break;
 }
 return DefWindowProc(hWnd, message, wParam, lParam);
}

// WinMain calls registers our class, creates the dialog, and starts the
// message pump.

int AL_WIN_MAIN_FAR PASCAL WinMain(HINSTANCE instance,
                                   HINSTANCE previous_instance,
                                   LPSTR,
                                   int nCmdShow)
{
 hInstance = instance;
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
 }
 HWND hWnd = CreateWindow("ALClass",
                          "Windows Example 15",
                          WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
                          CW_USEDEFAULT, 0, 0, 0, NULL, NULL, hInstance, NULL);

 FARPROC lpfn = MakeProcInstance((FARPROC) MainDialogProc, hInstance);
 hDlgMain = CreateDialog(hInstance, "ALMainDialog", hWnd, (DLGPROC) lpfn);
 ShowWindow(hWnd, (short) nCmdShow);
 ShowWindow(hDlgMain, SW_SHOW);
 MSG msg;
 while (GetMessage(&msg, NULL, NULL, NULL)) {
  if (hDlgMain == NULL || !IsDialogMessage(hDlgMain, &msg)) {
   TranslateMessage(&msg);
   DispatchMessage(&msg);
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
              (short int) (rect.bottom - rect.top + GetSystemMetrics(SM_CYMENU) +
               GetSystemMetrics(SM_CYCAPTION)),
              SWP_NOZORDER);
}

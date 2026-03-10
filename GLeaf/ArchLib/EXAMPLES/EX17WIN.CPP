/*= Archive Library v2.12 ================================================
 |
 |  EXAMPLES\EX17WIN.CPP
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  EX17WIN.CPP demonstrates a few of the ALEntryList manipulation
 |  functions.  It lets you create a list, delete duplicates, and
 |  delete a list.
 |
 +- Notes ----------------------------------------------------------------
 |  #define AL_USING_DLL if your are linking to an Archive Library 16-bit
 |  import library.
 |  #define ZIP if you wish to create WIN00.ZIP.
 |
 +- Functions ------------------------------------------------------------
 |  ALEntryList::FillListBox()
 |  ALEntryList::UnmarkDuplicates()
 |
 +=======================================================================*/

#define STRICT
#include <windows.h>
#include <stdarg.h>

#define AL_USING_DLL
#include "al.h"
#include "ex17win.h"

HINSTANCE hInstance;
HWND hDlgMain;
ALEntryList *pList = 0;

BOOL AL_EXPORT CALLBACK AboutDialogProc(HWND, UINT, WPARAM, LPARAM);
void SizeFramingWindow(HWND hDlg);
void MakeList(HWND hDlg);
void DeleteList(HWND hDlg);
void DeleteDuplicates(HWND hDlg);

// In this program, we have a main window, but it is just a dummy framing
// window for the dialog box.  This dialog box is where all the action
// takes place.  It handles input keystrokes and button presses from the
// dialog.

BOOL AL_EXPORT CALLBACK MainDialogProc(HWND hDlg,
                                       UINT message,
                                       WPARAM wParam,
                                       LPARAM)
{
 switch (message) {

 // At initialization time, we just initialize the title bar, the input
 // text box, and center/size the main window.

 case WM_INITDIALOG :
  SizeFramingWindow(hDlg);
  SetWindowText(hDlg, "Windows Example 17");
  SetDlgItemText(hDlg, AL_DIR_MASK, "*.CPP");
  return (TRUE);

  // Most of the activity in this program takes place in response to button
  // presses in the dialog.  Those come through as WM_COMMAND messages,
  // and are handled right here.

 case WM_COMMAND :
  switch (wParam) {
  case AL_ABOUT : {
    FARPROC lpfnAboutDlgProc = MakeProcInstance((FARPROC) AboutDialogProc, hInstance);
    DialogBox(hInstance, "ALAboutDialog", 0, (DLGPROC) lpfnAboutDlgProc);
    (void) FreeProcInstance(lpfnAboutDlgProc);
  }
  return TRUE;

  // These are the handlers for the three button presses.

 case AL_CREATE_LIST :
  MakeList(hDlg);
  return TRUE;
 case AL_DELETE_LIST :
  DeleteList(hDlg);
  return TRUE;
 case AL_DELETE_DUPLICATES :
  DeleteDuplicates(hDlg);
  return TRUE;
 case AL_EXIT :
 case WM_QUIT :
 case WM_DESTROY :
  if (pList) {
   delete pList;
   pList = 0;
  }
  PostMessage(GetParent(hDlg), WM_DESTROY, 0, 0);
  DestroyWindow(hDlgMain);
  hDlgMain = 0;
  return TRUE;

  }
  break;
 }
 return FALSE;
}

// This routine creates a archive entry list.  We create the list
// by adding wild card files from the current directory.  After
// creating the archive, we can dump its contents to the list box.
// If you want to create some duplicates, put a string like this
// in the text box: "*.*, *.BAT".

void MakeList(HWND hDlg)
{
 char dir_mask[128];

 if (pList)
  delete pList;
#if defined (ZIP)
  ALMemory temp;
  ALPkArchive archive(temp);
  pList = new ALEntryList(0, PkTools());
#else
  ALMemory temp;
  ALGlArchive archive(temp);
  pList = new ALEntryList(0, GlTools());
#endif

 GetDlgItemText(hDlg, AL_DIR_MASK, dir_mask, 128);
 pList->AddWildCardFiles(dir_mask);
 pList->FillListBox(hDlg, AL_INPUT_FILES);
}

// Deleting a list is pretty obvious.  We clear out the list box that
// contained the list.

void DeleteList(HWND hDlg)
{
 if (pList)
  delete pList;
 pList = 0;
 SendDlgItemMessage(hDlg, AL_INPUT_FILES, LB_RESETCONTENT, 0, 0);
}

// Here we delete any duplicate entries, then update the list box with the
// new, non-duplicated list.

void DeleteDuplicates(HWND hDlg)
{
 if (pList) {
  pList->UnmarkDuplicates(*pList);
  pList->DeleteUnmarked();
  pList->FillListBox(hDlg, AL_INPUT_FILES);
 }
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
  SetFocus(hDlgMain); // insure that the Dialog Box has the focus
  break;
 case WM_DESTROY :
  PostQuitMessage(0);
  break;
 }
 return DefWindowProc(hWnd, message, wParam, lParam);
}

// WinMain first registers our class.  It then creates the dialog, and
// starts the message pump.

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
                          "Windows Example 17",
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
              (short int) (rect.bottom - rect.top +
              GetSystemMetrics(SM_CYMENU) + GetSystemMetrics(SM_CYCAPTION)),
              SWP_NOZORDER);
}

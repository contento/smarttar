/*= Archive Library v2.12 ================================================
 |
 |  EXAMPLES\EX19WIN.CPP
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  EX19WIN.CPP demonstrates some of the functions in the ALName class.
 |  It lets you create an ALName object, rename it, and delete it.  After
 |  these operations, it prints out some key states on the ALName object.
 |
 +- Notes ----------------------------------------------------------------
 |  #define AL_USING_DLL if your are linking to an Archive Library 16-bit
 |  import library.
 |  #define ZIP if you wish to create WIN00.ZIP.
 |
 +- Functions ------------------------------------------------------------
 |  ALName::GetName()
 |  ALName::GetOldName()
 |  ALName::GetSafeOldName()
 |
 +=======================================================================*/

#define STRICT
#include <windows.h>
#include <stdarg.h>

#define AL_USING_DLL
#include "al.h"
#include "ex19win.h"

HINSTANCE hInstance;
HWND hDlgMain;
ALName *name = 0;

void SizeFramingWindow(HWND hDlg);

// After any of the operations on the ALName object, this routine is called
// to update the stats in the dialog box.

void Update(HWND hDlg)
{
 char buf[21];
 const char AL_DLL_FAR *p;

 int good_tag = name->GoodTag();
 wsprintf(buf, "%d", good_tag);
 SetDlgItemText(hDlg, AL_GOOD_TAG, buf);
 if (good_tag) {
  SetDlgItemText(hDlg, AL_NAME, name->GetName());
  p = name->GetOldName();
  if (!p)
   p = "<Null pointer>";
  SetDlgItemText(hDlg, AL_OLD_NAME, p);
  p = name->GetSafeOldName();
  if (p[0] == '\0')
   p = "Empty string";
  SetDlgItemText(hDlg, AL_SAFE_OLD_NAME, p);
 }
#ifdef _DEBUG
 else {
  SetDlgItemText(hDlg, AL_NAME, "<Invalid object>");
  SetDlgItemText(hDlg, AL_OLD_NAME, "<Invalid object>");
  SetDlgItemText(hDlg, AL_SAFE_OLD_NAME, "<Invalid object>");
}
#endif
}

// In this program, we have a main window, but it is just a dummy framing
// windows for the dialog box.  This dialog box proc is where all the action
// takes place.  It handles input keystrokes and button presses from the
// dialog.

BOOL AL_EXPORT CALLBACK MainDialogProc(HWND hDlg,
                                       UINT message,
                                       WPARAM wParam,
                                       LPARAM)
{
 switch (message) {

 // This sets the title bar and creates the initial copy of the ALName object.

 case WM_INITDIALOG :
  SizeFramingWindow(hDlg);
  SetWindowText(hDlg, "Windows Example 19");
  name = new ALName("Test name");
  Update(hDlg);
  return (TRUE);

  // The button presses form the dialog are passed into this routine as
  // WM_COMMAND messages.

 case WM_COMMAND :
  switch (wParam) {
  case AL_CREATE :
   name = new ALName("Test name");
   Update(hDlg);
   break;
  case AL_DELETE :
   delete name;
   Update(hDlg);
   break;
  case AL_RENAME :
   *name = "New name";
   Update(hDlg);
   break;
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

// WinMain registers our class.  It then creates the dialog, and starts the
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
                          "Windows Example 19",
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
               GetSystemMetrics(SM_CYCAPTION)), SWP_NOZORDER);
}

/*= Archive Library v2.12 ================================================
 |
 |  EXAMPLES\EX20WIN.CPP
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  EX20WIN.CPP demonstrates some of the timedate functions in ALStorage.
 |  It is a "read-only" program that doesn't modify any of the files it
 |  messes with.  It just lets you double click on a file, then see the
 |  time date stamps for that file.
 |
 +- Notes ----------------------------------------------------------------
 |  #define AL_USING_DLL if your are linking to an Archive Library 16-bit
 |  import library.
 |
 +- Functions ------------------------------------------------------------
 |  ALTimeDate::GetTimeDate()
 |  ALTimeDate::GetUnixTime()
 |
 +=======================================================================*/

#define STRICT
#include <windows.h>
#include <stdarg.h>
#include <time.h>

#define AL_USING_DLL
#include "al.h"
#include "ex20win.h"

HINSTANCE hInstance;
HWND hDlgMain;

void SizeFramingWindow(HWND hDlg);
void ReadDir(HWND hDlg);

// When the user double clicks on a filename, they call this routine,
// which creates an object to go with the file, then displays the
// time stamps for the file, in various formats.

void UpdateFileStats(HWND hDlg)
{
 char buf[128];
 int i;
 int len;
 long stamp;

 i = (int) SendDlgItemMessage(hDlg, AL_INPUT_FILES, LB_GETCURSEL, 0, 0);
 if (i == LB_ERR)
  return;
 len = (int) SendDlgItemMessage(hDlg, AL_INPUT_FILES, LB_GETTEXTLEN, i, 0);
 if (i == LB_ERR || len < 0 || len > 127)
  return;
 SendDlgItemMessage(hDlg, AL_INPUT_FILES, LB_GETTEXT, i, (LPARAM) (LPSTR) buf);
 SetDlgItemText(hDlg, AL_FILE_NAME, buf);
 ALFile file(buf);
 file.Open();
 file.Close();
 if (file.mStatus < AL_SUCCESS) {
  SetDlgItemText(hDlg, AL_TIME_STAMP, "<Error>");
  return;
 }
 wsprintf(buf, "%ld", stamp = file.mTimeDate.GetUnixTime());
 SetDlgItemText(hDlg, AL_TIME_STAMP, buf);
 SetDlgItemInt(hDlg, AL_YEARS, (int) (stamp / 365L / 24L / 60L / 60L), 0);
 struct tm tblock;
 file.mTimeDate.GetTimeDate(&tblock);
 char *p = asctime(&tblock);
 strcpy(buf, p);
 buf[24] = '\0';
 SetDlgItemText(hDlg, AL_DATE_AND_TIME, buf);
}

// In this program, we have a main window, but it is just a dummy framing
// window for the dialog box.  This dialog box proc is where all the action
// takes place.  It handles input keystrokes and button presses from the
// dialog.

BOOL AL_EXPORT CALLBACK MainDialogProc(HWND hDlg,
                                       UINT message,
                                       WPARAM wParam,
                                       LPARAM lParam)
{
 switch (message) {
 case WM_INITDIALOG :
  SizeFramingWindow(hDlg);
  SetWindowText(hDlg, "Windows Example 20");
  ReadDir(hDlg);
  return (TRUE);

 case WM_COMMAND :
  switch (LOWORD(wParam)) {

  // The only message of note here is the the double click on a file name
  // in the input file name list box.

  case AL_INPUT_FILES : {
#if defined (AL_FLAT_MODEL)
   AL_UNUSED_PARAMETER(lParam);
   switch (HIWORD(wParam)) {
#else
    switch (HIWORD(lParam)) {
#endif
    case LBN_DBLCLK :
     UpdateFileStats(hDlg);
     break;
     }
    }
    break;
   case IDOK :                                                                //User has pressed enter
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

// This is a straightforward expansion of wildcards to fill a directory
// listing box.  The same piece of code used in a zillion other examples.

void ReadDir(HWND hDlg)
{
 SendDlgItemMessage(hDlg, AL_INPUT_FILES, LB_RESETCONTENT, 0, 0);
 ALWildCardExpander files("*.*");
 char AL_DLL_FAR *p;
 while ((p = files.GetNextFile()) != 0)
  SendDlgItemMessage(hDlg,
   AL_INPUT_FILES,
   LB_ADDSTRING,
   0,
   (LPARAM) ((LPSTR) p));

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
  SetFocus(hDlgMain);                                                         // insure that the Dialog Box has the focus
  break;
 case WM_DESTROY :
  PostQuitMessage(0);
  break;
 }
 return DefWindowProc(hWnd, message, wParam, lParam);
}

// WinMain first registers our class.  It then creates the dialog, and
// starts up the message pump.

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
                          "Windows Example 20",
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
              (short int) (rect.bottom - rect.top + GetSystemMetrics(SM_CYMENU) +
               GetSystemMetrics(SM_CYCAPTION)),
              SWP_NOZORDER);
}

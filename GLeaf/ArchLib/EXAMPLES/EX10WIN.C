/*= Archive Library v2.12 ================================================
 |
 |  EXAMPLES\EX10WIN.C
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  This is a grab bag type function.  It just creates and opens
 |  disk and memory files, and demonstrates status and information
 |  routines supported by the ALStorage class.
 |
 +- Notes ----------------------------------------------------------------
 |  #define AL_USING_DLL if your are linking to an Archive Library 16-bit
 |  import library.
 |  #define ZIP if you wish to create WIN00.ZIP.
 |
 +- Functions ------------------------------------------------------------
 |  ALStorageGetStatusDetail()
 |  ALStorageGetStatusString()
 |  ALStorageGetType()
 |
 +=======================================================================*/

#define STRICT
#include <windows.h>
#include <stdarg.h>

#define AL_USING_DLL
#include "al.h"
#include "ex10win.h"

HINSTANCE hInstance;
HWND hDlgMain;

void SizeFramingWindow(HWND hDlg);
void ReadDir(HWND hDlg);
void UpdateFileInfo(HWND hDlg, hALStorage file);
void UpdateFileName(HWND hDlg);
hALStorage OpenDiskFile(HWND hDlg);
hALStorage OpenMemoryFile(HWND hDlg, int message);

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
 hALStorage file;

 switch (message) {
 /*
   Upon initialization, we set the window title, the archive name, and
   then read in the current directory into a list box.
 */
 case WM_INITDIALOG :
  SizeFramingWindow(hDlg);
  SetWindowText(hDlg, "Windows Example 10");
  ReadDir(hDlg);
  return (TRUE);
  /*
    WM_COMMAND messages are used to send button presses and keystrokes.
    We process most of them here.
  */
 case WM_COMMAND :
  switch (LOWORD(wParam)) {
   /*
     Every time a different file is selected in the list box, we write the
     filename out to the text control that holds file names.
   */
  case AL_INPUT_FILES : {
#if defined (AL_FLAT_MODEL)
    AL_UNUSED_PARAMETER(lParam);
    switch (HIWORD(wParam)) {
#else
    switch (HIWORD(lParam)) {
#endif
     case LBN_SELCHANGE :
      UpdateFileName(hDlg);
      break;
      }
     }
     break;
     /*
       This command is a response to a button press.  It opens the disk file
       corresponding to the name currently selected in the list box.  Then
       it calls a subroutine to display everything there is to know about
       the file.  Well, not everything.
     */
    case AL_OPEN_DISK_FILE :
     file = OpenDiskFile(hDlg);
     UpdateFileInfo(hDlg, file);
     deleteALStorage(file);
     break;
     /*
       Creating the memory file is very similar to the disk file.  The difference
       here is that we have to create a new object, since we don't have a memory
       object just sitting around waiting to be opened.
     */
    case AL_CREATE_WIN_MEMORY_OBJECT :
    case AL_CREATE_HUGE_MEMORY_OBJECT :
    case AL_CREATE_MEMORY_OBJECT :
     file = OpenMemoryFile(hDlg, LOWORD(wParam));
     if (file) {
      UpdateFileInfo(hDlg, file);
      deleteALStorage(file);
     }
     break;
     /*
       If the user presses the enter key, we will normally treat it as a command
       to open the disk file.
     */
    case IDOK :                                                                /* User has pressed enter */
     if (GetFocus() == GetDlgItem(hDlg, AL_FILE_NAME) ||
      GetFocus() == GetDlgItem(hDlg, AL_INPUT_FILES)) {
      file = OpenDiskFile(hDlg);
      UpdateFileInfo(hDlg, file);
      deleteALStorage(file);
     }
     break;
     /*  Normal exit routine. */
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

 /*
   Every time the user moves to a different file name in the list box,
   this routine is called.  Its job is to get the new file name, then stuff
   it into the text box that holds the file name.
 */

void UpdateFileName(HWND hDlg)
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
 SetDlgItemText(hDlg, AL_FILE_NAME, buf);
}

 /*
   When the user asks for the file info, or hits the enter key, we stuff
   the status info into a couple of text boxes, then stuff the storage
   type into another one.  Since we are opening both disk files and memory
   objects, you should be able to see two different types of things
   stuffed into the text box containing the storage type.
 */

void UpdateFileInfo(HWND hDlg, hALStorage file)
{
 SetDlgItemText(hDlg,
  AL_FILE_STATUS_STRING,
  ALStorageGetStatusString(file));
 SetDlgItemText(hDlg,
  AL_FILE_STATUS_DETAIL,
  ALStorageGetStatusDetail(file));
 SetDlgItemInt(hDlg, AL_FILE_TYPE, ALStorageGetType(file), 0);
 SetFocus(GetDlgItem(hDlg, AL_EXIT));
}

 /*
   When the program starts up, it reads the contents of the current
   directory into a list box.  The user then picks files out of the
   list box for status info.
 */
void ReadDir(HWND hDlg)
{
 hALExpander files;
 char AL_DLL_FAR *p;

 files = newALExpander("*.*", 0, AL_UPPER);
 while ((p = ALExpanderGetNextFile(files)) != 0)
  SendDlgItemMessage(hDlg,
                     AL_INPUT_FILES,
                     LB_ADDSTRING,
                     0,
                     (LPARAM) ((LPSTR) p));
 SetFocus(GetDlgItem(hDlg, AL_INPUT_FILES));
}

/*
  Before printing the status info for a particular type of file, we have
  to get the name, create an object, then open it and close it. At
  that point its structure should be completely filled out and ready
  for display.
*/

hALStorage OpenDiskFile(HWND hDlg)
{
 char buf[128];
 hALStorage file;

 GetDlgItemText(hDlg, AL_FILE_NAME, buf, 128);
 file = newALFile(buf);
 ALStorageOpen(file);
 ALStorageClose(file);
 return file;
}

/*
  If the user wants to work with a memory object instead of a disk file,
  this guy creates it, opens it and closes it.  Good status data should
  be on hand at that point.
*/

hALStorage OpenMemoryFile(HWND hDlg, int message)
{
 char buf[128];
 hALStorage file;
 long l;

 GetDlgItemText(hDlg, AL_FILE_NAME, buf, 128);
 switch (message) {
 case AL_CREATE_MEMORY_OBJECT :
  file = newALMemory(buf, 0, 0);
  break;
  #if !defined (AL_FLAT_MODEL)
  case AL_CREATE_HUGE_MEMORY_OBJECT :
   file = newALHugeMemory(buf, 0, 0);
   break;
  #endif
 case AL_CREATE_WIN_MEMORY_OBJECT :
  file = newALWinMemory(buf, 0, 0);
  break;
 default :
  return 0;
 }
 ALStorageCreate(file, - 1L);
 for (l = 0; l < 70000L; l++)
  ALStorageWriteChar(file, (char) (l % 256));
 ALStorageClose(file);
 return file;
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
  case AL_EXIT :
   PostQuitMessage(0);
  }
  break;
 case WM_SETFOCUS :
  SetFocus(hDlgMain);                                                         /* insure that the Dialog Box has the focus */
  break;
 case WM_DESTROY :
  PostQuitMessage(0);
  break;
 }
 return DefWindowProc(hWnd, message, wParam, lParam);
}

/*
  WinMain registers our class.  It then creates the dialog, and starts the
  message pump.
*/

int AL_WIN_MAIN_FAR PASCAL WinMain(HINSTANCE instance,
                                   HINSTANCE previous_instance,
                                   LPSTR lpStr,
                                   int nCmdShow)
{
 HWND hWnd;
 FARPROC lpfn;
 MSG msg;

 AL_UNUSED_PARAMETER(lpStr);

 hInstance = instance;
 if (previous_instance == 0) {
  WNDCLASS wc;
  wc.style = 0;
  wc.lpfnWndProc = (WNDPROC) MainWndProc;
  wc.cbClsExtra = 0;
  wc.cbWndExtra = 0;
  wc.hInstance = instance;
  wc.hIcon = LoadIcon(hInstance, "ALIcon");
  wc.hCursor = LoadCursor(0, IDC_ARROW);
  wc.hbrBackground =(HBRUSH) GetStockObject(WHITE_BRUSH);
  wc.lpszMenuName = "ALMenu";
  wc.lpszClassName = "ALClass";
  if (!RegisterClass(&wc))
   return FALSE;
 }
 hWnd = CreateWindow("ALClass",
                     "Windows Example 10",
                     WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
                     CW_USEDEFAULT, 0, 0, 0, 0, 0, hInstance, NULL);
 lpfn = MakeProcInstance((FARPROC) MainDialogProc, hInstance);
 hDlgMain = CreateDialog(hInstance, "ALMainDialog", hWnd, (DLGPROC) lpfn);
 ShowWindow(hWnd, (short) nCmdShow);
 ShowWindow(hDlgMain, SW_SHOW);
 while (GetMessage(&msg, 0, 0, 0)) {
  if (hDlgMain == NULL || !IsDialogMessage(hDlgMain, &msg)) {
   TranslateMessage(&msg);
   DispatchMessage(&msg);
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
 int x;
 int y;

 GetWindowRect(hDlg, &rect);
 y = GetSystemMetrics(SM_CYSCREEN);
 y -= (rect.bottom - rect.top);
 y -= GetSystemMetrics(SM_CYMENU) + GetSystemMetrics(SM_CYCAPTION);
 y /= 2;
 x = GetSystemMetrics(SM_CXSCREEN);
 x -= rect.right - rect.left;
 x /= 2;
 SetWindowPos(GetParent(hDlg), 0, (short) x, (short) y,
                        (short int) (rect.right - rect.left),
                        (short int) (rect.bottom - rect.top +
                        GetSystemMetrics(SM_CYMENU) + GetSystemMetrics(SM_CYCAPTION)),
                        SWP_NOZORDER);
}

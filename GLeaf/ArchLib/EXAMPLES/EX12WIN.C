/*= Archive Library v2.12 ================================================
 |
 |  EXAMPLES\EX12WIN.C
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  EX12WIN.C shows how several of the rename functions work, in this case
 |  on ALFile objects.  It lets you pick a file out of a list, and rename
 |  it, rename it to a backup file, and unrename it.  A little care is in
 |  order here, because those rename operations are really taking place!
 |
 |  This program is functionally equivalent to EX12WIN.CPP.  The C version
 |  uses the translation layer functions to get the job done, but if
 |  you put them side by side, you won't see too much difference.
 |
 +- Notes ----------------------------------------------------------------
 |  #define AL_USING_DLL if your are linking to an Archive Library 16-bit
 |  import library.
 |  #define ZIP if you wish to create WIN00.ZIP.
 |
 +- Functions ------------------------------------------------------------
 |  ALStorageRename()
 |  ALStorageRenameToBackup()
 |  ALStorageSeek()
 |  ALStorageUnRename()
 |
 +=======================================================================*/

#define STRICT
#include <windows.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#define AL_USING_DLL
#include "al.h"
#include "ex12win.h"

HINSTANCE hInstance;
HWND hDlgMain;

void SizeFramingWindow(HWND hDlg);
void ReadDir(HWND hDlg);
hALStorage file = 0;

void NewFile(HWND hDlg);
void RenameFile(HWND hDlg);
void RenameFileToBackup(HWND hDlg);
void UnRenameFile(HWND hDlg);
void SeekTest(HWND hDLg);

/*
  The main window in this program is a dialog box, and this is the dialog
  box procedure.  It has to handle all of the keyboard and button input
  from the user.  The most interesting inputs to this function will be
  the keyboard/mouse commands that slip in via WM_COMMAND.
*/

BOOL AL_EXPORT CALLBACK MainDialogProc(HWND hDlg,
                                       UINT message,
                                       WPARAM wParam,
                                       LPARAM lParam)
{
 switch (message) {
 /*
   First time up we size the window, set the title in the caption, then read
   the directory into the list box.  The user can pick files out of the list
   box to fiddle with.
 */
 case WM_INITDIALOG :
  SizeFramingWindow(hDlg);
  SetWindowText(hDlg, "Windows Example 12");
  ReadDir(hDlg);
  return (TRUE);

 case WM_COMMAND :
  switch (LOWORD(wParam)) {
  /*
    Every time the user moves to a different file in the list box, I create
    a new ALFile object to go with their selection.  This is the file that
    is stored in the global file handle.  All the other commands act on
    this file.
  */
  case AL_INPUT_FILES :
#if defined (AL_FLAT_MODEL)
   AL_UNUSED_PARAMETER(lParam);
   switch (HIWORD(wParam)) {
#else
   switch (HIWORD(lParam)) {
#endif
   case LBN_SELCHANGE :
    NewFile(hDlg);
    break;
   }
   break;
   /*
     The next four commands comprise the meat of the program.  Each of them is
     handled by a procedure that does all the work.
   */
   case AL_RENAME :
    RenameFile(hDlg);
    break;
   case AL_RENAME_TO_BACKUP :
    RenameFileToBackup(hDlg);
    break;
   case AL_UNRENAME :
    UnRenameFile(hDlg);
    break;
   case AL_SEEK :
    SeekTest(hDlg);
    break;
    /*
      Normally when I exit, I will have an active ALFile object that I have
      been working on.  When this is the case, I delete the object so I won't
      leave any memory hanging around.
    */
   case AL_EXIT :
   case WM_QUIT :
   case WM_DESTROY :
    PostMessage(GetParent(hDlg), WM_DESTROY, 0, 0);
    DestroyWindow(hDlgMain);
    hDlgMain = 0;
    if (file) {
     deleteALStorage(file);
     file = 0;
    }
    return TRUE;
   }
   break;
 }
 return FALSE;
}

 /*
   The only reason the seek test is in this example is that I had to throw
   in an example for the Seek() function.  Somehow it got left out of all
   the other examples, and I had to throw it in here.

   This routine tests the accuracy of ALStorageSeek() by reading in a buffer
   using the file I/O functions (and seek()) from stdio.h.  We then read in
   a buffer using the same technique for ALStorage.  If the two buffers
   are identical, the test is pronounced to be a smashing success.
 */

void SeekTest(HWND hDlg)
{
 char buf1[128];
 unsigned char buf2[128];
 size_t count;
 FILE *f;
 long size;

 if (!file)
  return;
#if defined (AL_USING_DLL) && !defined (AL_FLAT_MODEL)
  _fstrcpy(buf1, ALStorageGetName(file));
  f = fopen(buf1, "rb");
#else
  f = fopen(ALStorageGetName(file), "rb");
#endif
 if (!f) {
  SetDlgItemText(hDlg, AL_STATUS, "Couldn't open file for seek test.");
  return;
 }
 fseek(f, 0L, SEEK_END);
 size = ftell(f);
 if (size < 1) {
  SetDlgItemText(hDlg, AL_STATUS, "File to small for seek test.");
  fclose(f);
  return;
 }
 fseek(f, size / 2, SEEK_SET);
 count = fread(buf1, 1, 128, f);
 fclose(f);
 if (count == 0) {
   SetDlgItemText(hDlg, AL_STATUS, "Something went wrong in fread().");
  return;
 }
 ALStorageOpen(file);
 ALStorageSeek(file, size / 2);
 if (ALStorageReadBuffer(file, buf2, 128) != count) {
  SetDlgItemText(hDlg, AL_STATUS, "Byte count mismatch in ReadBuffer().");
  ALStorageClose(file);
  return;
 }
 ALStorageClose(file);
 if (memcmp(buf1, buf2, count) != 0) {
  SetDlgItemText(hDlg, AL_STATUS, "Data mismatch in Seek Test!");
  return;
 }
 SetDlgItemText(hDlg, AL_STATUS, "Seek test was a success.");
}

/*
  Each time I either use my arrow keys or use the mouse to select a new file
  from the list box, I call this routine.  It constructs a new ALFile
  object using the current name selected in the list box.  Note that if
  there was an old file that was being used, it is deleted.  I also
  send the name, old name, and status to the appropriate text boxes in
  the dialog.
*/

void NewFile(HWND hDlg)
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
 if (file)
  deleteALStorage(file);
 file = newALFile(buf);
 SetDlgItemText(hDlg, AL_OLD_NAME, ALStorageGetOldName(file));
 SetDlgItemText(hDlg, AL_NEW_NAME, "<None>");
 SetDlgItemText(hDlg, AL_STATUS, ALStorageGetStatusDetail(file));
}

/*
  This function is called in response to a button push.  It renames the
  current file, then updates a bunch of text boxes.
*/

void RenameFile(HWND hDlg)
{
 char buf[128];

 if (!file)
   return;
 GetDlgItemText(hDlg, AL_NEW_NAME, buf, 128);
 if (strcmp(buf, "<None>") == 0)
  strcpy(buf, "");
 ALStorageRename(file, buf, 1);
 SetDlgItemText(hDlg, AL_FILE_NAME, ALStorageGetName(file));
 SetDlgItemText(hDlg, AL_OLD_NAME, ALStorageGetOldName(file));
 SetDlgItemText(hDlg, AL_NEW_NAME, "<None>");
 SetDlgItemText(hDlg, AL_STATUS, ALStorageGetStatusDetail(file));
 ReadDir(hDlg);
}

/*
  This function is called in response to a button push.  It renames the
  current file, then updates a bunch of text boxes.
*/

void RenameFileToBackup(HWND hDlg)
{
 if (!file)
  return;
 ALStorageRenameToBackup(file, 1);
 SetDlgItemText(hDlg, AL_FILE_NAME, ALStorageGetName(file));
 SetDlgItemText(hDlg, AL_OLD_NAME, ALStorageGetOldName(file));
 SetDlgItemText(hDlg, AL_NEW_NAME, "<None>");
 SetDlgItemText(hDlg, AL_STATUS, ALStorageGetStatusDetail(file));
 ReadDir(hDlg);
}

/*
  This function is called in response to a button push.  It un-renames the
  current file, then updates a bunch of text boxes.
*/
void UnRenameFile(HWND hDlg)
{
 if (!file)
  return;
 ALStorageUnRename(file, 1);
 SetDlgItemText(hDlg, AL_FILE_NAME, ALStorageGetName(file));
 SetDlgItemText(hDlg, AL_OLD_NAME, ALStorageGetOldName(file));
 SetDlgItemText(hDlg, AL_NEW_NAME, "<None>");
 SetDlgItemText(hDlg, AL_STATUS, ALStorageGetStatusDetail(file));
 ReadDir(hDlg);
}

/*
  Every time we do a rename or something like that, we call this guy.
  He does a wild card expansion on the current directory and puts all
  of the names in the list box.  That way, if I renamed a file for
  some reason, I can see the results immediately reflected in the
  directory listing.
*/

void ReadDir(HWND hDlg)
{
 hALExpander files;
 char AL_DLL_FAR *p;

 SendDlgItemMessage(hDlg, AL_INPUT_FILES, LB_RESETCONTENT, 0, 0);
 files = newALExpander("*.*", 0, AL_MIXED);
  while ((p = ALExpanderGetNextFile(files)) != 0)
  SendDlgItemMessage(hDlg,
   AL_INPUT_FILES,
   LB_ADDSTRING,
   0,
   (LPARAM) ((LPSTR) p));
SetFocus(GetDlgItem(hDlg, AL_INPUT_FILES));
}


/*
  The main window is nothing more than a shell that exists to take care of
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
                     "Windows Example 12",
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

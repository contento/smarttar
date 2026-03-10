/*= Archive Library v2.12 ================================================
 |
 |  EXAMPLES\EX03WIN.CPP
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  EX03WIN.CPP demonstrates ALArchiveBase::Delete().  It lets you
 |  select a list of files to delete from an archive, then performs
 |  the deletion.  The list boxes are updated right afterwards so you
 |  can see the results.
 |
 |  This is a very simple Windows program.  It works by creating a modeless
 |  dialog box, then wrapping a framing window around it.  By using a framing
 |  window as the main window we are able to handle accelerator keys, menus,
 |  and icons a bit easier. Because of this approach, WinMain() and
 |  MainWndProc() are not very exciting, and have been moved down to the
 |  bottom of the file.  MainWndProc() does handle menus and accelerator
 |  keys, so it comes first.
 |
 +- Notes ----------------------------------------------------------------
 |  #define AL_USING_DLL if your are linking to an Archive Library 16-bit
 |  import library.
 |  #define ZIP if you wish to create WIN00.ZIP.
 |
 +- Functions ------------------------------------------------------------
 |  ALArchiveBase::Delete()
 |
 +=======================================================================*/

#define STRICT
#include <windows.h>
#include <stdarg.h>

#define AL_USING_DLL
#include "al.h"
#include "ex03win.h"

int NEAR iInstanceNumber;
HINSTANCE hInstance;
HWND hDlgMain = 0;
ALArchive *pArchive = 0;

void Delete (HWND hDlg);
void SizeFramingWindow(HWND hDlg);

// This function gets the name of an archive, then uses the
// ALArchiveBase::FillListBox() function to fill it up with
// names.

void ReadArchive(HWND hDlg, int archive_name, int list_box)
{
 char input_name[128];

 GetDlgItemText(hDlg, archive_name, input_name, 128);
 SendDlgItemMessage(hDlg, archive_name, EM_SETSEL, (WPARAM) - 1, (LPARAM) - 1);
#if defined (ZIP)
 ALPkArchive archive(input_name);
#else
 ALGlArchive archive(input_name);
#endif
 archive.FillListBox(hDlg, list_box);
}

// This function performs the deletion.  it gets the name of the archive
// from a text box, then gets the list, then sets marks on the list based
// on the names set in the list box.  The archive delete function then takes
// care of the hard work.

// There is one tricky bit here.  The pArchive pointer is a global variable.
// If we are in the middle of updating or deleting, the other functions
// in this program can figure that out by looking at the pointer.  They
// can also set the error member in pArchive, causing an abort of the
// delete process.

void Delete (HWND hDlg)
{
 char input_name[128];

 ALWindowsMessage monitor(AL_MONITOR_JOB,
                          GetDlgItem(hDlg, AL_PROGRESS_TEXT),
                          AL_SEND_RATIO,
                          GetDlgItem(hDlg, AL_PROGRESS_BAR),
                          ALGaugeSetPosition);
 GetDlgItemText(hDlg, AL_INPUT, input_name, 128);
#if defined (ZIP)
 pArchive = new ALPkArchive(input_name);
 ALEntryList list(&monitor, PkTools());
#else
 pArchive = new ALGlArchive(input_name);
 ALEntryList list(&monitor, GlTools());
#endif
 if (!pArchive)
  return;
 pArchive->ReadDirectory(list);
 list.ClearMarks();
 int count = list.SetMarksFromListBox(hDlg, AL_INPUT_LIST);
 EditDisplay(hDlg,
             AL_DEBUG,
             "%d file%s selected\r\n",
             count,
             (count > 1 ? "s are" : " is"));

 // The new archive has a blank name.  This tells the delete function
 // to rename it after the deletion is done.

#if defined (ZIP)
 ALPkArchive archive("");
#else
 ALGlArchive archive("");
#endif
 pArchive->Delete (list, archive);
 SetDlgItemText(hDlg, AL_OUTPUT, pArchive->GetStorageObject()->mName.GetSafeName());
 ReadArchive(hDlg, AL_INPUT, AL_INPUT_LIST);
 ReadArchive(hDlg, AL_OUTPUT, AL_OUTPUT_LIST);
 delete pArchive;
 pArchive = 0;
}

// This is the window procedure for the modeless dialog.  It is where
// all the commands and so on are handled.  Most of the activity in
// the program is invoked as a results of button presses in the
// main dialog windows.

BOOL AL_EXPORT CALLBACK MainDialogProc(HWND hDlg,
                                       UINT message,
                                       WPARAM wParam,
                                       LPARAM)
{
 switch (message) {

  // On initialization, we size our framing windows, then set up the window
  // caption, followd by setting up the two text boxes in this dialog.

 case WM_INITDIALOG :
  SizeFramingWindow(hDlg);
  char buf[81];
  wsprintf(buf, "Windows Example 03 <instance %d>", iInstanceNumber);
  SetWindowText(GetParent(hDlg), buf);
#if defined (ZIP)
  wsprintf(buf, "win%02d.zip", iInstanceNumber);
#else
  wsprintf(buf, "win%02d.gal", iInstanceNumber);
#endif
  SetDlgItemText(hDlg, AL_INPUT, buf);
  SetDlgItemText(hDlg, AL_OUTPUT, "");
  return (TRUE);

  // WM_COMMAND message are generated for most of the button presses and
  // keystrokes that we are concerned with.

 case WM_COMMAND :
  switch (wParam) {

   // IDOK indicatest that the user pressed enter.  We process that as
   // a command to read the input archive if the focus is in the right spot.

  case IDOK :                                                                  //User has pressed enter
   if (GetFocus() == GetDlgItem(hDlg, AL_INPUT)) {
    ReadArchive(hDlg, AL_INPUT, AL_INPUT_LIST);
    ReadArchive(hDlg, AL_OUTPUT, AL_OUTPUT_LIST);
   }
   break;

   // When we are supposed to read the input archive, it usually means there
   // is a new filename there.  That means we read in the archive using
   // the function provided here, then read in the contents of the backup
   // archive, if one exists.

  case AL_READ_INPUT :
   ReadArchive(hDlg, AL_INPUT, AL_INPUT_LIST);
   ReadArchive(hDlg, AL_OUTPUT, AL_OUTPUT_LIST);
   return TRUE;

   // Before we perform the deletion, we have to check to see if another
   // deletion is already in progress.

  case AL_DELETE :
   if (!pArchive) {
    Delete (hDlg);
    if (pArchive) {
     delete pArchive;
     pArchive = 0;
    }
   }
   return TRUE;

   // Normally a request to exit is no big deal.  But if I am already in the
   // process of performing a deletion, I don't want to try and exist.
   // In that case, I just set the error status for the archive, so it will
   // abort at its own sweet pace.

  case AL_EXIT :
  case WM_QUIT :
  case WM_DESTROY :
   if (pArchive) {
    pArchive->GetStorageObject()->mStatus.SetError(AL_USER_ABORT,
                                                   "User pressed abort key");
    FlashWindow(GetParent(hDlg), 1);
    FlashWindow(GetParent(hDlg), 0);
    return FALSE;
   }
   PostMessage(GetParent(hDlg), WM_DESTROY, 0, 0);
   DestroyWindow(hDlgMain);
   hDlgMain = 0;
   return TRUE;

   // To abort a deletion in progress, we just call the set error routine
   // and set the archives status to an error.  The delete function will see
   // this as soon as it comes up for air, and it will exit prematurely.

  case AL_ABORT :
   if (pArchive)
    pArchive->GetStorageObject()->mStatus.SetError(AL_USER_ABORT,
                                                   "User pressed abort key");
  return TRUE;

  default :
   break;
  }
  break;
 }
 return FALSE;
}

// The about box dialog procedure is pretty boring.  The most interesting
// thing it does is center itself on the screen.

BOOL AL_EXPORT CALLBACK
AboutDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM)
{
 switch (message) {
 case WM_INITDIALOG :
  RECT rc;
  GetWindowRect(hDlg, &rc);
  SetWindowPos(hDlg,
               NULL,
               ((GetSystemMetrics(SM_CXSCREEN) - (rc.right - rc.left)) / 2),
               ((GetSystemMetrics(SM_CYSCREEN) - (rc.bottom - rc.top)) / 2),
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

// The main window is nothing more than a shell that exists to create
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
    SetFocus(GetDlgItem(hDlgMain, AL_INPUT));
   break;
  case AL_ABOUT :
   DialogBox(hInstance, "ALAboutDialog", 0, AboutDialogProc);
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

// WinMain calls the initialization routine for the progress gauge,
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
  wc.hInstance = hInstance;
  wc.hIcon = LoadIcon(hInstance, "ALIcon");
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground =(HBRUSH) GetStockObject(WHITE_BRUSH);
  wc.lpszMenuName = "ALMenu";
  wc.lpszClassName = "GreenleafDialogClass";
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
 HWND hWnd = CreateWindow("GreenleafDialogClass",
                          "Greenleaf Example Program",
                          WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
                          CW_USEDEFAULT, 0, 0, 0, NULL, NULL, hInstance, NULL);

 FARPROC lpfn = MakeProcInstance((FARPROC) MainDialogProc, hInstance);
 hDlgMain = CreateDialog(hInstance, "ALMainDialog", hWnd, (DLGPROC) lpfn);
 ShowWindow(hWnd, nCmdShow);
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
 SetWindowPos(GetParent(hDlg), NULL, x, y,
              rect.right - rect.left,
              rect.bottom - rect.top + GetSystemMetrics(SM_CYMENU) +
               GetSystemMetrics(SM_CYCAPTION),
              SWP_NOZORDER);
}

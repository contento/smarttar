/*= Archive Library v2.12 ================================================
 |
 |  EXAMPLES\EX02WIN.CPP
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  EX02WIN.CPP demonstrates ALArchiveBase::Create() and
 |  ALArchiveBase::Append().  Both of these functions have overloaded
 |  versions so that you can use an archive or a list of storage objects
 |  as your input.  In this case, the input comes from an archive.  The
 |  program lets you select a list of files from the input archive, and
 |  then gives you the choice of either copying or appending to a new
 |  archive.
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
 |  ALArchive::Append()
 |  ALArchiveBase::Create()
 |  ALArchiveBase::ReadDirectory()
 |  ALEntryList::ClearMarks()
 |  ALEntryList::SetMarksFromListBox()
 |
 +=======================================================================*/

#define STRICT
#include <windows.h>
#include <stdarg.h>

#define AL_USING_DLL
#include "al.h"
#include "ex02win.h"

int NEAR iInstanceNumber;
HINSTANCE hInstance;
HWND hDlgMain = 0;
ALArchive *pArchive = 0;
void SizeFramingWindow(HWND hDlg);

// With a member function dedicated to filling list boxes, displaying the
// contents of an archive in a list box becomes real easy.

void ReadArchive(HWND hDlg, int archive_name, int list_box)
{
 char input_name[128];

 GetDlgItemText(hDlg, archive_name, input_name, 128);
#if defined (ZIP)
 ALPkArchive p(input_name);
#else
 ALGlArchive p(input_name);
#endif
 p.FillListBox(hDlg, list_box);
}

// To perform this function, I create a new list by reading the contents
// of the input archive.  I then set the marks based on what the user
// has set in the input list box.  The list is then ready to be passed
// as a parameter to the Create() command, and we are done.  After the
// command has been executed, we update the list box displays of both
// archive's contents.

void Copy(HWND hDlg)
{
 char input_name[128];

 GetDlgItemText(hDlg, AL_INPUT, input_name, 128);
 ALWindowsMessage monitor(AL_MONITOR_JOB,
                          GetDlgItem(hDlg,
                          AL_PROGRESS_TEXT),
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
 if (!pArchive)
  return;

 pArchive->ReadDirectory(list);
 list.ClearMarks();
 int count = list.SetMarksFromListBox(hDlg, AL_INPUT_LIST);
 EditDisplay(hDlg,
             AL_DEBUG,
             "%d file%s selected\r\n",
             count,
             (count != 1 ? "s are" : " is"));

 char archive_name[128];
 GetDlgItemText(hDlg, AL_OUTPUT, archive_name, 128);
#if defined (ZIP)
 ALPkArchive archive(archive_name);
#else
 ALGlArchive archive(archive_name);
#endif
 archive.Create(*pArchive, list);

 ReadArchive(hDlg, AL_INPUT, AL_INPUT_LIST);
 ReadArchive(hDlg, AL_OUTPUT, AL_OUTPUT_LIST);
}

// Append is just like Copy().  If I was smarter I could figure out a good
// way to combine these two procedures.  I did combine them in the C
// version of this program, EX02WIN.C.
//

void Append(HWND hDlg)
{
 char input_name[128];

 ALWindowsMessage monitor(AL_MONITOR_JOB,
                          GetDlgItem(hDlg, AL_PROGRESS_TEXT),
                          AL_SEND_RATIO,
                          GetDlgItem(hDlg, AL_PROGRESS_BAR),
                          ALGaugeSetPosition);
 GetDlgItemText(hDlg, AL_INPUT, input_name, 128);
#if defined (ZIP)
 if ((pArchive = new ALPkArchive(input_name)) == 0)
  return;
 ALEntryList list(&monitor, PkTools());
#else
 if ((pArchive = new ALGlArchive(input_name)) == 0)
  return;
 ALEntryList list(&monitor, GlTools());
#endif

 pArchive->ReadDirectory(list);
 list.ClearMarks();
 int count = list.SetMarksFromListBox(hDlg, AL_INPUT_LIST);
 EditDisplay(hDlg,
             AL_DEBUG,
             "%d file%s selected\r\n",
             count,
             (count > 1 ? "s are" : " is"));
 char archive_name[128];
 GetDlgItemText(hDlg, AL_OUTPUT, archive_name, 128);
#if defined (ZIP)
 ALPkArchive archive(archive_name);
#else
 ALGlArchive archive(archive_name);
#endif
 archive.Append(*pArchive, list);
 ReadArchive(hDlg, AL_INPUT, AL_INPUT_LIST);
 ReadArchive(hDlg, AL_OUTPUT, AL_OUTPUT_LIST);
}

// This is the window procedure for the dialog.  It handles all the commands
// except accelerator keys and menu items, which get shipped to the
// framing window.

BOOL AL_EXPORT CALLBACK MainDialogProc(HWND hDlg,
                                       UINT message,
                                       WPARAM wParam,
                                       LPARAM)
{
 switch (message) {

 // When the dialog first gets created, we have to initialize the
 // title bar text, plus the contents of the text boxes that list the
 // name of the input archive and the output archive.

 case WM_INITDIALOG :
  SizeFramingWindow(hDlg);
  char buf1[81];
  char buf2[81];
  wsprintf(buf1, "Windows Example 02 <instance %d>", iInstanceNumber);
  SetWindowText(GetParent(hDlg), buf1);
#if defined (ZIP)
  wsprintf(buf1, "WIN%02d.ZIP", iInstanceNumber);
  wsprintf(buf2, "OUT%02d.ZIP", iInstanceNumber);
#else
  wsprintf(buf1, "WIN%02d.GAL", iInstanceNumber);
  wsprintf(buf2, "OUT%02d.GAL", iInstanceNumber);
#endif
  SetDlgItemText(hDlg, AL_INPUT, buf1);
  SetDlgItemText(hDlg, AL_OUTPUT, buf2);
  return (TRUE);

  // Most of the interesting things in the dialog happen when a WM_COMMAND
  // message is received.  These are usually in response to the press
  // of buttons in the dialog.

 case WM_COMMAND :
  switch (wParam) {

   // We get this when the user hits return.  What we do depends on where
   // the focus is currently located.

  case IDOK :
   if (GetFocus() == GetDlgItem(hDlg, AL_INPUT))
    ReadArchive(hDlg, AL_INPUT, AL_INPUT_LIST);
   else if (GetFocus() == GetDlgItem(hDlg, AL_OUTPUT))
    ReadArchive(hDlg, AL_OUTPUT, AL_OUTPUT_LIST);
   break;

   // Read the input archive when this button gets clicked.

  case AL_READ_INPUT :
   ReadArchive(hDlg, AL_INPUT, AL_INPUT_LIST);
   return TRUE;

   // Read the output archive when this button gets clicked.

  case AL_READ_OUTPUT :
   ReadArchive(hDlg, AL_OUTPUT, AL_OUTPUT_LIST);
   return TRUE;

   // When I get a click on the copy button, I execute the Copy() procedure.
   // Note that if a copy or append is already in progress, I figure that
   // out by the presence of the global variable pArchive. In that case,
   // I just skip it.

  case AL_COPY :
   if (!pArchive) {
    Copy(hDlg);
    if (pArchive) {
     delete pArchive;
     pArchive = 0;
    }
   }
   return TRUE;

   // Append is just like Copy().

  case AL_APPEND :
   if (!pArchive) {
    Append(hDlg);
    if (pArchive) {
     delete pArchive;
     pArchive = 0;
    }
   }
   return TRUE;

   // If the user tries to quit  while a copy or append is in progress, I
   // do my best to create an error, and flash his window.  Since I'm not
   // sure if my error is going to take, I don't quit just yet.
   // If nothing is already in progress, I just do a normal quit.

  case AL_EXIT :
  case WM_QUIT :
  case WM_DESTROY :
   if (pArchive) {
    pArchive->GetStorageObject()->mStatus.SetError(AL_USER_ABORT,
                                                   "User pressed abort key");
                                                   FlashWindow(GetParent(hDlg),
                                                   1);

    FlashWindow(GetParent(hDlg), 0);
    return FALSE;
   }
   PostMessage(GetParent(hDlg), WM_DESTROY, 0, 0);
   DestroyWindow(hDlgMain);
   hDlgMain = 0;
   return TRUE;

   // To abort an operation in progress, I just set the error code for the
   // archive storage object.  This should cause it to abort next time it comes
   // up for air in the compression routine.

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

//
// The about box is pretty much what you would expect.
//

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
  case AL_GOTO_OUTPUT :
   if (hDlgMain)
    SetFocus(GetDlgItem(hDlgMain, AL_OUTPUT));
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
  wc.hInstance = hInstance;
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
                          "ArchiveLib Example Program",
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
 return msg.wParam;
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
                        rect.bottom - rect.top +
                        GetSystemMetrics(SM_CYMENU) + GetSystemMetrics(SM_CYCAPTION),
                        SWP_NOZORDER);
}

/*= Archive Library v2.12 ================================================
 |
 |  EXAMPLES\EX04WIN.CPP
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  EX04WIN.CPP demonstrates ALMemory::ALMemory().  It lets you
 |  compress selected input files to a memory archive.  It then turns around
 |  and decompresses the objects to a new memory files, then compares the
 |  contents of the two.
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
 |  ALStorage::~ALStorage()
 |  ALStorage::Compare()
 |  ALStorage::Delete()
 |  ALFile::ALFile()
 |  ALFile::~ALFile()
 |  ALMemory::ALMemory()
 |  ALArchiveBase::Extract()
 |  ALArchive::ALArchive()
 |  ALName::GetSafeName()
 |
 +=======================================================================*/

#define STRICT
#include <windows.h>
#include <stdarg.h>

#define AL_USING_DLL
#include "al.h"
#include "ex04win.h"

#if defined (AL_BORLAND) || !defined (AL_FLAT_MODEL)
 #define AL_3D
 #include "ctl3d.h"
#else
 #define Ctl3dColorChange()
 #define Ctl3dRegister(a)
 #define Ctl3dAutoSubclass(a)
 #define Ctl3dUnregister(a)
#endif

int NEAR iInstanceNumber;
HINSTANCE hInstance;
HWND hDlgMain = 0;
ALStorage *pObject = 0;

void SizeFramingWindow(HWND hDlg);

// This routine is a bit longer than I might have liked.  This is because
// it really is several routines combined into one.  It's most important
// tasks are to first create a memory based archive with a bunch of files
// stuffed into it, then extract all those files into newly created
// memory objects.  Finally, the orginal files are compared to the memory
// objects to make sure the compress/extract cycle is good.

void CompressFiles(HWND hDlg, int list_box)
{
 // The first section of this routine is pretty similar to lots of other
 // example programs, both under Windows and DOS.  We create a new archive
 // using an ALMemory object as the base, then create a list with elements
 // picked out from a list box, then insert them into an archive.

 ALWindowsMessage monitor(AL_MONITOR_OBJECTS,
                          GetDlgItem(hDlg, AL_PROGRESS_TEXT),
                          AL_SEND_RATIO,
                          GetDlgItem(hDlg, AL_PROGRESS_BAR));
 pObject = new ALWinMemory("archive in RAM");
 if (!pObject)
  return;
#if defined (ZIP)
 ALPkArchive archive(*pObject);
 ALEntryList list(&monitor); // deliberately use default engine
#else
 ALGlArchive archive(*pObject);
 ALEntryList list(&monitor);
#endif
 list.MakeEntriesFromListBox(hDlg, list_box);
 archive.Create(list);
 if (pObject->mStatus < 0) {
  delete pObject;
  pObject = 0;
  MessageBox(hDlg, "Error creating archive", "EX04WIN", MB_ICONEXCLAMATION);
  return;
 }

 // Here I modify the job list so that the archive data records will
 // be extracted to memory objects instead of file objects.  Then I
 // execute the extract function, creating a scad of new ALMemory
 // objects.  At this point, the only way I have of getting to those
 // memory objects is by way of the list, so I have to hang on to it.
 // But I'm done with the archive, so I can delete it.

 ALEntry *job;
 for (job = list.GetFirstEntry();
  job != 0;
  job = job->GetNextEntry()) {
  ALName *temp = new ALName(job->mpStorageObject->mName);
  delete job->mpStorageObject;
#if defined (AL_FLAT_MODEL)
  job->mpStorageObject = new ALMemory(*temp);
#else
  job->mpStorageObject = new ALHugeMemory(*temp);
#endif
  delete temp;
 }
 archive.Extract(list);
 if (archive.mStatus < 0) {
  delete pObject;
  pObject = 0;
  MessageBox(hDlg,
             "Error extracting from archive",
             "EX04WIN",
             MB_ICONEXCLAMATION);
  return;
 }
 pObject->Delete ();
 delete pObject;
 pObject = 0;

 //  Now I iterate through the list.  For every entry in the list, I compare
 //  the memory object with the original ALFile object.  note that I am
 //  using a monitor object to track the progress of the comparison.  This
 //  is kind of a tricky bit.  Anyway, I do the compare and print the
 //  results.

 for (job = list.GetFirstEntry();
  job != 0;
  job = job->GetNextEntry()) {
  pObject = new ALFile(job->mpStorageObject->mName);
  char buf[128];
  wsprintf(buf, "Comparing %s", (LPSTR) job->mpStorageObject->mName.GetSafeName());
  SetDlgItemText(hDlg, AL_PROGRESS_TEXT, buf);
  pObject->mpMonitor = &monitor;

  // Since I am monitoring objects, I need to set up these two data members in
  // the monitor if I expect it to work properly.

  pObject->mpMonitor->mlObjectSize = -1;
  pObject->mpMonitor->mlObjectStart = 0;
  int result = pObject->Compare(*job->mpStorageObject);
  if (result != AL_SUCCESS)
   SetDlgItemText(hDlg, AL_PROGRESS_TEXT, "Failure!");
  if (pObject->mStatus < 0) {
   delete pObject;
   pObject = 0;
   return;
  } else {
   delete pObject;
   pObject = 0;
   SetDlgItemText(hDlg, AL_PROGRESS_TEXT, "Success!");
  }
 }
}

// This is the window procedure for the modeless dialog.  It is where
// all the commands and so on are handled.

BOOL AL_EXPORT CALLBACK MainDialogProc(HWND hDlg,
                                       UINT message,
                                       WPARAM wParam,
                                       LPARAM lParam)
{
 switch (message) {

  // This is where I initialize the window title, and clear/set any other
  // controls to their proper initial state.

 case WM_INITDIALOG :
  SizeFramingWindow(hDlg);
  char buf[81];
  wsprintf(buf, "Windows Example 04 <instance %d>", iInstanceNumber);
  SetWindowText(GetParent(hDlg), buf);
  SendDlgItemMessage(hDlg,
                     AL_INPUT_LIST,
                     LB_DIR,
                     DDL_READWRITE,
                     (LPARAM) ((LPSTR) "*.*"));
  return (TRUE);

 case WM_SYSCOLORCHANGE :
  Ctl3dColorChange();
  break;

  // Everything exciting the user wants to make happen gets to the program
  // byt way of the WM_COMMAND message handler.

 case WM_COMMAND :
  switch (LOWORD(wParam)) {

   // The only command I am locking for on the list box is the double click.
   // If I see a double click, it means it is time to compress the files.

  case AL_INPUT_LIST :
#if defined (AL_FLAT_MODEL)
   AL_UNUSED_PARAMETER(lParam);
   switch (HIWORD(wParam)) {
#else
   switch (HIWORD(lParam)) {
#endif
    case LBN_DBLCLK :
     if (!pObject)
      CompressFiles(hDlg, AL_INPUT_LIST);
     break;
     }
     break;

     // This button or the return key both start the Compression cycle.

    case AL_CYCLE :
    case IDOK :
     if (!pObject)
      CompressFiles(hDlg, AL_INPUT_LIST);
     break;

     // If the user tries to exit while work is in progress, we just set the
     // error status and do something annoying.  Otherwise we close the window
     // and shut things down.

    case AL_EXIT :
    case WM_QUIT :
     if (pObject) {
      pObject->mStatus.SetError(AL_USER_ABORT, "User pressed abort key");
      FlashWindow(GetParent(hDlg), 1);
      FlashWindow(GetParent(hDlg), 0);
      return FALSE;
     }
     PostMessage(GetParent(hDlg), WM_DESTROY, 0, 0);
     DestroyWindow(hDlgMain);
     hDlgMain = 0;
     return TRUE;

     // Likewise for abort, we set the error status for the object in question.

    case AL_ABORT :
     if (pObject)
      pObject->mStatus.SetError(AL_USER_ABORT, "User pressed abort key");
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
  case AL_ABOUT :
   DialogBox(hInstance, "ALAboutDialog", 0, AboutDialogProc);
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

 // WinMain calls the initialization routine for the progress gauge,
 // registers our class.  It then creates the dialog, and starts the
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
   wc.hInstance = hInstance;
   wc.hIcon = LoadIcon(hInstance, "ALIcon");
   wc.hCursor = LoadCursor(NULL, IDC_ARROW);
   wc.hbrBackground =(HBRUSH) GetStockObject(WHITE_BRUSH);
   wc.lpszMenuName = 0;
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
  Ctl3dRegister(hInstance);
  Ctl3dAutoSubclass(hInstance);
  HWND hWnd = CreateWindow("GreenleafDialogClass",
                           "Greenleaf Example Program",
                           WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
                           CW_USEDEFAULT, 0, 0, 0, NULL, NULL,hInstance, NULL);

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
  Ctl3dUnregister(instance);
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
  y -= GetSystemMetrics(SM_CYCAPTION);
  y /= 2;
  int x = GetSystemMetrics(SM_CXSCREEN);
  x -= rect.right - rect.left;
  x /= 2;
  SetWindowPos(GetParent(hDlg), NULL, x, y,
               rect.right - rect.left ,
               rect.bottom - rect.top + GetSystemMetrics(SM_CYCAPTION),
               SWP_NOZORDER);
}

/*= Archive Library v2.12 ================================================
 |
 |  EXAMPLES\EX00WIN.CPP
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  EX00WIN.CPP program creates an archive and adds files to it, while
 |  using an ALBarGraph monitor.  By default it uses *.DEF as its input
 |  files, but you can modify this by entering new data in the
 |  AL_INPUT_FILES text box.
 |
 |  EX00WIN.CPP uses a dialog as its main window.  Unlike most of the
 |  other Windows examples, this guy doesn't have a dummy frame window
 |  around the dialog.
 |
 +- Notes ----------------------------------------------------------------
 |  #define AL_USING_DLL if your are linking to an Archive Library 16-bit
 |  import library.
 |  #define ZIP if you wish to create WIN00.ZIP.
 |
 +- Functions ------------------------------------------------------------
 |  ALArchiveBase::Create()
 |  ALWindowsMessage::ALWindowsMessage()
 |
 +=======================================================================*/

#define STRICT
#include <windows.h>
#include <stdarg.h>

// Only if I'm linking to a 16-bit import library.
//#define AL_USING_DLL

#include "al.h"
#include "ex00win.h"

// Only use CTL3D when possible

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
ALArchive *pArchive = 0;

extern "C" BOOL AL_EXPORT CALLBACK AboutDialogProc(HWND, UINT, WPARAM, LPARAM);

// This the routine that gets invoked in response to the user pressing
// the compress button.  This is dispatched from the dialog procedure.
// It just creates a monitor, then a list, then an archive.  If that
// all goes okay, the wild cards get added to the list, then the Create
// procedure is called.
//
// There is one slightly tricky bit here.  The pointer to the archive
// object is kept in a global variable.  We abort the archiving procedure
// by calling SetError for that archive.  This means that we have to be
// really careful to set that pointer back to zero when we destroy the
// archive.

void Compress(HWND hDlg)
{
 char archive_name[128];
 char input_name[128];

 ALWindowsMessage monitor(AL_MONITOR_JOB,
                          GetDlgItem(hDlg, AL_PROGRESS_TEXT),
                          AL_SEND_BYTE_COUNT,
                          GetDlgItem(hDlg, AL_PROGRESS_NUMBER));
 GetDlgItemText(hDlg, AL_ARCHIVE_NAME, archive_name, 128);
#if defined (ZIP)
 pArchive = new ALPkArchive(archive_name);
 ALEntryList list(&monitor, PkCompressTools());
#else
 pArchive = new ALGlArchive(archive_name);
 ALEntryList list(&monitor, GlTools());
#endif
 if (pArchive) {
  GetDlgItemText(hDlg, AL_INPUT_FILES, input_name, 128);
  list.AddWildCardFiles(input_name);
  pArchive->Create(list);
  SetDlgItemText(hDlg, AL_PROGRESS_TEXT, pArchive->mStatus.GetStatusString());
  delete pArchive;
  pArchive = 0;
 } else
  SetDlgItemText(hDlg, AL_PROGRESS_TEXT, "Done, allocation failure");
}

// This is the dialog procedure for our main dialog.  In this program, all
// the important work gets done here.  Most of it gets down by the
// handler for WM_COMMAND, which processes the button presses from the
// dialog.

BOOL AL_EXPORT CALLBACK MainDialogProc(HWND hDlg,
                                       UINT message,
                                       WPARAM wParam,
                                       LPARAM)
{
 switch (message) {

 // We respond to the init message by positioning the dialog on the screen,
 // initializing the windows title, the setting up the initial values of
 // all the text boxes.

 case WM_INITDIALOG :
  RECT rc;
  GetWindowRect(hDlg, &rc);
  SetWindowPos(hDlg,
               NULL,
               ((GetSystemMetrics(SM_CXSCREEN) - (rc.right - rc.left)) / 2),
               ((GetSystemMetrics(SM_CYSCREEN) - (rc.bottom - rc.top)) / 2),
               0,
               0,
               SWP_NOSIZE | SWP_NOACTIVATE);

  char buf[81];
  wsprintf(buf, "Windows Example 00 <instance %d>", iInstanceNumber);
  SetWindowText(hDlg, buf);
#if defined (ZIP)
  wsprintf(buf, "WIN%02d.ZIP", iInstanceNumber);
#else
  wsprintf(buf, "WIN%02d.GAL", iInstanceNumber);
#endif
  SetDlgItemText(hDlg, AL_ARCHIVE_NAME, buf);
  SetDlgItemText(hDlg, AL_INPUT_FILES, "*.DEF");
  SetDlgItemText(hDlg, AL_PROGRESS_TEXT, "");
  SetDlgItemText(hDlg, AL_PROGRESS_NUMBER, "");
  return (TRUE);

  // Have to support this message in order to make the CTL3D stuff work.

 case WM_SYSCOLORCHANGE :
  Ctl3dColorChange();
  break;

  // WM_COMMAND is for all the button presses.

 case WM_COMMAND :
  switch (wParam) {

   // If the user wants to compress, we do so, but only if no compression
   // is already in progress.

  case AL_COMPRESS :
   if (!pArchive)
    Compress(hDlg);
   return TRUE;

   // If the user wants to quit, I have to set the archive to an error status
   // first.  Otherwise, the compression code will keep right on going even
   // after we have killed our dialog window.  Code like this is used all over
   // the place in all of the example programs.

  case AL_EXIT :
  case WM_QUIT :
  case WM_DESTROY :
   if (pArchive)
    pArchive->GetStorageObject()->mStatus.SetError(AL_USER_ABORT,
                                                   "User pressed abort key");
  EndDialog(hDlg, TRUE);
  return TRUE;

  // We abort a compression in progress the same way, by setting the
  // error code for the archive storage object.  Likewise, code that looks
  // just like this is used all throughout our example programs.

  case AL_ABORT :
   if (pArchive)
    pArchive->GetStorageObject()->mStatus.SetError(AL_USER_ABORT,
                                                   "User pressed abort key");
   return TRUE;

  case AL_ABOUT :
   DialogBox(hInstance, "ALAboutDialog", 0, AboutDialogProc);
   return TRUE;
  }
  break;
 }
 return FALSE;
}

// The about procedure just displays the about box.

BOOL AL_EXPORT CALLBACK AboutDialogProc(HWND hDlg,
                                        UINT message,
                                        WPARAM wParam,
                                        LPARAM)
{
 switch (message) {
 case WM_INITDIALOG :
  RECT rc;
  GetWindowRect(hDlg, &rc);
  SetWindowPos(hDlg,
               NULL,
               ((GetSystemMetrics(SM_CXSCREEN) - (rc.right - rc.left)) / 2),
               ((GetSystemMetrics(SM_CYSCREEN) - (rc.bottom - rc.top)) / 2),
               0,
               0,
               SWP_NOSIZE | SWP_NOACTIVATE);
  break;
 case WM_COMMAND :
  switch (wParam) {
  case IDOK :
  case AL_EXIT :
  case WM_QUIT :
  case WM_DESTROY :
   EndDialog(hDlg, TRUE);
   return TRUE;
  }
  break;
 }
 return FALSE;
}

// WinMain just has to dispatch the dialog box.  It also calls the routine
// to register the ALGauge class, and it also sets up the CTL3d stuff.  Note
// that you can run multiple instances of this program, (good for testing
// the DLL), so we also get some instance info.

int AL_WIN_MAIN_FAR PASCAL WinMain(HINSTANCE instance,
                                   HINSTANCE previous_instance,
                                   LPSTR,
                                   int)
{
 hInstance = instance;
 if (!ALGaugeInit(instance, previous_instance))
  return (FALSE);
 if (previous_instance == 0)
  iInstanceNumber = 0;
 else {
#ifndef AL_FLAT_MODEL
  GetInstanceData(previous_instance,
                  (PBYTE) (void _near *) &iInstanceNumber,
                  sizeof iInstanceNumber);
#endif
  iInstanceNumber++;
 }
 Ctl3dRegister(instance);
 Ctl3dAutoSubclass(instance);
 DialogBox(instance, "ALMainDialog", 0, MainDialogProc);
 Ctl3dUnregister(instance);
 return 0;
}

/*= Archive Library v2.12 ================================================
 |
 |  EXAMPLES\EX24WIN.C
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  EX24WIN.C creates an archive and adds files to it, while using an
 |  ALBarGraph monitor.  By default it uses *.CPP and *.DEF as its input
 |  files, but you can modify this by entering new data in the
 |  AL_INPUT_FILES text box.
 |
 |  EX24WIN.C uses a dialog as its main window.  Unlike most of the
 |  other Windows examples, this guy doesn't have a dummy frame window
 |  around the dialog.
 |
 |  EX24WIN.C also demonstrates the use of a callback function under the
 |  Simplified Interface.  The callback function is called periodically,
 |  and takes care of keepin the user interface updated.
 |
 +- Notes ----------------------------------------------------------------
 |  #define AL_USING_DLL if your are linking to an Archive Library 16-bit
 |  import library.
 |
 +- Functions ------------------------------------------------------------
 |  ALCreate()
 |
 +=======================================================================*/

#define STRICT
#include <windows.h>
#include <stdarg.h>

#define AL_USING_DLL
#include "alsimple.h"
#include "ex24win.h"
#include "algauge.h"

#if defined (AL_BORLAND) || !defined (AL_FLAT_MODEL)
 #define AL_3D
 #include "ctl3d.h"
#else
 #define Ctl3dColorChange()
 #define Ctl3dRegister(a)
 #define Ctl3dAutoSubclass(a)
 #define Ctl3dUnregister(a)
#endif

int iInstanceNumber;
HINSTANCE hInstance;

BOOL AL_EXPORT CALLBACK AboutDialogProc(HWND, UINT, WPARAM, LPARAM);

HWND dialog;

/*
  This is the callback function that is called during compression.
  When a file is first opened for insertion into the archive,
  the filename parameter is passed in the name parameter, and the
  two ratios are passed with values of -1.  When a progress update
  is taking place, the name is passed with a value of 0 (null pointer),
  and the two ratios are passed with valid data.
*/

void AL_EXPORT AL_DLL_FAR my_callback(const char AL_DLL_FAR *name,
                                      int object_ratio,
                                      int job_ratio)
{
 if (name)
  SetDlgItemText(dialog, AL_PROGRESS_TEXT, name);
 if (object_ratio > 0)
  SendMessage(GetDlgItem(dialog, AL_FILE_PROGRESS),
              ALGaugeSetPosition,
              object_ratio,
              object_ratio);
if (job_ratio > 0)
 SendMessage(GetDlgItem(dialog, AL_JOB_PROGRESS),
             ALGaugeSetPosition,
             job_ratio,
             job_ratio);
}

/*
  This the routine that gets invoked in response to the user pressing
  the compress button.  This is dispatched from the dialog procedure.
  It calls ALCreate() in the simplifed interface, which does all of
  the hard work.
*/

void Compress(HWND hDlg)
{
 char archive_name[128];
 char input_name[128];

 EnableWindow(GetDlgItem(hDlg, AL_COMPRESS), 0);
 dialog = hDlg;
 GetDlgItemText(hDlg, AL_ARCHIVE_NAME, archive_name, 128);
 GetDlgItemText(hDlg, AL_INPUT_FILES, input_name, 128);
 ALCreate(archive_name, input_name, 0, (CALLBACK_FN) my_callback);
 dialog = 0;
 EnableWindow(GetDlgItem(hDlg, AL_COMPRESS), 1);
}

/*
  This is the dialog procedure for our main dialog.  In this program, all
  the important work gets done here.  Most of it gets down by the
  handler for WM_COMMAND, which processes the button presses from the
  dialog.
*/

BOOL AL_EXPORT CALLBACK MainDialogProc(HWND hDlg,
                                       UINT message,
                                       WPARAM wParam,
                                       LPARAM lParam)
{
 RECT rc;
 char buf[81];

 AL_UNUSED_PARAMETER(lParam);
 switch (message) {
 /*
   We respond to the init message by positioning the dialog on the screen,
   initializing the windows title, the setting up the initial values of
   all the text boxes.
 */
 case WM_INITDIALOG :
  GetWindowRect(hDlg, &rc);
  SetWindowPos(hDlg,
               0,
               ((GetSystemMetrics(SM_CXSCREEN) - (rc.right - rc.left)) / 2),
               ((GetSystemMetrics(SM_CYSCREEN) - (rc.bottom - rc.top)) / 2),
               0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
  wsprintf(buf, "Windows example 24 <instance %d>", iInstanceNumber);
  SetWindowText(hDlg, buf);
  wsprintf(buf, "WIN%02d.ZIP", iInstanceNumber);
  SetDlgItemText(hDlg, AL_ARCHIVE_NAME, buf);
  SetDlgItemText(hDlg, AL_INPUT_FILES, "*.CPP, *.DEF");
  SetDlgItemText(hDlg, AL_PROGRESS_TEXT, "");
  return (TRUE);
  /* Have to support this message in order to make the CTL3D stuff work. */
 case WM_SYSCOLORCHANGE :
  Ctl3dColorChange();
  break;
  /* WM_COMMAND is for all the button presses. */
 case WM_COMMAND :
  switch (wParam) {
   /*
     If the user wants to compress, we do so, but only if no compression
     is already in progress.
   */
  case AL_COMPRESS :
   if (!dialog)
    Compress(hDlg);
   return TRUE;
   /*
     Note that when you are using the simplified interface, there
     isn't a quick and easy way to abort your compression routine.
   */
  case AL_EXIT :
  case WM_QUIT :
  case WM_DESTROY :
   if (dialog == 0) {
    EndDialog(hDlg, TRUE);
    return TRUE;
   }
   return FALSE;
  case AL_ABOUT :
   DialogBox(hInstance, "ALAboutDialog", 0, (DLGPROC) AboutDialogProc);
   return TRUE;
  }
  break;
 }
 return FALSE;
}

/* The about procedure just displays the about box. */
BOOL AL_EXPORT CALLBACK AboutDialogProc(HWND hDlg,
                                        UINT message,
                                        WPARAM wParam,
                                        LPARAM lParam)
{
 RECT rc;

 AL_UNUSED_PARAMETER(lParam);

 switch (message) {
 case WM_INITDIALOG :
  GetWindowRect(hDlg, &rc);
  SetWindowPos(hDlg,
               0,
               ((GetSystemMetrics(SM_CXSCREEN) - (rc.right - rc.left)) / 2),
               ((GetSystemMetrics(SM_CYSCREEN) - (rc.bottom - rc.top)) / 2),
               0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
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

/*
  WinMain just has to dispatch the dialog box.  It also calls the routine
  to register the ALGauge class, and it also sets up the CTL3d stuff.  Note
  that you can run multiple instances of this program, (good for testing
  the DLL), so we also get some instance info.
*/

int AL_WIN_MAIN_FAR PASCAL WinMain(HINSTANCE instance,
                                   HINSTANCE previous_instance,
                                   LPSTR cmd_line,
                                   int cmd_show)
{
 AL_UNUSED_PARAMETER(cmd_line);
 AL_UNUSED_PARAMETER(cmd_show);

 hInstance = instance;
 if (previous_instance == 0)
  iInstanceNumber = 0;
#if !defined (AL_FLAT_MODEL)
 else {
  GetInstanceData(previous_instance,
                  (PBYTE) (void _near *) &iInstanceNumber,
                  sizeof iInstanceNumber);
 iInstanceNumber++;
}
#endif

 ALGaugeInit(hInstance, previous_instance);
 Ctl3dRegister(instance);
 Ctl3dAutoSubclass(instance);
 DialogBox(instance, "ALMainDialog", 0, (DLGPROC) MainDialogProc);
 Ctl3dUnregister(instance);
 return 0;
}

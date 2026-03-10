/*= Archive Library v2.12 ================================================
 |
 |  EXAMPLES\EX05WIN.C
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  EX05WIN.C demonstrates the capabilities of the debug libraries by doing
 |  a few bad things.  Most of the bad things should cause an assertion
 |  error, with a good explanation of what happened.  However, this will
 |  only work if you link with the debug versions of the libraries!
 |
 |  A lot of the other demo programs in the library use a dummy framing
 |  window that surrounds the dialog box.  This guy skips all that and
 |  just uses the dialog as the main window, period.  This makes it
 |  nice and simple.  In exchange for that, we give up accelerator keys
 |  and a few other goodies.
 |
 |  This program is functionally equivalent to EX05WIN.CPP.  The C version
 |  uses the translation layer functions to get the job done, but if
 |  you put them side by side, you won't see too much difference.
 |
 +- Notes ----------------------------------------------------------------
 |  #define AL_USING_DLL if your are linking to an Archive Library 16-bit
 |  import library.
 |  #define ZIP if you wish to create WIN00.ZIP.
 |
 +- Functions ------------------------------------------------------------
 |  deleteALStorage()
 |
 +=======================================================================*/

#define STRICT
#include <windows.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>

#define AL_USING_DLL
#include "arclib.h"
#include "filestor.h"
#include "ex05win.h"

#if defined (AL_BORLAND) || !defined (AL_FLAT_MODEL)
 #define AL_3D
 #include "ctl3d.h"
#else
 #define Ctl3dColorChange()
 #define Ctl3dRegister(a)
 #define Ctl3dAutoSubclass(a)
 #define Ctl3dUnregister(a)
#endif

HINSTANCE hInstance;
BOOL AL_EXPORT CALLBACK AboutDialogProc(HWND, UINT, WPARAM, LPARAM);

/*
  This is the main window procedure, which in this program happens
  to be a dialog box.  This guy responds to a bunch of different button
  clicks.
*/

BOOL AL_EXPORT CALLBACK MainDialogProc(HWND hDlg,
                                       UINT message,
                                       WPARAM wParam,
                                       LPARAM lParam)
{
 RECT rc;
 hALStorage hFile;

 AL_UNUSED_PARAMETER(lParam);

 switch (message) {
/*
  The dialog initialization code sets the position for the main window.
  I also might disable one of the tests, because it won't work for Borland
  under large model windows.  This is because I can't do a heap walk under
  Windows in large model.  I can walk the Windows heap using TOOLHELP.DLL,
  but since Borland uses a subsegment allocator, I might not find a good
  pointer there.

  In the C version, I don't support the code to check for a write past
  the end of an array.  Since memory for the array is allocated using
  malloc(), not new, I don't have a write picket at the end.
*/
 case WM_INITDIALOG :
  GetWindowRect(hDlg, &rc);
  SetWindowPos(hDlg,
               0,
               ((GetSystemMetrics(SM_CXSCREEN) - (rc.right - rc.left)) / 2),
               ((GetSystemMetrics(SM_CYSCREEN) - (rc.bottom - rc.top)) / 2),
               0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
#if !defined (AL_MICROSOFT) && defined (AL_LARGE_DATA)
  EnableWindow(GetDlgItem(hDlg, AL_DELETE_BAD_POINTER), 0);
#endif
  EnableWindow(GetDlgItem(hDlg, AL_WRITE_PAST_END), 0);
  return (TRUE);
 case WM_SYSCOLORCHANGE :
  Ctl3dColorChange();
  break;
 case WM_COMMAND :
  switch (wParam) {
  case AL_EXIT :
  case WM_QUIT :
  case WM_DESTROY :
   EndDialog(hDlg, TRUE);
   return TRUE;
  /*
    This button causes me to delete an object twice.  Since my destructor has
    a check for the GoodTag() member function, it should catch this easily.
  */
  case AL_DESTROY_TWICE : {
    hFile = newALFile("");
    deleteALStorage(hFile);
    deleteALStorage(hFile);
    return TRUE;
  }
  /*
    Here is where I try to delete a pointer that isn't in the heap.  As long
    as I can do a heapwalk, I can catch this.  I can't do a heap walk under
    a couple of different memory models under Windows, so this routine gets
    turned off there.  Microsoft was nice enough to provide a heapwalk for
    large memory model Windows programs.  Two points for them.
  */
 case AL_DELETE_BAD_POINTER : {
   char *p = malloc(250);
   deleteALArchive((hALArchive) p);
   return TRUE;
 }
 /*
   This just does a bunch of heap stuff.  When you have the debug libraries
   linked in you will see that this takes noticeably longer.
 */
case AL_EXERCISE : {
  char **p = malloc(250 * sizeof (char *));
  EnableWindow(GetDlgItem(hDlg, AL_EXERCISE), 0);
  srand((unsigned) time(NULL));
  if (p) {
   int i;
   for (i = 0; i < 250; i++)
    p[i] = malloc(rand() % 250);
   for (i = 0; i < 250; i++)
    if (p[i])
     free(p[i]);
    free(p);
  }
  EnableWindow(GetDlgItem(hDlg, AL_EXERCISE), 1);
  return TRUE;
}
/*
  Here I create an object, then mung the bytes before it.  Under normal
  circumstances, this would garbage your heap.  But in debug mode, we
  have tossed in a four byte picket at the start of the object, and so
  we catch this garbage when the object is deleted.
*/
case AL_UNDERSHOOT : {
  hFile = newALFile("");
  ((LPSTR) hFile)[- 1] = 0;
  deleteALStorage(hFile);
  return TRUE;
}
case AL_ABOUT :
 DialogBox(hInstance, "ALAboutDialog", 0, (DLGPROC) AboutDialogProc);
 return TRUE;
  }
  break;
 }
 return FALSE;
}

/*
  The about box dialog procedure is pretty boring.  The most interesting
  thing it does is center itself on the screen.
*/

BOOL AL_EXPORT CALLBACK
AboutDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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
  Since we aren't using a framing window in this program, WinMain() really
  has an easy time of it.  If we took away the CTL3D stuff, there would
  be virtually nothing in here at all.
*/

int AL_WIN_MAIN_FAR PASCAL WinMain(HINSTANCE instance,
                                   HINSTANCE prev,
                                   LPSTR cmd_line,
                                   int cmd_show)
{
 AL_UNUSED_PARAMETER(prev);
 AL_UNUSED_PARAMETER(cmd_line);
 AL_UNUSED_PARAMETER(cmd_show);

 hInstance = instance;
 Ctl3dRegister(instance);
 Ctl3dAutoSubclass(instance);
 DialogBox(instance, "ALMainDialog", 0, (DLGPROC) MainDialogProc);
 Ctl3dUnregister(instance);
 return 0;
}

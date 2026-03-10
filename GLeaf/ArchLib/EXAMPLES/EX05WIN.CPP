/*= Archive Library v2.12 ================================================
 |
 |  EXAMPLES\EX05WIN.CPP
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  EX05WIN.CPP demonstrates the capabilities of the debug libraries by
 |  doing a few bad things.  Most of the bad things should cause an
 |  assertion error, with a good explanation of what happened.  However,
 |  this will only work if you link with the debug versions of the
 |  libraries!
 |
 |  A lot of the other demo programs in the library use a dummy framing
 |  window that surrounds the dialog box.  This guy skips all that and
 |  just uses the dialog as the main window, period.  This makes it
 |  nice and simple.  In exchange for that, we give up accelerator keys
 |  and a few other goodies.
 |
 +- Notes ----------------------------------------------------------------
 |  #define AL_USING_DLL if your are linking to an Archive Library 16-bit
 |  import library.
 |
 +- Functions ------------------------------------------------------------
 |
 +=======================================================================*/

#define STRICT
#include <windows.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>

#define AL_USING_DLL
#include "al.h"
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
extern "C" BOOL AL_EXPORT CALLBACK AboutDialogProc(HWND, UINT, WPARAM, LPARAM);

// This is the main window procedure, which in this program happens
// to be a dialog box.  This guy responds to a bunch of different button
// clicks.

BOOL AL_EXPORT CALLBACK MainDialogProc(HWND hDlg,
                                       UINT message,
                                       WPARAM wParam,
                                       LPARAM)
{
 switch (message) {

  // The dialog initialization code sets the position for the main window.
  // I also might disable one of the tests, because it won't work for Borland
  // under large model windows.  This is because I can't do a heap walk under
  // Windows in large model.  I can walk the Windows heap using TOOLHELP.DLL,
  // but since Borland uses a subsegment allocator, I might not find a good
  // pointer there.

 case WM_INITDIALOG :
  RECT rc;
  GetWindowRect(hDlg, &rc);
  SetWindowPos(hDlg,
               NULL,
               ((GetSystemMetrics(SM_CXSCREEN) - (rc.right - rc.left)) / 2),
               ((GetSystemMetrics(SM_CYSCREEN) - (rc.bottom - rc.top)) / 2),
               0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
#if !defined (AL_MICROSOFT) && defined (AL_LARGE_DATA)
  EnableWindow(GetDlgItem(hDlg, AL_DELETE_BAD_POINTER), 0);
#endif
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

   // This button causes me to delete an object twice.  Since my destructor has
   // a check for the GoodTag() member function, it should catch this easily.

  case AL_DESTROY_TWICE : {
    ALFile *p = new ALFile();
    delete p;
    delete p;
    return TRUE;
  }

  // Writing one byte past the end of an array is a breeze to catch.  I have
  // a 4 byte picket at the end of the object, so I will catch this, without
  // causing a GPF.

 case AL_WRITE_PAST_END : {
   char *p = new char[100];
   p[100] = 0;
   delete p;
   return TRUE;
 }

  // Here is where I try to delete a pointer that isn't in the heap.  As long
  // as I can do a heapwalk, I can catch this.  I can't do a heap walk under
  // a couple of different memory models under Windows, so this routine gets
  // turned off there.  Microsoft was nice enough to provide a heapwalk for
  // large memory model Windows programs.  Two points for them.

 case AL_DELETE_BAD_POINTER : {
   char *p = new char[250];
   delete p;
   p = new char[100];
   p++;
   delete p;
   return TRUE;
 }

// This just does a bunch of heap stuff.  When you have the debug libraries
// linked in you will see that this takes noticeably longer.

 case AL_EXERCISE : {
   EnableWindow(GetDlgItem(hDlg, AL_EXERCISE), 0);
   char **p = new char *[250];
   srand((unsigned) time(NULL));
   if (p) {
     int i;
    for (i = 0; i < 250; i++)
     p[i] = new char[rand() % 250];
    for (i = 0; i < 250; i++)
     if (p[i])
      delete p[i];
     delete p;
 }
 EnableWindow(GetDlgItem(hDlg, AL_EXERCISE), 1);
 return TRUE;
}

// Here I create an object, then mung the bytes before it.  Under normal
// circumstances, this would garbage your heap.  But in debug mode, we
// have tossed in a four byte picket at the start of the object, and so
// we catch this garbage when the object is deleted.

 case AL_UNDERSHOOT : {
#if defined (ZIP)
   ALArchive *p = new ALPkArchive("test.zip");
#else
   ALArchive *p = new ALGlArchive("test.gal");
#endif
  ((LPSTR) p)[- 1] = 0;
  delete p;
  return TRUE;
 }
 case AL_ABOUT :
  DialogBox(hInstance, "ALAboutDialog", 0, AboutDialogProc);
  return TRUE;
 }
 break;
 }
 return FALSE;
}

// The about box dialog procedure is pretty boring.  The most interesting
// thing it does is center itself on the screen.

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

// Since we aren't using a framing window in this program, WinMain() really
// has an easy time of it.  If we took away the CTL3D stuff, there would
// be virtually nothing in here at all.

int AL_WIN_MAIN_FAR PASCAL WinMain(HINSTANCE instance,
                                   HINSTANCE,
                                   LPSTR,
                                   int)
{
 hInstance = instance;
 Ctl3dRegister(instance);
 Ctl3dAutoSubclass(instance);
 DialogBox(instance, "ALMainDialog", 0, MainDialogProc);
 Ctl3dUnregister(instance);
 return 0;
}

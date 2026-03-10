/*= Archive Library v2.12 ================================================
 |
 |  EXAMPLES\EX23WIN.CPP
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  EX23WIN.CPP is used to benchmark the performance of our various
 |  compression engines.  The program looks at the Input Files text
 |  box for a wildcard specification for a group of files.  The program
 |  expands the wild card specification and individually compresses and
 |  decompresses each of the files.  The temporary files created by this
 |  process are deleted.
 |
 |  After the whole batch of files is done, a summary of the compression
 |  time and statistics is displayed.
 |
 |  If the optional Compress Only check box is checked, the
 |  program runs in "compress-only" mode.  In this case, the decompression
 |  state is skipped.  You can use this to determine the ratio between
 |  compression and decompresion times.
 |
 |  If the Rude Mode check box is checked, the program doesn't service
 |  the Windows message loop while it is compression.
 |
 |  EX23WIN.CPP does one funky thing to try to give us real fast
 |  performance.  Instead of using the default buffer size of 4096
 |  bytes, I jack it up to a hefty 28672 bytes.  This gives us a few
 |  percentage points.  Just change the definition of BIG_BUFFER to
 |  benchmark at the default size.
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
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

#define AL_USING_DLL
#include "al.h"
#include "ex23win.h"

#if defined (AL_BORLAND) || !defined (AL_FLAT_MODEL)
 #define AL_3D
 #include "ctl3d.h"
#else
 #define Ctl3dColorChange()
 #define Ctl3dRegister(a)
 #define Ctl3dAutoSubclass(a)
 #define Ctl3dUnregister(a)
#endif

#ifndef BIG_BUFFER
 #define BIG_BUFFER 28672
#endif

HCURSOR hHourGlassCursor;
HINSTANCE hInstance;

// This storage object is used when compressing or decompressing.  If
// the user wants to abort the compression, it just has to set the
// error flag for this object.  If the object pointer is set to 0, it means
// no compression is currently in progress, so the Abort button does
// nothing.

ALStorage *compressed = 0;

extern "C" BOOL AL_EXPORT CALLBACK AboutDialogProc(HWND, UINT, WPARAM, LPARAM);

// The time class is a very modest convenience.

class TIME {
 public :
 long milliseconds;
 TIME() { milliseconds = GetTickCount(); }
 TIME(long t) { milliseconds = t; }
 TIME operator - (TIME &);
};

TIME TIME::operator - (TIME &rhs)
{
 return TIME(milliseconds - rhs.milliseconds);
}

// This function takes two file sizes and returns the compression ratio
// in the standard format we expect.

inline int ratio(long plain, long compressed)
{
 if (plain == 0)
  return 0;
 compressed *= 100;
 return (int) (100 - (compressed / plain));
}

// This function allocates a character buffer, then prints the
// time into it.  It is up to the caller to free the returned
// string.

char *format_time(const TIME &t)
{
 long m = t.milliseconds;
 int hours;
 int minutes;
 int seconds;
 int hsecs;

 char *s = (char *) malloc(25);
 if (s) {
  hours = (int) (m / (1000L * 60L * 60L));
  m -= hours * (1000L * 60L * 60L);
  minutes = (int) (m / (1000L * 60L));
  m -= minutes * (1000L * 60L);
  seconds = (int) (m / (1000L));
  m -= seconds * 1000L;
  hsecs = (int) (m / 10L);
  sprintf(s, "%02d:%02d:%02d.%02d", hours, minutes, seconds, hsecs);
 }
 return s;
}

// This is the first of two possible compression functions.  It is very
// much like the main loop in EX23CON.CPP.  It just sets up a wild card
// expander object, then starts chunking out file names.  Each time it
// gets a new file name, it compresses it, and optionally expands it.
// When the whole thing is over, the resulting stats are stuffed into
// the dialog, somewhere.

void NiceCompress(HWND hDlg, int greenleaf)
{
 char input_name[128];
 char buf[128];
 long total_input = 0;
 long total_compressed = 0;
 int compress_only = IsDlgButtonChecked(hDlg, AL_COMPRESS_ONLY);

 ALWindowsMessage monitor(AL_MONITOR_OBJECTS,
                          GetDlgItem(hDlg, AL_PROGRESS_TEXT),
                          AL_SEND_BYTE_COUNT,
                          GetDlgItem(hDlg, AL_PROGRESS_NUMBER));
 monitor.mlObjectStart = 0;
 GetDlgItemText(hDlg, AL_INPUT_FILES, input_name, 128);
 ALWildCardExpander expander(input_name);
 ALCompressor *compressor;
 ALDecompressor *decompressor;
 if (greenleaf) {
  compressor = new ALGlCompressor(AL_GREENLEAF_LEVEL_4);
  decompressor = new ALGlDecompressor(AL_GREENLEAF_LEVEL_4);
 } else {
#if defined (AL_FLAT_MODEL)
  compressor = new ALPkCompressor(6, 15, 8);
#else
  compressor = new ALPkCompressor(6, 14, 7);
#endif
  decompressor = new ALPkDecompressor;
 }

 // This code clears out all the boxes on the dialog that might be left
 // over from the last run of this program.

 SendDlgItemMessage(hDlg, AL_DATA, LB_RESETCONTENT, 0, 0);
 SetDlgItemText(hDlg, AL_TOTAL_INPUT, "");
 SetDlgItemText(hDlg, AL_TOTAL_COMPRESSED, "");
 SetDlgItemText(hDlg, AL_RATIO, "");
 SetDlgItemText(hDlg, AL_TIME, "");

 char AL_DLL_FAR *name;
 TIME global_start;

 // This is the loop where all of the work happens.  All we do here is
 // get a new file name and use it as the input file.  We then compress
 // to a second file, and then optionally expand to a third file.

 while ((name = expander.GetNextFile()) != 0) {
  TIME local_start;
  ALFile input(name, BIG_BUFFER);
  compressed = new ALFile("", BIG_BUFFER);
  ALFile output("", BIG_BUFFER);
  ALName filename(name);
  filename.StripPath();
  SetDlgItemText(hDlg, AL_PROGRESS_TEXT, (LPSTR) filename);
  input.mpMonitor = &monitor;
  compressor->Compress(input, *compressed);
  if (compressed->mStatus < AL_SUCCESS)
   break;
  if (!compress_only) {
   input.mpMonitor = 0;
   output.mpMonitor = &monitor;
   decompressor->Decompress(*compressed, output);
   output.Delete ();
   if (compressed->mStatus < AL_SUCCESS)
    break;
  }
  total_input += input.GetSize();
  total_compressed += compressed->GetSize();
  TIME local_stop;
  char *s;
  wsprintf(buf,
           "%s\t%ld\t%ld\t%d%%\t%s",
           (LPSTR) filename,
           input.GetSize(),
           compressed->GetSize(),
           ratio(input.GetSize(), compressed->GetSize()),
           (LPSTR)(s = format_time(local_stop - local_start)));
  if (s)
   free(s);

  // I stuff the file stats in the list box, then make sure that the currently
  // selected position in the list box is on the screen.

  int pos = (int) SendDlgItemMessage(hDlg,
                                     AL_DATA,
                                     LB_ADDSTRING,
                                     0,
                                     (LPARAM) (LPSTR) buf);
                                     SendDlgItemMessage(hDlg,
                                     AL_DATA,
                                     LB_SETCURSEL,
                                     pos,
                                     0);
  SendDlgItemMessage(hDlg,
                     AL_DATA,
                     LB_SETCURSEL,
                     (WPARAM) - 1,
                     0);
  compressed->Delete ();
  delete compressed;
  compressed = 0;
 }
 if (compressed) {
  delete compressed;
  compressed = 0;
 }
 TIME global_stop;
 sprintf(buf, "%ld", total_input);
 SetDlgItemText(hDlg, AL_TOTAL_INPUT, buf);
 sprintf(buf, "%ld", total_compressed);
 SetDlgItemText(hDlg, AL_TOTAL_COMPRESSED, buf);
 sprintf(buf, "%d%%", ratio(total_input, total_compressed));
 SetDlgItemText(hDlg, AL_RATIO, buf);
 char *s = format_time(global_stop - global_start);
 if (s) {
  sprintf(buf, "%s", s = format_time(global_stop - global_start));
  SetDlgItemText(hDlg, AL_TIME, buf);
  free(s);
 } else
 SetDlgItemText(hDlg, AL_TIME, "");
 delete compressor;
 delete decompressor;
}

// In order to blow off the Windows message loop, we have to derive a new
// file class, which is what we are doing here with ALRudeFile.  This
// class only needs a constructor and a new Yield function (the destructor
// is here because of DLL problems when we take the default dtor.)
// The Yield function does nothing.  This overrides the base class
// Yield function, which services the message loop.

class AL_CLASS_TYPE ALRudeFile : public ALFile {
 public :
 virtual AL_PROTO ~ALRudeFile() {; }
 void AL_DLL_FAR * AL_PROTO operator new (size_t size) { return:: new char[size]; }
 AL_PROTO ALRudeFile(const char AL_DLL_FAR *name = "");
 virtual void AL_PROTO YieldTime() {; }
};

AL_PROTO ALRudeFile::ALRudeFile(const char AL_DLL_FAR *name)
: ALFile(name, BIG_BUFFER, AL_MIXED)
{}

// RudeCompress is just like NiceCompress(), except that it uses rude
// files instead of ALFile.  It doesn't bother updating most of
// the elements on the screen, because we want to be as rude as
// possible.  One kind of not rude thing here is the hourglass cursor
// we put up to let you know that you aren't going to be able to do
// anything else for a while.

void RudeCompress(HWND hDlg, int greenleaf)
{
 char input_name[128];
 char buf[128];
 long total_input = 0;
 long total_compressed = 0;
 int compress_only = IsDlgButtonChecked(hDlg, AL_COMPRESS_ONLY);


 HCURSOR old_cursor = SetCursor(hHourGlassCursor);
 GetDlgItemText(hDlg, AL_INPUT_FILES, input_name, 128);
 ALWildCardExpander expander(input_name);

 ALCompressor *compressor;
 ALDecompressor *decompressor;

 if (greenleaf) {
  compressor = new ALGlCompressor(AL_GREENLEAF_LEVEL_4);
  decompressor = new ALGlDecompressor(AL_GREENLEAF_LEVEL_4);
 } else {
#if defined (AL_FLAT_MODEL)
  compressor = new ALPkCompressor(6, 15, 8);
#else
  compressor = new ALPkCompressor(6, 14, 7);
#endif
  decompressor = new ALPkDecompressor;
 }

 SendDlgItemMessage(hDlg, AL_DATA, LB_RESETCONTENT, 0, 0);
 SetDlgItemText(hDlg, AL_TOTAL_INPUT, "");
 SetDlgItemText(hDlg, AL_TOTAL_COMPRESSED, "");
 SetDlgItemText(hDlg, AL_RATIO, "");
 SetDlgItemText(hDlg, AL_TIME, "");
 SetDlgItemText(hDlg, AL_PROGRESS_NUMBER, "");

 char AL_DLL_FAR *name;
 TIME global_start;
 while ((name = expander.GetNextFile()) != 0) {
  TIME local_start;
  ALRudeFile input(name);
  compressed = new ALRudeFile;
  ALRudeFile output;
  ALName filename(name);
  filename.StripPath();
  SetDlgItemText(hDlg, AL_PROGRESS_TEXT, (LPSTR) filename);
  compressor->Compress(input, *compressed);
  if (compressed->mStatus < AL_SUCCESS)
   break;
  if (!compress_only) {
   decompressor->Decompress(*compressed, output);
   output.Delete ();
   if (compressed->mStatus < AL_SUCCESS)
    break;
  }
  total_input += input.GetSize();
  total_compressed += compressed->GetSize();
  compressed->Delete ();
  delete compressed;
  compressed = 0;
 }
 if (compressed) {
  delete compressed;
  compressed = 0;
 }
 TIME global_stop;
 sprintf(buf, "%ld", total_input);
 SetDlgItemText(hDlg, AL_TOTAL_INPUT, buf);
 sprintf(buf, "%ld", total_compressed);
 SetDlgItemText(hDlg, AL_TOTAL_COMPRESSED, buf);
 sprintf(buf, "%d%%", ratio(total_input, total_compressed));
 SetDlgItemText(hDlg, AL_RATIO, buf);
 char *s = format_time(global_stop - global_start);
 if (s) {
  sprintf(buf, "%s", s = format_time(global_stop - global_start));
  SetDlgItemText(hDlg, AL_TIME, buf);
  free(s);
 } else
 SetDlgItemText(hDlg, AL_TIME, "");
 SetCursor(old_cursor);
 delete compressor;
 delete decompressor;
}

// This text message used to be in the RC file, but for some reason
// Microsoft's RC.EXE decided it was too long to compile.

char *help = "This program is used to run benchmarks on ArchiveLib's "
             "compression performance.  Simply enter a wild-card file "
             "specification in the Input Files text box, then press the "
             "Compress button.  When the program completes, it will "
             "print statistics regarding compression speed and ratio.  The "
             "Compress Only check box can be used to disable decompression.  "
             "When the Rude Mode check box is selected the compression "
             "routines will not service the Windows message loop.";

// This is the dialog procedure for our main dialog.  In this program, all
// the important work gets done here.  Most of it gets down by the
// handler for WM_COMMAND, which processes the button presses from the
// dialog.

BOOL AL_EXPORT CALLBACK MainDialogProc(HWND hDlg,
                                       UINT message,
                                       WPARAM wParam,
                                       LPARAM)
{
 int tab_stops[] = { 65, 120, 175, 195 };
 switch (message) {

  // We respond to the init message by positioning the dialog on the screen,
  // initializing the windows title, the setting up the initial values of
  // all the text boxes.

 case WM_INITDIALOG :
  RECT rc;
  SetDlgItemText(hDlg, AL_HELP, help);
  SendDlgItemMessage(hDlg, AL_DATA, LB_SETTABSTOPS, 4, (LPARAM) (LPINT) tab_stops);
  /* I use this line of code to help me line up the columns...
  SendDlgItemMessage(hDlg,
                     AL_DATA,
                     LB_ADDSTRING,
                     0,
                     (LPARAM)(LPSTR) "A\tB\tC\tD\tE");
  */
  GetWindowRect(hDlg, &rc);
  SetWindowPos(hDlg,
               NULL,
               ((GetSystemMetrics(SM_CXSCREEN) - (rc.right - rc.left)) / 2),
               ((GetSystemMetrics(SM_CYSCREEN) - (rc.bottom - rc.top)) / 2),
               0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
  SetWindowText(hDlg, "Windows example 23");
  SetDlgItemText(hDlg, AL_INPUT_FILES, "C:\\COMMAND.*");
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

 case AL_COMPRESS_GREENLEAF :
  if (compressed == 0)
   if (IsDlgButtonChecked(hDlg, AL_RUDE))
    RudeCompress(hDlg, 1);
   else
    NiceCompress(hDlg, 1);
   return TRUE;

  case AL_COMPRESS_ZIP :
   if (compressed == 0)
    if (IsDlgButtonChecked(hDlg, AL_RUDE))
     RudeCompress(hDlg, 0);
    else
     NiceCompress(hDlg, 0);
    return TRUE;

    // If the user wants to quit, I have to set one of the file objects to an
    // error status first.  Otherwise, the compression code will keep right on
    // going even after we have killed our dialog window.  Code like this is used
    // all over the place in all of the example programs.

   case AL_EXIT :
   case WM_QUIT :
   case WM_DESTROY :
    if (compressed != 0)
     compressed->mStatus.SetError(AL_USER_ABORT, "User pressed abort key");
    else
     EndDialog(hDlg, TRUE);
    return TRUE;

    // We abort a compression in progress the same way, by setting the
    // error code for the storage object.  Likewise, code that looks
    // just like this is used all throughout our example programs.

   case AL_ABORT :
    if (compressed != 0)
     compressed->mStatus.SetError(AL_USER_ABORT, "User pressed abort key");
   return TRUE;

  case AL_ABOUT :
   DialogBox(hInstance, "ALAboutDialog", hDlg, AboutDialogProc);
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
 LPARAM) {
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
 case WM_QUIT :
 case WM_DESTROY :
  EndDialog(hDlg, TRUE);
  return TRUE;
 case WM_COMMAND :
  switch (wParam) {
  case IDOK :
  case AL_EXIT :
   EndDialog(hDlg, TRUE);
   return TRUE;
   }
  break;
 }
 return FALSE;
}

// WinMain just has to dispatch the dialog box.  It also sets up the CTL3d
// stuff.

int AL_WIN_MAIN_FAR PASCAL WinMain(HINSTANCE instance,
                                   HINSTANCE,
                                   LPSTR,
                                   int)
{
 hInstance = instance;
 hHourGlassCursor = LoadCursor(NULL, IDC_WAIT);
 Ctl3dRegister(instance);
 Ctl3dAutoSubclass(instance);
 DialogBox(instance, "ALMainDialog", 0, MainDialogProc);
 Ctl3dUnregister(instance);
 DestroyCursor(hHourGlassCursor);
 return 0;
}

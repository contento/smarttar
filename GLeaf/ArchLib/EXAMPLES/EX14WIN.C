/*= Archive Library v2.12 ================================================
 |
 |  EXAMPLES\EX14WIN.C
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  EX14WIN.C compresses a file while giving you a whole gaggle of options
 |  on how to do it.  You select the input file, a compression engine, and
 |  a compression type (archive, compressed object, or raw compress).  You
 |  then tell the program to perform the compression, and away it goes.
 |  It makes the compressed object in memory.  This program is a little
 |  contrived, because it is demonstrating a few slightly offbeat functions
 |  that are hard to exploit in a small example.
 |
 |  This program is functionally equivalent to EX14WIN.CPP.  The C version
 |  uses the translation layer functions to get the job done, but if
 |  you put them side by side, you won't see too much difference.
 |
 +- Notes ----------------------------------------------------------------
 |  #define AL_USING_DLL if your are linking to an Archive Library 16-bit
 |  import library.
 |  #define ZIP if you wish to create WIN00.ZIP.
 |
 +- Functions ------------------------------------------------------------
 |  deleteALEngine()
 |  ALEngineCompress()
 |  ALEngineDecompress()
 |  ALEngineGetStatusCode()
 |  ALEngineGetStatusDetail()
 |  ALEngineGetStatusString()
 |  ALEngineGetTypeCode()
 |  ALEngineGetTypeString()
 |  ALEngineSetError()
 |  newALCopyEngine()
 |  newALGreenleafEngine()
 |  ALEntrySetEngine()
 |  ALMonitorSetObjectStart()
 |  ALMonitorSetObjectSize()
 |
 +=======================================================================*/

#define STRICT
#include <windows.h>
#include <stdlib.h>
#include <assert.h>

#define AL_USING_DLL
#include "al.h"
#include "ex14win.h"

/*
  This program demonstrates the Append() function, using the overloaded
  version that appends files to an archive.

  This example could be rewritten to be a little simpler, for example by
  using a library function to load the list box with the archive entries.
*/

int iInstanceNumber;
HINSTANCE hInstance;
HWND hDlgMain;
hALMonitor monitor = 0;

BOOL AL_EXPORT CALLBACK AboutDialogProc(HWND hDlg,
                                        UINT message,
                                        WPARAM wParam,
                                        LPARAM);
void SizeFramingWindow(HWND hDlg);
void MakeArchive(char *file_name, hALCompressor compressor);
void MakeCompressedObject(char *file_name, hALCompressor compressor);
void MakeRawFile(char *file_name, hALCompressor compressor, hALDecompressor decompressor);
void ReadDir(HWND hDlg);

/*
  When it is time to create the compressed whatever, I will always
  have to get the file name.  This procedure takes care of figuring out what
  item in the list box is selected, then extracting that name and putting
  it into a buffer.
*/

char *GetFileName(HWND hDlg)
{
 char *buf;
 int i;
 int len;

 i = (int) SendDlgItemMessage(hDlg, AL_INPUT_FILES, LB_GETCURSEL, 0, 0);
 if (i == LB_ERR)
  return 0;
 len = (int) SendDlgItemMessage(hDlg, AL_INPUT_FILES, LB_GETTEXTLEN, i, 0);
 if (len == LB_ERR)
  return 0;
 buf = malloc(len + 1);
 if (buf == 0)
  return 0;
 SendDlgItemMessage(hDlg, AL_INPUT_FILES, LB_GETTEXT, i, (LPARAM) (LPSTR) buf);
 return buf;
}

/*
  One other thing I need to do when it is time to compress a file is to get
  a compression engine.  This routine takes care of that by creating a new
  engine, which will either by the copy engine or the greenleaf engine,
  depending on which check box the user clicked.
*/

hALCompressor GetCompressor(HWND hDlg)
{
 hALCompressor compressor;
 if (IsDlgButtonChecked(hDlg, AL_COPY_ENGINE))
  compressor = newALCopyCompressor();
 else if (IsDlgButtonChecked(hDlg, AL_DEFLATE_ENGINE))
  compressor = newALPkCompressor(AL_DEFAULT, AL_DEFAULT, AL_DEFAULT);
 else if (IsDlgButtonChecked(hDlg, AL_GREENLEAF_ENGINE))
  compressor = newALGlCompressor(AL_GREENLEAF_LEVEL_2, 0);
 else
  return 0;
 SetDlgItemText(hDlg, AL_ENGINE_TYPE_STRING, ALCompressorGetTypeString(compressor));
 SetDlgItemInt(hDlg, AL_ENGINE_TYPE_INT, ALCompressorGetTypeCode(compressor), 0);
 SetDlgItemText(hDlg, AL_ENGINE_STATUS_DETAIL, "");
 SetDlgItemText(hDlg, AL_ENGINE_STATUS_INT, "");
 return compressor;
}

hALDecompressor GetDecompressor(HWND hDlg)
{
 hALDecompressor decompressor;
 if (IsDlgButtonChecked(hDlg, AL_COPY_ENGINE))
  decompressor = newALCopyDecompressor();
 else if (IsDlgButtonChecked(hDlg, AL_DEFLATE_ENGINE))
  decompressor = newALPkDecompressor();
 else if (IsDlgButtonChecked(hDlg, AL_GREENLEAF_ENGINE))
  decompressor = newALGlDecompressor(AL_GREENLEAF_LEVEL_2);
 else
  return 0;
 return decompressor;
}

/*
  Filling up the list box with the contents of the current directory
  is pretty easy when you have a nifty class like ALWildCardExpander
  around to take care of the hard parts.
*/

void ReadDir(HWND hDlg)
{
 char dir_mask[128];
 hALExpander files;
 char AL_DLL_FAR *p;

 GetDlgItemText(hDlg, AL_DIR_MASK, dir_mask, 128);
 SendDlgItemMessage(hDlg, AL_INPUT_FILES, LB_RESETCONTENT, 0, 0);

 files = newALExpander(dir_mask, 0, AL_LOWER);
 while ((p = ALExpanderGetNextFile(files)) != 0)
  SendDlgItemMessage(hDlg,
                     AL_INPUT_FILES,
                     LB_ADDSTRING,
                     0,
                     (LPARAM) ((LPSTR) p));
 deleteALExpander(files);
}

/*
  If the user selected the option to make an archive, this routine
  will be the one that gets called.  It gets the appropriate file
  name, compression engine, then creates a list, creates a new entry
  for the list, and does the job.

  The tricky bit down at the bottom is there to avoid having the
  destructor for the compression engine called.  We don't want to
  do this, because somebody else created the engine, and they might not
  want it deleted when the list goes away.
*/

void MakeArchive(char *file_name, hALCompressor compressor)
{
 hALStorage archive_file;
 hALArchive archive;
 hALEntryList list;
 hALEntry entry;

 archive_file = newALMemory("Archive file in memory", 0, 0);
#if defined (ZIP)
 archive = newALPkArchiveFromStorage(archive_file);
#else
 archive = newALGlArchiveFromStorage(archive_file);
#endif
 list = newALListCopyTools(monitor);
 entry = newALEntry(list, newALFile(file_name), compressor, 0);
 ALArchiveCreate(archive, list);
 assert(ALEntryGetCompressor(entry) == compressor);
 ALEntrySetCompressor(entry, 0);                                               /* The tricky bit */
 deleteALEntryList(list);
 deleteALArchive(archive);
 deleteALStorage(archive_file);
}

/*
  If the user wants a compressed object instead of an archive, this routine
  gets called instead.  It sets up the monitor to track the progress for this
  type of operation, which is a little bit instructive.
*/

void MakeCompressedObject(char *file_name, hALCompressor compressor)
{
 hALStorage archive_file;
 hALCompressed object;
 hALStorage file;

 archive_file = newALMemory("Compressed Object in memory", 0, 0);
 object = newALCompressed(archive_file, compressor, 0);
 file = newALFile(file_name);
 ALStorageSetMonitor(file, monitor);
 ALMonitorSetObjectStart(monitor, 0);
 ALMonitorSetObjectSize(monitor, - 1);
 ALCompressedInsert(object, file);
 deleteALStorage(file);
 deleteALCompressed(object);
 deleteALStorage(archive_file);
}

/*
  Raw compression is a lot like making a compressed object, except in
  this case we have to call the compression engine directly.  Once
  again, we use a monitor here.
*/

void MakeRawFile(char *file_name, hALCompressor compressor, hALDecompressor
  decompressor)
{
 hALStorage output_file;
 hALStorage input_file;
 hALStorage temp_file;

 output_file = newALMemory("Archive file in memory", 0, 0);
 input_file = newALFile(file_name);
 ALStorageSetMonitor(input_file, monitor);
 ALMonitorSetObjectStart(monitor, 0);
 ALMonitorSetObjectSize(monitor, - 1);
 ALCompress(compressor, input_file, output_file);
 /*
   This is how I decompress afterwards.  I don't display the results,
   because my dialog is already too darned crowded.
 */
 temp_file = newALMemory("temp file in memory", 0, 0);
 ALDecompress(decompressor,
  output_file,
  temp_file,
  ALStorageGetSize(output_file));
 /* Force an error here! */
#if 0
  ALStorageOpen(temp_file);
  ALStorageWriteChar(temp_file, 0xff);
  ALStorageClose(temp_file);
#endif
 if (ALStorageCompare(input_file, temp_file) < AL_SUCCESS)
  ALCompressorSetError(compressor, AL_COMPARE_ERROR, "Comparison failed in MakeRawFile");

deleteALStorage(input_file);
deleteALStorage(output_file);
deleteALStorage(temp_file);
}

/*
  In this program, the main window is essentially a dialog box, and this
  procedure is the dialog box procedure.  It takes care of processing all
  the user input, like button presses.  This dialog box actually has a
  framing window around it, so it isn't the only window we have to worry
  about, but it is the only one that is really doing anything.
*/

BOOL AL_EXPORT CALLBACK MainDialogProc(HWND hDlg,
                                       UINT message,
                                       WPARAM wParam,
                                       LPARAM lParam)
{
 char *name;
 hALCompressor compressor;
 hALDecompressor decompressor;

 AL_UNUSED_PARAMETER(lParam);

 switch (message) {
  /*
    When we first come up, we have to set the title bar, check the default
    radio buttons, and fill the list box with the contents of the current
    directory.  As an aid to the rest of the program, we create a monitor
    here that can be used by everyone else whenever they need one.
  */
 case WM_INITDIALOG :
  SizeFramingWindow(hDlg);
  SetWindowText(hDlg, "Windows Example 14");
  SetDlgItemText(hDlg, AL_DIR_MASK, "*.DEF");
  CheckDlgButton(hDlg, AL_RAW_COMPRESS, 1);
  CheckDlgButton(hDlg, AL_GREENLEAF_ENGINE, 1);
  ReadDir(hDlg);
  monitor = newALWindowsMessage(AL_MONITOR_OBJECTS,
                                0,
                                AL_SEND_RATIO,
                                GetDlgItem(hDlg, AL_PROGRESS_BAR),
                                ALGaugeSetPosition);
  assert(monitor);
  return (TRUE);

  /* All the button presses and other neat stuff come in via WM_COMMAND. */
 case WM_COMMAND :
  switch (wParam) {
   /*
     If either one of these buttons is selected it means we are picking the
     kind of engine that is going to be used in the next pass of compression.
     We clear out all the engine text boxes in the dialog, since they will
     all be updated whenever the next compression pass takes place.
   */
  case AL_GREENLEAF_ENGINE :
  case AL_COPY_ENGINE :
   SetDlgItemText(hDlg, AL_ENGINE_TYPE_STRING, "");
   SetDlgItemText(hDlg, AL_ENGINE_TYPE_INT, "");
   SetDlgItemText(hDlg, AL_ENGINE_STATUS_DETAIL, "");
   SetDlgItemText(hDlg, AL_ENGINE_STATUS_INT, "");
   break;
   /*
     If somebody hits Return in the directory mask text box, we read a new
     copy of the directory.
   */
  case IDOK :                                                                  /* User has pressed enter */
   if (GetFocus() == GetDlgItem(hDlg, AL_DIR_MASK))
    ReadDir(hDlg);
   break;
  case AL_ABOUT : {
    FARPROC lpfnAboutDlgProc = MakeProcInstance((FARPROC) AboutDialogProc,
      hInstance);
    DialogBox(hInstance, "ALAboutDialog", 0, (DLGPROC) lpfnAboutDlgProc);
    (void) FreeProcInstance(lpfnAboutDlgProc);
  }
  return TRUE;
  /* A click on the read dir button causes just that to happen. */
 case AL_READ_DIR :
  ReadDir(hDlg);
  return TRUE;
  /*
    If the user clicks on compress, we go out and get the file name, and the
    engine, then call the appropriate one of the routines, depending on
    which radio button has been checked.
  */
 case AL_COMPRESS :
  EnableWindow(GetDlgItem(hDlg, AL_COMPRESS), 0);
  EnableWindow(GetDlgItem(hDlg, AL_EXIT), 0);
  name = GetFileName(hDlg);
  compressor = GetCompressor(hDlg);
  decompressor = GetDecompressor(hDlg);
  if (name && compressor) {
   if (IsDlgButtonChecked(hDlg, AL_COMPRESSED_OBJECT))
    MakeCompressedObject(name, compressor);
   else if (IsDlgButtonChecked(hDlg, AL_RAW_COMPRESS))
    MakeRawFile(name, compressor, decompressor);
   else if (IsDlgButtonChecked(hDlg, AL_ARCHIVE))
    MakeArchive(name, compressor);
   SetDlgItemText(hDlg, AL_ENGINE_STATUS_DETAIL, ALCompressorGetStatusDetail(compressor));
   SetDlgItemInt(hDlg, AL_ENGINE_STATUS_INT, ALCompressorGetStatusCode(compressor), 1);
  }
  if (name)
   free(name);
  if (compressor)
   deleteALCompressor(compressor);
  if (decompressor)
   deleteALDecompressor(decompressor);
  EnableWindow(GetDlgItem(hDlg, AL_COMPRESS), 1);
  EnableWindow(GetDlgItem(hDlg, AL_EXIT), 1);
  return TRUE;
 case AL_EXIT :
 case WM_QUIT :
 case WM_DESTROY :
  deleteALMonitor(monitor);
  PostMessage(GetParent(hDlg), WM_DESTROY, 0, 0);
  DestroyWindow(hDlgMain);
  hDlgMain = 0;
  return TRUE;
  }
  break;
 }
 return FALSE;
}

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
               (short int) ((GetSystemMetrics(SM_CXSCREEN) - (rc.right - rc.left)) / 2),
               (short int) ((GetSystemMetrics(SM_CYSCREEN) - (rc.bottom - rc.top)) / 2),
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

/*
  The main window is nothing more than a shell that manages the
  the dialog box, takes menu commands, and processes accelerator keys.
*/

LONG AL_EXPORT CALLBACK MainWndProc(HWND hWnd,
                                    UINT message,
                                    WPARAM wParam,
                                    LPARAM lParam)
{
 switch (message) {
 case WM_COMMAND :
  switch (wParam) {
  case AL_ABOUT : {
    FARPROC lpfnAboutDlgProc = MakeProcInstance((FARPROC) AboutDialogProc,
      hInstance);
    DialogBox(hInstance, "ALAboutDialog", 0, (DLGPROC) lpfnAboutDlgProc);
    (void) FreeProcInstance(lpfnAboutDlgProc);
  }
 return TRUE;
 case AL_EXIT :
  PostQuitMessage(0);
  }
  break;
 case WM_SETFOCUS :
  SetFocus(hDlgMain);                                                          /* insure that the Dialog Box has the focus */
  break;
 case WM_DESTROY :
  PostQuitMessage(0);
  break;
 }
 return DefWindowProc(hWnd, message, wParam, lParam);
}

/*
  WinMain calls the initialization routine for the progress gauge,
  and registers our class.  It then creates the dialog, and starts the
  message pump.
*/

int AL_WIN_MAIN_FAR PASCAL WinMain(HINSTANCE instance,
                                   HINSTANCE previous_instance,
                                   LPSTR lpStr,
                                   int nCmdShow)
{
 FARPROC lpfn;
 HWND hWnd;
 MSG msg;

 AL_UNUSED_PARAMETER(lpStr);
 hInstance = instance;
 if (!ALGaugeInit(instance, previous_instance))
  return FALSE;
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
                     "Windows Example 14",
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

/*= Archive Library v2.12 ================================================
 |
 |  EXAMPLES\EX14WIN.CPP
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  EX14WIN.CPP compresses a file while giving you a whole gaggle of options
 |  on how to do it.  You select the input file, a compression engine, and
 |  a compression type (archive, compressed object, or raw compress).  You
 |  then tell the program to perform the compression, and away it goes.  It
 |  makes the compressed object in memory.  This program is a little
 |  contrived, because it is demonstrating a few slightly offbeat functions
 |  that are hard to exploit in a small example.
 |
 +- Notes ----------------------------------------------------------------
 |  #define AL_USING_DLL if your are linking to an Archive Library 16-bit
 |  import library.
 |  #define ZIP if you wish to create WIN00.ZIP.
 |
 +- Functions ------------------------------------------------------------
 |  ALCompressionEngine::~ALCompressionEngine()
 |  ALEngine::Compress()
 |  ALEngine::Decompress()
 |  ALCopyEngine::ALCopyEngine()
 |  ALGreenleafEngine::ALGreenleafEngine()
 |
 +=======================================================================*/

#define STRICT
#include <windows.h>
#include <stdarg.h>

#define AL_USING_DLL
#include "al.h"
#include "ex14win.h"

int iInstanceNumber;
HINSTANCE hInstance;
HWND hDlgMain;
ALArchive *pArchive = 0;
ALWindowsMessage *monitor = 0;

BOOL AL_EXPORT CALLBACK AboutDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM);
void SizeFramingWindow(HWND hDlg);
void MakeArchive(char *file_name, ALCompressor *compressor);
void MakeCompressedObject(char *file_name, ALCompressor *compressor);
void MakeRawFile(char *file_name, ALCompressor *compressor, ALDecompressor *decompressor);
void ReadDir(HWND hDlg);

// When it is time to create the compressed whatever, I will always
// have to get the file name.  This guy takes care of figuring out what
// item in the list box is selected, then extracting that item and putting
// it into a buffer.

char *GetFileName(HWND hDlg)
{
 char *buf;

 int i = (int) SendDlgItemMessage(hDlg, AL_INPUT_FILES, LB_GETCURSEL, 0, 0);
 if (i == LB_ERR)
  return 0;
 int len = (int) SendDlgItemMessage(hDlg, AL_INPUT_FILES, LB_GETTEXTLEN, i, 0);
 if (len == LB_ERR)
  return 0;
 buf = new char[len + 1];
 if (buf == 0)
  return 0;
 SendDlgItemMessage(hDlg, AL_INPUT_FILES, LB_GETTEXT, i, (LPARAM) (LPSTR) buf);
 return buf;
}

// One other thing I need to do when it is time to compress a file is to get
// a compression engine.  This routine takes care of that by creating a new
// engine, which will either by the copy engine or the greenleaf engine,
// depening on which check box the user clicked.

ALCompressor *GetCompressor(HWND hDlg)
{
 ALCompressor *compressor;
 if (IsDlgButtonChecked(hDlg, AL_COPY_ENGINE))
  compressor = new ALCopyCompressor;
 else if (IsDlgButtonChecked(hDlg, AL_DEFLATE_ENGINE))
  compressor = new ALPkCompressor;
 else if (IsDlgButtonChecked(hDlg, AL_GREENLEAF_ENGINE))
  compressor = new ALGlCompressor;
 else
  return 0;
 SetDlgItemText(hDlg, AL_ENGINE_TYPE_STRING, compressor->mszCompressionType);
 SetDlgItemInt(hDlg, AL_ENGINE_TYPE_INT, compressor->miCompressionType, 0);
 SetDlgItemText(hDlg, AL_ENGINE_STATUS_DETAIL, "");
 SetDlgItemText(hDlg, AL_ENGINE_STATUS_INT, "");
 return compressor;
}

ALDecompressor *GetDecompressor(HWND hDlg)
{
 ALDecompressor *decompressor;
 if (IsDlgButtonChecked(hDlg, AL_COPY_ENGINE))
  decompressor = new ALCopyDecompressor;
 else if (IsDlgButtonChecked(hDlg, AL_DEFLATE_ENGINE))
  decompressor = new ALPkDecompressor;
 else if (IsDlgButtonChecked(hDlg, AL_GREENLEAF_ENGINE))
  decompressor = new ALGlDecompressor;
 else
  return 0;
 return decompressor;
}

// Filling up the list box with the contents of the current directory
// is pretty easy when you have a nifty class like ALWildCardExpander
// around to take care of the hard parts.

void ReadDir(HWND hDlg)
{
 char dir_mask[128];

 GetDlgItemText(hDlg, AL_DIR_MASK, dir_mask, 128);
 SendDlgItemMessage(hDlg, AL_INPUT_FILES, LB_RESETCONTENT, 0, 0);
 ALWildCardExpander files(dir_mask);
 char AL_DLL_FAR *p;
 while ((p = files.GetNextFile()) != 0)
  SendDlgItemMessage(hDlg,
                     AL_INPUT_FILES,
                     LB_ADDSTRING,
                     0,
                     (LPARAM) ((LPSTR) p));

}

// If the user selected the option to make an archive, this routine
// will be the one that gets called.  It gets the appropriate file
// name, compression engine, then creates a list, creates a new entry
// for the list, and does the job.

// The tricky bit down at the bottom is there to avoid having the
// destructor for the compression engine called.  We don't want to
// do this, because somebody else created the engine, and they might not
// want it deleted when the list goes away.

void MakeArchive(char *file_name, ALCompressor *compressor)
{
 ALWinMemory archive_file("Archive file in memory");
#if defined (ZIP)
 ALPkArchive archive(archive_file);
 ALEntryList list(monitor, PkTools());
#else
 ALGlArchive archive(archive_file);
 ALEntryList list(monitor, PkTools());
#endif
 ALEntry *entry = new ALEntry(list, new ALFile(file_name), compressor, 0);
 archive.Create(list);
 entry->mpCompressor = 0;                                                      //I have to do this to avoid having
 // engine deleted
}

// If the user wants a compressed object instead of an archive, this routine
// gets called instead.  It sets up the monitor to track the progress for this
// type of operation, which is a little bit instructive.

void MakeCompressedObject(char *file_name, ALCompressor *compressor)
{
#if defined (AL_FLAT_MODEL)
 ALMemory archive_file("Archive file in memory");
#else
 ALHugeMemory archive_file("Archive file in memory");
#endif
 ALCompressedObject object(archive_file, compressor, 0);
 ALFile file(file_name);
 file.mpMonitor = monitor;
 monitor->mlObjectStart = 0;
 monitor->mlObjectSize = -1;
 object.Insert(file);

 // How do you force an error here?  Start this program, get
 // a list box full of files.  Select a file to compress,
 // then, switch to a DOS box, and delete the file.  When
 // you try to make the compressed object, bingo! error.

 if (object.mStatus < AL_SUCCESS) {
  MessageBox(0, "Error in compressed object!", "ArchiveLib Error", MB_OK);
  object.ClearError();
 }
}

// Raw compression is a lot like making a compressed object, except in
// this case we have to call the compression engine directly.  Once
// again, we use a monitor here.

void MakeRawFile(char *file_name,
                 ALCompressor *compressor,
                 ALDecompressor *decompressor)
{
 ALWinMemory output_file("Archive file in memory");
 ALFile input_file(file_name);
 input_file.mpMonitor = monitor;
 monitor->mlObjectStart = 0;
 monitor->mlObjectSize = -1;
 compressor->Compress(input_file, output_file);

 // This is how I decompress afterwards.  I don't display the results,
 // because my dialog is already too darned crowded.

#if defined (AL_FLAT_MODEL)
 ALMemory temp_file;
#else
 ALHugeMemory temp_file;
#endif
 output_file.mpMonitor = monitor;
 monitor->mlObjectStart = 0;
 monitor->mlObjectSize = -1;
 decompressor->Decompress(output_file, temp_file, output_file.GetSize());

 // Force an error here!

#if 0
 temp_file.Open();
 temp_file.WriteChar(0xff);
 temp_file.Close();
#endif
 if (input_file.Compare(temp_file) < AL_SUCCESS)
  compressor->mStatus.SetError(AL_COMPARE_ERROR, "Comparison failed in MakeRawFile");
}

// In this program, the main window is essentially a dialog box, and this
// procedure is the dialog box procedure.  It takes care of processing all
// the user input, like button presses.  This dialog box actually has a
// framing window around it, so it isn't the only window we have to worry
// about, but it is the only one that is really doing anything.

BOOL AL_EXPORT CALLBACK MainDialogProc(HWND hDlg,
                                       UINT message,
                                       WPARAM wParam,
                                       LPARAM)
{
 char *name;
 ALCompressor *compressor;
 ALDecompressor *decompressor;

 switch (message) {
 case WM_INITDIALOG :

  // When we first come up, we have to set the title bar, check the default
  // radio buttons, and fill the list box with the contents of the current
  // directory.  As an aid to the rest of the program, we create a monitor
  // here that can be used by everyone else whenever they need one.

  SizeFramingWindow(hDlg);
  SetWindowText(hDlg, "Windows Example 14");
  SetDlgItemText(hDlg, AL_DIR_MASK, "*.DEF");
  CheckDlgButton(hDlg, AL_RAW_COMPRESS, 1);
  CheckDlgButton(hDlg, AL_GREENLEAF_ENGINE, 1);
  ReadDir(hDlg);
  monitor = new ALWindowsMessage(AL_MONITOR_OBJECTS,
                                 0,
                                 AL_SEND_RATIO,
                                 GetDlgItem(hDlg, AL_PROGRESS_BAR),
                                 ALGaugeSetPosition);
  AL_ASSERT(monitor, "Memory allocation failure creating monitor!");
  return (TRUE);

  // All the button presses and other neat stuff come in via WM_COMMAND.

 case WM_COMMAND :
  switch (wParam) {

   // If either one of these buttons is selected it means we are picking the
   // kind of engine that is going to be used in the next pass of compression.
   // We clear out all the engine text boxes in the dialog, since they will
   // all be updated whenever the next compression pass takes place.

  case AL_DEFLATE_ENGINE :
  case AL_GREENLEAF_ENGINE :
  case AL_COPY_ENGINE :
   SetDlgItemText(hDlg, AL_ENGINE_TYPE_STRING, "");
   SetDlgItemText(hDlg, AL_ENGINE_TYPE_INT, "");
   SetDlgItemText(hDlg, AL_ENGINE_STATUS_DETAIL, "");
   SetDlgItemText(hDlg, AL_ENGINE_STATUS_INT, "");
   break;

   // If somebody hits Return in the directory mask text box, we read a new
   // copy of the directory.

  case IDOK :                                                                  //User has pressed enter
   if (GetFocus() == GetDlgItem(hDlg, AL_DIR_MASK))
    ReadDir(hDlg);
   break;
  case AL_ABOUT : {
   FARPROC lpfnAboutDlgProc = MakeProcInstance((FARPROC) AboutDialogProc, hInstance);
   DialogBox(hInstance, "ALAboutDialog", 0, (DLGPROC) lpfnAboutDlgProc);
   (void) FreeProcInstance(lpfnAboutDlgProc);
  }
  return TRUE;

  // A click on the read dir button causes just that to happen.

 case AL_READ_DIR :
  ReadDir(hDlg);
  return TRUE;

  // If the user clicks on compress, we go out and get the file name, and the
  // engine, then call the appropriate one of the routines, depending on
  // which radio button has been checked.

 case AL_COMPRESS :
  EnableWindow(GetDlgItem(hDlg, AL_COMPRESS), 0);
  EnableWindow(GetDlgItem(hDlg, AL_EXIT), 0);
  name = GetFileName(hDlg);
  compressor = GetCompressor(hDlg);
  decompressor = GetDecompressor(hDlg);
  if (name && compressor && decompressor) {
   if (IsDlgButtonChecked(hDlg, AL_COMPRESSED_OBJECT))
    MakeCompressedObject(name, compressor);
   else if (IsDlgButtonChecked(hDlg, AL_RAW_COMPRESS))
    MakeRawFile(name, compressor, decompressor);
   else if (IsDlgButtonChecked(hDlg, AL_ARCHIVE))
    MakeArchive(name, compressor);
   SetDlgItemText(hDlg, AL_ENGINE_STATUS_DETAIL, compressor->mStatus.GetStatusDetail());
   SetDlgItemInt(hDlg, AL_ENGINE_STATUS_INT, (int) compressor->mStatus, 1);
  }
  delete[] name;
  delete compressor;
  delete decompressor;
  EnableWindow(GetDlgItem(hDlg, AL_COMPRESS), 1);
  EnableWindow(GetDlgItem(hDlg, AL_EXIT), 1);
  return TRUE;
 case AL_EXIT :
 case WM_QUIT :
 case WM_DESTROY :
  delete monitor;
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
AboutDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM)
{
 switch (message) {
 case WM_INITDIALOG :
  RECT rc;
  GetWindowRect(hDlg, &rc);
  SetWindowPos(hDlg,
               NULL,
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

// The main window is nothing more than a shell that manages the
// the dialog box, takes menu commands, and processes accelerator keys.

LONG AL_EXPORT CALLBACK MainWndProc(HWND hWnd,
                                    UINT message,
                                    WPARAM wParam,
                                    LPARAM lParam)
{
 switch (message) {
 case WM_COMMAND :
  switch (wParam) {
  case AL_ABOUT : {
   FARPROC lpfnAboutDlgProc = MakeProcInstance((FARPROC) AboutDialogProc, hInstance);
   DialogBox(hInstance, "ALAboutDialog", 0, (DLGPROC) lpfnAboutDlgProc);
   (void) FreeProcInstance(lpfnAboutDlgProc);
  }
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
// and registers our class.  It then creates the dialog, and starts the
// message pump.

int AL_WIN_MAIN_FAR PASCAL WinMain(HINSTANCE instance,
                                   HINSTANCE previous_instance,
                                   LPSTR,
                                   int nCmdShow)
{
 hInstance = instance;
 if (!ALGaugeInit(instance, previous_instance))
  return FALSE;
 if (previous_instance == 0) {
  WNDCLASS wc;
  wc.style = 0;
  wc.lpfnWndProc = MainWndProc;
  wc.cbClsExtra = 0;
  wc.cbWndExtra = 0;
  wc.hInstance = instance;
  wc.hIcon = LoadIcon(hInstance, "ALIcon");
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground =(HBRUSH) GetStockObject(WHITE_BRUSH);
  wc.lpszMenuName = "ALMenu";
  wc.lpszClassName = "ALClass";
  if (!RegisterClass(&wc))
   return FALSE;
 }
 HWND hWnd = CreateWindow("ALClass",
                          "Windows Example 14",
                          WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
                          CW_USEDEFAULT, 0, 0, 0, NULL, NULL, hInstance, NULL);

 FARPROC lpfn = MakeProcInstance((FARPROC) MainDialogProc, hInstance);
 hDlgMain = CreateDialog(hInstance, "ALMainDialog", hWnd, (DLGPROC) lpfn);
 ShowWindow(hWnd, (short) nCmdShow);
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
 SetWindowPos(GetParent(hDlg), NULL, (short) x, (short) y,
              (short int) (rect.right - rect.left),
              (short int) (rect.bottom - rect.top + GetSystemMetrics(SM_CYMENU) +
               GetSystemMetrics(SM_CYCAPTION)), SWP_NOZORDER);
}

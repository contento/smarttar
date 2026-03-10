/*= Archive Library v2.12 ================================================
 |
 |  EXAMPLES\EX14PM.CPP
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  EX14PM.CPP compresses a file while giving you a whole gaggle of options
 |  on how to do it.  You select the input file, a compression engine, and
 |  a compression type (archive, compressed object, or raw compress).  You
 |  then tell the program to perform the compression, and away it goes.  It
 |  makes the compressed object in memory.  This program is a little
 |  contrived, because it is demonstrating a few slightly offbeat functions
 |  that are hard to exploit in a small example.
 |
 +- Notes ----------------------------------------------------------------
 |  #define ZIP if you wish to use ZIP compression.
 |
 +- Functions ------------------------------------------------------------
 |  ALCompressionEngine::~ALCompressionEngine()
 |  ALEngine::Compress()
 |  ALEngine::Decompress()
 |  ALCopyEngine::ALCopyEngine()
 |  ALGreenleafEngine::ALGreenleafEngine()
 |
 +=======================================================================*/

#define INCL_WIN
#define INCL_WINDIALOGS
#define INCL_WINPOINTERS

#include <os2.h>
#include <stdio.h>
#include <string.h>

#include "ex14pm.h"
#include "arclib.h"
#include "pmmon.h"
#include "glarc.h"
#include "glengn.h"
#include "pkarc.h"
#include "pkengn.h"
#include "copyengn.h"
#include "filestor.h"
#include "memstore.h"
#include "wildcard.h"

ALMonitor *monitor = 0;
void ReadDir(HWND hDlg)
{
 char dir_mask[128];

 WinQueryDlgItemText(hDlg, AL_WILDSPEC, 128, dir_mask);
 WinSendDlgItemMsg(hDlg, AL_DIRECTORY, LM_DELETEALL, 0, 0);
 ALWildCardExpander files(dir_mask);
 char *p;
 while ((p = files.GetNextFile()) != 0)
  WinSendDlgItemMsg(hDlg,
                    AL_DIRECTORY,
                    LM_INSERTITEM,
                    MPFROMSHORT(LIT_SORTASCENDING),
                    MPFROMP((PSZ) p));
}

char *GetFileName(HWND hDlg)
{
 char *buf;

 int i = (int) WinSendDlgItemMsg(hDlg, AL_DIRECTORY, LM_QUERYSELECTION, 0, 0);
 if (i == LIT_NONE)
  return 0;
 int len = (int) WinSendDlgItemMsg(hDlg,
                                   AL_DIRECTORY,
                                   LM_QUERYITEMTEXTLENGTH,
                                   MPFROMSHORT((SHORT) i),
                                   0);
 buf = new char[len + 1];
 if (buf == 0)
  return 0;
 WinSendDlgItemMsg(hDlg,
                   AL_DIRECTORY,
                   LM_QUERYITEMTEXT,
                   MPFROM2SHORT((short) i,
                   (short) len),
                   MPFROMP((PSZ) buf));
 return buf;
}

ALCompressor *GetCompressor(HWND hDlg)
{
 ALCompressor *compressor;
 if (WinSendDlgItemMsg(hDlg, AL_COPY, BM_QUERYCHECK, 0, 0) == (MRESULT) 1)
  compressor = new ALCopyCompressor;
 else if (WinSendDlgItemMsg(hDlg, AL_PK, BM_QUERYCHECK, 0, 0) == (MRESULT) 1)
  compressor = new ALPkCompressor;
 else if (WinSendDlgItemMsg(hDlg, AL_GREENLEAF, BM_QUERYCHECK, 0, 0) == (MRESULT) 1)
  compressor = new ALGlCompressor;
 else
  return 0;
 WinSetDlgItemText(hDlg, AL_ENGINE_TYPE_STRING, compressor->mszCompressionType);
 WinSetDlgItemShort(hDlg, AL_ENGINE_TYPE_INT, (short int) compressor->miCompressionType, 0);
 WinSetDlgItemText(hDlg, AL_ENGINE_STATUS_DETAIL, "");
 WinSetDlgItemText(hDlg, AL_ENGINE_STATUS_INT, "");
 return compressor;
}

ALDecompressor *GetDecompressor(HWND hDlg)
{
 ALDecompressor *decompressor;
 if (WinSendDlgItemMsg(hDlg, AL_COPY, BM_QUERYCHECK, 0, 0) == (MRESULT) 1)
  decompressor = new ALCopyDecompressor;
 else if (WinSendDlgItemMsg(hDlg, AL_PK, BM_QUERYCHECK, 0, 0) == (MRESULT) 1)
  decompressor = new ALPkDecompressor;
 else if (WinSendDlgItemMsg(hDlg, AL_GREENLEAF, BM_QUERYCHECK, 0, 0) == (MRESULT) 1)
  decompressor = new ALGlDecompressor;
 else
  return 0;
 return decompressor;
}

void MakeCompressedObject(char *file_name, ALCompressor *compressor)
{
 ALMemory archive_file("Archive file in memory");
 ALCompressedObject object(archive_file, compressor, 0);
 ALFile file(file_name);
 file.mpMonitor = monitor;
 monitor->mlObjectStart = 0;
 monitor->mlObjectSize = -1;
 object.Insert(file);
 file.mpMonitor = 0;
}

void MakeRawFile(char *file_name,
                 ALCompressor *compressor,
                 ALDecompressor *decompressor)
{
 ALMemory output_file("Archive file in memory");
 ALFile input_file(file_name);
 input_file.mpMonitor = monitor;
 monitor->mlObjectStart = 0;
 monitor->mlObjectSize = -1;
 compressor->Compress(input_file, output_file);
 input_file.mpMonitor = 0;

 // This is how I decompress afterwards.  I don't display the results,
 // because my dialog is already too darned crowded.

 ALMemory temp_file;
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

void MakeArchive(char *file_name, ALCompressor *compressor)
{
 ALMemory archive_file("Archive file in memory");
#if defined (ZIP)
 ALPkArchive archive(archive_file);
 ALEntryList list(monitor, PkTools());
#else
 ALGlArchive archive(archive_file);
 ALEntryList list(monitor, PkTools());
#endif
 ALEntry * entry = new ALEntry(list, new ALFile(file_name), compressor, 0);
 archive.Create(list);
 entry->mpCompressor = 0;
}

void Compress(HWND hDlg)
{
 char *name;
 ALCompressor *compressor;
 ALDecompressor *decompressor;

 WinEnableWindow(WinWindowFromID(hDlg, AL_COMPRESS), 0);
 WinEnableWindow(WinWindowFromID(hDlg, AL_EXIT), 0);
 name = GetFileName(hDlg);
 compressor = GetCompressor(hDlg);
 decompressor = GetDecompressor(hDlg);
 if (name && compressor && decompressor) {
  if (WinSendDlgItemMsg(hDlg, AL_COMPRESSED_OBJECT, BM_QUERYCHECK, 0, 0) == (MRESULT) 1)
   MakeCompressedObject(name, compressor);
  else if (WinSendDlgItemMsg(hDlg, AL_RAW_COMPRESSION, BM_QUERYCHECK, 0, 0) == (MRESULT) 1)
   MakeRawFile(name, compressor, decompressor);
  else if (WinSendDlgItemMsg(hDlg, AL_ARCHIVE, BM_QUERYCHECK, 0, 0) == (MRESULT) 1)
   MakeArchive(name, compressor);
  WinSetDlgItemText(hDlg,
                    AL_ENGINE_STATUS_DETAIL,
                    compressor->mStatus.GetStatusDetail());
                    WinSetDlgItemShort(hDlg,
                    AL_ENGINE_STATUS_INT,
                    (short int) compressor->mStatus,
                    1);
 }
 if (name)
  delete[] name;
 if (compressor)
  delete compressor;
 if (decompressor)
  delete decompressor;
 WinEnableWindow(WinWindowFromID(hDlg, AL_COMPRESS), 1);
 WinEnableWindow(WinWindowFromID(hDlg, AL_EXIT), 1);
}

MRESULT EXPENTRY AboutBoxDialogProc(HWND hwndDlg,
                                    ULONG msg,
                                    MPARAM mp1,
                                    MPARAM mp2)
{
 switch (msg) {
 case WM_INITDLG :
  TrimSystemMenu(hwndDlg);
  CenterWindow(hwndDlg);
  break;
 default :
  return WinDefDlgProc(hwndDlg, msg, mp1, mp2);
 }
 return (MRESULT) FALSE;
}

MRESULT EXPENTRY MainDialogProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
 switch (msg) {
 case WM_INITDLG :
  WinSetDlgItemText(hwnd, AL_WILDSPEC, "*.CPP");
  ReadDir(hwnd);
  WinSendDlgItemMsg(hwnd, AL_GREENLEAF, BM_SETCHECK, MPFROM2SHORT(TRUE, 0), 0);
  WinSendDlgItemMsg(hwnd, AL_RAW_COMPRESSION, BM_SETCHECK, MPFROM2SHORT(TRUE, 0), 0);
  SetupSlider(hwnd, AL_PROGRESS_BAR, 0); // Must do this first!!!
  monitor = new ALOs2Message(AL_MONITOR_OBJECTS,
                             WinWindowFromID(hwnd, AL_PROGRESS_TEXT),
                             AL_SEND_RATIO,
                             WinWindowFromID(hwnd, AL_PROGRESS_BAR),
                             SLM_SETSLIDERINFO);
  break;
 case WM_COMMAND :
  switch (LOUSHORT(mp1)) {
  case AL_ABOUT :
   WinDlgBox(HWND_DESKTOP,
             hwnd,
             AboutBoxDialogProc,
             NULLHANDLE,
             AL_ABOUT_DIALOG,
             0);
   break;
  case AL_READ_DIR :
   ReadDir(hwnd);
   break;
  case AL_COMPRESS :
   Compress(hwnd);
   break;
  case AL_EXIT :
   WinPostMsg(hwnd, WM_QUIT, 0, 0);
   return FALSE;
  }
  break;
 case WM_CLOSE :
  WinPostMsg(hwnd, WM_QUIT, 0, 0);
  return FALSE;
 default :
  return WinDefDlgProc(hwnd, msg, mp1, mp2);
 }
 return (MRESULT) FALSE;
}

int main()
{
 HAB hab;
 HMQ hmq;
 HWND hwndDlg;
 QMSG qmsg;
 HPOINTER hIcon;

 hab = WinInitialize(0);
 hmq = WinCreateMsgQueue(hab, 0);
 hIcon = WinLoadPointer(HWND_DESKTOP, 0, AL_ICON);
 hwndDlg = WinLoadDlg(HWND_DESKTOP, // parent
                      HWND_DESKTOP, // owner
                      MainDialogProc, // dialog window proc
                      0, // module handle
                      AL_MAIN_DIALOG, // dialog template ID
                      0); // No application data
 WinSendMsg(hwndDlg, WM_SETICON, (MPARAM) hIcon, 0);
 CenterWindow(hwndDlg);
 if (hwndDlg)
  while (WinGetMsg(hab, &qmsg, NULLHANDLE, 0, 0))
   WinDispatchMsg(hab, &qmsg);
 WinDestroyWindow(hwndDlg);
 WinDestroyMsgQueue(hmq);
 WinTerminate(hab);
 return 0;
}

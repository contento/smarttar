/*= Archive Library v2.12 ================================================
 |
 |  EXAMPLES\EX23PM.CPP
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  EX23PM.CPP is used to benchmark the performance of our various
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
 |  EX23PM.CPP does one funky thing to try to give us real fast
 |  performance.  Instead of using the default buffer size of 4096
 |  bytes, I jack it up to a hefty 28672 bytes.  This gives us a few
 |  percentage points.  Just change the definition of BIG_BUFFER to
 |  benchmark at the default size.
 |
 +=======================================================================*/

#define INCL_WIN
#define INCL_WINDIALOGS
#define INCL_WINPOINTERS

#include <os2.h>
#include <stdio.h>
#include <string.h>

#include "ex23pm.h"
#include "arclib.h"
#include "pkengn.h"
#include "glengn.h"
#include "arclib.h"
#include "pmmon.h"
#include "wildcard.h"
#include "filestor.h"

#if !defined (BIG_BUFFER)
 #define BIG_BUFFER 28672
#endif

HAB hab;
ALStorage *compressed = 0;

// The time class is a very modest convenience.

class TIME {
 public :
 long milliseconds;
 TIME() { milliseconds = WinGetCurrentTime(hab); }
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

 char *s = new char[25];
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

void Compress(HWND hDlg)
{
 char input_name[128];
 char buf[128];
 long total_input = 0;
 long total_compressed = 0;
 int compress_only = (int) WinSendDlgItemMsg(hDlg,
                                             AL_COMPRESS_ONLY,
                                             BM_QUERYCHECK,
                                             0,
                                             0);

 ALOs2Message monitor(AL_MONITOR_OBJECTS,
                      WinWindowFromID(hDlg, AL_FILE),
                      AL_SEND_BYTE_COUNT,
                      WinWindowFromID(hDlg, AL_BYTE_COUNT));
 monitor.mlObjectStart = 0;
 WinEnableWindow(WinWindowFromID(hDlg, AL_COMPRESS), 0);
 WinQueryDlgItemText(hDlg, AL_INPUT_FILES, 128, input_name);
 ALWildCardExpander expander(input_name);
#if defined (ZIP)
 ALPkCompressor compressor;
 ALPkDecompressor decompressor;
#else
 ALGlCompressor compressor;
 ALGlDecompressor decompressor;
#endif

// This code clears out all the boxes on the dialog that might be left
// over from the last run of this program.

 WinSendDlgItemMsg(hDlg, AL_RESULTS, LM_DELETEALL, 0, 0);
 WinSetDlgItemText(hDlg, AL_TOTAL_INPUT, "");
 WinSetDlgItemText(hDlg, AL_TOTAL_COMPRESSED, "");
 WinSetDlgItemText(hDlg, AL_RATIO, "");
 WinSetDlgItemText(hDlg, AL_TOTAL_TIME, "");

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
  WinSetDlgItemText(hDlg, AL_FILE, (PSZ) filename);
  input.mpMonitor = &monitor;
  compressor.Compress(input, *compressed);
  if (compressed->mStatus < AL_SUCCESS)
   break;
  if (!compress_only) {
   input.mpMonitor = 0;
   output.mpMonitor = &monitor;
   decompressor.Decompress(*compressed, output);
   output.Delete ();
   if (compressed->mStatus < AL_SUCCESS)
    break;
  }
  total_input += input.GetSize();
  total_compressed += compressed->GetSize();
  TIME local_stop;
  char *s;
  sprintf(buf,
          "%14.14s  %9ld  %9ld  %6d%%  %s",
          (PSZ) filename,
          input.GetSize(),
          compressed->GetSize(),
          ratio(input.GetSize(), compressed->GetSize()),
          (PSZ)(s = format_time(local_stop - local_start)));
  if (s)
  delete[] s;

  // I stuff the file stats in the list box, then make sure that the currently
  // selected position in the list box is on the screen.

  int pos = (int) WinSendDlgItemMsg(hDlg,
                                    AL_RESULTS,
                                    LM_INSERTITEM,
                                    MPFROMSHORT(LIT_END),
                                    MPFROMP((PSZ) buf));
 WinSendDlgItemMsg(hDlg,
                   AL_RESULTS,
                   LM_SELECTITEM,
                   MPFROMSHORT((short) pos),
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
 WinSetDlgItemText(hDlg, AL_TOTAL_INPUT, buf);
 sprintf(buf, "%ld", total_compressed);
 WinSetDlgItemText(hDlg, AL_TOTAL_COMPRESSED, buf);
 sprintf(buf, "%d%%", ratio(total_input, total_compressed));
 WinSetDlgItemText(hDlg, AL_RATIO, buf);
 char *s = format_time(global_stop - global_start);
 if (s) {
  sprintf(buf, "%s", s = format_time(global_stop - global_start));
  WinSetDlgItemText(hDlg, AL_TOTAL_TIME, buf);
  delete[] s;
 } else
  WinSetDlgItemText(hDlg, AL_TOTAL_TIME, "");
 WinEnableWindow(WinWindowFromID(hDlg, AL_COMPRESS), 1);
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
  case AL_COMPRESS :
   Compress(hwnd);
   break;
  case AL_ABORT :
   if (compressed != 0)
    compressed->mStatus.SetError(AL_USER_ABORT, "User pressed abort key");
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
 HMQ hmq;
 HWND hwndDlg;
 QMSG qmsg;
 HPOINTER hIcon;

 hab = WinInitialize(0);
 hmq = WinCreateMsgQueue(hab, 0);
 hIcon = WinLoadPointer(HWND_DESKTOP, 0, AL_ICON);
 hwndDlg = WinLoadDlg(HWND_DESKTOP,                                            /* parent                 */
                      HWND_DESKTOP,                                                                /* owner                  */
                      MainDialogProc,                                                              /* dialog window proc     */
                      0,                                                                           /* module handle          */
                      AL_MAIN_DIALOG,                                                              /* dialog template ID     */
                      0);                                                                          /* No application data    */
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

/*= Archive Library v2.12 ================================================
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  EX23CON is used to benchmark the performance of our various
 |  compression engines.  It is called with a single command line argument,
 |  which is a wildcard specification for a group of files.  The program
 |  expands the wild card specification and individually compresses and
 |  decompresses each of the files.  The temporary files created by this
 |  process are deleted.
 |
 |  After the whole batch of files is done, a summary of the compression
 |  time and statistics is displayed.
 |
 |  If the optional -c argument is appended to the command line, the
 |  program runs in "compress-only" mode.  In this case, the decompression
 |  state is skipped.  You can use this to determine the ratio between
 |  compression and decompresion times.
 |
 |  This example program does one thing to try to give us real fast
 |  performance.  Instead of using the default buffer size of 4096
 |  bytes, I jack it up to a hefty 28672 bytes. This gives us a few
 |  percentage points.  Just change the definition of BIG_BUFFER to
 |  benchmark at the default size.
 |
 +- Functions ------------------------------------------------------------
 |  ALEngine::Compress()
 |  ALEngine::Decompress()
 |
 +- Notes ----------------------------------------------------------------
 |  The default version of this program doesn't use a monitor.
 |  To test the effects of a monitor on program performance, just
 |  #define MONITOR.
 |
 |  This is a non-destructive test!  None of the input files are
 |  modified at any time!
 |
 +========================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <iostream.h>
#include <conio.h>

#include "al.h"
#if !defined (AL_OS2)
 #include <dos.h>
#endif

// This program runs under standard MS-DOS and extended DOS.  Under
// extended DOS, we use the Windows API to get the time of day.  To
// make the source code less messy, the details of the time of day
// are concealed in this class.

class TIME {
 public :
 long milliseconds;
 TIME();
 TIME(long t) { milliseconds = t; }
 TIME operator - (TIME &);
};

// The time constructor does the job of initializing the time of day.
// It has to be done differently under Extended DOS as opposed to
// standard real mode DOS.

#if defined (AL_WINDOWS)

 inline TIME::TIME(void)
 {
  milliseconds = GetTickCount();
 }

#elif defined (AL_OS2)

 inline TIME::TIME(void)
 {
  DATETIME t;
  DosGetDateTime(&t);
  milliseconds = t.hundredths * 10L;
  milliseconds += t.seconds * 1000L;
  milliseconds += t.minutes * 1000L * 60L;
  milliseconds += t.hours * 1000L * 60L * 60L;
 }

#else

 inline TIME::TIME(void)
 {
  #if defined (AL_SYMANTEC)
   struct dos_time_t t;
  #else
   struct dostime_t t;
  #endif
  _dos_gettime(&t);
  milliseconds = t.hsecond * 10L;
  milliseconds += t.second * 1000L;
  milliseconds += t.minute * 1000L * 60L;
  milliseconds += t.hour * 1000L * 60L * 60L;
 }

#endif

// To determine how long an operation took, I need to calculate the
// difference of two times.  This is strictly a convenience.

TIME TIME::operator - (TIME &rhs)
{
 return TIME(milliseconds - rhs.milliseconds);
}

// A standard function for calculating the compression ratio of a file,
// give the input and output sizes.

inline int ratio(long plain, long compressed)
{
 if (plain == 0)
  return 0;
 compressed *= 100;
 return (int) (100 - (compressed / plain));
}

// There are many places in the code where I want to send the time
// to an output stream.  This function takes care of that.

ostream& operator << (ostream& s, TIME t)
{
 long m = t.milliseconds;
 int hours;
 int minutes;
 int seconds;
 int hsecs;

 hours = (int) (m / (1000L * 60L * 60L));
 m -= hours * (1000L * 60L * 60L);
 minutes = (int) (m / (1000L * 60L));
 m -= minutes * (1000L * 60L);
 seconds = (int) (m / (1000L));
 m -= seconds * 1000L;
 hsecs = (int) (m / 10L);
 (void) s.width(2);
 char temp = s.fill('0');
 s << (int) hours << ':';
 (void) s.width(2);
 s <<(int) minutes << ':';
 (void) s.width(2);
 s << (int) seconds << '.';
 (void) s.width(2);
 s << (int) hsecs;
 s.fill(temp);
 return s;
}

// main() is kind of long, but it should be pretty straightforward.
// Most of the body of the code is grunt code to either create
// nice looking output or to collect statistics.

#if !defined (BIG_BUFFER)
 #define BIG_BUFFER 28672
#endif

int main(int argc, char *argv[])
{
 long total_input = 0;
 long total_compressed = 0;
 int compress_only_flag = 0;
 int greenleaf_flag = 0;
 int bad = 0;

 while (argc >= 2) {
  if (stricmp(argv[1], "-c") == 0) {
   argc--;
   argv++;
   compress_only_flag = 1;
  } else if (stricmp(argv[1], "-g") == 0) {
   argc--;
   argv++;
   greenleaf_flag = 1;
  } else
  break;
 }
 cout.rdbuf()->setbuf((char *) 0, 0); // Some compilers buffer cout

 if (argc < 2) {
  cout << "Usage: EX23CON [-c] [-g] file[s]\n\n"
       << "  -c   compress files only, decompress pass is skipped\n"
       << "  -g   specifies the Greenleaf compression engine\n";
  exit(1);
 }

 ALWildCardExpander expander(argv[1]);
 ALCompressor *compressor;
 ALDecompressor *decompressor;

 if (greenleaf_flag) {
  compressor = new ALGlCompressor(AL_GREENLEAF_LEVEL_4);
  decompressor = new ALGlDecompressor(AL_GREENLEAF_LEVEL_4);
  cout << "\nUsing the Greenleaf engine\n";
 } else {
  #if defined (AL_FLAT_MODEL)
   compressor = new ALPkCompressor(6, 15, 8);
  #else
   compressor = new ALPkCompressor(6, 14, 7);
  #endif
  decompressor = new ALPkDecompressor;
  cout << "Using the ZIP compatible engine\n";
 }

 char *name;
 TIME global_start;
 ALSpinner monitor(AL_MONITOR_OBJECTS, cout);

 // This is the big loop where we compress all the files.

 while ((name = expander.GetNextFile()) != 0) {
  TIME local_start;
  ALFile input(name, BIG_BUFFER);
  ALFile compressed("", BIG_BUFFER);

  // Print out the name of the file, sans drive and path

  ALName filename(name);
  filename.StripPath();
  (void) cout.width(13);
  cout << filename;
  #if defined (MONITOR)
   compressed.mpMonitor = &monitor;
   monitor.mlObjectStart = 0;
  #endif
  compressor->Compress(input, compressed);
  cout << " ";
  (void) cout.width(9);
  cout << input.GetSize() << " ";
  (void) cout.width(9);
  cout << compressed.GetSize();
  ALFile output("", BIG_BUFFER);
  if (!compress_only_flag)
   decompressor->Decompress(compressed, output);
  total_input += input.GetSize();
  total_compressed += compressed.GetSize();
  (void) cout.width(4);
  cout << " ";
  (void) cout.width(4);
  cout << ratio(input.GetSize(), compressed.GetSize()) << "%";
  TIME local_stop;
  cout << "  " <<(local_stop - local_start);
  if (!compress_only_flag) {
   int test = output.Compare(input);
   if (test < 0)
    bad++;
   cout << " compare: " << (test ? "Fail" : "Pass");
  }
  cout << endl;
  if (!compress_only_flag)
   output.Delete();
  compressed.Delete();
 }
 TIME global_stop;
 cout << "\nTotal input     : " << total_input << " bytes.\n";
 cout << "Total compressed: " << total_compressed << " bytes.\n";
 cout << "Ratio           : " << ratio(total_input, total_compressed) << "%\n";
 cout << "Total time      : " << (global_stop - global_start) << endl;
 cout << "Total failures  : " << bad << endl;
 return 0;
}

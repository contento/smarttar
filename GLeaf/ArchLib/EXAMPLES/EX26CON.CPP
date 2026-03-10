/*= Archive Library v2.12 ================================================
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  EX26COn demonstrates ALExtract(), one of the ZIP file manipulation
 |  functions in the simplified interface.  It creates a directory named
 |  "temp", then extracts the contents of CON00.ZIP into that directory.
 |  Note that EX00CON.CPP can be used to create CON00.ZIP.
 |
 |  This program also demonstrates the use of the callback function
 |  in the simplified interface.
 |
 +- Functions ------------------------------------------------------------
 |  ALExtract()
 |  ALReadDir()
 |  ALFreeDir()
 |
 +========================================================================*/

#include <iostream.h>
#include <iomanip.h>
#include <conio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#if defined (AL_BORLAND)
 #include <dir.h>
#else
 #include <direct.h>
#endif

#include "alsimple.h"

// The callback function for the simplified interface gets
// called at at two different times.  First, when an object
// is being added to the archive, the callback function
// is called with a valid filename in parameter name,
// and -1 in the two numeric paramters.  Second, at various
// times during the compression cycle, it is called with
// a 0 (null pointer) for the parameter name, and valid ratios
// in the two numeric parameters.
//
// This simple callback function prints file names, and prints
// both ratios during progress.

void my_callback(const char AL_DLL_FAR *name,
 int object_ratio,
 int job_ratio)
{
 if (name)
  cout << "\n" << name << "  ";
 if (object_ratio >= 0) {
  char buf[24];
  sprintf(buf, "%d%%  %d%%", object_ratio, job_ratio);
  cout << "          \b\b\b\b\b\b\b\b\b\b" << buf;
  for (int i = 0; i < (int) strlen(buf); i++)
   cout << '\b';
 }
 cout.flush();
}

main()
{
 cout << "Archive Library 2.11\nEX26CON.CPP\n\n";
 cout << "Press ESC to abort, any other key to continue..." << flush;
 int c = getch();
 cout << "\n";
 if (c == 0 || c == 0x1b || c == 3)
  exit(1);

 mkdir("temp");
 if (chdir("temp") == 0) {
  struct ALZipDir *z = ALReadDir("..\\con00.zip", 0, 0);
  if (z != 0) {
   int i = ALExtract(z, 0, my_callback);
   cout << "\n\nALExtract returned " << i << "\n";
   ALFreeDir(z);
  } else
  cout << "\n\nError reading zip file directory\n";
  chdir("..");
 }
 return 0;
}

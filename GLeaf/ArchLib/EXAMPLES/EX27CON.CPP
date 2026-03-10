/*= Archive Library v2.12 ================================================
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  EX27CON demonstrates ALDelete(), one of the ZIP file manipulation
 |  functions in the simplified interface.  It goes through the directory
 |  read in from CON00.ZIP, and marks every other file for deletion.  It
 |  then calls ALDelete() to actually delete the objects.  You can create
 |  a copy of CON00.ZIP with EX00CON.CPP.
 |
 |  This program also demonstrates the use of the callback function
 |  in the simplified interface.
 |
 +- Functions ------------------------------------------------------------
 |  ALDelete()
 |  ALReadDir()
 |  ALFreeDir()
 |
 +========================================================================*/

#include <iostream.h>
#include <iomanip.h>
#include <conio.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

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
// Note that the monitor function acts kind of funky when you
// are performing a delete operation.  Instead of writing the
// names of files that it is deleting, it prints the names
// of the files that are being kept.  That's because those are
// the files that are being copied from the old archive to the
// new archive.

void my_callback(const char AL_DLL_FAR *name, int object_ratio, int job_ratio)
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
}

main()
{
 cout << "Archive Library 2.11\nEX27CON.CPP\n\n";
 cout << "Press ESC to abort, any other key to continue..." << flush;
 int c = getch();
 cout << "\n\n";
 if (c == 0 || c == 0x1b || c == 3)
  exit(1);
 cout << endl;

 int i;
 int count;
 struct ALZipDir *z = ALReadDir("con00.zip", &count, 0);
 if (z != 0) {
  for (i = 0; i < count; i++)
   if (i % 2)
    z[i].mark = 0;
   else
    cout << "Deleting " << z[i].name << endl;
   int j = ALDelete(z, my_callback);
   cout << "\n\nALDelete returned " << j << endl;
   ALFreeDir(z);
 } else
 cout << "\n\nError reading zip file directory\n";
 return 0;
}

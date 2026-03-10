/*= Archive Library v2.12 ================================================
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  This example program demonstrates several functions in the
 |  ArchiveLib simplified interface.  It first adds a set of files
 |  to an archive using ALCreate().  It then reads the directory
 |  for the archive and prints its contents.  It then appends
 |  additional files to the archive.
 |
 |  This program also demonstrates the use of the callback function
 |  in the simplified interface.
 |
 +- Functions ------------------------------------------------------------
 |  ALCreate()
 |  ALReadDir()
 |  ALFreeDir()
 |  ALAppend()
 |
 +========================================================================*/

#include <conio.h>
#include <iostream.h>
#include <iomanip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
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
 cout.flush();
}

// A simple function that is used when I want to display a ratio

int ratio(struct ALZipDir *z)
{
 return int (100L - ((100L * z->compressed_size) / (long) z->size));
}

main()
{
 cout << "Archive Library 2.11\nEX25CON.CPP\n\n";
 cout << "Press ESC to abort, any other key to continue..." << flush;
 int c = getch();
 cout << "\n\n";
 if (c == 0 || c == 0x1b || c == 3)
  exit(1);

 int i;
 int count;
 int error;
 struct ALZipDir *z;
 char *levels[] = { "Stored ",
  "DeflatN",
  "DeflatX",
  "DeflatF",
  "DeflatS" };
 cout << "Creating CON25.ZIP with *.CPP\n";
 i = ALCreate("con25.zip", "*.cpp", 0, my_callback);
 printf("\nALCreate() returned %d\n\n", i);
 z = ALReadDir("con25.zip", &count, &error);
 if (z != 0 && error == AL_SUCCESS)
  printf("Contents of CON25.ZIP:\n\n");
 for (i = 0; i < count; i++)
  printf("%7ld %s %7ld %3d%% %02d-%02d-%04d %02d:%02d %s %s\n",
   z[i].size,
   levels[z[i].level],
   z[i].compressed_size,
   ratio(z + i),
   z[i].month,
   z[i].date,
   z[i].year,
   z[i].hour,
   z[i].minute,
   z[i].name,
   z[i].comment);
 ALFreeDir(z);
 printf("\nAppending c:\\command.* to CON25.ZIP\n");
 i = ALAppend("con25.zip", "c:\\command.*", 1, my_callback);
 printf("\n\n");
 printf("ALAppend() returned %d\n\n", i);
 cout << "Press any key to exit..." << flush;
 getch();

 return i;
}

/*= Archive Library v2.12 ================================================
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  EX28CON demonstrates ALSetName() and ALSetComment(), two of the ZIP
 |  file manipulation functions in the ArchiveLib simplified interface. It
 |  walks through the directory of CON00.ZIP, and prepends a "temp/" string
 |  to each of the objects in the directory.  A subsequent call to
 |  ALExtract() would then extract the files to a temp directory, instead
 |  of the current directory.
 |
 +- Functions ------------------------------------------------------------
 |  ALReadDir()
 |  ALFreeDir()
 |  ALSetName()
 |  ALSetComment()
 |
 +- Notes ----------------------------------------------------------------
 |  pkzip -vc con00.zip to view comments.
 |
 +========================================================================*/

#include <iostream.h>
#include <iomanip.h>
#include <conio.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "alsimple.h"

// A simple function that is used when I want to display a ratio

inline int ratio(struct ALZipDir *z)
{
 return int (100L - ((100L * z->compressed_size) / (long) z->size));
}

main()
{
 cout << "Archive Library 2.11\nEX28CON.CPP\n\n";
 cout << "Press ESC to abort, any other key to continue..." << flush;
 int c = getch();
 cout << "\n\n";
 if (c == 0 || c == 0x1b || c == 3)
  exit(1);

 cout << "Modifying names and comments in con00.zip\n";
 ALZipDir *z = ALReadDir("con00.zip", 0, 0);
 int i;
 if (z != 0) {
  for (i = 0; z[i].size != - 1; i++) {
   char *p = new char[(strlen(z[i].name) + 10)];
   strcpy(p, "temp/");
   strcat(p, z[i].name);
   ALSetName(z + i, p);
   delete[] p;
   char buf[81];
   sprintf(buf, "Modified path for entry %d", i);
   ALSetComment(z + i, buf);
  }
  ALWriteDir(z);
  ALFreeDir(z);
 } else
 cout << "Error reading zip file directory\n";
 int count;
 int error;
 z = ALReadDir("con00.zip", &count, &error);
 if (z != 0) {
  cout << "Contents of con00.zip:\n\n" << flush;
  for (i = 0; i < count; i++)
   printf("%7ld %7ld %3d%% %-20s %s\n",
    z[i].size,
    z[i].compressed_size,
    ratio(z + i),
    z[i].name,
    z[i].comment);
 ALFreeDir(z);
 } else
 cout << "Error reading zip file directory\n";
 return 0;
}

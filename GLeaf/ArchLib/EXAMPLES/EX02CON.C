/*= Archive Library v2.12 ================================================
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  EX02CON adds all EX??WIN.CPP files to the existing CON00.GAL or
 |  CON00.ZIP archive.  If you have not already created CON00.GAL or
 |  CON00.ZIP, you can do so easily by executing EX00CON or EX01CON.
 |
 +- Functions ------------------------------------------------------------
 |  ALArchiveAppend()
 |  ALEntryListAddWildCardFiles()
 |  newALGlArchive()
 |  newALListGlCompressFileTools()
 |  newALListPkCompressFileTools()
 |  newALPkArchive()
 |  newALSpinner()
 |
 +- Notes ----------------------------------------------------------------
 |  Compile with -DZIP to create CON00.ZIP.
 |
 +========================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

#include "arclib.h"
#include "pkarc.h"
#include "glarc.h"
#include "spinner.h"

int main()
{
 hALArchive archive;
 hALMonitor monitor;
 hALEntryList list;
 int c;

 printf("Archive Library 2.11\nEX02CON.C\n\n");
 printf("Press ESC to abort, any other key to continue...");
 fflush(stdout);
 c = getch();
 printf("\n\n");
 if (c == 0 || c == 0x1b || c == 3)
  exit(1);

 monitor = newALSpinner(AL_MONITOR_OBJECTS);

 #if defined (ZIP)
  printf("Appending all ex??win.c files to con00.zip.\n\n");
  archive = newALPkArchive("con00.zip");
  list = newALListPkCompressFileTools(monitor, 9, 13, 6);
 #else
  printf("Appending all ex??win.cpp files to con00.gal.\n\n");
  archive = newALGlArchive("con00.gal");
  list = newALListGlCompressFileTools(monitor, AL_GREENLEAF_LEVEL_1);
 #endif

 ALEntryListAddWildCardFiles(list, "ex??win.c", 0);
 c = ALArchiveAppend(archive, list);
 printf("%d files were appended.\n", c);

 deleteALMonitor(monitor);
 deleteALEntryList(list);
 deleteALArchive(archive);

 return c;
}

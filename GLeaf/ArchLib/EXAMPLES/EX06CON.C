/*= Archive Library v2.12 ================================================
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  EX06CON creates a list of files using the ability of ALWildCardExpander
 |  to traverse subdirectories when expanding wild card file specifications.
 |  Of course, you might not have any subdirectories when you run this.
 |  If not, try creating some and putting *.CPP and *.OBJ files in some of
 |  them.  Or even better, change the file spec to "..\\*.CPP" to get
 |  lots-o-files.
 |
 +- Functions ------------------------------------------------------------
 |  ALArchiveGetComment()
 |  ALArchiveReadDirectory()
 |  ALEntryGetCompressedSize()
 |  ALEntryGetMark()
 |  ALEntryGetNextEntry()
 |  ALEntryListGetFirstEntry()
 |  ALStorageGetName()
 |  ALStorageGetSize()
 |
 +- Notes ----------------------------------------------------------------
 |  Compile with -DZIP to create CON06.ZIP.
 |
 +========================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

#include "arclib.h"
#include "bargraph.h"
#include "wildcard.h"
#include "glarc.h"
#include "pkarc.h"

int main()
{
 hALArchive archive;
 hALMonitor monitor;
 hALEntryList list;
 int c;

 printf("Archive Library 2.11\nEX06CON.C\n\n");
 printf("Press ESC to abort, any other key to continue...");
 fflush(stdout);
 c = getch();
 printf("\n");
 if (c == 0 || c == 0x1b || c == 3)
  exit(1);
 
 monitor = newALBarGraph(AL_MONITOR_OBJECTS);

#if defined (ZIP)
 printf("\nRecursively adding *.c to con06.zip\n");
 archive = newALPkArchive("con06.zip");
 list = newALListPkTools(monitor, AL_DEFAULT, AL_DEFAULT, AL_DEFAULT);
#else
 printf("\nRecursively adding *.c to con06.gal\n");
 archive = newALGlArchive("con06.gal");
 list = newALListGlTools(monitor, AL_DEFAULT);
#endif

 ALEntryListAddWildCardFiles(list, "*.c", 1);

 ALArchiveCreate(archive, list);

 deleteALArchive(archive);
 deleteALEntryList(list);
 deleteALMonitor(monitor);

 return 0;
}

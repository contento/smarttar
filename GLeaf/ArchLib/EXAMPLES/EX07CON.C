/*= Archive Library v2.12 ================================================
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  EX07CON extracts files from CON00.GAL or CON00.ZIP, and puts them
 |  in a new subdirectory called TEMP.  In order to make the files
 |  go to the subdirectory, we have to walk through the entry list and
 |  change the name of each and every object so it includes the TEMP\
 |  directory specification.
 |
 +- Functions ------------------------------------------------------------
 |  ALArchiveExtract()
 |  ALEntryListGetFirstEntry()
 |  ALArchiveReadDirectory()
 |  ALEntryGetNextEntry()
 |  ALEntryGetStorage()
 |  ALStorageSetName()
 |  newALBarGraph()
 |
 +- Notes ----------------------------------------------------------------
 |  Compile with -DZIP to use CON00.ZIP.
 |
 +========================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>
#include <conio.h>

#include "al.h"

int main()
{
 hALArchive archive;
 hALMonitor monitor;
 hALEntryList list;
 hALEntry entry;
 int c;

 printf("Archive Library 2.11\nEX07CON.C\n\n");
 printf("Press ESC to abort, any other key to continue...");
 fflush(stdout);
 c = getch();
 printf("\n\n");
 if (c == 0 || c == 0x1b || c == 3)
  exit(1);

 monitor = newALBarGraph(AL_MONITOR_OBJECTS);
#if defined (ZIP)
 archive = newALPkArchive("con00.zip");
 list = newALListPkDecompressFileTools(monitor);
#else
 archive = newALGlArchive("con00.gal");
 list = newALListGlDecompressFileTools(monitor);
#endif

 mkdir("temp");
 ALArchiveReadDirectory(archive, list);
 entry = ALEntryListGetFirstEntry(list);
 while (entry) {
  char name[80];
  strcpy(name, "temp\\");
  strcat(name, ALStorageGetName(ALEntryGetStorage(entry)));
  printf("New name: %s\n", name);
  ALStorageSetName(ALEntryGetStorage(entry), name);
  entry = ALEntryGetNextEntry(entry);
 }
 ALArchiveExtract(archive, list);

 deleteALMonitor(monitor);
 deleteALArchive(archive);
 deleteALEntryList(list);

 return 0;
}

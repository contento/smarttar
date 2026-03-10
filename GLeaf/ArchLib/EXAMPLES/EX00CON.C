/*= Archive Library v2.12 ================================================
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  EX00CON creates an archive and adds files to it, while using an
 |  ALBarGraph monitor.  It adds files to either a CON00.GAL or
 |  CON00.ZIP.  Any existing CON00.GAL or CON00.ZIP gets replaced.
 |
 +- Functions ------------------------------------------------------------
 |  ALArchiveGetStatusCode()
 |  ALArchiveGetStorage()
 |  ALEntryListAddWildCardFiles()
 |  ALStorageGetName()
 |  newALBarGraph()
 |  newALGlArchive()
 |  newALListGlCompressTools()
 |  newALListPkCompressTools()
 |  newALPkArchive()
 |
 +- Notes ----------------------------------------------------------------
 |  Compile with -DZIP to create CON00.ZIP.
 |
 +========================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

#include "arclib.h"
#include "bargraph.h"
#include "pkarc.h"
#include "glarc.h"

main()
{
 hALArchive archive;
 hALMonitor monitor;
 hALEntryList list;
 int c;
 
 printf("Archive Library 2.11\nEX00CON.C\n\n");
 printf("Press ESC to abort, any other key to continue...");
 fflush(stdout);
 c = getch();
 printf("\n\n");
 if (c == 0 || c == 0x1b || c == 3)
  exit(1);

 monitor = newALBarGraph(AL_MONITOR_OBJECTS);

 #if defined (ZIP)
  archive = newALPkArchive("con00.zip");
  list = newALListPkCompressTools(monitor, 6, 13, 6);
 #else
  archive = newALGlArchive("con00.gal");
  list = newALListGlCompressTools(monitor, AL_GREENLEAF_LEVEL_4);
 #endif

 ALEntryListAddWildCardFiles(list, "*.c", 0);
 c = ALArchiveCreate(archive, list);
 printf("%d files added to %s using a newALBarGraph monitor.\n",
  c, ALStorageGetName(ALArchiveGetStorage(archive)));
 printf("Create() returned %s\n", ALArchiveGetStatusDetail(archive));

 deleteALMonitor(monitor);
 deleteALEntryList(list);
 deleteALArchive(archive);

 return 0;
}

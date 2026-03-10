/*= Archive Library v2.12 ================================================
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  EX03CON uses the existing CON00.GAL archive (create it with EX01CON
 |  if you don't have it) and deletes every other entry.  Note that when
 |  we call Delete(), we specify an Archive with an empty name, which
 |  causes ALArchiveDelete() to rename the input to a backup, and rename
 |  the output to the original input name.
 |
 +- Functions ------------------------------------------------------------
 |  ALArchiveDelete()
 |  ALEntryGetNextEntry()
 |  ALEntryListClearMarks()
 |  ALEntryListGetFirstEntry()
 |  ALEntrySetMark()
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

int main()
{
 hALArchive archive;
 hALArchive temp;
 hALEntryList list;
 hALEntry entry;
 long total = 0;
 int c;
 
 printf("Archive Library 2.11\nEX03CON.C\n\n");
 printf("Press ESC to abort, any other key to continue...");
 fflush(stdout);
 c = getch();
 printf("\n\n");
 if (c == 0 || c == 0x1b || c == 3)
  exit(1);

 #if defined (ZIP)
  printf("Deleting every other object from con00.zip\n");
  archive = newALPkArchive("con00.zip");
  temp = newALPkArchive("");
  list = newALListPkTools(0, 6, 13, 6);
 #else
  printf("Deleting every other object from con00.gal\n");
  archive = newALGlArchive("con00.gal");
  temp = newALGlArchive("");
  list = newALListGlTools(0, AL_GREENLEAF_LEVEL_4);
 #endif
 ALArchiveReadDirectory(archive, list);
 ALEntryListClearMarks(list, 0); /* Clear every mark */

 entry = ALEntryListGetFirstEntry(list);
 while (entry) {
  total++;
  if (total % 2) {
   ALEntrySetMark(entry);
   printf("Deleting: %s\n", ALStorageGetName(ALEntryGetStorage(entry)));
  }
  entry = ALEntryGetNextEntry(entry);
 }

 c = ALArchiveDelete(archive, list, temp);
 printf("%d entries were deleted.\n", c);

 deleteALArchive(archive);
 deleteALArchive(temp);
 deleteALEntryList(list);

 return c;
}

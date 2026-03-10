/*= Archive Library v2.12 ================================================
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  EX04CON demonstrates ArchiveLib's ability to set comments both for an
 |  entire archive as well as individual objects within the archive.  If
 |  you do not have a CON00.GAL or CON00.ZIP, you can create one using
 |  EX00CON or EX01CON.
 |
 +- Functions ------------------------------------------------------------
 |  ALArchiveReadDirectory()
 |  ALArchiveSetComment()
 |  ALArchiveWriteDirectory()
 |  ALEntryGetStorage()
 |  ALEntryListGetFirstEntry()
 |  ALEntrySetComment()
 |  ALStorageGetName()
 |  newALListFullTools()
 |
 +- Notes ----------------------------------------------------------------
 |  Compile with -DZIP to use CON00.ZIP.
 |  pkzip -vc CON00.ZIP to view comments.
 |
 +========================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

#include "al.h"

main()
{
 hALArchive archive;
 hALEntryList list;
 hALEntry entry;
 int c;

 printf("Archive Library 2.11\nEX04CON.C\n\n");
 printf("Press ESC to abort, any other key to continue...\n");
 fflush(stdout);
 c = getch();
 printf("\n");
 if (c == 0 || c == 0x1b || c == 3)
  exit(1);

 #if defined (ZIP)
  archive = newALPkArchive("con00.zip");
 #else
  archive = newALGlArchive("con00.gal");
 #endif
 list = newALListFullTools(0);
 ALArchiveReadDirectory(archive, list);
 printf("Setting archive comment\n");
 ALArchiveSetComment(archive, "Greenleaf was here.");

 entry = ALEntryListGetFirstEntry(list);
 printf("Setting comment for: %s\n",
  ALStorageGetName(ALEntryGetStorage(entry)));
 ALEntrySetComment(entry, "Greenleaf was here.");
 ALArchiveWriteDirectory(archive, list);

 deleteALArchive(archive);
 deleteALEntryList(list);

 return 0;
}

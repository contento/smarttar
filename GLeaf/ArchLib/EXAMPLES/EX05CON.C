/*= Archive Library v2.12 ================================================
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  EX05CON displays a brief listing of an archive's contents.  If you
 |  do not specify an archive name on the command line, by default, it
 |  will display the contents of CON00.GAL or CON00.ZIP.  If you don't
 |  have a copy of CON00.GAL or CON00.ZIP, you can create one using EX00CON.
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
 |  Compile with -DZIP to use CON00.ZIP.
 |  pkzip -vc CON00.ZIP to view comments.
 |
 +========================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>

#include "arclib.h"
#include "glarc.h"
#include "pkarc.h"

int dump(hALEntryList list);

int main(int argc, char *argv[])
{
 hALArchive archive;
 hALEntryList list;
 char *comment;
 int c;

 printf("Archive Library 2.11\nEX05CON.C\n\n");
 printf("Press ESC to abort, any other key to continue...");
 fflush(stdout);
 c = getch();
 if (c == 0 || c == 0x1b || c == 3)
  exit(1);

 if (argc == 2)
#if defined (ZIP)
  archive = newALPkArchive(argv[1]);
 else
  archive = newALPkArchive("con00.zip");
#else
  archive = newALGlArchive(argv[1]);
 else
  archive = newALGlArchive("con00.gal");
#endif
 list = newALListCopyTools(0);
 ALArchiveReadDirectory(archive, list);
 printf("\n");
 comment = ALArchiveGetComment(archive);
 if (comment)
 printf("Archive comment: %s\n", comment);
 dump(list);

 deleteALArchive(archive);
 deleteALEntryList(list);

 return 1;
}

int dump(hALEntryList list)
{
 long total = 0;
 long tsize = 0;
 long tcomp = 0;
 hALEntry entry;

 entry = ALEntryListGetFirstEntry(list);
 if (entry == 0) {
  printf("No entries in archive\n");
  return 1;
 }
 printf("==============================================================\n");
 printf("M  Name              Size        Comp   Comment\n");
 printf("--------------------------------------------------------------\n");
 while (entry != 0) {
  total++;
  tsize += ALStorageGetSize(ALEntryGetStorage(entry));
  tcomp += ALEntryGetCompressedSize(entry);
  printf(ALEntryGetMark(entry) ? "*" : " ");
  printf("%11s", ALStorageGetName(ALEntryGetStorage(entry)));
  printf("%13ld", ALStorageGetSize(ALEntryGetStorage(entry)));
  printf("%12ld", ALEntryGetCompressedSize(entry));
  printf("%22s", ALEntryGetComment(entry));
  printf("\n");
  entry = ALEntryGetNextEntry(entry);
 }
 printf("--------------------------------------------------------------\n");
 printf(" Total ");
 printf("%3ld", total);
 printf("%15ld", tsize);
 printf("%12ld\n", tcomp);

 return 0;
}

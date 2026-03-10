/*= Archive Library v2.12 ================================================
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  EX33CON shows a how to extract using ZIP decryption.
 |
 +- Functions ------------------------------------------------------------
 |  newALBarGraph()
 |  newALPkArchive()
 |  newALListPkDecompressFileTools()
 |  ALEntryListAddZipCrypto()
 |  ALArchiveReadDirectory()
 |  ALTranslateError()
 |  ALArchiveExtract()
 |
 +========================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <al.h>
#include <zipcrypt.h>

main()
{
 hALArchive archive;
 hALMonitor monitor;
 hALEntryList list;
 int ret, c;

 printf("Archive Library 2.11\nEX33CON.C\n\n");
 printf("Press ESC to abort, any other key to continue...");
 fflush(stdout);
 c = getch();
 printf("\n\n");
 if (c == 0 || c == 0x1b || c == 3)
  exit(1);

 monitor = newALBarGraph(AL_MONITOR_OBJECTS);
 archive = newALPkArchive("crypto00.zip");
 list = newALListPkDecompressFileTools(monitor);
 ALEntryListAddZipCrypto(list, "pass");
 if((ret = ALArchiveReadDirectory(archive, list)) < 0)
 {
  puts(ALTranslateError(ret));
  return -1;
 }
 if((ret = ALArchiveExtract(archive, list)) < 0)
 {
  puts(ALTranslateError(ret));
  return -1;
 }

 deleteALMonitor(monitor);
 deleteALArchive(archive);
 deleteALEntryList(list);

 return 0;
}

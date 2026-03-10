/*= Archive Library v2.12 ================================================
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  EX30CON shows a couple of different ways to encrypt entries in a
 |  ZIP file.
 |
 +- Functions ------------------------------------------------------------
 |  ALMemoryGetBuffer()
 |  ALCompress()
 |  ALStorageOpen()
 |  ALStorageReadChar()
 |
 +========================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include "arclib.h"
#include "pkarc.h"
#include "arclist.h"
#include "spinner.h"
#include "zipcrypt.h"

main()
{
 hALMonitor spinner;
 hALEntryList list00;
 hALEntryList list01;
 hALArchive arc00;
 hALArchive arc01;
 hALEntry entry;
 hALCrypto crypt;
 int count = 0;
 int c;

 printf("Archive Library 2.11\nEX30CON.C\n\n");
 printf("Press ESC to abort, any other key to continue...");
 fflush(stdout);
 c = getch();
 printf("\n\n");
 if (c == 0 || c == 0x1b || c == 3)
  exit(1);

 spinner = newALSpinner(AL_MONITOR_OBJECTS);

/*
  This method encrypts every file using the crypto engine
  found in the toolkit.
*/

 list00 = newALListPkTools(spinner, 6, 13, 6);
 ALEntryListAddZipCrypto(list00, "swordfish");
 ALEntryListAddWildCardFiles(list00, "*.c", 0);
 arc00 = newALPkArchive("crypto00.zip");
 printf("Creating crypto00.zip\n");
 ALArchiveCreate(arc00, list00);

/*
  I can use a different strategy to just encrypt some files, using
  the file name as a password.
*/

 list01 = newALListPkTools(spinner, 6, 13, 6);
 ALEntryListAddWildCardFiles(list01, "*.c", 0);
 arc01 = newALPkArchive("crypto01.zip");
 for (entry = ALEntryListGetFirstEntry(list01);
  entry;
  entry = ALEntryGetNextEntry(entry), count++)
  if (count % 2) {
   const char *name;
   hALStorage storage;
   storage = ALEntryGetStorage(entry);
   name = ALStorageGetName(storage);

   crypt = newALZipCrypto(name);
   ALEntrySetCryptoEngine(entry, crypt);
   deleteALCrypto(crypt);
  }
  printf("Creating crypto01.zip\n");
  ALArchiveCreate(arc01, list01);

 deleteALMonitor(spinner);
 deleteALEntryList(list01);
 deleteALEntryList(list00);
 deleteALArchive(arc01);
 deleteALArchive(arc00);

 return 1;
}

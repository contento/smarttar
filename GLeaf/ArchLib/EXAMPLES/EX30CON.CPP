/*= Archive Library v2.12 ================================================
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  EX30CON shows a couple of different ways to encrypt entries in a ZIP
 |  file.
 |
 +- Functions ------------------------------------------------------------
 |  ALEntryList::AddWildCardFiles()
 |  ALEntryList::GetFirstEntry()
 |  ALEntry::GetNextEntry()
 |
 +========================================================================*/

#include <iostream.h>
#include <stdlib.h>
#include <conio.h>
#include "arclib.h"
#include "pkarc.h"
#include "spinner.h"
#include "zipcrypt.h"

main()
{
 cout << "Archive Library 2.11\nEX30CON.CPP\n\n";
 cout << "Press ESC to abort, any other key to continue..." << flush;
 int c = getch();
 cout << "\n\n";
 if (c == 0 || c == 0x1b || c == 3)
  exit(1);

 ALSpinner spinner(AL_MONITOR_OBJECTS);

 // This method encrypts every file using the crypto engine
 // found in the toolkit.

 ALEntryList list00(&spinner, PkCompressTools() + ALZipCrypto("swordfish"));
 list00.AddWildCardFiles("*.cpp");
 ALPkArchive arc00("crypto00.zip");
 cout << "Creating crypto00.zip\n";
 arc00.Create(list00);

 // I can use a different strategy to just encrypt some files, using
 // the file name as a password.

 ALEntryList list01(&spinner, PkCompressTools());
 list01.AddWildCardFiles("*.cpp");
 ALPkArchive arc01("crypto01.zip");
 int count = 0;
 for (ALEntry *entry = list01.GetFirstEntry();
  entry;
  entry = entry->GetNextEntry(), count++)
  if (count % 2)
   entry->mpCrypto = new ALZipCrypto(entry->mpStorageObject->mName);
  cout << "Creating crypto01.zip\n";
  arc01.Create(list01);
  return 1;
}

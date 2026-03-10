/*= Archive Library v2.12 ================================================
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  EX32CON creates a ZIP file with the first entry encrypted using the
 |  filename as the password.  We then read in the list, check to see if
 |  an entry is encrypted, apply password if needed.
 |
 +- Functions ------------------------------------------------------------
 |  ALEntryList::AddWildCardFiles()
 |  ALEntryList::GetFirstEntry()
 |  ALEntry::GetNextEntry()
 |  ALEntry::GetEncrypted()
 |
 +========================================================================*/

#include <conio.h>
#include <stdlib.h>

#include "arclib.h"
#include "zipcrypt.h"
#include "pkarc.h"
#include "bargraph.h"

main()
{
 cout << "Archive Library 2.11\nEX32CON.CPP\n\n";
 cout << "Press ESC to abort, any other key to continue..." << flush;
 int c = getch();
 cout << "\n\n";
 if (c == 0 || c == 0x1b || c == 3)
  exit(1);

 ALEntry *entry;
 ALBarGraph monitor(AL_MONITOR_OBJECTS, cout);
 ALPkArchive archive("crypto00.zip");
 // list used to create
 ALEntryList list(&monitor, PkCompressTools());
 // list used to read in directory
 ALEntryList list2(&monitor, PkDecompressTools());
 // add two files to zip
 list.AddWildCardFiles("ex30con.cpp, ex32con.cpp");
 // encrypt first entry using the filename as the password
 entry = list.GetFirstEntry();
 entry->mpCrypto = new ALZipCrypto(entry->mpStorageObject->mName);
 cout << "Creating crypto00.zip\n";
 archive.Create(list);
 cout << archive.ReadDirectory(list2) << endl;

// Since the ALEntry data member, miEncrypted, is declared as a bit field,
// GetEncrypted can return one of two values.  0 if entry object is NOT
// encrypted.  -1 if entry object IS encrypted.

 for (entry = list2.GetFirstEntry(); entry; entry = entry->GetNextEntry())
 {
  // Here would be a good place to ask the user for a password.  Since
  // we already know it, just do it.
  if (entry->GetEncrypted())
   entry->mpCrypto = new ALZipCrypto(entry->mpStorageObject->mName);
 }
 archive.Extract(list2);
 cout << archive.mStatus.GetStatusString();
 return 0;
}

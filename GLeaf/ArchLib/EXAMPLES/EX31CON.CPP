/*= Archive Library v2.12 ================================================
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  EX31CON creates a ZIP file, then extracts all of its objects to
 |  ALMemory objects instead of ALFile objects.  To make sure the whole
 |  thing is working, we print out the resulting ALMemory object to the
 |  screen.
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
#include "pkengn.h"
#include "spinner.h"
#include "memstore.h"

main()
{
 cout << "Archive Library 2.11\nEX31CON.CPP\n\n";
 cout << "Press ESC to abort, any other key to continue..." << flush;
 int c = getch();
 cout << "\n\n";
 if (c == 0 || c == 0x1b || c == 3)
  exit(1);

 ALSpinner monitor(AL_MONITOR_OBJECTS);
 ALEntryList list(&monitor, PkTools());
 list.AddWildCardFiles("c:\\autoexec.bat, c:\\config.sys");
 ALPkArchive arc("con31.zip");
 cout << "Create returns: " << arc.Create(list) << endl;

 ALToolKit toolkit(ALPkDecompressor() + ALMemory());
 ALEntryList list2(&monitor, toolkit);

 arc.ReadDirectory(list2);

 cout << "Extract returns: " << arc.Extract(list2) << endl;
 cout << "\n\n";

 for (ALEntry *entry = list2.GetFirstEntry();
  entry;
  entry = entry->GetNextEntry()) {
  cout << "File: " << entry->mpStorageObject->mName << "\n\n";
  ALMemory *mem = (ALMemory *) entry->mpStorageObject;
  cout << mem->mpcUserBuffer;
  cout << "\nHit any key to continue...";
  getch();
  cout << endl;
 }
 return 1;
}

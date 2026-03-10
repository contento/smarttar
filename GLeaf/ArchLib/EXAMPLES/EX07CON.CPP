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
 |  ALArchiveBase::Extract()
 |  ALName::ALName()
 |  ALName::operator=()
 |  ALName::operator+()
 |
 +- Notes ----------------------------------------------------------------
 |  Compile with -DZIP to use CON00.ZIP.
 |
 +========================================================================*/

#include <conio.h>
#include <stdlib.h>
#include <direct.h>

#include "arclib.h"
#include "glarc.h"
#include "pkarc.h"
#include "bargraph.h"

main(int, char *[])
{
 cout << "Archive Library 2.11\nEX07CON.CPP\n\n";
 cout << "Press ESC to abort, any other key to continue..." << flush;
 int c = getch();
 cout << "\n\n";
 if (c == 0 || c == 0x1b || c == 3)
  exit(1);

 ALBarGraph monitor(AL_MONITOR_OBJECTS, cout);
 #if defined (ZIP)
  ALPkArchive archive("con00.zip");
  ALEntryList list(&monitor, PkDecompressTools());
 #else
  ALGlArchive archive("con00.gal");
  ALEntryList list(&monitor, GlDecompressTools());
 #endif

 archive.ReadDirectory(list);
 ALEntry *entry = list.GetFirstEntry();
 while (entry) {
  ALName name = entry->mpStorageObject->mName;
  name = ALName("temp\\") + (const char *) name;
  cout << "New name: " << name << endl;
  entry->mpStorageObject->mName = name;
  entry = entry->GetNextEntry();
 }

 return archive.Extract(list);
}

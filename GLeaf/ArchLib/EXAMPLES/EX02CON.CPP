/*= Archive Library v2.12 ================================================
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  EX02CON adds all EX??WIN.CPP files to the existing CON00.GAL or
 |  CON00.ZIP archive.  If you have not already created CON00.GAL or
 |  CON00.ZIP, you can do so easily by executing EX00CON.
 |
 +- Functions ------------------------------------------------------------
 |  ALArchive::AddWildCardFiles()
 |  ALArchive::Create()
 |  ALEntryList::ALEntryList()
 |  ALEntryList::ClearMarks()
 |  ALSpinner::ALSpinner()
 |
 +- Notes ----------------------------------------------------------------
 |  Compile with -DZIP to use CON00.ZIP.
 |
 +========================================================================*/

#include <conio.h>
#include <stdlib.h>
#include <iostream.h>
#include <iomanip.h>

#include "arclib.h"
#include "glarc.h"
#include "pkarc.h"
#include "spinner.h"

main()
{
 cout << "Archive Library 2.11\nEX02CON.CPP\n\n";
 cout << "Press ESC to abort, any other key to continue..." << flush;
 
 int c = getch();
 cout << "\n\n";
 if (c == 0 || c == 0x1b || c == 3)
  exit(1);

 ALSpinner monitor(AL_MONITOR_OBJECTS, cout);
 #if defined (ZIP)
  ALPkArchive archive("con00.zip");
  ALEntryList list(&monitor, PkTools());
 #else
  ALGlArchive archive("con00.gal");
  ALEntryList list(&monitor, GlTools());
 #endif

 cout << "Appending all ex??win.cpp files to "
      << archive.GetStorageObject()->mName.GetSafeName()
      << ".\n\n";

 // The following lines do not accomplish anything, but they do
 // demonstrate that ALArchiveBase::Append() does not append list entries
 // that are not marked.
 // Read archive directory.
 archive.ReadDirectory(list);
 // Clear marks of current list so we do not get an error about duplicates.
 list.ClearMarks();
 // Comment out the above line to see an error.

 list.AddWildCardFiles("ex??win.cpp");
 c = archive.Append(list);
 cout << c << " files were appended.\n";

 return 0;
}

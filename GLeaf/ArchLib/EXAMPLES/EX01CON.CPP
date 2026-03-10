/*= Archive Library v2.12 ================================================
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  EX01CON creates an archive and adds files to it, while using an
 |  ALSpinner monitor.  It adds files to either a CON01.GAL or
 |  CON01.ZIP.  Any existing CON01.GAL or CON01.ZIP gets replaced.
 |
 +- Functions ------------------------------------------------------------
 |  ALArchive::Create()
 |  ALArchive::AddWildCardFiles()
 |  ALEntryList::ALEntryList()
 |  ALSpinner::ALSpinner()
 |
 +- Notes ----------------------------------------------------------------
 |  Compile with -DZIP to create CON01.ZIP.
 |
 +========================================================================*/

#include <conio.h>
#include <stdlib.h>

#include "arclib.h"
#include "spinner.h"
#include "pkarc.h"
#include "glarc.h"
#include "pkengn.h"

main()
{
 cout << "Archive Library 2.11\nEX01CON.CPP\n";
 cout << "\nPress ESC to abort, any other key to continue..." << flush;

 ALSpinner monitor(AL_MONITOR_OBJECTS, cout);
 #if defined (ZIP)
  ALPkArchive archive("con01.zip");
  ALEntryList list(&monitor, PkCompressTools());
 #else
  ALGlArchive archive("con01.gal");
  ALEntryList list(&monitor, GlCompressTools());
 #endif

 int c = getch();
 cout << "\n\n";
 if (c == 0 || c == 0x1b || c == 3)
  exit(1);

 list.AddWildCardFiles("ex??con.cpp, ex??pm.rc");
 cout << archive.Create(list)
      << " files added to "
      << archive.GetStorageObject()->mName.GetSafeName()
      << " using an ALSpinner monitor.\n";
 cout << "Create() returned " << archive.mStatus << endl;

 return archive.mStatus;
}

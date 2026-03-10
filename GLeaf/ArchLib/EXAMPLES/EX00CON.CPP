/*= Archive Library v2.12 ================================================
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  EX00CON creates an archive and adds files to it, while using an
 |  ALBarGraph monitor.  It adds files to either a CON00.GAL or
 |  CON00.ZIP.  Any existing CON00.GAL or CON00.ZIP gets replaced.
 |
 +- Functions ------------------------------------------------------------
 |  ALArchive::Create()
 |  ALArchive::AddWildCardFiles()
 |  ALEntryList::ALEntryList()
 |  ALBarGraph::ALBarGraph()
 |
 +- Notes ----------------------------------------------------------------
 |  Compile with -DZIP to create CON00.ZIP.
 |
 +========================================================================*/

#include <conio.h>
#include <stdlib.h>

#include "arclib.h"
#include "bargraph.h"
#include "pkarc.h"
#include "glarc.h"
#include "pkengn.h"

main()
{
 cout << "Archive Library 2.11\nEX00CON.CPP\n\n";
 cout << "Press ESC to abort, any other key to continue..." << flush;

 ALBarGraph monitor(AL_MONITOR_OBJECTS, cout);
 #if defined (ZIP)
  ALPkArchive archive("con00.zip");
  ALEntryList list(&monitor, PkCompressTools());
 #else
  ALGlArchive archive("con00.gal");
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
 << " using an ALBarGraph monitor.\n";
 cout << "Create() returned " << archive.mStatus << endl;

 return archive.mStatus;
}

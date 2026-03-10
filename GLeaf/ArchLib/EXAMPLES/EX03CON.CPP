/*= Archive Library v2.12 ================================================
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  EX03CON uses the existing CON00.GAL or CON00.ZIP archive (create it
 |  with EX00CON if you don't have it) and deletes every other entry.
 |  Note that when we call Delete(), we specify an Archive with an empty
 |  name, which causes ALArchiveBase::Delete() to rename the input to a
 |  backup, and rename the output to the original input name.
 |
 +- Functions ------------------------------------------------------------
 |  ALArchiveBase::Delete()
 |  ALEntry::GetNextEntry()
 |  ALEntry::SetMark()
 |  ALEntryList::GetFirstEntry()
 |  ALEntryList::GetNextEntry()
 |
 +- Notes ----------------------------------------------------------------
 |  Compile with -DZIP to use CON00.ZIP.
 |
 +========================================================================*/

#include <conio.h>
#include <stdlib.h>

#include "arclib.h"
#include "glarc.h"
#include "pkarc.h"
#include "bargraph.h"
#include "filestor.h"

main()
{
 cout << "Archive Library 2.11\nEX03CON.CPP\n\n";
 cout << "Press ESC to abort, any other key to continue..." << flush;
 int c = getch();
 cout << "\n\n";
 if (c == 0 || c == 0x1b || c == 3)
  exit(1);

 ALBarGraph monitor(AL_MONITOR_OBJECTS, cout);
 long total = 0;

 #if defined (ZIP)
  ALEntryList list(&monitor, PkCompressFileTools());
  ALPkArchive archive("con00.zip");
  ALPkArchive temp("");
 #else
  ALEntryList list(&monitor, GlDecompressTools());
  ALGlArchive archive("con00.gal");
  ALGlArchive temp("");
 #endif
 cout << "Deleting every other object from "
      << archive.GetStorageObject()->mName.GetSafeName()
      << ".\n";

 archive.ReadDirectory(list);
 ALEntry *entry = list.GetFirstEntry();
 list.ClearMarks();
 while (entry) {
  total++;
  if (total % 2)
   entry->SetMark();
  entry = entry->GetNextEntry();
 }
 c = archive.Delete (list, temp);
 cout << c << " entries were deleted.\n";

 return c;
}

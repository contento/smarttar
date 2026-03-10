/*= Archive Library v2.12 ================================================
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  EX04CON demonstrates ArchiveLib's ability to set comments both for an
 |  entire archive as well as individual objects within the archive.  If
 |  you do not have a CON00.GAL or CON00.ZIP, you can create one using
 |  EX00CON.
 |
 +- Functions ------------------------------------------------------------
 |  ALArchiveBase::SetComment()
 |  ALArchiveBase::ReadDirectory()
 |  ALArchiveBase::WriteDirectory()
 |  ALEntry::GetFirstEntry()
 |  ALEntry::SetComment()
 |
 +- Notes ----------------------------------------------------------------
 |  Compile with -DZIP to use CON00.ZIP.
 |  pkzip -vc CON00.ZIP to view comments.
 |
 +========================================================================*/

#include <conio.h>
#include <stdlib.h>
#include <iostream.h>
#include <iomanip.h>

#include "arclib.h"
#include "glarc.h"
#include "pkarc.h"

main()
{
 cout << "Archive Library 2.11\nEX04CON.CPP\n\n";
 cout << "Press ESC to abort, any other key to continue..." << flush;

 int c = getch();
 cout << "\n\n";
 if (c == 0 || c == 0x1b || c == 3)
  exit(1);

 #if defined (ZIP)
  ALPkArchive archive("con00.zip");
 #else
  ALGlArchive archive("con00.gal");
 #endif
 ALEntryList list;

 archive.ReadDirectory(list);

 cout << "Setting archive comment." << endl;
 archive.SetComment("Greenleaf was here.");

 ALEntry *entry = list.GetFirstEntry();
 cout << "Setting comment for: "
      << entry->mpStorageObject->mName
      << endl;
 entry->SetComment("Greenleaf was here.");
 archive.WriteDirectory(list);

 return 0;
}

/*= Archive Library v2.12 ================================================
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  EX33CON demonstrates how to update a zip file.
 |
 +- Functions ------------------------------------------------------------
 |  ALPkArchive::Create()
 |  ALPkArchive::Delete()
 |  ALPkArchive::Append()
 |  ALEntryList::AddWildCardFiles()
 |  ALEntryList::ClearMarks()
 |  ALEntry::SetMark()
 |
 +========================================================================*/

#include <conio.h>
#include <stdlib.h>

#include "arclib.h"
#include "bargraph.h"
#include "pkarc.h"
#include "pkengn.h"

main()
{
 int x = 0;

 ALPkArchive *zip = new ALPkArchive("con33.zip");
 ALEntryList list(0, PkTools());

// Create CON33.ZIP with AUTOEXEC.BAT, AUTOEXEC.DOS, CONFIG.SYS
// and CONFIG.DOS.

 list.AddWildCardFiles("c:\\autoexec.*,c:\\config.*");
 zip->Create(list);

// Clear the marks in the list.
 list.ClearMarks();

// Toggle every other entry list to "ON".
 ALEntry *entry = list.GetFirstEntry();
 while (entry)
 {
  x++;
  if(x % 2)
   entry->SetMark();
  entry = entry->GetNextEntry();
 }

// Display the zip object's file name before Delete().
 cout << zip->GetStorageObject()->mName << endl;

// Delete the entries whose mark is set.
 zip->Delete(list, ALPkArchive(""));

// This creates two new files, CON33.ZIP and CON33.BAK.  CON33.ZIP will
// now contain just AUTOEXEC.DOS and CONFIG.DOS.  CON33.BAK contains
// the original four files.

// Display the zip object's file name after Delete().
 cout << zip->GetStorageObject()->mName << endl;

// Delete the zip object.
 delete zip;

// Let's re-establish the zip object to CON33.ZIP.
 zip = new ALPkArchive("con33.zip");

// We want to append back into CON33.ZIP AUTOEXEC.BAT and CONFIG.SYS.
 list.AddWildCardFiles("c:\\autoexec.bat,c:\\config.sys");
 zip->Append(list);

// Delete the zip object.
 delete zip;

// That's it!
 return 0;
}

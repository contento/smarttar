/*= Archive Library v2.12 ================================================
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  EX05CON displays a brief listing of an archive's contents.  If you
 |  do not specify an archive name on the command line, by default, it
 |  will display the contents of CON00.GAL or CON00.ZIP.  If you don't
 |  have a copy of CON00.GAL or CON00.ZIP, you can create one using EX00CON.
 |
 +- Functions ------------------------------------------------------------
 |  ALStorage::GetSize()
 |  ALArchiveBase::GetComment()
 |  ALEntry::GetComment()
 |  ALEntry::GetCompressedSize()
 |  ALEntry::GetMark()
 |
 +- Notes ----------------------------------------------------------------
 |  Compile with -DZIP to use CON00.ZIP.
 |  pkzip -vc CON00.zip to view comments.
 |
 +========================================================================*/

#include <iostream.h>
#include <stdlib.h>
#include <iomanip.h>
#include <time.h>
#include <string.h>
#include <conio.h>

#include "arclib.h"
#include "pkarc.h"
#include "glarc.h"

ostream &operator << (ostream &ostr, ALEntryList &list);

main(int argc, char *argv[])
{
 char arcname[80];

 cout << "Archive Library 2.11\nEX05CON.CPP\n\n";
 cout << "Press ESC to abort, any other key to continue..." << flush;
 int c = getch();
 cout << "\n\n";
 if (c == 0 || c == 0x1b || c == 3)
  exit(1);

 if (argc == 2)
  strcpy(arcname, argv[1]);
 else
  #if defined (ZIP)
   strcpy(arcname, "con00.zip");
   ALPkArchive archive(arcname);
  #else
   strcpy(arcname, "con00.gal");
   ALGlArchive archive(arcname);
  #endif

  ALEntryList list;

  archive.ReadDirectory(list);
  if (archive.GetComment())
   cout << "Archive comment: " << archive.GetComment() << endl;
   cout << list; //This uses the overloaded stream operator, shown below

  return 1;
}

ostream &operator << (ostream &ostr, ALEntryList &list)
{
 long total = 0;
 long tsize = 0;
 long tcomp = 0;
 short int month, day, year;
 struct tm tblock;

 ALEntry *entry = list.GetFirstEntry();

 if (entry == 0)
  return ostr << "No entries in list";

 ostr << "================================================================\n";
 ostr << "M  Name               Size        Comp   Comment\n";
 ostr << "----------------------------------------------------------------\n";

 while (entry)
 {
  total++;
  tsize += entry->mpStorageObject->GetSize();
  tcomp += entry->GetCompressedSize();
  ostr << (entry->GetMark() ? "*" : " ");
  ostr << setw(13) << entry->mpStorageObject->mName;
  ostr << setw(12) << entry->mpStorageObject->GetSize();
  ostr << setw(12) << entry->GetCompressedSize();
  if (entry->GetComment())
   ostr << "   " << entry->GetComment();
  ostr << endl;
  entry = entry->GetNextEntry();
 }
 ostr << "----------------------------------------------------------------\n";

 ostr << " Total";
 ostr << setw(4) << total;
 ostr << setw(16) << tsize;
 ostr << setw(12) << tcomp << endl;

 return ostr;
}

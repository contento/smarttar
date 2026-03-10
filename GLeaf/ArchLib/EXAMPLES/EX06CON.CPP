/*= Archive Library v2.12 ================================================
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  EX06CON creates a list of files using the ability of ALWildCardExpander
 |  to traverse subdirectories when expanding wild card file specifications.
 |  Of course, you might not have any subdirectories when you run this.
 |  If not, try creating some and putting *.CPP and *.OBJ files in some of
 |  them.  Or even better, change the file spec to "..\\*.CPP" to get
 |  lots-o-files.
 |
 +- Functions ------------------------------------------------------------
 |  ALEntryList::AddWildCardFiles()
 |  ALArchive::Create()
 |
 +- Notes ----------------------------------------------------------------
 |  Compile with -DZIP to create CON06.ZIP.
 |
 +========================================================================*/

#include <conio.h>
#include <stdlib.h>

#include "arclib.h"
#include "pkarc.h"
#include "glarc.h"
#include "bargraph.h"

main(int, char *[])
{
 cout << "Archive Library 2.11\nEX06CON.CPP\n\n";
 cout << "Press ESC to abort, any other key to continue..." << flush;
 int c = getch();
 cout << "\n\n";
 if (c == 0 || c == 0x1b || c == 3)
  exit(1);

 ALBarGraph monitor(AL_MONITOR_OBJECTS, cout);

 cout << "Recursively adding *.cpp and *.obj to ";

 #if defined (ZIP)
  cout << "con06zip\n";
  ALPkArchive archive("con06.zip");
  ALEntryList list(&monitor, PkTools());
 #else
  cout << "con06.gal\n";
  ALGlArchive archive("con06.gal");
  ALEntryList list(&monitor);
 #endif
 list.AddWildCardFiles("*.cpp, *.obj", 1);

 return archive.Create(list);
}

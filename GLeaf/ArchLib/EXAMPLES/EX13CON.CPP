/*= Archive Library v2.12 ================================================
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  EX13CON demonstrates how you can create a derived class and use
 |  ALStorage::YieldTime() to monitor for abort conditions.  In this case
 |  all it takes is a new class with two functions:  a ctor and a
 |  new version of YieldTime().  You can't do this without writing C++
 |  code, sorry VB and C programmers.  But the C++ is pretty easy, it
 |  wouldn't be hard to drop a module like this into your C program.
 |
 +- Functions ------------------------------------------------------------
 |  ALMemory::ALMemory()
 |  ALEntryList::AddWildCardFiles()
 |
 +- Notes ----------------------------------------------------------------
 |  Compile with -DZIP to use ZIP compression.
 |
 +========================================================================*/

#include <iostream.h>
#include <stdlib.h>
#include <iomanip.h>
#include <conio.h>

#include "arclib.h"
#include "pkarc.h"
#include "glarc.h"
#include "bargraph.h"
#include "memstore.h"

// This derived class has to have a constructor, but it gets the base
// class ctor to do all the work for.  The YieldTime() function is
// where the real work takes place.  Everything else is done by
// ALMemory, so we get off light here.

#if defined (HUGE)
 class AL_CLASS_TYPE MyFile : public ALHugeMemory {
  public :
   AL_PROTO MyFile(const char *name) : ALHugeMemory(name) {; }
#else
 class AL_CLASS_TYPE MyFile : public ALMemory {
  public :
   AL_INLINE_PROTO MyFile(const char *name) : ALMemory(name) {; }
#endif
   virtual void AL_PROTO YieldTime();
   virtual AL_INLINE_PROTO ~MyFile() {; }
 };

 // ALStorage::YieldTime() actually does some worthwhile work, so we
 // have to be sure to call it.  All we do here in addition to that is
 // check for a keyboard hit.  If we see one, we set the error status on
 // this so that the archiving function will abort.

 void AL_PROTO MyFile::YieldTime()
 {
  if (kbhit()) {
   getch();
   mStatus.SetError(AL_USER_ABORT, "User hit abort key...");
  }
  #if defined (HUGE)
   ALHugeMemory::YieldTime();
  #else
   ALMemory::YieldTime();
  #endif
 }

 // The main() function here is a lot like EX00CON.CPP.  The main difference
 // is that we are archiving to a memory object instead of a disk object.
 // The only reason for that is so we make less clutter.

 main()
 {
  cout << "Archive Library 2.11\nEX13CON.CPP\n\n";
  cout << "Press ESC to abort, any other key to continue..." << flush;
  int c = getch();
  cout << "\n\n";
  if (c == 0 || c == 0x1b || c == 3)
   exit(1);

  cout << "Adding *.cpp to a MyFile object using an ALBarGraph monitor.\n"
       << "Press any key to abort...\n\n";

  MyFile archive_file("Memory based archive");
  #if defined (ZIP)
   ALPkArchive archive(archive_file);
  #else
   ALGlArchive archive(archive_file);
  #endif
  ALBarGraph monitor(AL_MONITOR_OBJECTS, cout);

  ALEntryList list(&monitor);

  list.AddWildCardFiles("*.cpp");
  archive.Create(list);
  cout << "\n" << archive.mStatus << "\n";

  return archive.mStatus;
 }

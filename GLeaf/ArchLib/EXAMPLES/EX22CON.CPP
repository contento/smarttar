/*= Archive Library v2.12 ================================================
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  EX22CON shows how you can set up a monitor and use it to allow for an
 |  abort path during archiving.  We derive a new monitor class from
 |  ALBargraph.  We are going to use ALBargraph's existing functions, and
 |  just add a keyboard check.  So to the end user it will look exactly
 |  like the existing bargraph.
 |
 +- Functions ------------------------------------------------------------
 |  ALEntryList::AddWildCardFiles()
 |  ALArchive::Create()
 |
 +- Notes ----------------------------------------------------------------
 |  Compile with -DZIP to create CON22.ZIP.
 |
 +========================================================================*/

#include <iostream.h>
#include <stdlib.h>
#include <conio.h>

#include "arclib.h"
#include "bargraph.h"
#include "pkarc.h"
#include "glarc.h"

// Our new class has to have a ctor, plus we add a Progress() function.
// The Progress() function gets called a lot, so that is where we get
// to check the keyboard for a keypress.

class MyMonitor : public ALBarGraph {
 public :
  MyMonitor() : ALBarGraph(AL_MONITOR_OBJECTS) {; }
  void AL_PROTO Progress(long object_so_far, ALStorage AL_DLL_FAR &object);
  virtual AL_INLINE_PROTO ~MyMonitor() {; }
};

// The Progress() function is clearly pretty lazy.  The first thing
// it does is call ALBarGraph::Progress(), so we don't have to do any
// U/I stuff.  All we do then is check to see if the user hit a keystroke.
// If so, we set the error flag on the object, which should lead to an
// abort of the archiving function.

void MyMonitor::Progress(long object_tell, ALStorage& object)
{
 ALBarGraph::Progress(object_tell, object);
 if (kbhit()) {
  getch();
  object.mStatus.SetError(AL_USER_ABORT, "User hit abort key...");
 }
}

// This should look just like EX00CON.CPP.  It is nearly identical, with
// the only important difference being the fact that it creates monitor
// from my derived class.

int main()
{
 MyMonitor monitor;

 cout << "Archive Library 2.11\nEX22CON.CPP\n\n";
 cout << "Press ESC to abort, any other key to continue..." << flush;
 int c = getch();
 cout << "\n\n";
 if (c == 0 || c == 0x1b || c == 3)
  exit(1);

 #if defined (ZIP)
  ALPkArchive archive("con22.zip");
  ALEntryList list(&monitor, PkCompressTools());
 #else
  ALGlArchive archive("con22.gal");
  ALEntryList list(&monitor, GlCompressTools());
 #endif

 cout << "Adding *.cpp to "
      << archive.GetStorageObject()->mName
      << " using an ALBarGraph monitor.\n\n";

 list.AddWildCardFiles("*.cpp");
 archive.Create(list);
 cout << "\n" << archive.mStatus << endl;

 return archive.mStatus;
}

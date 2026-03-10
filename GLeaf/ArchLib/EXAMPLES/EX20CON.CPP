/*= Archive Library v2.12 ================================================
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  EX20CON shows you a little demo of how to derive a new class of
 |  ALMonitor.  This monitor just prints out a byte count while the object
 |  progresses.  It doesn't support AL_MONITOR_JOB, just the mode that
 |  monitors individual objects.  But it wouldn't be too much more
 |  complicated if it did.
 |
 +- Functions ------------------------------------------------------------
 |  ALMonitor::ArchiveOperation()
 |  ALMonitor::Progress()
 |
 +- Notes ----------------------------------------------------------------
 |  Compile with -DZIP to use ZIP compression.
 |
 +========================================================================*/

#include <iostream.h>
#include <stdlib.h>
#include <conio.h>

#include "arclib.h"
#include "pkarc.h"
#include "glarc.h"

// All you need to create a simple new class of ALMonitor is a new
// ctor, and two member functions.  The behavior of Progress() and
// ArchiveOperation() are fairly well documented, and you can see
// complete working examples in our three monitor classes provided with
// ArchiveLib.

class AL_CLASS_TYPE DemoMonitor : public ALMonitor {
 public :
   AL_INLINE_PROTO DemoMonitor() : ALMonitor(AL_MONITOR_OBJECTS) {; }
   virtual AL_INLINE_PROTO ~DemoMonitor() {; }
 protected :
   virtual void AL_PROTO Progress( long mlObjectSoFar, ALStorage& object );
   virtual void AL_PROTO ArchiveOperation( ALArchiveOperation operation,
                                           ALArchive *,
                                           ALEntry * );
};

// Unlike EX00CON.CPP this example insists that you provide an
// archive name and a list of input files on the command line.
// It then performs a straightforward archive creation.

int main(int argc, char *argv[])
{
 cout << "Archive Library 2.11\nEX20CON.CPP\n\n";
 cout << "Press ESC to abort, any other key to continue..." << flush;
 int c = getch();
 cout << "\n\n";
 if (c == 0 || c == 0x1b || c == 3)
  exit(1);

 if (argc < 3) {
  cerr << "Usage: EX20CON archive files [files...]\n";
  return 1;
 }

 ALName archive_name = argv[1];
 DemoMonitor monitor;
 #if defined (ZIP)
  if (strchr(archive_name, '.') == 0)
   archive_name = archive_name + ".zip";
  ALPkArchive archive((char *) archive_name);
  ALEntryList list(&monitor, PkCompressTools());
 #else
  if (strchr(archive_name, '.') == 0)
   archive_name = archive_name + ".gal";
  ALGlArchive archive((char *) archive_name);
  ALEntryList list(&monitor);
 #endif
 for (int i = 2; argv[i] && strlen(argv[i]); i++)
  list.AddWildCardFiles(argv[i]);

 // We let the user input multiple wild-card specs, so it would be really
 // easy to get some duplicates in the list.  These function calls
 // make sure that can't happen.

 list.UnmarkDuplicates(list);
 list.DeleteUnmarked();
 cout << "\nList of files to be compressed: ";
 for (ALEntry *entry = list.GetFirstEntry();
  entry;
  entry = entry->GetNextEntry())
  cout << entry->mpStorageObject->mName << " ";
  cout << "\nCompressing...\n";
 archive.Create(list);
 return 0;
}

// The progress routine prints out the current object name and size, then
// does a CR with no LF.  This means it will keep overwriting the name
// of the object and the size as long as the archive progresses.

void AL_PROTO DemoMonitor::Progress(long object_tell, ALStorage& object)
{
 ALMonitor::Progress(object_tell, object);
 cout << object.mName
      << "  : "
      << mlByteCount
      << "/"
      << object.GetSize()
      << "\r"
      << flush;
}

// When we are done with an object, we print an LF, so that the last
// thing printed on the line gets left there.  For this particular
// class, that ought to be the object name followed by its file size.

void AL_PROTO DemoMonitor::ArchiveOperation(ALArchiveOperation operation,
                                            ALArchive *,
                                            ALEntry *)
{
 switch (operation) {
 case AL_EXTRACTION_CLOSE :
 case AL_COPY_CLOSE :
 case AL_INSERTION_CLOSE :
  cout << "\n";
  break;
 default :
  break;
 }
 cout << flush;
}

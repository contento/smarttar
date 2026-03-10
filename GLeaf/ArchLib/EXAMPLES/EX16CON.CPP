/*= Archive Library v2.12 ================================================
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  EX16CON first creates a whole bunch of ALMemory objects, initializing
 |  them with a bunch of fixed data so that they compress fairly well.
 |  It then does a bunch of manipulation of the memory objects in their
 |  list, deleting some, changing the compression engine of others, and
 |  clearing the marks of even more.  Finally, it creates the archive,
 |  then displays the list for your viewing pleasure.
 |
 +- Functions ------------------------------------------------------------
 |  ALEntryList::ALGetFirstEntry()
 |  ALEntry::ALGetNextEntry()
 |  ALEntry::SetComment()
 |  ALEntry::ClearMark()
 |  ALEntry::SetMarkState()
 |  ALStorage::Create()
 |  ALStorage::WriteBuffer()
 |  ALStorage::Close()
 |
 +- Notes ----------------------------------------------------------------
 |  Compile with -DZIP to create CON16.ZIP.
 |
 +========================================================================*/

#include <iostream.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iomanip.h>
#include <conio.h>

#include "arclib.h"
#include "pkarc.h"
#include "glarc.h"
#include "glengn.h"
#include "pkengn.h"
#include "bargraph.h"
#include "memstore.h"
#include "copyengn.h"

main()
{
 cout << "Archive Library 2.11\nEX16CON.CPP\n\n";
 cout << "Press ESC to abort, any other key to continue..." << flush;
 int c = getch();
 cout << "\n\n";
 if (c == 0 || c == 0x1b || c == 3)
  exit(1);

 char temp[128];
 ALBarGraph monitor(AL_MONITOR_JOB);

 #if defined (ZIP)
  ALPkArchive archive("con16.zip");
 #else
  ALGlArchive archive("con16.gal");
 #endif
 ALEntryList list(&monitor);

 // The first step is to create 24 ALMemory objects.  Each one has the
 // contents of the temp buffer written out to it, with the temp buffer
 // being just a little different every time.  As each object is created,
 // I add it to the list, along with a new copy of the Greenleaf engine
 // or PKZip engine.

 memset(temp, 'A', 128);
 for (int i = 0; i < 24; i++) {
  char name[20];
  sprintf(name, "Buffer.%02d", i);
  ALStorage *obj = new ALMemory(name);
  obj->Create();
  memset(temp + i, i, i);
  obj->WriteBuffer((unsigned char *) temp, 128);
  obj->Close();
  #if defined (ZIP)
   new ALEntry(list, obj, new ALPkCompressor, new ALPkDecompressor);
  #else
   new ALEntry(list, obj, new ALGlCompressor, new ALGlDecompressor);
  #endif
 }

 // Now for fun, I am going to delete every third entry in list

 ALEntry *entry = list.GetFirstEntry();
 for (; entry != 0;) {
  ALEntry *next_entry = entry->GetNextEntry();
  delete entry;
  entry = next_entry->GetNextEntry();
  if (entry)
   entry = entry->GetNextEntry();
 }

 // Now, for more fun, I am going to change the compression engine in every
 // third entry.

 entry = list.GetFirstEntry();
 for (; entry != 0;) {
  #if defined (ZIP)
   if (entry->mpCompressor->miCompressionType !=
    AL_COMPRESSION_DEFLATE) {
   #else
    if (entry->mpCompressor->miCompressionType !=
     AL_COMPRESSION_GREENLEAF) {
    #endif
    cout << "Unknown engine type!\n";
    exit(1);
    }
    delete entry->mpCompressor;
    entry->mpCompressor = new ALCopyCompressor;
    entry->SetComment("No compression here");
    entry = entry->GetNextEntry();
    if (entry)
     entry = entry->GetNextEntry();
    if (entry)
     entry = entry->GetNextEntry();
   }

   // Now, for the last bit of fun, I unmark every other entry

   entry = list.GetFirstEntry();
   for (; entry != 0;) {
    entry->ClearMark();
    entry = entry->GetNextEntry();
    if (entry) {
     entry->SetMarkState(1);
     entry = entry->GetNextEntry();
    }
   }

   // Having done all that, it is time to create the archive, then print
   // out the list, along with the CRC and compression ratio for all
   // the entries.

   int status = archive.Create(list);
   cout << "Archive status: " << archive.mStatus << endl;
   cout << "\nList status:\n\n";
   if (status >= 0) {
    for (entry = list.GetFirstEntry();
     entry;
     entry = entry->GetNextEntry()) {
     cout << entry->mpStorageObject->mName
          << " :  ";
     if (entry->GetMark()) {
      cout << entry->CompressionRatio()
           << "%  ";
      (void) cout.width(8);
      cout << hex
           << entry->GetCrc32()
           << dec
           << "\n";
     } else {
      cout << "Not marked\n";
     }
    }
   }
   cout << "\n";
   cout << "List code:   " << (int) list.mStatus << "\n";
   cout << "List string: " << list.mStatus.GetStatusString() << "\n";
   cout << "List detail: " << list.mStatus.GetStatusDetail() << "\n";

   return status;
 }

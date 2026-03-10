/*= Archive Library v2.12 ================================================
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  EX09CON demonstrates a bunch of miscellaneous things.  It creates an
 |  in-memory archive, adds a bunch of source code to the archive, then
 |  adds a memory buffer.  It then extracts EX09CON.CPP to an in-memory
 |  object, and finally dumps it on the screen for you to see.
 |
 +- Functions ------------------------------------------------------------
 |  ALArchive::ALArchive()
 |  ALEntry::ALEntry()
 |  ALEntryList::DeleteUnmarked()
 |  ALEntryList::SetMarks()
 |  ALEntryList::ToggleMarks()
 |  ALStorage::Close()
 |  ALStorage::Open()
 |  ALStorage::ReadChar()
 |
 +- Notes ----------------------------------------------------------------
 |  Compile with -DZIP to use ZIP compression.
 |
 +========================================================================*/

#include <stdlib.h>
#include <string.h>
#include <conio.h>

#include "arclib.h"
#include "memstore.h"
#include "glarc.h"
#include "glengn.h"
#include "pkarc.h"
#include "pkengn.h"

void create_archive(ALArchive &archive, ALEntryList &list);
void create_output_list(ALArchive &archive, ALEntryList &list);

int main(void)
{
 cout << "Archive Library 2.11\nEX09CON.CPP\n\n";
 cout << "Press ESC to abort, any other key to continue..." << flush;
 int c = getch();
 cout << "\n\n";
 if (c == 0 || c == 0x1b || c == 3)
  exit(1);

 ALMemory archive_file("Try using a name like this for a disk file!");
 #if defined (ZIP)
  ALPkArchive archive(archive_file);
 #else
  ALGlArchive archive(archive_file);
 #endif

 ALEntryList *input_list = new ALEntryList(0, PkTools(6, 13, 6));

 AL_ASSERT(input_list != 0, "ALEntryList constructor didn't work!");
 create_archive(archive, *input_list); //Create the memory archive
 delete input_list;

 ALEntryList output_list;
 //
 // Next we work on the output list in order to figure out which files
 // to extract.  I then perform the extraction and print the results.
 //
 create_output_list(archive, output_list);

 archive.Extract(output_list);
 ALEntry *job = output_list.GetFirstEntry();
 if (!job) {
  cout << "Empty archive!\n";
  return 1;
 }
 job->mpStorageObject->Open();
 while ((c = job->mpStorageObject->ReadChar()) >= 0)
  cout << (char) c;
 job->mpStorageObject->Close();
 cout << "Done\n";
 return 0;
}

//
// This function is called to create the in-memory archive.  It
// adds a bunch of example programs to the list, then a single
// memory object.  After creating the archive with these
// objects, it prints out some status stuff and returns.
//
void create_archive(ALArchive &archive, ALEntryList &list)
{
 char *buffer = "Adding this buffer to the archive!";

 list.AddWildCardFiles("EX0?CON.CPP");
 #if defined (ZIP)
  new ALEntry(list,
  new ALMemory("buffer", buffer, strlen(buffer)),
  new ALPkCompressor(6, 13, 6),
  new ALPkDecompressor);
 #else
  new ALEntry(list,
  new ALMemory("buffer", buffer, strlen(buffer)),
  new ALGlCompressor,
  new ALGlDecompressor);
 #endif
 archive.Create(list);
 cout << "Returned status integer = "
      << (int) archive.mStatus << "\n";
 cout << "Returned status string = "
      << archive.mStatus.GetStatusString() << "\n";
 cout << "Returned status detail = "
      << archive.mStatus << "\n";
}

//
// This function is called just before extracting the list from
// the archive.  Essentially all it does is insure that the only
// object to be extracted from the archive will be EX09CON.CPP, and
// it will be extracted to a memory object instead of a file.
//
void create_output_list(ALArchive &archive, ALEntryList &list)
{
 archive.ReadDirectory(list);
 list.ToggleMarks(); //Turns off all marks
 if (list.SetMarks("ex09con.cpp") < 0)
  exit(1);
 list.DeleteUnmarked();
 ALEntry *job = list.GetFirstEntry();
 if (job) {
  delete job->mpStorageObject;
  job->mpStorageObject = new ALMemory;
 }
}

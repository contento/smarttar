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
 |  ALEntryClearMark()
 |  deleteALEntry()
 |  ALEntryCompressionRatio()
 |  ALEntryGetCrc32()
 |  ALEntrySetMarkState()
 |  ALEntryGetEngine()
 |  ALEntryGetStorage()
 |  ALEntrySetEngine()
 |  ALEntryListGetStatusCode()
 |  ALEntryListGetStatusDetail()
 |  ALEntryListGetStatusString()
 |
 +========================================================================*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

#include "arclib.h"
#include "memstore.h"
#include "pkengn.h"
#include "glengn.h"
#include "glarc.h"
#include "copyengn.h"
#include "bargraph.h"

int main()
{
 char temp[128];
 hALMonitor monitor;
 hALEntryList list;
 hALArchive archive;
 hALEntry entry;
 int i;
 int status;
 int c;

 printf("Archive Library 2.11\nEX16CON.C\n\n");
 printf("Press ESC to abort, any other key to continue...");
 fflush(stdout);
 c = getch();
 printf("\n\n");
 if (c == 0 || c == 0x1b || c == 3)
  exit(1);

 monitor = newALBarGraph(AL_MONITOR_JOB);
 list = newALListCopyTools(monitor);
 archive = newALGlArchive("con16.gal");

/*
  The first step is to create 24 ALMemory objects.  Each one has the
  contents of the temp buffer written out to it, with the temp buffer
  being just a little different every time.  As each object is created,
  I add it to the list, along with a new copy of the greenleaf engine.
*/

 memset(temp, 'A', 128);
 for (i = 0; i < 24; i++) {
  char name[20];
  hALStorage obj;

  sprintf(name, "Buffer %02d", i);
  obj = newALMemory(name, 0, 0);
  ALStorageCreate(obj, AL_DEFAULT);
  memset(temp + i, i, i);
  ALStorageWriteBuffer(obj, (unsigned char *) temp, 128);
  ALStorageClose(obj);
  if (i &1)
   newALEntry(list, obj, newALPkCompressor(AL_DEFAULT,
                                           AL_DEFAULT,
                                           AL_DEFAULT),
                                           0);
  else
   newALEntry(list, obj, newALGlCompressor(AL_GREENLEAF_LEVEL_2, 0), 0);
 }

/*
  Now for fun, I am going to delete every third entry in list
*/

 entry = ALEntryListGetFirstEntry(list);
 for (; entry != 0;) {
  hALEntry next_entry = ALEntryGetNextEntry(entry);
  deleteALEntry(entry);
  entry = ALEntryGetNextEntry(next_entry);
  if (entry)
   entry = ALEntryGetNextEntry(entry);
 }

/*
  Now, for more fun, I am going to change the compression engine in every
  third entry.
*/

 entry = ALEntryListGetFirstEntry(list);
 for (; entry != 0;) {
  hALCompressor compressor;
  hALDecompressor decompressor;
  compressor = ALEntryGetCompressor(entry);
  if (ALCompressorGetTypeCode(compressor) != AL_COMPRESSION_GREENLEAF &&
   ALCompressorGetTypeCode(compressor) != AL_COMPRESSION_DEFLATE &&
   ALCompressorGetTypeCode(compressor) != AL_COMPRESSION_COPY) {
   printf("Unknown compressor type!\n");
   exit(1);
  }
  deleteALCompressor(compressor);
  ALEntrySetCompressor(entry, newALCopyCompressor());
  ALEntrySetComment(entry, "No compression here");
  decompressor = ALEntryGetDecompressor(entry);
  printf("Decompressor = %s\n",
   decompressor ? ALDecompressorGetTypeString(decompressor) : "None");
  entry = ALEntryGetNextEntry(entry);
  if (entry)
   entry = ALEntryGetNextEntry(entry);
  if (entry)
   entry = ALEntryGetNextEntry(entry);
 }

/*
   Now, for the last bit of fun, I unmark every other entry
*/

 entry = ALEntryListGetFirstEntry(list);
 for (; entry != 0;) {
  ALEntryClearMark(entry);
  entry = ALEntryGetNextEntry(entry);
  if (entry) {
   ALEntrySetMarkState(entry, 1);
   entry = ALEntryGetNextEntry(entry);
  }
 }

/*
  Having done all that, it is time to create the archive, then print
  out the list, along with the CRC and compression ratio for all
  the entries.
*/

 status = ALArchiveCreate(archive, list);
 printf("Archive status: %s\n", ALArchiveGetStatusString(archive));
 printf("\nList status:\n\n");
 if (status >= 0) {
  for (entry = ALEntryListGetFirstEntry(list);
   entry;
   entry = ALEntryGetNextEntry(entry)) {
   printf("%s", ALStorageGetName(ALEntryGetStorage(entry)));
   printf(" :  ");
   if (ALEntryGetMark(entry)) {
    printf("%d%%", ALEntryCompressionRatio(entry));
    printf("  %08lx\n", ALEntryGetCrc32(entry));
   } else
   printf("Not marked\n");
  }
 }
 printf("\n");
 printf("List code   : %d\n", ALEntryListGetStatusCode(list));
 printf("List string : %s\n", ALEntryListGetStatusString(list));
 printf("List detail : %s\n", ALEntryListGetStatusDetail(list));
 deleteALEntryList(list);
 deleteALMonitor(monitor);
 deleteALArchive(archive);

 return status;
}

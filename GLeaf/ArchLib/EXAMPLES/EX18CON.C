/*= Archive Library v2.12 ================================================
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  EX18CON ties together a bunch of marginally related functions.  It
 |  starts off by creating a data file on disk, and writing some fixed
 |  patterns out to it.  Next, the data file gets inserted into a compressed
 |  object, also on disk.  Then the weird part:  If a random number hits, I
 |  randomly change a byte in the compressed object.  Finally, I extract the
 |  original data file from the compressed object, for better or worse.
 |
 |  After this is all done, I have three TMP files on the disk.  In an
 |  imitation of what you might do in a real debugging situation, I check
 |  to see if the input and output match.  If they do match, I just go
 |  ahead and delete them, all is well.  If they don't match, I want to
 |  hang on to the files.  Not only do I hang on to the files, I give
 |  them a funny time date stamp, and set them to be read-only.
 |
 +- Functions ------------------------------------------------------------
 |  newALCompressedObject()
 |  deleteALCompressed()
 |  ALCompressedExtract()
 |  ALCompressedInsert()
 |  ALCompressedGetStatusCode()
 |  ALCompressedGetStatusDetail()
 |  ALCompressedGetStatusString()
 |  ALCompressedSetError()
 |  ALStoragePackedAttributes()
 |  ALStorageSetFromDosAttributes()
 |  ALStorageSetFromPackedAtts()
 |  ALStorageSetTimeDateFromUnix()
 |
 +- Notes ----------------------------------------------------------------
 |  Compile with -DZIP to use ZIP compression.
 |
 +========================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>

#include "al.h"

int main()
{
 hALStorage DataFile;
 hALStorage CompressedFile;
 hALStorage OutputFile;
 hALCompressor Compressor;
 hALDecompressor Decompressor;
 hALCompressed CompressedObject;
 int status;
 short unsigned int atts;
 int i;
 int j;
 int c;

 printf("Archive Library 2.11\nEX18CON.C\n\n");
 printf("Press ESC to abort, any other key to continue...");
 fflush(stdout);
 c = getch();
 printf("\n\n");
 if (c == 0 || c == 0x1b || c == 3)
  exit(1);

#if defined (ZIP)
 Compressor = newALPkCompressor(AL_DEFAULT, AL_DEFAULT, AL_DEFAULT);
 Decompressor = newALPkDecompressor();
#else
 Compressor = newALGlCompressor(AL_DEFAULT, 0);
 Decompressor = newALGlDecompressor(AL_DEFAULT);
#endif

/*
  Here is where I create the input data file and the compressed object.
  This is all fairly realistic, you could write code that looks a lot
  like this.
*/

 DataFile = newALFile("");
 CompressedFile = newALFile("");
 OutputFile = newALFile("");
 CompressedObject = newALCompressed(CompressedFile, Compressor, Decompressor);
 ALStorageCreate(DataFile, - 1); /* Temporary file */
 for (i = 0; i < 64; i++)
  for (j = i; j < 64; j++)
   ALStorageWriteChar(DataFile, j);
 ALStorageClose(DataFile);
 ALCompressedInsert(CompressedObject, DataFile);

/*
  If the one in four chance hits, I change a single byte in the compressed
  file.  This will almost always cause an expansion error, although it
  is possible to create a benign change (like the 1/256 chance that you
  write over the old byte with a new byte that is the same!)
*/

 srand((unsigned) time(NULL));
 if ((rand() &3) == 0) {
  printf("Mangling the compressed file!\n");
  ALStorageOpen(CompressedFile);
  ALStorageSeek(CompressedFile, 32 + (rand() &31));
  ALStorageWriteChar(CompressedFile, (rand() &0xff));
  ALStorageClose(CompressedFile);
 }

/*
  Now I try to extract the output file.  Once I have done that, I perform
  a compare to the original to see how things went.  Note that there is
  a tricky bit here.  The only time ALFile objects will get new time/date
  and attribute stamps is if they are closed immediately after having been
  created.  The CompressedObject->Extract() call will create the file,
  then close it.  Once it is closed, the flag indicating that it was just
  created will be cleared, so I won't be able to set the time stamp.  But,
  if *I* create it here, the Extract() function will leave it open after
  performing its magic.  After it returns, I have to close the file,
  but I can mung the time/date stamp first.
*/

 ALStorageCreate(OutputFile, AL_DEFAULT);
 ALCompressedExtract(CompressedObject, OutputFile);
 ALStorageSeek(OutputFile, 0L);
 status = ALStorageCompare(DataFile, OutputFile);
 printf("Compare returned : %d\n", status);

/*
  If the comparison passed, I just delete all of the TMP files created
  here.  (Note that they all have TMP names because I didn't give them
  any names in their constructors.)
*/

 if (status >= 0) {
  ALStorageDelete(DataFile);
  ALStorageDelete(CompressedFile);
  ALStorageClose(OutputFile);
  ALStorageDelete(OutputFile);
 } else {
  ALStorageSetTimeDateFromUnix(
   OutputFile,
   20L*365L*24L*60L*60L + 5L*24L*60L*60L + 12L*60L*60L);
  atts = ALStoragePackedAttributes(OutputFile);
  atts = (short unsigned int) (atts | ATTR_READ_ONLY);
  ALStorageSetFromPackedAtts(OutputFile, atts);
  ALStorageClose(OutputFile);
  ALCompressedSetError(CompressedObject,
   status,
   ALStorageGetStatusDetail(DataFile));
 }
 printf("Compressed object status code:    %d\n",
  ALCompressedGetStatusCode(CompressedObject));
 printf("Compressed object status string:  %s\n",
  ALCompressedGetStatusString(CompressedObject));
 printf("Compressed object status detail:  %s\n",
  ALCompressedGetStatusDetail(CompressedObject));

 deleteALCompressed(CompressedObject);
 deleteALCompressor(Compressor);
 deleteALDecompressor(Decompressor);
 deleteALStorage(CompressedFile);
 deleteALStorage(DataFile);
 deleteALStorage(OutputFile);

 return status;
}

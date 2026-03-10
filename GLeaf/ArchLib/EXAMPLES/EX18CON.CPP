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
 |  ALCompressedObject::ALCompressedObject()
 |  ALCompressedObject::~ALCompressedObject()
 |  ALCompressedObject::Extract()
 |  ALCompressedObject::Insert()
 |  ALStatus::GetStatusCode()
 |  ALStatus::GetStatusString()
 |  ALStatus::GetStatusDetail()
 |  ALStatus::SetError();
 |  ALFileAttributes::PackedAttributes()
 |  ALFileAttributes::SetFromDosAttributes()
 |  ALFileAttributes::SetFromPackedAttributes()
 |  ALStorage::SetTimeDate()
 |
 +- Notes ----------------------------------------------------------------
 |  Compile with -DZIP to use ZIP compression.
 |
 +========================================================================*/

#include <time.h>
#include <stdlib.h>
#include <conio.h>

#include "al.h"

main()
{
 ALFile DataFile;
 ALFile CompressedFile;
 ALFile OutputFile;

 cout << "Archive Library 2.11\nEX18CON.CPP\n\n";
 cout << "Press ESC to abort, any other key to continue..." << flush;
 int c = getch();
 cout << "\n\n";
 if (c == 0 || c == 0x1b || c == 3)
  exit(1);

 #if defined (ZIP)
  ALPkCompressor Compressor(3);
  ALPkDecompressor Decompressor;
 #else
  ALGlCompressor Compressor(AL_GREENLEAF_LEVEL_4);
  ALGlDecompressor Decompressor;
 #endif
 ALCompressedObject *CompressedObject;

 // Here is where I create the input data file and the compressed object.
 // This is all fairly realistic, you could write code that looks a lot
 // like this.

 CompressedObject = new ALCompressedObject(CompressedFile, &Compressor, &
   Decompressor);
 DataFile.Create(); // Temporary file
 for (int i = 0; i < 64; i++)
  for (int j = i; j < 64; j++)
   DataFile.WriteChar(j);
 DataFile.Close();
 CompressedObject->Insert(DataFile);

 // If the one in four chance hits, I change a single byte in the compressed
 // file.  This will almost always cause an expansion error, although it
 // is possible to create a benign change (like the 1/256 chance that you
 // write over the old byte with a new byte that is the same!)

 srand((unsigned) time(NULL));
 if ((rand() &3) == 0) {
  cout << "Mangling the compressed file!\n";
  CompressedFile.Open();
  CompressedFile.Seek(32 + (rand() &31));
  CompressedFile.WriteChar(rand() &0xff);
  CompressedFile.Close();
 }

 // Now I try to extract the output file.  Once I have done that, I perform
 // a compare to the original to see how things went.  Note that there is
 // a tricky bit here.  The only time ALFile objects will get new time/date
 // and attribute stamps is if they are closed immediately after having been
 // created.  The CompressedObject->Extract() call will create the file,
 // then close it.  Once it is closed, the flag indicating that it was just
 // created will be cleared, so I won't be able to set the time stamp.  But,
 // if *I* create it here, the Extract() function will leave it open after
 // performing its magic.  After it returns, I have to close the file,
 // but I can mung the time/date stamp first.

 OutputFile.Create(); //I need to do this in order to set the time and atts
 CompressedObject->Extract(OutputFile);
 OutputFile.Seek(0L);
 int status = DataFile.Compare(OutputFile);
 cout << "Compare returned : " << status << "\n";

 // If the comparison passed, I just delete all of the TMP files created
 // here.  (Note that they all have TMP names because I didn't give them
 // any names in their constructors.)

 if (status >= 0) {
  DataFile.Delete ();
  CompressedFile.Delete ();
  OutputFile.Close();
  OutputFile.Delete ();
 } else {

  // If the comparison failed, I am going to set their time/date stamps and
  // their attributes, using one of two methods.  Note that the Close()
  // routine will only set their attributes if the files have a valid
  // time/date stamp.  Since they are newly created, they don't have one
  // yet.  So I have to cook one up here.

  cout << "Object "
       << (OutputFile.mTimeDate.Valid() ? "has" : "does not have")
       << " a valid time stamp\n";
  struct tm* tblock;
  time_t t;
  t = time(0);
  tblock = localtime(&t);
  if (tblock->tm_sec > 30) {
   tblock->tm_year--;
   OutputFile.mTimeDate.SetTimeDate(tblock);
   cout << "Setting time stamp using tblock\n";
  } else {
   cout << "Setting time stamp using a long\n";
   OutputFile.mTimeDate.SetTimeDate(20L*365L*24L*60L*60L + 5L*24L*60L*60L + 12L
     *60L*60L); // Might be 1/1/1990
  }
  short unsigned int atts = OutputFile.mAttributes.PackedAttributes();
  atts = (short unsigned int) (atts | 1);
  OutputFile.mAttributes.SetFromPackedAttributes(atts); //Make it Read Only
  OutputFile.Close();
  CompressedObject->mStatus.SetError(status,
   DataFile.mStatus.GetStatusDetail());
 }
 cout << "Compressed object status code:    "
      << CompressedObject->mStatus.GetStatusCode()
      << endl;
 cout << "Compressed object status string:  "
      << CompressedObject->mStatus.GetStatusString()
      << endl;
 cout << "Compressed object status detail:  "
      << CompressedObject->mStatus.GetStatusDetail()
      << endl;
 delete CompressedObject;

 return status;
}

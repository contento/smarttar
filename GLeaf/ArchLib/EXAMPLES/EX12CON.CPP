/*= Archive Library v2.12 ================================================
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  EX12CON demonstrates how you can use the ReadHeaderData() and
 |  WriteHeaderData() functions to store information in an ALCompressedObject.
 |  This example derives a new class of ALCompressedObject that uses
 |  the private data area of the compressed object to store the name of
 |  the file that is compressed there.  When the object is being extracted,
 |  its name is updated in ReadHeaderData.
 |  You can also read the header data directly, without waiting for the
 |  Extract() function to be called.  This requires that you open the
 |  compressed object, then seek to location 12.
 |
 +- Functions ------------------------------------------------------------
 |  ALCompressed::ReadHeaderData()
 |  ALCompressed::WriteHeaderData()
 |
 +- Notes ----------------------------------------------------------------
 |  Compile with -DZIP to use ZIP compression.
 |
 +========================================================================*/

#include <time.h>
#include <stdlib.h>
#include <conio.h>

#include "al.h"

// This new derived class overrides the virtual function ReadHeaderData
// and WriteHeaderData.

class AL_CLASS_TYPE MyCompressed : public ALCompressedObject {
 public :
  AL_INLINE_PROTO MyCompressed(ALStorage AL_DLL_FAR &storage,
                               ALCompressor AL_DLL_FAR &compressor,
                               ALDecompressor AL_DLL_FAR &decompressor)
                               : ALCompressedObject(storage,
                                                    &compressor,
                                                    &decompressor) {; };
  virtual AL_INLINE_PROTO ~MyCompressed() {; }

  // I use this member function to read the header data whenever I want to.

  void AL_PROTO ReadHeader();

  // These special functions are declared, but not defined.  I don't want the
  // compiler to generate default versions of these functions, so I specifically
  // declare them here.

 protected :
  AL_PROTO MyCompressed(MyCompressed AL_DLL_FAR &);
  MyCompressed AL_DLL_FAR &AL_PROTO operator = (const MyCompressed AL_DLL_FAR&);

  // The two virtual functions that I override in this class.

 protected :
  int AL_PROTO WriteHeaderData(ALStorage AL_DLL_FAR * object);
  int AL_PROTO ReadHeaderData(ALStorage AL_DLL_FAR * object);
};

// In the base class, this virtual function does nothing at all.
// In the derived class, I use this function to write the name of
// the input file into the compressed object, using ArchiveLib string
// format.

int AL_PROTO MyCompressed::WriteHeaderData(ALStorage AL_DLL_FAR * object)
{
 mpStorageObject->WriteString((const char *) object->mName);
 return AL_SUCCESS;
}

// Before the output file gets extracted, this virtual function gets
// called.  I read the stored file name out of the compressed object,
// and change the name of the output object.  This code is a little
// trickier than I would like because it has to read in data in the
// ArchiveLibrary ReadString format.

int AL_PROTO MyCompressed::ReadHeaderData(ALStorage AL_DLL_FAR *output)
{
 short int len;

 if (mpStorageObject->ReadGlShort(len) < 0)
  return AL_READ_ERROR;
 char *new_string = new char[len + 1];
 if (new_string) {
  mpStorageObject->ReadBuffer((unsigned char *) new_string, len);
  new_string[len] = '\0';
 } else
 return AL_READ_ERROR;
 cout << "Output object should be named: " << new_string << "\n";
 if (output)
  output->mName = new_string;
 return AL_SUCCESS;
}

// If you want to read the header data in outside of Extract() function,
// this is what you have to do.

void AL_PROTO MyCompressed::ReadHeader()
{
 cout << "Calling ReadHeaderData directly:\n";
 mpStorageObject->Open();
 mpStorageObject->Seek(12);
 ReadHeaderData(0);
 mpStorageObject->Close();
}

main()
{
 cout << "Archive Library 2.11\nEX12CON.CPP\n\n";
 cout << "Press ESC to abort, any other key to continue..." << flush;
 int c = getch();
 cout << "\n\n";
 if (c == 0 || c == 0x1b || c == 3)
  exit(1);

 ALMemory DataFile("Memory objects can have long names");
 ALFile CompressedFile;
 ALMemory OutputFile("Original output name");
 #if defined (ZIP)
  ALPkCompressor Compressor(3, 13);
  ALPkDecompressor Decompressor;
 #else
  ALGlCompressor Compressor(AL_GREENLEAF_LEVEL_4);
  ALGlDecompressor Decompressor;
 #endif
 MyCompressed *CompressedObject;

 // Here is where I create the input data file and the compressed object.
 // This is all fairly realistic, you could write code that looks a lot
 // like this.

 CompressedObject = new MyCompressed(CompressedFile, Compressor, Decompressor);
 DataFile.Create(); // Temporary file
 for (int i = 0; i < 64; i++)
  for (int j = i; j < 64; j++)
   DataFile.WriteChar(j);
 DataFile.Close();
 cout << "Compressing\n";

 // When compressed using the derived class, the input object name will get
 // stored in the compressed file along with the compressed data.

 CompressedObject->Insert(DataFile);

 // When I extract, the output file name should get set to the same as
 // that of the input file.

 cout << "Extracting\n";
 cout << "Output object is named: " << OutputFile.mName << "\n";
 CompressedObject->Extract(OutputFile);
 int status = DataFile.Compare(OutputFile);
 cout << "Compare returned : " << status << "\n";
 cout << "Output object is named: " << OutputFile.mName << "\n";

 // To read the data in the header of a compressed object, you can also
 // seek directly:

 CompressedObject->ReadHeader();
 delete CompressedObject;
 return status;
}

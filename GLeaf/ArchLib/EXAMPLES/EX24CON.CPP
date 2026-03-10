/*= Archive Library v2.12 ================================================
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  This example gives a quick demo on how to insert and extract
 |  files from an ALCompressedObject.  It provides a command line interfac
 |  that makes it easy to test the functions to insert and extract files
 |  from an ALCompressedObject.  Remember that if you create a compressed
 |  object of your own, and you want to test it with this program,
 |  you will have to make sure your compression engine has the SAME
 |  parameters as the one used here.  Modify this program to match whatever
 |  you have used to create the objects.
 |
 +- Functions ------------------------------------------------------------
 |  ALCompressedObject::Extract()
 |  ALCompressedOjbect::Insert()
 |
 +- Notes ----------------------------------------------------------------
 |  Compile with -DZIP to use ZIP compression.
 |
 +========================================================================*/

#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <iostream.h>

#include "al.h"

// Global guys, easier to create them here just one time than have to
// create them in each of the three subroutines.

#if defined (ZIP)
 ALPkCompressor Compressor;
 ALPkDecompressor Decompressor;
#else
 ALGlCompressor Compressor(AL_GREENLEAF_LEVEL_4);
 ALGlDecompressor Decompressor(AL_GREENLEAF_LEVEL_4);
#endif
ALSpinner monitor(AL_MONITOR_OBJECTS);

// Prototypes for the three command handlers.

int test(char *object_name);
int insert(char *object_name, char *file_name);
int extract(char *object_name, char *file_name);

// main() just has to parse the command line and then pick the appropriate
// routine to call.  If the command line looks bogus we print out the
// usage information and exit.

int main(int argc, char *argv[])
{
 cout << "Archive Library 2.11\nEX24CON.CPP\n\n";
 cout << "Press ESC to abort, any other key to continue..." << flush;
 int c = getch();
 cout << "\n\n";
 if (c == 0 || c == 0x1b || c == 3)
  exit(1);

 if (argc == 3 && strcmp(argv[1], "-t") == 0)
  return test(argv[2]);
 else if (argc == 4 && strcmp(argv[1], "-i") == 0)
  return insert(argv[2], argv[3]);
 else if (argc == 4 && strcmp(argv[1], "-x") == 0)
  return extract(argv[2], argv[3]);
 cout << "Usage: EX24CON -t | -i | -x compressed_file [file]\n\n"
      << "  -t  Test the compressed file\n"
      << "  -i  Insert a file into the compressed file\n"
      << "  -x  Extract a file from the compressed file\n";
 return 1;
}

// This subroutine is called when the program is executed with the -t
// option.  It extracts the compressed file out of the compressed object,
// then prints out the CRC and length so you can compare them to the
// CRC and length in the compressed object.  It then deletes the
// temporary file and exits.
//
// Note that getting the information out of the compressed object is
// done using hardcoded reads from specific locations.

int test(char *object_name)
{
 int status;
 ALFile object_file(object_name);
 ALCompressedObject object(object_file, &Compressor, &Decompressor);
 ALFile file("");
 file.mpMonitor = &monitor;
 monitor.mlObjectStart = 0;
 status = object.Extract(file);
 file.Delete ();
 cout << "Status: " << object.mStatus.GetStatusDetail() << "\n";
 cout << "Extracted file has length: " << file.GetSize() << "\n";
 cout << "Extracted file has CRC: " << hex << file.GetCrc32() << "\n";
 object_file.Open();
 long uncompressed_size;
 long compressed_size;
 long crc_32;
 object_file.ReadGlLong(uncompressed_size);
 cout << dec
      << "Compressed object indicates an uncompressed size of "
      << uncompressed_size
      << "\n";
 object_file.ReadGlLong(compressed_size);
 cout << "Compressed object indicates a compressed size of "
      << compressed_size
      << "\n";
 object_file.ReadGlLong(crc_32);
 cout << hex
      << "Compressed object indicates an CRC of "
      << ~crc_32
      << "\n";
 return status;
}

// This routine is used to insert a file into a compressed object.  Because
// of the way compressed objects work, this is a destructive operation, so
// I ask for permission if the compressed file already exists.  After that
// it is a simple matter to call the Insert() function and then exit.

int insert(char *object_name, char *file_name)
{
 int status;
 FILE *p = fopen(object_name, "r");
 if (p) {
  char answer;
  cout << file_name << " exists.  Overwrite? (Y/N) " << flush;
  cin >> answer;
  cout << endl;
  if (answer != 'Y' && answer != 'y')
   return 1;
  fclose(p);
 }
 ALFile object_file(object_name);
 ALCompressedObject object(object_file, &Compressor, &Decompressor);
 ALFile file(file_name);
 file.mpMonitor = &monitor;
 monitor.mlObjectStart = 0;
 status = object.Insert(file);
 cout << "Status: " << object.mStatus.GetStatusDetail() << "\n";
 return status;
}

// This routine is used to extract a file from a compressed object.  Because
// of the way compressed objects work, this is a destructive operation, so
// I ask for permission if the output file already exists.  After that
// it is a simple matter to call the Extract() function and then exit.

int extract(char *object_name, char *file_name)
{
 int status;
 FILE *p = fopen(file_name, "r");
 if (p) {
  char answer;
  cout << file_name << " exists.  Overwrite? (Y/N) " << flush;
  cin >> answer;
  cout << endl;
  if (answer != 'Y' && answer != 'y')
   return 1;
  fclose(p);
 }
 ALFile object_file(object_name);
 ALCompressedObject object(object_file, &Compressor, &Decompressor);
 ALFile file(file_name);
 file.mpMonitor = &monitor;
 monitor.mlObjectStart = 0;
 status = object.Extract(file);
 cout << "Status: " << object.mStatus.GetStatusDetail() << "\n";

 return status;
}

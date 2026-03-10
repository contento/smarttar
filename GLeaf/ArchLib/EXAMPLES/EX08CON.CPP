/*= Archive Library v2.12 ================================================
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  EX08CON does a simple compression of a file to a memory buffer.
 |  It uses the raw compression function, instead of using an archive
 |  or ALCompressedObject as its target.  This is the sort of code you
 |  would use if you were compressing or expanding from your own types
 |  of proprietary files.
 |
 +- Functions ------------------------------------------------------------
 |  ALStorage::GetCrc32()
 |  ALStorage::InitCrc32()
 |  ALFile::ALFile()
 |
 +========================================================================*/

#include <conio.h>
#include <stdlib.h>

#include "arclib.h"
#include "filestor.h"
#include "memstore.h"
#include "copyengn.h"

main()
{
 cout << "Archive Library 2.11\nEX08CON.CPP\n\n";
 cout << "Press ESC to abort, any other key to continue..." << flush;
 int c = getch();
 cout << "\n\n";
 if (c == 0 || c == 0x1b || c == 3)
  exit(1);

 ALFile input("EX08CON.CPP");
 ALMemory output("In memory buffer");
 ALCompressor *engine = new ALCopyCompressor;

 // This shouldn't ever fail, which is the point of an assertion.

 AL_ASSERT_OBJECT(engine, ALCompressor, "EX08CON.CPP");

 if (input.Open() != AL_SUCCESS)
 {
  input.mStatus.SetError(AL_SUCCESS, "");
  input.mName = "..\\EXAMPLES\\EX08CON.CPP";
  input.Open();
 }

 if (input.mStatus != AL_SUCCESS)
 {
  cout << "Could not open EX08CON.CPP!\n";
  return 1;
 }

 output.Create();
 input.InitCrc32();
 engine->Compress(input, output);
 long crc = ~input.GetCrc32();
 cout << "Crc = " << hex << crc << "\n";
 delete engine;

 return 1;
}

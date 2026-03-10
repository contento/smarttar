/*= Archive Library v2.12 ================================================
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  EX34CON is a small C example demonstrating passing hALStorage objects.
 |
 +- Functions ------------------------------------------------------------
 |  newALGlCompressor()
 |  newALMemory()
 |  ALCompress()
 |  ALDecompress()
 |  deleteALStorage()
 |  deleteALCompressor()
 |  deleteALDecompressor()
 |  ALMemoryGetBuffer()
 |  ALStorageGetSize()
 |
 +========================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>

#include "al.h"
#include "cmpobj.h"

hALStorage CompressMe(char *buffer)
{
 hALCompressor Compressor;
 hALStorage CompressedMem;
 hALStorage DataMem;
 int length;

 length = strlen(buffer)+1;
 Compressor = newALGlCompressor(AL_DEFAULT, AL_DEFAULT);
 DataMem = newALMemory("memory object", buffer, length);
 CompressedMem = newALMemory("memory compressed object", 0, 0);
 ALCompress(Compressor, DataMem, CompressedMem);
 deleteALCompressor(Compressor);
 deleteALStorage(DataMem);
 return CompressedMem;
}

char *DecompressMe(hALStorage CompressedMem)
{
 hALDecompressor Decompressor;
 hALStorage OutputMem;
 long size;
 char *p;

 Decompressor = newALGlDecompressor(AL_DEFAULT);
 OutputMem = newALMemory("memory output object", 0, 0);
 size = ALStorageGetSize(CompressedMem);
 ALDecompress(Decompressor, CompressedMem, OutputMem, size);
 p = ALMemoryGetBuffer(OutputMem);
 deleteALDecompressor(Decompressor);
 deleteALStorage(OutputMem);
 return p;
}

int main()
{
 char *buffer = "aaaaaaaaaaaaaaaaaaaaaaaaa";
 hALStorage result;
 int c;

 printf("Archive Library 2.11\nEX34CON.C\n\n");
 printf("Press ESC to abort, any other key to continue...");
 fflush(stdout);
 c = getch();
 printf("\n\n");
 if (c == 0 || c == 0x1b || c == 3)
  exit(1);

 result = CompressMe(buffer);
 puts(DecompressMe(result));
 deleteALStorage(result);
 return 0;
}

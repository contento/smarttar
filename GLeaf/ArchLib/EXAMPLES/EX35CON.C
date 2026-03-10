/*= Archive Library v2.12 ================================================
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  EX35CON is a small C example demonstrating passing compressed buffers.
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

#include "memstore.h"
#include "glengn.h"

char *ExpressCompress(char* Data, long *CompressSize)
{
 hALStorage DataMem;
 hALStorage CompressedMem;
 hALCompressor Compressor;
 int DataSize;
 char *p;
 long size;

 /*
  * DataSize equals string length of Data plus one to ALSO compress
  * the termination character.
  */
 DataSize = strlen(Data) + 1;
 Compressor = newALGlCompressor(AL_GREENLEAF_LEVEL_4, 0);
 DataMem = newALMemory("memory object", Data, DataSize);
 CompressedMem = newALMemory("memory compressed object", 0, 0);
 ALCompress(Compressor, DataMem, CompressedMem);
 ALStorageOpen(CompressedMem);
 size = ALStorageGetSize(CompressedMem);
 ALStorageClose(CompressedMem);
 *CompressSize = size;
 p = ALMemoryGetBuffer(CompressedMem);
 /*
  * I have to tell the storage object that I am taking charge
  * of the buffer.  If I don't, he will delete it when I call
  * deleteALStorage(CompressedMem);
  */
 ALMemoryBaseSetBufferOwner( CompressedMem, 1 );
 deleteALCompressor(Compressor);
 deleteALStorage(CompressedMem);
 deleteALStorage(DataMem);
 return p;
}

char *ExpressUnCompress(char *Data, long CompressSize, long *ReturnSize)
{
 hALStorage CompressedMem;
 hALStorage OutputMem;
 hALDecompressor Decompressor;
 char *p;
 long size;

 Decompressor = newALGlDecompressor(AL_GREENLEAF_LEVEL_4);
 CompressedMem = newALMemory("memory compressed object", Data, (int)CompressSize);
 ALStorageOpen(CompressedMem);
 size = ALStorageGetSize(CompressedMem);
 ALStorageClose(CompressedMem);

 OutputMem = newALMemory("memory output object", 0, 0);
 ALDecompress(Decompressor, CompressedMem, OutputMem, size);
 *ReturnSize = ALStorageGetSize(OutputMem);
 p = ALMemoryGetBuffer(OutputMem);
 deleteALDecompressor(Decompressor);
 deleteALStorage(OutputMem);
 deleteALStorage(CompressedMem);
 /* Now we free Data. */
 free( Data );
 return p;
}

void main()
{
	char SampleData[] = "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";
	char *CompressedString;
	char *UncompressedString;
	long CompressSize, UnCompressSize;
 int c;

 printf("Archive Library 2.11\nEX34CON.C\n\n");
 printf("Press ESC to abort, any other key to continue...");
 fflush(stdout);
 c = getch();
 printf("\n\n");
 if (c == 0 || c == 0x1b || c == 3)
  exit(1);

	/* Pass CompressSize by value */
	CompressedString = ExpressCompress(SampleData,&CompressSize);

	/* Now to decompress */
	UncompressedString = ExpressUnCompress(CompressedString, CompressSize, &UnCompressSize);

	printf("Uncompressed String: %s",UncompressedString);
}

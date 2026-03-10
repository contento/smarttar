/*= Archive Library v2.12 ================================================
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  EX14CON creates an ALMemory storage object using an object owned
 |  memory buffer.  Then I take the buffer away so I can use it myself.
 |
 +- Functions ------------------------------------------------------------
 |  ALMemoryBaseGetBufferSize()
 |  ALMemoryGetBuffer()
 |  ALMemoryBaseGetBufferOwner()
 |  ALMemoryBaseSetBufferOwner()
 |
 +========================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

#include "arclib.h"
#include "memstore.h"

int main()
{
 hALStorage memory;
 int i;
 unsigned char *buffer;
 int c;

 printf("Archive Library 2.11\nEX14CON.C\n\n");
 printf("Press ESC to abort, any other key to continue...");
 fflush(stdout);
 c = getch();
 printf("\n\n");
 if (c == 0 || c == 0x1b || c == 3)
  exit(1);
 memory = newALMemory("My memory", 0, 0);
 ALStorageCreate(memory, AL_DEFAULT);
 for (i = 0; i < 20000; i++)
  ALStorageWriteChar(memory, i &255);

 printf("ALMemory buffer size before FlushBuffer() = %u\n",
  ALMemoryBaseGetBufferSize(memory));
 ALStorageFlushBuffer(memory);
 printf("ALMemory buffer size after FlushBuffer() = %u\n",
  ALMemoryBaseGetBufferSize(memory));
 ALStorageClose(memory);
 printf("ALMemory buffer size after Close() = %u\n",
  ALMemoryBaseGetBufferSize(memory));
 printf("ALMemory owner = %s\n",
  ALMemoryBaseGetBufferOwner(memory) ? "User" : "Object");
 ALMemoryBaseSetBufferOwner(memory, 1);
 buffer = (unsigned char *) ALMemoryGetBuffer(memory);
 for (i = 0; i < 20000; i++)
  if (buffer[i] != (unsigned char) (i &255))
   break;
  printf("Comparison %s\n", i == 20000 ? "passed" : "failed");

 deleteALStorage(memory);
 return 1;
}

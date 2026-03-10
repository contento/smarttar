/*= Archive Library v2.12 ================================================
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  EX19CON compresses an input file of your choosing into a huge buffer.
 |  It then gets a pointer into the buffer and uses that to see if things
 |  worked as they should have.
 |
 +- Functions ------------------------------------------------------------
 |  ALMemoryGetBuffer()
 |  ALCompress()
 |  ALStorageOpen()
 |  ALStorageReadChar()
 |
 +========================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include "arclib.h"
#include "copyengn.h"
#include "filestor.h"
#include "memstore.h"

main(int argc, char *argv[])
{
 int c;

 printf("Archive Library 2.11\nEX19CON.C\n\n");
 printf("Press ESC to abort, any other key to continue...");
 fflush(stdout);
 c = getch();
 printf("\n\n");
 if (c == 0 || c == 0x1b || c == 3)
  exit(1);

{
 hALStorage f;
 hALStorage h;
 hALCompressor c;
 int ch;
#if defined (AL_FLAT_MODEL)
  char *p;
  h = newALMemory("My big buffer", 0, - 1);
#else
  char _huge *p;
  h = newALHugeMemory("My big buffer", 0, - 1);
#endif
  c = newALCopyCompressor();
  if (argc > 1) {
   f = newALFile(argv[1]);
   printf("Compressing %s\n", argv[1]);
  } else {
   f = newALFile("ex19con.exe");
   printf("Compressing ex19con.exe\n");
  }
  printf("Compress returned %d\n",
   ALCompress(c, f, h));
#if defined (AL_FLAT_MODEL)
  p = ALMemoryGetBuffer(h);
#else
  p = ALHugeMemoryGetBuffer(h);
#endif
  ALStorageOpen(f);
  while ((ch = ALStorageReadChar(f)) >= 0)
   if (ch != (*p++ &0xff)) {
    printf("Mismatch!\n");
    return 0;
   }
   printf("Comparison passed!\n");

  deleteALStorage(h);
  deleteALStorage(f);
  deleteALCompressor(c);
 }

 return 1;
}

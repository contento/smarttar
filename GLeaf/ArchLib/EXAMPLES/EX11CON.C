/*= Archive Library v2.12 ================================================
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  EX11CON is used to test the hardware independent WriteGl...()
 |  functions.  It does this by creating a test file, writing a bunch of
 |  data out to it, then reading the same data back in.
 |
 +- Functions ------------------------------------------------------------
 |  ALStorageCreate()
 |  ALStorageIsOpen()
 |  ALStorageReadLong()
 |  ALStorageReadShort()
 |  ALStorageWriteChar()
 |  ALStorageWriteLong()
 |  ALStorageWriteShort()
 |  ALStorageWriteString()
 |
 +- Notes ----------------------------------------------------------------
 |  The first time you run it, it just creates TESTFILE.DAT.  From then on,
 |  it will perform the read tests.  If you want it to go back and create
 |  the file again, you just need to delete TESTFILE.DAT.
 |
 +========================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>

#include "al.h"

int create_test(void);
int read_test(void);

int main()
{
 FILE *f = fopen("TESTFILE.DAT", "r");
 int c;

 printf("Archive Library 2.11\nEX11CON.C\n\n");
 printf("Press ESC to abort, any other key to continue...");
 fflush(stdout);
 c = getch();
 printf("\n\n");
 if (c == 0 || c == 0x1b || c == 3)
  exit(1);

 if (f) {
  fclose(f);
  read_test();
 } else
 create_test();
 return 1;
}

/*
  This routine is called if the file didn't already exist when the program
  started.  It creates the file, then writes out the data using the
  portable functions.  The data is pretty simple, no tricks here.
*/

int create_test()
{
 hALStorage file;
 long spot;
 int i;

 printf("Creating TESTFILE.DAT\n");
 printf("Run this program a second time to test data file structure.\n");
 file = newALFile("TESTFILE.DAT");
 ALStorageCreate(file, AL_DEFAULT);
 if (!ALStorageIsOpen(file)) {
  printf("Error creating file!\n");
  return 1;
 }
 ALStorageWriteGlLong(file, 0x12345678L);
 ALStorageWriteGlLong(file, 0xfedcba98L);
 ALStorageWritePkLong(file, 0x87654321L);
 ALStorageWritePkShort(file, 0x2345);
 ALStorageWriteGlShort(file, 0x5566);
 ALStorageWriteGlShort(file, (short int) 0x9988);
 for (i = 0; i < 256; i++)
  ALStorageWriteChar(file, i);
 ALStorageWriteString(file, "ABCDE");
 spot = ALStorageTell(file);
 ALStorageWriteGlLong(file, spot);
 deleteALStorage(file);
 return 0;
}

/*
  The second time you run this program, you read back in all the data
  from the file that was created earlier.  This gets a little more
  complicated, because all the data has to be read into local variables,
  then checked for errors.  That's why this routine is twice as long
  as the guy that writes stuff out.
*/

int read_test()
{
 hALStorage file;
 long dat1 = 0;
 long dat2 = 0;
 short int dat3 = 0;
 short int dat4 = 0;
 int i;
 short length = 0;
 char buffer[5];
 long my_spot;
 long his_spot = 0;

 printf("Reading TESTFILE.DAT\n");
 file = newALFile("TESTFILE.DAT");
 ALStorageOpen(file);
 if (!ALStorageIsOpen(file)) {
  printf("Error opening file!\n");
  return 1;
 }
 ALStorageReadGlLong(file, &dat1);
 ALStorageReadGlLong(file, &dat2);
 if (dat1 != 0x12345678L || dat2 != 0xfedcba98L) {
  printf("Error reading long data\n");
  return 1;
 }
 ALStorageReadPkLong(file, &dat1);
 if (dat1 != 0x87654321L) {
  printf("Error reading long Pk data\n");
  return 1;
 }
 ALStorageReadPkShort(file, &dat3);
 if (dat3 != 0x2345) {
  printf("Error reading short Pk data\n");
  return 1;
 }
 ALStorageReadGlShort(file, &dat3);
 ALStorageReadGlShort(file, &dat4);
 if (dat3 != 0x5566 || dat4 != (short) 0x9988) {
  printf("Error reading short data\n");
  return 1;
 }
 for (i = 0; i < 256; i++) {
  int c = ALStorageReadChar(file);
  if (c != i) {
   printf("Error reading characater data, position %d\n", i);
   return 1;
  }
 }

/*
  Reading strings is still a hassle, you have to do it manually, there is
  no readstring function available as a public member.
*/

 ALStorageReadGlShort(file, &length);
 ALStorageReadBuffer(file, (unsigned char *) buffer, 5);
 if (length != 5 || memcmp(buffer, "ABCDE", 5) != 0) {
  printf("Error reading string\n");
  return 1;
 }
 my_spot = ALStorageTell(file);
 ALStorageReadGlLong(file, &his_spot);
 if (my_spot != his_spot) {
  printf("Error in Tell()!\n");
  return 1;
 }
 printf("File passed all tests.\n");
 deleteALStorage(file);
 return 0;
}

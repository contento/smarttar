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
 |  ALStorage::Create()
 |  ALStorage::IsOpen()
 |  ALStorage::ReadGlLong()
 |  ALStorage::ReadGlShort()
 |  ALStorage::Tell()
 |  ALStorage::WriteChar()
 |  ALStorage::WriteGlLong()
 |  ALStorage::WriteGlShort()
 |  ALStorage::WriteString()
 |
 +- Notes ----------------------------------------------------------------
 |  The first time you run it, it just creates TESTFILE.DAT.  From then on,
 |  it will perform the read tests.  If you want it to go back and create
 |  the file again, you just need to delete TESTFILE.DAT.
 |
 +========================================================================*/

#include <iostream.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

#include "al.h"

void create_test();
void read_test();

main()
{
 FILE *f = fopen("TESTFILE.DAT", "r");

 cout << "Archive Library 2.11\nEX11CON.CPP\n\n";
 cout << "Press ESC to abort, any other key to continue..." << flush;

 int c = getch();
 cout << "\n\n";
 if (c == 0 || c == 0x1b || c == 3)
  exit(1);

 if (f) {
  fclose(f);
  read_test();
 } else
 create_test();
 return 1;
}

// This routine is called if the file didn't already exist when the program
// started.  It creates the file, then writes out the data using the
// portable functions.  The data is pretty simple, no tricks here.

void create_test()
{
 cout << "Creating TESTFILE.DAT\n";
 cout << "Run this program a second time to test data file structure\n";
 ALFile file("TESTFILE.DAT");
 file.Create();
 if (!file.IsOpen()) {
  cout << "Error creating file!\n";
  return;
 }
 file.WriteGlLong(0x12345678L);
 file.WriteGlLong(0xfedcba98L);
 file.WritePkLong(0x87654321L);
 file.WritePkShort(0x02345);
 file.WriteGlShort(0x5566);
 file.WriteGlShort((short int) 0x9988);
 for (int i = 0; i < 256; i++)
  file.WriteChar(i);
 file.WriteString("ABCDE");
 long spot = file.Tell();
 file.WriteGlLong(spot);
}

// The second time you run this program, you read back in all the data
// from the file that was created earlier.  This gets a little more
// complicated, because all the data has to be read into local variables,
// then checked for errors.  That's why this routine is twice as long
// as the guy that writes stuff out.

void read_test()
{
 cout << "Reading TESTFILE.DAT\n";
 ALFile file("TESTFILE.DAT");
 file.Open();
 if (!file.IsOpen()) {
  cout << "Error opening file!\n";
  return;
 }
 long dat1 = 0;
 long dat2 = 0;
 file.ReadGlLong(dat1);
 file.ReadGlLong(dat2);
 if (dat1 != 0x12345678L || dat2 != 0xfedcba98L) {
  cout << "Error reading long data\n";
  return;
 }
 file.ReadPkLong(dat1);
 if (dat1 != 0x87654321L) {
  cout << "Error reading long Pk data\n";
  return;
 }
 short int dat3 = 0;
 short int dat4 = 0;
 file.ReadPkShort(dat3);
 if (dat3 != 0x02345) {
  cout << "Error reading short Pk data\n";
  return;
 }
 file.ReadGlShort(dat3);
 file.ReadGlShort(dat4);
 if (dat3 != 0x5566 || dat4 != (short) 0x9988) {
  cout << "Error reading short data\n";
  return;
 }
 for (int i = 0; i < 256; i++) {
  int c = file.ReadChar();
  if (c != i) {
   cout << "Error reading characater data, position " << i << ".\n";
   return;
  }
 }

 // Reading strings is still a hassle, you have to do it manually, there is
 // no readstring function available as a public member.

 short length = 0;
 unsigned char buffer[5] = "";
 file.ReadGlShort(length);
 file.ReadBuffer(buffer, 5);
 if (length != 5 || memcmp(buffer, "ABCDE", 5) != 0) {
  cout << "Error reading string\n";
  return;
 }
 long his_spot = 0;
 long my_spot = file.Tell();
 file.ReadGlLong(his_spot);
 if (my_spot != his_spot) {
  cerr << "Error in Tell()!\n";
  return;
 }
 cout << "File passed all tests.\n";
}

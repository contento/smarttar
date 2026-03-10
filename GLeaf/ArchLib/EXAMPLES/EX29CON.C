/*= Archive Library v2.12 ================================================
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  EX29CON is used to make sure our file timestamping is working properly.
 |  It does this by checking the time stamp of this program, ex29con.exe,
 |  and creating a new file called ex29con.bak and manipulating its stamp.
 |
 +- Functions ------------------------------------------------------------
 |  ALStorageGetTime()
 |  ALStorageGetDate()
 |
 +========================================================================*/

#include <stdio.h>
#include <conio.h>
#include <process.h>

#include "arclib.h"
#include "filestor.h"

main()
{
 int c;
 short int h;
 short int m;
 short int s;
 short int month;
 short int date;
 short int year;
 hALStorage f;

 printf("Archive Library 2.11\nEX29CON.C\n\n");
 printf("Press ESC to abort, any other key to continue...");
 fflush(stdout);
 c = getch();
 printf("\n\n");
 if (c == 0 || c == 0x1b || c == 3)
  exit(1);

 f = newALFile("ex29con.exe");
 ALStorageOpen(f);
 ALStorageClose(f);
 ALStorageGetDate(f, &month, &date, &year);
 ALStorageGetTime(f, &h, &m, &s);

 printf("ex29con.exe was : %d/%d/%d  %02d:%02d:%02d\n", month, date, year,
                                                        h, m, s);
 deleteALStorage(f);

 f = newALFile("ex29con.bak");
 ALStorageCreate(f, 1024);
 printf("ex29con.bak should change to 7/4/1996, 18:01:00\n");
 printf("Before: ");
 system("DIR ex29con.*");
 ALStorageSetDate(f, 7, 4, 1996);
 ALStorageSetTime(f, 18, 01, 01);
 ALStorageClose(f);
 printf("After: ");
 system("DIR ex29con.*");

 deleteALStorage(f);

 return 1;
}

/* SHRTEST1.C - First part of the shared segment test program */

/************************************************************************/
/*	Copyright (C) 1986-1990 Phar Lap Software, Inc.			*/
/*	Unpublished - rights reserved under the Copyright Laws of the	*/
/*	United States.  Use, duplication, or disclosure by the 		*/
/*	Government is subject to restrictions as set forth in 		*/
/*	subparagraph (c)(1)(ii) of the Rights in Technical Data and 	*/
/*	Computer Software clause at 252.227-7013.			*/
/*	Phar Lap Software, Inc., 60 Aberdeen Ave., Cambridge, MA 02138	*/
/************************************************************************/

#include <stdio.h>
#define INCL_DOSERRORS
#define INCL_DOSMEMMGR
#include <os2.h>
#include <phapi.h>
#include <string.h>
#include <process.h>

UCHAR DosAllocShrSeg_name[] = "DosAllocShrSeg";
UCHAR spawnl_name[] = "spawnl";
UCHAR DosFreeSeg_name[] = "DosFreeSeg";

#define FP_MAKE(off, sel) (((_segment)(sel)) :> (void _based(void) *)(off))


/*

main function

*/

main()

{

	static UCHAR name1[] = "\\sharemem\\seg1";
	static UCHAR name2[] = "\\sharemem\\seg2";
	static UCHAR name3[] = "\\sharemem\\seg3";

	static UCHAR msg1[] = "This is shared segment #1";
	static UCHAR msg2[] = "Hello from shared segment #2";
	static UCHAR msg3[] = "This is the last shared segment";

	SEL sel1, sel2, sel3;
	INT rc;

	/* Create the 3 shared segments */

	rc = DosAllocShrSeg(sizeof(msg1), name1, &sel1);
	if(!error_check(rc, DosAllocShrSeg_name))
	{
		strcpy(FP_MAKE(0, sel1), msg1);
	} 
	rc = DosAllocShrSeg(sizeof(msg2), name2, &sel2);
	if(!error_check(rc, DosAllocShrSeg_name))
	{
		strcpy(FP_MAKE(0, sel2), msg2);
	} 
	rc = DosAllocShrSeg(sizeof(msg3), name3, &sel3);
	if(!error_check(rc, DosAllocShrSeg_name))
	{
		strcpy(FP_MAKE(0, sel3), msg3);
	} 

	/* Exec. to the second part of the test */

	rc = spawnl(P_WAIT, "SHRTEST2", NULL);
	error_check(rc, spawnl_name);	

	/* Print out the shared segments after running the second
	   part of the test. */

	printf("Shared segment #1 is now \"%s\".\n", FP_MAKE(0, sel1));
	printf("Shared segment #2 is now \"%s\".\n", FP_MAKE(0, sel2));
	printf("Shared segment #3 is now \"%s\".\n", FP_MAKE(0, sel3));

	/* Free the 3 shared segments */

	rc = DosFreeSeg(sel1);
	error_check(rc, DosFreeSeg_name);
	rc = DosFreeSeg(sel2);
	error_check(rc, DosFreeSeg_name);
	rc = DosFreeSeg(sel3);
	error_check(rc, DosFreeSeg_name);

	/* Exit */

	return 0;

}


/*

error_check - Check for an error return 

*/

error_check(rc, namep)

INT rc;			/* Return code */
UCHAR *namep;		/* Name pointer */

{

	if(rc == 0)
		return FALSE;
	printf("Unexpected error in function \"%s\" -- error code %d.\n",
	       namep, rc);
	return TRUE;

}






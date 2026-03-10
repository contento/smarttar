/* SHRTEST2.C - Second part of the shared segment test program */

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

UCHAR DosGetShrSeg_name[] = "DosGetShrSeg";
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

	static UCHAR new_msg1[] = "Moe";
	static UCHAR new_msg2[] = "Curly";
	static UCHAR new_msg3[] = "Larry";

	SEL sel1, sel2, sel3;
	INT rc;

	/* Get the 3 shared segments */

	rc = DosGetShrSeg(name1, &sel1);
	if(!error_check(rc, DosGetShrSeg_name))
	{
		printf("Shared segment #1 is \"%s\".\n", FP_MAKE(0, sel1));
		strcpy(FP_MAKE(0, sel1), new_msg1);
	} 
	rc = DosGetShrSeg(name2, &sel2);
	if(!error_check(rc, DosGetShrSeg_name))
	{
		printf("Shared segment #2 is \"%s\".\n", FP_MAKE(0, sel2));
		strcpy(FP_MAKE(0, sel2), new_msg2);
	} 
	rc = DosGetShrSeg(name3, &sel3);
	if(!error_check(rc, DosGetShrSeg_name))
	{
		printf("Shared segment #3 is \"%s\".\n", FP_MAKE(0, sel3));
		strcpy(FP_MAKE(0, sel3), new_msg3);
	} 

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
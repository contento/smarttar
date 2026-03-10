/* REPLACE.C - Sample API replacement package for 286|DOS-Extender */

/************************************************************************/
/*	Copyright (C) 1986-1990 Phar Lap Software, Inc.			*/
/*	Unpublished - rights reserved under the Copyright Laws of the	*/
/*	United States.  Use, duplication, or disclosure by the 		*/
/*	Government is subject to restrictions as set forth in 		*/
/*	subparagraph (c)(1)(ii) of the Rights in Technical Data and 	*/
/*	Computer Software clause at 252.227-7013.			*/
/*	Phar Lap Software, Inc., 60 Aberdeen Ave., Cambridge, MA 02138	*/
/************************************************************************/

#define INCL_DOSERRORS
#include <os2.h>
#include <phapi.h>
#include <string.h>


/*

API table entry format

*/

typedef struct
{
	UCHAR *mnamep;		/* Module name pointer */
	UCHAR *pnamep;		/* Pointer to the entry point name or
				   ordinal number string */
	PFN new_funcp;		/* New function pointer */
	PFN *old_funcpp;	/* Pointer to old function pointer */
} API_ENT;


/*

API table

*/

UCHAR doscalls[] = "doscalls";

PFN oDosAllocShrSegp = {NULL};
PFN oDosGetShrSegp = {NULL};
PFN oDosFreeSegp = {NULL};

USHORT APIENTRY _loadds myDosAllocShrSeg(USHORT size, PSZ namep, PSEL selp);
USHORT APIENTRY _loadds myDosFreeSeg(SEL sel);
USHORT APIENTRY _loadds myDosGetShrSeg(PSZ namep, PSEL selp);

API_ENT api_tab[] =
{

/* List of functions to be replaced */

doscalls,	"DOSALLOCSHRSEG", myDosAllocShrSeg,	&oDosAllocShrSegp,
doscalls,	"DOSFREESEG",	  myDosFreeSeg,  	&oDosFreeSegp,
doscalls,	"DOSGETSHRSEG",   myDosGetShrSeg,  	&oDosGetShrSegp,

/* End of table marker */

NULL
};


/*

Save all and restore all register macros

*/

#define SAVEALL _asm pusha \
		_asm push es

#define RESTALL _asm pop es \
		_asm popa

/*

Shared segment name table

*/

#define SHR_MAXSIZE 512

UCHAR shr_table[SHR_MAXSIZE];

UINT shr_size;


/*

Function declarations

*/

UCHAR *shr_locname(PSZ namep);
UCHAR *shr_locsel(SEL sel);
void shr_delete(UCHAR *p);


/*

shrseg_init - Initialization routine

*/

shrseg_init()

{

	register API_ENT *p;
	register INT rc;
	UCHAR fail[256];
	USHORT mhand;
	UCHAR *q;

	/* Walk the API table, installing the our routines in
	   place of the standard 286|DOS-Extender routines */

	for(p = api_tab; p->mnamep != NULL; ++p)
	{
		rc = DosGetModHandle(p->mnamep, &mhand);
		if(rc != 0)
		{
		 	rc = DosLoadModule(fail, sizeof(fail), 
				           p->mnamep, &mhand);
                        if(rc != 0)
			{
				wilprintf("SHRSEG.DLL: Can't load \"%s.DLL\" -- error code %d.\n",
				          p->mnamep, rc);
				return 0;
			}
		}
		q = p->pnamep;
		if(*q == '#')
			++q;
		rc = DosGetProcAddr(mhand, p->pnamep, p->old_funcpp);
		if(rc != 0)
			*p->old_funcpp = NULL;
		rc = DosSetProcAddr(mhand, p->pnamep, p->new_funcp);
		if(rc != 0)
		{
		 	wilprintf("SHRSEG.DLL: Can't set API function \"%s.%s\" -- error code %d.\n",
				  p->mnamep, q, rc);
			return 0;
		}
	}

	/* Return */

	return 1;

}	


/*

myDosAllocShrSeg - My version of the OS/2 DosAllocShrSeg function

*/

USHORT APIENTRY _loadds myDosAllocShrSeg(USHORT size, PSZ namep, PSEL selp)

{

	INT rc;

	/* Save all of the registers */

	SAVEALL;

	/* If the shared segment already exists, then return with
	   an error. */

	if(shr_locname(namep) != NULL)
	{
		rc = ERROR_ALREADY_EXISTS;
		goto ret1;
	}

	/* Allocate the segment */
	
	rc = DosAllocSeg(size, selp, 0);
	if(rc != 0)
		goto ret1;

	/* Add the name of the segment to the shared segment table */

	rc = shr_add(namep, *selp);
	if(rc != 0)
	{
		DosFreeSeg(*selp);
	}

	/* Restore the saved registers and return. */

ret1:	RESTALL;
	return rc;

} 


/*

myDosGetShrSeg - My version of the OS/2 DosGetShrSeg function

*/

USHORT APIENTRY _loadds myDosGetShrSeg(PSZ namep, PSEL selp)

{

	UCHAR *p;
	INT rc;

	/* Save all of the registers */

	SAVEALL;

	/* Look up the segment name in the shared segment table. 
	   If the name can't be found, return with an error. */

	p = shr_locname(namep);
	if(p == NULL)
	{
		rc = ERROR_FILE_NOT_FOUND;
		goto ret1;
	}
	
	/* Make an data alias to the shared segment. */ 

	rc = DosCreateDSAlias(*(PSEL)p, selp);

	/* Restore the registers and return */

ret1:	RESTALL;
	return rc;

}


/*

myDosFreeSeg - My version of the OS/2 DosFreeSeg function

*/

USHORT APIENTRY _loadds myDosFreeSeg(SEL sel)

{

	UCHAR *p;
	INT rc;

	/* Save all of the registers */

	SAVEALL;

	/* If the selector being freed is a shared segment, then
	   delete its name from the shared segment name table. */
	
	p = shr_locsel(sel);
	if(p != NULL)
		shr_delete(p);
	
	/* Now call the standard DosFreeSeg routine to actually
	   free the segment. */

	rc = (*oDosFreeSegp)(sel);

	/* If the saved ES contains the selector just freed, then
	   zero it to avoid a memory protection fault. */

	_asm 	   
	{
		pop	ax
		cmp	ax,sel
		jne	label1
		sub	ax,ax
	label1:	push	ax
	}

	/* Restore the registers and return */

ret1:	RESTALL;
	return rc;

}


/*

shr_locname - Locate a name in the shared segment table

*/

UCHAR *shr_locname(PSZ namep)

{

	UCHAR *p;		
	UCHAR *ep;

	ep = shr_table + shr_size;
	for(p = shr_table; p < ep; 
	    p += sizeof(SEL) + strlen(p + sizeof(SEL)) + 1)
	{
		if(strcmpi(p + sizeof(SEL), namep) == 0)
		{
			return p;
		}
	}
	return NULL;

}


/*

shr_locsel - Locate a selector in the shared segment table

*/

UCHAR *shr_locsel(SEL sel)

{

	UCHAR *p;		
	UCHAR *ep;

	ep = shr_table + shr_size;
	for(p = shr_table; p < ep;
	    p += sizeof(SEL) + strlen(p + sizeof(SEL)) + 1)
	{
		if(*(PSEL)p == sel)
		{
			return p;
		}
	}
	return NULL;

}


/*

shr_add - Add a name to the shared segment table

*/

shr_add(PSZ namep, SEL sel)

{


	INT n;
	UCHAR *p;
	INT rc;

	/* Get the length of the new name */

	n = strlen(namep);

	/* Return with an error if the shared segment name table doesn't
	   have enough room for the new name */

	if(shr_size + sizeof(SEL) + n + 1 > sizeof(shr_table))
	{
		rc = ERROR_SHARING_BUFFER_EXCEEDED;
		goto ret1;
	}

	/* Store the new selector and new name at the end of the
	   shared segment name table. */

	p = shr_table + shr_size;
	*(PSEL)p = sel;
	strcpy(p + sizeof(SEL), namep);
	shr_size += sizeof(SEL) + n + 1;
	
	/* Return */

	rc = 0;
ret1:	return rc;

}


/*

shr_delete - Delete an entry in the shared segment name table

*/

void shr_delete(UCHAR *p)

{
	
	UINT n;
	UINT soff;
	UINT eoff;

	/* Calculate the size of the entry in the shared segment
	   name table */

	n = sizeof(SEL) + strlen(p + sizeof(SEL)) + 1;

	/* If there are any entries in the table after the entry
	   being delete, then move them forware on top of the
	   entry being deleted. */

	soff = p - shr_table;
	eoff = soff + n;
	if(eoff < shr_size)
		memcpy(p, p + n, shr_size - eoff);

	/* Adjust the table size. */

	shr_size -= n;

	/* Return */

	return;

}	
	



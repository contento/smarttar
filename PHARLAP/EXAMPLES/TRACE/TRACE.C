/* TRACE.C - Sample API trace package for 286|DOS-Extender */

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
#include <phapi.h>


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

Trace table

*/

UCHAR doscalls[] = "doscalls";


USHORT  APIENTRY _loadds tDosAllocSeg(USHORT size, PSEL selp, USHORT flags);
USHORT (APIENTRY *oDosAllocSegp)(USHORT size, PSEL selp, USHORT flags);
USHORT  APIENTRY _loadds tDosFreeSeg(SEL sel);
USHORT (APIENTRY *oDosFreeSegp)(SEL sel);
USHORT  APIENTRY _loadds tDosReallocSeg(USHORT nsize, SEL sel);
USHORT (APIENTRY *oDosReallocSegp)(USHORT nsize, SEL sel);
USHORT  APIENTRY _loadds tDosAllocHuge(USHORT segcnt, USHORT psize, 
		                      PSEL selp, USHORT maxcnt, USHORT flags);
USHORT (APIENTRY *oDosAllocHugep)(USHORT segcnt, USHORT psize, 
		                      PSEL selp, USHORT maxcnt, USHORT flags);
USHORT  APIENTRY _loadds tDosReallocHuge(USHORT segcnt, USHORT psize, SEL sel);
USHORT (APIENTRY *oDosReallocHugep)(USHORT segcnt, USHORT psize, SEL sel);

API_ENT trace_tab[] =
{

/* List of functions to be traced */

doscalls,	"DOSALLOCSEG",	  (PFN)tDosAllocSeg,	(PFN*)&oDosAllocSegp,
doscalls,	"DOSFREESEG",	  (PFN)tDosFreeSeg,	(PFN*)&oDosFreeSegp,
doscalls,	"DOSREALLOCSEG",  (PFN)tDosReallocSeg,	(PFN*)&oDosReallocSegp,
doscalls,	"DOSALLOCHUGE",	  (PFN)tDosAllocHuge,	(PFN*)&oDosAllocHugep,
doscalls,	"DOSREALLOCHUGE", (PFN)tDosReallocHuge, (PFN*)&oDosReallocHugep,

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

ULONG realsize(USHORT size);


/*

trace_init - Initialization routine

*/

trace_init()

{

	extern void pascal _far trace_cleanup(USHORT);

	register API_ENT *p;
	register INT rc;
	UCHAR fail[256];
	USHORT mhand;

	/* Walk the trace table, installing the trace routines in
	   place of the standard API routines */

	for(p = trace_tab; p->mnamep != NULL; ++p)
	{
		rc = DosGetModHandle(p->mnamep, &mhand);
		if(rc != 0)
		{
		 	rc = DosLoadModule(fail, sizeof(fail), 
				           p->mnamep, &mhand);
                        if(rc != 0)
				return 0;
		}
		rc = DosGetProcAddr(mhand, p->pnamep, p->old_funcpp);
		if(rc != 0)
			return 0;
		rc = DosSetProcAddr(mhand, p->pnamep, p->new_funcp);
		if(rc != 0)
			return 0;
	}

	/* Install a clean up routine */

	DosExitList(EXLST_ADD, trace_cleanup);

	/* Return */

	return 1;

}	


/*

trace_cleanup - Trace cleanup routine

*/

void pascal _far trace_cleanup(USHORT tc)

{

	wilprintf("TRACE.DLL termination code -- %d.\n", tc);

	DosExitList(EXLST_EXIT, 0);

}


/*

realsize - Real size routine

*/

ULONG realsize(USHORT size)

{

	if(size == 0)
		return 65536L;
	else
		return size;

}


/*

Trace routines

*/	

USHORT APIENTRY _loadds tDosAllocSeg(USHORT size, PSEL selp, USHORT flags)

{

	int rc;
	
	SAVEALL;
	rc = (*oDosAllocSegp)(size, selp, flags);
	wilprintf("DosAllocSeg Size=<<%04lX>>, Flags=<<%04X>>, ", 
	          realsize(size), flags);
	if(rc != 0)
		wilprintf("Error code=%d", rc);
	else
		wilprintf("Selector=<<%04X>>", *selp);
	wilprintf("\n");
	RESTALL;

	return rc;

}

USHORT APIENTRY _loadds tDosFreeSeg(SEL sel)

{

	int rc;

	SAVEALL;
	rc = (*oDosFreeSegp)(sel);
	wilprintf("DosFreeSeg Selector=<<%04X>>", sel);
	if(rc != 0)
		wilprintf(", Error code=%d", rc);
	wilprintf("\n");
	_asm 		/* If the old ES == SEL, then old ES = 0 */
	{
		pop	ax
		cmp	ax,sel
		jne	label1
		sub	ax,ax
	label1:	push	ax
	}
	RESTALL;

	return rc;

}

USHORT APIENTRY _loadds tDosReallocSeg(USHORT nsize, SEL sel)

{

	int rc;
	
	SAVEALL;
	rc = (*oDosReallocSegp)(nsize, sel);
	wilprintf("DosReallocSeg Selector=<<%04X>>, New size=<<%04lX>>", 
	          sel, realsize(nsize));
	if(rc != 0)
		wilprintf(", Error code=%d", rc);
	wilprintf("\n");
	RESTALL;

	return rc;

}

USHORT APIENTRY _loadds tDosAllocHuge(USHORT segcnt, USHORT psize, 
		                      PSEL selp, USHORT maxcnt, USHORT flags)

{

	int rc;
	
	SAVEALL;
	rc = (*oDosAllocHugep)(segcnt, psize, selp, maxcnt, flags);
	wilprintf("DosAllocHuge Icount=<<%d>>, Maxcount=<<%d>>, Psize=<<%04X>>, Flags=<<%04X>>,\n",
	          segcnt, maxcnt, psize, flags);
	wilprintf("             ");
	if(rc != 0)
		wilprintf("Error code=%d", rc);
	else
		wilprintf("Selector=<<%04X>>", *selp);
	wilprintf("\n");
	RESTALL;

	return rc;

}

USHORT APIENTRY _loadds tDosReallocHuge(USHORT segcnt, USHORT psize, SEL sel)

{

	int rc;
	
	SAVEALL;
	rc = (*oDosReallocHugep)(segcnt, psize, sel);
	wilprintf("DosReallocHuge Selector=<<%04X>>, Newcount=<<%d>>, Psize=<<%04X>>",
	          sel, segcnt, psize);
	if(rc != 0)
		wilprintf(",\n               Error code=%d", rc);
	wilprintf("\n");
	RESTALL;

	return rc;

}

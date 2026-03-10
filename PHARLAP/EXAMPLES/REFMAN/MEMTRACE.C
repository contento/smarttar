/*
MEMTRACE.C - Sample API trace package for 286|DOS-Extender

bcc286 -dll memtrace.c memtrace.def -start=START

MEMTRACE.DEF:
	LIBRARY		MEMTRACE
	DESCRIPTION 'SAMPLE API TRACE PACKAGE'
	IMPORTS		_r286printf=PHAPI._wilprintf

*/

#include <stdlib.h>
#include <stdio.h>
#include <phapi.h>

extern void r286printf(char *, ...);

/* pointers to existing functions */
USHORT (APIENTRY *fDosAllocSeg)(USHORT size, PSEL selp, USHORT flags);
USHORT (APIENTRY *fDosFreeSeg)(SEL sel);
USHORT (APIENTRY *fDosReallocSeg)(USHORT nsize, SEL sel);

/* prototypes for new (trace) functions */
USHORT APIENTRY _loadds tDosAllocSeg(USHORT size, PSEL selp, USHORT flags);
USHORT APIENTRY _loadds tDosFreeSeg(SEL sel);
USHORT APIENTRY _loadds tDosReallocSeg(USHORT nsize, SEL sel);

/* auto-initialization entry point to install trace functions */
USHORT APIENTRY Start(void)
{
	USHORT doscalls;
	USHORT rc;
	
	/* get handle to DOSCALLS */
	if (DosGetModHandle("DOSCALLS", &doscalls) != 0)
		if (DosLoadModule(0, 0, "DOSCALLS", &doscalls) != 0)
			return 0;	/* can't load DLL */
	
	/* get function pointers to existing functions */
	rc = DosGetProcAddr(doscalls, "DOSALLOCSEG", (PPFN)&fDosAllocSeg);
	if (! rc) rc = DosGetProcAddr(doscalls, "DOSFREESEG", (PPFN)&fDosFreeSeg);
	if (! rc) rc = DosGetProcAddr(doscalls, "DOSREALLOCSEG", (PPFN)&fDosReallocSeg);
	
	/* install the new (trace) functions */
	if (! rc) rc = DosSetProcAddr(doscalls, "DOSALLOCSEG", (PFN)tDosAllocSeg);
	if (! rc) rc = DosSetProcAddr(doscalls, "DOSFREESEG", (PFN)tDosFreeSeg);
	if (! rc) rc = DosSetProcAddr(doscalls, "DOSREALLOCSEG", (PFN)tDosReallocSeg);
	
	return !rc;	/* <>0 = okay to load DLL */
}

/* Save all and restore all register macros */

#ifdef _MSC_VER
#define SAVEALL() _asm pusha \
                  _asm push es

#define RESTALL() _asm pop es \
	          _asm popa
#else
#define SAVEALL() _asm pusha ; _asm push es

#define RESTALL() _asm pop es ; _asm popa
#endif

/* Trace routines */	
USHORT APIENTRY _loadds tDosAllocSeg(USHORT size, PSEL selp, USHORT flags)
{
	USHORT rc;
	
	SAVEALL();
	rc = (*fDosAllocSeg)(size, selp, flags);
	r286printf("DosAllocSeg Size=%04lX", size ? (long) size : 10000L);
	r286printf(", Flags=%04X, ", flags);
	if (rc != 0)
		r286printf("Error code=%d", rc);
	else
		r286printf("Selector=%04X", *selp);
	r286printf("\n");
	RESTALL();

	return rc;
}

USHORT APIENTRY _loadds tDosFreeSeg(SEL sel)
{
	USHORT rc;

	SAVEALL();
	/* If the old ES == SEL, then old ES = 0 */
	/* This compensates for a bug on the 80x86 */
	_asm	pop	ax
	_asm	cmp	ax,sel
	_asm	jne	label1
	_asm	sub	ax,ax
label1:	_asm	push 	ax
	rc = (*fDosFreeSeg)(sel);
	r286printf("DosFreeSeg Selector=%04X", sel);
	if (rc != 0)
		r286printf(", Error code=%d", rc);
	r286printf("\n");
	RESTALL();

	return rc;
}

USHORT APIENTRY _loadds tDosReallocSeg(USHORT nsize, SEL sel)
{
	USHORT rc;
	
	SAVEALL();
	rc = (*fDosReallocSeg)(nsize, sel);
	r286printf("DosReallocSeg Selector=%04X, New size=", sel);
	r286printf("%04lX", nsize ? (long) nsize : 10000L);
	if (rc != 0)
		r286printf(", Error code=%d", rc);
	r286printf("\n");
	RESTALL();

	return rc;
}


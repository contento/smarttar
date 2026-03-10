#include <stdio.h>
#include <stdlib.h>
#include <alloc.h>

#include <dos.h>
#include <phapi.h>

#ifdef __BORLANDC__

struct pharheapinfo
{
	unsigned size;
	unsigned prev_real;
	unsigned prev_free;
	unsigned next_free;
};

extern unsigned __base;
extern unsigned __top;
extern unsigned __hincr;
	
int heapwalk(struct heapinfo *hi)
{	
	int rc;
	struct pharheapinfo *this,*next;
	
	if (hi->ptr == NULL)
	{
		/* if there is no heap, return _HEAPEMPTY */
		if (__base == 0) {
			rc = _HEAPEMPTY;
			goto end_walk;
		}

		/* find the start of the heap */
		this = MAKEP(__base,0);
		next = MAKEP(__base,this->prev_real);
	}
	else
	{
		/* get the start of this pharheapinfo structure */
		this = MAKEP(FP_SEG(hi->ptr),FP_OFF(hi->ptr));
		
		/* if this is the end, return _HEAPEND */
		if ((FP_SEG(hi->ptr) == __top) &&
			(FP_OFF(hi->ptr) == 0)) {
			rc = _HEAPEND;
			goto end_walk;
			}

		/* find the next block in the heap */
		if (FP_OFF(hi->ptr) == 0)
			this = MAKEP(FP_SEG(hi->ptr)+__hincr,0);
		next = MAKEP(FP_SEG(this),this->prev_real);
	}

	/* fill in the values for the heapinfo structure */
	hi->in_use = next->size & 1L;
	hi->size = next->size & 0xFFFE;
	hi->ptr = next;
	rc = _HEAPOK;

end_walk:
	return(rc);
}


int heapfillfree(unsigned int fillvalue)
{
	struct heapinfo hi;
	char *area;
	int rc;
	int i;

	hi.ptr = NULL;
	
	if ((rc=heapwalk(&hi))!=_HEAPEMPTY) {
		do {
			if (hi.in_use == 0) {
				area = hi.ptr;
				for (i=0;i<(hi.size-8);i++) {
					area[i+8] = fillvalue;
				}
			}				
		}
		while ((rc=heapwalk(&hi))!=_HEAPEND);
		if (rc == _HEAPEND)
			rc = _HEAPOK;
		else
			rc = _HEAPCORRUPT;
	}

	
	
	return(rc);
}

int heapcheckfree(unsigned int fillvalue)
{
	struct heapinfo hi;
	char *area;
	int rc;
	int i;

	hi.ptr = NULL;
	
	if ((rc=heapwalk(&hi))!=_HEAPEMPTY) {
		do {
			if (hi.in_use == 0) {
				area = hi.ptr;
				for (i=0;i<(hi.size-8);i++) {
					if (area[i+8]!=fillvalue) {
						rc = _BADVALUE;
						goto exit_check;
					}
				}
			}
		}
		while ((rc=heapwalk(&hi))!=_HEAPEND);
		if (rc == _HEAPEND)
			rc = _HEAPOK;
		else
			rc = _HEAPCORRUPT;
	}

exit_check:
	
	return(rc);
}

/* 
** this is an experiment in controlled paranoia
** the pointer passed in may or may not be valid, so lets see if
** we can keep the program from dying in that case.....
*/
int heapchecknode(void *node)
{
	int rc;
	unsigned seg;
	unsigned off;
	
	seg = FP_SEG(node);
	off = FP_OFF(node);
	
	/* check the segment, verify that it is within the LDT and heap */
	/* the offset must be even */
	if ((seg&7)!=7 || seg<__base || seg>__top || (off&1)!=0) {
		rc = _BADNODE;
		goto exit_node;
	}

	/* all segments in the heap have 64K limits, so the pointer is safe */

	/* now, check to see if this is a valid node */
	/*** is the size+offset less than the limit ***/
	/*** does the prev_real point to another good block? ***/
	/*** if the block is free, are next_free and prev_free valid? ***/

exit_node:		
	return(rc);
}

#endif
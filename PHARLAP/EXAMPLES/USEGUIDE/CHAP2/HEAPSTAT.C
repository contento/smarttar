/* 
HEAPSTAT.C
Heap status routines for Borland C

See test program LIST.C
*/

#include <stdlib.h>
#include <alloc.h>

#include "heapstat.h"

#if 0		/* Borland heap check routines are not ported yet */
int (*walkf[2])(struct heapinfo *phi) =   { heapwalk, farheapwalk };
int (*statf[2])(void) =             { heapcheck, farheapcheck } ;
#endif

void heap_stats(HEAP h, HEAP_STATS *hs)
{
#if 0		/* Borland heap check routines are not ported yet */
    struct heapinfo hi;

    hs->heap = h;
    hs->used = hs->free = hs->blks = 0;
    hs->status = (*statf[h])();
    switch(hs->status)
    {
        case _HEAPEMPTY: hs->status_str = "empty"; break;
        case _HEAPOK: hs->status_str = "ok"; break;
	case _HEAPCORRUPT: hs->status_str = "corrupted"; break;
        default: hs->status_str = "???"; break;
    }
    if(hs->status != _HEAPOK)
        return;

    hi.ptr = NULL;
    /* walk the heap, calling walk function for each block */
    while ((*walkf[h])(&hi) == _HEAPOK)
    {
        if(hi.in_use)
            hs->used += hi.size;
  	else
            hs->free += hi.size;
        hs->blks++;
    }
#endif
}

void print_heap_stats(HEAP_STATS *hs)
{
#if 0		/* Borland heap check routines are not ported yet */
    printf("%s heap used=%lu free=%lu blks=%lu status=%s\n",
        (hs->heap == NEARHEAP) ? "near" : "far",
        hs->used,
        hs->free,
        hs->blks,
        hs->status_str);
#endif
}

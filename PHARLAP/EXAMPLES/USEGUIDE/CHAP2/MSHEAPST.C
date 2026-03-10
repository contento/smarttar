/* 
HEAPSTAT.C
Heap status routines for Microsoft C

See test program LIST.C
*/

#include <stdlib.h>
#include <malloc.h>

#include "heapstat.h"
extern int _cdecl printf(const char *, ...);

int (*walkf[2])(_HEAPINFO *phi) =   { _nheapwalk, _fheapwalk };
int (*statf[2])(void) =             { _nheapchk, _fheapchk } ;

static char *heap_status[] = { "???", "empty", "ok", "bad-begin", 
    "bad-node", "end", "bad-ptr" };

void heap_stats(HEAP h, HEAP_STATS *hs)
{
    _HEAPINFO hi;

    hs->heap = h;
    hs->used = hs->free = hs->blks = 0;
    hi._pentry = 0;
    /* walk the heap, calling walk function for each block */
    while ((*walkf[h])(&hi) == _HEAPOK)
    {
        switch (hi._useflag)
        {
            case _USEDENTRY: hs->used += hi._size; break;
            case _FREEENTRY: hs->free += hi._size; break;
        }
        hs->blks++;
    }
    hs->status = (*statf[h])();
    hs->status_str = heap_status[- hs->status];
}

void print_heap_stats(HEAP_STATS *hs)
{
    printf("%s heap used=%lu free=%lu blks=%lu status=%s\n",
        (hs->heap == NEARHEAP) ? "near" : "far",
        hs->used,
        hs->free,
        hs->blks,
        hs->status_str);
}

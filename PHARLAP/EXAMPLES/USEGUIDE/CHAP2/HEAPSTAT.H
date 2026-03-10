/*
HEAPSTAT.H
Heap status routines for Microsoft C
*/

#ifndef HEAPSTAT_H
#define HEAPSTAT_H
typedef enum { NEARHEAP, FARHEAP } HEAP;

typedef struct {
    HEAP heap;
    unsigned long free;
    unsigned long used;
    unsigned long blks;
    char *status_str;
    int status;
    } HEAP_STATS;
    
void heap_stats(HEAP h, HEAP_STATS *hs);
void print_heap_stats(HEAP_STATS *hs);
#endif


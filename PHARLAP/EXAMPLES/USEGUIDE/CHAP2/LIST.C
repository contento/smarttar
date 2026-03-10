/* 
LIST.C
usage: list [nodesize]
allocates and frees a lot of memory; uses linked list; 
    can specify node size

real mode:
    C:\>cl -AL list.c heapstat.c
    C:\>list

protected mode:
    C:\>cl -AL -Lp list.c heapstat.c
    C:\>run286 list
*/

#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <time.h>

#include "heapstat.h"

typedef struct node {
    unsigned long num;
    void *data;
    struct node *next;
    } NODE;
    
main(int argc, char *argv[])
{
    HEAP_STATS h;
    NODE *p, *q;
    time_t t1, t2;
    unsigned long nodes = 0;
    unsigned nodesize = (argc > 1) ? atoi(argv[1]) : 512;
    
    time(&t1);
    
    for (q=NULL; ; q->next = p)
    {
        p = q;
        if ((q = malloc(sizeof(NODE))) == NULL)
            break;
        if ((q->data = malloc(nodesize)) == NULL)
        {
            free(q);
            break;
        }
        q->num = nodes++;
        if ((nodes % 512) == 0)
            printf("%lu nodes: %lu seconds\n", 
                nodes, time(&t2) - t1);
    }
        
    printf("%lu nodes: %lu seconds\n", 
        nodes, time(&t2) - t1);
    printf("Allocated %uK\n", 
        (nodes * (sizeof(NODE)+nodesize)) >> 10);

    heap_stats(FARHEAP, &h);
    print_heap_stats(&h);
    
    for ( ; p != NULL; p = q)
    {
        q = p->next;
        if (p->num != --nodes)
            printf("list corrupt: nodes=%lu num=%lu\n", 
                nodes, p->num);
        free(p->data);
        free(p);
    }

    puts(nodes? "fail" : "ok");
    return (int)nodes;
}


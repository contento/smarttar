/*
TIMELOOP.C
cl -Lp -Ox timeloop.c

NOTE: This program now works under Windows 3 enhanced mode. 
*/

#include <stdio.h>
#include <process.h>
#include <phapi.h>

#ifndef FALSE
#define FALSE 0
#define TRUE 1
#endif

#define IRQ0          0x08
#define TICKS         20

#define TICKS_PER_SEC ((double) 18.2)

int timer_count = 0;
int flag = FALSE;

REALPTR old_real_timer_tick;
PIHANDLER old_prot_timer_tick;
void _interrupt _far new_prot_timer_tick(REGS16);

void fail(char *s) { puts(s); exit(1); }
    
void main(void)
{
    DESC desc;
    double duration;
    double per_iter;
    long int i = 0;
    unsigned short mycs, myds;

    /* VERY IMPORTANT -- Lock down our code and data segments
       because we don't want them paged out when running under
       Windows 3.0. */

    _asm	mov	mycs,cs
    _asm	mov	myds,ds
    DosLockSegPages(mycs);
    DosLockSegPages(myds);

    /* install our new timer tick routine */
    DosSetPassToProtVec(IRQ0, (PIHANDLER)new_prot_timer_tick,
        &old_prot_timer_tick, &old_real_timer_tick);
    
    /* wait for loop to start */
    do {;} while (! flag);
    
    /* timing loop */
    flag = FALSE;
    do
    {
        DosGetSegDesc(SELECTOROF(new_prot_timer_tick), (PDESC)&desc);
        i++;
    }
    while (! flag);
    
    duration = ((double) TICKS) / ((double) TICKS_PER_SEC);
    per_iter = 1.0e6 * (duration / ((double) i));
	/* corrected from 286dosx Developer's Guide, p. 193 */
    
    printf("We executed the call %ld times in %.02lf seconds.\n",
        i, duration);
    printf("The call took an average of %u microseconds.\n",
        (unsigned) per_iter);
    
    /* restore old timer tick routines */
    DosSetRealProtVec(IRQ0, old_prot_timer_tick, old_real_timer_tick,
        NULL, NULL);
}

void interrupt far new_prot_timer_tick()
{
    timer_count++;
    if (timer_count >= TICKS)
    {
        timer_count = 0;
        flag = TRUE;
    }
    
    /* chain to the old interrupt */
    DosChainToRealIntr(old_real_timer_tick);
    /*NOTREACHED*/
}


/*
 * IRQ4.C - COM1 interrupt handler.
 *
 * This module contains the interrupt service routine for COM1 interrupts.
 * It moves characters to/from the serial I/O chip to/from the transmit
 * and receive queues as appropriate.
 *
 * Compiler options:
 *  -Ox     To reduce inp and outp functions to inline instructions
 *  -G2     To enable 286 code generation
 *  -DDOSX286   To compile to run in protected mode
 *  -DNOQUEUES  To use external transmit and receive queues
 *  -DDLL       To compile as a DLL
 */ 

#include <stdlib.h>
#include <stdio.h>

#include <phapi.h>
#include "io.h"
#include "queue.h"

#pragma check_stack(off)    /* So we can run at interrupt time */

/*
 * If we are being compiled as a DLL, we will set up the symbol
 * _acrtused to prevent the C runtime library from getting yanked
 * in when we are linked.
 */
#ifdef DLL
int _acrtused;
#endif

/*
 * We will create the transmit and receive queues here unless the
 * symbol NOQUEUES is defined. In this case we will use the queues
 * which are defined in some other module.
 */

#ifndef NOQUEUES
QUEUE xq, rq;
#else
extern QUEUE far xq, far rq;
#endif

/*
 * Set up the name of the handler correctly depending on whether we
 * are going to run in real mode or protected mode.
 */
#ifdef DOSX286
void interrupt far prot_irq4_hnd(REGS16 r)
#else
void interrupt far real_irq4_hnd(REGS16 r)
#endif
{
    int id;
    int ch;

/* Read the interrupt ID port of the 8250 to determine the cause of the
   interrupt then take appropriate action.  We can get more than one
   interrupt condition simultaneously, so we will have to keep processing
   interrupts in a loop until there are no more to process */

    while(1)
    {
        id = inp(SIO_BASE + SIO_INTID);
        if(id & SIO_IPEND)
        {
            break;
        }
        switch(id & SIO_IDMSK)
        {
        case SIO_IDXMTE:
            if( !(inp(SIO_BASE+SIO_LSR) & SIO_LSXHRE ))
                break;  /* should never happen */

            ch = queue_extract(&xq);
            if(ch != -1)
                outp(SIO_BASE + SIO_XMITR, ch);

            break;

        case SIO_IDRCVD:
            ch = inp(SIO_BASE + SIO_RECVR);
            queue_insert(&rq, ch);
            break;

        case SIO_IDMSR:
            inp(SIO_BASE + SIO_MSR);  /* Read MSR to clear it */
            break;

        case SIO_IDRLS:
            inp(SIO_BASE + SIO_LSR);  /* Read LSR to clear it */
            break;
        }
    }

/* On at least one computer in the world this is required -- not sure why.  */

    if( queue_status(&xq)  && (inp(SIO_BASE+SIO_LSR) & SIO_LSXHRE) )
            outp(SIO_BASE + SIO_XMITR, queue_extract(&xq));

/* Ok, we are done processing the 8250 interrupt.  Issue an EOI to the 8259. */

    outp(PIC1_BASE + PIC_CMD, PIC_NSEOI);
    return;
}

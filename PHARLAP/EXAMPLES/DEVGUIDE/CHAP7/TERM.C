/*
 * TERM.C - Glass TTY terminal emulation package.
 *
 * This is a simple terminal emulator which uses interrupt driven I/O
 * to access the COM1 communications port.
 *
 * The emulator can in real mode or protected mode, and the protected
 * mode version can either handle interrupts in protected mode only
 * (using DosSetPassToProtVec) or in both real mode and protected mode
 * (using DosSetRealProtVec).  When running with interrupt handlers
 * for both modes, the interrupt service routines manipulate a single
 * set of queues which are in a real mode DLL.
 *
 * Turning on the optimizer turns the inp and outp function calls
 * into inline in and out instructions.
 *
 * We must disable stack probes for any functions which may be called
 * at interrupt time because it is possible that they will be entered
 * on a different stack than the application stack and therefore generate
 * spurious stack overflow errors.
 *
 * Any code which must run at interrupt time must also take care not to
 * take the address of any local variables (unless we are in large model)
 * because they will reside on the interrupt stack which is not in the
 * data segment.
 */

#include <stdlib.h>
#include <stdio.h>
#include <dos.h>
#include <bios.h>
#include <phapi.h>
#include <dos.h>

#include "io.h"
#include "queue.h"

#define FALSE 0
#define TRUE 1

#define KYB_F10 0x4400  /* F10 key = scan code 44, keyb char 0 */
#define KYB_F1  0x3B00  /* F1 key = scan code 3B, key char 0 */
#define IRQ4_VEC 0x0C   /* Interrupt vector for IRQ4 */

#define _KEYBRD_READ  0
#define _KEYBRD_READY 1

#define _COM_INIT 	0
#ifndef _MSC_VER
#define _COM_CHR8	0x03
#define _COM_STOP1	0x00
#define _COM_NOPARITY	0x00
#define _COM_9600	0xe0 
#endif


/* Global data */

#ifdef DOSX286
PIHANDLER old_irq4_prot;
REALPTR old_irq4_real;
#else
void (interrupt far *old_irq4)();
#endif

/*
 * The transmit and receive queues and interrupt service routine(s)
 * reside in IRQ4.C.
 */
extern QUEUE far xq, far rq;
void interrupt far prot_irq4_hnd(REGS16);
void interrupt far real_irq4_hnd(REGS16);
void comm_xmitstr();

void fail(char *s) { puts(s); putchar(7); exit(1); }

main()
{
    int exit_flag;
    int keychar;

    if(queue_init(&xq) || queue_init(&rq))
        fail("Can't initialize I/O queues\n");
    if(init_handlers())
        fail("Can't initialize interrupt handlers\n");

    printf("TERM - Phar Lap Terminal Emulator Demonstration Program\n");
    printf(" (Type F1 to send test string, F10 to exit)\n");

    exit_flag = FALSE;
    do
    {
/*
 * If there is a key available at the keyboard, we will read it and 
 *  take local action or send it out the serial port.  If there is a
 * character in the receive buffer, we will get it and call MS-DOS
 * to display it.
 */
        if(_bios_keybrd(_KEYBRD_READY))
        {
            keychar = _bios_keybrd(_KEYBRD_READ);
            if(keychar == KYB_F10)
                exit_flag = TRUE;
            else if(keychar == KYB_F1)
                comm_xmitstr("Phar Lap Terminal Emulator \
Demonstration Program\r\n");
            else
            {
                keychar &= 0xFF;
                if(keychar != 0)
                    comm_xmitch(keychar);
            }
        }
        while(comm_rcvsts())
            putch(comm_rcvch() & 0xFF);
    } while(!exit_flag);

    if(clean_handlers())
        fail("Can't clean-up interrupt handlers\n");
    return 0;
}

/* init_handlers - Install interrupt handlers for serial channel COM1 */

init_handlers()
{
    int mask;
#ifdef BOTHMODES
    REALPTR realhp;     /* Real mode ptr to real_irq4_hnd routine */
#endif

/* Set up COM1 to 9600 Baud, 8 bits, 1 stop bit, no parity */
    mask = _COM_9600 | _COM_CHR8 | _COM_STOP1 | _COM_NOPARITY;
    _bios_serialcom(_COM_INIT, 0, mask);

/*
 * Install the prot_irq4_hnd routine as the service routine for COM1
 * interrupts which occur in protected mode and the real_irq4_hnd routine
 * as the service routine for COM1 interrupts which occur in real mode.
 * We will first have to obtain the real mode address of the
 * real_irq4_hnd routine.
 */
#ifndef DOSX286
    _disable();
    old_irq4 = _dos_getvect(IRQ4_VEC);
    _dos_setvect(IRQ4_VEC, real_irq4_hnd);
#else
#ifdef BOTHMODES
    realhp = DosProtToReal((PVOID) real_irq4_hnd);
    if(realhp == 0)
        return -1;
    _disable();
    DosSetRealProtVec(IRQ4_VEC, (PIHANDLER)prot_irq4_hnd, realhp,
                &old_irq4_prot, &old_irq4_real);
#else
    _disable();
    DosSetPassToProtVec(IRQ4_VEC, (PIHANDLER)prot_irq4_hnd, &old_irq4_prot,
                &old_irq4_real);
#endif
#endif

/* Program COM1 to request interrupts for all events, bring up DTR, RTS
   and enable interrupts */
    outp(SIO_BASE + SIO_INTEN, SIO_INTSON);
    outp(SIO_BASE + SIO_MCR, SIO_MCRON);

/* Set the 8259 mask to allow IRQ4 interrupts */
    mask = inp(PIC1_BASE + PIC_MASK);
    mask &= ~(1 << 4);      /* Clear bit 4 to enable IRQ4 */
    outp(PIC1_BASE + PIC_MASK, mask);
    _enable();

    return 0;
}

/* clean_handlers - Clean up interrupt handlers for serial channel COM1 */

clean_handlers()
{
    int mask;

    _disable();
/* Shut down COM1 */
    outp(SIO_BASE + SIO_INTEN, SIO_INTSOFF);
    outp(SIO_BASE + SIO_MCR, SIO_MCROFF);

/* Mask off IRQ4 interrupts */
    mask = inp(PIC1_BASE + PIC_MASK);
    mask |= (1 << 4);       /* Set bit 4 to mask IRQ4 */
    outp(PIC1_BASE + PIC_MASK, mask);

/* Restore the old interrupt vector values */
#ifdef DOSX286
    DosSetRealProtVec(IRQ4_VEC, old_irq4_prot, old_irq4_real,
              NULL, NULL);
#else
    _dos_setvect(IRQ4_VEC, old_irq4);
#endif
    _enable();

    return 0;
}

/* comm_xmitstr - Write an EOS terminated string to the comm port */

void comm_xmitstr(strp)
char *strp;
{
    while(*strp != '\0')
        comm_xmitch(*strp++);
}

/* comm_xmitch - Transmit a character out the comm port, waiting if necessary */

comm_xmitch(ch)
int ch;
{
    int xmitch; /* character to transmit */

/* Add the character to the transmit queue.  If we inserted the
   character into an empty queue (length = 1) and the transmitter
   holding register is empty, then we will dequeue a character
   and give it to the serial chip. */

    while(queue_insert(&xq, ch) == -1)
        ;
    if((queue_status(&xq) == 1) &&
       (inp(SIO_BASE + SIO_LSR) & SIO_LSXHRE))
    {
        xmitch = queue_extract(&xq);
        outp(SIO_BASE + SIO_XMITR, xmitch);
    }
    return 0;
}

/* comm_rcvch - Receive a character from the comm port */

comm_rcvch()
{
    return queue_extract(&rq);
}

/* comm_rcvsts - Return # of chars available in comm port receive buffer */

comm_rcvsts()
{
    return queue_status(&rq);
}

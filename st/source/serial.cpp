//
// [ SERIAL.CPP ]
//

#include "stdst.h"

#include <stddef.h>
#include <serial.h>

// --- registers (10) del 8250 UART (7 ports)
#define TXR             0       /*  Transmit register (WRITE) */
#define RXR             0       /*  Receive register  (READ)  */
#define IER             1       /*  Interrupt Enable          */
#define IIR             2       /*  Interrupt ID              */
#define LCR             3       /*  Line control              */
#define MCR             4       /*  Modem control             */
#define LSR             5       /*  Line Status               */
#define MSR             6       /*  Modem Status              */
#define DLL             0       /*  Divisor Latch Low         */
#define DLH             1       /*  Divisor latch High        */
//
// --- Mask for each register
//
// Modem Output Control Register (MCR).
#define DTR             0x01
#define RTS             0x02
#define MC_INT		      0x08
// Interrupt Enable Register (IER).
#define RX_INT          0x01
// Interrupt Identification Register (IIR).
#define RX_ID           0x04
#define RX_MASK         0x07
//
// --- 8259 address  (Programmable Interrupt Controller (PIC))
//     and its masks
//
#define IMR   0x21   /* Interrupt Mask Register port */
#define ICR   0x20   /* Interrupt Control Port       */
#define EOI   0x20   /* End Of Interrupt */
#define IRQ3  0xF7   /* COM2 */
#define IRQ4  0xEF   /* COM1 */

//
// --- [ SERIAL ] ------------------------------------------------------------
//
void interrupt (far *SERIAL::OldSerialIV)(...) = NULL;
char *SERIAL::Buffer    = NULL;
WORD  SERIAL::BufLen    = 0;
WORD  SERIAL::BufStart  = 0;
WORD  SERIAL::BufEnd    = 0;
WORD  SERIAL::Port      = SERIAL::COM1;
WORD  SERIAL::PortBase  = 0;
WORD  SERIAL::Installed = FALSE;

SERIAL::SERIAL(WORD port, WORD speed, WORD parity, WORD bits, WORD stopBits, WORD bufLen)
{
    BufLen = bufLen;
    Port = port;
    Buffer = new char[BufLen];
    if (Set(port, speed, parity, bits, stopBits))
    {
        // --- install ISR
        OldSerialIV = getvect((port == COM1)?0x0C:0x0B);
        setvect((port == COM1)?0x0C:0x0B, NewSerialISR);
        // --- active port
        char byte;
        // enable UART interupts
        disable();
        byte = inportb(PortBase + MCR) | MC_INT;
        outportb(PortBase + MCR, byte);
        outportb(PortBase + IER, RX_INT);
        byte = inportb(IMR) & ((port == COM1)?IRQ4:IRQ3);
        outportb(IMR, byte);
        enable();
        // active modem
        byte = inportb(PortBase + MCR) | DTR | RTS;
        outportb(PortBase + MCR, byte);
        Installed = TRUE;
    }
}

SERIAL::~SERIAL()
{
    // disable interrupts
    char byte;
    disable();
    byte = inportb(IMR) | ~((Port == COM1)?IRQ4:IRQ3);
    outportb(IMR, byte);
    outportb(PortBase + IER, 0);
    byte = inportb(PortBase + MCR) & ~MC_INT;
    outportb(PortBase + MCR, byte);
    enable();
    // inactive modem
    outportb(PortBase + MCR, 0);
    // uninstall ISR
    setvect((Port == COM1)?0x0C:0x0B, OldSerialIV);
    delete [] Buffer;
}

BOOL SERIAL::Set(WORD port, WORD speed, WORD parity, WORD bits, WORD stopBits)
{
    // --- set port
    WORD offset;
    switch (port)
    {
    case COM1 :
        offset = 0x0000;
        break;
    case COM2 :
        offset = 0x0002;
        break;
    default   :
        return FALSE;
    }
    WORD far *RS232Addr = (WORD far *)MK_FP(0x0040, offset);
    if (*RS232Addr == NULL)
        return FALSE;
    PortBase = *RS232Addr;
    // --- set speed in bauds
    switch (speed)
    {
    case S300 :
        speed =   300;
        break;
    case S600 :
        speed =   600;
        break;
    case S1200:
        speed =  1200;
        break;
    case S2400:
        speed =  2400;
        break;
    case S4800:
        speed =  4800;
        break;
    case S9600:
        speed =  9600;
        break;
    case S19200:
        speed = 19200;
        break;
    default:
        return FALSE;
    }
    WORD divisor = (WORD) (115200L/speed);
    disable();
    char byte = inportb(PortBase + LCR);
    outportb(PortBase + LCR, (byte | 0x80));    // set DLAB
    outportb(PortBase + DLL, (divisor & 0x00FF));
    outportb(PortBase + DLH, ((divisor >> 8) & 0x00FF));
    outportb(PortBase + LCR, byte);             // reset DLAB
    enable();
    //
    // set other parameters
    //
    // Bits
    if (bits > S8)
        return FALSE;
    WORD otherSettings = bits+2;
    // --- set stop bits
    if (stopBits > S2)
        return FALSE;
    otherSettings |= ((stopBits == S1) ? 0x00 : 0x04);
    // --- set parity
    if (parity > EVEN)
        return FALSE;
    switch (parity)
    {
    case NONE:
        otherSettings |= 0x00;
        break;
    case ODD :
        otherSettings |= 0x08;
        break;
    case EVEN:
        otherSettings |= 0x18;
        break;
    }
    disable();
    outportb(PortBase+LCR, otherSettings);
    enable();
    return TRUE;
}

void interrupt SERIAL::NewSerialISR(...)
{
    disable();
    if ((inportb(PortBase+IIR)&RX_MASK)==RX_ID)
    {
        if (((BufEnd+1) & BufLen-1) == BufStart)
            ; // Overflow, ignore it !!!
        Buffer[BufEnd++] = inportb(PortBase+RXR);
        BufEnd &= BufLen - 1;
    }
    outportb(ICR, EOI);
    enable();
}

BOOL SERIAL::Put(char byte)
{
    DWORD timeOut = 0x0000FFFFL;
    outportb(PortBase+MCR, MC_INT|DTR|RTS);
    // wait for Clear To Send (CTS)
    while ((inportb(PortBase + MSR) & CTS) == 0)
        if (!(--timeOut))
            return FALSE;
    timeOut = 0x0000FFFFL;
    // wait for transmitter register empty
    while ((inportb(PortBase + LSR) & XMTRDY) == 0)
        if (!(--timeOut))
            return FALSE;
    disable();
    outportb(PortBase+TXR, byte);
    enable();
    return TRUE;
}

BOOL SERIAL::Put(const char *msg)
{
    BOOL ok = TRUE;
    int i = 0;
    while (msg[i])
    {
        ok &= Put(msg[i++]);
    }
    return ok;
}

// This routine returns the current value in the buffer
BOOL SERIAL::Get(char &byte)
{
    if (BufStart == BufEnd)
        return FALSE;
    byte = Buffer[BufStart];
    BufStart++;
    BufStart %= BufLen;
    return TRUE;
}

WORD SERIAL::GetStatus(void)
{
    return (inportb(PortBase + MSR) << 8) | inportb(PortBase + LSR);
}

/*
 * IO.H - Definitions for interface to 8250 serial chip and 8259
 *    interrupt controller.
 */

/* 8250 serial chip registers */

#define SIO_BASE    0x3F8   /* Base I/O address of COM1 */
#define SIO_XMITR   0x00    /* Offset to XMIT data register */
#define SIO_RECVR   0x00    /* Offset to RECV data register */
#define SIO_INTEN   0x01    /* Offset to Interrupt enable register */
#define SIO_INTID   0x02    /* Offset to Interrupt ID register */
#define SIO_LCR     0x03    /* Offset to Line control register */
#define SIO_MCR     0x04    /* Offset to Modem control register */
#define SIO_LSR     0x05    /* Offset to Line status register */
#define SIO_MSR     0x06    /* Offset to Modem status register */

/* Interrupt enable register bits */

#define SIO_ENMSK   0x0F    /* Mask for enable bits */
#define SIO_ENRCVI  0x01    /* Enable RCV char available ints */
#define SIO_ENXMITI 0x02    /* Enable XMIT holding register empty ints */
#define SIO_ENRLSI  0x04    /* Enable receive line status ints */
#define SIO_ENMSRI  0x08    /* Enable modem status register ints */

#define SIO_INTSON (SIO_ENXMITI | SIO_ENRCVI | SIO_ENRLSI | SIO_ENMSRI)
#define SIO_INTSOFF 0

/* Interrupt ID register bits */

#define SIO_IPEND   0x01    /* Interrupt is pending */
#define SIO_IDMSK   0x06    /* Mask for Interrupt ID bits */
#define SIO_IDMSR   0x00    /* Modem status register changed */
#define SIO_IDXMTE  0x02    /* Transmitter holding register empty */
#define SIO_IDRCVD  0x04    /* Received data is available */
#define SIO_IDRLS   0x06    /* Received line status register */

/* Modem control register bits */

#define SIO_MCRDTR  0x01    /* DTR signal */
#define SIO_MCRRTS  0x02    /* RTS signal */
#define SIO_MCRINT  0x08    /* Enable interrupts */
#define SIO_MCRLOOP 0x10    /* Enable internal loopback */

#define SIO_MCRON (SIO_MCRDTR | SIO_MCRRTS | SIO_MCRINT)
#define SIO_MCROFF 0

/* Line status register bits */

#define SIO_LSRCV   0x01    /* Char is available in the data register */
#define SIO_LSOVRUN 0x02    /* Receive overrun error */
#define SIO_LSPAR   0x04    /* Parity error detected */
#define SIO_LSFRAM  0x08    /* Framing error detected */
#define SIO_LSBRK   0x10    /* Break detected */
#define SIO_LSXHRE  0x20    /* Transmit holding register is empty */
#define SIO_LSXSHRE 0x40    /* Transmit shift and holding registers empty */

/* Definitions for 8259 PIC chip */

#define PIC1_BASE   0x20    /* I/O address of PIC master */
#define PIC_CMD     0x00    /* Offset of command register */
#define PIC_MASK    0x01    /* Offset of mask register */

#define PIC_NSEOI   0x20    /* Non-specific EOI command */

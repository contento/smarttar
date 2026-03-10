/* ISWIN3E.C - Test if running under Windows 3.0 enhanced mode */

#include <phapi.h>

BOOL iswin3e()
{
    REGS16 regs;        /* Registers for the Windows 2F call */
    UCHAR major, minor; /* Major and minor version numbers of Windows */

    memset(&regs, 0, sizeof(regs));
    regs.ax = 0x1600;
    DosRealIntr(0x2F, &regs, 0L, 0);        
    major = (regs.ax & 0xFF);
    minor = (regs.ax >> 8) & 0xFF;
    return (major > 1 && major != 0x80);
}
    

/* 
DPMITELL.C
cl -Lp -DDOSX286 dpmitell.c iswin3e.c
run286 dpmitell
*/

#include <stdlib.h>
#include <stdio.h>
#include <dos.h>

typedef int BOOL;
#ifndef FALSE
#define FALSE 0
#define TRUE  1
#endif
    
extern BOOL iswin3e();  /* see ISWIN3E.C -- is Win 3 Enhanced mode running? */

typedef unsigned long ULONG;

#pragma pack(1)

typedef struct
{
    ULONG dpmi_largest;         /* Largest available free block */
    ULONG dpmi_unlock_max;      /* Max. unlock page count */
    ULONG dpmi_lock_max;        /* Max. lock page count */
    ULONG dpmi_lin_size;        /* Size in bytes of linear address space */
    ULONG dpmi_unlock_count;    /* Number of unlocked pages */
    ULONG dpmi_fpage_count;     /* Number of free pages */
    ULONG dpmi_ppage_count;     /* Number of physical pages */
    ULONG dpmi_lin_free;        /* Free linear address page count */
    ULONG dpmi_swapf_size;      /* Size of swap file in pages */
    ULONG dpmi_rsrv[3];
} DPMI_FREEMEM; 

void dos_exit(unsigned char err)
{ 
    _asm mov al, err
    _asm mov ah, 04ch
    _asm int 21h
}

void fail(char *s) { puts(s); dos_exit(1); }

int dpmi_mode(void)
{
    _asm mov ax, 1686h
    _asm int 2fh
    _asm not ax
}

void dpmi_version(unsigned char *pmaj, unsigned char *pmin, 
                  unsigned *pflags, unsigned char *pproc, 
                  unsigned char *ppicm, unsigned char *ppics)
{
    _asm push di
    _asm mov ax, 0400h
    _asm int 31h
    _asm mov di, pmaj
    _asm mov [di], ah
    _asm mov di, pmin
    _asm mov [di], al
    _asm mov di, pflags
    _asm mov [di], bx
    _asm mov di, pproc
    _asm mov [di], cl
    _asm mov di, ppicm
    _asm mov [di], dh
    _asm mov di, ppics
    _asm mov [di], dl
    _asm pop di
}

void dpmi_getmeminfo(DPMI_FREEMEM far *pinfo)
{
    _asm push di
    _asm mov ax, word ptr pinfo+2
    _asm mov es, ax
    _asm mov di, word ptr pinfo
    _asm mov ax, 0500h
    _asm int 31h
    _asm pop di
}

#ifndef DOSX286
BOOL dpmi_init(void)
{
    void (far *dpmi)();
    unsigned hostdata_seg, hostdata_para, dpmi_flags;
    
    _asm {
        mov ax, 1687h           // test for DPMI presence
        int 2Fh
        and ax, ax
        jnz nodpmi              // if (AX == 0) DPMI is present
        mov dpmi_flags, bx
        mov hostdata_para, si   // paras for DPMI host private data
        mov dpmi, di
        mov dpmi+2, es          // DPMI protected-mode switch entry point
        jmp short gotdpmi
        }
nodpmi:
    return FALSE;
gotdpmi:
    if (_dos_allocmem(hostdata_para, &hostdata_seg) != 0)
        fail("can't allocate memory");
    
    /* enter protected mode */
    _asm mov ax, hostdata_seg
    _asm mov es, ax
    _asm mov ax, dpmi_flags
    (*dpmi)();
        
    return TRUE;
}
#endif

dpmi_info()
{
    static DPMI_FREEMEM info;
    unsigned char major, minor, mpic, spic, proc;
    unsigned flags, savsize;

    dpmi_version(&major, &minor, &flags, &proc, &mpic, &spic);
    printf("DPMI version number.............................%d.%d\n",
           major, minor);
    printf("DOS mode........................................%s\n",
           (flags & 0x2) ? "Real" : "Virtual 8086");
    printf("Virtual memory..................................%s\n",
           (flags & 0x4) ? "Yes" : "No");
    printf("Master PIC base interrupt.......................%02X\n",
           mpic);
    printf("Slave PIC base interrupt........................%02X\n",
           spic);
    printf("Processor.......................................80%d86\n",
           proc);

    dpmi_getmeminfo(&info);
    printf("Size of the linear address space................%ldK bytes\n",
           (info.dpmi_lin_size * 4096) / 1024);
    printf("Amount of free linear address space.............%ldK bytes\n",
           (info.dpmi_lin_free * 4096) / 1024);
    printf("Size of largest free linear block...............%ldK bytes\n",
           info.dpmi_largest / 1024);
    printf("Size of the virtual memory swap file............%ldK bytes\n",
           (info.dpmi_swapf_size * 4096) / 1024);
    printf("Total amount of physical memory.................%ldK bytes\n",
           (info.dpmi_ppage_count * 4096) / 1024);
    printf("Amount of free physical memory..................%ldK bytes\n",
           (info.dpmi_fpage_count * 4096) / 1024);
}

main()
{
#ifdef DOSX286
    if (! dpmi_mode())
        fail("This program requires DPMI");
    if (! iswin3e())
        fail("This program requires Windows 3 Enhanced mode");
#else   
    if (! dpmi_init())
        fail("This program requires DPMI");
#endif  

    dpmi_info();

#ifndef DOSX286
    dos_exit(0);
#endif
}


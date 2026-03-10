/*
GDT.C -- Map the Global Descriptor Table (GDT) into our address
space, using DosMapLinSeg(), and then display a map of descriptors
in the GDT. Use the Intel SGDT instruction to get the *linear*
(not physical) base address of the GDT.

cl -Lp -G2 gdt.c

bcc286 gdt.c

sample output running under DOS:
    Ring 3
    GDT base=0004B622 limit=00FF
    IDT base=0004ADEA limit=07FF
    LDT=0020
    TSS=0050
    GDT=0137:0000 selectors=32
    [0000] base=00000000 limit=0000 acc=00 INVALID  
    [0008] base=0003CC30 limit=FFFF acc=9B CODE  
    [0010] base=00048890 limit=FFFF acc=93 DATA  
    [0018] base=0004CF60 limit=FFFF acc=92 DATA  
    [0020] base=0004DB50 limit=0FFF acc=82 SYSTEM DATA  [LDT]
    [0028] base=0004DB50 limit=FFFF acc=93 DATA  
    [0030] base=0004ADEA limit=07FF acc=92 DATA  
    [0038] base=0004B622 limit=00FF acc=92 DATA  
    [0040] base=000B8000 limit=FFFF acc=F2 DATA DPL=03 
    [0048] base=00000000 limit=FFFF acc=92 DATA  
    [0050] base=0004B5F6 limit=002B acc=83 SYSTEM DATA  [TSS]
    [0058] base=0004B5F6 limit=002B acc=92 DATA  
    ...
        
sample output running under Windows 3.0 enhanced mode:
    Ring 1
    GDT base=8010011C limit=010F
    IDT base=80680000 limit=02FF
    LDT=0078
    TSS=0018
    GDT=01C5:0000 selectors=34 @ 8 bytes
    [0000] base=00000000 limit=0000 acc=00 INVALID 
    [0008] base=00036850 limit=FFFF acc=9B CODE 
    [0010] base=00036850 limit=FFFF acc=93 DATA 
    [0018] base=80010390 limit=2069 acc=8B SYSTEM CODE [TSS]
    [0020] base=8010011C limit=FFFF acc=92 DATA 
    [0028] base=00000000 limit=FFFF acc=9B CODE 
    [0030] base=00000000 limit=FFFF acc=93 DATA 
    [0038] base=80018240 limit=02FF acc=BB CODE 
    [0040] base=00000400 limit=02FF acc=B3 DATA 
    [0048] base=00000000 limit=FFFF acc=B0 DATA READ-ONLY 
    [0050] base=80482000 limit=0FFF acc=B3 DATA 
    [0058] base=00000522 limit=0100 acc=B2 DATA 
    [0060] base=804FB000 limit=1FFF acc=82 SYSTEM DATA 
    [0068] base=00000000 limit=FFFF acc=B3 DATA 
    [0070] base=80601000 limit=0FFF acc=B3 DATA 
    [0078] base=8067F000 limit=0FFF acc=82 SYSTEM DATA [LDT]
    ...
*/

#include <stdlib.h>
#include <stdio.h>
#include <dos.h>
#include <phapi.h>

#pragma pack(1)

typedef struct {
    unsigned limit;
    unsigned lo;
    unsigned hi;    
    } GDTR;
    
typedef GDTR IDTR;
    
typedef struct {
    unsigned char accessed   : 1;
    unsigned char read_write : 1;
    unsigned char conf_exp   : 1;
    unsigned char code       : 1;
    unsigned char xsystem    : 1;
    unsigned char dpl        : 2;
    unsigned char present    : 1;
    } ACCESS;
    
typedef struct {
    unsigned limit;
    unsigned addr_lo;
    unsigned char addr_hi;
    unsigned char access;
    unsigned char reserved;
    unsigned char addr_xhi;
    } DESCRIPTOR;   
    
void sgdt(GDTR far *pgdtr)
{
    _asm les bx, dword ptr pgdtr
    _asm sgdt es:[bx]
}

void sidt(IDTR far *pidtr)
{
    _asm les bx, dword ptr pidtr
    _asm sidt es:[bx]
}

unsigned sldt(void)
{
    _asm sldt ax
}

unsigned str(void)
{
    _asm str ax
}

void display(unsigned i, DESCRIPTOR far *d)
{
    static int did_init = 0;
    static unsigned my_ldt;
    static unsigned my_tss;
    unsigned char acc = *((unsigned char far *) &d->access);
    
    if (! did_init)
    {
        my_ldt = sldt() >> 3;
        my_tss = str() >> 3;
        did_init++;
    }
    
    printf("[%04X] base=%02X%02X%04X limit=%04X acc=%02X ",
        i << 3, d->addr_xhi, d->addr_hi, d->addr_lo, d->limit, acc);

    if (acc == 0)
        printf("INVALID ");
    else
    {
        ACCESS far *pa = (ACCESS far *) &d->access;
        if (pa->xsystem == 0)
            printf("SYSTEM ");
        printf("%s ", pa->code ? "CODE" : "DATA");
        if (! pa->read_write)
            printf("%s ", pa->code ? "EXEC-ONLY" : "READ-ONLY");
    }
    if (i == my_ldt)                printf("[LDT]");
    else if (i == my_tss)           printf("[TSS]");
    printf("\n");
}

void fail(char *s) { puts(s); exit(1); }

main(int argc, char *argv[])
{
    GDTR gdtr;
    IDTR idtr;
    void far *fp;
    DESCRIPTOR far *gdt;
    unsigned long base;
    unsigned long ibase;
    unsigned selectors;
    unsigned short sel;
    unsigned start=0, stop=0;
    unsigned i;
    
    if (argc > 1)
        sscanf(argv[1], "%04X", &start);
    if (argc > 2)
        sscanf(argv[2], "%04X", &stop);

    fp = main;
    printf("Ring %d\n", SELECTOROF(fp) & 3);
    
    sgdt(&gdtr);
    base = MAKEULONG(gdtr.lo, gdtr.hi); // linear address
    printf("GDT base=%08lX limit=%04X\n", base, gdtr.limit);
    
    sidt(&idtr);
    ibase = MAKEULONG(idtr.lo, (unsigned) idtr.hi); // linear address
    printf("IDT base=%08lX limit=%04X\n", ibase, idtr.limit);
    
    printf("LDT=%04X\n", sldt());
    printf("TSS=%04X\n", str());

    /*
        Somewhat buried in here, but here's the call to
        DosMapLinSeg()! base contains the linear address of
        the GDT, and gdtr.limit contains its limit (size-1).
    */
    if (DosMapLinSeg(base, gdtr.limit + 1, &sel) != 0)
        fail("can't map GDT seg");
    gdt = MAKEP(sel, 0);

    selectors = (gdtr.limit + 1) / sizeof(DESCRIPTOR);
    
    start >>= 3;
    stop >>= 3;
    
    if ((start > (selectors-1)) || (stop > (selectors-1)))
        fail("no such selector");
    
    if (start && (! stop))
        display(start, &gdt[start]);
    else if (stop)
        for (i=start; i<=stop; i++)
            display(i, &gdt[i]);
    else     
        for (i=0; i<selectors; i++)
            display(i, &gdt[i]);

    DosFreeSeg(sel);
    return 0;
}


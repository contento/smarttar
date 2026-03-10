/*
PDEV.C
walking DOS device chain from 286|DOS-Extender

protected-mode version:
    C:\>bcc286 pdev.c
    C:\>pdev
    NUL     
    $IPCUST 
    WD80030 
    XMSXXXX0
    CON     
    ...
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dos.h>

#include <phapi.h>

/* some device attribute bits */
#define CHAR_DEV        (1 << 15)
#define INT29           (1 << 4)
#define IS_CLOCK        (1 << 3)
#define IS_NUL          (1 << 2)

#pragma pack(1)

/* Structure of an MS-DOS device header */
typedef struct DeviceDriver {
    struct DeviceDriver far *next;
    unsigned attr;
    unsigned strategy;
    unsigned intr;
    union {
        BYTE name[8];
        BYTE blk_cnt;
        } u;
    } DeviceDriver;

/* Structure of MS-DOS List of Lists */
typedef struct {
    void far *dpb;
    void far *sft;
    DeviceDriver far *clock;
    DeviceDriver far *con;
    unsigned max_bytes;
    void far *disk_buff;
    void far *cds;
    void far *fcb;
    unsigned prot_fcb;
    unsigned char blk_dev;
    unsigned char lastdrv;
    DeviceDriver nul;   /* not a pointer */
    unsigned char join;
    // ...
    } ListOfLists;  // DOS 3.1+
        
void fail(char *s) { puts(s); exit(1); }

unsigned mapped = 0;

void far *map_real(void far *rptr, unsigned long size)
{
    SEL sel;
    if (DosMapRealSeg(FP_SEG(rptr), size + FP_OFF(rptr), &sel) != 0)
        fail("DosMapRealSeg fail");
    mapped++;
    return MAKEP(sel, FP_OFF(rptr));    
}

void free_mapped_seg(void far *fp)
{
    if (DosFreeSeg(FP_SEG(fp)) != 0)
        fail("DosFreeSeg fail");
    mapped--;
}

main()
{
    ListOfLists far *doslist;
    DeviceDriver far *dd;
    DeviceDriver far *next;
    REGS16 r;
    SEL sel;
    char buff[9];

/* call INT 21h Function 52h: Get List of Lists */
    memset(&r, 0, sizeof(r));
    r.ax = 0x5200;
    DosRealIntr(0x21, &r, 0L, 0);
    if (! (doslist = MAKEP(r.es, r.bx)))
        fail("INT 21h Function 52h not supported");
    doslist = map_real(doslist, sizeof(ListOfLists));

#define PARANOID	
#ifdef PARANOID
	/* The next three blocks of code are for error checking */

    /* NUL is part of DOSLIST, not a pointer, so don't need to map */
    if (_fmemcmp(doslist->nul.u.name, "NUL     ", 8) != 0)
        fail("NUL name wrong");
    if (! (doslist->nul.attr & IS_NUL))
        fail("NUL attr wrong");
    
    /* CON is pointer, so need to map */
    dd = map_real(doslist->con, sizeof(DeviceDriver));
    if (_fmemcmp(dd->u.name, "CON     ", 8) != 0)
        fail("CON name wrong");
    if (! (dd->attr & CHAR_DEV))
        fail("CON attr wrong");
    free_mapped_seg(dd);
    
    dd = map_real(doslist->clock, sizeof(DeviceDriver));
    if (_fmemcmp(dd->u.name, "CLOCK$  ", 8) != 0)
        fail("CLOCK$ name wrong");
    if (! (dd->attr & IS_CLOCK))
        fail("CLOCK$ attr wrong");
    free_mapped_seg(dd);
#endif	
    
    /* Walk the linked list, printing out device names */
    for (dd = &doslist->nul;;) 
    { 
        if (dd->attr & CHAR_DEV)
	{
	    memcpy(buff, dd->u.name, sizeof(buff) - 1);
	    buff[sizeof(buff) - 1] = 0;
            printf("%.8Fs\n", buff); 
        }
        else
            printf("Block dev: %u unit(s)\n", dd->u.blk_cnt);
        next = dd->next;            /* get next pointer */
        free_mapped_seg(dd);        /* THEN free rmode seg */
        if (FP_OFF(next) == 0xFFFF)     /* is there a next? */
            break;
        dd = map_real(next, sizeof(DeviceDriver));  /* map it */
    }

    if (mapped)
        printf("%u remaining mapped selectors!\n", mapped);
    
    return mapped;  /* 0 indicates success */
}

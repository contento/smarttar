/* 
R_API.C -- rmode stack-based API; 
can be called from pmode

bcc r_api.c

cl -Asnu -Gs r_api.c

both switches are crucial:
    -Asnu           SS != DS
    -Gs             turn off stack checking
        
optionally:
    bind286 r_api.exe ptestapi.exe
	ptestapi

*/

#include <stdlib.h>
#include <stdio.h>
#include <dos.h>
#include <string.h>

#pragma pack(1)

/* wouldn't work with pascal calling convention! */

#ifdef _MSC_VER
typedef struct {	 /* Microsoft */
    unsigned es,ds,di,si,bp,sp,bx,dx,cx,ax,ip,cs,flags;
    } REG_PARAMS;
#else 
typedef struct {	 /* Borland */
    unsigned short bp, di, si, ds, es, dx, cx, bx, ax, ip, cs, flags;
    } REG_PARAMS;
#endif
    
typedef struct {
    unsigned x, y, z;
    } PARAM_BLOCK;
    
unsigned far entry(unsigned handle, PARAM_BLOCK far *pb)
{
    return (pb->x * pb->y * pb->z);
}

unsigned (far *fp)(unsigned handle, PARAM_BLOCK far *pb) = entry;

void interrupt far api(REG_PARAMS r)
{
    unsigned ah = r.ax & 0xFF00;
    switch (ah)
    {
        case 0x00:
            r.cx = ('P' << 8) + 'H';
            r.dx = ('A' << 8) + 'R';
            r.es = FP_SEG(fp);
            r.bx = FP_OFF(fp);
            r.flags &= ~1;
            break;
        default:
            r.flags |= 1;
            break;
    }
}

void (interrupt far *old_63)(void);

void fail(char *s) { puts(s); exit(1); }

main(int argc, char *argv[])
{
#ifdef _MSC_VER
    old_63 = _dos_getvect(0x63);
    _dos_setvect(0x63, api);
#else
    old_63 = getvect(0x63);
    setvect(0x63, api);
#endif
    putenv("PROMPT=[R_API] $p$g"); 
    if (strstr(strupr(argv[0]), "PTESTAPI"))
    {
        // bound executable: R_API running as rmode stub to PTESTAPI
        system("run286 ptestapi");
    }
    else
    {
        system(getenv("COMSPEC"));
    }
#ifdef _MSC_VER
    _dos_setvect(0x63, old_63);
#else
    setvect(0x63, old_63);
#endif
    puts("bye");
    return 0;
}


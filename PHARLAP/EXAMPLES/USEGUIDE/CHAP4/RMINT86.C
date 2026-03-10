/* RMINT86.C */

#include <dos.h>
#include <phapi.h>

#define IN(reg)      (r.reg = in->x.reg)
#define OUT(reg)     (out->x.reg = r.reg)
#define SEGIN(reg)   (r.reg = sregs->reg)
#define SEGOUT(reg)  (sregs->reg = r.reg)

int rm_int86x(int intno, union REGS *in, union REGS *out, 
   struct SREGS *sregs)
{
   REGS16 r;
   SEGIN(cs); SEGIN(ds); SEGIN(es);
   IN(ax); IN(bx); IN(cx); IN(dx); IN(si); IN(di);
   DosRealIntr(intno, &r, 0, 0);
   OUT(ax); OUT(bx); OUT(cx); OUT(dx); OUT(si); OUT(di);
   SEGOUT(cs); SEGOUT(ds); SEGOUT(es);
   if ((out->x.cflag = (r.flags & 1)) != 0)
      _doserrno = r.ax;
   return r.ax;
}


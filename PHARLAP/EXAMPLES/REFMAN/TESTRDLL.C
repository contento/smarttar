/*
TESTRDLL.C -- protected-mode program calls code in real-mode DLL,
    using DosRealFarCall().

cl -Lp testrdll.c -link realdll.lib
run286 testrdll
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <phapi.h>
#include "realdll.h"

void PUT(char far *s, unsigned y);
unsigned GET_NUM(void);
char far *GET_STR(char far *s);
unsigned long TICKS(void);
void init_lowmem_buf();
void fini_lowmem_buf();

void fail(char *s) { puts(s); exit(1); }

main()
{
    char s[80];
    unsigned long i, t;
    unsigned sec;
    
    init_lowmem_buf();

    PUT("hello world", 1066);
    if (GET_NUM() != 1066)
        fail("Intermode call failure - num");
    if (_fstrcmp(GET_STR(s), "hello world") != 0)
        fail("Intermode call failure - str");

    /* measure number of intermode calls/second */
    printf("Timing intermode calls...");
    for (i=0, t=TICKS(); i<30000; i++)
        TICKS();
    sec = (TICKS() - t) / 18.2;
    printf("%u intermode calls/second\n", 30000 / sec);
    
    fini_lowmem_buf();
}

unsigned GET_NUM(void)
{
    REGS16 regs;
    memset(&regs, 0, sizeof(REGS16));   // clear to zero
        
    /*
        Note use of DosProtToReal() here: we have a protected-mode pointer
        to the real-mode function get_num(), so we must convert it into a
        real-mode pointer, using DosProtToReal(), before passing it to
        DosRealFarCall().  Whew!  (As shown in TICKS() below, another way
        to do this is to call DosGetRealProcAddr().)

        Also, note that get_num takes no parameters, so we pass in a word
        count of 0, and don't push anything onto the stack.

        But, get_num() returns an unsigned.  C return values generally
        appear in registers.  A 16-bit return value appears in AX, a 32-bit
        return value appears in DX:AX.  To get this return value, we must
        pass in a pointer to a REGS16 structure.
    */
    if (DosRealFarCall(DosProtToReal((PVOID) get_num), &regs, 
        0L, 0) != 0)
        fail("DosRealFarCall - failure");
    
    return regs.ax; /* needed REGS16 structure for get_num retval in AX */

}

static BYTE far *lowmem_buf = 0;
static BYTE far *realptr_lowmem_buf;

void init_lowmem_buf(void)
{
    USHORT sel, para;
    if (DosAllocRealSeg(1024, &para, &sel) != 0)
        fail("Insufficient conventional memory");
    lowmem_buf = MAKEP(sel, 0);
    realptr_lowmem_buf = MAKEP(para, 0);
}

void fini_lowmem_buf(void)
{
    DosFreeSeg(SELECTOROF(lowmem_buf));
}
    
char far *GET_STR(char far *s)
{
    /*
        In this example, get_str() doesn't have a return value, and
        it doesn't expect any register-based parameters, so we don't
        need a REGS16 structure.

        However, get_str() does expect a far pointer to a string,
        and these 4 bytes (2 words) must be pushed on the stack. Note
        that we can't just push s on the stack, because that's a 
        protected-mode pointer.  Nor can we simply convert s using
        something like DosProtToReal(s), because there's no guarantee
        that s is located in conventional memory, or that the pointer
        can be converted.  

        Thus, what we did was allocate a buffer in low memory (see
        above, init_lowmem_buf), using DosAllocRealSeg(). That function
        returns us both a real-mode paragraph address and a protected-
        mode selector. We manipulate the protected-mode selector
        outselves, and use the real-mode paragraph address to communicate
        with the real-mode DLL.  Whew!

        When get_str() returns (via our indirect call using
        DosRealFarCall()), we copy the string out of the low-memory
        buffer, back into s.  For efficiency, of course, we could
        arrange for our protected-mode program to actually use the
        low-memory buffer, and thus avoid copying, but what's shown
        here is the most general solution.
    */
    if (DosRealFarCall(DosProtToReal((PVOID) get_str), NULL, 0L, 
        2, realptr_lowmem_buf) != 0)
        fail("DosRealFarCall - failure");
    /* how to move data from conv mem back into s! */
    _fstrcpy(s, lowmem_buf);
    return s;
}

void PUT(char far *s, unsigned y)
{
    /*
        This time, we have to copy from s into conventional memory
        _before_ calling the real-mode code.
    */
    _fstrcpy(lowmem_buf, s);
    
    /*
        Here, note that the real-mode put() function expects
        two parameters: a char far * and an unsigned. It is
        the responsibility of the protected-mode caller to arrange
        parameters properly on the stack when using DosRealFarCall()
        or DosVRealFarCall(). The function put() uses the Pascal
        calling convention, so we push the far string (using the
        real-mode buffer, of course) _after_ we push the unsigned.
            Because put() uses Pascal calling convention, we also
        make the wordcount negative, so 286|DOS-Extender knows how
        to pop the args off the real-mode stack.
    */
#if 0
{
    /* an example of using a DosV() function */
    struct put_args { 
        unsigned y;
        char far *s;
        } v;
    // void put(char far *s, unsigned y)
    v.s = realptr_lowmem_buf;
    v.y = y;
    if (DosVRealFarCall(DosProtToReal((PVOID) put), NULL, 0L, -3, &v) != 0)
        fail("DosRealFarCall - failure");
}
#else
    if (DosRealFarCall(DosProtToReal((PVOID) put), NULL, 0L, 
        -3, y, realptr_lowmem_buf) != 0)
        fail("DosRealFarCall - failure");
#endif          
}

unsigned long TICKS(void)
{
    static REALPTR f = 0;
    
    REGS16 regs;
    memset(&regs, 0, sizeof(REGS16));   // clear to zero

    /*
        Finally, in this example, the call the real-mode function
        ticks() as an alternative to porting the code to protected
        mode. The one downside (as revealed by the timing loop
        in main(), above) is that these intermode calls are much
        slower than "normal" calls: we can only call ticks() from
        protected mode about 2000 times per second.

        The ticks() function doesn't expect any parameters. Its
        return value is an unsigned long, which on the PC is almost
        always returned in DX:AX. To get this return value, we need
        to pass in a REGS16 structure. We assemble the long out
        of r.dx and r.ax using the MAKELONG() macro.

        Having examined four different examples of DosRealFarCall(),
        you might have noticed that there is a simple-minded
        relationship between the parameters and return value of the
        original real-mode code on the one hand, and the way we
        constructed our protected-mode "stub" on the other. We try
        to make the whole process of calling real-mode code from
        a protected-mode program as transparent as possible, by
        making the protected-mode surrogates look as much like the
        real-mode originals. This is amazingly similar to Remote
        Procedure Code (RPC) in network programming.
    */
        
    if (! f) /* do just one time to cut down overhead per timed call */
    {
        f = DosProtToReal((PVOID) ticks);
        
        /* Another way to do this is to use DosGetRealProcAddr and 
           DosLoadModule. Since these functions manipulate ASCII
           strings, if you use these everywhere, then no import .LIB
           for the .DLL is needed */
        {
            USHORT realdll;
            if((DosGetModHandle("REALDLL", &realdll) &&
                DosLoadModule(0, 0, "REALDLL", &realdll)) ||
                DosGetRealProcAddr(realdll, "_ticks", &f))
                    fail("DosGetProcAddr - failure");
        }
    }
        
    if (DosRealFarCall(f, &regs, 0L, 0) != 0)
        fail("DosRealFarCall - failure");
    
    /* needed REGS16 structure for time retval in DX:AX */
    return MAKELONG(regs.ax, regs.dx);
}

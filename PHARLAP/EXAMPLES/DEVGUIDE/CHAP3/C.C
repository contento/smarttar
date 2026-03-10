/*
C.C

Microsoft C 6.0:
    cl -DDOSX286 -Lp -AL c.c -MAmx -MAp a.asm
or:
    masm -DDOSX286 -mx -p a.asm;
    cl -DDOSX286 -Lp -AL c.c a.obj

Microsoft C 5.1:
    masm -DDOSX286 -mx -p a.asm;
    cl -DDOSX286 -Lp -AL c.c a.obj

Borland C++
    tasm a /mx /p /DDOSX286;
    bcc286 c.c a.obj

*/

extern int init(void);
extern void subr(void), fini(void);

main()
{
    int i;
    if (! init())
        return 1;
    for (i=10000; i--; )    // simulate real work
        subr();
    fini();
    return 0;
}

/* 
ENUMPROC.C -- list all functions exported by a module
        
cl -Lp enumproc.c
run286 enumproc [DLL name]

sample output:
    run286 enumproc doscalls
    .......
    38                     DOSREALLOCSEG    0067:2026
    39                        DOSFREESEG    0067:2858
    40                      DOSALLOCHUGE    0067:20A6
    41                   DOSGETHUGESHIFT    0067:5E76
    42                    DOSREALLOCHUGE    016F:02D0
    43                  DOSCREATECSALIAS    0067:5162
    44                     DOSLOADMODULE    007F:04B2
    45                    DOSGETPROCADDR    007F:05A2
    46                     DOSFREEMODULE    007F:082A
    47                   DOSGETMODHANDLE    007F:04E6
    48                     DOSGETMODNAME    007F:052C
    49                 DOSGETMACHINEMODE    0067:5558
    50                           DOSBEEP    016F:157E
    .......
*/

#include <stdlib.h>
#include <stdio.h>
#include <phapi.h>

void fail(char *s) { puts(s); exit(1); }

main(int argc, char *argv[])
{
    char func[256];
    HMODULE h;
    unsigned ord;
    unsigned did_load = 0;
    
    if (argc < 2)
        fail("usage: enumproc [module name]");
    
    if (DosGetModHandle(argv[1], &h) != 0)
    {
        if (DosLoadModule(0, 0, argv[1], &h) != 0)
            fail("can't load module");
        did_load++;
    }

    ord = 0;
    while (DosEnumProc(h, func, &ord) == 0)
    {
        PFN fn;
        if (DosGetProcAddr(h, func, &fn) != 0)
            fail("DosGetProcAddr fail");
        printf("%u\t%32s\t%Fp\n", ord, func, fn);
    }
    
    if (did_load)
        DosFreeModule(h);
}

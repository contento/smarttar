/* 
EXECTEST.C
cl -Lp exectest.c
*/

#include <stdlib.h>
#include <process.h>

#ifndef CL_PATH
#define CL_PATH		"\\c600\\binp\\cl.exe"
#endif

main(int argc, char *argv[])
{
    int mode = P_WAIT;
    int i;
    
    for (i=1; i<argc; i++)
        if (argv[i][0] == '-')
            if (argv[i][1] == 'N' || argv[i][1] == 'n')
                mode = P_NOWAIT;
            
    /* use banner so we can see when DOS extender runs */
    putenv("RUN286=-banner");
    
    /* this is fine: doesn't load a second DOS extender */
    spawnlp(mode, CL_PATH, "cl", "-Lp", "exectest.c", NULL);

    /* this won't work under Windows: loads a second DOS extender */
    spawnlp(mode, "run286.exe", "run286", CL_PATH, "-Lp", "exectest.c", NULL);

#if 0
    /* why don't we just do this? */
    system(CL_PATH " -Lp exectest.c");
    /* because system() uses getenv("COMSPEC"), which is generally
       COMMAND.COM. Protected-mode programs cannot currently be run
       directly from the COMMAND.COM prompt without starting a
       second copy of the DOS extender */
#endif

    /* but this won't work under Windows either, because it loads 
       a second DOS extender */
    system("run286 " CL_PATH " -Lp exectest.c");
}


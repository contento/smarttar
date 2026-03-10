/* 
GORUN286.C
GORUN286: 1.0 -- Copyright (C) 1990 Phar Lap Software, Inc.
Real mode stub loader for Phar Lap's 286|DOS-Extender
See BIND286 for instructions

cl gorun286.c

NOTE: This is only a SAMPLE stub loader. This is not identical to
source code for the version of GORUN286.EXE that ships with
286|DOS-Extender version 1.3 and higher. In particular, it cannot
be configured with CFIG286.
*/

#include <stdlib.h>
#include <string.h>
#include <process.h>
#include <errno.h>
#include <dos.h>

#define STDOUT 1

char *run286_exe = "run286.exe";

char *banner = 
    "GORUN286: 1.0 -- Copyright (C) 1990 Phar Lap Software, Inc.\r\n" ;

main(int argc, char *argv[])
{
    char **argv2;
    int dummy;
    int i;

    // argv[0] should be name of protected-mode program, 
    // not of this stub loader
    if (strstr(strupr(argv[0]), "GORUN286"))
    {
        char *msg = "Real mode stub loader for Phar Lap's 286|DOS-Extender\r\nSee BIND286 for instructions\r\n";
        _dos_write(STDOUT, banner, strlen(banner), &dummy);
        _dos_write(STDOUT, msg, strlen(msg), &dummy);
        return 1;
    }
    
    if (! (argv2 = malloc((argc+2) * sizeof(char *))))
        return 1;
    argv2[0] = run286_exe;
    for (i=0; i<argc; i++)
        argv2[i+1] = argv[i];
    argv2[argc+1] = 0;
    execvp(run286_exe, argv2);
    /* if get here, error */
    if (errno == ENOENT)
    {
        char *msg = "This program requires Phar Lap's 286|DOS-Extender\r\n";
        _dos_write(STDOUT, msg, strlen(msg), &dummy);
    }
    else
    {
        char *msg = "Fatal Error 286.4000: Can't load RUN286 -- ";
        char *err = sys_errlist[errno];
        char *crlf = "\r\n";
        _dos_write(STDOUT, banner, strlen(banner), &dummy);
        _dos_write(STDOUT, msg, strlen(msg), &dummy);
        _dos_write(STDOUT, err, strlen(err), &dummy);
        _dos_write(STDOUT, crlf, 2, &dummy);
    }
    return 1;
}

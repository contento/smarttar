#include <iostream.h>
#include <stdlib.h>
#include <process.h>
#include <string.h>
#include <st_util.h>

int main(int , char *argv[])
{
    char password[9];
    strcpy(password, "\x4E\x74\x78\x12"); // see CFG.CPP
    _Decrypt(password, sizeof(password));
    char passwordCmd[20];
    strcpy(passwordCmd, "-s");
    strcat(passwordCmd, password);
    spawnlp(P_WAIT, "pkzip.exe", "", passwordCmd,
            argv[1], argv[2], argv[3], argv[4],
            argv[5], argv[6], argv[7], argv[8],
            NULL
           );
    return 0;
}
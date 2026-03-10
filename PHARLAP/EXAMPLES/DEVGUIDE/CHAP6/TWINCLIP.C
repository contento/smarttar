/*
TWINCLIP.C
Test WinClip -- non-Windows program reads Windows clipboard

SEE ALSO WINCLIP.C, PWINCLIP.C, WINCLIP.H

TWINCLIP.C -- test file; no change needed for 286|DOS-Extender
WINCLIP.H -- header file for Windows API for DOS apps
WINCLIP.C -- real mode implementation of API
PWINCLIP.C -- protected mode implementation of API

protected mode:
    C:\>bcc286 twinclip.c pwinclip.c
within Windows DOS box:
    C:\>twinclip
        
sample output:      
WINOLDAP v. 2.0
Resolution: 640 x 480 (16 colors)
Clipboard data: 1248 bytes
Clipboard contents:
    ... [contents of clipboard]
*/

#include <stdlib.h>
#include <stdio.h>

#include "winclip.h"

extern unsigned long GetClipboardSize();
extern char far *GetClipboardData();

void fail(char *s) { puts(s); exit(1); }

int main()
{
    unsigned long size;
    char far *buf;
    int maj, min;
    
    if (WinOldApVersion(&maj, &min))
        printf("WINOLDAP v. %d.%d\n", maj, min);
    else
        fail("This program requires WINOLDAP");

    printf("Resolution: %u x %u (%u colors)\n",
        GetDeviceCaps(HORZRES), GetDeviceCaps(VERTRES),
        GetDeviceCaps(NUMCOLORS));
    
    OpenClipboard();
    
    if (! (size = GetClipboardSize(CF_TEXT)))
        puts("No text in clipboard");
    else if ((buf = GetClipboardData(CF_TEXT)) != NULL)
    {
        printf("Clipboard data: %lu bytes\n", size);
        printf("Clipboard contents:\n%Fs\n", buf);  /* Far string */
        FreeClipboardData(buf);
    }

    CloseClipboard();

    return 0;
}


/*
FILE.C
Displays files, using several varieties of INT 21h calls 
    (_dos_xxx functions, intdosx, inline assembler)
Calling INT 21h Function 0Ah (Buffered Keyboard Input) allows
    command-line editing if a program such as CED is installed
This program requires Miscrosoft C 6, or higher, or Borland C++ 3.0,
    or higher.
        
real mode:
    C:\>cl file.c
    C:\>bcc file.c
    C:\>file
    File? ...
        
protected mode:
    C:\>cl -Lp file.c slibpe.lib protmode.def
    C:\>bcc286 file.c
    C:\>file
    File? ...
*/

#include <stdlib.h>
#include <dos.h>
#include <fcntl.h>
#include <share.h>
#include <string.h>

#pragma pack(1)

/* structure for INT 21h Function 0Ah (Buffered Keyboard Input) */
typedef struct {
    unsigned char max;
    unsigned char got;
             char buf[1];
    } BUFFER;
    
#define MAX_CHAR    80

#define STDERR      2

int display_file(char *s);
char far *dos_gets(BUFFER far *str);
void dos_putcrlf(void);

char cant_open[] = "Can't open file\r\n";

main(void)
{
    BUFFER *s;
    unsigned wcount;
    
    s = malloc(82);
    if (! s)
        return 1;
    
    for (;;)
    {
        /* prompt for filename */
        if (_dos_write(STDERR, "File? ", 6, &wcount))
            break;
        
        /* get filename */
        s->max = 80;
        if (! (dos_gets(s) && s->buf[0]))
            break;
        
        if (display_file(s->buf) != 0)
            _dos_write(STDERR, cant_open, strlen(cant_open), &wcount);
    }
    
    return 0;
}

/* call INT 21h Function 0Ah (Buffered Keyboard Input) */
char far *dos_gets(BUFFER far *string)
{
    _asm push ds
    _asm lds dx, string
    _asm mov ah, 0ah
    _asm int 21h
    _asm pop ds
    dos_putcrlf();
    string->buf[string->got] = '\0';
    return string->buf;
}

int display_file(char *s)
{
    unsigned rcount, wcount;
    int f;
    int ret;
    
    /* open file */
    if ((ret = _dos_open(s, O_RDWR | SH_DENYNO, &f)) != 0)
        return ret;
    
    /* display file */
    while ((_dos_read(f, s, 80, &rcount) == 0) && rcount)
        if (_dos_write(STDERR, s, rcount, &wcount) != 0)
            break;
        
    /* write one more CRLF */
    dos_putcrlf();
    
    /* close file */
    _dos_close(f);
    
    return 0;
}

/* Write line terminator with intdosx */
void dos_putcrlf(void)
{
    union REGS regs;
    struct SREGS sregs;
    /* "_far" so we can use FP_SEG */
    char _far *p = "\r\n";

    segread(&sregs);

    regs.h.ah = 0x40;    /* a.k.a _dos_write */
    regs.x.bx = STDERR;
    regs.x.cx = 2;
    regs.x.dx = FP_OFF(p);
    sregs.ds  = FP_SEG(p);

    intdosx(&regs, &regs, &sregs);
}

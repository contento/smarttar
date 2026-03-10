/*
WINCLIP.H
*/

/* predefined clipboard formats */
typedef enum { CF_TEXT=1, CF_BITMAP, CF_METAFILEPICT, CF_SYLK, 
    CF_DIF, CF_TIFF, CF_OEMTEXT } CF_FORMAT;

/* GetDeviceCaps index */
#define TECHNOLOGY    2
#define HORZRES       8     /*  Horizontal width in pixels        */
#define VERTRES       10    /*  Vertical   width in pixels        */
#define NUMCOLORS     24

int WinOldApVersion(int *maj, int *min);
unsigned GetDeviceCaps(unsigned cap);
void OpenClipboard(void);
unsigned long GetClipboardSize(CF_FORMAT format);
char far *GetClipboardData(CF_FORMAT format);
void CloseClipboard(void);
void FreeClipboardData(char far *buf);


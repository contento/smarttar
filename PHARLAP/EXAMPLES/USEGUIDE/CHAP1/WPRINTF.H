/* 
WPRINTF.H -- direct-screen-write printf()
*/

typedef enum { 
    BLUEFORE=1, GREENFORE=2, REDFORE=4, 
    LOW=8, 
    BLUEBACK=16, GREENBACK=32, REDBACK=64,
    BLINK=128 } ATTRIB;

#define NORMAL  (BLUEFORE | GREENFORE | REDFORE)
#define REVERSE (BLUEBACK | GREENBACK | REDBACK)

#define TOP             0
#define BOTTOM          24
#define LEFT            0
#define RIGHT           79

int video_mode(void);
unsigned get_vid_mem(void);
int video_init(void);
void wrt_str(int y, int x, ATTRIB attr, unsigned char *p);
void wprintf(int y, int x, ATTRIB attr, char *fmt, ...);
void clear(int starty, int startx, int endy, int endx, ATTRIB attr);

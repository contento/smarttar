/* $Id: graph286.h 1.2 92/06/16 10:17:49 rob Exp $ */

/* #define's below are for _graphicsgetbufstate() and _graphicssetbufstate() */
#define GSTATE_BUFFERED 0
#define GSTATE_UNBUFFERED 1

#ifdef __cplusplus
extern "C" {
#endif	

int _far _graphicsgetbufstate(void);
int _far _graphicssetbufstate(int new_state);

void _far _graphicsflushbuf(void);

#ifdef __cplusplus
}
#endif	

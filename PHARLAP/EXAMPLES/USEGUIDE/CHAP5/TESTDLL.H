/* TESTDLL.H */
#if 1
#define EXPORT      _far _loadds _export
#define P_EXPORT    EXPORT _pascal
#define C_EXPORT    EXPORT _cdecl

void P_EXPORT put(char far *s, unsigned y);
unsigned C_EXPORT get_num(void);
char far * C_EXPORT get_str(char far *s);
#else
void far pascal put(char far *s, unsigned y);
unsigned far get_num(void);
char far * far get_str(char far *s);
#endif


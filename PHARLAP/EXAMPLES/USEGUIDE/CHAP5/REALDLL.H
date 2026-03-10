/*
REALDLL.H -- services provided by real-mode DLL
*/

extern void far pascal put(char far *s, unsigned y);
extern unsigned far get_num(void);
extern void far get_str(char far *s);
extern unsigned long far ticks(void);


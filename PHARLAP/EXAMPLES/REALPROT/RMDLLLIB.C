#define  SUCCESS  0
#define  FAILURE  1

#include <stdio.h>   /* for definition of NULL */
#ifdef __BORLANDC__
#include <mem.h>
#else
#include <memory.h>
#endif
#include <phapi.h>

#define  MK_FP(seg, ofs)   ((void far *)\
                           (((unsigned long) (seg) << 16) | (unsigned) (ofs)))

/* step 1 */
static char c; // just need a little bit, you know.
               // It's needed to load up DS, take it out and you'll see what
               // happens.  If you have other statics, you may replaces this
               // with them.

extern   void  _far  *  _loadds  RM_malloc(size_t);
extern   void  _far  _loadds  RM_free(void *);

extern   int   Draw_Line(int, int, int, int),
               Fill_In_Params(char *, int *);

extern   char  _far  *foo(void);

extern   void  _far  *  _cdecl   _loadds  malloc(size_t);
extern   void  _far  _loadds  free(void far *);
extern   int   _loadds  test_malloc(void);

#ifdef MI_86
extern    int _aaltstkovr[];
#endif

int   _loadds  My_Draw_Line(int x1, int y1, int x2, int y2) {
   return(Draw_Line(x1, y1, x2, y2));
}  /* END OF My_Draw_Line() */

char  _far  * _loadds My_foo()   {
   return(foo());
}  /* END OF foo() */

int   _loadds  My_Fill_In_Params(char *param, int *dog_age)   {
   return(Fill_In_Params(param, dog_age));
}  /* END OF My_Fill_In_Params() */

void  _far  *  _loadds  malloc(size_t n)  {
REGS16   regs;
SHORT word_count = sizeof(size_t) / 2;
USHORT   result;

#ifdef MI_86
   if (_aaltstkovr[2] != 0)
      _aaltstkovr[2] = 0;
#endif

   memset(&regs, 0, sizeof(REGS16));

   result = DosProtFarCall((PFN)RM_malloc, &regs, 0, word_count, n);
   if (result != SUCCESS)  {
      puts("An error occured on the DosProtFarCall.\n");
      return(NULL);
   }  else  {
      return(MAKEP(regs.dx, regs.ax));
   }
}  /* END OF malloc() */

void  _far  _loadds  free(void far *p) {
REGS16   regs;
SHORT    word_count = sizeof(p) / 2;
USHORT   result;

#ifdef MI_86
   if (_aaltstkovr[2] != 0)
      _aaltstkovr[2] = 0;
#endif

   memset(&regs, 0, sizeof(REGS16));
   result = DosProtFarCall((PFN)RM_free, &regs, 0, word_count, p);
   if (result != SUCCESS)  {
      puts("An error occured on the DosProtFarCall.");
   }
}  /* END OF free() */

int   _loadds  test_malloc()  {
unsigned char  *p;

//
//   disable buffering on stdout so that real mode output shows up.
//
   setvbuf(stdout,NULL,_IONBF,0);

   /* allocate 1K */
   p = malloc(1024);
   /* we don't bother checking for NULL here, because where we do the
      DosAllocRealSeg (RM_malloc in pmlibdll.c) we check to see if
      it was successful, if not we exit out of the program (a nasty thing
      to do in most circumstances (I didn't clean myself up!))           */

   /* do something here with the memory if you like */

   /* free it */
   free(p);
   printf("Allocated and freed 1K\n");
   return(SUCCESS);
}  /* END OF test_malloc() */

#ifndef __BORLANDC__
void  main()   {
   /* why do I need this?! (cuz ms wants it) */
}  /* END OF dummy main() */
#endif

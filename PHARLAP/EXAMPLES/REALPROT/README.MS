Steps needed to use a real mode library in protected mode as a DLL for the
Microsoft Visual C/C++ compiler:

1) We have a library called real.lib which we want to use in a protected mode
   program.  For informational purposes only,  here is what the source for my
   library  looks like.  And here are the steps I took to create it, remember
   that you'll be getting this real mode library from a 3rd party.
   cl -c -AL -Gs real.c
   del real.lib
   lib real.lib +real.obj;


------------------------------------------------------------------------------
/* this is a real mode library */
#include <stdio.h>

extern  int Draw_Line(int, int, int, int),
            Fill_In_Params(char *, int *);

extern   char  *foo(void);

int   real_variable = 123;

int   Draw_Line(int x1, int y1, int x2, int y2) {
   /* do something here */
   return(1);
}  /* END OF Draw_Line() */

char  *foo()   {
   return("This is a test");
}  /* END OF foo() */

int   Fill_In_Params(char *param, int *dog_age)   {
   param[0] = 'D';
   param[1] = 'C';
   param[2] = 'M';
   param[3] = '\0';
   *dog_age = 11;
   return(1);
}  /* END OF Fill_In_Params() */
------------------------------------------------------------------------------
   I show you the source so you'll see what the functions  do in the library.
   IT IS FOR INFORMATIONAL PURPOSES ONLY.  IT IS NOT PART OF THE NORMAL STEPS
   TO USE A  REAL  MODE  LIBRARY  WITH A  PROTECTED  MODE  PROGRAM.  YOU CAN,
   HOWEVER, USE THIS AS A WAY TO TEST OUT, AND GET A FEEL FOR THE PROCESS.



1.1) Now let us create a file called RMDLLLIB.C

------------------------------------------------------------------------------
#define  SUCCESS  0
#define  FAILURE  1

#include <stdio.h>   /* for definition of NULL */
#include <memory.h>
#include "phapi.h"

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


extern    int _aaltstkovr[];

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

   if (_aaltstkovr[2] != 0)
      _aaltstkovr[2] = 0;

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

   if (_aaltstkovr[2] != 0)
      _aaltstkovr[2] = 0;

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

void  main()   {
   /* why do I need this?! (cuz ms wants it) */
}  /* END OF dummy main() */
------------------------------------------------------------------------------

1.2) Compile it using cl -ALu -FPa -Gs2 -Oi -c rmdlllib.c
1.3) Note: Please see your manual regarding mixing real and protected mode code


2) We will now  link /nod rmdlllib.obj,rmdlllib.dll,,real.lib+llibca.lib,
   rmdlllib.def in order to create  rmdlllib.dll.  real.lib, as you probably
   remember, is our original real mode library, whose goal it is of ours to
   use. Here below is rmdlllib.def

------------------------------------------------------------------------------
LIBRARY rmdlllib
EXETYPE  WINDOWS
DESCRIPTION 'SAMPLE REAL MODE DLL'
EXPORTS
   _My_Draw_Line
   _My_foo
   _My_Fill_In_Params
   _real_variable
   _test_malloc
IMPORTS
   _RM_malloc=mainapp._RM_malloc
   _RM_free=mainapp._RM_free

------------------------------------------------------------------------------

2.1) Now do a markreal rmdlllib.dll, which will mark the dll created as being
     the correct exetype for our purposes.

2.2) Note: Please see your manual regarding mixing real and protected mode code


3) Let us now create rmdlllib.lib for our use later by doing a
   implib rmdlllib.lib rmdlllib.def


4) Now we are going to get pretty dirty here.  I'll create a  file called
   pmlibdll.c which will do the actual interfacing by using some Phar Lap
   calls. We will compile it by doing a cl -c -AL -Gs pmlibdll.c
------------------------------------------------------------------------------
#define  SUCCESS  0
#define  FAILURE  1

#include <stdio.h>   /* for definition of NULL */
#include <stdlib.h>  /* for exit */
#include <dos.h>
#include <memory.h>
#include "phapi.h"   /* or <phapi.h> */

struct   hp {
   USHORT   para,
            sel;
   void  *next;
};

typedef  struct   hp heap;

extern   int   _loadds  test_malloc(void);

extern   int   init_malloc(PFN, PFN);

static   heap  *first;

extern   void  _far  *  _loadds  RM_malloc(size_t);
extern   void  _far  _loadds  RM_free(void *);



extern   int   _far  My_Draw_Line(int, int, int, int),
                     My_Fill_In_Params(char far *, int far *);

extern   char  _far  *My_foo(void);

extern   void  init_lowmem_buf(void);

REGS16   regs;

static   BYTE  far   *lowmem_buf = 0;
static   BYTE  far   *realptr_lowmem_buf;

char  _far  *foo()   {
char  _far  *result;
   if (DosRealFarCall(DosProtToReal((PVOID)My_foo), &regs, 0, 0)) {
      return("DosRealFarCall failed");
   }
   if (!regs.dx)  {
      return("NULL");
   }
   result = DosRealToProt((REALPTR)MAKEP(regs.dx, regs.ax));
   if (result) {
      return(result);
   }
   if (DosMapRealSeg(regs.dx, 0x10000, &regs.dx))  {
      return("DosMapRealSeg failed");
   }
   return(MAKEP(regs.dx, regs.ax));
}  /* END OF foo() */

int   Draw_Line(int x1, int y1, int x2, int y2) {
   DosRealFarCall(DosProtToReal((PVOID)My_Draw_Line), &regs, 0, 4, x1, y1, x2, y2);
   return(regs.ax);
}  /* END OF Draw_Line() */

int   Fill_In_Params(char far *param, int far *dog_age)  {
   DosRealFarCall(DosProtToReal((PVOID)My_Fill_In_Params), &regs, 0, 4,
      &realptr_lowmem_buf[0], &realptr_lowmem_buf[4]);
   /* you may want to pass the size of memory to be copied, but I'll simply
      hardwire it in for simplicity's (?) sake.                              */
   _fmemcpy(param, &lowmem_buf[0], 4); /* param is only 4 chars long */
   _fmemcpy(dog_age, &lowmem_buf[4], sizeof(int));
   return(regs.ax);
}  /* END OF Fill_In_Params() */

int   lets_test_malloc()   {
   DosRealFarCall(DosProtToReal((PVOID)test_malloc), &regs, 0, 0);
}  /* END OF test_malloc() */

void  init_lowmem_buf() {
USHORT   sel,
         para;
   /* allocate 1K of conventional (real) memory */
   if (DosAllocRealSeg(1024, &para, &sel) != SUCCESS) {
      printf("Insufficient conventional memory\n");
      exit(FAILURE);
   }
   lowmem_buf = MAKEP(sel, 0);
   realptr_lowmem_buf = MAKEP(para, 0);
}  /* END OF init_lowmem_buf() */

void  _far  *  _loadds  RM_malloc(size_t n)  {
heap  *h,
      *i;
USHORT   para,
         sel,
         ret;

   ret = DosAllocRealSeg((ULONG)n, &para, &sel);
   if (ret != SUCCESS)  {
      printf("DosAllocRealSeg failure code=%d\n", ret);
      exit(4);
   }
   if (first == NULL)   {
      first = malloc(sizeof(*first));
      first->next = NULL;
      first->para = para;
      first->sel = sel;
   }  else  {
      h = first;
      while (h->next != NULL) {
         h = h->next;
      }
      h->next = malloc(sizeof(*h));
      if (h->next == NULL) {
         printf("Fatal Error:  Unable to allocate heap control block.\nExiting!\n");
         exit(3);
      }  else  {
         h = h->next;
         h->para = para;
         h->sel = sel;
         h->next = NULL;
      }
   }
   return(MAKEP(para, 0));
}  /* END OF RM_malloc() */

void  _far  _loadds  RM_free(void *p)  {
heap  *h,
      *i;
USHORT   para,
         ret;

   if (p == NULL) {
      printf("Fatal Error:  Attempting to free NULL pointer.\n");
      exit(5);
   }

   para = FP_SEG(p);
   h = first;
   i = NULL;
   while ((h->para != para) && (h->next != NULL))  {
      i = h;
      h = h->next;
   }

   if (h->para != para) {
      printf("Fatal Error:  Bad REAL mode pointer.\nExiting!");
      exit(4);
   }

   ret = DosFreeSeg(h->sel);
   if (ret != 0)  {
      printf("Non-fatal Error:  Unable to free segment.\n");
   }

   if (i == NULL) {
      first = h->next;
   }  else  {
      i->next = h->next;
   }
   free(h);
}  /* END OF RM_free() */
------------------------------------------------------------------------------


5) Two more steps, we're almost there.  This is our main application, here is
   how I chose to compile it cl -c -AL -Gs mainapp.c

------------------------------------------------------------------------------
#define  SUCCESS  0

#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
#include "phapi.h"

extern   void  Init_It(void _far *);

extern   int   Draw_Line(int, int, int, int),
               Fill_In_Params(char *, int *);

extern   char  *foo(void);

extern   void  init_lowmem_buf(void);

extern   int   _far  real_variable; /* variable from real mode */

extern   int   lets_test_malloc(void);

char  first_param[4];
int   second_param;

REGS16   regs;

/* back door to fully shut off stack checking in Microsoft */
extern    int _aaltstkovr[];

void  main()   {
int   ret_code;

   setvbuf(stdout, NULL, _IONBF, 0);   /* flush buffer quicker! */

   /* kill stack checking, backdoor */
   if (_aaltstkovr[2] != 0)
      _aaltstkovr[2] = 0;

   init_lowmem_buf();   /* if it fails, it exits the pgm */

   printf("Draw_Line(1, 2, 3, 4)=%d\n", Draw_Line(1, 2, 3, 4));

   printf("foo returned '%s'\n", foo());

   printf("Calling Fill_In_Params(0x%X)\n", first_param);
   ret_code = Fill_In_Params(&first_param[0], &second_param);
   printf("ret_code=%d first_param = '%s' second_param=%d\n", ret_code, first_param, second_param);

   printf("real_variable=%d\n", real_variable);

   /* tests a real mode DLL calling back to protected mode code */
   lets_test_malloc();

   printf("Done.\n");
}  /* END OF main() */

------------------------------------------------------------------------------

5.1) One more thing must be done before we go further, we must create
     mainapp.def, this will enable us to do a malloc call from real->prot
------------------------------------------------------------------------------
NAME mainapp
DESCRIPTION 'Test program for calling back to PROT mode from real mode'
STACKSIZE 4096
STUB 'gorun286.exe'

EXPORTS
   _RM_malloc
   _RM_free
------------------------------------------------------------------------------

6) This is the final step.  We must link it all together and then
   test it by doing a run286 mainapp.

   link mainapp.obj pmlibdll.obj/co/noe,mainapp.exe,,rmdlllib.lib 
        phapi.lib llibcep.lib/nod,mainapp.def;

   Below is the output from run286 mainapp
------------------------------------------------------------------------------
Draw_Line(1, 2, 3, 4)=1
foo returned 'This is a test'
Calling Fill_In_Params(0x21A)
ret_code=1 first_param = 'DCM' second_param=11
real_variable=123
Allocated and freed 1K
Done.
------------------------------------------------------------------------------

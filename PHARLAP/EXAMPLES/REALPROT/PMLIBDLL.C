#define  SUCCESS  0
#define  FAILURE  1

#include <stdio.h>   /* for definition of NULL */
#include <stdlib.h>  /* for exit */
#include <dos.h>
#ifdef   __BORLANDC__
 #include   <mem.h>
#else
 #include   <memory.h>
#endif
#include <phapi.h>

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
   DosRealFarCall(DosProtToReal((PVOID)My_Draw_Line), &regs, 0, 4, x1,
		  y1, x2, y2);
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

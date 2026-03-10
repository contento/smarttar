#define  SUCCESS  0

#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
#include <phapi.h>

extern   void  Init_It(void _far *);

extern   int   Draw_Line(int, int, int, int),
               Fill_In_Params(char *, int *);

extern   char  *foo(void);

extern   void  init_lowmem_buf(void);

extern   int   _far  real_variable; /* variable from real mode */

extern   int   lets_test_malloc(void);

char  first_param[4];
int   second_param;

static REGS16   regs;

#ifdef MI_86_
/* back door to fully shut off stack checking in Microsoft */
extern    int _aaltstkovr[];
#endif

void  main(void)
{
   int   ret_code;

   setvbuf(stdout, NULL, _IONBF, 0);   /* flush buffer quicker! */

#ifdef MI_86
   /* kill stack checking, backdoor */
   if (_aaltstkovr[2] != 0)
      _aaltstkovr[2] = 0;
#endif

   init_lowmem_buf();   /* if it fails, it exits the pgm */

   printf("Draw_Line(1, 2, 3, 4)=%d\n", Draw_Line(1, 2, 3, 4));

   printf("foo returned '%s'\n", foo());

   printf("Calling Fill_In_Params(0x%X)\n", first_param);
   ret_code = Fill_In_Params(&first_param[0], &second_param);
   printf("ret_code=%d first_param = '%s' second_param=%d\n", ret_code,
	  first_param, second_param);

   printf("real_variable=%d\n", real_variable);

   /* tests a real mode DLL calling back to protected mode code */
   lets_test_malloc();

   printf("Done.\n");
}  /* END OF main() */


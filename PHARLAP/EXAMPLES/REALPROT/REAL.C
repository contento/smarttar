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

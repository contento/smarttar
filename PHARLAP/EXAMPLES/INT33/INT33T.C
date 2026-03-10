/* INT33T.C - Mouse INT 33H test program */

/************************************************************************/
/*	Copyright (C) 1986-1990 Phar Lap Software, Inc.			*/
/*	Unpublished - rights reserved under the Copyright Laws of the	*/
/*	United States.  Use, duplication, or disclosure by the 		*/
/*	Government is subject to restrictions as set forth in 		*/
/*	subparagraph (c)(1)(ii) of the Rights in Technical Data and 	*/
/*	Computer Software clause at 252.227-7013.			*/
/*	Phar Lap Software, Inc., 60 Aberdeen Ave., Cambridge, MA 02138	*/
/************************************************************************/

#include <stdio.h>
#include <phapi.h>

#define FOREVER while(1)

int _near x;
int _near y;
int _near but;


/*

Main routine

*/

main()

{
	extern void _far _loadds _saveregs event_hand();
	static void (*handp)() = event_hand;

	int rc, butc, px, py;

	/* Reset the mouse driver and exit it there is no mouse. */

	_asm
	{
		mov	ax,0
		int	33h
		mov	rc,ax
		mov	butc,bx
	}
	if(rc == 0)
	{
		printf("Mouse support is not available.\n");
		return 1; 
	}
	printf("Mouse support is available.\n");
	printf("Number of mouse buttons = <<%d>>.\n", butc);
	printf("\n");

	/* Turn on the mouse pointer on the screen */

	_asm
	{
		mov	ax,1h
		int	33h
	}

	/* Register a mouse event handler */

	_asm
	{
		mov	ax,0ch
		mov	cx,7Fh
		les	dx,handp
		int	33h
	}

	/* Display the mouse position until one of the mouse buttons
	   has been pressed */

	px = -1;
	py = -1;
	FOREVER
	{
		if(but & 0x7)
			break;
		if(px == x && py == y)
			continue;
		px = x;
		py = y;
		printf("Mouse = <<%d,%d>>     \r", x, y);
	}
	printf("\n");
	if(but & 0x1)
		printf("Left button is pressed.\n");
	if(but & 0x2)
		printf("Center button is pressed.\n");
	if(but & 0x4)
		printf("Right button is pressed.\n");

	/* Turn off the mouse pointer on the screen */

	_asm
	{
		mov	ax,2h
		int	33h
	}

	/* Reset the mouse driver */

	_asm
	{
		mov	ax,0
		int	33h
	}

	/* All done. */

	printf("All done.\n");
	return 0;

}


/*

Mouse event handler

*/

void _far _loadds _saveregs event_hand()

{

	_asm
	{
		mov	but,bx
		mov	x,cx
		mov	y,dx
	}

	return;

}

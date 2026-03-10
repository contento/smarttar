//	PERSIST4.CPP (PERSIST) - Abstract persistent graphic objects.
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA
//  May be freely copied, used and distributed.

#include <conio.h>
#include <stdio.h>
#include "draw.h"
#include "persist.hpp"

main()
{
	// Initialize the screen.
	InitializeDisplay();

	// Initialize the graphics objects.
	GRAPHIC_OBJECT *object[3];
	object[0] = new CIRCLE(100, 150, 50);
	object[1] = new RECTANGLE(200, 100, 300, 200);
	object[2] = new TRIANGLE(400, 100, 350, 200, 450, 200);

	// Draw the objects.
	for (int i = 0; i < 3; i++)
	{
		object[i]->Draw();
		delete object[i];
	}

	// Get user input then restore the screen.
	getch();
	RestoreDisplay();
	return (0);
}

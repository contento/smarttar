//	PERSIST6.CPP (PERSIST) - Basic read from disk.
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA
//  May be freely copied, used and distributed.

#include <conio.h>
#include <stdio.h>
#include "draw.h"
#include "persist.hpp"

main()
{
	// Set up the graphics screen display.
	InitializeDisplay();

	// Load the graphics objects.
	FILE *file = fopen("persist.dat", "rb");
	if (file)
	{
		GRAPHIC_OBJECT *object[3];
 		object[0] = new CIRCLE(file);
		object[1] = new RECTANGLE(file);
		object[2] = new TRIANGLE(file);
		fclose(file);

		// Draw the objects.
		for (int i = 0; i < 3; i++)
		{
			object[i]->Draw();
			delete object[i];
		}

		// Get user input and restore the screen.
		getch();
		RestoreDisplay();
	}
	else
	{
		// Restore the screen and print an error.
		RestoreDisplay();
		printf("ERROR: The \"persist.dat\" file could not be found.\n");
	}

	return (0);
}

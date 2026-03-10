//	PERSIST5.CPP (PERSIST) - File generator for persistent graphic objects.
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA
//  May be freely copied, used and distributed.

#include <io.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <conio.h>
#include <stdio.h>
#include "draw.h"
#include "persist.hpp"

main()
{
	// Initialize the graphics objects.
	GRAPHIC_OBJECT *object[3];
	object[0] = new CIRCLE(100, 150, 50);
	object[1] = new RECTANGLE(200, 100, 300, 200);
	object[2] = new TRIANGLE(400, 100, 350, 200, 450, 200);

	// Store the objects.
	FILE *file = fopen("persist.dat", "wb");
//	printf("Generating GRAPHICS.DAT ");										// BUG.1670
	printf("Generating 'persist.dat' ");
	for (int i = 0; i < 3; i++)
	{
		printf("*");
		object[i]->Store(file);
		delete object[i];
	}
	printf(" Done!\n");
	fclose(file);
 
	return (0);
}

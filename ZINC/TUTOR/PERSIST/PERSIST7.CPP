//	PERSIST7.CPP (PERSIST) - Abstract read from disk.
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA
//  May be freely copied, used and distributed.

#include <conio.h>
#include <stdio.h>
#include "draw.h"
#include "persist.hpp"

#if (__BORLANDC__ >= 0x0300) || defined(_MSC_VER) || (__ZTC__ >= 0x0310) || defined(__WATCOMC__)
GRAPHIC_OBJECT::JUMP_ELEMENT GRAPHIC_OBJECT::_jumpTable[] =
#else
JUMP_ELEMENT GRAPHIC_OBJECT::_jumpTable[] =
#endif 
{
	{ ID_CIRCLE, 	CIRCLE::New },
	{ ID_RECTANGLE,	RECTANGLE::New },
	{ ID_TRIANGLE,	TRIANGLE::New },
	{ 0, NULL }		// end of array.
};

GRAPHIC_OBJECT *GRAPHIC_OBJECT::New(FILE *file)
{
	// Read the type from disk.
	short type;
	int fileObject = fread(&type, sizeof(type), 1, file);
	if (!fileObject)
		return (NULL);

	// Get the New pointer and create the object.
	for (int i = 0; _jumpTable[i].newFunction; i++)
		if (type == _jumpTable[i].type)
			return (_jumpTable[i].newFunction(file, L_SKIP_TYPE));
	return (NULL);
}

main()
{
	// Set up the graphics screen display.
	InitializeDisplay();

	FILE *file = fopen("persist.dat", "rb");
	if (file)
	{
		int fileObjects = 1;
		do
		{
			GRAPHIC_OBJECT *object = GRAPHIC_OBJECT::New(file);
			if (object)
			{
				object->Draw();
				delete object;
			}
			else
				fileObjects = 0;
		} while (fileObjects);
		fclose(file);

		// Get user input then restore the screen.
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

//	PERSIST2.C (PERSIST) - Persistent graphic objects using structures.
//	COPYRIGHT (C) 1990-1992.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include <conio.h>
#include "draw.h"

struct CIRCLE
{
	short column, line, radius;
};

struct RECTANGLE
{
	short left, top, right, bottom;
};

struct TRIANGLE
{
	short triangle[8];
};

main()
{
	/* Initialize the graphics objects. */
	struct CIRCLE circle = { 100, 150, 50 };
	struct RECTANGLE rectangle = { 200, 100, 300, 200 };
	struct TRIANGLE triangle = { { 400, 100, 350, 200, 450, 200, 400, 100 } };

	/* Initialize the screen. */
	InitializeDisplay();

	/* Draw the objects. */
	DrawCircle(circle.column, circle.line, circle.radius);
	DrawRectangle(rectangle.left, rectangle.top, rectangle.right, rectangle.bottom);
	DrawTriangle(triangle.triangle);

	/* Get user input then restore the screen. */
	getch();
	RestoreDisplay();
	return (0);
}

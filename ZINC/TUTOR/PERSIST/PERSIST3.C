//	PERSIST3.C (PERSIST) - Persistent graphic objects using structures.
//	COPYRIGHT (C) 1990-1992.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include <conio.h>
#include "draw.h"

#define ID_CIRCLE		1
#define ID_RECTANGLE	2
#define ID_TRIANGLE		3

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

struct GRAPHIC_OBJECT
{
	int type;
	union
	{
		struct TRIANGLE triangle;
		struct RECTANGLE rectangle;
		struct CIRCLE circle;
	} graphic;
};

void DrawObject(struct GRAPHIC_OBJECT *object)
{
	if (object->type == ID_CIRCLE)
		DrawCircle(object->graphic.circle.column, object->graphic.circle.line,
			object->graphic.circle.radius);
	else if (object->type == ID_RECTANGLE)
		DrawRectangle(object->graphic.rectangle.left,
			object->graphic.rectangle.top, object->graphic.rectangle.right,
			object->graphic.rectangle.bottom);
	else if (object->type == ID_TRIANGLE)
		DrawTriangle(object->graphic.triangle.triangle);
}

main()
{
	/* Initialize the graphics objects. */
	struct GRAPHIC_OBJECT object1 = { ID_CIRCLE, { { { 100, 150, 50 } } } };
	struct GRAPHIC_OBJECT object2 = { ID_RECTANGLE, { { { 200, 100, 300, 200 } } } };
	struct GRAPHIC_OBJECT object3 = { ID_TRIANGLE, { { { 400, 100, 350, 200, 450, 200, 400, 100 } } } };

	/* Initialize the screen. */
	InitializeDisplay();

	/* Draw the objects. */
	DrawObject(&object1);
	DrawObject(&object2);
	DrawObject(&object3);

/* START BLOCK COMMENT
**		DrawCircle(circle.column, circle.line, circle.radius);
**		DrawRectangle(rectangle.left, rectangle.top, rectangle.right, rectangle.bottom);
**		DrawTriangle(triangle.triangle);
END BLOCK COMMENT */

	/* Get user input then restore the screen. */
	getch();
	RestoreDisplay();
	return (0);
}

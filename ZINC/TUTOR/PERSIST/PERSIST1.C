//	PERSIST1.C (PERSIST) - Simple display of graphic objects.
//	COPYRIGHT (C) 1990-1992.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#if defined(_MSC_VER)
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <graph.h>

main()
{
	int triangle[] = { 400, 100, 350, 200, 450, 200, 400, 100 };

	/* Initialize the screen. */
	if (!_setvideomode(_MAXRESMODE))
	{
		printf("ERROR: Initialization of the graphics display failed.\n");
		return (1);
	}

	/* Draw the graphic objects. */
	_ellipse(_GBORDER, 50, 100, 150, 200);
	_rectangle(_GBORDER, 200, 100, 300, 200);
	_polygon(_GBORDER, (struct _xycoord *)triangle, 4);

	/* Get user input then restore the screen. */
	_getch();
	_setvideomode(_DEFAULTMODE);

	return (0);
}
#endif

#if defined(__WATCOMC__)
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <graph.h>

main()
{
	struct xycoord triangle[] = { 400, 100, 350, 200, 450, 200, 400, 100 };

	/* Initialize the screen. */
	if (!_setvideomode(_MAXRESMODE))
	{
		printf("ERROR: Initialization of the graphics display failed.\n");
		return (1);
	}

	/* Draw the graphic objects. */
	_ellipse(_GBORDER, 50, 100, 150, 200);
	_rectangle(_GBORDER, 200, 100, 300, 200);
	_polygon(_GBORDER, 4, (struct xycoord *)triangle);

	/* Get user input then restore the screen. */
	getch();
	_setvideomode(_DEFAULTMODE);

	return (0);
}
#endif


#ifdef __ZTC__
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <fg.h>

main()
{
	int i;
	int lines;
	int ellipse[] = { 100, 150 };
	int triangle[] = { 400, 100, 350, 200, 450, 200, 400, 100 };
	fg_box_t box = { 200, 100, 300, 200 };

	/* Initialize the screen. */
	if (!fg_init())
	{
		printf("ERROR: Initialization of the graphics display failed.\n");
		return (1);
	}

	/* re-adjust the coordinates based on bottom-left based system. */
	lines = fg.displaybox[FG_Y2];
	for (i = 1; i < 2; i += 2)
		ellipse[i] = lines - ellipse[i];
	for (i = 1; i < 8; i += 2)
		triangle[i] = lines - triangle[i];
	for (i = 1; i < 4; i += 2)
		box[i] = lines - box[i];

	/* Draw the graphic objects. */
	fg_drawellipse(FG_WHITE, FG_MODE_SET, ~0, ellipse[0], ellipse[1],
		50, 50, 0, 3600, fg.displaybox);
	fg_drawbox(FG_WHITE, FG_MODE_SET, ~0, FG_LINE_SOLID,
		box, fg.displaybox);
	fg_drawpolygon(FG_WHITE, FG_MODE_SET, ~0, FG_LINE_SOLID,
		3, triangle, fg.displaybox);

	/* Get user input then restore the screen. */
	getch();
	fg_term();

	return (0);
}
#endif


#ifdef __BORLANDC__
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <graphics.h>

main()
{
	int triangle[] = { 400, 100, 350, 200, 450, 200, 400, 100 };

	/* Initialize the screen. */
	int mode;
	int driver = DETECT;
	initgraph(&driver, &mode, 0L);
	if (graphresult() != grOk)
	{
		printf("ERROR: Initialization of the graphics display failed.\n");
		exit(1);
	}

	/* Draw the graphic objects. */
	circle(100, 150, 50);
	rectangle(200, 100, 300, 200);
	drawpoly(4, triangle);

	/* Get user input then restore the screen. */
	getch();
	closegraph();

	return (0);
}
#endif

//	DRAW.C (PERSIST) - Draw functions (compiler specific).
//	COPYRIGHT (C) 1990-1992.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include <stdio.h>
#include "draw.h"

#if defined(_MSC_VER)
#include <stdlib.h>
#include <graph.h>

void InitializeDisplay(void)
{
	if (!_setvideomode(_MAXRESMODE))
	{
		printf("ERROR: Initialization of the graphics display failed.\n");
		exit(1);
	}
}
void RestoreDisplay(void)
{
	_setvideomode(_DEFAULTMODE);
}
void DrawCircle(short column, short line, short radius)
{
	_ellipse(_GBORDER, column - radius, line - radius,
		column + radius, line + radius);
}
void DrawRectangle(short left, short top, short right, short bottom)
{
	_rectangle(_GBORDER, left, top, right, bottom);
}
void DrawTriangle(short *triangle)
{
	_polygon(_GBORDER, (struct _xycoord *)triangle, 4);
}

#elif defined(__WATCOMC__)
#include <stdlib.h>
#include <graph.h>

void InitializeDisplay(void)
{
	if (!_setvideomode(_MAXRESMODE))
	{
		printf("ERROR: Initialization of the graphics display failed.\n");
		exit(1);
	}
}
void RestoreDisplay(void)
{
	_setvideomode(_DEFAULTMODE);
}
void DrawCircle(short column, short line, short radius)
{
	_ellipse(_GBORDER, column - radius, line - radius,
		column + radius, line + radius);
}
void DrawRectangle(short left, short top, short right, short bottom)
{
	_rectangle(_GBORDER, left, top, right, bottom);
}
void DrawTriangle(short *triangle)
{
	_polygon(_GBORDER, 4, (struct _xycoord *)triangle);
}

#elif defined(__ZTC__)
#include <stdlib.h>
#include <fg.h>

void InitializeDisplay(void)
{
	if (!fg_init())
	{
		printf("ERROR: Initialization of the graphics display failed.\n");
		exit(1);
	}
}
void RestoreDisplay(void)
{
	fg_term();
}
void DrawCircle(short column, short line, short radius)
{
	fg_drawellipse(FG_WHITE, FG_MODE_SET, ~0,
		column, fg.displaybox[FG_Y2] - line,
		radius, radius, 0, 3600, fg.displaybox);
}
void DrawRectangle(short left, short top, short right, short bottom)
{
	fg_box_t drawBox;
	drawBox[FG_X1] = left;
	drawBox[FG_Y1] = fg.displaybox[FG_Y2] - top;
	drawBox[FG_X2] = right;
	drawBox[FG_Y2] = fg.displaybox[FG_Y2] - bottom;

	fg_drawbox(FG_WHITE, FG_MODE_SET, ~0, FG_LINE_SOLID,
		drawBox, fg.displaybox);
}
void DrawTriangle(short *triangle)
{
	int i;
	int drawTriangle[8];
	for (i = 0; i < 8; i ++)
		drawTriangle[i] = triangle[i];
	for (i = 1; i < 8; i += 2)
		drawTriangle[i] = fg.displaybox[FG_Y2] - drawTriangle[i];

	fg_drawpolygon(FG_WHITE, FG_MODE_SET, ~0, FG_LINE_SOLID,
		3, drawTriangle, fg.displaybox);
}

#elif defined(__BORLANDC__)
#include <stdlib.h>
#include <graphics.h>

void InitializeDisplay(void)
{
	int mode;
	int driver = DETECT;
	initgraph(&driver, &mode, NULL);
	if (graphresult() != grOk)
	{
		printf("ERROR: Initialization of the graphics display failed.\n");
		exit(1);
	}
}
void RestoreDisplay(void)
{
	closegraph();
}
void DrawCircle(short column, short line, short radius)
{
	circle(column, line, radius);
}
void DrawRectangle(short left, short top, short right, short bottom)
{
	rectangle(left, top, right, bottom);
}
void DrawTriangle(short *triangle)
{
	drawpoly(4, (int *)triangle);
}
#endif

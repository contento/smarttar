/* Motif code definition file. */
#include <X11/Intrinsic.h>

/*	_drawingArea is the blank widget we use for drawing. */
extern Widget _drawingArea;

#if defined(__hpux) || defined(sun) || defined(__sgi) || defined(_IBM_RS6000)
#define ZIL_BIGENDIAN
#endif

#define byteSwap(val)	((val) = (((unsigned short)(val) >> 8) & 0xff) | (val) << 8)

void DrawCircle(column, line, radius)
	short column;
	short line;
	short radius;
{
	Display *xDisplay;
	GC xGc;
	xDisplay = XtDisplay(_drawingArea);
	xGc = XDefaultGC(xDisplay, DefaultScreen(xDisplay));

#if defined(ZIL_BIGENDIAN)
	byteSwap(column);
	byteSwap(line);
	byteSwap(radius);
#endif

	XDrawArc(xDisplay, XtWindow(_drawingArea), xGc,
		column - radius, line - radius,
		radius + radius, radius + radius, 0, 360 * 64);
}

void DrawRectangle(left, top, right, bottom)
	short left;
	short top;
	short right;
	short bottom;
{
	Display *xDisplay;
	GC xGc;
	xDisplay = XtDisplay(_drawingArea);
	xGc = XDefaultGC(xDisplay, DefaultScreen(xDisplay));

#if defined(ZIL_BIGENDIAN)
	byteSwap(left);
	byteSwap(top);
	byteSwap(right);
	byteSwap(bottom);
#endif

	XDrawRectangle(xDisplay, XtWindow(_drawingArea), xGc,
		left, top, right - left, bottom - top);
}

void DrawTriangle(triangle)
	short *triangle;
{
	XPoint xPoints[4];
	int i;

	Display *xDisplay;
	GC xGc;
	xDisplay = XtDisplay(_drawingArea);
	xGc = XDefaultGC(xDisplay, DefaultScreen(xDisplay));

	for (i = 0; i < 4; i++)
	{
#if defined(ZIL_BIGENDIAN)
		byteSwap(*(triangle + i*2));
		byteSwap(*(triangle + i*2+1));
#endif
		xPoints[i].x = triangle[i*2];
		xPoints[i].y = triangle[i*2+1];
	}

	XDrawLines(XtDisplay(_drawingArea), XtWindow(_drawingArea), xGc,
		xPoints, 4, CoordModeOrigin);
}


#include <windows.h>

extern HDC _hDC;

void DrawCircle(short column, short line, short radius)
{
	Arc(_hDC, column - radius, line - radius, column + radius,
		line + radius, column, line - radius, column, line - radius);
}

void DrawRectangle(short left, short top, short right, short bottom)
{
	Rectangle(_hDC, left, top, right, bottom);
}

void DrawTriangle(short *triangle)
{
#if defined(WIN32)
	MoveToEx(_hDC, triangle[0], triangle[1], NULL);
#else
	MoveTo(_hDC, triangle[0], triangle[1]);
#endif
	LineTo(_hDC, triangle[2], triangle[3]);
	LineTo(_hDC, triangle[4], triangle[5]);
	LineTo(_hDC, triangle[6], triangle[7]);
}


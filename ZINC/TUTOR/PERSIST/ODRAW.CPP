/* OS/2 code definition files. */
#define INCL_GPI
extern "C"
{
#include <os2.h>
}

/*	_hps is the presentation space required when painting information to
 *	the client area of the window.
 */
extern HPS _hps;

/*	_transform is needed because window coordinates are left/bottom based
 *	in OS/2.
 *		_transform.x is the true column position of the client window.
 *		_transform.y is the true height of the client window.
 *	These values are set when the WM_PAINT message is received and are
 *	used to offset the drawing positions of the circle, rectangle and
 *	triangle.
 */
extern POINTL _transform;

void DrawCircle(short column, short line, short radius)
{
	ARCPARAMS arc;
	POINTL os2Position;

	arc.lP = arc.lQ = radius;
	arc.lR = arc.lS = 0;
	GpiSetArcParams(_hps, &arc);

	os2Position.x = column - _transform.x;
	os2Position.y = _transform.y - line;
	GpiMove(_hps, &os2Position);

	GpiFullArc(_hps, DRO_OUTLINE, MAKEFIXED(1, 0));
}

void DrawRectangle(short left, short top, short right, short bottom)
{
	POINTL os2Position;

	os2Position.x = left - _transform.x;
	os2Position.y = _transform.y - top;
	GpiMove(_hps, &os2Position);
	os2Position.x = right + 1 - _transform.x;
	os2Position.y = _transform.y - bottom + 1;
	GpiBox(_hps, DRO_OUTLINE, &os2Position, 0, 0);
}

void DrawTriangle(short *triangle)
{
	int i;
	POINTL os2Position;

	os2Position.x = triangle[0] - _transform.x;
	os2Position.y = _transform.y - triangle[1];
	GpiMove(_hps, &os2Position);
	for (i = 1; i <= 3; i++)
	{
		os2Position.x = triangle[i*2] - _transform.x;
		os2Position.y = _transform.y - triangle[i*2+1];
		GpiLine(_hps, &os2Position);
	}
}


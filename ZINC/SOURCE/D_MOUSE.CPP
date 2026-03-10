//	Zinc Interface Library - D_MOUSE.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#if defined(__BCPLUSPLUS__) | defined(__TCPLUSPLUS__)
#include <mem.h>
#endif
#include <dos.h>
#include "ui_evt.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

// ----- UID_MOUSE ----------------------------------------------------------

#define gnd	BACKGROUND
#define BLK	BLACK
#define wte	WHITE
#define gRy	LIGHTGRAY

static UCHAR _view[164] =
{
	0, 0, 10, 16,
	BLK,BLK,gnd,gnd,gnd,gnd,gnd,gnd,gnd,gnd,
	BLK,wte,BLK,gnd,gnd,gnd,gnd,gnd,gnd,gnd,
	BLK,wte,wte,BLK,gnd,gnd,gnd,gnd,gnd,gnd,
	BLK,wte,wte,wte,BLK,gnd,gnd,gnd,gnd,gnd,
	BLK,wte,wte,wte,wte,BLK,gnd,gnd,gnd,gnd,
	BLK,wte,wte,wte,wte,wte,BLK,gnd,gnd,gnd,
	BLK,wte,wte,wte,wte,wte,wte,BLK,gnd,gnd,
	BLK,wte,wte,wte,wte,wte,wte,wte,BLK,gnd,
	BLK,wte,wte,wte,wte,wte,wte,wte,BLK,BLK,
	BLK,wte,wte,wte,wte,wte,BLK,BLK,BLK,gnd,
	BLK,wte,wte,BLK,wte,wte,BLK,gnd,gnd,gnd,
	BLK,BLK,BLK,BLK,BLK,wte,wte,BLK,gnd,gnd,
	gnd,gnd,gnd,gnd,BLK,wte,wte,BLK,gnd,gnd,
	gnd,gnd,gnd,gnd,gnd,BLK,wte,wte,BLK,gnd,
	gnd,gnd,gnd,gnd,gnd,BLK,wte,wte,BLK,gnd,
	gnd,gnd,gnd,gnd,gnd,gnd,BLK,BLK,gnd,gnd
};

static UCHAR _edit[79] =
{
	2, 6, 5, 15,
	BLK,BLK,gnd,BLK,BLK,
	gnd,gnd,BLK,gnd,gnd,
	gnd,gnd,BLK,gnd,gnd,
	gnd,gnd,BLK,gnd,gnd,
	gnd,gnd,BLK,gnd,gnd,
	gnd,gnd,BLK,gnd,gnd,
	gnd,gnd,BLK,gnd,gnd,
	gnd,gnd,BLK,gnd,gnd,
	gnd,gnd,BLK,gnd,gnd,
	gnd,gnd,BLK,gnd,gnd,
	gnd,gnd,BLK,gnd,gnd,
	gnd,gnd,BLK,gnd,gnd,
	gnd,gnd,BLK,gnd,gnd,
	gnd,gnd,BLK,gnd,gnd,
	BLK,BLK,gnd,BLK,BLK
};

static UCHAR _wait[409] =
{
	7, 13, 15, 27,
	BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,
	BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,
	BLK,BLK,wte,wte,wte,wte,wte,wte,wte,wte,wte,wte,wte,BLK,BLK,
	gnd,BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,gnd,
	gnd,BLK,BLK,wte,wte,wte,wte,wte,wte,wte,wte,wte,BLK,BLK,gnd,
	gnd,BLK,BLK,wte,wte,wte,wte,wte,wte,wte,wte,wte,BLK,BLK,gnd,
	gnd,BLK,BLK,wte,wte,wte,wte,wte,wte,gRy,wte,wte,BLK,BLK,gnd,
	gnd,BLK,BLK,wte,gRy,wte,gRy,wte,gRy,wte,gRy,wte,BLK,BLK,gnd,
	gnd,BLK,BLK,wte,wte,gRy,wte,gRy,wte,gRy,wte,wte,BLK,BLK,gnd,
	gnd,gnd,BLK,BLK,wte,wte,gRy,wte,gRy,wte,wte,BLK,BLK,gnd,gnd,
	gnd,gnd,gnd,BLK,BLK,wte,wte,gRy,wte,wte,BLK,BLK,gnd,gnd,gnd,
	gnd,gnd,gnd,gnd,BLK,BLK,wte,wte,wte,BLK,BLK,gnd,gnd,gnd,gnd,
	gnd,gnd,gnd,gnd,gnd,BLK,BLK,wte,BLK,BLK,gnd,gnd,gnd,gnd,gnd,
	gnd,gnd,gnd,gnd,gnd,BLK,BLK,wte,BLK,BLK,gnd,gnd,gnd,gnd,gnd,
	gnd,gnd,gnd,gnd,gnd,BLK,BLK,wte,BLK,BLK,gnd,gnd,gnd,gnd,gnd,
	gnd,gnd,gnd,gnd,BLK,BLK,wte,wte,wte,BLK,BLK,gnd,gnd,gnd,gnd,
	gnd,gnd,gnd,BLK,BLK,wte,wte,wte,wte,wte,BLK,BLK,gnd,gnd,gnd,
	gnd,gnd,BLK,BLK,wte,wte,wte,gRy,wte,wte,wte,BLK,BLK,gnd,gnd,
	gnd,BLK,BLK,wte,wte,wte,wte,wte,wte,wte,wte,wte,BLK,BLK,gnd,
	gnd,BLK,BLK,wte,wte,wte,wte,gRy,wte,wte,wte,wte,BLK,BLK,gnd,
	gnd,BLK,BLK,wte,wte,wte,gRy,wte,gRy,wte,wte,wte,BLK,BLK,gnd,
	gnd,BLK,BLK,wte,wte,gRy,wte,gRy,wte,gRy,wte,wte,BLK,BLK,gnd,
	gnd,BLK,BLK,wte,gRy,wte,gRy,wte,gRy,wte,gRy,wte,BLK,BLK,gnd,
	BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,
	BLK,BLK,wte,wte,wte,wte,wte,wte,wte,wte,wte,wte,wte,BLK,BLK,
	BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK
};

static UCHAR _move[365] =
{
	9, 9, 19, 19,
	gnd,gnd,gnd,gnd,gnd,gnd,gnd,gnd,gnd,BLK,gnd,gnd,gnd,gnd,gnd,gnd,gnd,gnd,gnd,
	gnd,gnd,gnd,gnd,gnd,gnd,gnd,gnd,BLK,wte,BLK,gnd,gnd,gnd,gnd,gnd,gnd,gnd,gnd,
	gnd,gnd,gnd,gnd,gnd,gnd,gnd,BLK,wte,wte,wte,BLK,gnd,gnd,gnd,gnd,gnd,gnd,gnd,
	gnd,gnd,gnd,gnd,gnd,gnd,BLK,wte,wte,wte,wte,wte,BLK,gnd,gnd,gnd,gnd,gnd,gnd,
	gnd,gnd,gnd,gnd,gnd,BLK,BLK,BLK,BLK,wte,BLK,BLK,BLK,BLK,gnd,gnd,gnd,gnd,gnd,
	gnd,gnd,gnd,gnd,BLK,gnd,gnd,gnd,BLK,wte,BLK,gnd,gnd,gnd,BLK,gnd,gnd,gnd,gnd,
	gnd,gnd,gnd,BLK,BLK,gnd,gnd,gnd,BLK,wte,BLK,gnd,gnd,gnd,BLK,BLK,gnd,gnd,gnd,
	gnd,gnd,BLK,wte,BLK,gnd,gnd,gnd,BLK,wte,BLK,gnd,gnd,gnd,BLK,wte,BLK,gnd,gnd,
	gnd,BLK,wte,wte,BLK,BLK,BLK,BLK,BLK,wte,BLK,BLK,BLK,BLK,BLK,wte,wte,BLK,gnd,
	BLK,wte,wte,wte,wte,wte,wte,wte,wte,wte,wte,wte,wte,wte,gnd,wte,wte,wte,BLK,
	gnd,BLK,wte,wte,BLK,BLK,BLK,BLK,BLK,wte,BLK,BLK,BLK,BLK,BLK,wte,wte,BLK,gnd,
	gnd,gnd,BLK,wte,BLK,gnd,gnd,gnd,BLK,wte,BLK,gnd,gnd,gnd,BLK,wte,BLK,gnd,gnd,
	gnd,gnd,gnd,BLK,BLK,gnd,gnd,gnd,BLK,wte,BLK,gnd,gnd,gnd,BLK,BLK,gnd,gnd,gnd,
	gnd,gnd,gnd,gnd,BLK,gnd,gnd,gnd,BLK,wte,BLK,gnd,gnd,gnd,BLK,gnd,gnd,gnd,gnd,
	gnd,gnd,gnd,gnd,gnd,BLK,BLK,BLK,BLK,wte,BLK,BLK,BLK,BLK,gnd,gnd,gnd,gnd,gnd,
	gnd,gnd,gnd,gnd,gnd,gnd,BLK,wte,wte,wte,wte,wte,BLK,gnd,gnd,gnd,gnd,gnd,gnd,
	gnd,gnd,gnd,gnd,gnd,gnd,gnd,BLK,wte,wte,wte,BLK,gnd,gnd,gnd,gnd,gnd,gnd,gnd,
	gnd,gnd,gnd,gnd,gnd,gnd,gnd,gnd,BLK,wte,BLK,gnd,gnd,gnd,gnd,gnd,gnd,gnd,gnd,
	gnd,gnd,gnd,gnd,gnd,gnd,gnd,gnd,gnd,BLK,gnd,gnd,gnd,gnd,gnd,gnd,gnd,gnd,gnd
};

static UCHAR _horizontal[196] =
{
	7, 5, 16, 12,
	gnd,gnd,gnd,gnd,gnd,BLK,gnd,gnd,gnd,gnd,BLK,gnd,gnd,gnd,gnd,gnd,
	gnd,gnd,gnd,gnd,BLK,BLK,gnd,gnd,gnd,gnd,BLK,BLK,gnd,gnd,gnd,gnd,
	gnd,gnd,gnd,BLK,wte,BLK,gnd,gnd,gnd,gnd,BLK,wte,BLK,gnd,gnd,gnd,
	gnd,gnd,BLK,wte,wte,BLK,gnd,gnd,gnd,gnd,BLK,wte,wte,BLK,gnd,gnd,
	gnd,BLK,wte,wte,wte,BLK,BLK,BLK,BLK,BLK,BLK,wte,wte,wte,BLK,gnd,
	BLK,wte,wte,wte,wte,wte,wte,wte,wte,wte,wte,wte,wte,wte,wte,BLK,
	BLK,wte,wte,wte,wte,wte,wte,wte,wte,wte,wte,wte,wte,wte,wte,BLK,
	gnd,BLK,wte,wte,wte,BLK,BLK,BLK,BLK,BLK,BLK,wte,wte,wte,BLK,gnd,
	gnd,gnd,BLK,wte,wte,BLK,gnd,gnd,gnd,gnd,BLK,wte,wte,BLK,gnd,gnd,
	gnd,gnd,gnd,BLK,wte,BLK,gnd,gnd,gnd,gnd,BLK,wte,BLK,gnd,gnd,gnd,
	gnd,gnd,gnd,gnd,BLK,BLK,gnd,gnd,gnd,gnd,BLK,BLK,gnd,gnd,gnd,gnd,
	gnd,gnd,gnd,gnd,gnd,BLK,gnd,gnd,gnd,gnd,BLK,gnd,gnd,gnd,gnd,gnd,
};

static UCHAR _vertical[196] =
{
	5, 7, 12, 16,
	gnd,gnd,gnd,gnd,gnd,BLK,BLK,gnd,gnd,gnd,gnd,gnd,
	gnd,gnd,gnd,gnd,BLK,wte,wte,BLK,gnd,gnd,gnd,gnd,
	gnd,gnd,gnd,BLK,wte,wte,wte,wte,BLK,gnd,gnd,gnd,
	gnd,gnd,BLK,wte,wte,wte,wte,wte,wte,BLK,gnd,gnd,
	gnd,BLK,wte,wte,wte,wte,wte,wte,wte,wte,BLK,gnd,
	BLK,BLK,BLK,BLK,BLK,wte,wte,BLK,BLK,BLK,BLK,BLK,
	gnd,gnd,gnd,gnd,BLK,wte,wte,BLK,gnd,gnd,gnd,gnd,
	gnd,gnd,gnd,gnd,BLK,wte,wte,BLK,gnd,gnd,gnd,gnd,
	gnd,gnd,gnd,gnd,BLK,wte,wte,BLK,gnd,gnd,gnd,gnd,
	gnd,gnd,gnd,gnd,BLK,wte,wte,BLK,gnd,gnd,gnd,gnd,
	BLK,BLK,BLK,BLK,BLK,wte,wte,BLK,BLK,BLK,BLK,BLK,
	gnd,BLK,wte,wte,wte,wte,wte,wte,wte,wte,BLK,gnd,
	gnd,gnd,BLK,wte,wte,wte,wte,wte,wte,BLK,gnd,gnd,
	gnd,gnd,gnd,BLK,wte,wte,wte,wte,BLK,gnd,gnd,gnd,
	gnd,gnd,gnd,gnd,BLK,wte,wte,BLK,gnd,gnd,gnd,gnd,
	gnd,gnd,gnd,gnd,gnd,BLK,BLK,gnd,gnd,gnd,gnd,gnd
};

static UCHAR _left[173] =
{
	5, 5, 12, 12,
	BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,gnd,gnd,gnd,gnd,
	BLK,wte,wte,wte,wte,wte,BLK,gnd,gnd,gnd,gnd,gnd,
	BLK,wte,wte,wte,wte,BLK,gnd,gnd,gnd,gnd,gnd,gnd,
	BLK,wte,wte,wte,wte,BLK,gnd,gnd,gnd,gnd,gnd,gnd,
	BLK,wte,wte,wte,wte,wte,BLK,gnd,gnd,gnd,gnd,BLK,
	BLK,wte,BLK,BLK,wte,wte,wte,BLK,gnd,gnd,BLK,BLK,
	BLK,BLK,gnd,gnd,BLK,wte,wte,wte,BLK,BLK,wte,BLK,
	BLK,gnd,gnd,gnd,gnd,BLK,wte,wte,wte,wte,wte,BLK,
	gnd,gnd,gnd,gnd,gnd,gnd,BLK,wte,wte,wte,wte,BLK,
	gnd,gnd,gnd,gnd,gnd,gnd,BLK,wte,wte,wte,wte,BLK,
	gnd,gnd,gnd,gnd,gnd,BLK,wte,wte,wte,wte,wte,BLK,
	gnd,gnd,gnd,gnd,BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK
};

static UCHAR _right[173] =
{
	5, 5, 12, 12,
	gnd,gnd,gnd,gnd,BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,
	gnd,gnd,gnd,gnd,gnd,BLK,wte,wte,wte,wte,wte,BLK,
	gnd,gnd,gnd,gnd,gnd,gnd,BLK,wte,wte,wte,wte,BLK,
	gnd,gnd,gnd,gnd,gnd,gnd,BLK,wte,wte,wte,wte,BLK,
	BLK,gnd,gnd,gnd,gnd,BLK,wte,wte,wte,wte,wte,BLK,
	BLK,BLK,gnd,gnd,BLK,wte,wte,wte,BLK,BLK,wte,BLK,
	BLK,wte,BLK,BLK,wte,wte,wte,BLK,gnd,gnd,BLK,BLK,
	BLK,wte,wte,wte,wte,wte,BLK,gnd,gnd,gnd,gnd,BLK,
	BLK,wte,wte,wte,wte,BLK,gnd,gnd,gnd,gnd,gnd,gnd,
	BLK,wte,wte,wte,wte,BLK,gnd,gnd,gnd,gnd,gnd,gnd,
	BLK,wte,wte,wte,wte,wte,BLK,gnd,gnd,gnd,gnd,gnd,
	BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,gnd,gnd,gnd,gnd
};

static UCHAR _position[125] =
{
	5, 5, 11, 11,
	gnd,gnd,gnd,gnd,BLK,BLK,BLK,gnd,gnd,gnd,gnd,
	gnd,gnd,gnd,gnd,BLK,wte,BLK,gnd,gnd,gnd,gnd,
	gnd,gnd,gnd,gnd,BLK,wte,BLK,gnd,gnd,gnd,gnd,
	gnd,gnd,gnd,gnd,BLK,wte,BLK,gnd,gnd,gnd,gnd,
	BLK,BLK,BLK,BLK,BLK,wte,BLK,BLK,BLK,BLK,BLK,
	BLK,wte,wte,wte,wte,wte,wte,wte,wte,wte,BLK,
	BLK,BLK,BLK,BLK,BLK,wte,BLK,BLK,BLK,BLK,BLK,
	gnd,gnd,gnd,gnd,BLK,wte,BLK,gnd,gnd,gnd,gnd,
	gnd,gnd,gnd,gnd,BLK,wte,BLK,gnd,gnd,gnd,gnd,
	gnd,gnd,gnd,gnd,BLK,wte,BLK,gnd,gnd,gnd,gnd,
	gnd,gnd,gnd,gnd,BLK,BLK,BLK,gnd,gnd,gnd,gnd
};

static int _upToDate = TRUE;
static int _horizontalRatio = 1;
static int _verticalRatio = 1;

#define MAX_EVENTS	20
static unsigned sFifo[MAX_EVENTS];						// BUG.2104, from mods suggested by FMC
static int xFifo[MAX_EVENTS];
static int yFifo[MAX_EVENTS];
static int zState;
static int volatile in, out;

static void mouseQueueEventFunc(unsigned state, int *horizontalMickeys, int *verticalMickeys)
{
	int deltaX = *horizontalMickeys / _horizontalRatio;
	int deltaY = *verticalMickeys / _verticalRatio;

	// Make sure the mouse is still on the screen.
	if (state != zState || (deltaX || deltaY) && in == out)
	{
		zState = state;
		int i = in;
		if (++i >= MAX_EVENTS)
			i = 0;
		if (i != out)
		{
			sFifo[i] = state;
			xFifo[i] = deltaX;
			yFifo[i] = deltaY;
			in = i;
		}
		// Update the mickey count.
		*horizontalMickeys %= _horizontalRatio;
		*verticalMickeys %= _verticalRatio;
	}
}

UID_MOUSE::UID_MOUSE(DEVICE_STATE _state, DEVICE_IMAGE _image) :
	UI_DEVICE(E_MOUSE, _state), image(_image)
{
	if (!I_MouseOpen())	// Try to initialize the mouse driver.
		return;
	mouseQueueEvent = mouseQueueEventFunc;
	position.column = position.line = 0;
	offset.column = offset.line = 0;
	installed = TRUE;
}

UID_MOUSE::~UID_MOUSE(void)
{
	// Restore the old mouse information.
	if (installed) {
		mouseQueueEvent = NULL;
		I_MouseClose();
	}
	installed = FALSE;
}

// Initialize the standard zinc cursors.
struct MOUSE_ITEM
{
	EVENT_TYPE image;
	UCHAR *bitmapImage;
} _mouseTable[] = {
	{ DM_VIEW, 		_view },
	{ DM_EDIT, 		_edit },
	{ DM_WAIT, 		_wait },
	{ DM_MOVE, 		_move },
	{ DM_HORIZONTAL, 	_horizontal },
	{ DM_VERTICAL, 		_vertical },
	{ DM_DIAGONAL_ULLR,	_left },
	{ DM_DIAGONAL_LLUR,	_right },
	{ DM_POSITION, 		_position },
	{ 0, 0 } };

EVENT_TYPE UID_MOUSE::Event(const UI_EVENT &event)
{
	// Make sure the mouse is installed.
	if (!installed)
		return (D_OFF);

	// Switch on the raw code.
	DEVICE_STATE oldState = state;
	DEVICE_IMAGE oldImage = image;
	UI_POSITION oldPosition = position;
	EVENT_TYPE ccode = event.type;
	switch (ccode)
	{
	case S_INITIALIZE:
		_horizontalRatio = display->isText ? (display->columns == 40 ? 16 : 8) : 1;
		_verticalRatio = display->isText ? 8 : 1;
		position.column = display->columns / 2;
		position.line = display->lines / 2;
		oldImage = 0;
		oldState = D_OFF;
		break;

	case S_DEINITIALIZE:							// BUG.1066
		display->DeviceMove(MOUSE_IMAGE, display->columns, display->lines);	// BUG.1526
//		state = D_OFF;							// BUG.internal
		state = D_HIDE;
		break;

	case S_POSITION:
		position = event.position;
		break;

	case DM_VIEW:
	case DM_EDIT:
	case DM_WAIT:
	case DM_MOVE:
	case DM_HORIZONTAL:
	case DM_VERTICAL:
	case DM_DIAGONAL_ULLR:
	case DM_DIAGONAL_LLUR:
	case DM_POSITION:
		if (state != D_OFF)
		{
			image = ccode;
			state = D_ON;
		}
		break;

	case D_ON:
		display->DeviceMove(MOUSE_IMAGE,				// BUG.1526
				    position.column - offset.column,
				    position.line - offset.line);
		if (state == D_OFF)
		{
			out = in;
			state = D_ON;
		}
		break;

	case D_OFF:
		state = D_OFF;
		break;

	case D_HIDE:
		display->DeviceMove(MOUSE_IMAGE, display->columns, display->lines);	// BUG.1526
		state = event.type;
		break;

	default:
		return (ccode);
	}

	// Reset the mouse information.
	if (oldImage == image && oldState == state && oldPosition == position)
		return (ccode);
	if ((oldPosition != position && state != D_HIDE))
		_upToDate = display->DeviceMove(MOUSE_IMAGE,
			position.column - offset.column, position.line - offset.line);
	if (oldImage != image)
	{
		for (int i = 0; _mouseTable[i].image; i++)
			if (_mouseTable[i].image == image && display->isText)
			{
				_upToDate = display->DeviceSet(MOUSE_IMAGE, position.column,
					position.line, 1, 1, NULL);
				break;
			}
			else if (_mouseTable[i].image == image)
			{
				UCHAR *bitmapImage = _mouseTable[i].bitmapImage;
				image = _mouseTable[i].image;
				offset.column = bitmapImage[0];
				offset.line = bitmapImage[1];
				_upToDate = display->DeviceSet(MOUSE_IMAGE,
					position.column - offset.column,
					position.line - offset.line, bitmapImage[2],
					bitmapImage[3], &bitmapImage[4]);
				break;
			}
	}

	// Return the control code.
	return (ccode);
}

void UID_MOUSE::Poll(void)
{
	// Make sure the mouse is valid.
	if (installed && state != D_OFF && in != out)
	{
		UI_EVENT event;
		event.type = E_MOUSE;
		while (in != out)
		{
			if (++out >= MAX_EVENTS)
				out = 0;
			position.column += xFifo[out];
			position.line   += yFifo[out];
			if (position.column >= display->columns)
				position.column = display->columns - 1;
			else if (position.column < 0)
				position.column = 0;
			if (position.line >= display->lines)
				position.line = display->lines - 1;
			else if (position.line < 0)
				position.line = 0;
			event.rawCode = sFifo[out];
			event.position = position;
			eventManager->Put(event);
		}
		if (state != D_HIDE)
			_upToDate = display->DeviceMove(MOUSE_IMAGE,
				position.column - offset.column,
				position.line - offset.line);
	}
	// Mouse could not be updated during a previous paint operation.
	else if (!_upToDate)
	{
		DEVICE_IMAGE newImage = image;
		image = 0;
		Event(UI_EVENT(newImage));
		_upToDate = TRUE;
	}
}

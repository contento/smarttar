//	Zinc Interface Library - D_CURSOR.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "ui_evt.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

// WATCOM has a compiler bug if the constructor and destructor don't
// have any other functions in the file (Poll, Event). So definition
// of constructor & destructor are in d_cursor.
#if defined(__WATCOMC__)

UID_CURSOR::UID_CURSOR(DEVICE_STATE _state, DEVICE_IMAGE _image) :
	UI_DEVICE(E_CURSOR, _state), image(_image)
{
#if defined(ZIL_LINKBUG)
	extern void z_device_dummy(void);		// Bug fix for broken linkers.
	z_device_dummy();
#endif

	position.column = position.line = 0;
	offset.column = offset.line = 0;
	installed = TRUE;
}

UID_CURSOR::~UID_CURSOR(void)
{
	// No cursor information needs to be destroyed.
}

#endif

#define gnd	BACKGROUND
#define BLK	BLACK
static UCHAR _insert[32] =
{
	0, 0, 2, 14,
	BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,
	BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK
};
static UCHAR _overstrike[18] =
{
	0, 0, 1, 14,
	BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK
};

// ----- UID_CURSOR ---------------------------------------------------------

static int _upToDate = TRUE;

EVENT_TYPE UID_CURSOR::Event(const UI_EVENT &event)
{
	// Switch on the raw code.
	DEVICE_STATE oldState = state;
	DEVICE_IMAGE oldImage = image;
	UI_POSITION oldPosition = position;
	EVENT_TYPE ccode = event.type;
	switch (ccode)
	{
	case S_INITIALIZE:
		oldState = D_OFF;
		break;

	case S_DEINITIALIZE:
		state = D_HIDE;
		break;

	case D_OFF:
	case D_ON:
		state = event.type;
		break;

	case S_POSITION:
		position = event.position;
		if (state == D_HIDE)
			state = D_ON;
		break;

	case DC_INSERT:
	case DC_OVERSTRIKE:
		image = ccode;
		state = D_ON;
		break;
	}

	// Reset the cursor information.
	if (oldImage == image && oldState == state && oldPosition == position)
		return (ccode);
	if (state == D_ON && display->isText && position.column >= 0 &&
		position.column < display->columns && position.line >= 0 &&
		position.line < display->lines)
		I_CursorPosition(position.line+1, position.column+1, image);
	else if (display->isText)
		I_CursorRemove();		
	else if (state == D_ON)
	{
		UCHAR *bitmapImage = (image == DC_INSERT) ? _insert : _overstrike;
		offset.column = offset.line = 0;
		_upToDate = display->DeviceSet(CURSOR_IMAGE, position.column, 
			position.line, bitmapImage[2], display->TextHeight(NULL) + 1,
			&bitmapImage[4]);
	}
	else
		_upToDate = display->DeviceMove(CURSOR_IMAGE, display->columns,
			display->lines);

	// Return the control code.
	return (ccode);
}

void UID_CURSOR::Poll(void)
{
	// Make sure the cursor is valid.
	static UI_TIME lastTime;
	if (!installed || state == D_OFF || display->isText)
		return;

	// Check the blink state of the cursor.
	UI_TIME currentTime;
	long elapsedTime = currentTime - lastTime;								// BUG.1228
	if (Abs(elapsedTime) > blinkRate)
	{
		lastTime = currentTime;
		if (state == D_ON)
		{
			_upToDate = display->DeviceMove(CURSOR_IMAGE, display->columns,
				display->lines);
			state = D_HIDE;
		}
		else
		{
			_upToDate = display->DeviceMove(CURSOR_IMAGE,
				position.column + offset.column, position.line + offset.line);
			state = D_ON;
		}
	}
	// Cursor could not be updated during a previous paint operation.
	else if (!_upToDate)
	{
		DEVICE_IMAGE newImage = image;
		image = 0;
		Event(UI_EVENT(newImage));
		_upToDate = TRUE;
	}
}


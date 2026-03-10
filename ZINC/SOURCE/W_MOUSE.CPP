//	Zinc Interface Library - W_MOUSE.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "ui_evt.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

// ----- UID_MOUSE ----------------------------------------------------------

UID_MOUSE::UID_MOUSE(DEVICE_STATE _state, DEVICE_IMAGE _image) :
	UI_DEVICE(E_MOUSE, _state), image(_image)
{
	// Initialize the new mouse information.
	position.column = position.line = 0;
	offset.column = offset.line = 0;
	installed = TRUE;
}

UID_MOUSE::~UID_MOUSE(void)
{
}

EVENT_TYPE UID_MOUSE::Event(const UI_EVENT &event)
{
	// Initialize the standard windows cursors.
	struct MOUSE_ITEM
	{
		EVENT_TYPE image;
		LPSTR identification;
	} _mouseTable[] = {
		{ DM_VIEW, 			(LPSTR)IDC_ARROW },
		{ DM_EDIT, 			(LPSTR)IDC_IBEAM },
		{ DM_WAIT, 			(LPSTR)IDC_WAIT },
		{ DM_MOVE, 			(LPSTR)IDC_SIZE },
		{ DM_HORIZONTAL, 	(LPSTR)IDC_SIZEWE },
		{ DM_VERTICAL, 		(LPSTR)IDC_SIZENS },
		{ DM_DIAGONAL_ULLR, (LPSTR)IDC_SIZENWSE },
		{ DM_DIAGONAL_LLUR, (LPSTR)IDC_SIZENESW },
		{ DM_POSITION, 		(LPSTR)IDC_CROSS },
		{ 0, 0 } };

	// Switch on the raw code.
	EVENT_TYPE ccode = event.type;
	switch (ccode)
	{
	case S_POSITION:
		position = event.position;
		SetCursorPos(position.column, position.line);
		break;

	case S_INITIALIZE:
		ccode = image;
		image = 0;
		// Continue to default.
	default:
		for (int i = 0; _mouseTable[i].image && image != ccode; i++)
			if (_mouseTable[i].image == ccode)
			{
				HCURSOR hCursor = LoadCursor(0, _mouseTable[i].identification);
				SetCursor(hCursor);
			}
		break;
	}

	// Return the control code.
	return (ccode);
}

void UID_MOUSE::Poll(void)
{
}


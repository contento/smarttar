//	PERSIST8.CPP (PERSIST) - Abstract read from disk into Microsoft Windows.
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA
//  May be freely copied, used and distributed.

// OS/2 code definition files.
#define INCL_WINSYS
#define INCL_WINFRAMEMGR
#define INCL_WINWINDOWMGR
#include <stdio.h>
#include <string.h>
extern "C"
{
#include <os2.h>
}

// Persistent code definition files.
#include "draw.h"
#include "persist.hpp"

//	_hps is the presentation space required when painting information to
//	the client area of the window.
HPS _hps;

//	_transform is needed because window coordinates are left/bottom based
//	in OS/2.
//		_transform.x is the true column position of the client window.
//		_transform.y is the true height of the client window.
//	These values are set when the WM_PAINT message is received and are
//	used to offset the drawing positions of the circle, rectangle and
//	triangle.
//
POINTL _transform;

GRAPHIC_OBJECT::JUMP_ELEMENT GRAPHIC_OBJECT::_jumpTable[] =
{
	{ ID_CIRCLE, 	CIRCLE::New },
	{ ID_RECTANGLE,	RECTANGLE::New },
	{ ID_TRIANGLE,	TRIANGLE::New },
	{ 0, NULL }		// end of array.
};

GRAPHIC_OBJECT *GRAPHIC_OBJECT::New(FILE *file)
{
	// Read the type from disk.
	short type;
	int fileObject = fread(&type, sizeof(type), 1, file);
	if (!fileObject)
		return (NULL);

	// Get the New pointer and create the object.
	for (int i = 0; _jumpTable[i].newFunction; i++)
		if (type == _jumpTable[i].type)
			return (_jumpTable[i].newFunction(file, L_SKIP_TYPE));
	return (NULL);
}

MRESULT EXPENTRY WndProc(HWND hwnd, ULONG message, MPARAM mp1, MPARAM mp2)
{
	// Intercept the paint message.
	if (message == WM_PAINT)
	{
		RECTL os2Region;
		WinQueryWindowRect(hwnd, &os2Region);
		_transform.x = os2Region.xLeft;
		_transform.y = os2Region.yTop - os2Region.yBottom;
		_hps = WinBeginPaint(hwnd, NULL, &os2Region);
		WinFillRect(_hps, &os2Region, CLR_PALEGRAY);

		FILE *file = fopen("persist.dat", "rb");
		if (file)
		{
			int fileObjects = 1;
			do
			{
				GRAPHIC_OBJECT *object = GRAPHIC_OBJECT::New(file);
				if (object)
				{
					object->Draw();
					delete object;
				}
				else
					fileObjects = 0;
			} while (fileObjects);
			fclose(file);
		}
		else
		{
			char *text = "ERROR: The \"persist.dat\" file could not be found.";
			ULONG drawFlags = DT_LEFT | DT_VCENTER | DT_TEXTATTRS | DT_MNEMONIC;
			WinDrawText(_hps, strlen(text), (PSZ)text, &os2Region, 0, 0, drawFlags);
		}

		WinEndPaint(_hps);
		return (NULL);
	}

	// Let the default procedure process all other messages.
	return (WinDefWindowProc(hwnd, message, mp1, mp2));
}

main()
{
	// Initialize the OS/2 application and message queue.
	HAB hab = WinInitialize(0);
	HMQ hmq = WinCreateMsgQueue(hab, 0);

	// Create the presentation window.
	FRAMECDATA frameData;
	frameData.cb = sizeof(FRAMECDATA);
	frameData.flCreateFlags = FCF_TITLEBAR | FCF_SYSMENU | FCF_MINMAX |
		FCF_SIZEBORDER | FCF_SHELLPOSITION | FCF_TASKLIST;
	frameData.hmodResources = 0;
	frameData.idResources = 100;
	HWND frame = WinCreateWindow(HWND_DESKTOP, WC_FRAME, (PSZ)"Persistence",
		0, 0, 0, 450, 300, 0, HWND_BOTTOM, 0, &frameData, NULL);
	HWND client = WinCreateWindow(frame, WC_FRAME, NULL, WS_VISIBLE,
		0, 0, 0, 0, frame, HWND_BOTTOM, FID_CLIENT, NULL, NULL);
	WinSetWindowPtr(client, QWP_PFNWP, (PVOID)WndProc);
	WinShowWindow(frame, TRUE);
	WinUpdateWindow(frame);

	// Process messages until a WM_NULL is received.
	QMSG message;
	while (WinGetMsg(hab, &message, 0, 0, 0))
		WinDispatchMsg(hab, &message);

	// Restore the OS/2 application and message queue.
	WinDestroyMsgQueue(hmq);
	WinTerminate(hab);
    return (0);
}

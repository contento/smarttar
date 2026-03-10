//	PERSIST8.CPP (PERSIST) - Abstract read from disk into Microsoft Windows.
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA
//  May be freely copied, used and distributed.

#include <stdio.h>
#include <windows.h>
#include "draw.h"
#include "persist.hpp"

extern "C" { HDC _hDC; }

#if (__BORLANDC__ >= 0x0300) || defined(_MSC_VER) || (__ZTC__ >= 0x0310) || defined(__WATCOMC__)
GRAPHIC_OBJECT::JUMP_ELEMENT GRAPHIC_OBJECT::_jumpTable[] =
#else
JUMP_ELEMENT GRAPHIC_OBJECT::_jumpTable[] =
#endif 
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

#if defined(WIN32)
long FAR PASCAL WndProc(HWND hWnd, unsigned message, unsigned wParam, LONG lParam)
#else
long FAR PASCAL _export WndProc(HWND hWnd, unsigned message, unsigned wParam, LONG lParam)
#endif
{
    PAINTSTRUCT ps;

	switch (message)
	{
	case WM_KEYDOWN:
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_PAINT:
		{
	    BeginPaint(hWnd, &ps);
		_hDC = ps.hdc;

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
			WORD format = DT_SINGLELINE | DT_VCENTER | DT_LEFT;
			::DrawText(_hDC, (LPSTR)text, -1, &ps.rcPaint, format);
		}

	    EndPaint(hWnd, &ps);
		}
		break;

	default:
		return (DefWindowProc(hWnd, message, wParam, lParam));
	}
	return (NULL);
}

int PASCAL WinMain(HANDLE hInstance, HANDLE hPrevInstance, LPSTR, int nCmdShow)
{
	if (!hPrevInstance)
	{
		WNDCLASS wndclass;
		wndclass.style = CS_HREDRAW | CS_VREDRAW;
		wndclass.lpfnWndProc = WndProc;
		wndclass.cbClsExtra = 0;
		wndclass.cbWndExtra = 0;
		wndclass.hInstance = hInstance;
		wndclass.hIcon = LoadIcon(hInstance, "minIcon");
		wndclass.hCursor = LoadCursor(0, IDC_ARROW);
		wndclass.hbrBackground = GetStockObject(WHITE_BRUSH);
		wndclass.lpszMenuName = NULL;
		wndclass.lpszClassName = "Persistence";

		if (!RegisterClass(&wndclass))
			return (0);
	}

	HWND hWnd = CreateWindow("Persistence", "Persistence", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, 0, 0, hInstance, NULL);
	if (!hWnd)
    	return (0);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	MSG msg;
	while(GetMessage(&msg, 0, 0, 0))
		DispatchMessage(&msg);

    return (msg.wParam);
}

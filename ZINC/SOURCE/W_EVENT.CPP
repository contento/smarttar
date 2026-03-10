//	Zinc Interface Library - W_EVENT.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "ui_evt.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

#if defined(WIN32)
UI_EVENT::UI_EVENT(EVENT_TYPE _type, HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam)
#elif defined(__BCPLUSPLUS__) | defined(__TCPLUSPLUS__)
#	if __BORLANDC__ >= 0x0410		// Set by Borland C++ version 3.1
UI_EVENT::UI_EVENT(EVENT_TYPE _type, HWND hWnd, UINT wMsg, UINT wParam, LONG lParam)
#	else
UI_EVENT::UI_EVENT(EVENT_TYPE _type, HWND hWnd, WORD wMsg, WORD wParam, LONG lParam)
#	endif
#else
UI_EVENT::UI_EVENT(EVENT_TYPE _type, HWND hWnd, WORD wMsg, WORD wParam,	LONG lParam)
#endif
	: type(_type)
{
	// Set up the Windows event message.
	rawCode = modifiers = 0;
	message.hwnd = hWnd;
	message.message = wMsg;
	message.wParam = wParam,
	message.lParam = lParam;

	// Check for mouse or keyboard information.
	if ((wMsg >= WM_MOUSEFIRST && wMsg <= WM_MOUSELAST) ||
		(wMsg >= WM_NCMOUSEMOVE && wMsg <= WM_NCMBUTTONDBLCLK))
	{
#if defined(WIN32)
		POINTS point = MAKEPOINTS(lParam);
		position.column = point.x;
		position.line = point.y;
#else
		position.column = LOWORD(lParam);
		position.line = HIWORD(lParam);
#endif
	}
	else if (wMsg >= WM_KEYFIRST && wMsg <= WM_KEYLAST)
	{
		key.shiftState = 0;
		rawCode = key.value = wParam;
		static BYTE keyState[256];
		GetKeyboardState((BYTE FAR *)&keyState);
		if (keyState[VK_SHIFT] & 0x80)
			key.shiftState |= S_SHIFT;
		if (keyState[VK_CONTROL] & 0x80)
			key.shiftState |= S_CTRL;
		if (keyState[VK_MENU] & 0x80)
			key.shiftState |= S_ALT;
		if (keyState[VK_NUMLOCK] & 0x80)
			key.shiftState |= S_NUM_LOCK;
		if (keyState[VK_CAPITAL] & 0x80)
			key.shiftState |= S_CAPS_LOCK;
		if (keyState[VK_INSERT] & 0x80)
			key.shiftState |= S_INSERT;
		key.value |= key.shiftState << 8;
	}
}



//	Zinc Interface Library - I_WCCAT.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include <conio.h>
#include <signal.h>
#include <dos.h>
#include <graph.h>
#include <stdlib.h>
#define USE_RAW_KEYS
#include "ui_evt.hpp"

// ---- UI_TEXT_DISPLAY ----
static int _originalMode = -1;

#define OutPortByte 		outp
#define MKFP(s,o)	    	(((long)s << 16) | o)
#define MDA_SEGMENT			0xb000
#define MDA_LIN_SEGMENT		(char *)((MDA_SEGMENT) << 4)
#define COLOR_SEGMENT		0xb800
#define COLOR_LIN_SEGMENT	(char *)((COLOR_SEGMENT) << 4)

static char *_videoSegment;
static int _videoWidth;
static int _originalCursor;

void I_ScreenOpen(int *mode, int *lines, int *columns)
{
	union REGS inregs, outregs;

	// Save original text mode.
	struct videoconfig textInfo;
	if (_originalMode == -1)
	{
		_getvideoconfig(&textInfo);
		_originalMode = textInfo.mode;
	}
	if (*mode == TDM_43x80 &&
		_originalMode != 0 && _originalMode != 2 && _originalMode != 7)
	{
		_setvideomoderows(_DEFAULTMODE, 43);
	}
	else if (*mode == TDM_25x80 || *mode == TDM_25x40)
		_setvideomoderows(*mode, 25);
	else if (*mode != TDM_AUTO)
		_setvideomode(*mode);

	// Check for special text mode monitors.
	_getvideoconfig(&textInfo);
	*mode = textInfo.mode;
	*lines = textInfo.numtextrows;
	*columns = textInfo.numtextcols;

	_videoSegment = (*mode == 7) ? MDA_LIN_SEGMENT : COLOR_LIN_SEGMENT;
	_videoWidth = textInfo.numtextcols * 2;

	// Hide the text cursor;
	_originalCursor = _gettextcursor();
	I_CursorRemove();
	// Turn the blink attribute off.
	inregs.w.ax = 0x1003;
	inregs.h.bl = 0x0000;

#ifdef __386__
	int386(0x10, &inregs, &outregs);
#else
	int86(0x10, &inregs, &outregs);
#endif
}

void I_ScreenClose(void)
{
	// Turn the blink attribute back on.
	union REGS inregs, outregs;
	inregs.w.ax = 0x1003;
	inregs.h.bl = 0x01;
#ifdef __386__
	int386(0x10, &inregs, &outregs);
#else
	int86(0x10, &inregs, &outregs);
#endif

	_settextcursor(_originalCursor);
	_setvideomode(_originalMode);
	_clearscreen(_GCLEARSCREEN);
}

void I_ScreenPut(int left, int top, int right, int bottom, void *_buffer)
{
	int y, x; // Warning, coordinates are 1 based.
	char *buffer = (char *)_buffer;
	for (y = top - 1; y < bottom; y++)
	{
		char *screen = _videoSegment + y * _videoWidth;
		for (x = left - 1; x < right; x++)
		{
			screen[x*2] = *buffer++;
			screen[x*2+1] = *buffer++;
		}
	}
}

void I_ScreenGet(int left, int top, int right, int bottom, void *_buffer)
{
	int y, x; // Warning, coordinates are 1 based.
	char *buffer = (char *)_buffer;
	for (y = top - 1; y < bottom; y++)
	{
		char *screen = _videoSegment + y * _videoWidth;
		for (x = left - 1; x < right; x++)
		{
			*buffer++ = screen[x*2];
			*buffer++ = screen[x*2+1];
		}
	}
}

void I_CursorRemove(void)
{
	_settextcursor(0x2000);
}

int I_ScreenColor(int color)
{
	return (color & 0x0f);
}

void I_CursorPosition(int y, int x, int val)
{
	_settextposition(y, x);
	_settextcursor((val == DC_INSERT) ? 0x0007 : 0x0707);
}

//		  Narrow, Wide
#if defined(OEM)
	ichar_t _iCornerUL[] = { 0xDA, 0xC9 };
	ichar_t _iCornerUR[] = { 0xBF, 0xBB };
	ichar_t _iCornerLL[] = { 0xC0, 0xC8 };
	ichar_t _iCornerLR[] = { 0xD9, 0xBC };
	ichar_t _iHorizontal[] = { 0xC4, 0xCD };
	ichar_t _iVertical[] = { 0xB3, 0xBA };
	ichar_t _iLowerBox = 0xdc;
	ichar_t _iUpperBox = 0xdf;
	ichar_t _iFullBox = 0xdb;
	ichar_t *_iSBMiddleArrow = "\xfe";
#else
	ichar_t _iCornerUL[] = { 0x8d, 0x9d };
	ichar_t _iCornerUR[] = { 0x85, 0x95 };
	ichar_t _iCornerLL[] = { 0x86, 0x96 };
	ichar_t _iCornerLR[] = { 0x8c, 0x9c };
	ichar_t _iHorizontal[] = { 0x8a, 0x9a };
	ichar_t _iVertical[] = { 0x83, 0x93 };
	ichar_t _iLowerBox = 0x91;
	ichar_t _iUpperBox = 0x90;
	ichar_t _iFullBox = 0x92;
	ichar_t *_iSBMiddleArrow = "\x9e";
#endif
ichar_t *_iSBUpArrow = "";
ichar_t *_iSBDownArrow = "";
ichar_t *_iSBLeftArrow = "";
ichar_t *_iSBRightArrow = "";
ichar_t *_iWDownArrow = "[]";
ichar_t *_iWNormalMin = "[]";
ichar_t *_iWMinimized = "[]";
ichar_t *_iWNormalMax = "[]";
ichar_t *_iWMaximized = "[]";
ichar_t *_iActiveCheck   = "[X] ";
ichar_t *_iInactiveCheck = "[ ] ";
ichar_t *_iActiveRadio   = "() ";
ichar_t *_iInactiveRadio = "( ) ";
ichar_t *_iCheckMark = "*";

// ---- UID_MOUSE ----
static unsigned _oldMouseMask = 0;
static unsigned _oldMouseState = 0;
static unsigned _mouseState = 0;
static int _horizontalMickeys = 0;
static int _verticalMickeys = 0;

#define mouseInt	0x33
#define mouseInit	0
#define mouseUninit	0
#define mouseMickeys	11
#define mouseSetFunction	12
#define mouseExgFunction	20
static char _stopInterrupt = FALSE;

#pragma off (check_stack)
void _loadds far MouseISR(int max, int mcx, int mdx)
{
#pragma aux MouseISR parm [EAX] [ECX] [EDX]
	// Get the mouse information.
	while (_stopInterrupt)
		;
	_stopInterrupt = TRUE;	// Prevent the interrupt from multiple calls.

	// Set up new mouse state. (Current state is in max)
	if (FlagSet(max, 0x10))
	{
		_oldMouseState = 0;
		_mouseState = M_RIGHT_CHANGE;
	}
	else if (FlagSet(max, 0x08))
	{
		_oldMouseState = M_RIGHT;
		_mouseState = M_RIGHT | M_LEFT_CHANGE;
	}
	else if (FlagSet(max, 0x04))
	{
		_oldMouseState = 0;
		_mouseState = M_LEFT_CHANGE;
	}
	else if (FlagSet(max, 0x02))
	{
		_oldMouseState = M_LEFT;
		_mouseState = M_LEFT | M_LEFT_CHANGE;
	}
	else if (FlagSet(max, 0x01))
		_mouseState = _oldMouseState;
	else
		_oldMouseState = _mouseState = 0;
#ifdef __386__
	_mouseState = _mouseState | (*((UCHAR *)MK_FP(0x34, 0x417)) & 0xF);
#else
	_mouseState = _mouseState | (*((UCHAR _far *)MKFP(0x40, 0x17)) & 0xF);
#endif

	// Compute the mouse movement.
	static int _hcx = 0xFFFF;
	static int _hdx = 0xFFFF;
	if (_hcx != 0xFFFF)
	{
		_horizontalMickeys += (mcx - _hcx);
		_verticalMickeys += (mdx - _hdx);
	}
	_hcx = mcx;
	_hdx = mdx;

	// Set the internal mouse information for use in the Poll routine.
	if (mouseQueueEvent)
		mouseQueueEvent(_mouseState, &_horizontalMickeys, &_verticalMickeys);
	_stopInterrupt = FALSE;
}
#pragma on (check_stack)

int I_MouseOpen(void)
{
	union REGS regs;
	struct SREGS segregs;

	// Check for mouse driver installed.
	regs.w.ax = 0;
#ifdef __386__
	int386(mouseInt, &regs, &regs);
#else
	int86(mouseInt, &regs, &regs);
#endif

	if (regs.w.ax != 0xFFFF)
		return FALSE;

    segread(&segregs);
	regs.w.ax = mouseSetFunction;
	regs.w.cx = 0xFF;
	regs.x.edx = FP_OFF(MouseISR);
	segregs.es = FP_SEG(MouseISR);
#ifdef __386__
	int386x(mouseInt, &regs, &regs, &segregs);
#else
	int86x(mouseInt, &regs, &regs, &segregs);
#endif
	return TRUE;
}

void I_MouseClose(void)
{
	union REGS regs;

	regs.w.ax = mouseUninit;
#ifdef __386__
	int386(mouseInt, &regs, &regs);
#else
	int86(mouseInt, &regs, &regs);
#endif
}

void I_MouseHalt(int halt)
{
	_stopInterrupt = halt;
}

// ---- UID_KEYBOARD ----
static int _enhancedBios = 0;		// 0 or 0x10 (keyboard read)
static int _oldBreakState = 0;
static int sendBreak = 0;							// BUG.internal

void __interrupt __far CtrlCHandler(void)
{
	if (UID_KEYBOARD::breakHandlerSet == 0)
		exit(1);
	sendBreak = 1;
}

void I_KeyboardOpen(void)
{
	union REGS inregs, outregs;

	// Check for enhaced keyboard by calling extended shift status function.
	inregs.w.ax = 0x12FF;
#ifdef __386__
	int386(0x16, &inregs, &outregs);
#else
	int86(0x16, &inregs, &outregs);
#endif
	if (outregs.h.al != 0xFF)
	{
		_enhancedBios = 0x10;
	}
	else
	{
		// Double check for enhanced keyboard BIOS according to the
		// IBM PS/2 Technical Reference manual, page 2-103
		inregs.h.ah = 0x05;
		inregs.w.cx = 0xFFFF;
#ifdef __386__
		int386(0x16, &inregs, &outregs);
#else
		int86(0x16, &inregs, &outregs);
#endif
		if (outregs.h.al == 0x00)
		{
			// success, carry on, read that guy back ...
			inregs.h.ah = 0x10;
			for (int bufferIndex = 0; bufferIndex < 16; bufferIndex++)
			{
#ifdef __386__
				int386(0x16, &inregs, &outregs);
#else
				int86(0x16, &inregs, &outregs);
#endif
				if (outregs.w.ax == 0xFFFF)
				{
					_enhancedBios = 0x10;
					break;
				}
			}
		}
	}
	// Set break interupt to ignore Control-Break/Control-C.
	_dos_setvect(0x23, CtrlCHandler);

	// Save previous state of Control-Break using INT 21H, 00H.
	inregs.w.ax = 0x3300;
#ifdef __386__
	int386(0x21, &inregs, &outregs);
#else
	int86(0x21, &inregs, &outregs);
#endif
	_oldBreakState = outregs.h.dl;

	// Set Control-Break ON using INT 21H, 01H.
	inregs.w.ax = 0x3301;
	inregs.h.dl = 0x01;
#ifdef __386__
	int386(0x21, &inregs, &outregs);
#else
	int86(0x21, &inregs, &outregs);
#endif
}

void I_KeyboardClose(void)
{
	union REGS inregs, outregs;

	// Set Control-Break status to previous status.
	inregs.w.ax = 0x3301;
	inregs.h.dl = _oldBreakState;
#ifdef __386__
	int386(0x21, &inregs, &outregs);
#else
	int86(0x21, &inregs, &outregs);
#endif
}

void I_KeyboardRead(unsigned *rawCode, unsigned *shiftState, unsigned *value)
{
	union REGS inregs, outregs;

	if (sendBreak)
	{
		*rawCode = CTRL_C;
		*shiftState = 0;
		*value = 0x3;
		sendBreak = 0;
		return;
	}
	inregs.h.ah = _enhancedBios + 0x00;
#ifdef __386__
	int386(0x16, &inregs, &outregs);
#else
	int86(0x16, &inregs, &outregs);
#endif
	*rawCode = outregs.w.ax;
	*value = outregs.h.al;

	// Get the shift state using INT 16H, 12H(or 02H if not enhanced).
	inregs.h.ah = _enhancedBios + 0x02;
#ifdef __386__
	int386(0x16, &inregs, &outregs);
#else
	int86(0x16, &inregs, &outregs);
#endif
	*shiftState = outregs.h.al;
	if (_enhancedBios && (outregs.w.ax & 0x0800) && *value != 0)
		*shiftState &= ~0x08;
}

int I_KeyboardQuery(unsigned *shiftState)
{
	union REGS inregs, outregs;

	if (sendBreak)
		return (TRUE);
	// Test if keystroke waiting.
	if (!kbhit())
	{
		// No keystroke waiting.
		// Check for ALT key using INT 16H, 02H.
		inregs.h.ah = _enhancedBios + 0x02;
#ifdef __386__
		int386(0x16, &inregs, &outregs);
#else
		int86(0x16, &inregs, &outregs);
#endif
		*shiftState = outregs.h.al;
		return FALSE;
	}
	return TRUE;
}

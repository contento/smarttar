//	Zinc Interface Library - I_MSCAT.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include <conio.h>
#include <signal.h>
#include <dos.h>
#include <graph.h>
#include <stdlib.h>
#if defined(DOSX286)
#include <phapi.h>
#endif
#include "ui_evt.hpp"
#pragma hdrstop					// Microsoft pre-compiled header pragma.

#if defined(DOSX286)
#	define _int86	int86
#	define _int86x	int86x
#	define _REGS	REGS
#	define _SREGS	SREGS
#endif
// ---- UI_TEXT_DISPLAY ----
static int _originalMode = -1;

#define OutPortByte _outp
#define MKFP(s,o)	    (((long)s << 16) | o)
#define MDA_SEGMENT		0xb000
#define COLOR_SEGMENT	0xb800

static unsigned short _videoSegment;
static int _videoWidth;
static int _originalCursor;

void I_ScreenOpen(int *mode, int *lines, int *columns)
{
	union _REGS inregs, outregs;

	// Save original text mode.
	struct _videoconfig textInfo;
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

	_videoSegment = (*mode == 7) ? MDA_SEGMENT : COLOR_SEGMENT;
#if defined(DOSX286)
	DosMapRealSeg(_videoSegment, (long) 0xFFFF, &_videoSegment);
#endif
	_videoWidth = textInfo.numtextcols * 2;

	// Hide the text cursor;
	_originalCursor = _gettextcursor();
	I_CursorRemove();
	// Turn the blink attribute off.
	inregs.x.ax = 0x1003;
	inregs.h.bl = 0x0000;
	_int86(0x10, &inregs, &outregs);

	UCHAR newSetting = *((UCHAR _far *)MKFP(0x40, 0x65));
	newSetting &= ~0x20;
	OutPortByte(*((USHORT _far *)MKFP(0x40, 0x63)) + 4, newSetting);
	*((UCHAR _far *)MKFP(0x40, 0x65)) = newSetting;		// Update BIOS data area
}

void I_ScreenClose(void)
{
	// Turn the blink attribute back on.
	union _REGS inregs, outregs;
	inregs.x.ax = 0x1003;
	inregs.h.bl = 0x01;
	_int86(0x10, &inregs, &outregs);

	_settextcursor(_originalCursor);
	_setvideomode(_originalMode);
	_clearscreen(_GCLEARSCREEN);
}

void I_ScreenPut(int left, int top, int right, int bottom, void *_buffer)
{
	USHORT far *buffer = (USHORT far *)_buffer;
	int y, x;
	int width = (right - left + 1) * 2;
	unsigned videoOffset = (top - 1) * _videoWidth + (left - 1) * 2;
		
	for (y = top; y <= bottom; y++)
	{
		for (x = 0; x < width; x += 2)
			*((USHORT far*)MKFP(_videoSegment, videoOffset + x)) = *buffer++;
		videoOffset += _videoWidth;
	}
}

void I_ScreenGet(int left, int top, int right, int bottom, void *_buffer)
{
	USHORT far *buffer = (USHORT far *)_buffer;
	int y, x;
	int width = (right - left + 1) * 2;
	unsigned videoOffset = (top - 1) * _videoWidth + (left - 1) * 2;
	for (y = top; y <= bottom; y++)
	{
		for (x = 0; x < width; x += 2)
			*buffer++ = *((USHORT far*)MKFP(_videoSegment, videoOffset + x));
		videoOffset += _videoWidth;
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
static unsigned char _oldMouseState = 0;
static unsigned _mouseState = 0;
static int _horizontalMickeys = 0;
static int _verticalMickeys = 0;

#define mouseInt	0x33
#define mouseInit	0
#define mouseUninit	0
#define mouseMickeys	11
#define mouseSetFunction	12
#define mouseExgFunction	20
static int _stopInterrupt = FALSE;

#pragma check_stack (off)

static char _mouseStack[2048];
static unsigned _oldSP = 0;
static unsigned _oldSS = 0;

static void __loadds RealMouseISR(void)
{
	unsigned char _BL;

	__asm	mov _BL, bl

	union _REGS regs;

	// Get the mouse information.
	if (!_stopInterrupt)
	{
		_stopInterrupt = TRUE;	// Prevent the interrupt from multiple calls.

		// Set up new mouse state. (Current state is in _BL)
		_mouseState = ((((_oldMouseState ^ _BL) << 4) | _BL) << 8) | (*((UCHAR _far *)MKFP(0x40, 0x17)) & 0xF);
		_oldMouseState = _BL;

		// Compute the mouse movement.
		regs.x.ax = mouseMickeys;
		_int86(mouseInt, &regs, &regs);
		_horizontalMickeys += regs.x.cx;
		_verticalMickeys += regs.x.dx;

		// Set the internal mouse information for use in the Poll routine.
		if (mouseQueueEvent)
			mouseQueueEvent(_mouseState, &_horizontalMickeys, &_verticalMickeys);
		_stopInterrupt = FALSE;
	}
}

extern "C" void MouseISR(void)
{
	__asm	push	ds
	RealMouseISR();
	__asm	pop	ds
}

int I_MouseOpen(void)
{
	union _REGS regs;
	struct _SREGS segregs;

	regs.x.ax = 0;
	_int86(mouseInt, &regs, &regs);
	if (regs.x.ax != 0xFFFF)
		return FALSE;
	segregs.ds = 0;
	regs.x.cx = 0xFF;
	regs.x.dx = (unsigned)MouseISR;
	segregs.es = (unsigned)((long)MouseISR >> 16);
	regs.x.ax = mouseSetFunction;
	_int86x(mouseInt, &regs, &regs, &segregs);
	return TRUE;
}

void I_MouseClose(void)
{
	union _REGS regs;

	regs.x.ax = mouseUninit;
	_int86(mouseInt, &regs, &regs);
}

void I_MouseHalt(int halt)
{
	_stopInterrupt = halt;
}

// ---- UID_KEYBOARD ----
static int _enhancedBios = 0;		// 0 or 0x10 (keyboard read)
static int _oldBreakState = 0;

void __interrupt __far CtrlCHandler(void)
{
	if (UID_KEYBOARD::breakHandlerSet == 0)
		exit(1);
}

void I_KeyboardOpen(void)
{
	union _REGS inregs, outregs;

	// Check for enhaced keyboard by calling extended shift status function.
	inregs.x.ax = 0x12FF;
	_int86(0x16, &inregs, &outregs);
	if (outregs.h.al != 0xFF)
	{
		_enhancedBios = 0x10;
	}
	else
	{
		// Double check for enhanced keyboard BIOS according to the
		// IBM PS/2 Technical Reference manual, page 2-103
		inregs.h.ah = 0x05;
		inregs.x.cx = 0xFFFF;
		_int86(0x16, &inregs, &outregs);
		if (outregs.h.al == 0x00)
		{
			// success, carry on, read that guy back ...
			inregs.h.ah = 0x10;
			for (int bufferIndex = 0; bufferIndex < 16; bufferIndex++)
			{
				_int86(0x16, &inregs, &outregs);
				if (outregs.x.ax == 0xFFFF)
				{
					_enhancedBios = 0x10;
					break;
				}
			}
		}
	}
	// Set break interupt to ignore Control-Break/Control-C.
#ifdef DOSX286
	PIHANDLER tmp1;
	REALPTR tmp2;
	DosSetPassToProtVec(0x23, (PIHANDLER)CtrlCHandler, &tmp1, &tmp2);
#else
	_dos_setvect(0x23, CtrlCHandler);
#endif
	// Save previous state of Control-Break using INT 21H, 00H.
	inregs.x.ax = 0x3300;
	_int86(0x21, &inregs, &outregs);
	_oldBreakState = outregs.h.dl;

	// Set Control-Break ON using INT 21H, 01H.
	inregs.x.ax = 0x3301;
	inregs.h.dl = 0x01;
	_int86(0x21, &inregs, &outregs);
}

void I_KeyboardClose(void)
{
	union _REGS inregs, outregs;

	// Set Control-Break status to previous status.
	inregs.x.ax = 0x3301;
	inregs.h.dl = _oldBreakState;
	_int86(0x21, &inregs, &outregs);
}

void I_KeyboardRead(unsigned *rawCode, unsigned *shiftState, unsigned *value)
{
	union _REGS inregs, outregs;

	inregs.h.ah = _enhancedBios + 0x00;
	_int86(0x16, &inregs, &outregs);
	*rawCode = outregs.x.ax;
	*value = outregs.h.al;

	// Get the shift state using INT 16H, 12H(or 02H if not enhanced).
	inregs.h.ah = _enhancedBios + 0x02;
	_int86(0x16, &inregs, &outregs);
	*shiftState = outregs.h.al;
	if (_enhancedBios && (outregs.x.ax & 0x0800) && *value != 0)
		*shiftState &= ~0x08;
}

int I_KeyboardQuery(unsigned *shiftState)
{
	union _REGS inregs, outregs;

	// Test if keystroke waiting.
	USHORT flags;
	inregs.h.ah = _enhancedBios + 0x01;
	__asm	mov		ah, inregs.h.ah
	__asm	int		0x16
	__asm	pushf
	__asm	pop		ax
	__asm	mov		flags, ax
	if (flags & 0x40)
	{
		// No keystroke waiting.
		// Check for ALT key using INT 16H, 02H.
		inregs.h.ah = _enhancedBios + 0x02;
		_int86(0x16, &inregs, &outregs);
		*shiftState = outregs.h.al;
		return FALSE;
	}
	return TRUE;
}



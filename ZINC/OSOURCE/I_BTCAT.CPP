//	Zinc Interface Library - I_BTCAT.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

// Warning!!!  geninterrupt() generates different code depending on
// whether the parameter is a small constant, large constant, or variable.
// This difference can cause some of this module to break, especially
// where ES and AX are used.  Change carefully.

#include <signal.h>
#include <dos.h>
#include <mem.h>
#include <stdlib.h>
#ifdef DOSX286
#include <phapi.h>
#endif
#include "ui_evt.hpp"

// ---- UI_TEXT_DISPLAY ----
static int _originalMode = -1;
static void *_screenAddress = NULL;
static int _columns = 0;
static int _lines = 0;

void I_ScreenOpen(int *mode, int *lines, int *columns)
{
	union REGS regs;

	// Save original text mode.
	if (_originalMode == -1)
	{
		regs.h.ah = 0x0f;
		int86(0x10, &regs, &regs);
		_originalMode = regs.h.al;
	}

	// Initialize the text display.
	if (*mode != TDM_AUTO)
	{
		// Set the mode
		regs.h.ah = 0x00;
		regs.h.al = *mode;
		int86(0x10, &regs, &regs);
		// Check for special text mode monitors.
		regs.h.ah = 0x0f;
		int86(0x10, &regs, &regs);
		*mode = regs.h.al;
	}
	else
		*mode = _originalMode;
	_lines = *lines = *((UCHAR _far *)MK_FP(0x40, 0x84)) + 1;
	_columns = *columns = *((UCHAR _far *)MK_FP(0x40, 0x4a));

	// Alternate screen buffer ????
#if defined(DOSX286)
	REGS16 rRegs;
	rRegs.ax = 0xfe00;
	rRegs.di = 0x0000;
	rRegs.es = (*mode == 0x07 ? 0xb000 : 0xb800);
//	DosRealIntr(0x10, &rRegs, 0L, 0);
	unsigned short sel;
	DosMapRealSeg(rRegs.es, (long)_lines * _columns * 2, &sel);
	_screenAddress = MK_FP(sel, rRegs.di);
#else
	struct SREGS segs;
	regs.h.ah = 0xfe;
	regs.x.di = 0x0000;
	segs.es = (*mode == 0x07 ? 0xb000 : 0xb800);
	int86x(0x10, &regs, &regs, &segs);
	_screenAddress = MK_FP(segs.es, regs.x.di);
#endif
	I_CursorRemove();

	// Turn the blink attribute off.
	regs.x.ax = 0x1003;
	regs.h.bl = 0x00;
	int86(0x10, &regs, &regs);

	UCHAR newSetting = *((UCHAR _far *)MK_FP(0x40, 0x65));
	newSetting &= ~0x20;
	USHORT addr = *((USHORT _far *)MK_FP(0x40, 0x63)) + 4;
	outportb(addr, newSetting);
	// Update BIOS data area
	*((UCHAR _far *)MK_FP(0x40, 0x65)) = newSetting;
}

void I_ScreenClose(void)
{
	union REGS regs;

	// Turn the blink attribute back on.
	regs.x.ax = 0x1003;
	regs.h.bl = 0x01;
	int86(0x10, &regs, &regs);

	regs.h.ah = 0x00;
	regs.h.al = _originalMode;
	int86(0x10, &regs, &regs);
	regs.h.ah = 0x01;
	regs.x.cx = (_originalMode == 0x07 ? 0x0b0c : 0x0607);
	int86(0x10, &regs, &regs);
	regs.h.ah = 0x06;
	regs.h.al = 0x00;
	regs.h.bh = (_originalMode == 0x07 ? 0x0b0c : 0x0607);
	regs.x.cx = 0x0000;
	regs.x.dx = (_originalMode == 0x07 ? 0x1850 : 0x1850);
	int86(0x10, &regs, &regs);
}

void I_ScreenPut(int left, int top, int right, int bottom, void *buffer)
{
	int lineLen = 2*(right-left+1);
	for (int i=top-1, j=0; i < bottom; i++, j++)
		memcpy((short *)_screenAddress + i * _columns + left - 1,
		       (short *)buffer + j * _columns, lineLen);
}

void I_ScreenGet(int left, int top, int right, int bottom, void *buffer)
{
	int lineLen = 2*(right-left+1);
	for (int i=top-1, j=0; i < bottom; i++, j++)
		memcpy((short *)buffer + j * _columns,
		       (short *)_screenAddress + i * _columns + left - 1, lineLen);
}

int I_ScreenColor(int color)
{
	return (color & 0x0f);
}

void I_CursorRemove(void)
{
	union REGS regs;

	regs.h.ah = 0x02;
	regs.h.bh = 0;
	regs.h.dh = _lines;
	regs.h.dl = 0;
	int86(0x10, &regs, &regs);
}

void I_CursorPosition(int y, int x, int doInsert)
{
	union REGS regs;

	regs.h.ah = 0x02;
	regs.h.bh = 0;
	regs.h.dl = x-1;
	regs.h.dh = y-1;
	int86(0x10, &regs, &regs);
	regs.h.ah = 0x01;
	regs.x.cx = (doInsert ? 
		       (_originalMode == 0x07 ? 0x000c : 0x0007) :
		       (_originalMode == 0x07 ? 0x0b0c : 0x0607));
	int86(0x10, &regs, &regs);
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

#pragma inline
static unsigned _oldMouseSegment = 0;
static unsigned _oldMouseOffset = 0;

static void MouseISR(void)
{
	// Get the proper value to ds.
	asm push ds
	asm mov ax, DGROUP
	asm mov ds, ax

#if defined(DOSX286)
	REGS16 regs;
#else
	union REGS regs;
#endif
	
	// Get the mouse information.
	if (!_stopInterrupt)
	{
		_stopInterrupt = TRUE;	// Prevent the interrupt from multiple calls.

		// Set up new mouse state. (Current state is in _BL)
		_mouseState = ((((_oldMouseState ^ _BL) << 4) | _BL) << 8) | (*((UCHAR _far *)MK_FP(0x40, 0x17)) & 0xF);
		_oldMouseState = _BL;

		// Compute the mouse movement.
#ifdef  DOSX286
		// We can't call int86(0x33,,) here; we blow up in the extender.  
		// Instead, we simply issue the interrupt in real mode and 
		// shuttle the results back up to protected mode.

		memset(&regs, 0, sizeof(REGS16));
		regs.ax = mouseMickeys;
		DosRealIntr(mouseInt, &regs, 0L, 0);
		_horizontalMickeys += regs.cx;
		_verticalMickeys += regs.dx;
#else
		regs.x.ax = mouseMickeys;
		int86(mouseInt, &regs, &regs);
		_horizontalMickeys += regs.x.cx;
		_verticalMickeys += regs.x.dx;
#endif
		// Set the internal mouse information for use in the Poll routine.
		if (mouseQueueEvent)
			mouseQueueEvent(_mouseState, &_horizontalMickeys, &_verticalMickeys);
		_stopInterrupt = FALSE;
	}

	// Restore the ds register.
	asm pop ds
}

int I_MouseOpen(void)
{
#if !defined(DOSX286)
	unsigned char far *far *mouseVector = (unsigned char far *far *) (4 * mouseInt);
	if (*mouseVector && **mouseVector != 0xCF)
	{
		_AX = mouseExgFunction;
		_BX = 0;
		_DX = 0;
		_CX = 0;
		geninterrupt(mouseInt);
		_BX = _ES;
		_oldMouseMask = _CX;
		_oldMouseSegment = _BX;
		_oldMouseOffset = _DX;
	}
#endif
	_AX = 0;
	geninterrupt(mouseInt);
	if (_AX != 0xFFFF)
		return FALSE;
	_CX = 0xFF;
	_DX = FP_OFF(MouseISR);
	_AX = FP_SEG(MouseISR);
	_ES = _AX;
	_AX = mouseSetFunction;
	geninterrupt(mouseInt);
	return TRUE;
}

void I_MouseClose(void)
{
#if !defined(DOSX286)
	_CX = _oldMouseMask;
	_DX = _oldMouseOffset;
	_BX = _oldMouseSegment;
	_ES = _BX;
	_AX = mouseExgFunction;
	geninterrupt(mouseInt);
#endif
	_AX = mouseUninit;
	geninterrupt(mouseInt);
}

void I_MouseHalt(int halt)
{
	_stopInterrupt = halt;
}

// ---- UID_KEYBOARD ----
static int _enhancedBios = 0;		// 0 or 0x10 (keyboard read)
static int _oldBreakState = 0;

static int CtrlCHandler(void)
{
	if (UID_KEYBOARD::breakHandlerSet == 0)
		exit(1);
	return 1;
}

void I_KeyboardOpen(void)
{
	union REGS inregs, outregs;

	// Check for enhaced keyboard by calling extended shift status function.
	inregs.x.ax = 0x12FF;
	int86(0x16, &inregs, &outregs);
	if (outregs.h.al != 0xFF)
	{
		_enhancedBios = 0x10;
		return;
	}
	// Double check for enhanced keyboard BIOS according to the
	// IBM PS/2 Technical Reference manual, page 2-103
	inregs.h.ah = 0x05;
	inregs.x.cx = 0xFFFF;
	int86(0x16, &inregs, &outregs);
	if (outregs.h.al != 0x00)
		return;
	// success, carry on, read that guy back ...
	inregs.h.ah = 0x10;
	for (int bufferIndex = 0; bufferIndex < 16; bufferIndex++)
	{
		int86(0x16, &inregs, &outregs);
		if (outregs.x.ax == 0xFFFF)
		{
			_enhancedBios = 0x10;
			break;
		}
	}
	// Set break interupt to ignore Control-Break/Control-C.
	ctrlbrk(CtrlCHandler);
	// Save previous state of Control-Break using INT 21H, 00H.
	inregs.x.ax = 0x3300;
	int86(0x21, &inregs, &outregs);
	_oldBreakState = outregs.h.dl;

	// Set Control-Break ON using INT 21H, 01H.
	inregs.x.ax = 0x3301;
	inregs.h.dl = 0x01;
	int86(0x21, &inregs, &outregs);
}

void I_KeyboardClose(void)
{
	union REGS inregs, outregs;

	// Set Control-Break status to previous status.
	inregs.x.ax = 0x3301;
	inregs.h.dl = _oldBreakState;
	int86(0x21, &inregs, &outregs);
}

void I_KeyboardRead(unsigned *rawCode, unsigned *shiftState, unsigned *value)
{
	union REGS inregs, outregs;

	inregs.h.ah = _enhancedBios + 0x00;
	int86(0x16, &inregs, &outregs);
	*rawCode = outregs.x.ax;
	*value = outregs.h.al;

	// Get the shift state using INT 16H, 12H(or 02H if not enhanced).
	inregs.h.ah = _enhancedBios + 0x02;
	int86(0x16, &inregs, &outregs);
	*shiftState = outregs.h.al;
	if (_enhancedBios && (outregs.x.ax & 0x0800) && *value != 0)
		*shiftState &= ~0x08;
}

int I_KeyboardQuery(unsigned *shiftState)
{
	union REGS inregs, outregs;

	// Test if keystroke waiting.
	inregs.h.ah = _enhancedBios + 0x01;
	int86(0x16, &inregs, &outregs);
	if (outregs.x.flags & 0x40)
	{
		// No keystroke waiting.
		// Check for ALT key using INT 16H, 02H.
		inregs.h.ah = _enhancedBios + 0x02;
		int86(0x16, &inregs, &outregs);
		*shiftState = outregs.h.al;
		return FALSE;
	}
	return TRUE;
}



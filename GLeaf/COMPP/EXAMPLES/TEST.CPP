//*
//*  EXAMP001.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1989-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example demonstrates the GFBIOS::GFBIOS() constructor and the
//*  GFBIOS::~GFBIOS() destructor.
//*
#include "compplib.hpp"
#include "ibmkeys.h"
#include <iostream.h>

void main( void )
{
	int c;
	GFI8250 *sio;
	GFSerial *cp;

	sio = new GFI8250( COM2);//0x2F8, IRQ3);
	cp = new GFSerial( sio );
	for( ; ; ) {
		if( gfkbhit() ) {
			c = getkey();
			if( c == ESC ) {
				delete cp;
				delete sio;
				return;
			}
			else
				cp->PutChar( (unsigned char)c );
		}
		c = cp->GetChar();
		if( c != GCPP_TIMEOUT )
			cout << (char)c << flush;
	}
}

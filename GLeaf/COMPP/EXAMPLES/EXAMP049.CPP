//*
//*  EXAMP049.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1989-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example demonstrates the GFSerial::TXFlush(), GFSerial::RXFlush(),
//*  the GFSerial::operator << (const GCPP_BYTE *s), and
//*  GFSerial::PutString() functions.  We transmit strings of data to be
//*  echoed back to the receive buffer.  We then flush the first 20
//*  characters in the receive buffer.  To test the transmit flush, we
//*  place a string in the buffer and then delete all data remaining in
//*  the buffer.
//*
#include "compplib.hpp"
#include "ibmkeys.h"
#include <iostream.h>

unsigned char string[] = {"This is a small buffer of data to transmit."};

void main( void )
{
  int c;

#ifdef GCPP_OS2
  GFComOS2 sio( COM2 );
#else
  GFI8250 sio( COM2 );
#endif
  GFSerial cp( &sio );
  for(;;) {
    if( gfkbhit() ) {
      c = getkey();
      if( c == ESC )
        return;
      else if( c == F1 ) {
        cp.PutString( string );
        UseSysTime( &cp, 5 );
        cp.RXFlush( 20 );
      }
      else if( c == F2 ) {
        cp <<  string;
        cp.TXFlush();
      }
      else if( c == F3 )
        cp.PutString( string, -3 );
      else if( c == F4 )
        cp.PutString( string, -2 );
      else if( c == F5 )
        cp.PutString( string, 0x07 );
      else
        cp << c;
    }
    cp >> c;
    if( c != GCPP_TIMEOUT )
      cout << (char)c << flush;
  }
} 

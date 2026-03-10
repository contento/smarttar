//*
//*  EXAMP025.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1989-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example demonstrates the use of the GFLineStatus::SetHandShake()
//*  and GFLineStatus::GetHandShake() functions.  The user can poll the
//*  system for the current method of handshaking by pressing the F1 key.
//*  F3 will disable all handshaking.  F5 will enable DTR/DSR handshaking.
//*  F7 will enable RTS/CTS handshaking.
//*
#include "compplib.hpp"
#include "ibmkeys.h"
#include <iostream.h>

void main( void )
{
  int c;
  int in, out;

#ifdef GCPP_OS2
  GFComOS2 sio( COM2 );
#else
  GFI8250 sio( COM2, 0x2f8, IRQ3, 300, 100 );
#endif
  GFSerial cp( &sio );
  GFLineStatus ls( &cp );
  for(;;) {
    if( gfkbhit() ) {
      c = getkey();
      if( c == ESC )
        return;
      else if( c == F1 ) {
        ls.GetHandShake( in, out );
        switch( in ) {
          case GCPP_NONE:
            cout << "\nNo handshaking being used.\n" << flush;
            break;
          case XONXOFF:
            cout << "\nXONXOFF handshaking being used.\n" << flush;
            break;
          case DTR:
            cout << "\nDTR/DSR handshaking being used.\n" << flush;
            break;
          case RTS:
            cout << "\nRTS/CTS handshaking begin used.\n" << flush;
            break;
        }
      }
      else if( c == F3 )
        ls.SetHandShake( GCPP_NONE, GCPP_NONE );
      else if( c == F5 )
        ls.SetHandShake( DTR, DSR );
      else if( c == F7 )
        ls.SetHandShake( RTS, CTS );
      else if( c == F9 )
        ls.SetHandShake( XONXOFF, XONXOFF );
      else if( c == F2 ) {
        do {
          if( gfkbhit() ) {
            c = getkey();
            cout << "\nThere are " << (unsigned)(cp.RXCount()) <<
                " characters in the RX buffer" << flush;
          }
        } while( c != ESC );
      }
      else
        cp << c;
    }
    cp >> c;
    if( c != GCPP_TIMEOUT )
      cout << (char)c << flush;
  }
} 

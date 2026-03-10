//*
//*  EXAMP052.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1989-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example demonstrates the GFSerial::SetRXBuffer() and
//*  GFSerial::SetTXBuffer() functions.
//*
#include "compplib.hpp"
#include "ibmkeys.h"
#include <iostream.h>

void main( void )
{
  int c;
  int size;
  int status;

#ifdef GCPP_OS2
  GFComOS2 sio( COM2 );
#else
  GFI8250 sio( COM2, 0x2f8, IRQ3, 300, 300 );
#endif
  GFSerial cp( &sio );
  for(;;) {
    if( gfkbhit() ) {
      c = getkey();
      if( c == ESC )
        return;
      else if( c == F1 ) {
        cout << "\nMake the RXbuffer how big(1-30000)?: ";
        cin >> size;
        if( size > 0 && size < 30001 ) {
          status = cp.SetRXBuffer( size, 30 );
          cout << "The function status was " << status << flush;
        }
      }
      else if( c == F2 ) {
        cout << "\nMake the TXbuffer how big(1-20000)? ";
        cin >> size;
        if( size > 0 && size < 20001 ) {
          status = cp.SetTXBuffer( size );
          cout << "The function status was " << status << flush;
        }
      }
      else
        cp << c;
    }
    cp >> c;
    if( c != GCPP_TIMEOUT )
      cout << (char)c << flush;
  }
} 

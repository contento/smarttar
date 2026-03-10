//*
//*  EXAMP033.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1989-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example demonstrates the GFDataFormat::BaudRate()
//*  function.  The F1 key will retrieve the current baud
//*  rate and print it to the screen.  The F2 key will allow
//*  the user to set the baud rate from a list of choices.
//*
#include "compplib.hpp"
#include "ibmkeys.h"
#include <iostream.h>

static long newrate[] =
    { 300,1200,2400,4800,9600,19200,38400L,57600L,115200L };

void main( void )
{
  int c;
  int bd;

#ifdef GCPP_OS2
  GFComOS2 sio( COM2 );
#else
  GFI8250 sio( COM2 );
#endif
  GFSerial cp( &sio );
  GFDataFormat df( &cp );
  for(;;) {
    if( gfkbhit() ) {
      c = getkey();
      if( c == ESC )
        return;
      else if( c == F1 )
        cout << "\nThe baud rate is " << df.BaudRate() << "\n" << flush;
      else if( c == F2 ) {
        cout << "\nSelect new baud rate:\n";
        for( bd = 0; bd < 9; bd++ )
          cout << bd << ". " << newrate[ bd ] << "\n";
        cin >> bd;
        if( bd >= 0 && bd < 9 ) {
          df.BaudRate( newrate[ bd ] );
          df.Install( &cp );
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

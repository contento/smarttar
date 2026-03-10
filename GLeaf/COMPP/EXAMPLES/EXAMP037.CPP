//*
//*  EXAMP037.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1989-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example demonstrates the GFDataFormat::StopBits()
//*  function.  The F1 key will retrieve the current number
//*  of stop bits and print it to the screen.  The F2 key
//*  will allow the user to toggle the number of stop bits
//*  between 1 and 2.
//*
#include "compplib.hpp"
#include "ibmkeys.h"
#include <iostream.h>

void main( void )
{
  int c;

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
        cout << "\nThe number of stop bits is " << df.StopBits() << "\n" << flush;
      else if( c == F2 ) {
        df.StopBits() == 1 ? df.StopBits( 2 ) : df.StopBits( 1 );
        df.Install( &cp );
      }
      else
        cp << c;
    }
    cp >> c;
    if( c != GCPP_TIMEOUT )
      cout << (char)c << flush;
  }
} 

//*
//*  EXAMP024.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1989-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example demonstrates the use of the
//*  GFLineStatus::FramingError(), GFLineStatus::ParityError(),
//*  GFLineStatus::HardwareOverrun() and GFDataFormat::Parity()
//*  functions.  By pressing the F1 key the system will poll
//*  the error functions and report the results.
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
  GFLineStatus ls( &cp );
  GFDataFormat df( &cp );
  df.Parity( 'E' );
  for(;;) {
    if( gfkbhit() ) {
      c = getkey();
      if( c == ESC )
        return;
      else if( c == F1 ) {
        ls.Status();
        cout << "\nA framing error " <<
            (ls.FramingError() ? "has" : "has not") << " occurred.";
        cout << "\nA parity error " <<
            (ls.ParityError() ? "has" : "has not") << " occurred";
        cout << "\nA hardware overrun " <<
            (ls.HardwareOverrun() ? "has" : "has not") << " occurred\n" <<
            flush;
      }
      else
        cp << c;
    }
    cp >> c;
    if( c != GCPP_TIMEOUT )
      cout << (char)c << flush;
  }
} 

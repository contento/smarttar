//*
//*  EXAMP021.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1989-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example demonstrates the GFLineStatus::Carrier(),
//*  GFLineStatus::Cts(), GFLineStatus::Dsr() and
//*  GFLineStatus::Status() functions.  When the user hits the
//*  F1 key the program polls the system for the current state
//*  of the lines.
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
  for(;;) {
    if( gfkbhit() ) {
      c = getkey();
      if( c == ESC )
        return;
      else if( c == F1 ) {
        ls.Status();
        cout << "\nThe carrier line is " <<
            (ls.Carrier() ? "set!" : "not set!");
        cout << "\nThe CTS line is " <<
            (ls.Cts() ? "set!" : "not set!");
        cout << "\nThe DSR line is " <<
            (ls.Dsr() ? "set!" : "not set!") << "\n" << flush;
      }
      else
        cp << c;
    }
    cp >> c;
    if( c != GCPP_TIMEOUT )
      cout << (char)c << flush;
  }
} 

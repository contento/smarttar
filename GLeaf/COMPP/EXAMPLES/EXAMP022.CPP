//*
//*  EXAMP022.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1989-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example demonstrates the use of the GFLineStatus::CarrierChanged(),
//*  GFLineStatus::CtsChanged(), and GFLineStatus::DsrChanged() functions.
//*  We call the line status function to initialize the system and then poll
//*  the system whenever the F1 key is hit.
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
        cout << "\nCD has " <<
          (ls.CarrierChanged() ? "been changed." :
          "not been changed");
        cout << "\nCTS has " <<
          (ls.CtsChanged() ? "been changed." :
          "not been changed");
        cout << "\nDSR has " <<
          (ls.DsrChanged() ? "been changed." :
          "not been changed") << "\n" << flush;
      }
      else
        cp << c;
    }
    cp >> c;
    if( c != GCPP_TIMEOUT )
      cout << (char)c << flush;
  }
} 

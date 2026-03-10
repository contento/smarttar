//*
//*  EXAMP056.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1989-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example demonstrates the GFModem::AutoOff() and
//*  GFModem::AutoOn() functions.  We let the user toggle
//*  the modem auto-answer capability on and off.
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
  GFModem md;
  md.WaitForOk( 800 );
  if( md.Install( &cp ) != GCPP_OK ) {
    cout << "\nError Setting Up Modem Class\n";
    return;
  }
  for(;;) {
    if( gfkbhit() ) {
      c = getkey();
      if( c == ESC )
        return;
      else if( c == F1 ) {
        cout << "\nTurn modem auto-answer off\n" << flush;
        md.AutoOff( );
      }
      else if( c == F2 ) {
        cout << "\nTurn modem auto-answer on\n" << flush;
        md.AutoOn( );
      }
      else
        cp << c;
    }
    cp >> c;
    if( c != GCPP_TIMEOUT )
      cout << (char)c << flush;
  }
} 

//*
//*  EXAMP059.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1989-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example demonstrates the GFModem::DialMode() function.
//*  The F2 and F3 keys can be used to change the dialing mode
//*  to Touch Tone or Pulse.
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
  md.FixedDelay( 800 );
  if( md.Install( &cp ) != GCPP_OK ) {
    cout << "\nError setting up modem class\n";
    return;
  }
  for(;;) {
    if( gfkbhit() ) {
      c = getkey();
      if( c == ESC )
        return;
      else if( c == F1 )
        md.Dial( "250-3778" );
      else if( c == F2 ) {
        if( md.DialMode( GCPP_TONE ) == GCPP_TONE )
          cout << "\nModem set up for touch tone dialing\n" << flush;
        else
          cout << "\nError setting modem dialing mode\n" << flush;
      }
      else if( c == F3 ) {
        if( md.DialMode( GCPP_PULSE ) == GCPP_PULSE )
          cout << "\nModem set up for pulse dialing\n" << flush;
        else
          cout << "\nError setting modem dialing mode\n" << flush;
      }
      else
        cp << c;
    }
    cp >> c;
    if( c != GCPP_TIMEOUT )
      cout << (char)c << flush;
  }
} 

//*
//*  EXAMP029.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1989-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example demonstrates the GFLineStatus::RingDetected()
//*  function.  The program executes through the main loop.
//*  Upon detection of a ring we will issue an ATA command
//*  to the modem to answer the phone.
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
      else
        cp << c;
    }
    cp >> c;
    if( c != GCPP_TIMEOUT )
      cout << (char)c << flush;
    ls.Status();
    if( ls.RingDetected() ) {
      cout << "\nThe phone is ringing!" << flush;
      cp << (const GCPP_BYTE *)"ATA";
    }
  }
} 

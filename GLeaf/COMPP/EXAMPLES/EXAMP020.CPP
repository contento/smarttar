//*
//*  EXAMP020.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1989-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example demonstrates the GFLineStatus::BufferOverflow()
//*  function.  When the user hits the F1 key we loop until the
//*  user hits the ESC key or a buffer overflow condition occurs.
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
  GFI8250 sio( COM2, 0x2f8, IRQ3, 200, 200 );
#endif
  GFSerial cp( &sio );
  GFLineStatus ls( &cp );
  for(;;) {
    if( gfkbhit() ) {
      c = getkey();
      if( c == ESC )
        return;
      else if( c == F1 ) {
        cout << "\nWaiting for buffer overflow!";
        cout << "\nHit ESC to continue.\n" << flush;
        do {
          if( gfkbhit() )
            c = getkey();
          ls.Status();
        } while( c != ESC && !ls.BufferOverflow() );
        if( ls.BufferOverflow() )
          cout << "\nA buffer overflow has occurred!" << flush;
        else
          cout << "\nNo buffer overflow has occurred!" << flush;
      }
      else
        cp << c;
    }
    cp >> c;
    if( c != GCPP_TIMEOUT )
      cout << (char)c << flush;
  }
} 

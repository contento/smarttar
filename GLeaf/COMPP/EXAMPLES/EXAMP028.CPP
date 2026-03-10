//*
//*  EXAMP028.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1989-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example demonstrates the GFLineStatus::Ring()
//*  function.  By hitting the F1 key the user will place
//*  the program in a ring detection loop.  We will exit
//*  from the loop upon detection of RI line going high.
//*
#include "compplib.hpp"
#include "ibmkeys.h"
#include <iostream.h>

void main( void )
{
  int c;
  int ring;

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
        cout << "\nWaiting for the phone to ring.\n" << flush;
        do {
          if( gfkbhit() )
            c = getkey();
          ls.Status();
          ring = ls.Ring();
        } while( c != ESC && ring != GCPP_ON );
        if( ring == GCPP_ON ) {
          cout << "Trying to answer the phone.\n" << flush;
          cp << (const GCPP_BYTE *)"ATA";
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

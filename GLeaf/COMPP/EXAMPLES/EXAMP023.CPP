//*
//*  EXAMP023.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1989-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example demonstrates the use of the GFLineStatus::Dtr()
//*  and GFLineStatus::Rts() functions.  The user can check the
//*  line states by pressing the F1 key.  The F2 key will toggle
//*  the DTR line OFF/ON.  The F3 key will toggle the RTS line OFF/ON.
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
        cout << "\nDTR is " << (ls.Dtr() ? "set." : "not set.");
        cout << "\nRTS is " << (ls.Rts() ? "set." : "not set.") <<
            "\n" << flush;
      }
      else if( c == F2 )
        ls.Dtr() ? ls.Dtr( GCPP_OFF ) : ls.Dtr( GCPP_ON );
      else if( c == F3 )
        ls.Rts() ? ls.Rts( GCPP_OFF ) : ls.Rts( GCPP_ON );
      else
        cp << c;
    }
    cp >> c;
    if( c != GCPP_TIMEOUT )
      cout << (char)c << flush;
  }
} 

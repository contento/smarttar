//*
//*  EXAMP026.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1989-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example demonstrates the GFLineStatus::LineBreak()
//*  function.  The F1 key will poll the system for the
//*  current state of the break condition.  The F2 key
//*  will toggle the breaking system ON/OFF.
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
  GFLineStatus ls( &sio );
  for(;;) {
    if( gfkbhit() ) {
      c = getkey();
      if( c == ESC )
        return;
      else if( c == F1 ) {
        cout << (ls.LineBreak() ? "\nIn a breaking condition" :
            "\nNot in a breaking condition.") << flush;
      }
      else if( c == F2 )
        (ls.LineBreak() ? ls.LineBreak( GCPP_OFF ) : ls.LineBreak( GCPP_ON ));
      else
        cp << c;
    }
    cp >> c;
    if( c != GCPP_TIMEOUT )
      cout << (char)c << flush;
  }
} 

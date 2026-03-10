//*
//*  EXAMP045.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1989-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example demonstrates the GFSerial::GetChar() and
//*  GFSerial::PutChar() functions.  These functions are
//*  used instead of the ">>" and "<<" stream operators.
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
  for(;;) {
    if( gfkbhit() ) {
      c = getkey();
      if( c == ESC )
        return;
      else
        cp.PutChar( (unsigned char)c );
    }
    c = cp.GetChar( 55 );
    if( c != GCPP_TIMEOUT )
      cout << (char)c << flush;
  }
} 

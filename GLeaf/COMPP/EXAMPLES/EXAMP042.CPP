//*
//*  EXAMP042.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1989-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example demonstrates the GFSerial::EchoBack() function.
//*
#include "compplib.hpp"
#include "ibmkeys.h"
#include <iostream.h>

void main( void )
{
  int c;
  int toggle = 0;

  GFI8250 sio( COM2 );
  GFSerial cp( &sio );

  for(;;) {
    if( gfkbhit() ) {
      c = getkey();
      if( c == ESC )
        return;
      else if( c == F1 ) {
        if( toggle ) {
          toggle = 0;
          cp.EchoBack( 0 );
        }
        else {
          toggle = 1;
          cp.EchoBack( 1 );
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

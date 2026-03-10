//*
//*  EXAMP008.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1989-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example demonstrates the GFComOS2::GFComOS2(), GFI8250::GFI8250()
//*  GFSerial::GFSerial() constructors, and the GFComOS2::~GFComOS2(),
//*  GFI8250::~GFI8250(), GFSerial::~GFSerial() destructors.
//*
#include "compplib.hpp"
#include "ibmkeys.h"
#include <iostream.h>

void main( void )
{
  int c;
  GFSerial *cp;

#ifdef GCPP_OS2
  GFComOS2 *sio = new GFComOS2( COM2 );
#else
  GFI8250 *sio = new GFI8250( COM2 );
#endif
  cp = new GFSerial( sio );
  for(;;) {
    if( gfkbhit() ) {
      c = getkey();
      if( c == ESC ) {
        delete cp;
        delete sio;
        return;
      }
      else
        *cp << c;
    }
    *cp >> c;
    if( c != GCPP_TIMEOUT )
      cout << (char)c << flush;
  }
} 

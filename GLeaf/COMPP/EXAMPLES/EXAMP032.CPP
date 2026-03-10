//*
//*  EXAMP032.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1989-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example demonstrates the GFDataFormat::GFDataFormat()
//*  copy constructor, the GFDataFormat::~GFDataFormat() destructor
//*  and the GFDataFormat::Parity() function.  The GFDataFormat
//*  constructor uses a GFDataFormat parameter.  The user can
//*  query the port parity by pressing the F1 key.
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
  GFDataFormat df( &cp );
  for(;;) {
    if( gfkbhit() ) {
      c = getkey();
      if( c == ESC )
        return;
      else if( c == F1 ) {
        GFDataFormat *df2 = new GFDataFormat( df );
        df2->Parity( 'S' );
        cout << "\nThe df parity is " << df.Parity();
        cout << "\nThe df2 pairty is " << df2->Parity() << flush;
        delete df2;
      }
      else
        cp << c;
    }
    cp >> c;
    if( c != GCPP_TIMEOUT )
      cout << (char)c << flush;
  }
} 

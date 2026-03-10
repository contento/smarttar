//*
//*  EXAMP016.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1989-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example demonstrates the GFLineStatus::GFLineStatus()
//*  constructor using a GFSerial parameter and GFSerial::GetCommError().
//*
#include "compplib.hpp"
#include "ibmkeys.h"
#include <iostream.h>

void main( void )
{
  int c;
  unsigned short error;

	GFI8250 sio( COM4, 0x2f8, IRQ3);//, 200, 200 );
	GFSerial cp( &sio );
  if( cp.GetCommError( error ) != GCPP_OK ) {
    cout << "\nError opening the port\n";
    return;
  }
  GFLineStatus ls( &cp );
  for(;;) {
    if( gfkbhit() ) {
      c = getkey();
      if( c == ESC )
        return;
      else if( c == F1 ) {
        ls.Status();
        if( ls.Carrier() )
          cout << "\nCarrier is ON!" << flush;
        else
          cout << "\nCarrier is OFF!" << flush;
      }
      else
        cp << c;
    }
    cp >> c;
    if( c != GCPP_TIMEOUT )
      cout << (char)c << flush;
  }
}

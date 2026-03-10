//*
//*  EXAMP094.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1989-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example program demonstrates the use of the breakchk()
//*  function.  Break checking can be turned on once all
//*  interrupts have been turned off.  Use in DOS only.
//*
#include "compplib.hpp"
#include "ibmkeys.h"
#include <iostream.h>

void main( void )
{
  int c;
  unsigned short error;
  GFI8250 *sio;
  GFSerial *cp;

  breakchk( GCPP_OFF );
  sio = new GFI8250( COM2 );
  cp = new GFSerial( sio );
  if( cp->GetCommError( error ) != GCPP_OK ) {
    cout << "\nError opening the port\n";
    return;
  }
  for(;;) {
    if( gfkbhit() ) {
      c = getkey();
      if( c == ESC ) {
        delete cp;
        delete sio;
        breakchk( GCPP_ON );
        cout << "Hit the break key now to exit..." << flush;
        for ( ;; )
          getkey();
      }
      else
        cp->PutChar( (unsigned char)c );
    }
    c = cp->GetChar();
    if( c != GCPP_TIMEOUT )
      cout << (char)c << flush;
  }
}

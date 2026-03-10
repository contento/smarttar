//*
//*  EXAMP122.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1989-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example demonstrates the GFSerial::BlockingOn() and
//*  GFSerial::BlockingOff() routines.  All incoming data will be written
//*  to a file in 1K blocks.  The blocking routines should allow the data to
//*  be written without causing the port to drop incoming data.
//*
#include "compplib.hpp"
#include "ibmkeys.h"
#include <stdio.h>
#include <iostream.h>

void main( void )
{
  char buffer[1024];
  int counter = 0;
  int c;
  GFSerial *cp;
  GFI8250 *sio;
  GFLineStatus *ls;
  FILE *file;

  sio = new GFI8250( COM2 );
  cp = new GFSerial( sio );
  ls = cp->GetLineStatus();
  ls->SetHandShake( RTS, CTS );
  file = fopen( "examp122.txt", "wb" );
  for(;;) {
    if( gfkbhit() ) {
      c = getkey();
      if( c == ESC ) {
        delete cp;
        delete sio;
        fclose( file );
        return;
      }
      else
        *cp << c;
    }
    *cp >> c;
    if( c != GCPP_TIMEOUT ) {
      cout << (char)c << flush;
      buffer[ counter++ ] = (char)c;
      if( counter >= 1024 ) {
        cp->BlockingOn();
        fwrite( buffer, 1, 1024, file );
        cp->BlockingOff();
        counter = 0;
      }
    }
  }
} 

//*
//*  EXAMP015.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1989-1994 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example demonstrates the GFSmartArnet::SystemIdle() function.
//*
#include "compplib.hpp"
#include "ibmkeys.h"
#include <iostream.h>

class GFSArnet : public GFSmartArnet
{
  public:
    GFSArnet( int port ) : GFSmartArnet( port )
      { port_number = port; }
    SystemIdle( void )
      { cout << port_number << flush; return( GCPP_OK ); }

    int port_number;
};

void main( void )
{
  int c;
  int index;
  GFSArnet *SmartArnet[4];
  GFSerial *cp[4];

  for( index = 0; index < 4; index++ ) {
    SmartArnet[ index ] = new GFSArnet( index+4 );
    cp[ index ] =  new GFSerial( SmartArnet[ index ] );
  }
  for( ; ; ) {
    if( gfkbhit() ) {
      c = getkey();
      if( c == ESC ) {
        for( index = 0; index < 4; index++ ) {
          delete cp[ index ];
          delete SmartArnet[ index ];
        }
        return;
      }
      else
        for( index = 0; index < 4; index++ )
          cp[ index ]->PutChar( (unsigned char)c );
    }
    for( index = 0; index < 4; index++ ) {
      c = cp[ index ]->GetChar( 100 );
      if( c != GCPP_TIMEOUT )
        cout << (char)c << flush;
    }
  }
} 


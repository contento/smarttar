//*
//*  EXAMP013.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1989-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example demonstrates the GFSmartStarGate::SystemIdle() function.
//*
#include "compplib.hpp"
#include "ibmkeys.h"
#include <iostream.h>

class GFSStar : public GFSmartStarGate
{
  public:
    GFSStar( int port ) : GFSmartStarGate( port )
      { port_number = port; }
    int SystemIdle( void )
      { cout << port_number << flush; return( GCPP_OK ); }

    int port_number;
};

void main( void )
{
  int c;
  int index;
  GFSStar *SmartStar[4];
  GFSerial *cp[4];

  GFSmartStarGate::ConfigureSmartStarGateBoard( 0, 0xd000, 0x200, COM5, 16 );
  for( index = 0; index < 4; index++ ) {
    SmartStar[ index ] = new GFSStar( index + 4 );
    cp[ index ] =  new GFSerial( SmartStar[ index ] );
  }
  for( ; ; ) {
    if( gfkbhit() ) {
      c = getkey();
      if( c == ESC ) {
        for( index = 0; index < 4; index++ ) {
          delete cp[ index ];
          delete SmartStar[ index ];
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


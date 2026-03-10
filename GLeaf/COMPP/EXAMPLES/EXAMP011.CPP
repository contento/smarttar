//*
//*  EXAMP011.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1989-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example demonstrates the GFSmartDigiBoard::SystemIdle() function.
//*
#include "compplib.hpp"
#include "ibmkeys.h"
#include <iostream.h>

class GFSDigi : public GFSmartDigiBoard
{
  public:
    GFSDigi( int comport ) : GFSmartDigiBoard( comport )
      { port_number = comport; }
    int SystemIdle( void )
      { cout << port_number; return( GCPP_OK ); }

    int port_number;
};

void main( void )
{
  int c;
  int index;
  GFSDigi *SmartDigi[4];
  GFSerial *cp[4];

  for( index = 0; index < 4; index++ ) {
    SmartDigi[ index ] = new GFSDigi( index + 4 );
    cp[ index ] =  new GFSerial( SmartDigi[ index ] );
  }
  for( ; ; ) {
    if( gfkbhit() ) {
      c = getkey();
      if( c == ESC ) {
        for( index = 0; index < 4; index++ ) {
          delete cp[ index ];
          delete SmartDigi[ index ];
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

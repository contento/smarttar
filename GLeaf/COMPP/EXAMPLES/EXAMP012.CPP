//*
//*  EXAMP012.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1989-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example demonstrates the GFSmartStarGate::GFSmartStarGate()
//*  constructor, destructor and ConfigureSmartStarGateBoard() functions.
//*
#include "compplib.hpp"
#include "ibmkeys.h"
#include <iostream.h>

void main( void )
{
  int c;
  int index;
  GFSmartStarGate *SmartStar[4];
  GFSerial *cp[4];

  GFSmartStarGate::ConfigureSmartStarGateBoard( 0, 0xd000, 0x200, COM5, 16 );
  for( index = 0; index < 4; index++ ) {
    SmartStar[ index ] = new GFSmartStarGate( index + 4 );
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
      c = cp[ index ]->GetChar();
      if( c != GCPP_TIMEOUT )
        cout << (char)c << flush;
    }
  }
} 

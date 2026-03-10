//*
//*  EXAMP014.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1989-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example demonstrates the GFSmartArnet::GFSmartArnet()
//*  constructor and the GFSmartArnet::~GFSmartArnet() destructor.
//*
#include "compplib.hpp"
#include "ibmkeys.h"
#include <iostream.h>

void main( void )
{
  int c;
  int index;
  GFSmartArnet *SmartArnet[4];
  GFSerial *cp[4];

  for( index = 0; index < 4; index++ ) {
    SmartArnet[ index ] = new GFSmartArnet( index + 4 );
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
      c = cp[ index ]->GetChar();
      if( c != GCPP_TIMEOUT )
        cout << (char)c << flush;
    }
  }
} 

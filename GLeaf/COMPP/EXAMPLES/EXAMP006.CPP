//*
//*  EXAMP006.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1989-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example demonstrates the GFDigiPC constructor and
//*  GFDigiPC::MultiBoardSetup() functions.
//*
#include "compplib.hpp"
#include "ibmkeys.h"
#include <iostream.h>

#define STATUS_REGISTER  0x140
#define START_ADDRESS    0x100
#define START_PORT       3
#define NUMBER_OF_PORTS  4

class NewDigiPC : public GFDigiPC
{
  public:
    NewDigiPC( int port, unsigned short addr, int irq, int rx_size, int tx_size ) : GFDigiPC( port, addr, irq, rx_size, tx_size )
      { GCPP_UNUSED_PARAMETER( port ); }
    int SystemIdle( void )
      { cout << "Hello World\n" << flush; return( GCPP_OK ); }
};

void main( void )
{
  int c;
  int i;
  GFDigiPC *dboard_ir[ 4 ];
  GFSerial *dboard_sp[ 4 ];

  GFDigiPC::MultiBoardSetup( START_PORT, STATUS_REGISTER, NUMBER_OF_PORTS );
  for( i = 0; i < 4; i++ ) {
    dboard_ir[ i ] = new NewDigiPC( i+3,
                                   (unsigned short)(START_ADDRESS + (i * 8)),
                                   IRQ3,
                                   300,
                                   300 );
    dboard_sp[ i ] = new GFSerial( dboard_ir[ i ] );
  }

  for( ; ; ) {
    if( gfkbhit() ) {
      c = getkey();
      if( c == ESC ) {
        for( i = 3; i >= 0; i-- ) {
          delete dboard_sp[ i ];
          delete dboard_ir[ i ];
        }
        return;
      }
      else
        for( i = 0; i < 4; i++ )
          dboard_sp[ i ]->PutChar( (unsigned char)c );
    }
    for( i = 0; i < 4; i++ ) {
      c = dboard_sp[ i ]->GetChar();
      if( c != GCPP_TIMEOUT )
        cout << (char)c << flush;
    }
  }
} 

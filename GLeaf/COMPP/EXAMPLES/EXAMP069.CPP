//*
//*  EXAMP069.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1989-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example demonstrates the GFModem::SetSpeakerLevel().
//*
#include "compplib.hpp"
#include "ibmkeys.h"
#include <iostream.h>

class MyModem : public GFModem
{
  public:
    MyModem( void ) : GFModem()
      {}
    void ModemPrinter( char c )
      { cout << c; }
};

void main( void )
{
  int c;

  GFI8250 sio( COM2 );
  GFSerial cp( &sio );
  MyModem md;

  md.WaitForOk( 1000, "OK" );
  md.Install( &cp );

  for( ; ; ) {
    if( gfkbhit() ) {
      c = getkey();
      switch( c ) {
        case ESC:
          return;
        case F1:
          md.Dial("250-3778");
          break;
        case F2:
          md.SetSpeakerLevel( SPEAKER_OFF );
          break;
        case F3:
          md.SetSpeakerLevel( SPEAKER_ON_ALWAYS );
          break;
        default:
          cp.PutChar( (unsigned char)c );
      }
    }
    c = cp.GetChar( 18 );
    if( c != GCPP_TIMEOUT )
      cout << (char)c << flush;
  }
} 


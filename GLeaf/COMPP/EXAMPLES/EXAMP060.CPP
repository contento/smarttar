//*
//*  EXAMP060.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1989-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example demonstrates GFModem::ModemPrinter(), GFModem::EchoOn(),
//*  GFModem::EchoOff() and GFModem::FixedDelay().
//*
#include "compplib.hpp"
#include "ibmkeys.h"
#include <iostream.h>

class MyModem : public GFModem
{
  public:
    MyModem() : GFModem()
      {}
    void modem_printer( char c)
      { cout << c; }
};

void main( void )
{
  int c;

  GFI8250 sio( COM2 );
  GFSerial cp( &sio );
  MyModem md;

  md.FixedDelay( 800 );
  md.Install( &cp );
  md.AutoOn();
  md.DialMode( GCPP_TONE );
  md.EchoOn();

  for( ; ; ) {
    if( gfkbhit() ) {
      c = getkey();
      switch( c ) {
        case ESC: return;
        case F1:
          md.Dial("250-3778");
          break;
        case F2:
          md.HangUp();
          break;
        case F3:
          md.EchoOn();
          break;
        case F4:
          md.EchoOff();
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

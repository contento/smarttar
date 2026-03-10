//*
//*  EXAMP063.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1989-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example demonstrates the GFModem::ModemOnline(),
//*  GFModem::OnHook(), and GFModem::OffHook() functions.
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

  md.WaitForOk( 800, "OK" );
  md.Install( &cp );
  md.EchoOn();

  for( ; ; ) {
    if( gfkbhit() ) {
      c = getkey();
      switch( c ) {
        case ESC:
          return;
        case F1:
          UseSysTime( &cp, 30 );
          cp.PutString( (const GCPP_BYTE *)"+++", -1 );
          UseSysTime( &cp, 30 );
          md.EchoOff();
          md.ModemOnline();
          break;
        case F2:
          md.OnHook();
          break;
        case F3:
          md.OffHook();
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

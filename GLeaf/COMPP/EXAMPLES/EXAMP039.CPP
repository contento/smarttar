//*
//*  EXAMP039.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1989-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example demonstrates the UseSysTime() , GetComErrorName(),
//*  ElapsedSysTime(), and GFSerial::CallSystemIdle() functions. This
//*  example also shows how to change the SystemIdle() function.
//*
#include "compplib.hpp"
#include "ibmkeys.h"
#include <iostream.h>

class MyGFI8250 : public GFI8250
{
  public:
    MyGFI8250( int com = COM1,
               unsigned short ubase = USEDEFAULT,
               int irqno = USEDEFAULT,
               unsigned rxsize = GCPP_RXSIZE,
               unsigned txsize = GCPP_TXSIZE ) : GFI8250( com,
                                                        ubase,
                                                        irqno,
                                                        rxsize,
                                                        txsize)
      {}
  int SystemIdle( void )
    { cout << "A SystemIdle Routine\n"; return( GCPP_OK ); }
};

void main( void )
{
  int c;
  unsigned short error;
  long time;

  MyGFI8250 sio( COM2 );
  GFSerial cp( &sio );
  if( cp.GetCommError( error ) != GCPP_OK ) {
    cout << "\nError opening the port\n";
    cout << "\nError description: "<< GetCommErrorName( error ) << "\n";
    return;
  }
  for(;;) {
    if( gfkbhit() ) {
      c = getkey();
      switch( c ) {
        case ESC:
          return;
        case F1:
          cout << "Using System Time for the next quarter of a second\n";
          cout << "The SystemIdleFunction will be called during that time\n";
          UseSysTime( &cp, 250 );
          break;
        case F2:
          time = ElapsedSysTime();
          cout << "Time in milliseconds: " << time << "\n";
          UseSysTime( &cp, TICKS_PER_SECOND );
          time = ElapsedSysTime();
          cout << "After a timer( 18 ) the time in milliseconds is: ";
          cout << time << "\n";
          break;
        case F3:
          cp.CallSystemIdle();
        default:
          cp << c;
      }
    }
    cp >> c;
    if( c != GCPP_TIMEOUT )
      cout << (char)c << flush;
  }
}

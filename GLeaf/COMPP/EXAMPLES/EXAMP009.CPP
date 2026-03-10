//*
//*  EXAMP009.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1989-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example demonstrates the GFI8250::Set16550TriggerLevel() function.
//*
#include "compplib.hpp"
#include "ibmkeys.h"
#include <iostream.h>

void main( void )
{
  int c;

  GFI8250 sio ( COM2 );

  cout << "Trigger Level ?\n";
  cout << "F1 - TRIGGER_DISABLE\n";
  cout << "F2 - TRIGGER_04\n";
  cout << "defaults to TRIGGER_14 if you have a 16550 UART\n";
  cout << "If you do not have a 16550, SetTriggerLevel will do nothing\n";
  c = getkey();
  switch ( c ){
    case ESC:
      return;
    case F1:
      cout << "For 16550 UARTs: Disabling the Trigger Level\n";
      sio.Set16550TriggerLevel( TRIGGER_DISABLE );
      break;
    case F2:
      cout << "For 16550 UARTs: Setting Trigger Level 4\n";
      sio.Set16550TriggerLevel( TRIGGER_04 );
  }
  GFSerial cp( &sio );

  for( ; ; ) {
    if( gfkbhit() ) {
      c = getkey();
      if( c == ESC )
        return;
      else
        cp.PutChar( (unsigned char)c );
    }
    c = cp.GetChar();
    if( c != GCPP_TIMEOUT )
      cout << (char)c << flush;
  }
} 

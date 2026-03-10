//*
//*  EXAMP065.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1989-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example demonstrates the GFModem::Redial() function.
//*  The user can enter a number to dial and then use the
//*  F2 key to keep redialing the same number.
//*
#include "compplib.hpp"
#include "ibmkeys.h"
#include <iostream.h>

void main( void )
{
  int c;
  char number[20];

#ifdef GCPP_OS2
  GFComOS2 sio( COM2 );
#else
  GFI8250 sio( COM2 );
#endif
  GFSerial cp( &sio );
  GFModem md;
  md.FixedDelay( 800 );
  if( md.Install( &cp ) != GCPP_OK ) {
    cout << "\nError Setting Up Modem Class\n";
    return;
  }
  for(;;) {
    if( gfkbhit() ) {
      c = getkey();
      if( c == ESC )
        return;
      else if( c == F1 ) {
        cout << "\nEnter a number to dial: ";
        cin >> number;
        md.Dial( number );
      }
      else if( c == F2 ) {
        cout << "\nRedialing the number\n" << flush;
        md.Redial( );
      }
      else
        cp << c;
    }
    cp >> c;
    if( c != GCPP_TIMEOUT )
      cout << (char)c << flush;
  }
} 

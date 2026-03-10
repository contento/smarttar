//*
//*  EXAMP031.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1989-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example demonstrates the GFDataFormat::GFDataFormat()
//*  constructor and the GFDataFormat::Parity() function.
//*  The GFDataFormat constructor uses a GFSerial parameter.
//*  The user can query the port parity by using the F1 key.
//*  The F2 key will allow the user to set the parity for the
//*  port.
//*
#include "compplib.hpp"
#include "ibmkeys.h"
#include <iostream.h>

void main( void )
{
  int c;
  char pty;

#ifdef GCPP_OS2
  GFComOS2 sio( COM2 );
#else
  GFI8250 sio( COM2 );
#endif
  GFSerial cp( &sio );
  GFDataFormat df( &cp );
  GFLineStatus ls( &cp );
  for(;;) {
    if( gfkbhit() ) {
      c = getkey();
      if( c == ESC )
        return;
      else if( c == F1 ) {
        pty = df.Parity();
        switch( pty ) {
          case 'N':
            cout << "\nNo Parity" << flush;
            break;
          case 'O':
            cout << "\nOdd Parity" << flush;
            break;
          case 'E':
            cout << "\nEven Parity" << flush;
            break;
          case 'S':
            cout << "\nSpace Parity" << flush;
            break;
          case 'M':
            cout << "\nMark Parity\n" << flush;
            break;
          default:
            break;
        }
      }
      else if( c == F2 ) {
        cout << "\n\nN - No parity\nO - Odd parity" <<
        "\nE - Even parity\nS - Space parity" <<
        "\nM - Mark parity\n";
        cin >> pty;
        if( pty == 'N' || pty == 'O' || pty == 'E' ||
            pty == 'S' || pty == 'M' ) {
          df.Parity( pty );
          df.Install( &cp );
        }
      }
      else
        cp << c;
    }
    cp >> c;
    ls.Status();
    if( c != GCPP_TIMEOUT ) {
      cout << (char)c << flush;
      if( ls.ParityError() )
        cout << "\nThere has been a parity error\n" << flush;
    }
  }
} 

//*
//*  EXAMP070.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1989-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example demonstrates the GFModem::Write() and
//*  GFModem::WriteNoWait() functions.  This example will
//*  allow the user to control a modem from a list of commands.
//*
#include "compplib.hpp"
#include "ibmkeys.h"
#include <iostream.h>

void main( void )
{
  int c;
  int choice;

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
        cout << "\nA list of commands\n";
        cout << "  1. E0 - do not echo characters\n";
        cout << "  2. E1 - echo characters\n";
        cout << "  3. M1 - speaker on until carrier detected\n";
        cout << "  4. M2 - speaker on always\n";
        cout << "  5. V0 - Result codes as digits\n";
        cout << "  6. V1 - Result codes as words\n";
        cout << "  7. X0 - Basic result codes\n";
        cout << "  8. X1 - Extended result codes\n";
        cout << "  9. DT - Dail the phone\n";
        cin >> choice;
        switch( choice ) {
          case 1:
            md.Write( "E0" );
            break;
          case 2:
            md.Write( "E1" );
            break;
          case 3:
            md.Write( "M1" );
            break;
          case 4:
            md.Write( "M2" );
            break;
          case 5:
            md.Write( "V0" );
            break;
          case 6:
            md.Write( "V1" );
            break;
          case 7:
            md.Write( "X0" );
            break;
          case 8:
            md.Write( "X1" );
            break;
          case 9:
            md.WriteNoWait( "DT 250-3778" );
            break;
          default:
            break;
        }
      }
      else
        cp << c;
    }
    cp >> c;
    if( c != GCPP_TIMEOUT )
      cout << (char)c << flush;
  }
} 

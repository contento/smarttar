//*
//*  EXAMP061.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1989-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example demonstrates the GFModem::GetRegister() and
//*  GFModem::SetRegister() functions.  By pressing the F1 key, the
//*  user can get the S0 register setting.  The F2 key will allow
//*  the user to set the S0 to a value from 0 to 254.
//*
#include "compplib.hpp"
#include "ibmkeys.h"
#include <iostream.h>

static struct GFModemInfo mi = {
  "Modem Initialization",
  "AT",
  "\r",
  "X1V1E0 S0=0 S7=66",
  "DT",
  "DP",
  "...+++...ATH0",
  "Z",
  "S%d=%d",
  "S%d?",
  "S0=1",
  "S0=0",
  "A",
  "E1",
  "E0",
  "O",
  "H0",
  "H1",
  "V0",
  "V1",
  {"M0", "M1", "M2"},
  {"X0", "X1", "X2", "X3", "X4", "X5", "X6", "X7"}
};

void main( void )
{
  int c;
  int regval;
  char *sreg;

#ifdef GCPP_OS2
  GFComOS2 sio( COM2 );
#else
  GFI8250 sio( COM2 );
#endif
  GFSerial cp( &sio );
  GFModem md;
  md.WaitForOk( 800, "OK" );
  if( md.Install( &cp, &mi ) != GCPP_OK ) {
    cout << "\nError setting up modem class\n";
    return;
  }
  for(;;) {
    if( gfkbhit() ) {
      c = getkey();
      if( c == ESC )
        return;
      else if( c == F1 ) {
        cout << "\nGetting contents of modem register S0";
        if( ( sreg = md.GetRegister( 0 )) != NULL )
          cout << "\nS0 = " << sreg << "\n" << flush;
        else
          cout << "\nError getting register contents\n" << flush;
      }
      else if( c == F2 ) {
        cout << "\nEnter a value for S0 (0 - 254): ";
        cin >> regval;
        if( regval >= 0 && regval < 255 )
          if( md.SetRegister( 0, regval ) == GCPP_OK )
            cout << "\nModem register S0 was set correctly\n" << flush;
          else
            cout << "\nError setting modem regiser S0\n" << flush;
      }
      else
        cp << c;
    }
    cp >> c;
    if( c != GCPP_TIMEOUT )
      cout << (char)c << flush;
  }
} 

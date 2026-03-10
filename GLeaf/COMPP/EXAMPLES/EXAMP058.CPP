//*
//*  EXAMP058.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1989-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example demonstrates the GFModem::GFModem(), GFModem::Dial(),
//*  GFModem::HangUp(), GFModem::Install(), and GFModem::Reset() functions.
//*  We create the GFModem object and associate it with the open
//*  serial port.  We reset the modem and allow the user to dial
//*  and hangup.
//*
#include "compplib.hpp"
#include "ibmkeys.h"
#include <iostream.h>

static struct GFModemInfo mi = {
  "Modem Initialization",
  "AT",
  "\r",
  "X1V1E1 S0=0 S7=66",
  "DT",
  "DP",
  "~~~+++~~~ATH0",
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

#ifdef GCPP_OS2
  GFComOS2 sio( COM2 );
#else
  GFI8250 sio( COM2 );
#endif
  GFSerial cp( &sio );
  GFModem md;
  md.FixedDelay( 800 );
  if( md.Install( &cp, &mi ) != GCPP_OK ) {
    cout << "\nError Setting Up Modem Class\n";
    return;
  }
  md.Reset( );
  for(;;) {
    if( gfkbhit() ) {
      c = getkey();
      if( c == ESC )
        return;
      else if( c == F1 )
        md.Dial( "250-3778" );
      else if( c == F2 )
        md.HangUp( );
      else
        cp << c;
    }
    cp >> c;
    if( c != GCPP_TIMEOUT )
      cout << (char)c << flush;
  }
} 

//*
//*  EXAMP055.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1989-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example demonstrates the GFModem::GFModem() copy
//*  constructor using a GFModem parameter.  The second
//*  GFModem object is constructed from the first object.
//*
#include "compplib.hpp"
#include "ibmkeys.h"
#include <iostream.h>

void dial_phone( GFSerial *cp, GFModem *md );

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

static struct GFModemInfo mi2 = {
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
  md.WaitForOk( 800, "OK" );
  if( md.Install( &cp, &mi ) != GCPP_OK ) {
    cout << "\nError Setting Up Modem Class\n";
    return;
  }
  for(;;) {
    if( gfkbhit() ) {
      c = getkey();
      if( c == ESC )
        return;
      else if( c == F1 )
        dial_phone( &cp, &md );
      else
        cp << c;
    }
    cp >> c;
    if( c != GCPP_TIMEOUT )
      cout << (char)c << flush;
  }
} 

void dial_phone( GFSerial *cp, GFModem *md )
{
  GFModem md2( *md );
  md2.Install( cp, &mi2 );
  md2.Dial( "250-3778" );
}

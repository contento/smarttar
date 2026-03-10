//*
//*  EXAMP062.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1989-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example demonstrates the GFModem::Initialize(), GFModem::EchoOn()
//*  and GFModem::WaitForOk() functions.
//*
#include "compplib.hpp"
#include "ibmkeys.h"
#include <string.h>
#include <iostream.h>

static struct GFModemInfo mi = {
  "Modem Initialization",
  "AT",
  "\r",
  "X1V1E1 S0=0",
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
  long msecs;
  char modem_input[81];

#ifdef GCPP_OS2
  GFComOS2 sio( COM2 );
#else
	GFI8250 sio( COM2 );
//	GFI8250 sio( COM4, 0x2f8, IRQ3);//, 200, 200 );
#endif
  GFSerial cp( &sio );
  GFModem md;

  md.WaitForOk( 800, "OK" );
  md.Install( &cp, &mi );
  cout << "Initializing modem\n";
  md.Initialize();
  md.DialMode( GCPP_TONE );
  md.Dial("250-3778");
  msecs = 50000L;
  while( msecs > 0 ) {
    msecs = md.GetModemInput( modem_input, 81, msecs );
    if( strstr( modem_input, "CONNECT" ) != 0 ) {
      cout << "Connection successfully made ! \n";
      return;
    }
  }
  cout << " Failed to connect !\n";
}

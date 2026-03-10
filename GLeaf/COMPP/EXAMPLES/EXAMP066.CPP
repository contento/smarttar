//*
//*  EXAMP066.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1989-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example demonstrates the GFModem::GetModemInput(),
//*  GFModem::AnswerPhone(), GFModem::AbortModemFunction() and
//*  GFModem::SetAbortKey() functions.
//*
#include "compplib.hpp"
#include "ibmkeys.h"
#include <string.h>
#include <iostream.h>

static struct GFModemInfo mi = {
  "Modem Initialization",
  "AT",
  "\r",
  "X1V1E0 S0=0 S7=66",
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

class MyModem : public GFModem
{
  public:
    MyModem( void ) : GFModem()
      {}
    void ModemPrinter( char c)
      { cout << c << flush; }
};


class CommPortHandler
{
  public:
    GFComLink *sio;
    GFSerial *cp;
    GFModem *md;
    GFDataFormat *df;
    int c;
    char modem_input[81];

    CommPortHandler( int port );
    ~CommPortHandler( void );
    int WaitForCall( void );
    int Answer( void );
};

void main(void)
{
  CommPortHandler MyPort( COM2 );

  MyPort.WaitForCall();
}

CommPortHandler::CommPortHandler( int port )
{
  sio = new GFI8250( port );
  cp = new GFSerial( sio );
  md = new MyModem();
  df = cp->GetDataFormat();

  md->WaitForOk( 800, "OK" );
  md->Install( cp, &mi );
  md->SetVerbalResponse();
  md->EchoOn();
  md->SetAbortKey( ALTX );
  cout << "Setup complete\n";
}

CommPortHandler::~CommPortHandler( void )
{
  delete( md );
  delete( cp );
  delete( sio );
}


int CommPortHandler::WaitForCall()
{
  for(;;) {
    cout << "Waiting for the phone to RING \n";
    if( md->GetModemInput( modem_input, 81, 1000 ) == GCPP_USER_ABORT )
      return( GCPP_USER_ABORT );
    if( strcmp( modem_input, "RING" ) == 0 ) {
      cout << "The phone is ringing\n";
      c = Answer();
      return c;
    }
  }
}

int CommPortHandler::Answer()
{
  long baud = 0L;
  long msecs = 30000L;

  cout << "Answering phone\n";
  md->AnswerPhone();
  while( msecs > 0 ) {
    msecs = md->GetModemInput( modem_input, 81, msecs );
    if( strstr( modem_input, "CONNECT 9600" ) != 0 )
       baud = 9600L;
    if( strstr( modem_input, "CONNECT 4800" ) != 0 )
       baud = 4800L;
    if( strstr( modem_input, "CONNECT 2400" ) != 0 )
       baud = 2400L;
    if( strstr( modem_input, "CONNECT 1200" ) != 0 )
       baud = 1200L;
    if( strstr( modem_input, "NO CARRIER" ) != 0 )
      msecs = baud = 0;
  }
  if( baud > 0) {
    cout << "\nBaud rate detected as " << baud << "\n";
    df->BaudRate( baud );
  }
  else {
    cout << "\nFailed to Connect\n";
    return( 1 );
  }
  cout << "CONNECT at " << baud << " Succesful !\n";
  cp->PutString( (const GCPP_BYTE *)(" You have been connected !") );
  cout << " --- Insert your application here ---\n";
  cout << " That's all folks\n";
  cp->PutString( (const GCPP_BYTE *)" That's all folks" );
  return( 0 );
}

//*
//*  EXAMP073.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1989-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example demonstrates the GFMonitor::TransferMessage(),
//*  GFMonitor::TransferAbort() and GFMonitor::SetAbortKey() functions.
//*  The example will send a file when the user hits the F1 key and it will
//*  receive a file when the user hits F2. The abort key was changed to
//*  CTRL_X from ESC (default).  GFMtr is derived from GFMonitor to show
//*  the usage of the TransferMessage() and TransferAbort() functions.
//*
#include "compplib.hpp"
#include "filexfer.hpp"
#include "ibmkeys.h"
#include <iostream.h>

GFLineStatus *ls;

class GFMtr : public GFMonitor
{
  public:
    void TransferMessage( char *message )
      { cout << "\nFilename: " << fname << "\n";
        cout << "\n Message: " << message << "\n"; }
    int TransferAbort( void );
};

void main( void )
{
  int c;
  int status;

  GFI8250 sio( COM1 );
  GFSerial cp( &sio );

  ls = new GFLineStatus( &cp );

  GFMtr mtr;
  GFXmodem1K txfr( &cp, &mtr );
  mtr.SetAbortKey( CTRL_X );

  for(;;) {
    if( gfkbhit() ) {
      c = getkey();
      if( c == ESC ){
        delete( ls );
        return;
      }
      else if( c == F1 ) {
        status = txfr.SendFile( "..\\examples\\EXAMP073.CPP" );
        cout << "\nSend file status = " << status << "\n" << flush;
      }else if( c == F2 ) {
        status = txfr.ReceiveFile( "TEST.DOC" );
        cout << "\nReceive file status = " << status << "\n" << flush;
      }
      else
        cp << c;
    }
    cp >> c;
    if( c != GCPP_TIMEOUT )
      cout << (char)c << flush;
  }
} 

int GFMtr::TransferAbort( void )
{
  unsigned key;

  while( gfkbhit() ) {
    key = getkey();
    if( key == transfer_abort_key )
      return( GCPP_ERR_U );
    else if( transfer_type == XTYPE_ASCII && sending )
      return( key );
  }
  ls->Status();
  if( !ls->Carrier() ) {
    cout << "Transfer aborted due to lost carrier\n";
    return( GCPP_ERROR );
  }
  return( GCPP_OK );

}

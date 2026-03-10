//*
//*  EXAMP086.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1989-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example demonstrates the GFYmodemG::GFYmodemG() constructor,
//*  GFYmodemG::~GFYmodemG() destructor, GFYmodemG::SendFile() and
//*  GFYmodemG::ReceiveFile functions.  The example will send a file
//*  when the user hits the F1 key and it will receive a file when the
//*  user hits F2.  GFMtr is derived from GFMonitor to show the usage
//*  of the UserMonitor() function.
//*
#include "compplib.hpp"
#include "filexfer.hpp"
#include "ibmkeys.h"
#include <iostream.h>

class GFMtr : public GFMonitor
{
  public:
    int UserMonitor( int code );
};

void main( void )
{
  int c;
  int status;
  GFYmodemG *txfr;

#ifdef GCPP_OS2
  GFComOS2 sio( COM2 );
#else
  GFI8250 sio( COM2 );
#endif
  GFSerial cp( &sio );
  GFMtr mtr;
  txfr = new GFYmodemG( &cp, &mtr );
  for(;;) {
    if( gfkbhit() ) {
      c = getkey();
      if( c == ESC ) {
        delete txfr;
        return;
      }
      else if( c == F1 ) {
        status = txfr->SendFile( "..\\examples\\EXAMP086.CPP" );
        cout << "\nSend file status = " << status << "\n" << flush;
      }
      else if( c == F2 ) {
        status = txfr->ReceiveFile();
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

int GFMtr::UserMonitor( int code )
{
  if( code == GCPP_OK ) {
    if( sending  )
      cout << "\nSent packet " << packno << " with " <<
          errors << " error(s)" << flush;
    else
      cout << "\nReceived packet " << packno << " with " <<
          errors << " error(s)" << flush;
  }
  else if( code > GCPP_OK )
    cout << "\nError code " << code << "\n" << flush;
  return( code );
}

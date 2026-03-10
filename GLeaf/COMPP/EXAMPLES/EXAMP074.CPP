//*
//*  EXAMP074.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1989-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example demonstrates the GFKermit::GFKermit() constructor,
//*  the GFKermit::~GFKermit() destructor, the GFMonitor::UserMonitor(),
//*  and GFKermit::ReceiveFile() functions.  The example will send a
//*  file when the user hits the F1 key.  GFMtr is derived from
//*  GFMonitor show the usage of the UserMonitor() function.
//*
#include "compplib.hpp"
#include "filexfer.hpp"
#include "ibmkeys.h"
#include <iostream.h>

class GFMtr:public GFMonitor
{
  public:
    int UserMonitor( int code );
};

void main( void )
{
  int c;
  int status;

#ifdef GCPP_OS2
  GFComOS2 sio( COM2 );
#else
  GFI8250 sio( COM2 );
#endif
  GFSerial cp( &sio );
  GFMtr mtr;
  for(;;) {
    if( gfkbhit() ) {
      c = getkey();
      if( c == ESC )
        return;
      else if( c == F1 ) {
        GFKermit *txfr = new GFKermit( &cp, &mtr );
        status = txfr->ReceiveFile( "TEST.DOC" );
        delete txfr;
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
  if( code == GCPP_OK )
    cout << "\nReceived packet " << packno << " with " <<
        errors << " error(s)";
  else if( code > GCPP_OK )
    cout << "\nError code " << code << "\n";
  cout.flush();
  return( code );
}

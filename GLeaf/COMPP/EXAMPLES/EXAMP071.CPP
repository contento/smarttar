//*
//*  EXAMP071.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1989-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example demonstrates the GFMonitor::AsciiPrinter(),
//*  GFTranstxt::SetLineFeedStripping() and GFTranstxt::SetRecurseFlag()
//*  functions.  A text file is sent when the user hits the F1 key and
//*  a text file is received when the F2 key is pressed.
//*
#include "compplib.hpp"
#include "filexfer.hpp"
#include "ibmkeys.h"
#include <iostream.h>

class GFMtr : public GFMonitor
{
  public:
    void AsciiPrinter( char c )
      { cout << c; }
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
  GFTranstxt txf( &cp, &mtr, 80 );
  txf.SetLineFeedStripping( GCPP_ON );
  txf.SetRecurseFlag( GCPP_ON );
  for(;;) {
    if( gfkbhit() ) {
      c = getkey();
      if( c == ESC )
        return;
      else if( c == F1 ) {
        status = txf.SendFile( "..\\examples\\EXAMP071.CPP" );
        cout << "\nSend file status = " << status << "\n" << flush;
      }
      else if( c == F2 ) {
        status = txf.ReceiveFile( "TEST.DOC" );
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

//*
//*  EXAMP072.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1989-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example demonstrates the GFMonitor::GFMonitor constructor(),
//*  GFTranstxt::GFTranstxt() constructor, destructor, and
//*  GFTranstxt::SendFile() functions.  A text file is sent when the
//*  user hits the F1 key.
//*
#include "compplib.hpp"
#include "filexfer.hpp"
#include "ibmkeys.h"
#include <iostream.h>

void main( void )
{
  int c;
  int status;
  GFTranstxt *txf;

#ifdef GCPP_OS2
  GFComOS2 sio( COM2 );
#else
  GFI8250 sio( COM2 );
#endif
  GFSerial cp( &sio );
  GFMonitor mtr;
  txf = new GFTranstxt( &cp, &mtr, 80 );
  for(;;) {
    if( gfkbhit() ) {
      c = getkey();
      if( c == ESC ) {
        delete txf;
        return;
      }
      else if( c == F1 ) {
        status = txf->SendFile( "..\\examples\\EXAMP072.CPP" );
        cout << "\nSend file status = " << status << "\n" << flush;
      }
      else
        cp << c;
    }
    cp >> c;
    if( c != GCPP_TIMEOUT )
      cout << (char)c << flush;
  }
} 

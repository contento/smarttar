//*
//*  EXAMP044.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1989-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example demonstrates the GFSerial::GetBufferInfo() function,
//*  and displays the current status of the GFLinkInfo structure.
//*  The F1 key is used to get this information.
//*
#include "compplib.hpp"
#include "ibmkeys.h"
#include <iostream.h>

void main( void )
{
  int c;
  struct GFLinkInfo binfo;

#ifdef GCPP_OS2
  GFComOS2 sio( COM2 );
#else
  GFI8250 sio( COM2 );
#endif
  GFSerial cp( &sio );
  for(;;) {
    if( gfkbhit() ) {
      c = getkey();
      if( c == ESC )
        return;
      else if( c == F1 ) {
        cp.GetBufferInfo( binfo );
        cout << "\nRxSize = " << binfo.rxsize;
        cout << "\nRxCount = " << binfo.rxcount;
        cout << "\nTxSize = " << binfo.txsize;
        cout << "\nTxCount = " << binfo.txcount;
        cout << "\nStatus = " << binfo.status << "\n" << flush;
      }
      else
        cp << c;
    }
    cp >> c;
    if( c != GCPP_TIMEOUT )
      cout << (char)c << flush;
  }
} 

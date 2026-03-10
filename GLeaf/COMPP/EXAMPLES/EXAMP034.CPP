//*
//*  EXAMP034.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1989-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example demonstrates the GFDataFormat::Install() and
//*  GFDataFormat::DataLength() functions.  We create an uninitialized
//*  GFDataFormat object and use Install() to link it to the serial
//*  port.  The F1 key will retrieve the current data length
//*  and print it to the screen.  The F2 key will allow
//*  the user to set the data length from a list of choices.
//*
#include "compplib.hpp"
#include "ibmkeys.h"
#include <iostream.h>

void main( void )
{
  int c;
  int dl;

#ifdef GCPP_OS2
  GFComOS2 sio( COM2 );
#else
  GFI8250 sio( COM2 );
#endif
  GFSerial cp( &sio );
  GFDataFormat df( NULL );
  df.Install( &cp );
  for(;;) {
    if( gfkbhit() ) {
      c = getkey();
      if( c == ESC )
        return;
      else if( c == F1 )
        cout << "\nThe data lenght is " << df.DataLength() << "\n" << flush;
      else if( c == F2 ) {
        cout << "\nSelect new datalength (5-8): ";
        cin >> dl;
        if( dl > 4 && dl < 9 ) {
          df.DataLength( dl );
          df.Install( &cp );
        }
      }
      else
        cp << c;
    }
    cp >> c;
    if( c != GCPP_TIMEOUT )
      cout << (char)c << flush;
  }
} 

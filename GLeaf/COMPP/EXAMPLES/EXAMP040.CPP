//*
//*  EXAMP040.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1989-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example demonstrates the GFSerial::Delay() function. We set
//*  up the port with a user specified delay between characters.
//*
#include <stdlib.h>
#include "compplib.hpp"
#include "ibmkeys.h"
#include <iostream.h>

void main( int argc, char *argv[] )
{
  int c;

  if( argc < 2 ) {
    cout << "\nEXAMP026 <number of milliseconds to delay>\n";
    return;
  }

#ifdef GCPP_OS2
  GFComOS2 sio( COM2 );
#else
  GFI8250 sio( COM2 );
#endif
  GFSerial cp( &sio );
  cp.Delay( atoi( argv[1] ) );
  for(;;) {
    if( gfkbhit() ) {
      c = getkey();
      if( c == ESC )
        return;
      else
        cp << c;
    }
    cp >> c;
    if( c != GCPP_TIMEOUT )
      cout << (char)c << flush;
  }
} 

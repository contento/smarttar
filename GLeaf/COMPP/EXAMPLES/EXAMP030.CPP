//*
//*  EXAMP030.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1989-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example demonstrates the GFLineStatus::SendBreak()
//*  function.  We use the user supplied value for the length
//*  of the break signal.
//*
#include <stdlib.h>
#include "compplib.hpp"
#include "ibmkeys.h"
#include <iostream.h>

void main( int argc, char *argv[] )
{
  int c;
  int length;

  if( argc < 2 ) {
    cout << "\nEXAMP016 <number of milliseconds to break>\n";
    return;
  }
  length = atoi( argv[ 1 ] );
#ifdef GCPP_OS2
  GFComOS2 sio( COM2 );
#else
  GFI8250 sio( COM2 );
#endif
  GFSerial cp( &sio );
  GFLineStatus ls( &cp );
  for(;;) {
    if( gfkbhit() ) {
      c = getkey();
      if( c == ESC )
        return;
      else if( c == F1 )
        ls.SendBreak( length );
      else
        cp << c;
    }
    cp >> c;
    if( c != GCPP_TIMEOUT )
      cout << (char)c << flush;
  }
} 

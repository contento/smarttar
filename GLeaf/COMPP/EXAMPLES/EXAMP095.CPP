//*
//*  EXAMP095.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1989-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example demonstrates the use of the getkey(), gfkbhit() and
//*  enhkbd() functions.
//*
#include "compplib.hpp"
#include "ibmkeys.h"
#include <iostream.h>

void main()
{
  int c;
  unsigned short error;

  GFI8250 sio( COM2 );
  GFSerial cp( &sio );
  if( cp.GetCommError( error ) != GCPP_OK ) {
    cout << "\nError opening the port\n";
    return;
  }
  cout << "This machine has " << (enhkbd() ? "a" : "NO") <<
      " enhanced keyboard.\n" << flush;
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

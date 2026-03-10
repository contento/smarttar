//*
//*  EXAMP050.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1989-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This terminal emulation example demonstrates the use of the
//*  GFSerial::Printf() function.  We will use the Printf() function
//*  to merge the first name, last name, and password into a
//*  Bulletin Board logon script.
//*
#include "compplib.hpp"
#include "ibmkeys.h"
#include <iostream.h>

char *fn = "Jerry";
char *ln = "Rosiek";
char *pw = "Friend";

void main( void )
{
  int c;
  int count;

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
        count = cp.Printf( "%s %s %s\n", fn, ln, pw );
        cout << "We printed " << count << " character to the port\n" << flush;
      }
      else
        cp << c;
    }
    cp >> c;
    if( c != GCPP_TIMEOUT )
      cout << (char)c << flush;
  }
} 

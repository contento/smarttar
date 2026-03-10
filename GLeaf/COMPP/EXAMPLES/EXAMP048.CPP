//*
//*  EXAMP048.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1989-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example demonstrates the GFSerial::GetString() function.
//*
#include "compplib.hpp"
#include "ibmkeys.h"
#include <iostream.h>

char *string = "This is a demonstration program!";

void main( void )
{
  int c;
  int i;
  char buffer[21];

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
        for( i = 0; i < 5; i++ ) {
          cp.PutString( (unsigned char *)string );
        }
        for( i = 0; i < 5; i++ ) {
          cp.GetString( (unsigned char *)buffer, 20, '\n', 100 );
          cout << buffer;
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

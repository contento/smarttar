//*
//*  EXAMP043.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1989-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This terminal emulation example demonstrates the use of the
//*  GFSerial::PutBuffer() and GFSerial::GetBuffer() functions.  We
//*  write the output buffer to the modem and then use GetBuffer to
//*  clear the receive buffer of the echoed characters.
//*
#include "compplib.hpp"
#include "ibmkeys.h"
#include <iostream.h>

unsigned char obf[] = "ATV1E1Q0X1 S7=66 S11=55 S0=0";

void main( void )
{
  int c;
  unsigned char ibf[40];

#ifdef GCPP_OS2
  GFComOS2 sio( COM2 );
#else
  GFI8250 sio( COM2 );
#endif
  GFSerial cp( &sio );
  cp.PutBuffer( obf, 28 );
  UseSysTime( &cp, 5 );
  cp.GetBuffer(  ibf, 40, 60 );
  cout << "\n" << ibf << "\n" << flush;
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

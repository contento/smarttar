//*
//*  EXAMP103.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1989-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example demonstrates the GFVt100::GFVt100() constructor,
//*  GFVt100::~GFVt100() destructor, GFVt100::GetKey() and
//*  GFVt100::GetChar() functions. This terminal program will use
//*  VT100 terminal emulation.
//*
#include "compplib.hpp"
#include "compptrm.hpp"
#include "ibmkeys.h"

void main( void )
{
  int c;

#ifdef GCPP_OS2
  GFComOS2 sio( COM2 );
#else
  GFI8250 sio( COM2 );
#endif
#if defined( GCPP_SYMANTEC )
  GFScreenZDisp scrn;
#else
  GFScreenVid scrn;
#endif
  GFVt100 gterm( &scrn, &sio, 255 );
  for( ; ; ) {
    if( gfkbhit() ) {
      c = gterm.GetKey();
      if( c == ESC )
        return;
      else
        gterm << c;
    }
    gterm.GetChar();
  }
} 

//*
//*  EXAMP101.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1989-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example demonstrates the GFAnsiTerminal::GFAnsiTerminal()
//*  constructor, GFAnsiTerminal::GetChar(), and GFAnsiTerminal::GetKey()
//*  functions.  This terminal program will use ANSI terminal emulation.
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
  GFScreenVid scrn;
  GFAnsiTerminal gterm( &scrn, &sio, 255 );
  for(;;) {
    if( scrn.KbdHit() ) {
      c = scrn.GetKey();
      if( c == ESC )
        return;
      gterm.PutChar( (GCPP_BYTE)c );
    }
    gterm.GetChar( 0 );
  }
} 

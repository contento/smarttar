//*
//*  EXAMP102.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1989-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example demonstrates the GFVt52::GFVt52() constructor,
//*  GFVt52::~GFVt52() destructor, GFVt52::GetChar() and GFVt52::GetKey()
//*  functions. This terminal program will use VT52 terminal emulation.
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
  GFVt52 gterm( &scrn, &sio, 255 );
  for( ; ; ) {
    if( gfkbhit() ) {
      c = gterm.GetKey();
      if( c == ESC )
        return;
      else
        gterm.PutChar( (GCPP_BYTE)c );
    }
    gterm.GetChar();
  }
} 

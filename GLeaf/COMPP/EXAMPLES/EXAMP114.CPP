//*
//*  EXAMP114.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1989-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example demonstrates the GFTeletype::GFTeletype() constructor
//*  GFTeletype::~GFTeletype() destructor, GFTeletype::GetKey() and
//*  GFTeletype::GetChar() functions. This terminal program will use
//*  Teletype terminal emulation.
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
  GFTeletype gterm( &scrn, &sio, 255 );
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

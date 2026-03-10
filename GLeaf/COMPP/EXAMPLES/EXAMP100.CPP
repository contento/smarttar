//*
//*  EXAMP100.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1989-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example demonstrates the GFMem::AllocateMemory() and
//*  GFMem::DeallocateMemory() functions.  The new versions of
//*  these functions will be used when memory allocation is
//*  needed.
//*
#include "compplib.hpp"
#include "filexfer.hpp"
#include "ibmkeys.h"
#include "_cppmem.hpp"
#include <iostream.h>
#include <stdlib.h>

void *GFMem::AllocateMemory( unsigned int bytes )
{
  return( calloc( 1, bytes ) );
}
void GFMem::DeallocateMemory( void *p )
{
  free( p );
}

void main( void )
{
  int c;
  int status;

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
        GFXmodem1K txfr( &cp );
        status = txfr.SendFile( "..\\examples\\EXAMP082.CPP" );
        cout << "\nSend file status = " << status << "\n" << flush;
      }
      else
        cp << c;
    }
    cp >> c;
    if( c != GCPP_TIMEOUT )
      cout << (char)c << flush;
  }
} 

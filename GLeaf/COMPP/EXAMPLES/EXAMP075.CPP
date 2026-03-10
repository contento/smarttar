//*
//*  EXAMP075.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1989-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This is a small multi-threaded example using the GFComOS2::GFComOS2()
//*  constructor.  This example starts a thread for reading characters
//*  from the port.  The main thread monitors the keyboard, and when the
//*  ESC key is pressed, the child thread is stopped.
//*
#define INCL_DOS
#include <os2.h>
#include <stdio.h>
#include <process.h>
#include "compplib.hpp"
#include "ibmkeys.h"

#if defined( GCPP_BORLAND ) || defined( GCPP_WATCOM )
typedef void ( *VOID_FPTR )( void *data );

void ReadPort( GFSerial *cp )
#else
void _Optlink ReadPort( GFSerial *cp )
#endif
{
  int c;

  for( ;; ) {
    *cp >> c;
    if( c != GF_TIMEOUT )
      putc( (char)c, stdout );
  }
}


void main( void )
{
  int c;
  TID ct;

  setvbuf( stdout, NULL, _IONBF, 0 );
  GFComOS2 sio( COM1 );
  GFSerial cp( &sio );

#if defined( GCPP_BORLAND ) || defined( GCPP_WATCOM )
  ct = _beginthread( (VOID_FPTR)ReadPort,
#if defined( GCPP_WATCOM )
                     NULL,
#endif
                     1000,
                     (void *)&cp );
#else
  ct = _beginthread( (void(_Optlink *)(void *))ReadPort,
                     NULL,
                     1000,
                     (void *)&cp );
#endif

  for( ;; ) {
    if( gfkbhit() ) {
      if( (c = getkey()) == ESC ) {
        DosKillThread( ct );
        return;
      }
      else
        cp << (char)c;
    }
  }
}

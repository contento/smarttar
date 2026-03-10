//*
//*  EXAMP116.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1989-1995 Greenleaf Software Inc.	All Rights Reserved.
//*
//*  This example demonstrates the GFComWin32::GFComWin32() constructor,
//*  GFComWin32::~GFComWin32() destructor, GFComWin32::GetEventMask(),
//*  GFComWin32::SetEventCharacter(), GFComWin32::SetEventMask(), and
//*  GFComWin32::WaitForEvent(). This is a simple multi-threaded example
//*  that starts a new thread to read data from the port.  The main
//*  process checks for keyboard input and sends the keystrokes out the
//*  port.  This is intended to be a loopback example where characters
//*  sent out the port are directed back to the receiver.
//*
#include "compplib.hpp"
#include "ibmkeys.h"
#include <iostream.h>
#include <stdio.h>
#include <process.h>

int thread_exited = 0;
int time_to_exit = 0;
GFComWin32 *sio;

typedef void ( *VOID_FPTR )( void *data );

void ReadPort( GFSerial *cp )
{
  int c;
  unsigned long mask;
  unsigned long masked_events;
  printf( "Processing read thread...\n" );

  sio->GetEventMask( mask );
  sio->SetEventCharacter( 0x1b );
  sio->SetEventMask( mask | ON_RECEIVE_CHARACTER | ON_RECEIVE_EVENT_CHARACTER );

  for( ;; ) {

    //*
    //* Suspend this thread until a character is received.
    //*
    sio->WaitForEvent( masked_events );

    //*
    //* The event character is used here to wake up the thread
    //* before exiting.  So, if the event character has been
    //* received, it is read from the port and discarded.
    //*
    if ( masked_events & ON_RECEIVE_EVENT_CHARACTER ) {
      cp->GetChar();
      if ( time_to_exit ) {
        thread_exited = 1;
        _endthread();
      }
    } else {

      //*
      //* Loop until there are no more characters to read
      //* or until another process wants to end this thread.
      //*
      while ( ( c = cp->GetChar() ) != GCPP_TIMEOUT ) {
        putc( (char)c, stdout );
        fflush( stdout );
        if ( time_to_exit ) {
          thread_exited = 1;
          _endthread();
        }
      }
    }
    if ( time_to_exit ) {
      thread_exited = 1;
      _endthread();
    }
  }
}


void main( void )
{
  int c;
  GFSerial *cp;

  sio = new GFComWin32( COM1 );
  cp = new GFSerial( sio );

#if defined( GCPP_WATCOM )
  _beginthread( (VOID_FPTR)ReadPort,
                NULL,
                1000,
                (void *)cp );
#else
  _beginthread( (VOID_FPTR)ReadPort,
                1000,
                (void *)cp );
#endif

  for( ;; ) {
    if( gfkbhit() ) {
      if( (c = getkey()) == ESC )
        time_to_exit = 1;
      cp->PutChar( (char)c );
    }
    if ( thread_exited )
      break;
  }
  delete cp;
  delete sio;
}

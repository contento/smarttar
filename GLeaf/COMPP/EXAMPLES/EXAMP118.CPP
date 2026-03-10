//*
//*  EXAMP118.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1989-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example demonstrates the GFNasi::GFNasi() constructor,
//*  GFNasi::~GFNasi() destructor, NasiCloseQuery(), NasiGetSession(),
//*  NasiLoaded(), NasiQueryFirstPort(), NasiQueryNextPort(), and
//*  NasiSetSession() functions.  After setting up a NASI session, the
//*  example searches for the specified port and opens it.  The
//*  example performs like a simple terminal emulator.
//*
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compplib.hpp"
#include "ibmkeys.h"

void emulate( char *name )
{
  GFNasi *sio;
  GFSerial *cp;
  unsigned short error;
  int c;

  sio = new GFNasi( name );
  cp = new GFSerial( sio );
  if( cp->GetCommError( error ) < GCPP_OK ) {
    printf( "Failed to open the port.  \n" );
    delete cp;
    delete sio;
    return;
  }
  for( ; ; ) {
    if( gfkbhit() ) {
      c = getkey();
      if( c == ESC ) {
        delete cp;
        delete sio;
        return;
      }
      cp->PutChar( c );
    }
    c = cp->GetChar();
    if( c != GCPP_TIMEOUT ) {
      putchar( (char) c );
      fflush( stdout );
    }
  }
}

void setup_session( void )
{
  char UserName[ 41 ];
  char Password[ 41 ];
  char Session[ 41 ];

  NasiGetSession( Session );
  if( strlen( Session ) == 0 ) {
    printf( "No session currently defined.\n\n" );
    printf( "UserName:     " );
    scanf( "%s", UserName );
    printf( "Password:     " );
    scanf( "%s", Password );
    printf( "Session Name: " );
    scanf( "%s", Session );
    NasiSetSession( UserName, Password, Session, 1 );
  }
  else
    printf( "Resuming session %s\n", Session );
}

int main( void )
{
  char *names[ 24 ];
  char buf[ 81 ];
  int i;
  NASI_QUERY q;
  int status;
  int number;

  if( !NasiLoaded() ) {
    printf( "Nasi support not present!\n" );
    printf( "Run NASI.EXE to access ports across the network\n" );
    exit( 0 );
  }
  setup_session();
  i = 0;
  printf( "Scanning network for NASI ports..." );
  status = NasiQueryFirstPort( &q );
  printf( "\n" );
  if( status <= 0 ) {
    printf( "I can't find any NASI ports!\n" );
    exit( 1 );
  }
  while( status > 0 && i < 24 ) {
    sprintf( buf,
             "%-9s %-9s %s",
             q.Server,
             q.GeneralPort,
             q.SpecificPort );
    printf( "%d) %s\n", i, buf );
    names[ i++ ] = strdup( buf );
    status = NasiQueryNextPort( &q );
  }
  NasiCloseQuery( &q );
  printf( "\nPick a port number: " );
  if( scanf( "%d", &number ) == 1 && number >= 0 && number < i )
    emulate( names[ number ] );
  while ( --i >= 0 )
    free( names[ i ] );
  return 0;
}

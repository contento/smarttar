//*
//* Usage MINTERMD [port]  e.g MINTERMD 1 for COM1
//*
//*
//* MINTERMD.CPP
//*
//* DESCRIPTION
//*
//* Example of a very basic terminal
//*
//*
//* The Greenleaf Comm++ Library
//*
//* Copyright (C) 1989-95 Greenleaf Software Inc.  All Rights Reserved.
//*
//* MODIFICATIONS
//*
//*

#include <stdio.h>
#include <stdlib.h>
#include "compplib.hpp"
#include "ibmkeys.h"

void main( int argc, char *argv[] )
{
  int c, error, portnum = COM1;
  unsigned short line_err;

  //*
  //* Check for command line arguments.  The first parm is the
  //* port number.
  //*
  setbuf( stdout, NULL );
  if( argc > 1 )
    portnum = atoi( argv[ 1 ] ) - 1 ;

  //*
  //* Open the ComLink class for the port.
  //*
  GFI8250 sio( portnum );

  //*
  //* Open the Serial object for the port.
  //*
  GFSerial cp( &sio );

  //*
  //* Check for any errors opening the port.  If there are any
  //* errors we are going to exit the program.
  //*
  if( (error = cp.GetCommError( line_err )) != 0 ) {
    printf( "Error opening port, error code = %d\n", error );
    return;
  }

  for( ; ; ) {
    if( gfkbhit() ) {
      c = getkey();
      if( c == ESC )
        return;
      else
        cp << c;
    }
    cp >> c;
    if( c != GCPP_TIMEOUT )
      putc( (char)c, stdout );
  }
}

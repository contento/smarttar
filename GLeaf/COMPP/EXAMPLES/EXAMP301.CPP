//*
//*
//*  EXAMP301.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1992-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example demonstrates the functions VidGoto(), VidPrintf(),
//*  VidPutc(), VidPuts(), VidGets() and VidBeep().  This example writes
//*  a formatted string on a window, uses VidPutc() and VidPuts() to
//*  write a character and CR/LF on the window. VidPokef() writes a
//*  formatted string, then the program exits after calling VidBeep().
//*
//*
#include <stdio.h>
#include <stdlib.h>
#include "video.h"
#include "ibmkeys.h"
#include "asciidef.h"

void main( void )
{
  int x = 245;
  int y = 'A';
  char buffer[ 30 ];
  VID_WINDOW *typewin;

  if ( VidInitialize( VID_NORMAL, 1 ) < 0 ) {
      printf(" Failed to initialize " );
      exit( 0 );
  }
  typewin = VidDefineWindow( 2, 1, 20, 75, 1 );
  VidDrawBorder( typewin );
  VidClearWindow( typewin );
  VidGoto( typewin, 3, 1 );
  VidPrintf( typewin,
             "This is a formatted string with the number %d ",
             x );
  VidGoto( typewin, 7, 25 );
  VidPrintf( typewin, "Here we output 'A', CR, LF, 'A' using VidPutc : " );
  VidPutc( typewin, y );
  VidPutc( typewin, CR );
  VidPutc( typewin, LF );
  VidPutc( typewin, y );
  VidGoto( typewin, 10, 2 );
  VidPuts( typewin, "This is a VidPuts() string. " );
  VidGoto( typewin, 4, 2 );
  VidPuts( typewin, "Enter something here: " );
  VidGets( typewin, buffer, 25 );
  VidPokef( typewin, 6, 2, "You typed in %s. ", buffer );
  VidGoto( typewin, 17, 1 );
  VidPrintf( typewin, "Beep!" );
  VidBeep();
  VidGoto( typewin, 18, 1 );
  VidPuts( typewin, "Hit any key to exit..." );
  getkey();
  VidBeep();
  VidTerminate( 1 );
}

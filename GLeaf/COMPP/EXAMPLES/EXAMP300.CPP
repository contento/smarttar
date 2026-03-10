//*
//*  EXAMP300.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1992-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//* This examples initializes the system with VidInitialize(), opens
//* a window with VidDefineWindow() and moves the cursor with VidGoto().
//* The function VidClearWindow() clears the window writes an 'A',
//* carriage return, line feed, and a 'B' before it writes 19 'A's
//* using VidPutc().  Then it uses VidGetLine() to get input from the
//* keyboard and displays this with both VidPokef() and VidPokes().  The
//* function VidPeekWord() is used here and should reflect the first two
//* characters entered. The program resets the video mode to 80x25
//* before it terminates and exits.
//*
//*
#include <stdio.h>
#include <stdlib.h>
#include "video.h"
#include "ibmkeys.h"
#include "asciidef.h"

void main( void )
{
  int y;
  int x = 'A';
  int i;
  char comp[ 27 ];
  VID_WINDOW *typewin;

  if ( VidInitialize( VID_NORMAL, 1 ) < 0 ) {
      printf(" Failed to initialize " );
      exit( 0 );
  }
  VidSet40x25();
  typewin = VidDefineWindow( 1, 1, 20, 38, 1 );
  VidDrawBorder( typewin );
  VidClearWindow( typewin );
  VidGoto( typewin, 3, 0 );
  VidPuts( typewin, "Output A, CR, LF, B with VidPutc:" );
  VidPutc( typewin, x );
  VidPutc( typewin, CR );
  VidPutc( typewin, LF );
  VidPutc( typewin, 'B' );
  VidGetLine( "Company: ", comp, 20 );
  for ( i = 0 ; i < 20 ; i++ )
      VidPokec( typewin, 5, i, x );
  VidPokef( typewin, 10, 1, "Variable i is %d", i );
  VidPokef( typewin, 12, 1, "String entered is %s", comp );
  VidPokes( typewin, 16, 5, comp );
  y = VidPeekWord( typewin, 16, 5 );
  VidGoto( typewin, 18, 0 );
  VidPrintf( typewin, "Peek at 16, 5: %c\n", y & 255 );
  y = VidPeekWord( typewin, 16, 6 );
  VidPrintf( typewin, "Peek at 16, 6: %c", y & 255 );
  getkey();
  VidSet80x25();
  VidTerminate( 1 );
}

//*
//*  EXAMP305.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1992-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example program demonstrates the use of VidClearToEndOfLine(),
//*  VidClearWindow(), VidClearToEndOfWindow(), and VidClearLine().  We
//*  display lines of text on the window, and wait for keyboard input.
//*  This example shows deleting part of a line, an entire line of text,
//*  and the rest of the window.
//*
#include <stdio.h>
#include <stdlib.h>
#include "video.h"
#include "ibmkeys.h"
#include "asciidef.h"

void main( void )
{
  int i;
  VID_WINDOW *typewin;
  char *string = "Greenleaf Software has a C++ Comm Library available!";

  if( VidInitialize( VID_NORMAL, 1 ) < 0 ) {
    printf(" Failed to initialize " );
    exit( 0 );
  }
  VidSelectFastDrivers();
  VidSet80x25();
  typewin = VidDefineWindow( 2, 1, 22, 65, 0 );
  VidDrawBorder( typewin );
  VidClearWindow( typewin );
  VidGoto( typewin, 3, 1 );
  for( i = 1 ; i < 17 ; i++ ) {
    VidGoto( typewin, i, 1 );
    VidPuts( typewin, string );
  }
  VidGoto( typewin, 22, 1 );
  VidPuts( typewin, "Hit any key to delete part of line 4..." );
  getkey();
  VidGoto( typewin, 4, 15 );
  VidClearToEndOfLine( typewin );
  VidGoto( typewin, 22, 1 );
  VidPuts( typewin, "Press another key to delete all of line 3..." );
  getkey();
  VidClearLine( typewin, 3 );
  VidGoto( typewin, 22, 1 );
  VidPuts( typewin, "Press another key to delete from line 6...  " );
  getkey();
  VidGoto( typewin, 7, 0 );
  VidClearToEndOfWindow( typewin );
  VidGoto( typewin, 22, 1 );
  VidPuts( typewin, "Press any key to exit program...            " );
  getkey();
  VidTerminate( 1 );
  exit( 0 );
}

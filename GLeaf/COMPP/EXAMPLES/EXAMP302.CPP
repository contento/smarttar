//*
//*
//*  EXAMP302.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1992-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example program demonstrates the use of the functions:
//*  VidSet80x50() and VidSet80x28().  The program writes many lines
//*  of text to the window then you may toggle between 80x50 line
//*  or 80x28 line mode by using F1 or F2 keys.  ESC will exit and
//*  return you to 80x25 line mode.
//*
//*
#include <stdio.h>
#include <stdlib.h>
#include "video.h"
#include "ibmkeys.h"
#include "asciidef.h"

void main( void )
{
  int i;
  int x;
  VID_WINDOW *typewin;

  if( VidInitialize( VID_NORMAL, 1 ) < 0 ) {
    printf(" Failed to initialize " );
    exit( 0 );
  }
  VidSelectFastDrivers();
  VidSet80x43();
  typewin = VidDefineWindow( 2, 1, 22, 65, 1 );

  for ( ; ; )  {
    VidClearWindow( typewin );
    VidGoto( typewin, 3, 1 );
    VidDrawBorder( typewin );
    for( i = 1 ; i < 17 ; i++ ) {
      VidGoto( typewin, i, 1 );
      VidPuts( typewin,
              "Greenleaf Software has a C++ Comm Library available!" );
    }
    VidGoto( typewin, 22, 1 );
    VidPuts( typewin, "Press ESC to exit program, F1 or F2 to change.  " );
    x = getkey();
    if( x == ESC )  {
      VidSet80x25();
      VidTerminate( 1 );
      exit( 0 );
    }
    if( x == F1 )
      VidSet80x50();
    if( x == F2 )
      VidSet80x28();
  }
}

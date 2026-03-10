//*
//*
//*  EXAMP308.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1992-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example program demonstrates the use of VidSetAttribute(),
//*  VidSaveWindow(), VidRestoreWindow(). This example changes the
//*  video mode to 80x43 then writes some text to the window.  The
//*  window is then saved, cleared and restored again.  Other
//*  functions used here include VidSet80x43(), VidSet80x25(),
//*  VidSetAttribute(), VidPhysicalGoto(), VidClearWindow(),
//*  VidDrawBorder(), VidGoto(), VidDefineWindow(), VidPuts(),
//*  VidTerminate(), VidHideCursor(), and VidShowCursor().
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
  VID_WINDOW *typewin;
  char *string = "Greenleaf Software has a C++ Comm Library available!";

  if( VidInitialize( VID_NORMAL, 1 ) < 0 ) {
    printf( " Failed to initialize " );
    exit( 0 );
  }
  VidSet80x43();
  typewin = VidDefineWindow( 2, 1, 40, 65, 0 );
  VidDrawBorder( typewin );
  VidClearWindow( typewin );
  VidGoto( typewin,8, 1 );
  VidSetAttribute( typewin, VID_REVERSE );
  for( i = 1 ; i < 30 ; i++ ) {
    VidGoto( typewin, i, 1 );
    VidPuts( typewin, string );
  }
  VidGoto( typewin, i + 1, 1 );
  VidPuts( typewin, "Press any key to save the screen and hide the cursor" );
  getkey();
  VidHideCursor();
  VidSaveWindow( typewin );
  VidClearWindow( typewin );
  VidPhysicalGoto( 42, 1 );
  printf( "The window has been saved!  ...press key to continue " );
  getkey();
  VidRestoreWindow( typewin, 0 );
  VidPhysicalGoto( 42, 1 );
  printf("The window has been restored!  ...press key to exit   " );
  getkey();
  VidSet80x25();
  VidShowCursor();
  VidTerminate( 1 );
  exit( 0 );
}

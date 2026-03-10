//*
//*  EXAMP304.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1992-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example program demonstrates the use of the VidSet80x25()
//*  and VidSet80x43() functions.  We write lines of text to the
//*  window and wait for the user to press a key.  The F1 and F2
//*  keys are used to change the video mode, and ESC is used to
//*  exit the program.  When the video mode is changed, the screen
//*  is updated.
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
  char *string = "Greenleaf Software has a C++ Comm Library available!";

  if( VidInitialize( VID_NORMAL, 1 ) < 0 ) {
    printf(" Failed to initialize " );
    exit( 0 );
  }
  VidSelectFastDrivers();
  VidSet80x43();
  typewin = VidDefineWindow( 2, 1, 22, 65, 1 );
  VidClearWindow( typewin );
  VidGoto( typewin, 3, 1 );
  for( ; ; )  {
    VidClearWindow( typewin );
    for( i = 1 ; i < 17 ; i++ ) {
      VidDrawBorder( typewin );
      VidGoto( typewin, i, 1 );
      VidPuts( typewin, string );
    }
    VidGoto( typewin, i+1, 1 );
    VidPuts( typewin, "F1 = 80x25  F2 = 80x43  ESC = exit " );
    x = getkey();
    if( x == ESC )  {
      VidSet80x25();
      VidTerminate( 1 );
      exit( 0 );
    }
    if( x == F1 )
      VidSet80x25();
    if( x == F2 )
      VidSet80x43();
  }
}

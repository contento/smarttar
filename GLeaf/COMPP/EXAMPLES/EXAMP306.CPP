//*
//*  EXAMP306.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1992-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example program demonstrates the use of VidScrollUp() and
//*  VidScrollDown().  This example displays several lines of text
//*  in a window then allows the user to scroll up or down with using
//*  the F1 or F2 keys.  Text that is scrolled off the window is lost.
//*  VidPhysicalGoto() is called to move the cursor just before printf().
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
  for( i = 1 ; i < 30; i++ ) {
    VidGoto( typewin, i, 1 );
    VidPuts( typewin, string );
  }
  VidPhysicalGoto( 42, 1 );
  printf(" Use F1 to Scroll Up - F2 to Scroll Down - ESC to Quit " );
  for( ; ; ) {
    if( gfkbhit() ) {
      x = getkey();
      if( x == ESC ) {
        VidSet80x25();
        VidTerminate( 1 );
        exit( 0 );
      }
      if( x == F1 )
        VidScrollUp( typewin, 2 );
        if( x == F2 )
          VidScrollDown( typewin, 2 );
    }
  }
}

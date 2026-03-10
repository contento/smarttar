//*
//*
//*  EXAMP307.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1992-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example program demonstrates VidSelectFastDrivers() and
//*  VidSelectBIOSDrivers().  The example sets the video mode to
//*  80x43, displays several lines of text in the window each time
//*  that F1 or F2 is hit. Toggle F1 and F2 key to see the difference
//*  in the BIOS and Fast Drivers.  The video mode is reset before
//*  exiting.
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

  if ( VidInitialize( VID_NORMAL, 1 ) < 0 ) {
      printf( " Failed to initialize " );
      exit( 0 );
  }

  VidSet80x43();
  typewin = VidDefineWindow( 2, 1, 40, 65, 0 );
  VidDrawBorder( typewin );
  VidClearWindow( typewin );
  VidGoto( typewin,8, 1 );
  for( i = 1 ; i < 30 ; i++ ) {
    VidGoto( typewin, i, 1 );
    VidPuts( typewin,  string );
  }
  VidPhysicalGoto( 42, 1 );
  printf( " F1 to Use BIOS Drivers -"
          " F2 to use Fast Drivers -"
          " ESC to Quit " );
  for( ; ; ) {
    if( gfkbhit() ) {
      x = getkey();
      if( x == ESC ) {
        VidSet80x25();
        VidTerminate( 1 );
        exit( 0 );
      }
      if( x == F1 ) {
        VidClearWindow( typewin );
        VidSelectBIOSDrivers();
        for( i = 3 ; i < 33 ; i++ ) {
          VidGoto( typewin, i, 1 );
          VidPuts( typewin, string );
        }
      }
      if( x == F2 ) {
        VidClearWindow( typewin );
        VidSelectFastDrivers();
        for( i = 1 ; i < 30 ; i++ ) {
          VidGoto( typewin, i, 1 );
          VidPuts( typewin, string );
        }
      }
    }
  }
}

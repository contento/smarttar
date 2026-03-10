//*
//*  EXAMP309.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1992-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example opens a window, writes to it, then gets the
//*  physical position of the hardware cursor with the function
//*  VidReadPhysicalPosition().  The row and col values of the
//*  cursor position are then displayed on the window.  Following
//*  that, the cursor position is changed with VidPhysicalGoto().
//*  The new position is obtained and displayed on the window.
//*
//*
#include <stdio.h>
#include <stdlib.h>
#include "video.h"
#include "ibmkeys.h"
#include "asciidef.h"

void main( void )
{
  unsigned char row;
  unsigned char col;
  VID_WINDOW *typewin;

  if( VidInitialize( VID_NORMAL, 1 ) < 0 ) {
    printf(" Failed to initialize " );
    exit( 0 );
  }
  typewin = VidDefineWindow( 0, 0, 20, 75, 0 );
  VidClearWindow( typewin );
  VidGoto( typewin, 3, 1 );
  VidPuts( typewin, "Greenleaf Software has C and C++ libraries." );
  VidReadPhysicalPosition( &row, &col );
  printf( "\nThe position is row = %d col = %d .\n", row, col );
  printf( "Press enter to continue" );
  getkey();
  VidPhysicalGoto( 10, 1 );
  VidReadPhysicalPosition( &row, &col );
  printf( "The position is row = %d col = %d .\n", row, col );
  getkey();
  VidPhysicalGoto( 15, 10 );
  VidReadPhysicalPosition( &row, &col );
  printf("The position is row = %d col = %d .\n", row, col );
  getkey();
  VidTerminate( 1 );
  exit( 0 );
}

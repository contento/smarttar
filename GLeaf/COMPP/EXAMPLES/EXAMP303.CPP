//*
//*  EXAMP303.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1992-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example program demonstrates the use of the VidMenu()
//*  function.  This example creates a simple menu from which you
//*  can select an item get the menu item string.
//*
//*
#include <stdio.h>
#include <stdlib.h>
#include "video.h"
#include "ibmkeys.h"
#include "asciidef.h"

void main( void )
{
  int choice;
  char *list[ 5 ];
  VID_WINDOW *typewin;

  list[ 0 ] = "First";
  list[ 1 ] = "Second";
  list[ 2 ] = "Third";
  list[ 3 ] = "Quit";
  list[ 4 ] = "";
  if( VidInitialize( VID_NORMAL, 1 ) < 0 ) {
    printf(" Failed to initialize " );
    exit( 0 );
  }
  typewin = VidDefineWindow( 2, 1, 20, 75, 1 );
  VidDrawBorder( typewin );
  VidClearWindow( typewin );
  VidGoto( typewin, 3, 1 );

  choice = VidMenu( 4, 4, list );
  VidGoto( typewin, 18, 1 );
  if( choice == -1 )
    VidPrintf( typewin, "No choice was made when ESC was pressed." );
  else
    VidPrintf( typewin, "Your choice is %s. ", list[ choice ] );
  getkey();
  VidTerminate( 1 );
}

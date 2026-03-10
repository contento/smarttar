//*
//*  EXAMP310.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1992-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example program demonstrates the use of VidSetWindowPage(),
//*  VidSelectPage(), VidSetAttribute() and VidFreeVid().  We define
//*  three windows and associate each with its own video page.  The F1,
//*  F2, and F3 keys are used to toggle the video page. Notice that the
//*  attribute is different in Window 1.  Before exiting, page 0 is
//*  selected to avoid problems that may arise.
//*
//*
#include <stdio.h>
#include <stdlib.h>
#include "video.h"
#include "ibmkeys.h"
#include "asciidef.h"

void main( void )
{
  int x;
  VID_WINDOW *winone;
  VID_WINDOW *wintwo;
  VID_WINDOW *winthree;

  printf( "Hit F1 or F2 or F3 to Select Windows\n" );
  printf( "... hit any key to continue " );
  getkey();
  if( VidInitialize( VID_NORMAL, 1 ) < 0 ) {
    printf( " Failed to initialize " );
    exit( 0 );
  }
  winone = VidDefineWindow( 1, 1, 9, 40, 1 );
  VidSetWindowPage( winone, 1 );
  VidDrawBorder( winone );
  VidClearWindow( winone );
  VidSetAttribute( winone, VID_REVERSE );
  VidPuts( winone, "This is Window # 1 " );
  wintwo = VidDefineWindow( 10, 1, 23, 60, 1 );
  VidSetWindowPage( wintwo, 2 );
  VidClearWindow( wintwo );
  VidPuts( wintwo, "This is Window # 2 " );
  winthree = VidDefineWindow( 1, 50, 8, 75, 1 );
  VidSetWindowPage( winthree, 3 );
  VidClearWindow( winthree );
  VidPuts( winthree, "This is Window # 3 " );
  for( ; ; ) {
    if( gfkbhit() ) {
      x = getkey();
      if( x == ESC ) {
        VidSelectPage( 0 );
        VidTerminate( 1 );
        VidFreeVid( winone );
        VidFreeVid( wintwo );
        VidFreeVid( winthree );
        exit( 0 );
      }
      if( x == F1 )
        VidSelectPage( 1 );
      if( x == F2 )
        VidSelectPage( 2 );
      if( x == F3 )
        VidSelectPage( 3 );
    }
  }
}

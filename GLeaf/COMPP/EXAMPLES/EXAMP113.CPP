//*
//*  EXAMP113.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1989-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example demonstrates the GFScreenVid::SetColumns() and
//*  GFScreenVid::SetRegion() functions.
//*
#include <stdio.h>
#include "comppdsp.hpp"
#include "ibmkeys.h"

void main( void )
{
  unsigned key;

  GFScreenVid scrn;
  scrn.SetColumns( 60 );
  scrn.SetRegion( 5, 20 );

  do {
    key = getkey();
    switch( key ) {
      case CURDN:
        scrn.CursorDown( 1 );
        break;
      case CURRT:
        scrn.CursorRight( 1 );
        break;
      case CURLF:
        scrn.CursorLeft( 1 );
        break;
      case CURUP:
        scrn.CursorUp( 1 );
        break;
      default:
        scrn.PutChar( (char)key );
    }
  } while( key != ESC );
}

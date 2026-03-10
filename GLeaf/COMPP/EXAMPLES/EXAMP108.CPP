//*
//*  EXAMP108.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1989-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example demonstrates the GFScreenDriver::Origin()
//*  function.  The example will change the origin and then
//*  work within the confines of the new origin.
//*
#include "comppdsp.hpp"
#include "ibmkeys.h"

void main( void )
{
  unsigned key;

#ifdef GCPP_SYMANTEC
  GFScreenZDisp scrn;
#else
  GFScreenVid scrn;
#endif

  scrn.MaxSize( 15, 60 );
  scrn.Origin( 10, 5 );
  scrn.Color( 15, 1 );
  scrn.Clear();

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
      case HOME:
        scrn.CursorHome();
      default:
        scrn.PutChar( (char)key );
    }
    scrn.Flush();
  } while( key != ESC );
}

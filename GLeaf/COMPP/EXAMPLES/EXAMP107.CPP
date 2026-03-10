//*
//*  EXAMP107.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1989-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example demonstrates the GFScreenVid::GetChar(),
//*  GFScreenVid::GetCols(), GFScreenVid::GetRows(),
//*  GFScreenVid::Printf(), GFScreenVid::ReadColor() and
//*  GFScreenVid::WhereAmI() functions.  The example allows
//*  the user to query the screen functions for the system
//*  dependent information.
//*
#include "comppdsp.hpp"
#include "ibmkeys.h"

void main( void )
{
  unsigned key;
  int row, col;
  int fg, bg;
  unsigned short data;

  GFScreenVid scrn;

  scrn.Color( 15, 1 );
  do {
    key = getkey();
    switch( key ) {
      case F1:
        scrn.Printf( "\nThe total rows and columns are %d and %d",
            scrn.GetRows(), scrn.GetCols() );
        scrn.WhereAmI( row, col );
        scrn.Printf( "\nThe current row and columns are %d and %d",
            row, col );
        break;
      case F2:
        data = scrn.GetChar();
        scrn.Printf( "\nThe character is '%c'", (data & 0x00ff) );
        scrn.Printf( "\nThe foreground attribute is %d",
            ((data >> 8) & 0x000f) );
        scrn.Printf( "\nThe background attribute is %d",
            ((data >> 12) & 0x000f) );
        break;
      case F3:
        scrn.ReadColor( fg, bg );
        scrn.Printf( "\nThe foreground attribute is %d", fg );
        scrn.Printf( "\nThe background attribute is %d", bg );
        break;
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
    scrn.Flush();
  } while( key != ESC );
}

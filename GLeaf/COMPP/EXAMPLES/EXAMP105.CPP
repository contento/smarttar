//*
//*  EXAMP105.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1989-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example demonstrates the GFScreenDriver::PutString(),
//*  GFScreenDriver::Clear(), GFScreenDriver::MoveTo(),
//*  GFScreenDriver::DeleteLine(), GFScreenDriver::InsertLine(),
//*  GFScreenDriver::InsertChar(), GFScreenDriver::DeleteChar(), and
//*  GFScreenDriver::PutChar() functions.  The example displays
//*  a poem incorrectly and then uses the screen
//*  functions to edit the poem into its correct order.
//*
#include "comppdsp.hpp"
#include "ibmkeys.h"

void main( void )
{
  int i;

#ifdef GCPP_SYMANTEC
  GFScreenZDisp scrn;
#else
  GFScreenVid scrn;
#endif

  scrn.Color( 15, 1 );
  scrn.Clear();
  scrn.MoveTo( 5, 3 );
  scrn.PutString( "Humpty Dumpty tas on the wall" );
  scrn.MoveTo( 6, 3 );
  scrn.PutString( "Humpty Dumpty had a great fall" );
  scrn.MoveTo( 7, 3 );
  scrn.PutString( "Could not put Humpty back together again" );
  getkey();
  scrn.MoveTo( 5, 17 );
  for( i = 0; i < 3; i++ )
    scrn.DeleteChar();
  for( i = 0; i < 3; i++ ) {
    scrn.MoveTo( 5, 17 + i );
    scrn.InsertChar();
  }
  scrn.MoveTo( 5, 17 );
  scrn.PutChar( 's' );
  scrn.PutChar( 'a' );
  scrn.PutChar( 't' );
  getkey();
  scrn.MoveTo( 7, 3 );
  scrn.InsertLine();
  scrn.PutString( "All the king's horses and all the king's men" );
  scrn.MoveTo( 8, 43 );
  getkey();
  scrn.MoveTo( 5, 3 );
  for( i = 0; i < 4; i++ )
    scrn.DeleteLine();
}

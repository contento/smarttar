//*
//*  EXAMP106.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1989-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example demonstrates the GFScreenDriver::Erase2EOL(),
//*  GFScreenDriver::Erase2EOP(), GFScreenDriver::Erase2SOP(),
//*  GFScreenDriver::Erase2SOL() and GFScreenDriver::Scroll()
//*  functions.  The example will fill the screen with data and
//*  then remove the data using the different screen functions.
//*
#include "comppdsp.hpp"
#include "ibmkeys.h"

void main( void )
{
  int i, j;

#ifdef GCPP_SYMANTEC
  GFScreenZDisp scrn;
#else
  GFScreenVid scrn;
#endif
  scrn.Color( 15, 1 );
  scrn.Clear();
  for( i = 0; i < 25; i++ )
    for( j = 0; j < 80; j++ )
      scrn.PutChar( (char)(65+i) );
  for( i = 0; i < 3; i++ )
    scrn.Scroll( 1 );
  for( i = 0; i < 3; i++ )
    scrn.Scroll( 0 );
  for( i = 0; i < 6; i++ )
    scrn.Scroll( 1 );
  for( i = 0; i < 3; i++ )
    scrn.Scroll( 0 );
  getkey();
  scrn.MoveTo( 12, 40 );
  scrn.Erase2EOL();
  scrn.Erase2SOL();
  getkey();
  scrn.MoveTo( 13, 40 );
  scrn.Erase2EOL();
  scrn.Erase2SOL();
  getkey();
  scrn.Erase2EOP();
  scrn.Erase2SOP();
}

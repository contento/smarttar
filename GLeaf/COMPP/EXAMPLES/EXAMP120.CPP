//*
//*  EXAMP120.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1989-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example demonstrates the GFScreenDriver::SetRegion(),
//*  GFScreenDriver::Index() and GFScreenDriver::ReverseIndex()
//*  functions.  The example will fill the screen with data and then
//*  scroll the specified region.
//*
#include "comppdsp.hpp"
#include "ibmkeys.h"

void main( void )
{
  unsigned int key;
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
  scrn.SetRegion( 10, 22 );
  for( ; ;  ) {
    key = getkey();
    switch( key ) {
      case CURDN:
        scrn.Index();
        break;
      case CURUP:
        scrn.ReverseIndex();
        break;
      case ESC:
        return;
      default:
        break;
    }
    scrn.Flush();
  }
}

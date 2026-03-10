//*
//*  EXAMP002.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1989-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example demonstrates the GFBIOS::SystemIdle() function.
//*
#include "compplib.hpp"
#include "ibmkeys.h"
#include <iostream.h>

class MyGFBIOS : public GFBIOS
{
  public:
    MyGFBIOS( int comport ) : GFBIOS( comport )
      { GCPP_UNUSED_PARAMETER( comport ); }
    int SystemIdle( void )
      { cout << "."; return( GCPP_OK ); }
};

void main( void )
{
  int c;
  MyGFBIOS sio( COM1 );
  GFSerial cp( &sio );

  for( ; ; ) {
    if( gfkbhit() ) {
      c = getkey();
      if( c == ESC )
        return;
      else
        cp.PutChar( (unsigned char)c );
    }
    c = cp.GetChar( 50 );
    if( c != GCPP_TIMEOUT )
      cout << (char)c << flush;
  }
} 

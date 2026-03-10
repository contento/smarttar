//*
//*  EXAMP007.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1989-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example demonstrates the GFExtendedBIOS constructor and
//*  GFExtendedBIOS::SystemIdle() function.
//*
#include "compplib.hpp"
#include "ibmkeys.h"
#include <iostream.h>

class MyBIOS : public GFExtendedBIOS
{
  public:
    MyBIOS( int comport ) : GFExtendedBIOS( comport )
      { GCPP_UNUSED_PARAMETER( comport ); }
    int SystemIdle( void )
      { cout << "."; return( GCPP_OK ); }
};

void main( void )
{
  int c;

  MyBIOS sio( COM2 );
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

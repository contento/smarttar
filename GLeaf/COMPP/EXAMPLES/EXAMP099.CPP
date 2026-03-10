//*
//*  EXAMP099.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1989-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example demonstrates the IsDESQViewRunning(),
//*  IsWindowsEnhancedMode(), SysYieldDESQViewTimeSlice(), and
//*  SysYieldWindowsTimeSlice() functions.
//*
#include "compplib.hpp"
#include "ibmkeys.h"
#include <iostream.h>

class MyComLink : public GFI8250
{
  public:
    MyComLink( int port ) : GFI8250( port )
      { GCPP_UNUSED_PARAMETER( port ); }
    int SystemIdle( void )
      { if( IsWindowsEnhancedMode() )
          SysYieldWindowsTimeSlice();
        else if( IsDESQViewRunning() )
          SysYieldDESQViewTimeSlice();
        return( GCPP_OK );  }
};
  
void main( void )
{
  int c;

  MyComLink sio( COM2 );
  GFSerial cp( &sio );
  for(;;) {
    if( gfkbhit() ) {
      c = getkey();
      if( c == ESC )
        return;
      else
        cp.PutChar( (unsigned char)c );
    }
    c = cp.GetChar( 55 );
    if( c != GCPP_TIMEOUT )
      cout << (char)c << flush;
  }
}

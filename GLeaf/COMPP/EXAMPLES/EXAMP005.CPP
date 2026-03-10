//*
//*  EXAMP005.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1989-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example demonstrates the GFComWin3 constructor, destructor,
//*  and SystemIdle() routines.
//*
#include "compplib.hpp"
#include "tw.h"

#ifdef GCPP_WINDOWS

TW_WINDOW *MainWin;

class MyComLink : public GFComWin3
{
  public:
    MyComLink( int port ) : GFComWin3( port )
      { GCPP_UNUSED_PARAMETER( port ); }
    int SystemIdle( void )
      { TWYield(); return( GCPP_OK ); }
};
  
int PASCAL WinMain( HINSTANCE hInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpszCmdLine,
                    int nCmdShow )
{
  int c;
  GCPP_UNUSED_PARAMETER( lpszCmdLine );

  MyComLink sio( COM2 );
  GFSerial cp( &sio );

  if( !TWInitialize( hInstance, hPrevInstance, nCmdShow, "EXAMP005.CPP" ) )
    return 0;
  MainWin = TWDefineWindow( 0, 10, 60, 25, 1, "MainWindow", "Courier New", 10 );
  for( ;; ) {
    if( TWKbhit( MainWin ) ) {
      c = TWGetkey( MainWin );
      if( c == -1 )
        break;
      else
        cp.PutChar( (unsigned char)c );
    }
    c = cp.GetChar( 100 );
    if( c != GCPP_TIMEOUT )
      TWPutc( MainWin, c );
  }
  return( 0 );
}
#endif

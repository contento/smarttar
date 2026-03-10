//*
//*  EXAMP110.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1989-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example demonstrates the GFScreenTW::GFScreenTW() constructor,
//*  the GFScreenTW::~GFScreenTW() destructor and the TWYield() function.
//*
//*  Note: This is a MS Windows example. When linking, use the Comm++
//*  Windows library.
//*

#define STRICT
#include <windows.h>
#include "comppdsp.hpp"
#include "compplib.hpp"
#include "tw.h"


class TestWin : public GFComWin3
{
  public:
    TestWin( int com ) : GFComWin3( com )
      {}
    ~TestWin( void )
      {}
    int SystemIdle( void )
      { TWYield(); return( GCPP_OK ); }
};

int PASCAL WinMain( HINSTANCE hInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpszCmdLine,
                    int nCmdShow )
{
  GFScreenTW *MainWindow;
  int c;

  GCPP_UNUSED_PARAMETER( lpszCmdLine );

  MainWindow = new GFScreenTW( hInstance,
                               hPrevInstance,
                               nCmdShow,
                               "Comm++  v 2.0",
                               "Comm++  EXAMP110",
                               1,
                               1 );

  MainWindow->PutString( "Greenleaf Software, Inc.\n" );
  MainWindow->PutString( "Now does Windows!\n" );
  MainWindow->PutString( "Hit the ESCape key to exit..." );

  TestWin sio( COM2 );
  GFSerial cp( &sio );

  while( !TWAppClosed ) {
    if( MainWindow->KbdHit() ) {
      c = MainWindow->GetKey();
      if( c == 27 )
        break;
      else
        cp.PutChar( (unsigned char)c );
    }
    c = cp.GetChar( 20 );
    if( c != GCPP_TIMEOUT )
      MainWindow->PutChar( (GCPP_BYTE)c );
  }
  delete MainWindow;
  return( 0 );
}

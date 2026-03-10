//*
//*  EXAMP316.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1992-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example opens a window with TWDefineWindow() and
//*  demonstrates the TWInsertChar() and TWDeleteChar()
//*  functions.
//*
//*
#include "tw.h"

#ifdef GCPP_WINDOWS

int PASCAL WinMain( HINSTANCE hInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpszCmdLine,
                    int nCmdShow )
{
  TW_WINDOW *MainWindow;
  int i;

  GCPP_UNUSED_PARAMETER( lpszCmdLine );

  if( !TWInitialize( hInstance,
                     hPrevInstance,
                     nCmdShow,
                     "Comm++ Example 316" ) )
    return( 0 );
  MainWindow = TWDefineWindow( 0,
                               0,
                               40,
                               15,
                               1,
                               "MainWindow",
                               "Courier New",
                               14 );
  for( i = 0 ; i < 5 ; i++ ) {
    TWSetAttribute( MainWindow, ( i & 1 ) ? 0x0f : 0xf0 );
    TWPokes( MainWindow,
             5 + i,
             5 + i,
             "Greenleaf Text Windows!" );
  }
  TWGoto( MainWindow, 12, 0 );
  TWPuts( MainWindow, "Hit any key to insert a character..." );
  TWGetkey( MainWindow );
  TWSetAttribute( MainWindow, 0xf0 );
  TWInsertChar( MainWindow, 5, 10, '!' );
  TWPuts( MainWindow, "\nHit any key to delete it..." );
  TWGetkey( MainWindow );
  TWSetAttribute( MainWindow, 0x0f );
  TWDeleteChar( MainWindow, 5, 10 );
  TWPuts( MainWindow, "\nHit any key to exit..." );
  TWGetkey( MainWindow );
  return( 0 );
}
#endif

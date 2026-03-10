//*
//*  EXAMP313.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1992-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example opens a window with TWDefineWindow() and demonstrates
//*  the TWScrollUp() and TWScrollDown() functions.
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
                     "Comm++ Example 313" ) )
    return( 0 );
  MainWindow = TWDefineWindow( 0,
                               10,
                               60,
                               25,
                               1,
                               "MainWindow",
                               "Courier New",
                               14 );
  for( i = 0 ; i < 10 ; i++ )
    TWPrintf( MainWindow,
              "Greenleaf Text Windows.  Line %02d:\n", i );
  TWPuts( MainWindow, "\nHit any key to scroll up by 5" );
  TWGetkey( MainWindow );
  TWScrollUp( MainWindow, 5 );
  TWPuts( MainWindow, "\nHit another key to scroll down by 5" );
  TWGetkey( MainWindow );
  TWScrollDown( MainWindow, 5 );
  TWPuts( MainWindow, "\nHit another key to exit..." );
  TWGetkey( MainWindow );
  return( 0 );
}

#endif

//*
//*  EXAMP315.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1992-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example opens a window with TWDefineWindow() and
//*  demonstrates the TWSetAttribute(), TWGoto(), TWPokef()
//*  and TWPokes() functions.
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
                     "Comm++ Example 315" ) )
    return( 0 );
  MainWindow = TWDefineWindow( 0,
                               0,
                               100,
                               25,
                               1,
                               "MainWindow",
                               "Courier New",
                               10 );
  for( i = 0 ; i < 5 ; i++ ) {
    TWSetAttribute( MainWindow, ( i & 1 ) ? 0x0f : 0xf0 );
    TWPokes( MainWindow,
             10 + i,
             5 + i,
             "Greenleaf Text Windows!" );
  }
  TWGoto( MainWindow, 20, 1 );
  TWPuts( MainWindow, "\nHit any key for more..." );
  TWGetkey( MainWindow );
  for( i = 0 ; i < 5 ; i++ ) {
    TWSetAttribute( MainWindow, ( i & 1 ) ? 0x0f : 0xf0 );
    TWPokef( MainWindow,
             14 - i,
             35 + i,
             "%s is on line number %d",
             (LPSTR) "Greenleaf Text Windows!", 14 - i );
  }
  TWGetkey( MainWindow );
  return( 0 );
}

#endif

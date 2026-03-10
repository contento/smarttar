//*
//*  EXAMP314.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1992-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example opens two windows with TWDefineWindow() and
//*  demonstrates the TWSetAttribute(), TWSetDefaultAttribute(),
//*  TWClearVirtualTerminal(), and TWClearWindow() functions.
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
  TW_WINDOW *ExtraWindow;
  int i;

  GCPP_UNUSED_PARAMETER( lpszCmdLine );

  if( !TWInitialize( hInstance,
                     hPrevInstance,
                     nCmdShow,
                     "Comm++ Example 314" ) )
    return( 0 );
  ExtraWindow = TWDefineWindow( 0,
                                0,
                                76,
                                23,
                                1,
                                "Extra Window",
                                "Fixedsys",
                                12 );
  MainWindow = TWDefineWindow( 5,
                               10,
                               35,
                               10,
                               0,
                               "MainWindow",
                               "Courier New",
                               20 );
  for( i = 0 ; i < 40 ; i++ ) {
    TWSetAttribute( MainWindow, ( i & 1 ) ? 0x0f : 0xf0 );
    TWPrintf( MainWindow,
              "Greenleaf Text Windows.  Line %02d\n", i );
  }
  TWPuts( MainWindow, "Hit any key to clear the virtual window..." );
  TWGetkey( MainWindow );
  TWClearVirtualTerminal( MainWindow );
  TWPuts( MainWindow, "\nHit any key to print to extra window..." );
  TWGetkey( MainWindow );
  TWSetDefaultAttribute( ExtraWindow, 0x0f );
  for( i = 0 ; i < 100 ; i++ ) {
    TWPrintf( ExtraWindow,
              "Greenleaf, the only way to do Windows.  Line %02d\n", i );
  }
  TWPuts( MainWindow, "\nHit any key to clear the extra window..." );
  TWGetkey( MainWindow );
  TWClearWindow( ExtraWindow );
  TWPuts( MainWindow, "\nHit any key to exit..." );
  TWGetkey( MainWindow );
  return( 0 );
}

#endif

//*
//*  EXAMP317.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1992-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example opens a window with TWDefineWindow() and
//*  demonstrates the TWClearToEndOfLine(), TWClearToEndOfWindow() and
//*  TWPokes() functions.
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
                     "Comm++ Example 317" ) )
    return( 0 );
  MainWindow = TWDefineWindow( 0,
                               0,
                               70,
                               15,
                               1,
                               "MainWindow",
                               "Courier New",
                               14 );
  for( i = 0 ; i < 13 ; i++ )
    TWPokes( MainWindow, 4, i*10, "0123456789" );
  TWPokes( MainWindow, 4, 130, "01" );

  for( i = 0 ; i < 5 ; i++ ) {
    TWSetAttribute( MainWindow, ( i & 1 ) ? 0x0f : 0xf0 );
    TWPokes( MainWindow,
             5 + i,
             45 + i,
             "Greenleaf Text Windows!" );
  }
  TWGoto( MainWindow, 5, 0 );
  TWPuts( MainWindow, "Hit any key to delete to end of line..." );
  TWGetkey( MainWindow );
  TWClearToEndOfLine( MainWindow,
                      MainWindow->VTWCaretRow,
                      MainWindow->VTWCaretCol );
  TWPuts( MainWindow, "\nHit any key to delete to end of window..." );
  TWGetkey( MainWindow );
  TWClearToEndOfWindow( MainWindow,
                        MainWindow->VTWCaretRow,
                        MainWindow->VTWCaretCol );
  TWGoto( MainWindow, 3, 0 );
  TWPuts( MainWindow, "Hit any key to exit this example program..." );
  TWGetkey( MainWindow );
  return( 0 );
}
#endif

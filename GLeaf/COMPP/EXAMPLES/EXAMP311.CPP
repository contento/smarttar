//*
//*  EXAMP311.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1992-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example opens a window with TWDefineWindow() and writes to the
//*  window using TWPuts() and TWPutc().  This program will take keyboard
//*  input and display it in the window.  The ESC key is used to exit
//*  the program, freeing instance data using TWFreeInstanceData() and the
//*  window using TWFreeWindow().
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
  int c;

  GCPP_UNUSED_PARAMETER( lpszCmdLine );

  if( !TWInitialize( hInstance,
                     hPrevInstance,
                     nCmdShow,
                     "Comm++ Example 311" ) )
    return( 0 );
  MainWindow = TWDefineWindow( 0,
                               20,
                               40,
                               25,
                               0,
                               "MainWindow",
                               "Fixedsys",
                               12 );
  TWPuts( MainWindow, "Greenleaf Software, Inc.\n" );
  TWPuts( MainWindow, "Now does Windows!\n" );
  TWPuts( MainWindow, "Hit the ESCape key to exit..." );
  while( !TWAppClosed ) {
    c = TWGetkey( MainWindow );
    if( c == 27 )
      break;
    TWPutc( MainWindow, c );
  }
  if( MainWindow ) {
    TWFreeInstanceData();
    TWFreeWindow( MainWindow );
  }
  return( 0 );
}

#endif

//*
//*  EXAMP318.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1992-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example opens a window with TWDefineWindow() and
//*  demonstrates TWGetLine().
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
  char my_string[ 81 ];

  GCPP_UNUSED_PARAMETER( lpszCmdLine );

  if( !TWInitialize( hInstance,
                      hPrevInstance,
                      nCmdShow,
                      "Comm++ Example 318" ) )
    return( 0 );
  MainWindow = TWDefineWindow( 0,
                               0,
                               70,
                               15,
                               1,
                               "MainWindow",
                               "Courier New",
                               14 );
  TWGetLine( "Example 318",
             "Please input a string with 40 or fewer chars",
             my_string,
             40 );
  TWPuts( MainWindow, "\n\nYou entered: " );
  TWSetAttribute( MainWindow, 0x0f );
  TWPuts( MainWindow, my_string );
  TWSetAttribute( MainWindow, 0xf0 );
  TWPuts( MainWindow, "\nHit any key to exit this example program..." );
  TWGetkey( MainWindow );
  return( 0 );
}

#endif

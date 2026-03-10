//*
//*  EXAMP312.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1992-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example opens a window with TWDefineWindow() and writes
//*  to the window with TWPrintf(), TWPuts(), TWPokec(). TWGetkey()
//*  will get keyboard input and exit.
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
  int row;
  int col;

  GCPP_UNUSED_PARAMETER( lpszCmdLine );

  if( !TWInitialize( hInstance,
                     hPrevInstance,
                     nCmdShow,
                     "Comm++ Example 312" ) )
    return( 0 );
  MainWindow = TWDefineWindow( 0,
                               10,
                               60,
                               25,
                               1,
                               "MainWindow",
                               "Courier New",
                               10 );
  for( i = 0 ; i < 20 ; i++ )
    TWPrintf( MainWindow,
              "%02d: Smaller fonts let you put more on your screen!\n",
              i );
  TWPuts( MainWindow, "\nHit any key to exit..." );
  i = 0;

  //*
  //* Picking up the current row and column this way is sort of
  //* undocumented.  It might be a good idea to add a function
  //* to do this.
  //*
  row = MainWindow->VTWCaretRow;
  col = MainWindow->VTWCaretCol - 1;
  while( !TWKbhit( MainWindow ) )
    TWPokec( MainWindow, row, col, "-\\|/"[ i++ & 3 ] );
  TWGetkey( MainWindow );
  return( 0 );
}

#endif

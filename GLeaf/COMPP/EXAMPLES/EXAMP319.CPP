//*
//*  EXAMP319.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1992-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example opens a window with TWDefineWindow() and
//*  demonstrates the TWGets() and TWMenu() functions.
//*
//*
#include "tw.h"

#ifdef GCPP_WINDOWS

char *menu[] = {
     "Eat",
     "Drink",
     "Be Merry",
     ""
};

int PASCAL WinMain( HINSTANCE hInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpszCmdLine,
                    int nCmdShow )
{
  TW_WINDOW *MainWindow;
  char buffer[ 21 ];
  int choice;

  GCPP_UNUSED_PARAMETER( lpszCmdLine );

  if( !TWInitialize( hInstance,
                     hPrevInstance,
                     nCmdShow,
                     "Comm++ Example 319" ) )
    return( 0 );
  MainWindow = TWDefineWindow( 0,
                               0,
                               70,
                               15,
                               0,
                               "MainWindow",
                               "Courier New",
                               14 );
  MainWindow->VTWCols = 60;
  TWPuts( MainWindow, "Hit any key to bring up the menu..." );
  TWGetkey( MainWindow );
  choice = TWMenu( MainWindow, 10, 30, menu );
  TWPrintf( MainWindow, "\n\nYou selected number %d\n\n", choice );
  TWPrintf( MainWindow, "Now please enter a 20 character or shorter string: " );
  TWGets( MainWindow, buffer, 20 );
  TWPrintf( MainWindow, "\n\nYou entered: " );
  TWSetAttribute( MainWindow, 0x0f );
  TWPuts( MainWindow, buffer );
  TWSetAttribute( MainWindow, 0xf0 );
  TWPuts( MainWindow, "\nHit any key to exit this example program..." );
  TWGetkey( MainWindow );
  return( 0 );
}

#endif

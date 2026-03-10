//*
//*  EXAMP112.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1989-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example demonstrates the GFScreenVid::HideCursor(),
//*  GFScreenVid::ShowCursor(), GFScreenVid::Tab(),
//*  GFScreenVid::InitTabs(), GFScreenVid::SetTabStop(),
//*  GFScreenVid::ClearAllTabs(), and GFScreenVid::ClearTabStop()
//*  functions.
//*
#include <stdio.h>
#include "comppdsp.hpp"
#include "ibmkeys.h"

void main( void )
{
  unsigned key;

  GFScreenVid scrn;
  scrn.InitTabs();

  do {
    key = getkey();
    switch( key ) {
      case F1:
        scrn.SetTabStop();
        break;
      case F2:
        scrn.HideCursor();
        break;
      case F3:
        scrn.ShowCursor();
        break;
      case CURDN:
        scrn.CursorDown( 1 );
        break;
      case CURRT:
        scrn.CursorRight( 1 );
        break;
      case CURLF:
        scrn.CursorLeft( 1 );
        break;
      case CURUP:
        scrn.CursorUp( 1 );
        break;
      case REVTAB:
        scrn.BackTab();
        break;
      case 0x09:  //* TAB
        scrn.Tab();
        break;
      case ALTTAB:
        scrn.ClearTabStop();
        break;
      case CTRLTAB:
        scrn.ClearAllTabs();
        break;

      default:
        scrn.PutChar( (char)key );
    }
  } while( key != ESC );
}

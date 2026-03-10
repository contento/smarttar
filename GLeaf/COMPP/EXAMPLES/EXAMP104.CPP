//*
//*  EXAMP104.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1989-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example demonstrates the GFScreenVid::GFScreenVid() constructor,
//*  GFScreenVid::~GFScreenVid() destructor, GFScreenZDisp::GFScreenZDisp()
//*  constructor and GFScreenZDisp::~GFScreenZDisp() destructor,
//*  GFScreenDriver::CursorHome(), GFScreenDriver::CursorUp(),
//*  GFScreenDriver::CursorDown(), GFScreenDriver::CursorRight(),
//*  GFScreenDriver::CursorLeft(), GFScreenDriver::BackTab(), and
//*  GFScreenDriver::Home() functions.  This example allows the user to
//*  move the cursor to any position on the screen.
//*
#include "comppdsp.hpp"
#include "ibmkeys.h"

void main( void )
{
  unsigned key;

#if defined( GCPP_SYMANTEC )
  GFScreenZDisp *scrn = new GFScreenZDisp();
#else
  GFScreenVid *scrn = new GFScreenVid();
#endif

  for( ; ;  ) {
    key = getkey();
    switch( key ) {
      case CURDN:
        scrn->CursorDown( 1 );
        break;
      case HOME:
        scrn->CursorHome();
        break;
      case CURRT:
        scrn->CursorRight( 1 );
        break;
      case CURLF:
        scrn->CursorLeft( 1 );
        break;
      case CURUP:
        scrn->CursorUp( 1 );
        break;
      case REVTAB:
        scrn->BackTab();
        break;
      case ESC:
        delete scrn;
        return;
      case ALTI:
        scrn->Invert();
        return;
      default:
        break;
    }
    scrn->Flush();
  }
}

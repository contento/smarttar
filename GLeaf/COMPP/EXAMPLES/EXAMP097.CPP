//*
//*  EXAMP097.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1989-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example program demonstrates the use of the kbshift()
//*  function.
//*
//*
#include "ibmkeys.h"
#include <iostream.h>

#ifndef GCPP_WINDOWS

char *states[] = { "Right shift", " Left shift", "    Control",
                   "        Alt", "Scroll lock", "   Num lock",
                   "  Caps lock", "Insert mode" };

void main( void )
{
  int shift_status;
  int i;

  shift_status = kbshift();
  for( i = 0 ; i < 8 ; i++ ) {
    cout << states[ i ] << (shift_status & 1 ? " ON" : " OFF") << "\n";
    shift_status >>= 1;
  }
}
#endif

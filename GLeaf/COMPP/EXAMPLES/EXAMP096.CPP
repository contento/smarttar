//*
//*  EXAMP096.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1989-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example program demonstrates the use of the kbfetch()
//*  function.
//*
//*
#include "ibmkeys.h"
#include <iostream.h>

#ifndef GCPP_WINDOWS

void main( void )
{
  int return_code;
  int ascii_value;
  int scan_code;

  return_code = kbfetch();
  ascii_value = return_code & 0xff;
  scan_code = return_code >> 8;
  if( ascii_value != 0 )
    if( ascii_value >= 0x20 )
      cout << "Ascii character = " << ascii_value << "\n";
    else
      cout << "Ascii character = ^" << (ascii_value + 64) << "\n";
  cout << "Scan code = " << scan_code << "\n";
}
#endif

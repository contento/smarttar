//*
//*  EXAMP090.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1989-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example demonstrates the GFChecksum::GFChecksum() constructor,
//*  destructor and the GFChecksum::CalculateBlock() functions.  We will
//*  calculate the 1 byte checksum for this source file.
//*
#include <stdio.h>
#include "compplib.hpp"
#include <iostream.h>

void main( void )
{
  unsigned long checksum;
  unsigned int fsize;
  FILE *infile;
  GCPP_BYTE buffer[2000];
  GFChecksum *check;

  if( (infile = fopen( "..\\examples\\EXAMP090.CPP", "rb" )) == NULL )
    return;
  fsize = fread(  buffer, sizeof(char), 2000, infile );
  fclose( infile );
  check = new GFChecksum();
  checksum = check->CalculateBlock( buffer, fsize );
  delete check;
  cout << "The checksum value for this file is " << checksum;
}

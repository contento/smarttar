//*
//*  EXAMP093.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1989-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example demonstrates the GFCrc32::GFCrc32() constructor,
//*  destructor, GFCrc32::CalculateBlock() and GFCrc32::CalculateCharacter()
//*  functions.  We will calculate the 2 byte checksum for every character
//*  and for the entire source file.
//*
#include <stdio.h>
#include "compplib.hpp"
#include <iostream.h>

void main( void )
{
  unsigned long crc;
  unsigned int fsize;
  FILE *infile;
  GCPP_BYTE buffer[2000];
  GFCrc32 *check;
  unsigned int i;

  if( (infile = fopen( "..\\examples\\EXAMP093.CPP", "rb" )) == NULL )
    return;
  fsize = fread(  buffer, sizeof(char), 2000, infile );
  fclose( infile );
  check = new GFCrc32();
  for( i = 0; i <= fsize; i++ ) {
    crc = check->CalculateCharacter( buffer[i], 0 );
    cout << "The CRC32 value for character "<< i << " is " << crc << "\n";
  }
  crc = check->CalculateBlock( buffer, fsize, 0 );
  cout << "The CRC32 value for this file is " << crc;
  delete check;
}

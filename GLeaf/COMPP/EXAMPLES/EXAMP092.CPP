//*
//*  EXAMP092.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1989-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example demonstrates the GFCrc16::GFCrc16() constructor,
//*  destructor, GFCrc16::CalculateBlock(), GFCrc16::CalculateCharacter(),
//*  and GFCrc16::CalculateCharacterModified() functions.  We will
//*  calculate the 2 byte checksum for every character and for the entire
//*  source file.
//*
#include <stdio.h>
#include "compplib.hpp"
#include <iostream.h>

void main( void )
{
  unsigned long crc1;
  unsigned long crc2;
  unsigned int fsize;
  FILE *infile;
  GCPP_BYTE buffer[2000];
  GFCrc16 *check;
  unsigned int i;

  if( (infile = fopen( "..\\examples\\EXAMP092.CPP", "rb" )) == NULL )
    return;
  fsize = fread(  buffer, sizeof(char), 2000, infile );
  fclose( infile );
  check = new GFCrc16();
  for( i = 0; i <= fsize; i++ ) {
    crc1 = check->CalculateCharacter( buffer[i], 0 );
    crc2 = check->CalculateCharacterModified( buffer[i], 0 );
    cout << "The CRC16 values for character "<< i << " are " << crc1 <<
        " and " << crc2 << "\n";
  }
  crc1 = check->CalculateBlock( buffer, fsize, 0 );
  cout << "The CRC16 value for this file is " << crc1;
  delete check;
}

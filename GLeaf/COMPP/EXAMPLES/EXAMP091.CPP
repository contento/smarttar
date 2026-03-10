//*
//*  EXAMP091.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1989-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example demonstrates the GFCheck2Byte::GFCheck2Byte()
//*  constructor and the GFCheck2Byte::CalculateBlock() function.  We
//*  will calculate the 2 byte checksum for this source file.
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
  GFCheck2Byte check;

  if( (infile = fopen( "..\\examples\\EXAMP091.CPP", "rb" )) == NULL )
    return;
  fsize = fread(  buffer, sizeof(char), 2000, infile );
  fclose( infile );
  checksum = check.CalculateBlock( buffer, fsize );
  cout << "The checksum value for this file is " << checksum;
}

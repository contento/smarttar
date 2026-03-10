//*
//*  EXAMP076.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1989-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example demonstrates GFTransfer::ReplaceFileEngine() by
//*  deriving a new file engine from GFFileIO and setting up the
//*  functions to use a memory buffer for the file transfer.  The
//*  example will send a file when the user hits the F1 key.
//*
#include "compplib.hpp"
#include "filexfer.hpp"
#include "ibmkeys.h"
#include "_cppmem.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <iostream.h>

//*
//* MyFileIO is my file engine class that is derived from
//* the base GFFileIO class.
//*
class MyFileIO : public GFFileIO
{
  public:
    MyFileIO( void );
    ~MyFileIO( void )
      {}
    int Initialize( char *input_name = NULL, int recurse_flag = 0 );
    int Open( char *filename, char *mode );
    int Close( void );
    int SetCurrentPosition( long pos, int origin );
    long GetCurrentPosition( void );
    int SetTimeDate( void )
      { return( GCPP_OK ); }
    int GetTimeDate( void );
    long GetLength( void );
    int GetNextName( void );
    int PutBuffer( unsigned char *buf, int buf_length );
    int GetBuffer( unsigned char *buf, int buf_length );
    int PutChar( char c );
    int GetChar( char *c );
    int Flush( void )
      { return( GCPP_OK ); }
  protected:
    long buffer_size;
    char filename[ 256 ];
    char *file_ptr;
    char *file_buffer;
};

//*
//* The constructor for MyFileIO initializes the data members
//* of the class.
//*
MyFileIO::MyFileIO( void )
{
  strcpy( filename, '\0' );
  file_ptr = 0;
  file_buffer = 0;
}

//*
//* Initialize() sets some initial values that need to
//* be set up each time a new file is transferred.
//*
int MyFileIO::Initialize( char *input_name, int recurse_flag )
{
  file_ptr = 0;
  recursive_find = recurse_flag;
  length = -1;
  if ( input_name )
    input_name_list = input_name;
  return( GCPP_OK );
}

//*
//* This version of Open() allocates a memory buffer, opens
//* the requested file, and reads that file into the memory
//* buffer.  This function assumes that the entire file can
//* be read with one call to fread().
//*
int MyFileIO::Open( char *filename, char *mode )
{
  FILE *fp;
  int status;

  fp = fopen( filename, mode );
  if ( !fp )
    return( GCPP_ERR_FE_OPEN );
  else {
    //*
    //* Get the length of the file so I know the size
    //* of the memory buffer I need to allocate.
    //*
    fseek( fp, 0, SEEK_END );
    buffer_size = ftell( fp );
    //*
    //* For a ZMODEM transfer, I need to know the length
    //* of the file, so I go ahead and set the length
    //* data member here.
    //*
    length = buffer_size;
    //*
    //* Allocate the memory buffer.
    //*
    file_buffer = ( char * )GFMem::AllocateMemory(
                                (int)(buffer_size & 0xffff ) );
    if ( !file_buffer ) {
      fclose( fp );
      return( GCPP_ERR_FE_MEM );
    }
    //*
    //* Reset to the beginning of the file and read the contents
    //* into the memory buffer.
    //*
    fseek( fp, 0, SEEK_SET );
    status = fread( file_buffer, 1, (int)( buffer_size & 0xffff ), fp );
    if ( status < buffer_size ) {
      fclose( fp );
      return( GCPP_ERR_FE_OPEN );
    }
    file_ptr = file_buffer;
  }
  //*
  //* Close the file and return successfully.
  //*
  fclose( fp );
  return( GCPP_OK );
}

//*
//* Close() is called after a file has been completely
//* transferred, so my memory buffer is not needed
//* any longer.  If I wanted to use the same memory
//* buffer for batch transfers, I wouldn't free the
//* memory until the destructor is called.
//*
int MyFileIO::Close( void )
{
  if ( file_buffer )
    GFMem::DeallocateMemory( file_buffer );
  file_ptr = 0;
  file_buffer = 0;
  return( GCPP_OK );
}

//*
//* SetCurrentPosition() is called when the file transfer
//* routines need to set the file pointer position to
//* a specific location in the file.  The file transfer
//* routines use the SEEK_CUR, SEEK_END, and SEEK_SET
//* for the origin.  This function would be much more
//* complicated if I did not have the entire file loaded
//* into the memory buffer.
//*
int MyFileIO::SetCurrentPosition( long pos, int origin )
{
  if ( origin == SEEK_CUR )
    file_ptr += pos;
  else if ( origin == SEEK_END )
    file_ptr = file_buffer + buffer_size + pos;
  else if ( origin == SEEK_SET )
    file_ptr = file_buffer + pos;
  return( GCPP_OK );
}

//*
//* GetCurrentPosition() is called when the file
//* transfer routines need to know the current
//* file pointer offset within the file.
//* The current file position is indicated by
//* file_ptr and file_buffer is the beginning
//* of the buffer.  Subtracting file_buffer from
//* file_ptr gives me the current offset.
//*
long MyFileIO::GetCurrentPosition( void )
{
  return( file_ptr - file_buffer );
}

//*
//* GetTimeDate() is called by the file transfer
//* functions that transfer the time and date
//* with the file.  This function gets the current
//* time and date and sets the data members with
//* the correct values.
//*
int MyFileIO::GetTimeDate( void )
{
  time_t ltime;
  struct tm *tblock;

  time( &ltime );
  tblock = localtime( &ltime );
  if ( tblock ) {
    year = tblock->tm_year + 1900;
    month = tblock->tm_mon + 1;
    day = tblock->tm_mday;
    hours = tblock->tm_hour;
    minutes = tblock->tm_min;
    seconds = tblock->tm_sec;
  } else {
    year = 1980;
    month = 1;
    day = 1;
    hours = 0;
    minutes = 0;
    seconds = 0;
  }
  return( GCPP_OK );
}

//*
//* GetLength() is called by the file transfer
//* routines that transfer the file length with
//* the file.
//*
long MyFileIO::GetLength( void )
{
  if ( length == -1 )
    length = buffer_size;
  return( length );
}

//*
//* GetNextName() is called when the next file name
//* needs to be known by the file transfers.  If
//* a batch send was taking place, input_name_list
//* would contain the list of filenames that was
//* passed to SendFile().  This function scans the
//* list of names for the next available name and
//* places it in input_name.  If a filename is found,
//* this function returns 1, otherwise it returns 0.
//*
int MyFileIO::GetNextName( void )
{
  int c;
  char *output = input_name;

  input_name[ 0 ] = '\0';
  for ( ; ; ) {
    c = *input_name_list;
    if( !isspace( c ) && c != ',' && c != ';' )
      break;
    else
      input_name_list++;
  }
  while( !isspace( c ) && c != ',' && c != ';' && c != '\0' ) {
    *output++ = (char) c;
    input_name_list++;
    c = *input_name_list;
  }
  *output = '\0';
  if ( input_name[ 0 ] != '\0' )
    return( 1 );
  else
    return( 0 );
}

//*
//* PutBuffer() is called when a block of data
//* needs to be written to the file.
//*
int MyFileIO::PutBuffer( unsigned char *buf, int buf_length )
{
  memcpy( file_ptr, buf, buf_length );
  return( GCPP_OK );
}

//*
//* GetBuffer() is called when a block of data
//* needs to be read from the file.
//*
int MyFileIO::GetBuffer( unsigned char *buf, int buf_length )
{
  int buf_size;
  if ( ( file_buffer + buffer_size ) >= ( file_ptr + buf_length ) )
    buf_size = buf_length;
  else
    buf_size = file_buffer + buffer_size - file_ptr;
  memcpy( buf, file_ptr, buf_size );
  return( buf_size );
}

//*
//* PutChar() is called when a single byte
//* needs to be written to the file.
//*
int MyFileIO::PutChar( char c )
{
  return( PutBuffer( (unsigned char *)&c, 1 ) );
}

//*
//* GetChar() is called when a single byte
//* needs to be read from the file.
//*
int MyFileIO::GetChar( char *c )
{
  return( GetBuffer( (unsigned char *)c, 1 ) );
}

void main( void )
{
  int c;
  int status;

  GFI8250 sio( COM1 );
  GFSerial cp( &sio );
  GFZmodem txfr( &cp );
  MyFileIO *Xmem = new MyFileIO();

  //*
  //* Call the function to replace the default
  //* file engine with the new derived engine
  //* that uses a memory buffer.
  //*
  txfr.ReplaceFileEngine( Xmem );

  GFDataFormat *df = cp.GetDataFormat();
  df->BaudRate( 19200 );

  for( ; ; ) {
    if( gfkbhit() ) {
      c = getkey();
      if( c == ESC )
        return;
      else if( c == F1 ) {
        status = txfr.SendFile( "..\\examples\\examp076.cpp" );
        cout << "\nSend file status = " << status << "\n";
        cout.flush();
      }
      else
        cp << c;
    }
    cp >> c;
    if( c != GF_TIMEOUT ) {
      cout << (char)c;
      cout.flush();
    }
  }
} 

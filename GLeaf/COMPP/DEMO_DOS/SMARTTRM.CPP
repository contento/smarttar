//*
//* SMARTTRM.CPP contains
//*
//*         MiniTerm::MiniTerm()
//*         MiniTerm::ReadAndWriteChar()
//*         CloseUpAndExit()
//*         Initialize()
//*         TileWindows()
//*         DebugStatusPuts()
//*         Usage()
//*         LoadParams()
//*         main()
//*
//*
//* The Greenleaf Comm++ Library
//*
//* Copyright (C) 1989-95 Greenleaf Software Inc.  All Rights Reserved.
//*
//* MODIFICATIONS
//*   June 14, 1993 - Incorrectly declared the MiniTerm destructor.
//*

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include "compplib.hpp"
#include "ibmkeys.h"
#include "video.h"

void DebugStatusPuts( char *buf );

//****************************************************************************
//* Class definitions
//****************************************************************************

class MiniTerm
{
  public:
    MiniTerm( int com, int row, int col );
    ~MiniTerm( void )
      { delete cp;  delete sio; }

    void TerminalGoto( void )
      { VidGoto( window, window->row, window->col ); }
    void PutChar( char c )
      { cp->PutChar( c ); }
    int ReadAndWriteChar( void );
    void DumpStatus( void )
      { cp->ViewPortStatus( DebugStatusPuts ); }

    GFSmartDigiBoard *sio;
    GFSerial *cp;
    VID_WINDOW *window;
};

struct Settings {
  int row;
  int col;
} settings[16];


//****************************************************************************
//* Global Data
//****************************************************************************

int active_window;
unsigned window_width;
unsigned window_height;
int split_screen;
VID_WINDOW *DebugWindow;
MiniTerm *TermList[16];


//****************************************************************************
//* Interface member functions for library systems.
//****************************************************************************

//*
//* MiniTerm::MiniTerm( int com, int row, int column )
//*
//* ARGUMENTS
//*   int com - The comm port to open.
//*   int row - The upper row of the window.
//*   int col - The leftmost column of the window.
//*
//* DESCRIPTION
//*   This constructor will open the specified port using the
//*   GFSmartDigiBoard class and and create a window for that
//*   port.
//*
//* SIDE EFFECTS
//*   Creates a MiniTerm object.
//*
//* RETURNS
//*   NONE
//*
//* AUTHOR
//*   Patrick Little
//*
//* MODIFICATIONS
//*
//*
MiniTerm::MiniTerm( int com, int row, int column )
{
  //*
  //* Open the port.
  //*
  sio = new GFSmartDigiBoard( com );
  cp = new GFSerial( sio );

  //*
  //* Create a window for output.
  //*
  window = VidDefineWindow( (unsigned char)row,
                            (unsigned char)column,
                            (unsigned char)(row + window_height - 1),
                            (unsigned char)(column + window_width - 1), 1 );
  VidDrawBorder( window );
  VidPokef( window, -1, 1, "[ COM%d ]", com+1 );
  VidClearWindow( window );
}


//*
//* int MiniTerm::ReadAndWriteChar( void )
//*
//* ARGUMENTS
//*   NONE
//*
//* DESCRIPTION
//*   This function just simplifes and clarifies the code some.  It
//*   will read from the buffer and write directly to the screen
//*   if it receives a valid character.
//*
//* SIDE EFFECTS
//*
//* RETURNS
//*   The character or code read.
//*
//* AUTHOR
//*   Patrick Little
//*
//* MODIFICATIONS
//*
//*
int MiniTerm::ReadAndWriteChar( void )
{
  int c = cp->GetChar();
  if( c >= GCPP_OK )
    VidPutc( window, c );
  return( c );
}


//*
//* void CloseUpAndExit( char *fmt, ... )
//*
//* ARGUMENTS
//*   char *fmt, ... - A format plus a list of parameters to be sent
//*                    to vsprintf() for output.
//*
//* DESCRIPTION
//*   This is a fatal error handler, used to shut things down and print
//*   any error message that is needed.
//*
//* SIDE EFFECTS
//*
//* RETURNS
//*   NONE
//*
//* AUTHOR
//*   Patrick Little
//*
//* MODIFICATIONS
//*
//*
void CloseUpAndExit( char *fmt, ... )
{
  va_list argptr;

  VidSet80x25();
  VidTerminate( TRUE );
  if( strlen( fmt ) != 0 ) {
    printf( "\nFatal error: " );
    va_start(argptr, fmt);
    vprintf(fmt, argptr);
    va_end(argptr);
    printf( "\n" );
    for( int i = 0; TermList[ i ] != NULL; i++ )
      delete TermList[ i ];
    exit( 1 );
  }
  for( int i = 0; TermList[ i ] != NULL; i++ )
    delete TermList[ i ];
  exit( 0 );
}


//*
//* void Initialize( int number, int base_port )
//*
//* ARGUMENTS
//*   int number - The number of ports in the system.
//*   int base_port - The begging port of the system.
//*
//* DESCRIPTION
//*   This routine initializes all the windows and ports.  This means the
//*   windows are defined and tossed up on the screen, and each of the
//*   ports are opened.  Note that the parameters for the ports and windows
//*   are assumed to have been set up elsewhere.
//*
//* SIDE EFFECTS
//*
//* RETURNS
//*   NONE
//*
//* AUTHOR
//*   Patrick Little
//*
//* MODIFICATIONS
//*
//*
void Initialize( int number, int base_port )
{
  unsigned short error;
  int result;

  //*
  //* Initialize the screen and open a debug window if specified.
  //*
  VidInitialize( VID_NORMAL, TRUE );
  DebugWindow = 0;
  if( split_screen ) {
    split_screen = VidSet80x43();
    if( split_screen ) {
      DebugWindow = VidDefineWindow( 25, 0, 42, 79, 0 );
      VidClearWindow( DebugWindow );
    }
  }

  //*
  //* Open the ports and windows.  We also check for errors.
  //*
  for( int i = 0; i < number; i++ ) {
    TermList[ i ] = new MiniTerm( base_port + i,
                                  settings[ i ].row,
                                  settings[ i ].col );
    if( (result = TermList[ i ]->cp->GetCommError( error )) < GCPP_OK )
      CloseUpAndExit( "Error opening %d when opening COM%d\n",
                      base_port + i, result );
  }
}


//*
//* void TileWindows( int number )
//*
//* ARGUMENTS
//*   int number - The number of ports to open.
//*
//* DESCRIPTION
//*   This routine tries to set up the windows so that they are as big as
//*   possible, and look nice on the screen.
//*
//* SIDE EFFECTS
//*
//* RETURNS
//*   NONE
//*
//* AUTHOR
//*   Patrick Little
//*
//* MODIFICATIONS
//*
//*
void TileWindows( int number )
{
  int i;
  int number_of_windows_across;

  if( number <= 1 ) {
    number_of_windows_across = 1;
    window_width = 78;
    window_height = 23;
  } else if ( number <= 2 ) {
    number_of_windows_across = 2;
    window_width = 38;
    window_height = 23;
  } else if ( number <= 4 ) {
    number_of_windows_across = 2;
    window_width = 38;
    window_height = 10;
  } else if ( number <= 8 ) {
    number_of_windows_across = 4;
    window_width = 18;
    window_height = 10;
  } else if ( number <= 16 ) {
    number_of_windows_across = 4;
    window_width = 18;
    window_height = 4;
  } else
     CloseUpAndExit( "Can't handle %d windows!\n", number );
  for( i = 0 ; i < number ; i++ ) {
    settings[ i ].row = 1;
    settings[ i ].row += ( window_height + 2 ) * ( i / number_of_windows_across);
    settings[ i ].col = 1;
    settings[ i ].col += ( window_width  + 2 ) * ( i % number_of_windows_across);
  }
}


//*
//* void DebugStatusPuts( char *buf )
//*
//* ARGUMENTS
//*   char *buf - The debug strings to print to the screen.
//*
//* DESCRIPTION
//*   This is the callback routine used to print the port status out to the
//*   dump window.  This routine is passed as an argument to the
//*   ViewPortStatus call, which then calls it repeatedly with lines of
//*   text to be dumped to the screen.
//*
//* SIDE EFFECTS
//*
//* RETURNS
//*   NONE
//*
//* AUTHOR
//*   Patrick Little
//*
//* MODIFICATIONS
//*
//*
void DebugStatusPuts( char *buf )
{
  static row = 0;

  if( !DebugWindow )
    return;
  if( *buf == '\0' )
    row = 0;
  else
    VidPokes( DebugWindow, row++, 0, buf );
}


//*
//* void Usage( void )
//*
//* ARGUMENTS
//*   NONE
//*
//* DESCRIPTION
//*   This routine prints out a Usage line for the user.
//*
//* SIDE EFFECTS
//*
//* RETURNS
//*   NONE
//*
//* AUTHOR
//*   Patrick Little
//*
//* MODIFICATIONS
//*
//*
void Usage( void )
{
  puts( "" );
  puts( "Usage:   SMARTTRM number-of-ports  first-port [optional-split]" );
  puts( "" );
  puts( "Examples:   SMARTTRM 5 COM5" );
  puts( "            SMARTTRM 16 COM3 SPLIT" );
  puts( "" );
  puts( "The second examples puts a port status dump on the screen" );
  puts( "using 43 line mode on an EGA or VGA display." );
  puts( " " );
  exit( 1 );
}


//*
//* void LoadParams( int argc, char *argv[], int *terms, int *base_port )
//*
//* ARGUMENTS
//*   int argc - The number of arguments on the command line, passed
//*              straight through from main().
//*   char *argv[] - The list of command line arguments, passed straight
//*                  through from main().
//*   int *terms - The number of terminal sessions is sent back to
//*                the calling routine through this variable.
//*   int *base_port - The number of the first port to be opened is passed
//*                    back to the calling routine through this variable.
//*
//* DESCRIPTION
//*   The user is supposed to call this routine with two parameters on the
//*   command line.  The first parameter is the number of ports to open.
//*   The second parameter indicates what the port number for the first
//*   port is.  A typical command line might look like this:
//*
//*      SMARTTRM  8 COM5
//*
//*   This tells me to open eight ports on my digiboard starting at COM5.
//*   This routine parses those two arguments and stuffs the appropriate
//*   results into the two argument pointers passed here.  Any errors on
//*   the command line result in a fatal error exit.
//*
//*   An optional third parameter has been added which allows users to
//*   specify that a split screen should be used, with the Port Status
//*   dump to show up on the bottome half of the screen.  This optional
//*   parameter is the word "SPLIT", which can just be appended to the
//*   existing command line:
//*
//*     SMARTTRM 8 COM5 SPLIT
//*
//* SIDE EFFECTS
//*
//* RETURNS
//*   NONE
//*
//* AUTHOR
//*   Patrick Little
//*
//* MODIFICATIONS
//*
//*
void LoadParams( int argc, char *argv[], int *terms, int *base_port )
{
  int i;

  split_screen = 0;
  if( argc < 3 )
    Usage();
  *terms = atoi( argv[ 1 ] );
  if( *terms == 0 )
    Usage();
  if( strnicmp( argv[ 2 ], "COM", 3 ) != 0 )
    Usage();
  if( sscanf( argv[ 2 ] + 3, "%d", base_port ) != 1 )
    Usage();
  if( argc >= 3 ) {
    for ( i = 0 ; argv[ 3 ][ i ] != '\0' ; i++ )
      argv[ 3 ][ i ] = (char) toupper( argv[ 3 ][ i ] );
    if ( strcmp( argv[ 3 ], "SPLIT" ) == 0 )
      split_screen = 1;
  }
  *base_port -= 1;
  printf( "\nTerms for %d ports, starting at COM%d:\n", *terms, *base_port + 1 );
  if( split_screen )
    printf( "\nDebug info will be displayed in the split screen\n");
  printf( "\nUse the TAB key to switch windows, ESC to exit\n" );
  printf( "\nHit any key to continue..." );
  getkey();
  puts( "" );
}


//*
//* void main( int argc, char *argv[] )
//*
//* ARGUMENTS
//*   int argc - The number of command line arguments
//*   char *argv[] - The command line arguments
//*
//* DESCRIPTION
//*   The main program for SMARTTRM has two jobs.  The first is to handle
//*   the command line arguments and initialization.  After that, the
//*   program just sits in a loop.  The loop reads in any characters from
//*   the keyboard and outputs them to the active window.  It then reads
//*   in any characters from each port and outputs the characters to that
//*   ports window.  The main loop also handles the special input character
//*   TAB, which advances the cursor to the next window.
//*
//* SIDE EFFECTS
//*
//* RETURNS
//*   NONE
//*
//* AUTHOR
//*   Patrick Little
//*
//* MODIFICATIONS
//*
//*
void main( int argc, char *argv[] )
{
  int c, i, j;
  int terms;
  int base_port;

  LoadParams( argc, argv, &terms, &base_port );
  TileWindows( terms );
  Initialize( terms, base_port );
  active_window = 0;

  for( ;; ) {
    if( gfkbhit() ) {
      c = getkey();
      switch( c ) {
        case ESC:
          CloseUpAndExit( "" );
          break;
        case '\t':
          active_window++;
          if( active_window >= terms )
            active_window = 0;
          TermList[ active_window ]->TerminalGoto();
          if( DebugWindow )
            VidClearWindow( DebugWindow );
          break;
        default:
          TermList[ active_window ]->PutChar( (char)c );
      }
    }
    for( i = 0; i < terms; i++ ) {
      for( j = 0; j < 30; j++ )
        if( TermList[ i ]->ReadAndWriteChar() < GCPP_OK )
          break;
    }
    TermList[ active_window ]->TerminalGoto();
    TermList[ active_window ]->DumpStatus();
  }
}

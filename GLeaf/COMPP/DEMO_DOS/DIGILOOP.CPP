//*
//*
//* DIGILOOP.CPP contains
//*
//*               MiniTerm::MiniTerm()
//*               MiniTerm::PortOpen()
//*               MiniTerm::PortDisplay()
//*               MiniTerm::draw_port_number()
//*               MiniTerm::draw_irq_number()
//*               MiniTerm::draw_address()
//*               MiniTerm::draw_shared_port_address()
//*               SetCoordinates()
//*               initialize_display()
//*               draw_box()
//*               draw_punctuated_line(int row,
//*               print_port_info()
//*               print_port_receive()
//*               print_port_transmit()
//*               print_help_and_exit()
//*               open_debug_window()
//*               random_key()
//*               initialize_options()
//*               CleanUpAndExit()
//*               SetUpRegularPort()
//*               SetUpDigiboard()
//*               display_and_qualify_options()
//*               churn_key()
//*               handle_command()
//*               DebugStatusPuts()
//*               send_auto_data()
//*               perform_loopback()
//*               main()
//*
//*
//* The Greenleaf Comm++ Library
//*
//* Copyright (C) 1989-95 Greenleaf Software Inc.  All Rights Reserved.
//*
//* MODIFICATIONS
//*   June 15, 1993 - Incorrectly declared the MiniTerm destructor.
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

class MiniTerm : public GFI8250
{
  public:
    MiniTerm( int name ) : GFI8250( name )
    {}
    MiniTerm( int name, int irq,
              unsigned short base,
              unsigned short statReg );
    ~MiniTerm( void )
      { if ( cp ) delete cp;  if( sioDigi ) delete sioDigi; }

    int PortOpen( void );
    int getIRQ( void )
      { return irqno; }
    unsigned short getBaseAddress( void )
      { return( (unsigned short)base ); }
    void draw_port_number( void );
    void draw_irq_number( void );
    void draw_address( void);
    void draw_shared_port_address( void );
    void PortDisplay( int mode );
    void DumpStatus( void )
      { cp->ViewPortStatus( DebugStatusPuts ); }
    void TerminalGoto( void )
      { VidGoto( window, window->row, window->col ); }

    int PortName;
    unsigned short StatusRegisterAddress;
    unsigned short BaseAddress;
    int IRQNumber;
    int IsActive;   //* Added so I'll know if the port is active or not.
                    //* Reason: Cannot close the port if it fails.
    int row;
    int column;
    VID_WINDOW *window;
    
    GFDigiPC *sioDigi;
    GFSerial *cp;
};

//****************************************************************************
//* Global Data
//****************************************************************************
#define DEBUG
#define MAX_PORT 35
#define BUF_LEN         5      //* The buffer size is real small in this
                               //* program.  This is so that if your TX
                               //* interrupts aren't working, you will get a
                               //* full buffer and return error codes
                               //* quickly.
int ActivePort;                //* When DisplayMode == 1 and not in
                               //* AutoMode, the windows will become little
                               //* TerminalEmulators ( and they will not
                               //* send random data ). This variable tells
                               //* me which of the ports is active
int NextFreePort;
int ActiveChannels;
int PossibleChannels;
int DebugPort = -1;
int AutoMode;
int DisplayMode;
unsigned window_width;
unsigned window_height;
int irq;
unsigned short base;
unsigned short statReg;
int board;
MiniTerm *PortList[MAX_PORT];
VID_WINDOW *MainWindow;
VID_WINDOW *DebugWindow = 0;
int SplitScreen = 0;
int PagedScreen = 0;


//***************************************************************************
//* Function prototypes
//***************************************************************************
int main( int argc, char *argv[] );
void initialize_options( int argc, char *argv[] );
void print_help_and_exit( int pause );
void CleanUpAndExit( int pause );
void perform_loopback( void );
int handle_command( int menu_key );
void SetUpRegularPort( int port_number );
void SetUpDigiboard( int channel_count );
void print_port_info( int mode );
void draw_box( int start_column );
void draw_punctuated_line( int row, int col, int start_char,
                           int middle_char, int tee_char, int end_char );
void print_port_transmit(int port,int key);
void print_port_receive(int port,int key);
void display_and_qualify_options( void );
void send_auto_data( void );
int open_debug_window( char *command, char *option );
int random_key( unsigned int guy );

//****************************************************************************
//* Interface member functions for library systems.
//****************************************************************************

//*
//* MiniTerm::MiniTerm( int name, int irq,
//*                     unsigned short base,
//*                     unsigned short statReg )
//*
//* DESCRIPTION
//*   This constructor will set all parameters needed for the
//*   Digiboard ports. The regular ports ( COM1-COM4 ) will use the
//*   GFI8250 inherited constructor. I inherited the class, because
//*   that was the only way I could have access to the protected data
//*   members of GFI8250 ( irqno and base ).
//*
//* RETURNS
//*   NONE
//*
//* AUTHOR
//*   Erika Parker
//*
//* MODIFICATIONS
//*
//*
MiniTerm::MiniTerm( int name, int irq,
                    unsigned short base,
                    unsigned short statReg )
{
  PortName = name;
  IRQNumber = irq;
  BaseAddress = base;
  StatusRegisterAddress = statReg;
  IsActive = 0;
}


//*
//* MiniTerm::PortOpen( void )
//*
//* DESCRIPTION
//*   This member function will open the specified port using the
//*   GFI8250 or GFDigiPC class depending on the type of port.
//*
//* RETURNS
//*   NONE
//*
//* AUTHOR
//*   Erika Parker
//*
//* MODIFICATIONS
//*
//*
MiniTerm::PortOpen( void )
{
  unsigned short error;

  if( PortName >= 0 && PortName < 2 && ( StatusRegisterAddress == 0 ) ) {
    sioDigi = NULL;
    cp = new GFSerial( this );
  }
  else{
    sioDigi = new GFDigiPC( PortName, BaseAddress, IRQNumber, BUF_LEN, BUF_LEN );
    cp = new GFSerial( sioDigi );
#ifdef DEBUG
    VidPrintf( MainWindow, "sioDigi = new GFDigiPC( %d,%5x,%d,%d,%d )\n",PortName,
             BaseAddress,IRQNumber,BUF_LEN,BUF_LEN );
    VidPuts( MainWindow, " cp = new GFSerial( sioDigi );\n" );
#endif
  }
  return( cp->GetCommError( error ) );
}


//*
//* MiniTerm::PortDisplay( int mode )
//*
//* ARGUMENTS
//*
//* int mode : Display mode to use
//*            0 means regular display ( columns )
//*            1 means new display ( using windows )
//*
//* DESCRIPTION
//* Calls several member functions to display data on the screen.
//*
//* RETURNS
//*   NONE
//*
//* AUTHOR
//*   Erika Parker
//*
//* MODIFICATIONS
//*
//*
void MiniTerm::PortDisplay( int mode )
{
  if( !mode ) {
    draw_port_number();
    draw_irq_number();
    draw_address();
    draw_shared_port_address();
  }
  else {
    window = VidDefineWindow( (unsigned char)row,
                              (unsigned char)column,
                              (unsigned char)(row + window_height - 1),
                              (unsigned char)(column + window_width - 1), 1 );
    VidDrawBorder( window );
    VidPokef( window, -1, 1, " COM%d ", PortName+1 );
    VidClearWindow( window );
  }
}


//*
//* void MiniTerm::draw_port_number( void )
//*
//* ARGUMENTS
//*
//* NONE
//*
//* DESCRIPTION
//*   This routine is called to print the port number for COMX.  It is only
//*   called once, when the screen initialization takes place.
//*
//* SIDE EFFECTS
//*   None.
//*
//* RETURNS
//*   Nothing.
//*
//* AUTHOR
//*   SM          December 12, 1992
//*
//* MODIFICATIONS
//*
//*
void MiniTerm::draw_port_number( void )
{
  VidGoto( MainWindow, ( PortName % 18 ) + 6, 1 + ( ( PortName / 18 ) * 40 ) );
  VidPrintf( MainWindow, "COM%d:", PortName + 1 );
}


//*
//* void MiniTerm::draw_irq_number( void )
//*
//* ARGUMENTS
//*   NONE
//*
//* DESCRIPTION
//*   This routine is called to print the IRQ number for a given port.  It is
//*   only called once, when the screen initialization takes place.
//*
//* SIDE EFFECTS
//*   None.
//*
//* RETURNS
//*   Nothing.
//*
//* AUTHOR
//*   SM          December 12, 1992
//*
//* MODIFICATIONS
//*
void MiniTerm::draw_irq_number( void )
{
  VidGoto( MainWindow, ( PortName % 18 ) + 6, 8 + ( ( PortName / 18 ) * 40 ) );
  if( IRQNumber != 0 )
    VidPrintf( MainWindow, "%3d", IRQNumber );
  else
    VidPrintf( MainWindow, "N/A" );
}


//*
//* void MiniTerm::draw_address( void )
//*
//* ARGUMENTS
//*   NONE
//*
//* DESCRIPTION
//*   This routine is called to print the address for the UART for a given
//*   port.   This information is only printed out when the screen
//*   initialization takes place.
//*
//* SIDE EFFECTS
//*   None.
//*
//* RETURNS
//*   Nothing.
//*
//* AUTHOR
//*   SM          December 12, 1992
//*
//*  MODIFICATIONS
//*
void MiniTerm::draw_address( void )
{
  VidGoto( MainWindow, ( PortName % 18 ) + 6, 15 + ( ( PortName / 18 ) * 40 ) );
  if( BaseAddress != 0 )
    VidPrintf( MainWindow, "%3x", BaseAddress );
  else
    VidPrintf( MainWindow, "N/A" );
}


//*
//* void MiniTerm::draw_shared_port_address( void )
//*
//* ARGUMENTS
//*   None.
//*
//* DESCRIPTION
//*   This routine is called to print the address of the shared interrupt
//*   status register for a given port.  This information is only printed out
//*   when the screen initialization takes place. Note that all the UARTS on a
//*   given shared card usually have the same shared port address.  With the
//*   Digiboards, ports on several cards will all have the same address.
//*
//* SIDE EFFECTS
//*   None.
//*
//* RETURNS
//*   Nothing.
//*
//* AUTHOR
//*   SM          December 12, 1992
//*
//* MODIFICATIONS
//*
void MiniTerm::draw_shared_port_address( void )
{
  VidGoto( MainWindow, ( PortName % 18 ) + 6, 23 + ( ( PortName / 18 ) * 40 ));
  if( StatusRegisterAddress != 0 )
    VidPrintf( MainWindow, "%03x", StatusRegisterAddress );
  else
    VidPrintf( MainWindow, "N/A" );
}


//**************************************************************************
//* Other functions
//**************************************************************************
void SetCoordinates( int number )
{
  int i;
  int number_of_windows_across;

  if( number <= 1 ) {
    number_of_windows_across = 1;
    window_width = 78;
    window_height = 23;
  }
  else if ( number <= 2 ) {
    number_of_windows_across = 2;
    window_width = 38;
    window_height = 23;
  }
  else if ( number <= 4 ) {
    number_of_windows_across = 2;
    window_width = 38;
    window_height = 10;
  }
  else if ( number <= 8 ) {
    number_of_windows_across = 4;
    window_width = 18;
    window_height = 10;
  }
  else if ( number <= 16 ) {
    number_of_windows_across = 4;
    window_width = 18;
    window_height = 4;
  }
  else if ( number <= 32 ) {
    number_of_windows_across = 8;
    window_width = 8;
    window_height = 4;
  }
  else
     VidPrintf( MainWindow, "Can't handle %d windows!\n", number );
  for( i = 0 ; i < MAX_PORT ; i++ )
    if( PortList[ i ] ) {
      PortList[ i ]->row = 1;
      PortList[ i ]->row += ( window_height + 2 ) * ( i / number_of_windows_across);
      PortList[ i ]->column = 1;
      PortList[ i ]->column += ( window_width  + 2 ) * ( i % number_of_windows_across);
    }
}


//*
//* void initialize_display( void )
//*
//* ARGUMENTS
//*   None.
//*
//* DESCRIPTION
//*   This routine initializes the display, then prints out a title box on the
//*   top three lines of the display.  If the user hits an error of some sort,
//*   the two big boxes that display the debug data won't make it up on the
//*   screen, but the title box will.
//*
//* SIDE EFFECTS
//*   None.
//*
//* RETURNS
//*   Nothing.
//*
//* AUTHOR
//*   SM          December 12, 1992
//*
//*  MODIFICATIONS
//*
void initialize_display( void )
{
    int i;
    char title[ 80 ];

    VidClearWindow( MainWindow );
    VidPokec( MainWindow, 0, 0, UL_CORNER );
    for( i = 0 ; i < 77 ; i++ )
      VidPokec( MainWindow ,0, i + 1, HORIZONTAL_LINE );
    VidPokec( MainWindow, 0, 78, UR_CORNER );
    VidPokec( MainWindow, 1, 0, VERTICAL_LINE );
    VidPokec( MainWindow, 1, 78, VERTICAL_LINE );
    VidPokec( MainWindow, 2, 0, LL_CORNER );
    for( i = 0 ; i < 77 ; i++ )
      VidPokec( MainWindow, 2, i + 1, HORIZONTAL_LINE );
    VidPokec( MainWindow, 2, 78, LR_CORNER );
    sprintf( title, "DIGILOOP - Greenleaf Comm++ v3.0 - ALT_M for Menu" );
    VidGoto( MainWindow, 1, 40 - ( strlen( title ) / 2 ) );
    VidPuts( MainWindow, title );
    VidGoto( MainWindow, 5, 0 );
}


//*
//* void draw_box( int start_column )
//*
//* ARGUMENTS
//*   start_column : The starting column to draw the box in.
//*
//* DESCRIPTION
//*   This routine is called to draw one of the boxes that displays all of
//*   the port info.  The same routine is used to draw two different port
//*   boxes at different starting columns.  Most of their work is done
//*   by the routine that draws a puncuated line.  You just call it with
//*   different parameters depending on which line it is.
//*
//* SIDE EFFECTS
//*   None.
//*
//* RETURNS
//*   Nothing.
//*
//* AUTHOR
//*   SM          December 12, 1992
//*
//*  MODIFICATIONS
//*
void draw_box( int start_column )
{
  int i;

  //* The first thing I do is print the column headers.  They have some
  //* ASCII text that I have to manually position and print.
  draw_punctuated_line( 3,
                        start_column,
                        UL_CORNER,
                        HORIZONTAL_LINE,
                        TOP_TEE,
                        UR_CORNER );
  draw_punctuated_line( 4,
                        start_column,
                        VERTICAL_LINE,
                        ' ',
                        VERTICAL_LINE,
                        VERTICAL_LINE );
  VidPokes( MainWindow, 4, start_column +  1, " Port " );
  VidPokes( MainWindow, 4, start_column +  8, "IRQ " );
  VidPokes( MainWindow, 4, start_column +  13 , "Address" );
  VidPokes( MainWindow, 4, start_column +  21, "StatReg" );
  VidPokes( MainWindow, 4, start_column + 29, " OUT" );
  VidPokes( MainWindow, 4, start_column + 34, " IN " );
  draw_punctuated_line( 5,
                        start_column,
                        LEFT_TEE,
                        HORIZONTAL_LINE,
                        CENTER_TEE,
                        RIGHT_TEE );


  //* After the title lines are all out, I print out the data lines.  There
  //* are lines for 18 ports.
  for( i = 6 ; i < 24 ; i++ )
    draw_punctuated_line( i,
                          start_column,
                          VERTICAL_LINE,
                          ' ',
                          VERTICAL_LINE,
                          VERTICAL_LINE );


  //* Finally I print out the bottom line.
  draw_punctuated_line( 24,
                        start_column,
                        LL_CORNER,
                        HORIZONTAL_LINE,
                        BOTTOM_TEE,
                        LR_CORNER );
}


//*
//*  void draw_punctuated_line( int row,
//*                             int col,
//*                             int start_char,
//*                             int middle_char,
//*                             int tee_char,
//*                             int end_char )
//*
//*
//* ARGUMENTS
//*   row        :  The row to start the puncuated line on.
//*   col        :  The column to start the punctuated line on.
//*   start_char :  The first character on the punctuated line.
//*   middle_char:  The 'line' character part of the line.
//*   tee_char   :  The tee character that fits in the middle of the line.
//*   end_char   :  The last character on the line, the right corner.
//*
//* DESCRIPTION
//*   All of the lines that compose the box are "punctuated" in the same
//*   places.  They just use different punctuation characters.  So it
//*   made sense to have one routine print out box lines.
//*
//* SIDE EFFECTS
//*   None.
//*
//* RETURNS
//*   Nothing.
//*
//* AUTHOR
//*   SM          December 12, 1992
//*
//* MODIFICATIONS
//*
//*
void draw_punctuated_line(int row,
                          int col,
                          int start_char,
                          int middle_char,
                          int tee_char,
                          int end_char )
{
  int i;

  for( i = 0 ; i < 38 ; i++ )     //* I clear the line before printing
    VidPokec( MainWindow, row, i + col, middle_char );
  VidPokec( MainWindow, row, col + 0, start_char );
  VidPokec( MainWindow, row, col + 7, tee_char );
  VidPokec( MainWindow, row, col + 12, tee_char );
  VidPokec( MainWindow, row, col + 20, tee_char );
  VidPokec( MainWindow, row, col + 28, tee_char );
  VidPokec( MainWindow, row, col + 33, tee_char );
  VidPokec( MainWindow, row, col + 38, end_char );
}


//*
//* void print_port_info( int mode )
//*
//* ARGUMENTS
//*   mode : A 0 for traditional output mode, 1 for new and exciting
//*          display mode.
//*
//* DESCRIPTION
//*   DIGILOOP has space on the screen to monitor 36 ports.  The ports are
//*   displayed side by side in two boxes that are 18 ports high.  This routine
//*   is called during initialization.  It draws the boxes on the screen.
//*
//* SIDE EFFECTS
//*   None.
//*
//* RETURNS
//*   Nothing.
//*
//* AUTHOR
//*   SM          December 12, 1992
//*
//* MODIFICATIONS
//*
//*
void print_port_info( int mode )
{
  int port;

  DisplayMode = mode;
  initialize_display();
  if( !mode ) {
    draw_box( 0 );
    draw_box( 40 );
  }
  for( port = 0; port < MAX_PORT; port++ )
    if( PortList[ port ] && PortList[ port ]->IsActive )
      PortList[ port ]->PortDisplay( DisplayMode );
  VidGoto( MainWindow, 24, 0 );
}


//*
//* void print_port_receive( int port, int key )
//*
//* ARGUMENTS
//*   port   :  The port that received the character.
//*   key    :  The character to be displayed, or an error code.
//*
//* DESCRIPTION
//*   When loopback is actually running, this routine is called to print out
//*   the character received from a given por.  If
//*   a negative value is sent, it means it is an error code, and it is
//*   printed in decimal format so we can see what the error code is.  Else
//*   it is printed as an ASCII character.
//*
//* SIDE EFFECTS
//*   None.
//*
//* RETURNS
//*   Nothing.
//*
//* AUTHOR
//*   SM          December 12, 1992
//*
//* MODIFICATIONS
//*
//*
void print_port_receive( int port, int key )
{
  int row;
  int col;
  int name;

  if( !DisplayMode ) {
    name = PortList[ port ]->PortName;
    port = name;

    row = ( port % 18 ) + 6;
    col = 34 + ( ( port / 18 ) * 40 );
    if( key < 0 )
      VidPokef( MainWindow, row, col, "%4d", key );
    else
      VidPokef( MainWindow, row, col, "%4c", key );
  }
  else {
    if( key > 0 )
      VidPutc( PortList[ port ]->window, key );
    else
      VidPokef( PortList[ port ]->window, 1, 1, "%4d", key );
  }
}


//*
//* void print_port_transmit( int port, int key )
//*
//* ARGUMENTS
//*   port  :  The port number to be updated.
//*   key   :  The input data, which will either be a character or an
//*            integer error code.
//*
//* DESCRIPTION
//*   Once actual loopback is underway, this routine is called to print out
//*   the character that was just transmitted to a given port.  If the value
//*   is less than 0, we print out the value in decimal format, since it is
//*   probably an error code returned from asiputc(). Otherwise we print
//*   out the ASCII character in the appropriate box on the screen.  This
//*   routine is called every time a character is transmitted.
//*
//* SIDE EFFECTS
//*   None.
//*
//* RETURNS
//*   Nothing.
//*
//* AUTHOR
//*   SM          December 12, 1992
//*
//* MODIFICATIONS
//*
//*
void print_port_transmit( int port, int key )
{
  int row;
  int col;
  int name;

  if( !DisplayMode ) {
    name = PortList[ port ]->PortName;
    port = name;

    row = ( port % 18 ) + 6;
    col = 29 + ( ( port / 18 ) * 40 );
    if( key < 0 )
      VidPokef( MainWindow, row, col, "%4d", key );
    else
      VidPokef( MainWindow, row, col, "%4c", key );
  }
}



//***********************************************************************
//*                MISC FUNCTIONS
//***********************************************************************

//*
//* void print_help_and_exit( int pause )
//*
//* ARGUMENTS
//*  pause  : A flag that indicates whether the program should pause
//*           before exiting or just blow past.
//*
//* DESCRIPTION
//*  This routine is used when a fatal error is encountered that has anything
//*  to do with a command line option.
//*
//* SIDE EFFECTS
//*  None.
//*
//* RETURNS
//*  Nothing.
//*
//* AUTHOR
//*  SM          December 12, 1992
//*
//* MODIFICATIONS
//*
//*
void print_help_and_exit( int pause )
{
  VidClearWindow( MainWindow );
  VidGoto( MainWindow, 0, 0 );
  VidPuts( MainWindow, "DIGILOOP has the following options:\n");
  if( IsMicroChannelBus() )
    VidPuts( MainWindow, " 1-2         :  Test COMx:\n" );
  else
    VidPuts( MainWindow, " 1-2         :  Test COMx:\n" );
  if( IsMicroChannelBus() )
    VidPuts( MainWindow, " DIGIBOARD n :  Test Standard Digiboard n=board number\n" );
  else
    VidPuts( MainWindow, " DIGIBOARD n :  Test Standard Digiboard n=number of channels\n" );
  VidPuts( MainWindow,
           " IRQ x       :  Set shared interrupt number to "
           "x (default depends on board)\n" );
  VidPuts( MainWindow,
           " REGISTER x  :  Set status register address to (hex) x\n" );
  VidPuts( MainWindow,
           " UART x      :  Set first UART address to (hex) x\n" );
  VidPuts( MainWindow,
           " MONITOR x   :  Select port dump using SPLIT screen, "
           "PAGE mode, or MONO monitor\n" );
  VidPuts( MainWindow,
           " NUMBER x    : Number of Digiboards on system\n ");
  VidPuts( MainWindow, "\n");
  VidPuts( MainWindow, "Use as many options as you like."
                       "  Upper and lower case both work fine.\n\n" );
  VidPuts( MainWindow,
           "Options are scanned and executed from left to "
           "right, so be sure\n" );
  VidPuts( MainWindow,
           "to enter the I, R, U, and N parameters before "
           "the D.\n" );
  CleanUpAndExit( pause );
}


//*
//* int open_debug_window( char *command, char *option )
//*
//* ARGUMENTS
//* char//*command :  The command line command.  Looking for the word
//*                  MONITOR.
//* char//*option  :  The MONITOR option, which should be SPLIT, PAGE
//*                  or MONO.
//*
//* DESCRIPTION
//*   This routine is called during the parsing of the command line options.
//*   One of the new capabilities added to DIGILOOP during the 3.10 upgrade
//*   was the ability to perform a continuous DumpPortStatus on an alternate
//*   window.  The user can set up an alternate display window in one of three
//*   different configurations.  On a dual monitor system, "MONITOR MONO" puts
//*   the debug window on the mono card while multloop runs on the CGA/EGA/VGA
//*   card.  On an EGA or VGA card, "MONITOR SPLIT" puts the monitor in 43
//*   line mode, so DIGILOOP runs in the top of the screen and the debug
//*   status is on the bottom.  Finally, "MONITOR PAGE" lets the user of
//*   a CGA/EGA/VGA card put DIGILOOP on page 0 of his card, and the
//*   debug status on page 1.  Both pages can't be viewed at the same
//*   time, but the user can switch back and forth between pages with the
//*   ALT_M menu, or with the ALT_S hot key.
//*
//* SIDE EFFECTS
//*   The video mode might be changed, and the DebugStatus window
//*   might be created.
//*
//* RETURNS
//*   TRUE if valid monitor options were found, FALSE otherwise.
//*
//* AUTHOR
//*   SM               March 20, 1991
//*
//* MODIFICATIONS
//*
//*
int open_debug_window( char *command, char *option )
{
  int i;

  if( command == NULL || option == NULL )
    return( 0 );
  //*
  //* Here I capitalize both strings.  I would rather use stricmp() to do
  //* a case insensitive string compare, but there are compiler difficulties
  //* down that road.
  //*
  for( i = 0 ; option[ i ] != '\0' ; i++ )
    option[ i ] = (char) toupper( option[ i ] );
  for( i = 0 ; command[ i ] != '\0' ; i++ )
    command[ i ] = (char) toupper( command[ i ] );
  if( strcmp( command, "MONITOR" ) != 0 )
    return( 0 );
  if( strcmp( option, "SPLIT" ) == 0 ) {
    SplitScreen = VidSet80x43();
    if( SplitScreen ) {
      DebugWindow = VidDefineWindow( 25, 0, 42, 79, 0 );
      VidClearWindow( DebugWindow );
    }
    return( 1 );
  }
  if( strcmp( option, "PAGE" ) == 0 ) {
    DebugWindow = VidDefineWindow( 0, 0, 17, 79, 0 );
    if( DebugWindow ) {
      VidSetWindowPage( DebugWindow, 1 );
      PagedScreen = 1;
    }
    VidSelectFastDrivers();
    return( 1 );
  }
  if( strcmp( option, "MONO" ) == 0 ) {
    if( PhysicalWindow->address != (unsigned int far *) 0xb0000000L ) {
      DebugWindow = VidDefineWindow( 0, 0, 17, 79, 0 );
      if( DebugWindow )
        DebugWindow->address = (unsigned int far *) 0xb0000000L;
    }
    return( 1 );
  }
  return( 0 );
}


//*
//* int random_key( unsigned int guy )
//*
//* ARGUMENTS
//*   guy : The seed, usually the last key.
//*
//* DESCRIPTION
//*   When running loopback tests, it is nice to throw different characters
//*   up on the screen.  This little routine puts out a pseudo-random number
//*   based on an input key.  It uses a software SRG generator, and won't
//*   return any key that isn't a printable character.
//*
//* SIDE EFFECTS
//*   None.
//*
//* RETURNS
//*   Nothing.
//*
//* AUTHOR
//*   SM          December 12, 1992
//*
//* MODIFICATIONS
//*
//*
int random_key( unsigned int guy )
{
  int new_guy;

  if( guy == 0 )    //* A value of zero just generates more zeros, so
    guy = 1;        //* I have to kludge my way around this.  Likewise,
  guy &= 0x7f;      //* anything over 127 is not good, and so I mask out
                    //* the top bit.

  switch( guy >> 5 ) { //* This part of the code is dedicated to forming  */
                       //* the XOR product of bits 5 and 6 of the input   */
    case 0 :
    case 3 :
      new_guy = 0;
      break;
    default :
      new_guy = 1;
      break;
  }

  new_guy += ( guy * 2 ) & 0x7f;          //* The old byte is shifted
  if( new_guy > 0x20 && new_guy < 0x7f )  //* over one bit, then the XOR
    return( new_guy );                    //* product is stuffed into the
  else                                    //* lower place.  If I hit an
    return(random_key(new_guy));          //* unprintable key, I just try
                                          //* again until I get one that I
}                                         //* can live with


//*
//* void initialize_options( int argc, char *argv[] )
//*
//* ARGUMENTS
//*  argc   : The count of arguments, passed along from main().
//*  argv[] : The arguments, passed along from main.
//*
//* DESCRIPTION
//*   This routine reads the command line and sets up all the options.  This
//*   is essentially the first thing done in the program.  Note that this
//*   is not a fancy command line parser like you see in a lot of programs.
//*   Options have to be seperated by white space, and there is not any
//*   latitude for variances in the syntax.  Error checking is also minimal.
//*
//*   The arguments passed to this routine from the command line mostly
//*   deal with setup of ports.  They deal with them by either calling
//*   Install...() functions, or by setting global variables that will
//*   be used later when those functions are called.  Note that arguments
//*   on the command line are processed sequentially, so the order in which
//*   they appear is important.
//*
//* SIDE EFFECTS
//*   None.
//*
//* RETURNS
//*   Nothing.
//*
//* AUTHOR
//*   SM          December 12, 1992
//*
//* MODIFICATIONS
//*
//*
void initialize_options( int argc, char *argv[] )
{
  int port;
  int error_flag;
  static int FirstTime=1;

  //*
  //* This section of code initializes everything in the global database to
  //* its default setting.  If the user wants DIGILOOP to do anything at all,
  //* she will have to put something on the command line to change at least
  //* one or two of these options.
  //*
  for( port = 0 ; port < MAX_PORT ; port++ )
    PortList[ port ] = NULL;

  error_flag = 0;

  PossibleChannels = 0;
  statReg = 0;
  irq = -1;
  base = 0;
  board = 0;
  if( IsMicroChannelBus() )
    NextFreePort = COM9;
  else
    NextFreePort = COM3;

  if( argc < 1 )              //* If there are not command line args at
    print_help_and_exit( 0 ); //* all, consider it a fatal error.  Let the
                              //* users see what her options are.
  //*
  //* This big loop scans through the the command line, turning things on.  It
  //* is all pretty simple.
  //*
  while( argc ) {
    switch( toupper( argv[ 0 ][ 0 ] ) ) {
      case '1' :
      case '2' :
        SetUpRegularPort( argv[ 0 ][ 0 ] - '1' );
        break;
      case 'R' :
        sscanf( argv[ 1 ], "%x", &statReg );
        argc--;
        argv++;
        VidPrintf( MainWindow,
                   "Shared status register for the next port/board will be set to %03x\n",
                   statReg );
        break;
      case 'I' :
        sscanf( argv[ 1 ], "%d", &irq );
        argc--;
        argv++;
        VidPrintf( MainWindow,
                   "IRQ number for the next port/board will be set to IRQ%d\n",
                   irq );
        break;
      case 'D' :
        if( FirstTime ) {
          if( IsMicroChannelBus() )
            //*
            //* Here multiboard setup
            //*
            ;
          else
            SetUpDigiboard( atoi( argv[ 1 ] ) );
          FirstTime = 0;
        }
        argc--;
        argv++;
        break;
      case 'U' :
        sscanf( argv[ 1 ], "%x", &base );
        argc--;
        argv++;
        VidPrintf( MainWindow,
                   "Next UART address will be set to %03x\n",
                   base );
        break;
      case 'M' :
        if( open_debug_window( argv[ 0 ], argv[ 1 ] ) ) {
          argc--;
          argv++;
        }
        else
          error_flag++;
        break;
      case 'N' :
        sscanf( argv[ 1 ], "%d", &board );
        argc--;
        argv++;
        break;
      default :
        error_flag++;
    }
    if( error_flag ) {
      VidPrintf( MainWindow, "Unknown option: %s\n\n", argv[ 0 ] );
      print_help_and_exit( 1 );
    }
    argc--;
    argv++;
  }
}


//*
//*  void CleanUpAndExit( int pause )
//*
//* ARGUMENTS
//*
//*  pause : A flag that indicates whether the program should pause
//*          and wait for the user to hit a key before exiting.
//*
//* DESCRIPTION
//*
//*  This is the universal exit routine.  It cleans up the video before
//*  exiting.
//*
//* SIDE EFFECTS
//*
//*  None.
//*
//* RETURNS
//*
//*  Nothing.
//*
//* AUTHOR
//*
//*  SM          December 12, 1992
//*
//* MODIFICATIONS
//*
//*
void CleanUpAndExit( int pause )
{
  int ports;

  if( pause ) {
    VidPrintf( MainWindow, "Hit any key to continue..." );
    getkey();
  }
  for( ports = 0; ports < MAX_PORT; ports++ )
    if( PortList[ports] != NULL )
      delete( PortList[ports] );

  if( SplitScreen )
    VidSet80x25();
  else if( PagedScreen )
    VidSelectPage( 0 );
  VidTerminate( 0 );
  exit( 0 );
}


//*
//* void SetUpRegularPort( int port_number )
//*
//* ARGUMENTS
//* port_number : Port to set up. 1-4
//*
//* DESCRIPTION
//*
//* SIDE EFFECTS
//*
//* RETURNS
//*
//* AUTHOR
//*
//* Erika Parker        January 18, 1993
//*
//* MODIFICATIONS
//*
//*
void SetUpRegularPort( int port_number )
{
  PortList[ PossibleChannels ] = new MiniTerm( port_number );
  PortList[ PossibleChannels ]->PortName = port_number;
  PortList[ PossibleChannels ]->IRQNumber = PortList[ PossibleChannels ]->getIRQ();
  PortList[ PossibleChannels ]->BaseAddress = PortList[ PossibleChannels ]->getBaseAddress();
  PortList[ PossibleChannels ]->StatusRegisterAddress = 0;
  PossibleChannels++;
}


//*
//* void SetUpDigiboard( int channel_count )
//*
//* ARGUMENTS
//*
//*  channel_count : The number of ports on the Digiboard, 4, 8, or 16.
//*
//* DESCRIPTION
//*   This routine is used to set up the Digiboard products for the
//*   standard PC bus.  This actually means one of several boards which all
//*   look more or less the same to the software.  In any case, it can include
//*   up to 32 ports of Digiboard.  The port addressing scheme used by default
//*   is set up to accomodate two Digiboard PC/16 boards in the same system.
//*   The user cannot change any of the port addresses on these boards, so that
//*   kind of makes them a sensible default.  What this means is that if you are
//*   trying to set up a Digiboard COM/4 or COM/8, you probably want to use
//*   the port mapping shown below:
//*
//*      Interrupt:               IRQ3
//*      First Uart Address:      0x100
//*      Status Register Address: 0x140
//*
//*   Note that the UART addressing scheme is a little complicated.  The
//*   routine below forces you into it though, unless you feel like modifying the
//*   code.  The port addressing is forced into the scheme used by the /16 boards:
//*
//*    Board 0:    Port 1 :  0x100      Board 1:     Port 1 :   0x188
//*                Port 2 :  0x108                   Port 2 :   0x190
//*                Port 3 :  0x110                   Port 3 :   0x198
//*                Port 4 :  0x118                   Port 4 :   0x208
//*                Port 5 :  0x120                   Port 5 :   0x210
//*                Port 6 :  0x128                   Port 6 :   0x218
//*                Port 7 :  0x130                   Port 7 :   0x220
//*                Port 8 :  0x138                   Port 8 :   0x228
//*                Port 9 :  0x148                   Port 9 :   0x230
//*                Port 10 : 0x150                   Port 10 :  0x238
//*                Port 11 : 0x158                   Port 11 :  0x240
//*                Port 12 : 0x160                   Port 12 :  0x248
//*                Port 13 : 0x168                   Port 13 :  0x250
//*                Port 14 : 0x170                   Port 14 :  0x258
//*                Port 15 : 0x178                   Port 15 :  0x260
//*                Port 16 : 0x180                   Port 16 :  0x268
//*
//*   What the addressing scheme means is that you can test two PC/16 boards
//*   using the default setup.  If you have a COM/4 or a COM/8 you need to
//*   set the DIP switches up to use these addresses.
//*
//*   On the command line, you need to give a number of ports to test.  If
//*   you are testing a COM/4, type "DIGILOOP D 4", for example.
//*
//*   Dip Switch and Jumper settings for a COM/8 are shown below:
//*
//*
//*                                        --------------------------
//*                                        | o     o     o  o  o  o | ON
//*   Status port selection DS1: 0x140     |    o     o             | OFF
//*                                        | 1  2  3  4  5  6  7  8 |
//*                                        --------------------------
//*
//*                                        --------------------------
//*                                        | o     o  o  o  o  o  o | ON
//*   UART Port 1 selection DS2: 0x100     |    o                   | OFF
//*                                        | 1  2  3  4  5  6  7  8 |
//*                                        --------------------------
//*
//*                                        --------------------------
//*                                        | o     o  o  o  o     o | ON
//*   UART Port 2 selection DS3: 0x108     |    o              o    | OFF
//*                                        | 1  2  3  4  5  6  7  8 |
//*                                        --------------------------
//*
//*                                        --------------------------
//*                                        | o     o  o  o     o  o | ON
//*   UART Port 3 selection DS4: 0x110     |    o           o       | OFF
//*                                        | 1  2  3  4  5  6  7  8 |
//*                                        --------------------------
//*
//*                                        --------------------------
//*                                        | o     o  o  o        o | ON
//*   UART Port 4 selection DS5: 0x118     |    o           o  o    | OFF
//*                                        | 1  2  3  4  5  6  7  8 |
//*                                        --------------------------
//*
//*                                        --------------------------
//*                                        | o     o  o     o  o  o | ON
//*   UART Port 5 selection DS6: 0x120     |    o        o          | OFF
//*                                        | 1  2  3  4  5  6  7  8 |
//*                                        --------------------------
//*
//*                                        --------------------------
//*                                        | o     o  o     o     o | ON
//*   UART Port 6 selection DS7: 0x128     |    o        o     o    | OFF
//*                                        | 1  2  3  4  5  6  7  8 |
//*                                        --------------------------
//*
//*                                        --------------------------
//*                                        | o     o  o        o  o | ON
//*   UART Port 7 selection DS8: 0x130     |    o        o  o       | OFF
//*                                        | 1  2  3  4  5  6  7  8 |
//*                                        --------------------------
//*
//*                                        --------------------------
//*                                        | o     o  o           o | ON
//*   UART Port 8 selection DS9: 0x138     |    o        o  o  o    | OFF
//*                                        | 1  2  3  4  5  6  7  8 |
//*                                        --------------------------
//*
//*                                    |-|
//*   IRQ line selection:  IRQ3:       |o|   o    o    o    o    o
//*                                    |o|   o    o    o    o    o
//*                                    |-|
//*                                    J85  J86  J87  J88  J89  J90
//*                                    IRQ3 IRQ5 IRQ7 IRQ6 IRQ4 IRQ2
//*
//*                                _   _   _   _   _   _   _   _
//* Interrupt selection: all ODD: |o| |o| |o| |o| |o| |o| |o| |o|  o   o
//* (Board ID 0)                  | | | | | | | | | | | | | | | |  _   _
//*                               |o| |o| |o| |o| |o| |o| |o| |o| |o| |o|
//*                                -   -   -   -   -   -   -   -  | | | |
//*                                o   o   o   o   o   o   o   o  |o| |o|
//*                                                                -   -
//*                                J1  J2  J3  J4  J5  J6  J7  J8  J9  J10
//*                                P1  P2  P3  P4  P5  P6  P7  P8  BOARD ID
//*
//*
//*  SIDE EFFECTS
//*   None.
//*
//*  RETURNS
//*   Nothing.
//*
//*  AUTHOR
//*   SM          December 12, 1992
//*
//*  MODIFICATIONS
//*
//*
void SetUpDigiboard( int channel_count )
{
  int offset;
  unsigned char board_count[4];
  unsigned short base_addresses[32];
  unsigned short current_base;

  if( board == 0 || board > 4 )
    board = 1;
  if( channel_count * board <= 0 || channel_count * board > 32) {
    VidPrintf( MainWindow, "Funny number of channels requested.\n" );
    print_help_and_exit( 1 );
  }
  if( statReg == 0 )
    statReg = 0x140;
  if( irq == -1 )
    irq = IRQ3;
  if( base == 0 )
    current_base = 0x100;
  else
    current_base = base;

  for( offset = 0; offset < 4; offset++ ) {
    if( offset < board )
      board_count[ offset ] = (unsigned char)channel_count;
    else
      board_count[ offset ] = 0;
  }
  for( offset = 0; offset < channel_count * board; offset++ ) {
    base_addresses[ offset ] = current_base;
    current_base += 8;
    if( current_base == 0x140 )
      current_base = 0x148;
    else if( current_base == 0x1a0 )
      current_base = 0x208;
  }

  GFDigiPC::MultiBoardSetup( NextFreePort, statReg,
                             board_count[ 0 ], board_count[ 1 ],
                             board_count[ 2 ], board_count[ 3 ] );

#ifdef DEBUG
  VidPrintf( MainWindow, "GFDigiPC::MultiBoardSetup( %d, %5x, %d, %d, %d, %d );\n",
             NextFreePort, statReg, board_count[0], board_count[1], board_count[2],
             board_count[3] );
#endif

  for( offset = 0 ; offset < channel_count * board; offset++ ) {
    PortList[ PossibleChannels ] = new MiniTerm( NextFreePort + offset,
                                                 irq,
                                                 base_addresses[ offset ],
                                                 statReg );
#ifdef DEBUG
    VidPrintf( MainWindow,
               "PortList[%d] = new MiniTerm( %d, %d, %5x, %5x );\n ",
               PossibleChannels,
               NextFreePort + offset,
               irq,
               base_addresses[ offset ],
               statReg );
#endif
    PossibleChannels++;
  }
  NextFreePort += channel_count;
  irq = -1;
  base = -1;
  statReg = -1;
  board = 0;
}


//*
//* void display_and_qualify_options( void )
//*
//* ARGUMENTS
//*  None.
//*
//* DESCRIPTION
//*   After the program has figured out what all the options are, this routine
//*   does two things.  It makes sure the options are legitimate values, and
//*   it displays the for the user to look at.  Any fatal option errors seen
//*   in here will cause a bail out with a syntax lesson for the user.
//*
//* SIDE EFFECTS
//*   None.
//*
//* RETURNS
//*   Nothing.
//*
//* AUTHOR
//*   SM          December 12, 1992
//*
//* MODIFICATIONS
//*
//*
void display_and_qualify_options( void )
{
  if( IsMicroChannelBus() )
    VidPrintf( MainWindow, "Microchannel architecture.\n" );
  else
    VidPrintf( MainWindow, "Standard PC bus.\n" );
  VidPrintf( MainWindow, "Baud Rate is 1200L\n");
  VidPrintf( MainWindow,
             "You need a loopback connector or a modem "
             "for each RS232 port.\n" );
  VidPrintf( MainWindow, "\n" );
  VidPrintf( MainWindow, "Active keys during execution:\n" );
  VidPrintf( MainWindow, "\n" );
  VidPrintf( MainWindow, "ALT-A : Toggles auto mode        "
                         "ALT-N : Port dump display goes to next port\n" );
  VidPrintf( MainWindow, "ALT-S : Switch display pages     " );
  VidPrintf( MainWindow, "ALT-T : Toggle display mode\n" );
  VidPrintf( MainWindow, "ESC   : Exit\n" );
}


//*
//* int churn_key( int key )
//*
//* ARGUMENTS
//*   key    :  The key pressed by the user, which will be spit out, or
//*             the current key value in auto mode.
//*
//* DESCRIPTION
//*   Churn key just takes a key as input, then spits characters out to
//*   all the active ports.  While it is doing that, it is also updating
//*   the display with the input characters, output characters, and port
//*   status information.
//*
//*   Note that if you are not running in auto mode, you can type commands
//*   to the first active port in the list, because on the first port your
//*   keystrokes aren't randomized.  The second through nth ports all get
//*   garbage.
//*
//* SIDE EFFECTS
//*   None.
//*
//* RETURNS
//*   A new random key value.
//*
//* AUTHOR
//*   SM          December 12, 1992
//*
//* MODIFICATIONS
//*   December 12, 1992     4.00A : Initial release
//*
//*
int churn_key( int key )
{
  int port;
  int status;
  int c;

  for( port = 0 ; port < MAX_PORT ; port++ ) {
    if( DisplayMode )       //* This is a little trick.
      port = ActivePort;
    if( PortList[ port ] && PortList[ port ]->IsActive ) {
      if(( status = PortList[ port ]->cp->PutChar( (unsigned char)key ) ) != GCPP_OK )
        print_port_transmit( port, status );
      else
        print_port_transmit( port, key );
      if( !DisplayMode )
        key = random_key( key );
    }
    if( DisplayMode )       //* So it will only be done once
      port = MAX_PORT;
  }
  if( ActiveChannels < 3 )
    UseSysTime( PortList[ port ]->cp, 2 );
  for( port = 0 ; port < MAX_PORT ; port++ ) {
    if( DisplayMode )       //* This is a little trick.
      port = ActivePort;
    if( PortList[ port ] && PortList[ port ]->IsActive ) {
      c = PortList[ port ]->cp->GetChar();
      print_port_receive( port, c );
    }
    if( DisplayMode )       //* So it will only be done once
      port = MAX_PORT;
  }
  return( key );
}


//*
//* int handle_command( int menu_key )
//*
//* ARGUMENTS
//*   menu_key   :   The keystroke the user pressed.
//*
//* DESCRIPTION
//*   When the user presses and ALT key, or some other non-ASCII key,
//*   this routine gets called to deal with it.  It processes the command
//*   and then returns.
//*
//* SIDE EFFECTS
//*   None.
//*
//* RETURNS
//*   0 means exit the program, 1 means all is okay, please continue.
//*
//* AUTHOR
//*   SM          December 12, 1992
//*
//* MODIFICATIONS
//*
//*
char *menu[] = {
    "Auto Transmit Mode  ALT_A",
    "Next Debug Port     ALT_N",
    "Toggle Display Mode ALT_T",
    "Next Port           TAB",
    "Exit                ALT_X",
    ""
};

int handle_command( int menu_key )
{
  if( menu_key == ALTM )
    menu_key = VidMenu( 5, 20, menu );
  switch( menu_key ) {
    case ALTA :     //* Toggle automode
    case 0    :
      AutoMode = !AutoMode;
      break;
    case ALTN :     //* Go to next debug port
    case 1    :
      do {
        DebugPort++;
        if( DebugWindow )
          VidClearWindow( DebugWindow );
          if( DebugPort >= MAX_PORT ) {
            DebugPort = -1;
            break;
          }
      } while( !PortList[ DebugPort ] );
      break;
    case ALTT :     //* Toggle display mode
    case 2    :
      if( DisplayMode == 0 )
        print_port_info( 1 );
      else
        print_port_info( 0 );
      break;
    case '\t':   //* Tab between windows only in DisplayMode = 1
    case 3:
      if( DisplayMode && !AutoMode ) {
        while( ! PortList[ActivePort]->IsActive ) {
          if( ActivePort >= MAX_PORT )
            ActivePort = 0;
          else
            ActivePort++;
        }
        if( ActivePort >= MAX_PORT )
          ActivePort = 0;
        else
          ActivePort++;
        PortList[ ActivePort ]->TerminalGoto();
      }
      break;
    case ALTX :     //* Exit the program, various ways.
    case ESC  :
    case 4    :
      return( 0 );
  }
  return( 1 );
}


//*
//* void DebugStatusPuts( char *buf )
//*
//* ARGUMENTS
//*   buf  :  The buffer to be displayed.
//*
//* DESCRIPTION
//*   This is the callback routine used by the the port status dump routine.
//*   All it does is display buffers in the dump windows.
//*
//* SIDE EFFECTS
//*   None.
//*
//* RETURNS
//*   Nothing.
//*
//* AUTHOR
//*   SM          December 12, 1992
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
//* void send_auto_data( void )
//*
//* ARGUMENTS
//*   None
//*
//* DESCRIPTION
//*   When in auto mode, every active port gets a semi-random string
//*   spit out continuosly.  It isn't completely random.  For one thing,
//*   it always has something like : "COM34" right in the middle of it.
//*   This is so when you are doing loopback and connection experiments
//*   you can tell who is connected to what.  Note that the semi random
//*   strings are stored in the AutoStrings[] array.  Note also that
//*   because of the unique nature of the display in Auto / Alternate
//*   mode, the strings are output backwards!
//*
//* SIDE EFFECTS
//*   None.
//*
//* RETURNS
//*   Nothing.
//*
//* AUTHOR
//*   SM          December 12, 1992
//*
//* MODIFICATIONS
//*
//*
void send_auto_data( int i )
{
  int port;
  int status;
  int c ;

  for( port = 0 ; port < MAX_PORT ; port++ ) {
    if( PortList[ port ] && PortList[ port ]->IsActive ) {
      c = random_key( i );
      status = PortList[ port ]->cp->PutChar( (unsigned char)c );
      if( status != GCPP_OK )
        print_port_transmit( port, status );
      else
        print_port_transmit( port, c );
    }
  }
  if( ActiveChannels < 3 )
    UseSysTime( PortList[ port ]->cp, 2 );
  for( port = 0 ; port < MAX_PORT ; port++ ) {
    if( PortList[ port ] && PortList[ port ]->IsActive ) {
      c = PortList[ port ]->cp->GetChar();
      print_port_receive( port, c );
    }
  }
}


//*
//* void perform_loopback( void )
//*
//* ARGUMENTS
//*   None.
//*
//* DESCRIPTION
//*   This is the main routine for the program.  All it does is put out a
//*   random character on all the opened ports.  It then reads back a single
//*   character from all the ports.  The results of the write and read
//*   operations are displayed on the screen.  If the user requests it, the
//*   loop goes into automode, where it continually outputs new characters
//*   to the ports.  When it is not in automode, it only outputs characters
//*   as the user types them in.  As it is reading in characters, it passes
//*   any ALT and Function keys to the command handler.  Normal keys are
//*   written out to the port.
//*
//* SIDE EFFECTS
//*   None.
//*
//* RETURNS
//*   Nothing.
//*
//* AUTHOR
//*   SM          December 12, 1992
//*
//* MODIFICATIONS
//*
//*
void perform_loopback( void )
{
  int key = 1;
  int ticker = 0;

  AutoMode = 0;
  ActivePort = 0;
  for( ; ; ) {          //* This loop runs until the routine exits
    if( gfkbhit() ) {
      key = getkey();
      if( (key & 0xff ) == 0 || key == ESC || key == '\t' ) {
        if( !handle_command( key ) ) {
          CleanUpAndExit( 0 );
          return;
        }
      }
      else
        key = churn_key( key );
    }
    else if( AutoMode ) {
      send_auto_data( ticker );
      ticker = ++ticker % 32;
    }
    if( DebugPort != -1 )
      PortList[ DebugPort ]->DumpStatus();
  }
}


//*
//* int main( int argc, char *argv[] )
//*
//* ARGUMENTS
//*   argc   : The count of command line arguments.  They aren't used
//*            here, instead they get passed along to initialize_options().
//*   argv[] : Pointers to the command line arguments.
//*
//* DESCRIPTION
//*   The main section of the program.  The program goes through three major
//*   sections.  The first section involves initialization.  This consists
//*   of scanning the input line and setting up all the appropriate switches.
//*   The options are displayed on the screen for the user to see.  The next
//*   section consists of calls to PortOpen() for all the ports.
//*   Finally, the loopback phase runs until the user kills it with the
//*   Escape key.
//*
//* SIDE EFFECTS
//*   None.
//*
//* RETURNS
//*   Nothing.
//*
//* AUTHOR
//*   SM          December 12, 1992
//*
//* MODIFICATIONS
//*
//*
int main( int argc, char *argv[] )
{
  int port;
  int status;

  VidInitialize( VID_NORMAL, TRUE );
  MainWindow = VidDefineWindow( 0, 0, 24, 79, 0 );
  initialize_display();             //* The display is initialized, then
                                    //* the command line is parsed.
  initialize_options( --argc, ++argv );
  VidPrintf( MainWindow,"%d PossibleChannelsSelected\n", PossibleChannels );
  display_and_qualify_options();

  //*
  //* At this point, I am ready to call the PortOpen() function for every
  //* port selected. The ones selected have all been flagged in the TryToOpen
  //* array.  A failure to open a port is flagged, but the program will
  //* continue to run.
  //*
  for( port = 0 ; port < MAX_PORT ; port++ ) {
    if( PortList[ port ] ) {
      status = PortList[port]->PortOpen();
      if( status < GCPP_OK ) {
        VidPrintf( MainWindow,
                   "Can't open COM%d:, Status = %d\n",
                   PortList[port]->PortName + 1,
                   status );
        PortList[ port ]->IsActive = 0;
      }
      else {
        PortList[ port ]->IsActive = 1;
        ActiveChannels++;
      }
    }
  }
  //*
  //* Now that I know how many windows we really have, I will set the row and
  //* column for every class and define it's window in case the user decides
  //* to use the other display mode.
  //*
  SetCoordinates( ActiveChannels );
  //*
  //* The last part of the program just prints all the port info on the
  //* screen, then runs loopback mode until the user stops it.
  //*
  if( ActiveChannels > 0 ) {
    VidPrintf( MainWindow,"\n%d ActiveChannels\n", ActiveChannels );
    VidPrintf( MainWindow, "\nHit any key to begin testing..." );
    getkey();
    print_port_info( 0 );
    perform_loopback();
  }
  else {
    VidPrintf( MainWindow, "\nNo ports to monitor " );
    getkey();
  }
  CleanUpAndExit( 0 );
  return( 0 );
}

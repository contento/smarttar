//*
//* FLOWDOSD.CPP        3.00A  June 15, 1995
//*
//* Contains: FLOWDOS demo
//*
//* The Greenleaf Comm++ Library
//*
//* Copyright (C) 1991-95 Greenleaf Software Inc.  All Rights Reserved.
//*
//* DESCRIPTION
//*
//* This program, FLOWDOS, is used to test/demo flow control between
//* two PCs.  In order to set it up, you need to connect two PCs together,
//* and then run FLOWDOS on both PCs.  If you have a null modem cable
//* that swaps RTS and CTS, you can test that type of flow control
//* Otherwise, you will just have to settle for testing XON/XOFF
//* flow control
//*
//* FLOWDOS puts up three windows on your screen.  Window 1 shows all
//* incoming data.  Window 2 shows a log of errors, packets sent, and
//* some other stuff.  Window 3 shows the port status dump.
//*
//* Once FLOWDOS is started up on both PCs, you can use the ALT keys shown
//* on the menu to turn either of the flow control mechanisms on or off.
//* You can start and stop FLOWDOS from sending packets by pressing
//* ALT_G.  When the remote end is sending packets, FLOWDOS keeps an eye
//* on what is coming in.  If a packet comes in that doesn't look good,
//* the error counter gets incremented.  The + and - keys adjust the ratio
//* between the amount of time FLOWDOS spends reading in characters versus
//* the amount of time spent writing characters.
//*
//* FLOWDOS has two command line options.  The first is a port
//* handle, which should be 0 for COM1, 1 for COM2, etc.  You can
//* have up to two ports running at the same time in case you want to test
//* flow control on the same machine.  The second is the NOSPLIT command, which
//* causes FLOWDOS to not try to split the screen. I find this
//* handy when running under DESQView.
//*
//* Five new keyboard commands are available.  F1 and F2 set and clear DTR,
//* F3 and F4 set and clear RTS.  With a breakout box, and some jumper
//* wires, you can force FLOWDOS into blocking states.  F10 swaps the
//* DumpPortStatus output port, back and forth between input and output.
//*
//* COMMAND SUMMARY :
//*
//*   +/-        :  These two keys alter the ration between input
//*                 and output time.  You can jack the ration between
//*                 the two up and down in order to force handhsaking.
//*
//*  ALT-X       :  Toggle XON/XOFF handshaking.
//*
//*  ALT-R       :  Toggle RTS/CTS handshaking.
//*
//*  ALT-D       :  Toggle DTR/DSR handshaking.
//*
//*  ALT-G       :  Toggle run/stop.
//*
//*  ALT-I       :  Initialize, reset error counters.
//*
//*  ALT-B       :  Cycle through the list of baud rates.
//*
//*  F1          :  Set DTR high.
//*
//*  F2          :  Set DTR low.
//*
//*  F3          :  Set RTS high.
//*
//*  F4          :  Set RTS low.
//*
//*  F10         :  Switch between ports, if you have more than one.
//*
//*  ESC         :  Exit.
//*
//* AUTHOR
//*
//*  SM               March 20, 1991
//*
//* MODIFICATIONS
//*
//*   April 1, 1993  :  Release for 2.00A
//*

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "compplib.hpp"
#include "ibmkeys.h"
#include "video.h"


//*
//* Local function prototypes.
//*
void handle_command( void );
void main( int argc, char *argv[] );
void DebugStatusPuts( char *buf );
void open_debug( int split );
void print_help( void );


//*
//* Class definition.
//*
class MiniTerm
{
  public:
    MiniTerm( int name, int mode );
    ~MiniTerm( void )
      { if ( cp ) delete cp; if ( sio ) delete sio; }
    int PortOpen( void );
    int PortClose( void );
    void DumpStatus( void )
      { cp->ViewPortStatus( DebugStatusPuts ); }
    void print_packet_number( void );
    void print_current_state( void );
    void InitializePortDisplay( int mode );
    int PortName;
    int FlowType; //* 0-None 1-XonXoff 2-RtsCts 3-DtrDsr
    int Running;
    int rx_count;
    int tx_count;
    unsigned int error_count;
    char outbound_buffer[ 81 ];
    char inbound_buffer[ 81 ];
    unsigned int outbound_packet_number;
    unsigned int inbound_packet_number;
    int out_index;
    int in_index;
    VID_WINDOW *MainWindow;
    VID_WINDOW *MessageWindow;
    GFI8250 *sio;
    GFSerial *cp;
    GFDataFormat *df;
    GFLineStatus *ls;
};


//*
//* Global variables.  There are lots of them, mostly out of laziness.
//*
long baud_rate = 1200L;
MiniTerm *PortList[2];
int Active = 0;
int ActivePorts = 0;
VID_WINDOW *DebugWindow;


//*
//*  MiniTerm::MiniTerm( int name, int mode )
//*
//*  DESCRIPTION
//*    The constructor will initialize some values for the class.
//*
//*  RETURNS
//*    NONE
//*
//*  AUTHOR
//*    Erika Parker
//*
//*  MODIFICATIONS
//*
//*
MiniTerm::MiniTerm( int name, int mode )
{
  PortName = name;
  FlowType = 0;
  Running = 0;
  rx_count = 50;
  tx_count = 50;
  error_count = 0;
  strcpy( outbound_buffer,"00000<0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ>\r\n" );
  outbound_packet_number = 0;
  inbound_packet_number = 0;
  out_index = 0;
  in_index = 0;
  MainWindow=NULL;
  InitializePortDisplay( mode );
}


//*
//*  MiniTerm::InitializePortDisplay( void )
//*
//*  DESCRIPTION
//*    InitializePortDisplay() will open the Main and Message Windows for
//*    the associated port.
//*
//*  RETURNS
//*    NONE
//*
//*  AUTHOR
//*    Erika Parker
//*
//*  MODIFICATIONS
//*
//*
void MiniTerm::InitializePortDisplay( int mode )
{
  if( mode ) {
    MainWindow = VidDefineWindow( 1, 1, 23, 53, 0 );
    VidDrawBorder( MainWindow );
    MessageWindow = VidDefineWindow( 1, 56, 23, 78, 0 );
    VidDrawBorder( MessageWindow );
  }
  else {
    MainWindow = VidDefineWindow( 0, 0, 7, 54, 0 );
    MessageWindow = VidDefineWindow( 1, 56, 6, 78, 0 );
    VidDrawBorder( MessageWindow );
  }
  VidClearWindow( MainWindow );
  VidClearWindow( MessageWindow );
  VidSaveWindow( MainWindow );
  VidSaveWindow( MessageWindow );
}


//*  MiniTerm::PortOpen( void )
//*
//*  DESCRIPTION
//*    PortOpen() will open the port using GFI8250.
//*
//*  RETURNS
//*    NONE
//*
//*  AUTHOR
//*    Erika Parker
//*
//*  MODIFICATIONS
//*
//*
MiniTerm::PortOpen( void )
{
  unsigned short error;

  sio = new GFI8250( PortName );
  cp = new GFSerial( sio );
  ls = cp->GetLineStatus();
  df = cp->GetDataFormat();
  return( cp->GetCommError( error ) );
}



//*
//*  void main( int argc, char *argv[] );
//*
//* ARGUMENTS
//*
//*  int argc :      The command line argument count.
//*
//*  char *argv[] :  There are two possible arguments on the command
//*                  line.  One is a port number, and the other is
//*                  the NOSPLIT flag.  Two port numbers can be
//*                  entered, which will let you use different
//*                  ports for input and output.
//*
//* DESCRIPTION
//*
//* The main program here is real simple.  It first opens the port(s), does
//* some other setup, prints some messages. It then goes into the master
//* control loop.  The master control loop does four things.  First, it
//* handles any keyboard input.  Then, it sends some characters.  Next,
//* it receives some characters.  Last, it outputs the new port status.
//* That is it.
//*
//* SIDE EFFECTS
//*
//*  None
//*
//* RETURNS
//*
//*  None, exit to DOS is from handle_command().
//*
//* AUTHOR
//*
//*  SM               March 20, 1991
//*
//* MODIFICATIONS
//*
//*
void main( int argc, char *argv[] )
{
  int c;
  int i;
  int p;
  int input_port_number;
  int output_port_number;
  int temp;
  int split;
  int status;

  split = 1;
  input_port_number = -1;
  output_port_number = -1;
  while( --argc > 0 ) {
    ++argv;
    if( strcmp( *argv, "NOSPLIT" ) == 0 )
      split = 0;
    else if( sscanf( *argv, "%d", &temp ) == 1 ) {
      if( input_port_number == -1 )
        input_port_number = temp;
      else if( output_port_number == -1 )
        output_port_number = temp;
      else {
        printf( "Too many ports on the command line!\n" );
        return;
      }
    }
  }
  open_debug( split );
  if( input_port_number == -1 )
    input_port_number = 0;
  PortList[ 0 ] = new MiniTerm( input_port_number, split );
  if( output_port_number != -1 && output_port_number != PortList[ 0 ]->PortName )
    PortList[ 1 ] = new MiniTerm( output_port_number, split );

  if( PortList[0] ) {
    status = PortList[ 0 ]->PortOpen();
    if( status < GCPP_OK ) {
      printf( "Error: %d opening COM%d", status, input_port_number + 1 );
      delete( PortList[ 0 ] );
      return;
    }
    ActivePorts++;
    Active=0;
  }
  if( PortList[1] ) {
    status = PortList[ 1 ]->PortOpen();
    if( status < GCPP_OK ) {
      printf( "Error: %d opening COM%d", status, output_port_number + 1 );
      delete( PortList[ 0 ] );
      delete( PortList[ 1 ] );
      return;
    }
    ActivePorts++;
  }

  print_help();
  PortList[ Active ]->print_current_state();

  for( ; ; ) {
    if( gfkbhit() )
      handle_command();

    //*
    //*  This is the GetChar() and PutChar() loop. It is done for both ports
    //*  ( if there second one exist. )
    //*
    for( p = 0; p < ActivePorts; p++ ) {
      //*
      //* If FLOWDOS has been set to go with ALT_G, he will send out some
      //* characters in the current packet here.  Every time a packet is
      //* sent out, this part of the program has to build a new packet with
      //* an updated packet number.
      //*
      if( PortList[ p ]->Running ) {
        for( i = 0 ; i < PortList[p]->tx_count ; i++ ) {
          if( PortList[ p ]->out_index == 0 ) {
            sprintf( PortList[ p ]->outbound_buffer, "%-5u",
                         ++PortList[ p ]->outbound_packet_number );
            PortList[ p ]->outbound_buffer[ 5 ] = '<';
            PortList[ p ]->print_packet_number();
          }
          if( PortList[ p ]->cp->PutChar(
              PortList[ p ]->outbound_buffer[ PortList[ p ]->out_index ] )
              < GCPP_OK )
            break;
          if( PortList[ p ]->outbound_buffer[ ++PortList[ p ]->out_index ] == '\0' )
            PortList[ p ]->out_index = 0;
        }
      }
      //*
      //* Regardless of the ALT_G setting, I always read in new characters.
      //* If I get the end of a packet, I see if it looks good.  If it
      //* doesn't, I bump the error flag.
      //*
      for( i = 0 ; i < PortList[ p ]->rx_count ; i++ ) {
        c = PortList[ p ]->cp->GetChar();
        if( c < GCPP_OK )
          break;
        PortList[ p ]->inbound_buffer[ PortList[ p ]->in_index++ ] = (char) c;
        if( c == '\n' || PortList[p]->in_index == 80 ) {
          PortList[ p ]->inbound_buffer[ PortList[ p ]->in_index ] = '\0';
          PortList[ p ]->in_index = 0;
          if( strcmp( PortList[ p ]->inbound_buffer + 5,
              PortList[ p ]->outbound_buffer + 5 ) ) {
            PortList[ p ]->error_count++;
            PortList[ p ]->print_packet_number();
          }
        }
        if( p == Active )
          VidPutc( PortList[ p ]->MainWindow, c );
      }
    }
    PortList[Active]->DumpStatus();
  }
}


//*
//*  void MiniTerm::open_debug( int split )
//*
//* ARGUMENTS
//*
//*  int split    :      If this flag is set to 0, FLOWDOS won't go into
//*                      split screen mode even if running on EGA/VGA.
//*
//* DESCRIPTION
//*
//* This routine opens up the DEBUG window that FLOWDOS uses.  If possible,
//* the screen is put into 43 line mode so there is more room for
//* stuff, even if it is kind of squinty.
//*
//* SIDE EFFECTS
//*
//*  Screen may be in 43 line mode.
//*
//* RETURNS
//*
//*  None.
//*
//* AUTHOR
//*
//*  SM               March 20, 1991
//*
//* MODIFICATIONS
//*
//*
void open_debug( int split )
{
  int monitor_type;

  monitor_type = VidInitialize( VID_NORMAL, TRUE );
  if( split ) {
    if( monitor_type == VID_EGA || monitor_type == VID_VGA || monitor_type == VID_MCGA )
      split = 1;
    else
      split = 0;
  }
  if( split ) {
    VidSet80x43();
    DebugWindow = VidDefineWindow( 25, 0, 42, 79, 0 );
  }
  else
    DebugWindow = VidDefineWindow( 8, 0, 24, 79, 0 );
  VidSetAttribute( DebugWindow, VID_REVERSE );
  VidClearWindow( DebugWindow );
}


//*
//*  void handle_command()
//*
//* ARGUMENTS
//*
//*  None.
//*
//* DESCRIPTION
//*
//* If the user hits a key, the main control loop calls this routine.
//* This routine just modifies one of the many local variables that
//* describe the current state of FLOWDOS.  If by chance our user selected
//* the ESC key, this program exits.
//*
//* SIDE EFFECTS
//*
//*  Lots of global variables may be modified.
//*
//* RETURNS
//*
//*  None.
//*
//* AUTHOR
//*
//*  SM               March 20, 1991
//*
//* MODIFICATIONS
//*
//*
void handle_command()
{
  int c;

  c = getkey();

  switch( c ) {
    case '+'    :
      PortList[ Active ]->rx_count -= 5 ;
      PortList[ Active ]->tx_count += 5 ;
      break;
    case '-'    :
      PortList[ Active ]->rx_count += 5 ;
      PortList[ Active ]->tx_count -= 5 ;
      break;

    //*
    //* Next three case statements can be optimized ( Erika )
    //*
    case ALTX    :
      if( PortList[ Active ]->FlowType != 1 ) {
        PortList[ Active ]->ls->SetHandShake( XONXOFF, XONXOFF );
        PortList[ Active ]->FlowType = 1;
      }
      else {
        PortList[ Active ]->ls->SetHandShake( GCPP_NONE, GCPP_NONE );
        PortList[ Active ]->FlowType = 0;
      }
      break;
    case ALTR :
      if( PortList[ Active ]->FlowType != 2 ) {
        PortList[ Active ]->ls->SetHandShake( RTS, CTS );
        PortList[ Active ]->FlowType = 2;
      }
      else {
        PortList[ Active ]->ls->SetHandShake( GCPP_NONE, GCPP_NONE );
        PortList[ Active ]->FlowType = 0;
      }
      break;
    case ALTD :
      if( PortList[ Active ]->FlowType != 3 ) {
        PortList[ Active ]->ls->SetHandShake( DTR, DSR );
        PortList[ Active ]->FlowType = 3;
      }
      else {
        PortList[ Active ]->ls->SetHandShake( GCPP_NONE, GCPP_NONE );
        PortList[ Active ]->FlowType = 0;
      }
      break;
    case ALTG :
      PortList[ Active ]->Running = !PortList[ Active ]->Running;
      break;
    case ALTI :
      PortList[ Active ]->outbound_packet_number = 0;
      PortList[ Active ]->inbound_packet_number = 0;
      PortList[ Active ]->out_index = 0;
      PortList[ Active ]->in_index = 0;
      PortList[ Active ]->error_count = 0;
      PortList[ Active ]->error_count = 0;
      break;
    case ALTB :
      if( baud_rate == 38400L )
        baud_rate = 57600L;
      else if( baud_rate == 57600L )
        baud_rate = 115200L;
      else if( baud_rate == 115200L )
        baud_rate = 300L;
      else
        baud_rate *= 2;
      PortList[ Active ]->df->BaudRate( baud_rate );
      break;
    case F1 :
      PortList[ Active ]->ls->Dtr( GCPP_ON );
      break;
    case F2 :
      PortList[ Active ]->ls->Dtr( GCPP_OFF );
      break;
    case F3 :
      PortList[ Active ]->ls->Rts( GCPP_ON );
      break;
    case F4 :
      PortList[ Active ]->ls->Rts( GCPP_OFF );
      break;
    case F10 :
      if( ActivePorts == 2 ) {
        VidSaveWindow( PortList[ Active ]->MainWindow );
        VidRestoreWindow( PortList[ !Active ]->MainWindow, 1 );
        VidSaveWindow( PortList[ Active ]->MessageWindow );
        VidRestoreWindow( PortList[ !Active ]->MessageWindow, 1 );
        Active = !Active;
      }
      break;
    case ESC    :
      VidSet80x25();
      VidTerminate( TRUE );
      if( ActivePorts == 2 ) {
        delete( PortList[ 0 ] );
        delete( PortList[ 1 ] );
      }
      else
        delete( PortList[ Active ] );
      exit( 0 );
      break;
    default :
      PortList[ Active ]->cp->PutChar( (unsigned char)c );
      break;
  }
  PortList[ Active ]->print_current_state();
}


//*
//*  void MiniTerm::print_current_state( void )
//*
//* ARGUMENTS
//*
//*  None.
//*
//* DESCRIPTION
//*
//* This routine is called at various times to print the current state
//* of FLOWDOS.  It dumps out some help info, then some status info.
//*
//* SIDE EFFECTS
//*
//*  None.
//*
//* RETURNS
//*
//*  None.
//*
//* AUTHOR
//*
//*  SM               March 20, 1991
//*
//* MODIFICATIONS
//*
//*
void MiniTerm::print_current_state( void )
{
  VidPrintf( MessageWindow, "\n\n" );
  VidPrintf( MessageWindow, "Cmds: I/G/X/R/+/-/ESC\n" );
  VidPrintf( MessageWindow, "TX/RX   = %d/%d\n", tx_count, rx_count );
  VidPrintf( MessageWindow, "Flow: " );
  if( FlowType == 1 )
    VidPrintf( MessageWindow, "XON " );
  if( FlowType == 2 )
    VidPrintf( MessageWindow, "RTS " );
  if( FlowType == 3 )
    VidPrintf( MessageWindow, "DTR" );
  VidPrintf( MessageWindow, "\n" );
  VidPrintf( MessageWindow, "%sRunning\n", Running ? "" : "Not " );
  print_packet_number();
}


//*
//*  void print_help( void )
//*
//* ARGUMENTS
//*
//*  None.
//*
//* DESCRIPTION
//*
//* Prints a little help info.
//*
//* SIDE EFFECTS
//*
//*  None.
//*
//* RETURNS
//*
//*  None.
//*
//* AUTHOR
//*
//*  SM               March 20, 1991
//*
//* MODIFICATIONS
//*
//*
void print_help( void )
{
  VidClearWindow( PortList[Active]->MainWindow );
  VidPrintf( PortList[Active]->MainWindow, "DOS Flow Control Test Program Commands:\n\n" );
  VidPrintf( PortList[Active]->MainWindow, " +/-    : In/Decrease transmit/receive ratio\n" );
  VidPrintf( PortList[Active]->MainWindow, " ALT_I  : Initialize communications buffers\n" );
  VidPrintf( PortList[Active]->MainWindow, " ALT_X  : Toggle XON/XOFF flow control\n" );
  VidPrintf( PortList[Active]->MainWindow, " ALT_R  : Toggle RTS/CTS flow control\n" );
  VidPrintf( PortList[Active]->MainWindow, " ALT_D  : Toggle DTR/DSR flow control\n" );
  VidPrintf( PortList[Active]->MainWindow, " ALT_B  : Cycle through the list of baud rates\n" );
  VidPrintf( PortList[Active]->MainWindow, " ALT_G  : Toggle running state (Go or Stop)\n" );
  VidPrintf( PortList[Active]->MainWindow, " F1     : Set DTR high\n" );
  VidPrintf( PortList[Active]->MainWindow, " F2     : Set DTR low\n" );
  VidPrintf( PortList[Active]->MainWindow, " F3     : Set RTS high\n" );
  VidPrintf( PortList[Active]->MainWindow, " F4     : Set RTS low\n" );
  VidPrintf( PortList[Active]->MainWindow, " F10    : Switch between ports\n" );
  VidPrintf( PortList[Active]->MainWindow, " ESC    : Exit program\n" );
}



//*
//*  void DebugStatusPuts( char//*buf )
//*
//* ARGUMENTS
//*
//*  The status stuff to print out.
//*
//* DESCRIPTION
//*
//* This is the routine passed to DumpPortStatus in order to print
//* the current state of the hardware and software.  It in turn dumps the
//* status info coming from the comm driver to a window so the user can
//* see it.
//*
//* SIDE EFFECTS
//*
//*  None.
//*
//* RETURNS
//*
//*  None.
//*
//* AUTHOR
//*
//*  SM               March 20, 1991
//*
//* MODIFICATIONS
//*
//*
void DebugStatusPuts( char *buf )
{
  static row = 0;

  if( *buf == '\0' )
    row = 0;
  else
    VidPokes( DebugWindow, row++, 0, buf );
}


//*
//*  void MiniTerm::print_packet_number( void )
//*
//* ARGUMENTS
//*
//* None.
//*
//* DESCRIPTION
//*
//* I don't even know why I put this in a separate function.  It doesn't
//* deserve a description.
//*
//* SIDE EFFECTS
//*
//*  None.
//*
//* RETURNS
//*
//*  None.
//*
//* AUTHOR
//*
//*  SM               March 20, 1991
//*
//* MODIFICATIONS
//*
//*
void MiniTerm::print_packet_number( void )
{
  if( this == PortList[ Active ] )
    VidPrintf( MessageWindow, "No.: %5u Errs: %5u\r", outbound_packet_number, error_count );
}

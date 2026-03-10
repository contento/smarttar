//*
//* VIDTERMD.CPP contains
//*
//*           TerminalProgram::TerminalProgram()
//*           TerminalProgram::~TerminalProgram()
//*           TerminalProgram::SetHandShake()
//*           TerminalProgram::SetPortData()
//*           TerminalProgram::FileTransfer()
//*           TerminalProgram::Redial()
//*           TerminalProgram::Dial()
//*           TerminalProgram::AnswerPhone()
//*           TerminalProgram::HangUp()
//*           TerminalProgram::DumpModemRegisters()
//*           TerminalProgram::InitializeModem()
//*           TerminalProgram::IdleWait()
//*           TerminalProgram::FlushTX()
//*           TerminalProgram::FlushRX()
//*           TerminalProgram::ToggleDTR()
//*           TerminalProgram::ToggleRTS()
//*           TerminalProgram::SendBreak()
//*           TerminalProgram::ToggleReading()
//*           TerminalProgram::ReadPort()
//*           TerminalProgram::WritePort()
//*           TerminalProgram::InstallModem()
//*           TerminalProgram::SendTestBuffer()
//*           DTPMonitor::UserMonitor()
//*           DTPMonitor::TransferMessage()
//*           DTPMonitor::TransferAbort()
//*           DTPModem::ModemPrinter()
//*           DOSTerminalProgram::DOSTerminalProgram()
//*           DOSTerminalProgram::SelectEmulation()
//*           DOSTerminalProgram::OpenPort()
//*           DOSTerminalProgram::PortSelection()
//*           DOSTerminalProgram::InitializeTransferMonitor()
//*           DOSTerminalProgram::StopTransferMonitor()
//*           DOSTerminalProgram::GetKeyIO()
//*           DOSTerminalProgram::KeyHit()
//*           DOSTerminalProgram::WriteStringBase()
//*           DOSTerminalProgram::WriteChar()
//*           DOSTerminalProgram::ErrorMessage()
//*           DOSTerminalProgram::DumpViewer()
//*           DOSTerminalProgram::DisplayParameters()
//*           DOSTerminalProgram::CheckParameters()
//*           DOSTerminalProgram::GetParameters()
//*           DisplayStatus()
//*           InitializeVideo()
//*           GoodBye()
//*           UpdateStatusLine()
//*           SystemCleanup()
//*           HandleDos()
//*           HandleUtility()
//*           HandlePhoneCommand()
//*           HandleCommand()
//*           main()
//*
//*
//* The Greenleaf Comm++ Library
//*
//* Copyright (C) 1989-95 Greenleaf Software Inc.  All Rights Reserved.
//*
//* MODIFICATIONS
//*
//*

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include "video.h"
#include "ibmkeys.h"
#include "compplib.hpp"
#include "compptrm.hpp"
#include "comppdsp.hpp"
#include "filexfer.hpp"

//*
//* Some defines
//*
#define NOINIT        0
#define TIME_INTERVAL 250
#define UP_ARROW      24
#define DOWN_ARROW    25
#define ZAUTODL1      '*'
#define ZAUTODL2      24
#define ZAUTODL3      'B'
#define ZAUTODL4      '0'
#define BPRESPND      ENQ
#define BPAUTO1       DLE
#define BPAUTO2       'B'

enum TerminalType { EMU_UNDEFINED, EMU_TELETYPE, EMU_ANSI, EMU_VT100, EMU_VT52 };
enum HandShakeType { HS_NONE, HS_XONXOFF, HS_RTSCTS, HS_DTRDSR };
enum TransferType { TP_ASCII, TP_KERMIT, TP_XMODEM, TP_XMODEM1K, TP_YMODEM, TP_ZMODEM, TP_XMODEM1KG, TP_YMODEMG, TP_CSB };

void DisplayStatus( char *data );
void UpdateStatusLine( void );

//****************************************************************************
//*
//*  Class TerminalProgram
//*
//*     This is the implementation for class TerminalProgram.  This class
//*     is platform independent.
//*
//****************************************************************************

class TerminalProgram {
  public:
    TerminalProgram( void );
    virtual ~TerminalProgram( void );

    virtual int SelectEmulation( void ) = 0;
    virtual int PortSelection( void ) = 0;
    virtual int OpenPort( void ) = 0;
    virtual void InitializeTransferMonitor( int sending ) = 0;
    virtual void StopTransferMonitor( void ) = 0;
    virtual unsigned GetKeyIO( void ) = 0;
    virtual int KeyHit( void ) = 0;
    virtual void WriteStringBase( char *message ) = 0;
    virtual void WriteChar( char c ) = 0;
    virtual void GetInputLine( char *header, char *buffer, int length ) = 0;
    virtual void ErrorMessage( char *message ) = 0;
    virtual void DumpViewer( void ) = 0;

    int SetHandShake( void );
    int SetPortData( void );
    void FileTransfer( int type, int sending );
    void HangUp( void );
    void AnswerPhone( void );
    void Dial( void );
    void Redial( void );
    void DumpModemRegisters( void );
    void InitializeModem( void );
    void IdleWait( long msecs );
    void FlushTX( void );
    void FlushRX( void );
    void ToggleDTR( void );
    void ToggleRTS( void );
    void SendBreak( void );
    void ToggleReading( void );
    int ReadPort( void );
    void WritePort( unsigned char c );
    void InstallModem( void );
    void SendTestBuffer( void );
    void WriteString( char *message, ... );

    GFComLink GCPP_DLL_FAR *CmLk;
    GFSerial GCPP_DLL_FAR *Port;
    GFDataFormat GCPP_DLL_FAR *DF;
    GFLineStatus GCPP_DLL_FAR *LS;
    GFModem GCPP_DLL_FAR *Modem;
    GFMonitor GCPP_DLL_FAR *Monitor;

    int OpenPortNumber;
    int PortNumber;
    long BaudRate;
    char Parity;
    int DataLength;
    int StopBits;
    HandShakeType HandShake;
    TerminalType UsingTerminalEmulation;
    TerminalType TerminalEmulation;
    int Reading;
    int LinkOpen;
    int InstallModemObject;
};

//* TerminalProgram::TerminalProgram( void )
//*
//* ARGUMENTS
//*   NONE
//*
//* DESCRIPTION
//*   This is the base constructor for the TerminalProgram class
//*
//* SIDE EFFECTS
//*   Creates a TerminalProgram object.
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
TerminalProgram::TerminalProgram( void )
{
  OpenPortNumber = -1;
  PortNumber = COM1;
  BaudRate = 19200L;
  Parity = 'N';
  DataLength = 8;
  StopBits = 1;
  HandShake = HS_NONE;
  TerminalEmulation = EMU_ANSI;
  UsingTerminalEmulation = EMU_UNDEFINED;
  Reading = 1;
  LinkOpen = FALSE;
  InstallModemObject = TRUE;
  CmLk = NULL;
  Port = NULL;
  Modem = NULL;
  Monitor = NULL;
  DF = NULL;
  LS = NULL;
}


//*
//* TerminalProgram::~TerminalProgram( void )
//*
//* ARGUMENTS
//*   NONE
//*
//* DESCRIPTION
//*   This is the destructor for the TerminalProgram class.  It will
//*   delete all the allocated objects in TerminalProgram.
//*
//* SIDE EFFECTS
//*   Destroys a TerminalProgram object.
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
TerminalProgram::~TerminalProgram( void )
{
  if( Modem )
    delete Modem;
  if( Monitor )
    delete Monitor;
}


//*
//* int TerminalProgram::SetHandShake( void )
//*
//* ARGUMENTS
//*   NONE
//*
//* DESCRIPTION
//*   This is a platform indepentent routine to set the handshake method
//*   for the program.
//*
//* SIDE EFFECTS
//*
//* RETURNS
//*   The result of the sethandshake function call.
//*
//* AUTHOR
//*   Patrick Little
//*
//* MODIFICATIONS
//*
//*
int TerminalProgram::SetHandShake( void )
{
  int result;

  if( LinkOpen ) {
    switch( HandShake ) {
      case HS_NONE:
        result = LS->SetHandShake( GCPP_NONE, GCPP_NONE ); break;
      case HS_XONXOFF:
        result = LS->SetHandShake( XONXOFF, XONXOFF ); break;
      case HS_RTSCTS:
        result = LS->SetHandShake( RTS, CTS ); break;
      case HS_DTRDSR:
        result = LS->SetHandShake( DTR, DSR ); break;
    }
  }
  return( result );
}


//*
//* int TerminalProgram::SetPortData( void )
//*
//* ARGUMENTS
//*   NONE
//*
//* DESCRIPTION
//*   This function will set the various port parameters for the
//*   open port.
//*
//* SIDE EFFECTS
//*
//* RETURNS
//*   The result of the setup function call.
//*
//* AUTHOR
//*   Patrick Little
//*
//* MODIFICATIONS
//*
//*
int TerminalProgram::SetPortData( void )
{
  if( LinkOpen )
    return( DF->SetUp( BaudRate, Parity, DataLength, StopBits ) );
  else
    return( GCPP_ERROR );
}


//*
//* int TerminalProgram::FileTransfer( int type, int sending, char *filename )
//*
//* ARGUMENTS
//*   int type - The type of transfer protocol to use.
//*   int sending - Flag to denote sending or receiving.
//*   char *filename - A pointer to the filename.
//*
//* DESCRIPTION
//*   This function will send or receive the files for the terminal
//*   program.
//*
//* SIDE EFFECTS
//*
//* RETURNS
//*   The result of the file transfer.
//*
//* AUTHOR
//*   Patrick Little
//*
//* MODIFICATIONS
//*
//*
void TerminalProgram::FileTransfer( int type, int sending )
{
  GFTransfer GCPP_DLL_FAR *Transfer;
  GFCompuServeBPlus GCPP_DLL_FAR *bplus;
  int result;
  char filename[45];
  char buffer[60];


  if( LinkOpen ) {
    //*
    //* Start the Transfer Monitor.
    //*
    InitializeTransferMonitor( sending );

    if ( type != TP_CSB ) {
      //*
      //* Get the file name
      //*
      if( sending || (!sending && type != TP_KERMIT && type != TP_YMODEM &&
          type != TP_YMODEMG  && type != TP_ZMODEM) ) {
        strcpy( buffer, "Name of File to " );
        if( sending )
          strcat( buffer, "Upload : " );
        else
          strcat( buffer, "Download : " );

        strcpy( filename, "" );

        GetInputLine( buffer, filename, 45 );

        if( strlen( filename ) == 0 ) {
          StopTransferMonitor();
          return;
        }
      }
      Port->RXFlush();
    }

    switch( type ) {
      case TP_ASCII :
        Transfer = new GFTranstxt( Port, Monitor, 1000 );
        break;
      case TP_CSB :
        bplus = new GFCompuServeBPlus( Port, Monitor );
        result = bplus->TransferFile();
        delete bplus;
        break;
      case TP_KERMIT :
        Transfer = new GFKermit( Port, Monitor );
        break;
      case TP_XMODEM :
        Transfer = new GFXmodem( Port, Monitor );
        break;
      case TP_XMODEM1K:
        Transfer = new GFXmodem1K( Port, Monitor );
        break;
      case TP_YMODEM :
        Transfer = new GFYmodem( Port, Monitor );
        break;
      case TP_ZMODEM :
        Transfer = new GFZmodem( Port, Monitor );
        break;
      case TP_XMODEM1KG :
        Transfer = new GFXmodem1KG( Port, Monitor );
        break;
      case TP_YMODEMG :
        Transfer = new GFYmodemG( Port, Monitor );
        break;
      default :
        return;
    }

    if( type != TP_CSB ) {
      if( sending )
        result = Transfer->SendFile( filename );
      else
        result = Transfer->ReceiveFile( filename );
      delete Transfer;
    }

    //*
    //* Exit and print out the results.
    //*
    StopTransferMonitor();

    //*
    //* Print out a result message
    //*
    if( result < GCPP_OK )
      WriteString( "Error return: %s\n", GetCommErrorName( result ) );
    else {
      strcpy( buffer, "Successful " );
      if( type == TP_CSB )
        strcat( buffer, "transfer is Completed.\n" );
      else {
        if( sending )
          strcat( buffer, "upload is Completed.\n" );
        else
          strcat( buffer, "download is Completed.\n" );
      }
      WriteString( buffer );
    }
  }
  return;
}


//*
//* void TerminalProgram::Redial( void )
//*
//* ARGUMENTS
//*   NONE
//*
//* DESCRIPTION
//*   This will redial the previously used phone number.
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
void TerminalProgram::Redial( void )
{
  if( LinkOpen ) {
    IdleWait( 800 );
    WriteString( "\nDialing..." );
    Modem->Redial();
  }
}


//*
//* void TerminalProgram::Dial( char *number )
//*
//* ARGUMENTS
//*   char *number - The phone number to dial.
//*
//* DESCRIPTION
//*   This function will dial the user supplied phone number.
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
void TerminalProgram::Dial( void )
{
  char number[30];

  if( LinkOpen ) {
    GetInputLine( "Phone Number: ", number, 29 );
    if( strlen( number ) != 0 ) {
      IdleWait( 800 );
      WriteString( "\nDialing..." );
      Modem->Dial( number );
    }
  }
}


//*
//* void TerminalProgram::AnswerPhone( void )
//*
//* ARGUMENTS
//*   NONE
//*
//* DESCRIPTION
//*   This function will wait in a loop until a ring is detected.
//*   It will then send an ATA to the modem to answer the phone.
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
void TerminalProgram::AnswerPhone( void )
{
  if( LinkOpen ) {
    WriteString( "\nPress ESC Key to Abort - " );
    WriteString( "Waiting For A Call" );
    for( ; ; ) {
      if( KeyHit () )
        if( GetKeyIO() == ESC )
          break;
      LS->Status();
      if( LS->Ring() ) {
        if( Modem->Write( "A" ) == GCPP_OK)
          return;
        WriteString( "\nError sending command to modem" );
        IdleWait( 100 );
        return;
      }
    }
    WriteString( "\nBreak due to user abort" );
    IdleWait( 100 );
  }
}


//*
//* void TerminalProgram::HangUp( void )
//*
//* ARGUMENTS
//*   NONE
//*
//* DESCRIPTION
//*   This function will attempt to hangup the modem.
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
void TerminalProgram::HangUp( void )
{
  if( LinkOpen ) {
    WriteString( "Disconnecting...\n" );
    Modem->HangUp();
  }
}


//*
//* void TerminalProgram::DumpModemRegisters( void )
//*
//* ARGUMENTS
//*   NONE
//*
//* DESCRIPTION
//*   This function will query the modem for the modem register values
//*   and print them to the screen.
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
void TerminalProgram::DumpModemRegisters( void )
{
  char buffer[80];
  int i;

  if( LinkOpen ) {
    for( i = 0 ; i < 17 ; i++ ) {
      sprintf( buffer, "Status register %d = %s\n", i, Modem->GetRegister( i ) );
      WriteString( buffer );
    }
  }
}


//*
//* void TerminalProgram::InitializeModem( void )
//*
//* ARGUMENTS
//*   NONE
//*
//* DESCRIPTION
//*   This function will send an initialization string to the
//*   modem.
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
void TerminalProgram::InitializeModem( void )
{
  if( LinkOpen ) {
    WriteString( "Initializing the modem...\n" );
    Modem->Initialize();
  }
}


//*
//* void TerminalProgram::IdleWait( long msecs )
//*
//* ARGUMENTS
//*   long msecs - The number of milliseconds to wait.
//*
//* DESCRIPTION
//*   This function allows us to wait a specifed amount of
//*   time while calling the system idle routine.
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
void TerminalProgram::IdleWait( long msecs )
{
  if( LinkOpen )
    UseSysTime( Port, msecs );
}


//*
//* void TerminalProgram::FlushTX( void )
//*
//* ARGUMENTS
//*   NONE
//*
//* DESCRIPTION
//*   This function will flush the TX buffer.
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
void TerminalProgram::FlushTX( void )
{
  if( LinkOpen )
    Port->TXFlush();
}


//*
//* void TerminalProgram::FlushRX( void )
//*
//* ARGUMENTS
//*   NONE
//*
//* DESCRIPTION
//*   This function will flush the RX buffer.
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
void TerminalProgram::FlushRX( void )
{
  if( LinkOpen )
    Port->RXFlush();
}


//*
//* void TerminalProgram::ToggleDTR( void )
//*
//* ARGUMENTS
//*   NONE
//*
//* DESCRIPTION
//*   Toggle the DTR line on or off.
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
void TerminalProgram::ToggleDTR( void )
{
  if( LinkOpen ) {
    LS->Status();
    if( LS->Dtr() ) {
      LS->Dtr( GCPP_OFF );
      WriteString( "\nSetting DTR: 0\n" );
    } else {
      LS->Dtr( GCPP_ON );
      WriteString( "\nSetting DTR: 1\n" );
    }
  }
}


//*
//* void TerminalProgram::ToggleRTS( void )
//*
//* ARGUMENTS
//*   NONE
//*
//* DESCRIPTION
//*   Toggle the RTS line on or off.
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
void TerminalProgram::ToggleRTS( void )
{
  if( LinkOpen ) {
    LS->Status();
    if( LS->Rts() ) {
      LS->Rts( GCPP_OFF );
      WriteString( "\nSetting RTS: 0\n" );
    } else {
      LS->Rts( GCPP_ON );
      WriteString( "\nSetting RTS: 1\n" );
    }
  }
}


//*
//* void TerminalProgram::SendBreak( void )
//*
//* ARGUMENTS
//*   NONE
//*
//* DESCRIPTION
//*   Send a line break.
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
void TerminalProgram::SendBreak( void )
{
  if( LinkOpen )
    LS->SendBreak();
}


//*
//* void TerminalProgram::ToggleReading( void )
//*
//* ARGUMENTS
//*   NONE
//*
//* DESCRIPTION
//*   This function will toggle the reading flag on or off.
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
void TerminalProgram::ToggleReading( void )
{
  if( LinkOpen ) {
    Reading = !Reading;
    if( Reading )
      WriteString( "\nReading = 1\n" );
    else
      WriteString( "\nReading = 0\n" );
  }
}


//*
//* int TerminalProgram::ReadPort( void )
//*
//* ARGUMENTS
//*   NONE
//*
//* DESCRIPTION
//*   This function will read the port and return a success or
//*   failure value.  Since we are using terminal emulation
//*   we don't have to do anything with the characters ourselves.
//*
//* SIDE EFFECTS
//*
//* RETURNS
//*   The charater read or GCPP_TIMEOUT.
//*
//* AUTHOR
//*   Patrick Little
//*
//* MODIFICATIONS
//*
//*
int TerminalProgram::ReadPort( void )
{
  int dbyte;

  if( LinkOpen ) {
    if( Reading ) {
      dbyte = Port->GetChar();
      if( dbyte == BPRESPND ) {
        GFCompuServeBPlus::Respond( Port );
        dbyte = Port->GetChar( 500 );
        if ( dbyte == BPAUTO1 ) {         //* Check for <DLE>
          dbyte = Port->GetChar( 55 );
          if ( dbyte == BPAUTO2 ) {       //* Check for 'B'
            FileTransfer( TP_CSB, 0 );    //* Start up a CompuServe B+ transfer
            return( GCPP_TIMEOUT );       //* Go ahead and return
          }
        }
      }
      if( dbyte == ZAUTODL1 ) {         //* Check for a ZPAD
        IdleWait( 40 );                 //* Wait for the other characters
        dbyte = Port->GetChar();
        if( dbyte == ZAUTODL1 ) {       //* Check for a ZPAD
          dbyte = Port->GetChar();
          if( dbyte == ZAUTODL2 ) {     //* Check for a ZDLE
            dbyte = Port->GetChar();
            if( dbyte == ZAUTODL3 ) {   //* Check for a ZHEX
              dbyte = Port->GetChar();
              if( dbyte == ZAUTODL4 ) { //* Check for a ZRQINIT
                dbyte = Port->GetChar();
                if( dbyte == ZAUTODL4 ) {
                  FileTransfer( TP_ZMODEM, 0 );
                }
              }
            }
          }
        }
      }
      return( dbyte );
    }
    else
      return( GCPP_TIMEOUT );
  }
  else
    return( GCPP_ERROR );
}


//*
//* void TerminalProgram::WritePort( unsigned char c )
//*
//* ARGUMENTS
//*   NONE
//*
//* DESCRIPTION
//*   This function will write a character to the port.
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
void TerminalProgram::WritePort( unsigned char c )
{
  if( LinkOpen )
    Port->PutChar( c );
}


//*
//* void TerminalProgram::InstallModem( void )
//*
//* ARGUMENTS
//*   NONE
//*
//* DESCRIPTION
//*   This function will attach the port object to the modem object.
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
void TerminalProgram::InstallModem( void )
{
  if( LinkOpen )
    if( InstallModemObject )
      if( Modem ) {
        Modem->Install( Port );
        InstallModemObject = FALSE;
      }
}


//*
//* void TerminalProgram::SendTestBuffer( void )
//*
//* ARGUMENTS
//*   NONE
//*
//* DESCRIPTION
//*   This function will write a test string out the port.
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
void TerminalProgram::SendTestBuffer( void )
{
  char *buffer = "echo Hello, this is a test of windows write buffer!\r";
  int status;

  status = Port->PutBuffer( (unsigned char *)buffer, strlen( buffer ) );
  if( status < GCPP_OK )
    WriteString( "\nError writing buffer: %d\n", status );
}

//*
//* void TerminalProgram::WriteString( char *message, ... )
//*
//* ARGUMENTS
//*   char *message - The string to write to the main window
//*
//* DESCRIPTION
//*   This member function encapsulates the platform dependent nature
//*   of dealing with the screen.
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
void TerminalProgram::WriteString( char *message, ... )
{
  va_list args;
  char buffer[512];

  va_start( args, message );
  if( vsprintf( (char *)buffer, message, args ) == EOF )
    return;
  va_end( args );
  WriteStringBase( buffer );
}

//*****************************************************************************
//*
//*   class DOSTerminalProgram
//*
//*       This class contains the platform specific code that is needed
//*       to operate in the different environments.
//*
//****************************************************************************

class DOSTerminalProgram : public TerminalProgram
{
  public:
    DOSTerminalProgram( void );
    ~DOSTerminalProgram( void );

    int SelectEmulation( void );
    int OpenPort( void );
    int PortSelection( void );
    void InitializeTransferMonitor( int sending );
    void StopTransferMonitor( void );
    unsigned GetKeyIO( void );
    int KeyHit( void );
    void WriteStringBase( char *message );
    void WriteChar( char c);
    void GetInputLine( char *header, char *buffer, int length );
    void ErrorMessage( char *message );
    void DumpViewer( void );

    void DisplayParameters( void );
    int CheckParameters( void );
    void GetParameters( void );

    GFScreenVid *MainWindow;
};

class DTPMonitor : public GFMonitor {
  public:
    int UserMonitor( int code );
    void TransferMessage( char *message );
    int TransferAbort( void );
};

class DTPModem : public GFModem {
  public:
    void ModemPrinter( char c );
};


//****************************************************************************
//* Global Data
//****************************************************************************
int SplitScreen = FALSE;
unsigned char MenuColumn;
VID_WINDOW *VIDTransferBox;
VID_WINDOW *VIDMessageBox;
VID_WINDOW *VIDStatusWindow;
VID_WINDOW *VIDDebugWindow;
DOSTerminalProgram *DTP;

char *GetPortNumber[] = {
    "1: COM1", "2: COM2", "3: COM3", "4: COM4", "5: COM5",
    "6: COM6", "7: COM7", "8: COM8", "" };

char *GetPortBaudRate[] = {
    "50", "75", "110", "134", "150", "300", "600", "1200", "2400",
    "4800", "9600", "19200", "38.4K", "57.6K", "115.2K", "" };

char *GetPortWordLength[] = {
    "8", "7", "6", "5", "" };

char *GetPortParity[] = {
    "None", "Odd", "Even", "Stick Mark", "Stick Space", "" };

char *GetPortStopBits[] = {
    "1", "2", "" };

char *GetHandshaking[] = {
    "None", "XON/XOFF", "RTS/CTS", "DTR/DSR", "" };

char *GetTerminalType[] = {
    "None", "ANSI", "VT100", "VT52", "" };

struct {
  char fname[21];
  int file_count;
  unsigned long packno;
  unsigned long nbytes;
  int block_size;
  unsigned long file_length;
  int errors;
  int transfer_type;
  int check;
} transfer_data;

//****************************************************************************
//* Interface member functions for library systems.
//****************************************************************************

//*
//* int DTPMonitor::UserMonitor( int code )
//*
//* ARGUMENTS
//*   int code - An error code from the file transfer system.
//*
//* DESCRIPTION
//*   This is the user_monitor for the file transfer system.  We
//*   have inherited and defined this virtual function in the
//*   DTPMonitor class.
//*
//* SIDE EFFECTS
//*
//* RETURNS
//*   Any error code.
//*
//* AUTHOR
//*   Patrick Little
//*
//* MODIFICATIONS
//*
//*
int DTPMonitor::UserMonitor( int code )
{
  static char *ttype[] = { "XMODEM", "XMODEM 1K", "YMODEM", "XMODEM 1KG",
                           "YMODEM G", "KERMIT", "ASCII", "ZMODEM", "CIS B+" };
  //*
  //* Update the status window and status line.
  //*
  DTP->DumpViewer();
  UpdateStatusLine();

  //*
  //* Update any data that has changed since the last call to this
  //* function.
  //*
  if( strcmp( transfer_data.fname, fname ) ) {
    if( fname != NULL ) {
      if( strlen( fname ) < 20 ) {
        VidPokef( VIDTransferBox, 0, 17, "%-20.20s", fname );
        strcpy( transfer_data.fname, fname );
      }
    }
    else
      VidPokef( VIDTransferBox, 0, 17, "%-20.20s", "<none>" );
  }

  if( transfer_data.file_count != file_count )
    VidPokef( VIDTransferBox, 1, 17, "%-7d", file_count );

  if( transfer_data.packno != packno )
    VidPokef( VIDTransferBox, 2, 17, "%-7ld", packno );

  if( transfer_data.nbytes != nbytes )
    VidPokef( VIDTransferBox, 3, 17, "%-7ld", nbytes );

  if( transfer_data.block_size != block_size )
    VidPokef( VIDTransferBox, 4, 17, "%-4d", block_size );

  if( transfer_data.file_length != file_length )
    VidPokef( VIDTransferBox, 5, 17, "%-7ld", file_length );

  if( transfer_data.errors != errors )
    VidPokef( VIDTransferBox, 6, 17, "%-5d", errors );

  if( transfer_data.transfer_type != transfer_type )
    VidPokef( VIDTransferBox, 7, 17, "%-10s", ttype[ transfer_type ] );

  if( transfer_data.check != check )
    VidPokef( VIDTransferBox, 8, 17, "%-15s", etype[ check ] );

  //*
  //* Update all data so that we can test it next time around.
  //*
  transfer_data.file_count = file_count;
  transfer_data.packno = packno;
  transfer_data.nbytes = nbytes;
  transfer_data.block_size = block_size;
  transfer_data.errors = errors;
  transfer_data.file_length = file_length;
  transfer_data.transfer_type = transfer_type;
  transfer_data.check = check;
  return( code );
}


//*
//* void DTPMonitor::TransferMessage( char *message )
//*
//* ARGUMENTS
//*   char *message - The message to print to a window
//*
//* DESCRIPTION
//*   This function will allow the user to have output of the interal
//*   workings of the file transfer system.  At various points in the
//*   protocol we will call with descriptive strings.
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
void DTPMonitor::TransferMessage( char *message )
{
  DTP->DumpViewer();
  UpdateStatusLine();

  VidPrintf( VIDMessageBox, "%s\n", message );
}


//*
//* int DTPMonitor::TransferAbort( void )
//*
//* ARGUMENTS
//*   NONE
//*
//* DESCRIPTION
//*   This function allows use to monitor for an abort key pressed by
//*   the user.
//*
//* SIDE EFFECTS
//*
//* RETURNS
//*   Any error code.
//*
//* AUTHOR
//*   Patrick Little
//*
//* MODIFICATIONS
//*
//*
int DTPMonitor::TransferAbort( void )
{
  DTP->DumpViewer();
  UpdateStatusLine();

  if( gfkbhit() )
    if( getkey() == ESC )
      return( GCPP_ERR_U );
  return( GCPP_OK );
}


//*
//* void DTPModem::ModemPrinter( char c )
//*
//* ARGUMENTS
//*   char c - The character read from the modem.  This is what we
//*            are going to print somewhere.
//*
//* DESCRIPTION
//*   This modem_printer for the GFModem class.  We have inherited and
//*   defined this virtual function for the DTPModem class.
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
void DTPModem::ModemPrinter( char c )
{
  DTP->WriteChar( c );
}



//****************************************************************************
//* class DOSTerminalProgram member function definitions
//****************************************************************************

//*
//* DOSTerminalProgram::DOSTerminalProgram( void )
//*
//* ARGUMENTS
//*   NONE
//*
//* DESCRIPTION
//*   This is the constructor for the DOSTerminalProgram class.  It just
//*   sets up the MainWindow member.
//*
//* SIDE EFFECTS
//*   Creates a DOSTerminalProgram object.
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
DOSTerminalProgram::DOSTerminalProgram( void )
{
  Modem = new DTPModem();
  Monitor = new DTPMonitor;
  MainWindow = new GFScreenVid( NOINIT, NOINIT );
  MainWindow->window = VidDefineWindow( 0, 0, 23, 79, 1 );
  MainWindow->MaxSize( MainWindow->window->rows, MainWindow->window->cols );
  MainWindow->Color( 15, 0 );
  MainWindow->Clear();
}


//*
//* DOSTerminalProgram::~DOSTerminalProgram( void )
//*
//* ARGUMENTS
//*   NONE
//*
//* DESCRIPTION
//*   This is the destructor for the DOSTerminalProgram class.
//*
//* SIDE EFFECTS
//*   NONE
//*
//* RETURNS
//*   NONE
//*
//* AUTHOR
//*   Patrick Little
//*
//* MODIFICATIONS
//*
DOSTerminalProgram::~DOSTerminalProgram( void )
{
  if( MainWindow )
    delete MainWindow;
  if( Port )
    delete Port;
  if( CmLk )
    delete CmLk;
}


//*
//* int DOSTerminalProgram::SelectEmulation( void )
//*
//* ARGUMENTS
//*   NONE
//*
//* DESCRIPTION
//*   This member function allows the selection of a terminal
//*   emulation type.
//*
//* SIDE EFFECTS
//*
//* RETURNS
//*   GCPP_OK or GCPP_ERROR
//*
//* AUTHOR
//*   Patrick Little
//*
//* MODIFICATIONS
//*
//*
int DOSTerminalProgram::SelectEmulation( void )
{
  unsigned short error;

  //*
  //* Select the type of emulation the user desires.
  //*
  if( UsingTerminalEmulation != TerminalEmulation ) {
    if( Port )
      delete Port;
    switch( TerminalEmulation ) {
      default:
      case EMU_TELETYPE:
        Port = new GFTeletype( MainWindow, CmLk );
        break;
      case EMU_ANSI:
        Port = new GFAnsiTerminal( MainWindow, CmLk );
        break;
      case EMU_VT100:
        Port = new GFVt100( MainWindow, CmLk );
        break;
      case EMU_VT52:
        Port = new GFVt52( MainWindow, CmLk );
        break;
    }

    if( Port == NULL ) {
      ErrorMessage( "Could not allocate enough memory" );
      return( GCPP_ERROR );
    } else {
      if( Port->GetCommError( error ) < GCPP_OK ) {
        ErrorMessage( "Failed to open the port" );
        return( GCPP_ERROR );
      }
    }
    UsingTerminalEmulation = TerminalEmulation;
  }
  return( GCPP_OK );
}


//*
//* int DOSTerminalProgram::OpenPort( void )
//*
//* ARGUMENTS
//*   NONE
//*
//* DESCRIPTION
//*   This member function is responsible for making sure the port is
//*   opened.
//*
//* SIDE EFFECTS
//*
//* RETURNS
//*   GCPP_OK or GCPP_ERROR
//*
//* AUTHOR
//*   Patrick Little
//*
//* MODIFICATIONS
//*
//*
int DOSTerminalProgram::OpenPort( void )
{
  DisplayParameters();
  for( ; ; ) {
    if( !CheckParameters() )
      return( GCPP_ERROR );
    if( PortSelection() == GCPP_OK)
      break;
  }
  return( GCPP_OK );
}


//*
//* int DOSTerminalProgram::PortSelection( void )
//*
//* ARGUMENTS
//*   NONE
//*
//* DESCRIPTION
//*   This member function allows the port selection and
//*   configuration.
//*
//* SIDE EFFECTS
//*
//* RETURNS
//*   GCPP_OK or GCPP_ERROR
//*
//* AUTHOR
//*   Patrick Little
//*
//* MODIFICATIONS
//*
//*
int DOSTerminalProgram::PortSelection( void )
{
  if( OpenPortNumber != PortNumber ) {
    if( Port != NULL ) {
      delete Port;
      Port = NULL;
    }
    if( CmLk != NULL )
      delete CmLk;
    LinkOpen = FALSE;
    InstallModemObject = TRUE;
    UsingTerminalEmulation = EMU_UNDEFINED;
    if( (CmLk = new GFI8250( PortNumber )) == NULL )
      return( GCPP_ERROR );
    LinkOpen = TRUE;
  }
  if( SelectEmulation() < GCPP_OK )
    return( GCPP_ERROR );
  OpenPortNumber = PortNumber;
  DF = Port->GetDataFormat();
  LS = Port->GetLineStatus();

  if( SetPortData() < GCPP_OK ) {
    ErrorMessage( "Failed to set new port parameters." );
    return( GCPP_ERROR );
  }
  if( SetHandShake() < GCPP_OK ) {
    ErrorMessage( "Failed to set handshake method" );
    return( GCPP_ERROR );
  }
  InstallModem();
  return( GCPP_OK );
}


//*
//* void DOSTerminalProgram::InitializeTransferMonitor( int sending )
//*
//* ARGUMENTS
//*   int sending - Transfer directin flag
//*
//* DESCRIPTION
//*   This member function is responsible for the initialization of the
//*   transfer monitor and associated window.
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
void DOSTerminalProgram::InitializeTransferMonitor( int sending )
{
  //*
  //* Save off the main window
  VidSaveWindow( MainWindow->window );

  //*
  //* Open the transfer and message windows.
  //*
  VIDTransferBox = VidDefineWindow( 1, 1, 9, 38, 1 );
  VidDrawBorder( VIDTransferBox );
  VidClearWindow( VIDTransferBox );

  VIDMessageBox = VidDefineWindow( 12, 25, 22, 78, 1 );
  VidDrawBorder( VIDMessageBox );
  VidClearWindow( VIDMessageBox );

  //*
  //* Initialize window and monitor elements.
  //*
  if( sending )
    VidPokes( VIDTransferBox, 0, 1, "  Sending file:" );
  else
    VidPokes( VIDTransferBox, 0, 1, "Receiving file:" );
  VidPokes( VIDTransferBox, 1, 1, "    File count:" );
  VidPokes( VIDTransferBox, 2, 1, "  Block number:" );
  VidPokes( VIDTransferBox, 3, 1, "    Byte total:" );
  VidPokes( VIDTransferBox, 4, 1, "    Block size:" );
  VidPokes( VIDTransferBox, 5, 1, "     File size:" );
  VidPokes( VIDTransferBox, 6, 1, "   Error count:" );
  VidPokes( VIDTransferBox, 7, 1, " Transfer type:" );
  VidPokes( VIDTransferBox, 8, 1, "   Block check:" );

  transfer_data.file_count = -1;
  transfer_data.packno = (unsigned long) -1;
  transfer_data.nbytes = (unsigned long) -1;
  transfer_data.block_size = -1;
  transfer_data.errors = -1;
  transfer_data.file_length = (unsigned long) -1;
  transfer_data.transfer_type = -1;
  transfer_data.check = -1;
}


//*
//* void DOSTerminalProgram::StopTransferMonitor( void )
//*
//* ARGUMENTS
//*   NONE
//*
//* DESCRIPTION
//*   This funciton is responsible for the closing of the
//*   transfer monitor system.
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
void DOSTerminalProgram::StopTransferMonitor( void )
{
  VidClearWindow( MainWindow->window );
  VidRestoreWindow( MainWindow->window, 1 );
  free( VIDTransferBox );
  free( VIDMessageBox );
}


//*
//* unsigned DOSTerminalProgram::GetKeyIO( void )
//*
//* ARGUMENTS
//*   NONE
//*
//* DESCRIPTION
//*   This member function is used to incapsulate the platform dependent
//*   nature of dealing with the keyboard.  It just get a key from the
//*   key board.
//*
//* SIDE EFFECTS
//*
//* RETURNS
//*   The current keystroke
//*
//* AUTHOR
//*   Patrick Little
//*
//* MODIFICATIONS
//*
//*
unsigned DOSTerminalProgram::GetKeyIO( void )
{
  return( getkey() );
}


//*
//* int DOSTerminalProgram::KeyHit( void )
//*
//* ARGUMENTS
//*   NONE
//*
//* DESCRIPTION
//*   This member function encapsulates the platform dependent manner
//*   of determing if there is a key stroke waiting to be retrived.
//*   This version just calls gfkbhit to check.
//*
//* SIDE EFFECTS
//*
//* RETURNS
//*   TRUE or FALSE
//*
//* AUTHOR
//*   Patrick Little
//*
//* MODIFICATIONS
//*
//*
int DOSTerminalProgram::KeyHit( void )
{
  return( gfkbhit() );
}


//*
//* void DOSTerminalProgram::WriteStringBase( char *message )
//*
//* ARGUMENTS
//*   char *message - The string to write to the main window
//*
//* DESCRIPTION
//*   This member function encapsulates the platform dependent nature
//*   of dealing with the screen.
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
void DOSTerminalProgram::WriteStringBase( char *message )
{
  MainWindow->Printf( message );
}


//*
//* void DOSTerminalProgram::WriteChar( char c )
//*
//* ARGUMENTS
//*   char c - The character to write to the screen.
//*
//* DESCRIPTION
//*   This member function encapsulates the platform dependent nature
//*   of dealing with the screen.
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
void DOSTerminalProgram::WriteChar( char c )
{
  MainWindow->PutChar( c );
}


//*
//* void DOSTerminalProgram::ErrorMessage( char *message )
//*
//* ARGUMENTS
//*   char *message - The error message to be displayed to the user.
//*
//* DESCRIPTION
//*   This function is responsible for displaying an error message to
//*   the user.  Since this function is local to the DOSTerminalProgram
///*  class it is able to easily deal with screen IO.
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
void DOSTerminalProgram::ErrorMessage( char *message )
{
  char *prompt1 = "Error detected";
  char *prompt2 = "Hit any key to continue or Q to exit";

  while( gfkbhit() )
    getkey();
  unsigned char col = (unsigned char)((PhysicalWindow->cols - 38) / 2);
  VID_WINDOW *temp_window = VidDefineWindow( 6, col, 14,
                                 (unsigned char) ( col + 37 ), 1 );
  if( temp_window != NULL ) {
    VidSaveWindow( temp_window );
    VidDrawBorder( temp_window );
    VidClearWindow( temp_window );
    VidPokes( temp_window, 1, ( 38 - strlen( prompt1 ) ) / 2, prompt1 );
    VidPokes( temp_window, 3, ( 38 - strlen( message ) ) / 2, message );
    VidPokes( temp_window, 7, ( 38 - strlen( prompt2 ) ) / 2, prompt2 );
    int c = getkey();
    if( toupper( c ) == 'Q' ) {
        DTP->DOSTerminalProgram::~DOSTerminalProgram();
        delete MainWindow;
        exit( -1 );
    }
    VidRestoreWindow( temp_window, 1 );
    free( temp_window );
  }
}


//*
//* void DOSTerminalProgram::DumpViewer( void )
//*
//* ARGUMENTS
//*   NONE
//*
//* DESCRIPTION
//*   This member funtion does the printing of the status to
//*   the debug window.
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
void DOSTerminalProgram::DumpViewer( void )
{
  Port->ViewPortStatus( DisplayStatus );
}


//*
//* void DOSTerminalProgram::DisplayParameters( void )
//*
//* ARGUMENTS
//*   NONE
//*
//* DESCRIPTION
//*   This function is responsible for the initial painting of the
//*   status window and port settings.
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
void DOSTerminalProgram::DisplayParameters( void )
{
  VidSetAttribute( VIDStatusWindow, VID_REVERSE );
  VidClearWindow( VIDStatusWindow );
  VidGoto( VIDStatusWindow, 0, 0 );
  VidPrintf( VIDStatusWindow, "COM%1d:%5ld,%c,%1d,%1d", PortNumber+1,
                              BaudRate, Parity, DataLength, StopBits );
  VidPokec( VIDStatusWindow, 0, 18, 179 );
  switch( HandShake ) {
    case HS_NONE:
      VidPokes( VIDStatusWindow, 0, 20, "NONE" ); break;
    case HS_XONXOFF:
      VidPokes( VIDStatusWindow, 0, 20, "XON/XOFF" ); break;
    case HS_RTSCTS:
      VidPokes( VIDStatusWindow, 0, 20, "RTS/CTS"); break;
    case HS_DTRDSR:
      VidPokes( VIDStatusWindow, 0, 20, "DTR/DSR" ); break;
  }
  VidPokec( VIDStatusWindow, 0, 38, 179 );
  VidPokes( VIDStatusWindow, 0, 40, "ALT-M for menu" );
  VidPokec( VIDStatusWindow, 0, 55, 179 );
  VidPokes( VIDStatusWindow, 0, 57, "DSR:  CTS:  RI:  CD: " );
}


//*
//* int DOSTerminalProgram::CheckParameters( void )
//*
//* ARGUMENTS
//*   NONE
//*
//* DESCRIPTION
//*   This function will contiune to loop until the user selects the
//*   abort option or accepts the current settings.
//*
//* SIDE EFFECTS
//*
//* RETURNS
//*   1 or 0
//*
//* AUTHOR
//*   Patrick Little
//*
//* MODIFICATIONS
//*
//*
int DOSTerminalProgram::CheckParameters( void )
{
  static char *menu[] = {
    "Accept current COM parameters", "Change current COM parameters",
    "Escape to DOS", "" };

  for( ; ; ) {
    DisplayParameters();
    int result = VidMenu( 5, MenuColumn, menu );
    if ( result == 0 )
      return( 1 );
    if ( result == 2 )
      return( 0 );
    GetParameters();
  }
}


//*
//* void DOSTerminalProgram::GetParameters( void )
//*
//* ARGUMENTS
//*   NONE
//*
//* DESCRIPTION
//*   This function allows the user to change the port settings.
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
void DOSTerminalProgram::GetParameters( void )
{
  static char *Menu[] = {
    "Port", "Baud Rate", "Word length", "Parity", "Stop Bits",
    "Handshaking", "Terminal Emulation", "" };

  for( ; ; ) {
    DisplayParameters();
    switch( VidMenu( 5, MenuColumn, Menu ) ) {
      case 0 :
        if( !IsMicroChannelBus() )
          GetPortNumber[ 4 ] = "";
        switch( VidMenu( 5, MenuColumn, GetPortNumber ) ) {
          case 0:  PortNumber = COM1;  break;
          case 1:  PortNumber = COM2;  break;
          case 2:  PortNumber = COM3;  break;
          case 3:  PortNumber = COM4;  break;
          case 4:  PortNumber = COM5;  break;
          case 5:  PortNumber = COM6;  break;
          case 6:  PortNumber = COM7;  break;
          case 7:  PortNumber = COM8;  break;
        }
        break;

      case 1 :
        switch( VidMenu( 5, MenuColumn, GetPortBaudRate ) ) {
          case 0:  BaudRate = 50L;       break;
          case 1:  BaudRate = 75L;       break;
          case 2:  BaudRate = 110L;      break;
          case 3:  BaudRate = 134L;      break;
          case 4:  BaudRate = 150L;      break;
          case 5:  BaudRate = 300L;      break;
          case 6:  BaudRate = 600L;      break;
          case 7:  BaudRate = 1200L;     break;
          case 8:  BaudRate = 2400L;     break;
          case 9:  BaudRate = 4800L;     break;
          case 10: BaudRate = 9600L;     break;
          case 11: BaudRate = 19200L;    break;
          case 12: BaudRate = 38400L;    break;
          case 13: BaudRate = 57600L;    break;
          case 14: BaudRate = 115200L;   break;
        }
        break;

      case 2 :
        switch ( VidMenu( 5, MenuColumn, GetPortWordLength ) ) {
          case 0:  DataLength = 8;  break;
          case 1:  DataLength = 7;  break;
          case 2:  DataLength = 6;  break;
          case 3:  DataLength = 5;  break;
        }
        break;

      case 3 :
        switch ( VidMenu( 5, MenuColumn, GetPortParity ) ) {
          case 0:  Parity = 'N';        break;
          case 1:  Parity = 'O';        break;
          case 2:  Parity = 'E';        break;
          case 3:  Parity = 'M';        break;
          case 4:  Parity = 'S';        break;
        }
        break;

      case 4 :
        switch ( VidMenu( 5, MenuColumn, GetPortStopBits ) ) {
          case 0:   StopBits = 1;  break;
          case 1:   StopBits = 2;  break;
        }
        break;

      case 5 :
        switch( VidMenu( 5, MenuColumn, GetHandshaking ) ) {
          case 0 : HandShake = HS_NONE; break;
          case 1 : HandShake = HS_XONXOFF; break;
          case 2 : HandShake = HS_RTSCTS; break;
          case 3 : HandShake = HS_DTRDSR; break;
        }
        break;
      case 6 :
        switch( VidMenu( 5, MenuColumn, GetTerminalType ) ) {
          case 0 : TerminalEmulation = EMU_TELETYPE; break;
          case 1 : TerminalEmulation = EMU_ANSI; break;
          case 2 : TerminalEmulation = EMU_VT100; break;
          case 3 : TerminalEmulation = EMU_VT52; break;
        }
        break;
      case -1 :
        return;
    }
  }
}


void DOSTerminalProgram::GetInputLine( char *header, char *buffer, int length )
{
  VidGetLine( header, buffer, length );
}

//****************************************************************************
//* Function definitions for VIDTERMD
//****************************************************************************

//*
//* void DisplayStatus( char *data )
//*
//* ARGUMENTS
//*   char *data - The data string to print.
//*
//* DESCRIPTION
//*   This is the platform dependent function to write to the debug
//*   data to the screen.
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
void DisplayStatus( char *data )
{
  static row = 0;

  if( VIDDebugWindow ) {
    if( *data == '\0' )
      row = 0;
    else
      VidPokes( VIDDebugWindow, row++, 0, data );
  }
}


//*
//* void InitializeVideo( int argc, char *argv[] )
//*
//* ARGUMENTS
//*   int argc - The number of arguments on the command line.
//*   char *argv[] - Char pointer array to the arguments.
//*
//* DESCRIPTION
//*   This function is responsible for the command line parsing and
//*   video setup.
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
void InitializeVideo( int argc, char *argv[] )
{
  VIDTYPE monitor_type = VidInitialize( VID_NORMAL, TRUE );

  VIDDebugWindow = NULL;
  MenuColumn = 25;
  VIDStatusWindow = VidDefineWindow( 24, 0, 24, 79, 0 );

  //*
  //* This is all to handle the debug window.
  //*
  if( argc > 1 ) {
    if( strstr( argv[1], "SPLIT" ) || strstr( argv[1], "split" ) ) {
      //*
      //* Setup for a split screen debug window.
      //*
      SplitScreen = VidSet80x43();
      if( SplitScreen )
        VIDDebugWindow = VidDefineWindow( 25, 0, 42, 79, 0 );
    } else if( strstr( argv[1], "PAGE" ) || strstr( argv[1], "page" ) ) {
      //*
      //* Setup for a multi page debug session.
      //*
      if( monitor_type != VID_MDA ) {
        VIDDebugWindow = VidDefineWindow( 0, 0, 17, 79, 0 );
        if( VIDDebugWindow )
          VidSetWindowPage( VIDDebugWindow, 1 );
        VidSelectFastDrivers();
      }
    } else if( strstr( argv[1], "MONO" ) || strstr( argv[1], "mono" ) ) {
      //*
      //* Setup for a mono, color debug session.
      //*
      if( monitor_type != VID_MDA ) {
        VIDDebugWindow = VidDefineWindow( 0, 0, 17, 79, 0 );
#if !defined( GCPP_X32 ) && !defined( GCPP_WIN32 ) && !defined( GCPP_OS2 )
        if( VIDDebugWindow )
          VIDDebugWindow->address = (unsigned int far *) 0xb0000000L;
#endif
      }
    }
    if( VIDDebugWindow )
      VidClearWindow( VIDDebugWindow );
  }
}


//*
//* int GoodBye( void )
//*
//* ARGUMENTS
//*   NONE
//*
//* DESCRIPTION
//*   This function handles exiting the program.  We just ask for
//*   an exit confirmation when the line is connected.
//*
//* SIDE EFFECTS
//*
//* RETURNS
//*
//* AUTHOR
//*   Patrick Little
//*
//* MODIFICATIONS
//*
//*
int GoodBye( void )
{
  DTP->MainWindow->Printf( "Confirm Exit (Y/N)? " );
  for( ;; ) {
    switch( getkey() ) {
      case 'Y' :
      case 'y' :
        DTP->HangUp();
        return( TRUE );
      case 'N' :
      case 'n' :
        return( FALSE );
      default :
        break;
    }
  }
#ifdef GCPP_MICROSOFT
  return( FALSE );
#endif
}


//*
//* void UpdateStatusLine( void )
//*
//* ARGUMENTS
//*   NONE
//*
//* DESCRIPTION
//*   This function will update the line status with the current
//*   line settings.
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
void UpdateStatusLine( void )
{
  DTP->LS->Status();
  if( DTP->LS->Dsr() )
    VidPokec( VIDStatusWindow, 0, 61, UP_ARROW );
  else
    VidPokec( VIDStatusWindow, 0, 61, DOWN_ARROW );

  if( DTP->LS->Cts() )
    VidPokec( VIDStatusWindow, 0, 67, UP_ARROW );
  else
    VidPokec( VIDStatusWindow, 0, 67, DOWN_ARROW );

  if( DTP->LS->Ring() )
    VidPokec( VIDStatusWindow, 0, 72, UP_ARROW );
  else
    VidPokec( VIDStatusWindow, 0, 72, DOWN_ARROW );

  if( DTP->LS->Carrier() )
    VidPokec( VIDStatusWindow, 0, 77, UP_ARROW );
  else
    VidPokec( VIDStatusWindow, 0, 77, DOWN_ARROW );
}


//*
//* void SystemCleanup( void )
//*
//* ARGUMENTS
//*   NONE
//*
//* DESCRIPTION
//*   This function will clean up all the system objects upon exit
//*   from the program.
//*
//* SIDE EFFECTS
//*   Destroy all objects allocated in the system
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
void SystemCleanup( void )
{
  if( SplitScreen )
    VidSet80x25();
  if( VIDDebugWindow )
    free( VIDDebugWindow );
  if( VIDStatusWindow )
    free( VIDStatusWindow );
  if( DTP )
    delete DTP;
}


//*
//* int HandleDos( void )
//*
//* ARGUMENTS
//*   NONE
//*
//* DESCRIPTION
//*   This function handles the different DOS options.
//*
//* SIDE EFFECTS
//*
//* RETURNS
//*   TRUE or FALSE
//*
//* AUTHOR
//*   Patrick Little
//*
//* MODIFICATIONS
//*
//*
int HandleDos( void )
{
  static char *Menu[] = {
    "Shell", "Hangup the modem and exit", "Exit, don't try to hang up", "" };

  switch ( VidMenu( 5, MenuColumn, Menu ) ) {
    case 0 :
      DTP->MainWindow->Clear();
      DTP->MainWindow->Printf( "\nCopyright 1989-1995 (C) Greenleaf Software Inc." );
      DTP->MainWindow->Printf( "\nAll rights reserved\n");
      DTP->MainWindow->Printf( "*** Type EXIT When Finished ***\n" );
      system( "command" );
      break;
    case 1 :
      if( GoodBye() )
        return( TRUE );
      break;
    case 2 :
      return( TRUE );
  }
  return( FALSE );
}



//*
//* void HandleUtility( void )
//*
//* ARGUMENTS
//*   NONE
//*
//* DESCRIPTION
//*   This function handles all the menus selections for the
//*   Utility menu.
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
void HandleUtility( void )
{
  static char *Menu[] = {
    "Change Parameters", "Clear Screen", "Dump Port Status",
    "Flush TX Buffer", "Flush RX Buffer", "Toggle DTR", "Toggle RTS",
    "Send Break", "Send Test Buffer", "Toggle Reading Flag", "" };

  switch( VidMenu( 5, MenuColumn, Menu ) ) {
    case 0 :
      do {
        DTP->GetParameters();
        DTP->PortSelection();
      } while( DTP->PortSelection() < GF_OK );
      break;
    case 1: DTP->MainWindow->Clear(); break;
    case 2: DTP->DumpViewer();        break;
    case 3: DTP->FlushTX();           break;
    case 4: DTP->FlushRX();           break;
    case 5: DTP->ToggleDTR();         break;
    case 6: DTP->ToggleRTS();         break;
    case 7: DTP->SendBreak();         break;
    case 8: DTP->SendTestBuffer();    break;
    case 9: DTP->ToggleReading();     break;
  }
}


//*
//* void HandlePhoneCommand( void )
//*
//* ARGUMENTS
//*   NONE
//*
//* DESCRIPTION
//*   This function will handle all the selections for the phone menus.
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
void HandlePhoneCommand( void )
{
  static char *Menu[] = {
      "Dial New Number", "Redial", "Answer", "Hang Up",
      "Dump Modem Registers", "Initialize Modem", "" };

  switch ( VidMenu( 5, MenuColumn, Menu ) ) {
    case 0: DTP->Dial();                break;
    case 1: DTP->Redial();              break;
    case 2: DTP->AnswerPhone();         break;
    case 3: DTP->HangUp();              break;
    case 4: DTP->DumpModemRegisters();  break;
    case 5: DTP->InitializeModem();     break;
  }
  DTP->DisplayParameters();
}


//*
//* int HandleCommand( void )
//*
//* ARGUMENTS
//*   NONE
//*
//* DESCRIPTION
//*   This function handles the menu selections for the main menus
//*   in the program.
//*
//* SIDE EFFECTS
//*
//* RETURNS
//*   TRUE or FALSE
//*
//* AUTHOR
//*   Patrick Little
//*
//* MODIFICATIONS
//*
//*
int HandleCommand( void )
{
  static char *Menu[] = { "Phone", "Send a file(s)", "Receive a file(s)",
      "Utility", "DOS", "" };
  static char *TMenu[] = { "Ascii", "Kermit", "Xmodem", "Xmodem 1K",
      "Ymodem", "Zmodem", "Xmodem 1KG", "Ymodem G", "" };

  switch( VidMenu( 5, MenuColumn, Menu ) ) {
    case 0 : HandlePhoneCommand();                                   break;
    case 1 : DTP->FileTransfer( VidMenu( 5, MenuColumn, TMenu), 1 ); break;
    case 2 : DTP->FileTransfer( VidMenu( 5, MenuColumn, TMenu), 0 ); break;
    case 3 : HandleUtility();                                        break;
    case 4 : return( HandleDos() );
  }
  return( FALSE );
}


//*
//* void main( int argc, char *argv[] )
//*
//* ARGUMENTS
//*   int argc - The number of command line arguments.
//*   char *argv[] - Pointers to the command line arguments.
//*
//* DESCRIPTION
//*   This is the main for the VIDTERM program.  The use will remain inside
//*   the main program loop until they select the exit option.
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
//*   November 5, 1993 - Added an extra check to fix the ZMODEM auto download
//*                      code.
//*
void main( int argc, char *argv[] )
{
  int c;
  int dbyte;
  long NextUpdate = ElapsedSysTime();

  //*
  //* Initialize the video and comm systems.
  //*
  InitializeVideo( argc, argv );

  if( (DTP = new DOSTerminalProgram()) == NULL )
    return;
  
  if( DTP->OpenPort() < GCPP_OK ) {
    SystemCleanup();
    return;
  }

  //*
  //* Go into the polling loop.  We will stay here until we are
  //* ready to exit the program.
  //*
  for( ; ; ) {
    if( gfkbhit () ) {
      c = DTP->GetKeyIO();
      if( c == ALTM ) {
        if( HandleCommand() )
          break;
      } else if ( c == ALTB )
        VidSelectPage( 0 );
      else if ( c == ALTN )
        VidSelectPage( 1 );
      else
        DTP->WritePort( (char)c );
    }

    //*
    //* Get any output from the port and place it to the screen.
    //*
    for( int count = 0 ; count < 50 ; count++ ) {
      dbyte = DTP->ReadPort();
      if( dbyte < GCPP_OK )
        break;
    }
    if( NextUpdate < ElapsedSysTime() ) {
      //*
      //* It must be time to update system data on screen.
      //*
      UpdateStatusLine();
      DTP->DumpViewer();
      NextUpdate = ElapsedSysTime() + TIME_INTERVAL;
    }
  }

  //*
  //* Time to cleanup the system so that we can exit the program.
  //*
  SystemCleanup();
}

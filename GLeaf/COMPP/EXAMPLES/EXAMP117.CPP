//*
//*  EXAMP117.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1994-95 Greenleaf Software Inc.	All Rights Reserved.
//*
//*  This example demonstrates the GFComWin3::EnableNotification(),
//*  GFComWin3::IsNotification(), GFComWin3::IsNotificationSuspended(),
//*  GFComWin3::SuspendNotification(), GFComWin3::ResumeNotification(),
//*  GFComWin3::SetEventCharacter(), GFComWin3::GetMaskedEvents(),
//*  GFComWin3::GetCommHandle(), GFSerial::GetComLinkPointer(), and
//*  SetTWCommNotifyHandler() functions.  This example uses comm
//*  notification to read characters from the port and display them in
//*  the window.
//*
//*

#include "compplib.hpp"

#if defined( GCPP_WINDOWS ) && !defined( GCPP_WIN32 )

#define STRICT
#include <windows.h>
#include <string.h>
#include "tw.h"
#include "ibmkeys.h"

#if defined( GCPP_USING_DLL )
	#define GCPP_EXPORT _export
  FARPROC lpProcMyHandler;
#else
	#define GCPP_EXPORT
#endif

typedef int ( GCPP_DLL_FAR GCPP_FUN *NOTIFY_HANDLER )( TW_WINDOW *tw,
                                                    HWND hwnd,
                                                    WPARAM wParam,
                                                    LPARAM lParam );

GFSerial *port;

int GCPP_FUN GCPP_EXPORT MyTWNotifyHandler( TW_WINDOW *tw,
																				HWND hwnd,
                                        WPARAM wParam,
                                        LPARAM lParam )
{
	int c;
	int status;
	GFComWin3 *sio = (GFComWin3 *)port->GetComLinkPointer();

	GCPP_UNUSED_PARAMETER( hwnd );

	if( sio->GetCommHandle() != (int)wParam )
		return( GCPP_GENERAL_ERROR );

	status = LOWORD( lParam );
	switch( status ) {
		case CN_RECEIVE:
			//*
			//* This is where the handler takes care of receive
			//* events.
			//*
			c = port->GetChar();
			TWPutc( tw, c );
			break;

		case CN_TRANSMIT:
			//*
			//* This is where the transmit events are handled.
			//*
			break;

		case CN_EVENT:
			//*
			//* If we wanted to do something based on the events
			//* that were masked for message notification, this
			//* is where we would do that.	We chose not to do
			//* anything here but clear the events.
			//*
			sio->GetMaskedEvents();
			break;
	}
	return( GCPP_OK );
}

int PASCAL WinMain( HINSTANCE hInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpszCmdLine,
                    int nCmdShow )
{
	GFComWin3 *sio;
	TW_WINDOW *MainWindow;
	int c;
	unsigned short error;
  char buffer[ 81 ];
    
	GCPP_UNUSED_PARAMETER( lpszCmdLine );

	if( !TWInitialize( hInstance,
											hPrevInstance,
											nCmdShow,
											"Comm++ Example 320" ) )
		return( 0 );
    
	MainWindow = TWDefineWindow( 0, 20, 40, 25, 0, "MainWindow", "Fixedsys", 12 );
	sio = new GFComWin3( COM2 );
	port = new GFSerial( sio );
	if( port->GetCommError( error ) < GCPP_OK ) {
		TWPrintf( MainWindow, "Failed to open the port." );
		return( 0 );
	}
	TWPuts( MainWindow, "Greenleaf Software, Inc.\n" );
	TWPuts( MainWindow, "Now does Windows!\n" );
	TWPuts( MainWindow, "Hit the ESCape key to exit..." );
	sio->EnableNotification( port, MainWindow->hwnd, 5, 25, ON_EVENTS_DISABLED );
	GFLineStatus *ls = port->GetLineStatus();
	ls->Dtr( GCPP_ON );

#if defined( GCPP_USING_DLL )
	lpProcMyHandler = MakeProcInstance( (FARPROC)MyTWNotifyHandler, hInstance );
	SetTWCommNotifyHandler( (NOTIFY_HANDLER)lpProcMyHandler );

	while( !( MainWindow->lpThis->TWAppClosed ) ) {
#else
	SetTWCommNotifyHandler( (NOTIFY_HANDLER)MyTWNotifyHandler );

	while( !TWAppClosed ) {
#endif
    if( TWKbhit( MainWindow ) ) {
      c = TWGetkey( MainWindow );
      if( c == 27 )
        break;
      else if( c == F1 )
        sio->SuspendNotification();
      else if( c == F2 )
        sio->ResumeNotification();
      else if( c == F3 ) {
        TWGetLine( "New Windows Event Character",
                   "The Character is:",
                   buffer,
                   1 );
        if( strlen( buffer ) )
          sio->SetEventCharacter( buffer[0] );
        else
          sio->SetEventCharacter( 0 );
      }
      port->PutChar( (char)c );
    }
    if( sio->IsNotification() && sio->IsNotificationSuspended() ) {
      c = port->GetChar();
      if( c != GCPP_TIMEOUT )
        TWPutc( MainWindow, c );
    }
  }
#if defined( GCPP_USING_DLL )
  FreeProcInstance( lpProcMyHandler );
#endif
	if( MainWindow ) {
    TWFreeInstanceData();
		TWFreeWindow( MainWindow );
  }
	if( port ) {
		delete port;
		delete sio;
	}
	return( 0 );
}

#else /* #ifdef GF_WINDOWS */
main() { return 1; }
#endif /* #ifdef GF_WINDOWS ... #else*/


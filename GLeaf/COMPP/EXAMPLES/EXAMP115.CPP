//*
//*  EXAMP115.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1989-1995 Greenleaf Software Inc.	All Rights Reserved.
//*
//*  This example demonstrates the GFCompuServeBPlus file transfer class.
//*
#include "compplib.hpp"
#include "filexfer.hpp"
#include "ibmkeys.h"
#include <iostream.h>

class GFMtr : public GFMonitor
{
  public:
    void TransferMessage( char *message );
};

void main( void )
{
	int c;
	int return_value;
	int last_char = -1;

	GFI8250 sio( COM1 );
	GFSerial cp( &sio );
	GFMtr mtr;
	GFCompuServeBPlus bplus( &cp, &mtr );

	for ( ; ; ) {
		if ( gfkbhit() ) {
			c = getkey();
			if ( c == ESC )
				return;
			else
				cp.PutChar( (unsigned char)c );
		}
		return_value = cp.GetChar();
    if ( return_value >= GCPP_OK ) {
			if ( return_value == ENQ )
        return_value = bplus.Respond( &cp );
      else if ( return_value == 'B' && last_char == DLE ) {
        bplus.SetSendFileInfoFlag( GCPP_OFF );
        bplus.SetStripPathOption( GCPP_ON );
        bplus.SetResumeDownloadFlag( GCPP_OFF );
        bplus.SetChecksumMethod( 3 );
				return_value = bplus.TransferFile();
      } else if ( return_value != DLE )
        cout << (char)return_value << flush;
			last_char = return_value;
		}
	}
}

void GFMtr::TransferMessage( char *message )
{
  cout << message << "\n" << flush;
}

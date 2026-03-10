//*
//*  EXAMP098.CPP
//*
//*  The Greenleaf Comm++ Library
//*
//*  Copyright (C) 1989-1995 Greenleaf Software Inc.  All Rights Reserved.
//*
//*  This example demonstrates the SubmodelType() , MachineType(),
//*  IsMicroChannelBus() and Modify8259Priority() functions.
//*
#include "compplib.hpp"
#include "ibmkeys.h"
#include <iostream.h>

void main( void )
{
  int c;

  cout << "MicroChannelBus: " << IsMicroChannelBus() << "\n";
  cout << "MachineType: " << MachineType() << "\n";
  cout << "SubmodelType: " << SubmodelType() << "\n";

  GFI8250 sio( COM2 );
  GFSerial cp( &sio );

  cout << "Setting priority to IRQ1\n";
  Modify8259Priority( IRQ1 );

  for(;;) {
    if( gfkbhit() ) {
      c = getkey();
      switch( c ){
        case ESC:
          return;
        case F1:
          cout << "\nSetting priority to IRQ1.\n";
          Modify8259Priority( IRQ1 );
          break;
        case F2:
          cout << "\nSetting priority to IRQ3.\n";
          Modify8259Priority( IRQ3 );
        default:
          cp << c;
      }
    }
    cp >> c;
    if( c != GCPP_TIMEOUT )
      cout << (char)c << flush;
  }
}

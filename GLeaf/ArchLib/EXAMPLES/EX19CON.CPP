/*= Archive Library v2.12 ================================================
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  EX19CON demos the wild card matching functions.  You can see that the
 |  wildcards we support here are just a little bit better than those that
 |  you get for free with MS-DOS wild card matching.
 |
 |  We also demo a bunch of the ALName functions here.
 |
 +- Functions ------------------------------------------------------------
 |  ALName::ALName()
 |  ALName::~ALName()
 |  ALName::ChangeExtension()
 |  ALName::ChangeTrailingChar()
 |  ALName::GetName()
 |  ALName::StripFileName()
 |  ALName::StripPath()
 |  ALName::WildCardMatch()
 |
 +========================================================================*/

#include <conio.h>
#include <stdlib.h>

#include "arclib.h"

int main()
{
 cout << "Archive Library 2.11\nEX19CON.CPP\n\n";
 cout << "Press ESC to abort, any other key to continue..." << flush;
 int c = getch();
 cout << "\n\n";
 if (c == 0 || c == 0x1b || c == 3)
  exit(1);

 ALName test("Greenleaf Software", AL_UPPER);
 cout << "test = " << test << endl;
 cout << "test.WildCardMatch( \" *Soft* \" ) = "
      << test.WildCardMatch("*Soft*")
      << endl;
 cout << "test.WildCardMatch( \" *Hard* \" ) = "
      << test.WildCardMatch("*Hard*")
      << endl;
 cout << "test.WildCardMatch( \" GREENLEAF ? ? ? ? ware \" ) = "
 << test.WildCardMatch("GREENLEAF ????ware")
 << "\n";
 ALName file("EX19CON.CPP");
 cout << "file = " << file.GetName() << endl;
 cout << "file.ChangeExtension() = "
      << file.ChangeExtension()
      << endl;
 file = "EX19CON.CPP";
 cout << "file.ChangeTrailingChar() = "
      << file.ChangeTrailingChar()
      << endl;
 file = "EX19CON.CPP";
 cout << "file.StripFileName() = "
      << file.StripFileName()
      << endl;
 file = "EX19CON.CPP";
 cout << "file.StripPath() = "
      << file.StripPath()
      << endl;

 return 1;
}

/*= Archive Library v2.12 ================================================
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  EX10CON is used to demonstrate some of the pattern matching functions.
 |  It is sort of a baby grep, able to look through any list of files for
 |  a particular string.
 |
 +- Functions ------------------------------------------------------------
 |  ALWildCardExander::GetNextFile()
 |
 +- Notes ----------------------------------------------------------------
 |  Usage: EX10CON pattern wild-card
 |  Defaults:  new *.CPP
 |
 +========================================================================*/

#include <fstream.h>
#include <conio.h>
#include <stdlib.h>
#include "al.h"
#include "_match.h"

void scan(char *name, char *pattern);

int main(int argc, char *argv[])
{
 cout << "Archive Library 2.11\nEX10CON.CPP\n\n";
 cout << "Press ESC to abort, any other key to continue..." << flush;
 int c = getch();
 cout << "\n\n";
 if (c == 0 || c == 0x1b || c == 3)
  exit(1);

 // The first part of the program just sets up the arguments, then
 // calls a couple of the match diagnostic functions.

 char *names = "*.CPP";
 char *pattern = "new";
 if (argc > 1)
  pattern = argv[1];
 if (argc > 2)
  names = argv[2];
 cout << "Pattern : " << pattern << "\n";
 int error_type;
 cout << "contains wild characters: "
      << is_pattern(pattern)
      << "\n";
 cout << "Is valid: "
      << is_valid_pattern(pattern, &error_type)
      << "\n";
 if (matche("[A-", "test") != MATCH_PATTERN)
  cout << "Logic error in match engine\n";
 cout << "Hit any key to continue..." << flush;
 getch();
 cout << endl;

 // The second part of the program creates an extended pattern, which
 // should look like this:  "*pattern*".  We are going to search for
 // that search pattern in each line of each file.  The file names are
 // expanded one by one, and the scanner subroutine is called.

 char *extended_pattern = new char[strlen(pattern) + 2];
 strcpy(extended_pattern, "*");
 strcat(extended_pattern, pattern);
 strcat(extended_pattern, "*");
 ALWildCardExpander *machine = new ALWildCardExpander(names);
 char *name;
 while ((name = machine->GetNextFile()) != 0)
  scan(name, extended_pattern);
 delete machine;
 return 1;
}

// This routine reads in a line at a time from the specified file. It
// tests each line for a match to the pattern.  Any matches are printed out,
// and that is that.

void scan(char *name, char *pattern)
{
 char buffer[133];
 int line_number = 0;
 
 ifstream input;
 input.open(name);
 do {
  input.getline(buffer, 133);
  line_number++;
  if (match(pattern, buffer))
   cout << name << " " << line_number << ":" << buffer << "\n";
 } while (!input.eof());
}

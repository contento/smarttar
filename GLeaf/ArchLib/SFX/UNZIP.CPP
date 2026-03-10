/*= Archive Library v2.12 ================================================
 |
 |  SFX\UNZIP.CPP
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  This is a template program for creating self-extracting PKZip files.
 |  This is NOT a stand-alone program.  Compile this.  UNZIP.EXE is used
 |  by MAKESFX.EXE.
 |
 +=======================================================================*/

#include <conio.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <dos.h>

#include "arclib.h"
#include "pkarc.h"
#include "filestor.h"
#include "bargraph.h"

void parseCommandLine(char *argv[]);
void MyExtract(ALPkArchive& archive, ALEntryList& list);
int DoesFileExist(char *fileName);

int dFlag = 0, oFlag = 0;
enum { FALSE, TRUE };

main(int argc, char *argv[])
{
 argc = argc;
 ALBarGraph monitor(AL_MONITOR_OBJECTS, cout);
 ALEntryList list(&monitor, PkDecompressFileTools());
 ALPkArchive archive(argv[0]);

 parseCommandLine(argv);
 cout << "GLSFX Self Extract Utility  Version 2.11 10-01-96\n";
 cout << "Copyright (c) 1996 Greenleaf Software, Inc.  All Rights Reserved\n\n";
 cout << "Do you wish to extract files? [Y/n]" << flush;
 int c = getch();
 if(c == 'N' || c == 'n')
  return 0;
 cout << endl << endl << flush;
 archive.ReadDirectory(list);
 MyExtract(archive, list);
 return 1;
}

void parseCommandLine(char *argv[])
{
 char c;
 while ((c=argv[1][0])=='/' || c=='-')
 {
  switch(tolower(argv[1][1]))
  {
   case 'd': // extract with path?
             dFlag = TRUE;
             break;
   case 'o': // overwrite files?
             oFlag = TRUE;
             break;
   default:
             cout << "ERROR!";
  }
  argv++;
 }
}

void MyExtract(ALPkArchive& archive, ALEntryList& list)
{
 int overwrite = 1;

 ALEntry *entry = list.GetFirstEntry();
 while(entry)
 {
  ALName temp(entry->mpStorageObject->mName);
  if(!dFlag)
  {
   temp.StripPath();
   entry->mpStorageObject->mName = temp;
  }
  if(!oFlag)
    if(DoesFileExist(entry->mpStorageObject->mName))
    {
     ALName stripped(entry->mpStorageObject->mName);
     stripped.StripPath();
     cout << stripped.GetSafeName() << " exists. Replace? [Y/n] " << flush;
     int c = getch();
     cout << endl;
     if(c == 'N' || c == 'n')
      overwrite = 0;
    }

  if(overwrite)
  {
   list.ClearMarks();
   list.SetMarks(entry->mpStorageObject->mName);
   archive.Extract(list);
  }
  overwrite = 1;
  entry = entry->GetNextEntry();
 }
}

int DoesFileExist(char *fileName)
{
 FILE *in;

 if((in = fopen(fileName, "r")) != NULL )
 {
  fclose(in);
  return 1;
 }
 return 0;
}

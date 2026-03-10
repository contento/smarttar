/*= Archive Library v2.12 ================================================
 |
 |  SFX\MAKESFX.CPP
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  This is the actual ZIP SFX creator.  It uses UNZIP.EXE as the template
 |  executable to prepend to the archive.
 |
 +- Note -----------------------------------------------------------------
 |  I get the size of MAKESFX.EXE and UNZIP.EXE.  Once you've obtained
 |  these numbers from compiling MAKESFX.CPP and UNZIP.CPP, you could hard
 |  code those numbers in, making MAKESFX.EXE independent.
 |
 +=======================================================================*/

#include <stdlib.h>
#include <io.h>
#include <fcntl.h>

#if !defined(__ZTC__) && !defined(__TURBOC__)
 #include <sys\types.h>
#endif
#include <sys\stat.h>

#include <iostream.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include "filestor.h"
#include "pkarc.h"

int LocateEcd();
int UpdateEcd();
int file_copy( char *from, char *to );

long offset;
long total_size = 0L;
int num_entries;
ALFile *sfx;

int main( int argc, char *argv[])
{
 int c;

 if (argc < 2) {
  cout << "MAKESFX Self-Extract Creator Utility  v1.0 10-17-96\n";
  cout << "Copyright (c) 1996 Greenleaf Software, Inc.  All Rights Reserved\n\n";
  cout << "Usage: makesfx zipfile\n\n";
  cout << "Example: makesfx data.zip\n";
  return -1;
 }
 cout << "MAKESFX Self-Extract Creator Utility  v1.0 10-17-96\n";
 cout << "Copyright (c) 1996 Greenleaf Software, Inc.  All Rights Reserved\n\n";
 cout << " Creating a self-extracting zip file!\n\n";
 // open specified zip file
 ALFile zip(argv[1]);
 if(zip.Open())
 {
  cout << "Can't open file!" << endl;
  return -1;
 }
 // create temporary file
 file_copy(argv[1], "~AL~.TMP");

 // get size of UNZIP.EXE
 ALFile unzip("UNZIP.EXE");
 unzip.Open();
 long unzip_size = unzip.GetSize();
 unzip.Close();

 // add size of UNZIP.EXE to the current mlCompressedObjectPosition of each
 // object stored in the zip file
 ALPkArchive archive("~AL~.TMP");
 ALEntryList list(0, PkTools());
 num_entries = archive.ReadDirectory(list);
 for (ALEntry *entry = list.GetFirstEntry(); entry; entry = entry->GetNextEntry())
 {
  // total_size is used in UpdateEcd()
  total_size += strlen( entry->mpStorageObject->mName);
  // add UNZIP.EXE size to mlCompressedObjectPosition
  entry->mlCompressedObjectPosition += unzip_size;
 }
 // write new directory
 archive.WriteDirectory(list);
 // change file name's extension to EXE
 ALName new_name(argv[1]);
 new_name.ChangeExtension(".EXE");
 sfx = new ALFile(new_name);
 // create self-extracting file
 sfx->Create();
 ALFile makesfx("makesfx.exe");
 // open MAKESFX.EXE
 makesfx.Open();
 // get size of MAKESFX.EXE
 long makesfx_size = makesfx.GetSize();
 // seek to the end of MAKESFX.EXE minus the size of UNZIP.EXE
 makesfx.Seek(makesfx_size - unzip_size);
 // read UNZIP.EXE from MAKESFX.EXE and write to sfx file
 while ((c = makesfx.ReadChar()) >= 0)
  sfx->WriteChar(c);
 // open temp file
 ALFile temp("~AL~.TMP");
 temp.Open();
 while ((c = temp.ReadChar()) >= 0)
  sfx->WriteChar(c);
 // close temp file
 temp.Close();
// delete temp file
 remove("~AL~.TMP");
 // update sfx size by closing and re-opening
 sfx->Close();
 sfx->Open();

 // locate the ZIP central directory
 if (LocateEcd())
 {
  cout << "ERROR - Cannot locate central directory!" << endl;
  return -1;
 }
 // update the ZIP central directory
 UpdateEcd();
 // close sfx file
 sfx->Close();
 // close makesfx file
 makesfx.Close();
 // delete sfx object
 if (sfx)
 {
  delete sfx;
  sfx = 0;
 }
 cout << argv[1] << " => " << new_name << endl;
 // we are finished!
 return 1;
}

// obtain the offset position of the end of central directory record
// code obtained from CPP_ALL\PKARC.CPP with minor modifications
int LocateEcd()
{
 const int BUF_SIZE = 256;
 char buf[BUF_SIZE + 4];

 offset = sfx->GetSize();
 long left_to_check = offset < 66000L ? offset : 66000L;
 buf[0] = '\0'; // prevent false trigger the first time through
 for (;;) {
  int number_to_read;
  if (left_to_check == 0)
   break;
  if (left_to_check < BUF_SIZE)
   number_to_read = (int) left_to_check;
  else
   number_to_read = BUF_SIZE;
  offset -= number_to_read;
  memcpy(buf + BUF_SIZE, buf, 4);
  sfx->Seek(offset);
  sfx->ReadBuffer((unsigned char *) buf, number_to_read);
  for (int i = number_to_read; i >= 0; i--) {
   if (buf[i + 0] == 'P' &&
    buf[i + 1] == 'K' &&
    buf[i + 2] == 5 &&
    buf[i + 3] == 6) {
    offset += i;
    return AL_SUCCESS;
   }
  }
  left_to_check -= number_to_read;
 }
 return -1;
}

// update the central directory
// code obtained from CPP_ALL\PKARC.CPP with minor modifications
int UpdateEcd()
{
 // position to the offset of the start of central directory
 sfx->Seek(offset + 16);
 // compute new central directory offset
 long dir_offset = offset - (num_entries * 46 + total_size);
 // write new offset
 sfx->WritePkLong(dir_offset);
 sfx->FlushBuffer();
 return AL_SUCCESS;
}

/*
** by: Walter Bright via Usenet C newsgroup
**
** modified by: Bob Stout based on a recommendation by Ray Gardner
**
** modified by: David Gersic to deal with binary files
**
** There is no point in going to asm to get high speed file copies. Since it
** is inherently disk-bound, there is no sense (unless tiny code size is
** the goal). Here's a C version that you'll find is as fast as any asm code
** for files larger than a few bytes (the trick is to use large disk buffers):
*/

int file_copy(char *from, char *to)
{
 int fdfrom,fdto;
 int bufsiz;

 fdfrom = open(from,O_RDONLY|O_BINARY,0);
 if (fdfrom < 0)
  return 1;

 /* Open R/W by owner, R by everyone else */

 fdto=open(to,O_BINARY|O_CREAT|O_TRUNC|O_RDWR,S_IREAD|S_IWRITE);
 if (fdto < 0)
  goto err;

 /* Use the largest buffer we can get */

 for (bufsiz = 0x4000; bufsiz >= 128; bufsiz >>= 1)
 {
  register char *buffer;

  buffer = (char *) malloc(bufsiz);
  if (buffer)
  {
   while (1)
   {
    register int n;

    n = read(fdfrom,buffer,bufsiz);
    if (n == -1)  /* if error */
     break;
    if (n == 0) /* if end of file */
    {
     free(buffer);
     close(fdto);
     close(fdfrom);
     return 0; /* success */
    }
    if (n != write(fdto,buffer,(unsigned) n))
     break;
   }
   free(buffer);
   break;
  }
 }
 close(fdto);
 remove(to);  /* delete any partial file */
err:  close(fdfrom);
 return 1;
}

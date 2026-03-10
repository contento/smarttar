/*= Archive Library v2.12 ================================================
 |
 |  H\PKARC.H
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  PKARC.H contains the class definition for class ALPkArchive.
 |
 +- Classes --------------------------------------------------------------
 |  ALPkArchive
 |
 +=======================================================================*/

#ifndef __PKARC_H__
#define __PKARC_H__

#ifdef __cplusplus

/*- Class Name -----------------------------------------------------------
 |  ALPkArchive
 |
 +- Version --------------------------------------------------------------
 |  2.11.00 01 Oct 1996 12:00
 |
 +- Description ----------------------------------------------------------
 |  ALPkArchive defines the interface to PKWare archive objects, on disk,
 |  memory, or whatever. It is a derived class that is a sibling to
 |  ALGlArchive.  Most of the interface to this archive class is defined in
 |  the base class.
 |
 +- Data Members ---------------------------------------------------------
 |  mlStartOfCentralDir  - An offset in the archive to the start of the
 |                         central directory.
 |
 |  mEcd                 - A copy of the End-Of-Central-Directory record,
 |                         as found in the ZIP file.
 |
 +- Member Functions -----------------------------------------------------
 |  ALPkArchive()              - Construct from a storage object
 |  ALPkArchive()              - Construct from a named ALFile object
 |  ~ALPkArchive()             - The virtual destructor.
 |  operator new()             - Memory allocation operator used when
 |                               the library is in a DLL
 |  ReadEcd()                  - Function used to read in the
 |                               End-Of-Central-Directory record, mEcd.
 |  LocateEcd()                - Locates the ECD by stumbling around near
 |                               the end of the file.
 |  AddJobs()                  - Virtual function that adds new objects
 |                               to the PKWare archive.
 |  WriteArchiveData()         - Called by the base class to write archive
 |                               specific data.
 |  ReadArchiveData()          - Called by the base class to read archive
 |                               specific data.
 |  PreCompress()              - Virtual function called by the base class
 |                               before compressing an object.
 |  PostCompress()             - Virtual function called by the base class
 |                               after compressing an object.
 |  PreDecompress()            - Virtual function called by the base class
 |                               before decompressing an object.
 |  PreWriteDir()              - Virtual function called by the base class
 |                               before writing the directory information
 |                               to the storage object.
 |  WriteDirEntry()            - Virtual function called by the base class
 |                               to write an entry in a directory.
 |  PostWriteDir()             - Virtual function called by the base class
 |                               after writing all the directory entries.
 |  PreCopyInput()             - Virtual function called by the base class
 |                               before copying a compressed file.
 |  ReadDirectory()            - Virtual function that reads in the entire
 |                               ZIP file directory.
 |
 +- Revision History -----------------------------------------------------
 |
 +-----------------------------------------------------------------------*/

// Tag public class
class AL_CLASS_TYPE ALPkArchive : public ALArchive {

// constructors, destructors, declarations, and friends

 protected :
 public :
  AL_PROTO ALPkArchive(ALStorage AL_DLL_FAR &);
  AL_PROTO ALPkArchive(const char AL_DLL_FAR *file_name);
  virtual AL_PROTO ~ALPkArchive();
#if defined(AL_USING_DLL) || defined(AL_BUILDING_DLL)
  void AL_DLL_FAR * AL_PROTO operator new(size_t size);
#endif

// The copy constructor and assignment operator do not exist.

 protected :
  AL_PROTO ALPkArchive(const ALPkArchive AL_DLL_FAR &);
  ALPkArchive AL_DLL_FAR & AL_PROTO operator=(ALPkArchive AL_DLL_FAR &rhs);

// member functions

 protected :
  int AL_PROTO ALPkArchive::ReadEcd();
  int AL_PROTO ALPkArchive::LocateEcd();
  int AL_PROTO AddJobs(ALEntryList AL_DLL_FAR & list);
  virtual int AL_PROTO WriteArchiveData();
  virtual int AL_PROTO ReadArchiveData();
  virtual void AL_PROTO PreCompress(ALEntry AL_DLL_FAR &entry);
  virtual void AL_PROTO PostCompress(ALEntry AL_DLL_FAR &entry);
  virtual void AL_PROTO PreDecompress(ALEntry AL_DLL_FAR &entry);
  virtual void AL_PROTO PreWriteDir();
  virtual void AL_PROTO WriteDirEntry(ALEntry AL_DLL_FAR &entry);
  virtual void AL_PROTO PostWriteDir();
  virtual void AL_PROTO PreCopyInput(ALEntry AL_DLL_FAR &entry);

// These functions provide the public API to an archive.

 public :
  virtual int AL_PROTO ReadDirectory(ALEntryList AL_DLL_FAR &list);

// protected data members

 protected :
  // Tag protected class
  struct AL_CLASS_TYPE {
     long mlOffset;
     short int muThisDiskNumber;
     short int muStartCentralDirDiskNumber;
     short int muThisDiskCentralDirEntries;
     short int muTotalCentralDirEntries;
     long mlSizeCentralDir;
  } mEcd;
  long mlStartOfCentralDir;

 public :
  AL_CLASS_TAG(_ALPkArchiveTag);
};

extern "C" {

#endif // #ifdef __cplusplus

AL_LINKAGE hALArchive AL_FUNCTION newALPkArchiveFromStorage(hALStorage storage);
AL_LINKAGE hALArchive AL_FUNCTION newALPkArchive(char AL_DLL_FAR *name);

#ifdef __cplusplus
};
#endif

#endif // #ifndef __PKARC_H__

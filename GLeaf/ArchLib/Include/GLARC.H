/*= Archive Library v2.12 ================================================
 |
 |  H\GLARC.H
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  GLARC.H contains the class definition for class ALGlArchive.
 |
 +- Classes --------------------------------------------------------------
 |  ALGlArchive
 |
 +=======================================================================*/

#ifndef __GLARC_H__
#define __GLARC_H__

#ifdef __cplusplus

/*- Class Name -----------------------------------------------------------
 |  ALGlArchive
 |
 +- Version --------------------------------------------------------------
 |  2.11.00 01 Oct 1996 12:00
 |
 +- Description ----------------------------------------------------------
 |  ALGlArchive defines the interface to greenleaf archive objects, on disk,
 |  memory, or whatever. It is a derived class that is a sibling to
 |  ALPkArchive.  Most of the interface to this archive class is defined in
 |  the base class.
 |
 +- Data Members ---------------------------------------------------------
 |  None
 |
 +- Member Functions -----------------------------------------------------
 |  ALGlArchive()            -   Construct from a storage object
 |  ALGlArchive()            -   Construct from a named ALFile object
 |  ~ALGlArchive()           -   The virtual destructor.
 |  operator new()           -   Memory allocation operator used when
 |                               the library is in a DLL
 |  CopyJobs()               -   Virtual function that copies compressed
 |                               objects from on GL archive to another.
 |  PreCreate()              -   Virtual function called by the base class
 |                               before constructing an archive.
 |  PreWriteDir()            -   Virtual function called by the base class
 |                               before writing the directory information
 |                               to the storage object.
 |  WriteDirEntry()          -   Virtual function called by the base class
 |                               to write an entry in a directory.
 |  PostWriteDir()           -   Virtual function called by the base class
 |                               after writing all the directory entries.
 |  ReadDirectory()          -   Read the directory into an ALEntryList
 |
 +- Revision History -----------------------------------------------------
 |
 +-----------------------------------------------------------------------*/

// Tag public class
class AL_CLASS_TYPE ALGlArchive : public ALArchive {

// constructors, destructors, declarations, and friends

 public :
  AL_PROTO ALGlArchive(ALStorage AL_DLL_FAR &object);
  AL_PROTO ALGlArchive(const char AL_DLL_FAR *file_name);
  virtual AL_PROTO ~ALGlArchive();
#if defined(AL_USING_DLL) || defined(AL_BUILDING_DLL)
  void AL_DLL_FAR * AL_PROTO operator new(size_t size);
#endif

// copy constructor and assignment operator do not exist
 protected :
  AL_PROTO ALGlArchive(const ALGlArchive AL_DLL_FAR &);
  ALGlArchive AL_DLL_FAR & AL_PROTO operator=(ALGlArchive AL_DLL_FAR &rhs);

// member functions
// used internally, not for public access

 protected :
  int AL_PROTO CopyJobs(ALArchive AL_DLL_FAR & source_archive, ALEntryList AL_DLL_FAR & list);
  virtual void AL_PROTO PreCreate();
  virtual void AL_PROTO PreWriteDir();
  virtual void AL_PROTO WriteDirEntry(ALEntry AL_DLL_FAR &entry);
  virtual void AL_PROTO PostWriteDir();

// functions provide the public API to an archive

  public :
   virtual int AL_PROTO ReadDirectory(ALEntryList AL_DLL_FAR &list);

// data members

  public :
   AL_CLASS_TAG(_ALGlArchiveTag);
};

extern "C" {

#endif // #ifdef __cplusplus

AL_LINKAGE hALArchive AL_FUNCTION newALGlArchiveFromStorage(hALStorage storage);
AL_LINKAGE hALArchive AL_FUNCTION newALGlArchive(char AL_DLL_FAR *name);

#ifdef __cplusplus
};
#endif

#endif // #ifndef __GLARC_H__

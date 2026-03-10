/*= Archive Library v2.12 ================================================
 |
 |  H\FILESTOR.H
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  FILESTOR.H contains the class definition for the frequently used ALFile
 |  class.
 |
 +- Classes --------------------------------------------------------------
 |  ALFile
 |
 +=======================================================================*/

#ifndef __FILESTOR_H__
#define __FILESTOR_H__

#include "arclib.h"

#ifdef __cplusplus

#include "stor.h"

/*- Class Name -----------------------------------------------------------
 |  ALFile
 |
 +- Version --------------------------------------------------------------
 |  2.11.00 01 Oct 1996 12:00
 |
 +- Description ----------------------------------------------------------
 |  This is the class declaration for ALFile.  This is the fundamental disk
 |  storage class used in Archive Library.  It is derived directly from
 |  ALStorage, so it has to implement versions of the five or six
 |  critical virtual functions, mostly relating to opening files, closing
 |  them, loading and flushing buffers.
 |
 +- Data Members ---------------------------------------------------------
 |  miHandle  -  The handle of the file when opened.  This is the value
 |               returned by the function call to open().
 |
 +- Member Functions -----------------------------------------------------
 |  ALFile()         - The constructor.
 |  ~ALFile()        - The virtual destructor.
 |  operator new()   - The memory allocation operator, only used when the
 |                     library is inside a DLL.
 |  MakeTempName()   - Private function used to generate temporary names.
 |  ReadAttributesFromFileSys()  - A function that has a different
 |                                 implementation for each supported O/S.
 |  WriteAttributesFromFileSys() - A function that has a different
 |                                 implementation for each supported O/S.
 |  MakeDirectory()  - The function that creates directories when needed.
 |  Clone()          - The toolkit function that will make copies of
 |                     the ALFile object when needed.
 |  Open()           - Virtual function to open the file.
 |  Create()         - Virtual function to create the file.
 |  Close()          - Virtual fn to close the file.
 |  LoadBuffer()     - Virtual fn to load the I/O buffer from a specific
 |                     address.
 |  FlushBuffer()    - Virtual fn to flush the I/O buffer.
 |  Seek()           - Virtual fn to seek to seek to a new position in the
 |                     file.
 |  Rename()         - Rename the underlying file.
 |  UnRename()       - Undo the rename process.
 |  Delete()         - Delete the disk file (not destroy!)
 |  RenameToBackup() - Rename the current file to a backup name.
 |
 +- Revision History -----------------------------------------------------
 |
 +-----------------------------------------------------------------------*/

// Tag public class
class AL_CLASS_TYPE ALFile : public ALStorage {

// constructors, destructors, assignment operator, friends, declarations

 public :
  AL_PROTO ALFile(const char AL_DLL_FAR *file_name = "",
                  int buffer_size = 4096,
#if defined(AL_WIN32) || defined(AL_OS2)
                  ALCase name_case = AL_MIXED_UP);
#elif defined(AL_UNIX)
                  ALCase name_case = AL_MIXED);
#else
                  ALCase name_case = AL_LOWER);
#endif
  virtual AL_PROTO ~ALFile();
#if defined(AL_USING_DLL) || defined(AL_BUILDING_DLL)
  void AL_DLL_FAR * AL_PROTO operator new(size_t size);
#endif

// As usual, I don't want the compiler to generate a default copy constructor,
// or an assignment operator here.  I force it to back off by declaring them
// here.  They do not exist!

 protected :
  AL_PROTO ALFile(ALFile AL_DLL_FAR &);
  ALFile AL_DLL_FAR & AL_PROTO operator=(const ALFile AL_DLL_FAR &);

// member functions, grouped by category.


// protected member manipulation, used inside library, not for public use

 protected :
  virtual void AL_PROTO MakeTempName(int i);
  int AL_PROTO ReadAttributesFromFileSys();
  void AL_PROTO WriteAttributesToFileSys();
  virtual int AL_PROTO MakeDirectory();

// file I/O access public interface

 public :
  virtual ALStorage AL_DLL_FAR * AL_PROTO Clone(const char AL_DLL_FAR *name,
                                                int object_type) const;
  virtual int AL_PROTO Open();
  virtual int AL_PROTO Create(long desired_size = -1L);
  virtual int AL_PROTO Close();
  virtual int AL_PROTO LoadBuffer(long address);
  virtual int AL_PROTO FlushBuffer();
  virtual int AL_PROTO Seek(long address);

// file name and underlying object manipulation public interface

 public :
  virtual int AL_PROTO Rename(const char AL_DLL_FAR *new_name = 0,
                              int delete_on_clash = 1);
  virtual int AL_PROTO UnRename(int delete_on_clash = 1);
  virtual int AL_PROTO Delete();
  virtual int AL_PROTO RenameToBackup(int delete_on_clash = 1);

// data members

 protected :
  int miHandle;
 public :
  AL_CLASS_TAG(_ALFileTag);
};

extern "C" {

#endif // #ifdef __cplusplus

AL_LINKAGE hALStorage AL_FUNCTION newALFile(char AL_DLL_FAR *file_name);

#ifdef __cplusplus
};
#endif

#endif // #ifndef __FILESTOR_H__

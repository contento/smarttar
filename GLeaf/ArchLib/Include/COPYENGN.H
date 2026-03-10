/*= Archive Library v2.12 ================================================
 |
 |  H\COPYENGN.H
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  COPYENGN.H contains the class definition for the derived classes
 |  ALCopyCompressor and ALCopyDecompressor.
 |
 +- Classes --------------------------------------------------------------
 |  ALCopyCompressor
 |  ALCopyDecompressor
 |
 +=======================================================================*/

#ifndef __COPYENG_H__
#define __COPYENG_H__

#include "arclib.h"

#ifdef __cplusplus

#include "engn.h"
#include "stor.h"

/*- Class Name -----------------------------------------------------------
 |  ALCopyCompressor
 |
 +- Version --------------------------------------------------------------
 |  2.11.00 01 Oct 1996 12:00
 |
 +- Description ----------------------------------------------------------
 |  ALCopyCompressor is a real simple class.  All it does is
 |  perform a raw binary copy of input to output.
 |
 +- Data Members ---------------------------------------------------------
 |  None
 |
 +- Member Functions -----------------------------------------------------
 |  ALCopyCompressor()     - The constructor.
 |  ~ALCopyCompressor()    - The virtual destructor.
 |  operator new()         - Memory allocation operator, only gets used
 |                           when the library is in a DLL.
 |  Clone()                - If an ALCopyCompressor object is stored in a
 |                           toolkit, this function will be called to
 |                           create new copies of itself.
 |  Compress()             - The compression routine.
 |
 +- Revision History -----------------------------------------------------
 |
 +-----------------------------------------------------------------------*/

// Tag public class
class AL_CLASS_TYPE ALCopyCompressor : public ALCompressor {

// constructors, destructors, declarations, friends

 public :
  AL_PROTO ALCopyCompressor();
  virtual AL_PROTO ~ALCopyCompressor();
#if defined(AL_USING_DLL) || defined(AL_BUILDING_DLL)
  void AL_DLL_FAR * AL_PROTO operator new(size_t size);
#endif

// The copy constructor and assignment operator do not exist.  I define
// them here to prevent the compiler from creating default versions.

 protected :
  AL_PROTO ALCopyCompressor(ALCopyCompressor AL_DLL_FAR &);
  ALCopyCompressor AL_DLL_FAR & AL_PROTO operator=(ALCopyCompressor AL_DLL_FAR & rhs);

// member functions

 public :
  virtual ALCompressor AL_DLL_FAR * AL_PROTO Clone(int engine_type) const;
  virtual int AL_PROTO Compress(ALStorage AL_DLL_FAR &input,
                                    ALStorage AL_DLL_FAR &output);
// data members

 public :
  AL_CLASS_TAG(_ALCopyCompressorTag);
};

/*- Class Name -----------------------------------------------------------
 |  ALCopyDecompressor
 |
 +- Version --------------------------------------------------------------
 |  2.11.00 01 Oct 1996 12:00
 |
 +- Description ----------------------------------------------------------
 |  ALCopyDecompressor is a real simple class.  All it does is perform a
 |  raw binary copy of input to output.  It looks an awful lot like its
 |  sibling, ALCopyCompressor.
 |
 +- Data Members ---------------------------------------------------------
 |  None
 |
 +- Member Functions -----------------------------------------------------
 |  ALCopyDecompressor()   - The constructor.
 |  ~ALCopyDecompressor()  - The virtual destructor.
 |  operator new()         - Memory allocation operator, only gets used
 |                           when the library is in a DLL.
 |  Clone()                - If an ALCopyCompressor object is stored in a
 |                           toolkit, this function will be called to
 |                           create new copies of itself.
 |  DeCompress()           - The decompression routine.
 |
 +- Revision History -----------------------------------------------------
 |
 +-----------------------------------------------------------------------*/

// Tag public class
class AL_CLASS_TYPE ALCopyDecompressor : public ALDecompressor {

// constructors, destructors, declarations, friends

 public :
  AL_PROTO ALCopyDecompressor();
  virtual AL_PROTO ~ALCopyDecompressor();
#if defined(AL_USING_DLL) || defined(AL_BUILDING_DLL)
  void AL_DLL_FAR * AL_PROTO operator new(size_t size);
#endif

// The copy constructor and assignment operator do not exist.  I define
// them here to prevent the compiler from creating default versions.

 protected :
  AL_PROTO ALCopyDecompressor(ALCopyDecompressor AL_DLL_FAR &);
  ALCopyDecompressor AL_DLL_FAR & AL_PROTO operator=(ALCopyDecompressor AL_DLL_FAR & rhs);

// member functions

 public :
  virtual ALDecompressor AL_DLL_FAR * AL_PROTO Clone(int engine_type) const;
  virtual int AL_PROTO Decompress(ALStorage AL_DLL_FAR &input,
                                  ALStorage AL_DLL_FAR &output,
                                  long compressed_length = -1);

// data members

 public :
  AL_CLASS_TAG(_ALCopyDecompressorTag);
};

extern "C" {

#endif // #ifdef __cplusplus

AL_LINKAGE hALCompressor AL_FUNCTION newALCopyCompressor(void);
AL_LINKAGE hALDecompressor AL_FUNCTION newALCopyDecompressor(void);

#ifdef __cplusplus
};
#endif

#endif // #ifndef __COPYENG_H__

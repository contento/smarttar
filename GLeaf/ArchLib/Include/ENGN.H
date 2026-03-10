/*= Archive Library v2.12 ================================================
 |
 |  H\ENGN.H
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  ENGN.H contains the class declaration for ALEngine, the base class
 |  used by all ArchiveLib compression and decompression engines.  It also
 |  contains the base class definition for two classes derived from
 |  ALEngine: ALCompressor and ALDecompressor.
 |
 +- Classes --------------------------------------------------------------
 |  ALEngine
 |  ALCompressor
 |  ALDecompressor
 |
 +=======================================================================*/

#ifndef __CMPENGN_H__
#define __CMPENGN_H__

#ifdef __cplusplus

/*- Class Name -----------------------------------------------------------
 |  ALEngine
 |
 +- Version --------------------------------------------------------------
 |  2.11.00 01 Oct 1996 12:00
 |
 +- Description ----------------------------------------------------------
 |  ALEngine is a base class with pure virtual functions, so it cannot be
 |  instantiated.  The ALCompressor and ALDecompressor classes are both
 |  derived from ALEngine.  ALEngine doesn't do any compression or
 |  decompression itself, but it does keep track of the compression type,
 |  error status, and a bunch of other useful stuff.
 |
 +- Data Members ---------------------------------------------------------
 |  miCompressionType   -  The compression type integer is what gets stored
 |                         in the archive directory, and what the archive
 |                         class looks at when extracting so it can figure
 |                         out what type of compression engine to construct
 |                         for a specific type of object.
 |
 |  mszCompressionType  -  This string just describes the compression type
 |                         in ASCII format suitable for printing or display.
 |
 |  mStatus             -  A standard ArchiveLib status object, stored
 |                         with the compression engine.  Check this after
 |                         completing a compression or decompression to
 |                         see how things went.
 |
 +- Member Functions -----------------------------------------------------
 |  ALEngine()             - The constructor.  Only called by the ctors for
 |                           derived classes.
 |  ~ALEngine()            - The virtual destructor.
 |  operator new()         - Memory allocation operator, only gets used
 |                           when the library is in a DLL.
 |  WriteEngineData()      - Private virtual function used to store engine
 |                           specific data.
 |  ReadEngineData()       - Private virtual function used to read back
 |                           engine specific data.
 |  ClearError()           - A helper function used to reset the mStatus
 |                           member if it gets in an error state.
 |
 +- Revision History -----------------------------------------------------
 |
 +-----------------------------------------------------------------------*/

// Tag public class
class AL_CLASS_TYPE ALEngine {

// constructors, destructors, declarations, and friends

 public :
  AL_PROTO ALEngine(ALCompressionType compression_type_int,
                    const char AL_DLL_FAR *compression_type_string);
  virtual AL_PROTO ~ALEngine();
#if defined(AL_USING_DLL) || defined(AL_BUILDING_DLL)
  void AL_DLL_FAR * AL_PROTO operator new(size_t size);
#endif

// The copy constructor and assignment operator do not exist.  I define
// them here to prevent the compiler from creating default versions.

 protected :
  AL_PROTO ALEngine(ALEngine AL_DLL_FAR &);
  ALEngine AL_DLL_FAR & AL_PROTO operator=(ALEngine AL_DLL_FAR & rhs);

// member functions

 public :
  virtual int AL_PROTO WriteEngineData(ALStorage AL_DLL_FAR * archive);
  virtual int AL_PROTO ReadEngineData(ALStorage AL_DLL_FAR * archive);
  void AL_PROTO ClearError();

// data members

 public :
  const ALCompressionType miCompressionType;
  const char AL_DLL_FAR *mszCompressionType;
  ALStatus mStatus;
  AL_CLASS_TAG(_ALEngineTag);
};

/*- Class Name -----------------------------------------------------------
 |  ALCompressor
 |
 +- Version --------------------------------------------------------------
 |  2.11.00 01 Oct 1996 12:00
 |
 +- Description ----------------------------------------------------------
 |  ALCompressor is the base class for all of the compression engines.
 |  It defines the virtual function Compress(), which is used by
 |  the archiving functions to perform compression.  This is the base
 |  class for the three compressors in ArchiveLib: the copy, greenleaf,
 |  and Pkware compressors.
 |
 +- Data Members ---------------------------------------------------------
 |  None
 |
 +- Member Functions -----------------------------------------------------
 |  ALCompressor()         - The constructor.  Only called by the ctors for
 |                           derived classes.
 |  ~ALCompressor()        - The virtual destructor.
 |  operator new()         - Memory allocation operator, only gets used
 |                           when the library is in a DLL.
 |  Clone()                - If the engine is placed in a toolkit, this
 |                           function can be called to provoke it into
 |                           making a copy of itself.
 |  Compress()             - Compress an input object to an output object.
 |                           This is the big deal.
 |
 +- Revision History -----------------------------------------------------
 |
 +-----------------------------------------------------------------------*/

// Tag public class
class AL_CLASS_TYPE ALCompressor : public ALEngine {

// constructors, destructors, declarations, and friends

 public :
  AL_PROTO ALCompressor(ALCompressionType compression_type_int,
                        const char AL_DLL_FAR *compression_type_string);
  virtual AL_PROTO ~ALCompressor();
#if defined(AL_USING_DLL) || defined(AL_BUILDING_DLL)
  void AL_DLL_FAR * AL_PROTO operator new(size_t size);
#endif

// The copy constructor and assignment operator do not exist.  I define
// them here to prevent the compiler from creating default versions.

 protected :
  AL_PROTO ALCompressor(ALCompressor AL_DLL_FAR &);
  ALCompressor AL_DLL_FAR & AL_PROTO operator=(ALCompressor AL_DLL_FAR & rhs);

// member functions

 public :
  virtual ALCompressor AL_DLL_FAR * AL_PROTO Clone(int engine_type) const = 0;
  virtual int AL_PROTO Compress(ALStorage AL_DLL_FAR &input_object,
                                ALStorage AL_DLL_FAR &output_object) = 0;
// data members

 public :
  AL_CLASS_TAG(_ALCompressorTag);
};

/*- Class Name -----------------------------------------------------------
 |  ALDecompressor
 |
 +- Version --------------------------------------------------------------
 |  2.11.00 01 Oct 1996 12:00
 |
 +- Description ----------------------------------------------------------
 |  ALDecompressor is the base class for all of the decompression engines.
 |  It defines the virtual function Decompress(), which is used by
 |  the archiving functions to perform decompression.  This is the base
 |  class for the three decompressors in ArchiveLib: the copy, greenleaf,
 |  and Pkware decompressors.
 |
 +- Data Members ---------------------------------------------------------
 |  None
 |
 +- Member Functions -----------------------------------------------------
 |  ALDecompressor()       - The constructor.  Only called by the ctors for
 |                           derived classes.
 |  ~ALDecompressor()      - The virtual destructor.
 |  operator new()         - Memory allocation operator, only gets used
 |                           when the library is in a DLL.
 |  Clone()                - If the engine is placed in a toolkit, this
 |                           function can be called to provoke it into
 |                           making a copy of itself.
 |  Decompress()           - Decompress an input object to an output object.
 |                           This is the big deal.
 |
 +- Revision History -----------------------------------------------------
 |
 +-----------------------------------------------------------------------*/

// Tag public class
class AL_CLASS_TYPE ALDecompressor : public ALEngine {

// constructors, destructors, declarations, and friends

 public :
  AL_PROTO ALDecompressor(ALCompressionType compression_type_int,
                          const char AL_DLL_FAR *compression_type_string);
  virtual AL_PROTO ~ALDecompressor();
#if defined(AL_USING_DLL) || defined(AL_BUILDING_DLL)
  void AL_DLL_FAR * AL_PROTO operator new(size_t size);
#endif

// The copy constructor and assignment operator do not exist.  I define
// them here to prevent the compiler from creating default versions.

 protected :
  AL_PROTO ALDecompressor(ALDecompressor AL_DLL_FAR &);
  ALDecompressor AL_DLL_FAR & AL_PROTO operator=(ALDecompressor AL_DLL_FAR & rhs);

// member functions

 public :
  virtual ALDecompressor AL_DLL_FAR * AL_PROTO Clone(int engine_type) const = 0;
  virtual int AL_PROTO Decompress(ALStorage AL_DLL_FAR &input_object,
                                  ALStorage AL_DLL_FAR &output_object,
                                  long compressed_length = -1L) = 0;

// data members

 public :
  AL_CLASS_TAG(_ALDecompressorTag);
};

#include "engn.inl"

extern "C" {

#endif // #ifdef __cplusplus

AL_LINKAGE void AL_FUNCTION ALDecompressorClearError(hALDecompressor this_object);
AL_LINKAGE int AL_FUNCTION ALCompress(hALCompressor this_object,
                                      hALStorage input,
                                      hALStorage output);
AL_LINKAGE int AL_FUNCTION ALDecompress(hALDecompressor this_object,
                                        hALStorage input,
                                        hALStorage output,
                                        long compressed_length);
AL_LINKAGE void AL_FUNCTION deleteALCompressor(hALCompressor this_object);
AL_LINKAGE void AL_FUNCTION deleteALDecompressor(hALDecompressor this_object);

AL_LINKAGE void AL_FUNCTION ALCompressorClearError(hALCompressor this_object);
AL_LINKAGE int AL_FUNCTION ALCompressorSetError(hALCompressor this_object,
                                                int error,
                                                char AL_DLL_FAR *text);
AL_LINKAGE int AL_FUNCTION ALCompressorGetStatusCode(hALCompressor this_object);
AL_LINKAGE char AL_DLL_FAR * AL_FUNCTION ALCompressorGetStatusString(hALCompressor this_object);
AL_LINKAGE char AL_DLL_FAR * AL_FUNCTION ALCompressorGetStatusDetail(hALCompressor this_object);
AL_LINKAGE int AL_FUNCTION ALCompressorGetTypeCode(hALCompressor this_object);
AL_LINKAGE char AL_DLL_FAR * AL_FUNCTION ALCompressorGetTypeString(hALCompressor this_object);
AL_LINKAGE void AL_FUNCTION ALDecompressorClearError(hALDecompressor this_object);
AL_LINKAGE int AL_FUNCTION ALDecompressorSetError(hALDecompressor this_object,
                                                  int error,
                                                  char AL_DLL_FAR *text);
AL_LINKAGE int AL_FUNCTION ALDecompressorGetStatusCode(hALDecompressor this_object);
AL_LINKAGE char AL_DLL_FAR * AL_FUNCTION ALDecompressorGetStatusString(hALDecompressor this_object);
AL_LINKAGE char AL_DLL_FAR * AL_FUNCTION ALDecompressorGetStatusDetail(hALDecompressor this_object);
AL_LINKAGE int AL_FUNCTION ALDecompressorGetTypeCode(hALDecompressor this_object);
AL_LINKAGE char AL_DLL_FAR * AL_FUNCTION ALDecompressorGetTypeString(hALDecompressor this_object);

#ifdef __cplusplus
};
#endif

#endif // #ifndef __ENGN_H__

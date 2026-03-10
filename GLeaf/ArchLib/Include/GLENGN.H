/*= Archive Library v2.12 ================================================
 |
 |  H\GLENGN.H
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  GLENGN.H contains the class declaration for ALGlCompressor and
 |  ALGlDecompressor.  These are the two classes that together impelement
 |  the complete greenleaf compression engine.
 |
 +- Classes --------------------------------------------------------------
 |  ALGlCompressor
 |  ALGlDecompressor
 |
 +=======================================================================*/

#ifndef __GRENENGN_H__
#define __GRENENGN_H__

#include "arclib.h"

#ifdef __cplusplus

#include "engn.h"

/*- Class Name -----------------------------------------------------------
 |  ALGlCompressor
 |
 +- Version --------------------------------------------------------------
 |  2.11.00 01 Oct 1996 12:00
 |
 +- Description ----------------------------------------------------------
 |  ALGlCompressor is the Greenleaf proprietary compression
 |  engine.  Compression engines have a simple API, so there aren't
 |  too many functions.  This class has two data members that
 |  are initialized in the constructor.  One of them, the compression level,
 |  has to be saved with the data in order for decompression to work
 |  properly.  It is saved and read with the virtual functions
 |  ReadEngineData() and WriteEngineData(), using a single short
 |  in the archive directory.
 |
 +- Data Members ---------------------------------------------------------
 |  miCompressionLevel      - This is the compression level that the
 |                            compressor will attempt to use, selected
 |                            from one of the five settings found in
 |                            the enumerated type in ALDEFS.H.  If the
 |                            file is incompressible and the engine
 |                            performs a straight binary copy this value
 |                            changes to AL_GREENLEAF_COPY.
 |
 |  miFailUncompressible    - This data member is used to flag the
 |                            action the compressor takes if a file
 |                            turns out to be incompressible.  If this
 |                            member is set, the engine will stop compressing,
 |                            seek back to the start of the file, and
 |                            just copy the data.  If this member is clear,
 |                            we don't ever check to see if the file is
 |                            compressing properly.
 |
 |
 +- Member Functions -----------------------------------------------------
 |  ALGlCompressor()        - The constructor.
 |  ~ALGlCompressor()       - The virtual destructor.
 |  operator new()          - The memory allocation operator, which is
 |                            only used when the library is in a DLL.
 |  WriteEngineData()       - The virtual function that writes private
 |                            configuration data.  For this class, this
 |                            function writes out a single short int
 |                            containing the compression level.
 |  ReadEngineData()        - The complement for the above function, used
 |                            during extraction.
 |  Compress()              - The routine that actually performs the
 |                            compression.
 |  Clone()                 - If a compression engine is added to a
 |                            toolkit, this function can be called to
 |                            create a new compressor.
 |  CompressionLevel()      - A member function that returns the compression
 |                            level for the engine.
 |
 +- Revision History -----------------------------------------------------
 |
 +-----------------------------------------------------------------------*/

// Tag public class
class AL_CLASS_TYPE ALGlCompressor : public ALCompressor {

// declarations, friends, constructors, destructors

 public :
#ifdef AL_FLAT_MODEL
  AL_PROTO ALGlCompressor(short int compression_level = AL_GREENLEAF_LEVEL_4,
                          short int fail_uncompressible = 0);
#else
  AL_PROTO ALGlCompressor(short int compression_level = AL_GREENLEAF_LEVEL_2,
                          short int fail_uncompressible = 0);
#endif
  virtual AL_PROTO ~ALGlCompressor();
#if defined(AL_USING_DLL) || defined(AL_BUILDING_DLL)
  void AL_DLL_FAR * AL_PROTO operator new(size_t size);
#endif

// The copy constructor and assignment operator do not exist.  I define
// them here to prevent the compiler from creating default versions.

 protected :
  AL_PROTO ALGlCompressor(ALGlCompressor AL_DLL_FAR &);
  ALGlCompressor AL_DLL_FAR & AL_PROTO operator=(ALGlCompressor AL_DLL_FAR & rhs);

// member functions

 protected :
  virtual int AL_PROTO WriteEngineData(ALStorage AL_DLL_FAR * archive);
  virtual int AL_PROTO ReadEngineData(ALStorage AL_DLL_FAR * archive);

 public :
  virtual ALCompressor AL_DLL_FAR * AL_PROTO Clone(int engine_type) const;
  virtual int AL_PROTO Compress(ALStorage AL_DLL_FAR &input,
                                ALStorage AL_DLL_FAR &output);
  short int AL_PROTO CompressionLevel();

// data members

 protected :
  short int miCompressionLevel;
  short int miFailUncompressible;

 public :
  AL_CLASS_TAG(_ALGlCompressorTag);
};

/*- Class Name -----------------------------------------------------------
 |  ALGlDecompressor
 |
 +- Version --------------------------------------------------------------
 |  2.11.00 01 Oct 1996 12:00
 |
 +- Description ----------------------------------------------------------
 |  ALGlDecompressor is the Greenleaf proprietary decompression
 |  engine.  Decompression engines have a simple API, so there aren't
 |  too many functions.  This class has a single data member (the
 |  compression level) that is initialized in the constructor.  The
 |  compression level has to be the same as it was when in the compressor
 |  when the file was compressed.
 |
 +- Data Members ---------------------------------------------------------
 |  miCompressionLevel    - This is the compression level that the
 |                          decompressor will use, selected
 |                          from one of the five settings found in
 |                          the enumerated type in ALDEFS.H.  This must
 |                          match the compression level that was used
 |                          by the ALGlCompressor engine when the
 |                          object was first compressed.
 |
 +- Member Functions -----------------------------------------------------
 |  ALGlDecompressor()      - The constructor.
 |  ~ALGlDecompressor()     - The virtual destructor.
 |  operator new()          - The memory allocation operator, which is
 |                            only used when the library is in a DLL.
 |  WriteEngineData()       - The virtual function that writes private
 |                            configuration data.  For this class, this
 |                            function writes out a single short int
 |                            containing the compression level.
 |  ReadEngineData()        - The complement for the above function, used
 |                            during extraction.
 |  Decompress()            - The routine that actually performs the
 |                            decompression.
 |  Clone()                 - If a compression engine is added to a
 |                            toolkit, this function can be called to
 |                            create a new compressor.
 |  CompressionLevel()      - A member function that returns the compression
 |                            level for the engine.
 |
 +- Revision History -----------------------------------------------------
 |
 +-----------------------------------------------------------------------*/

// Tag public class
class AL_CLASS_TYPE ALGlDecompressor : public ALDecompressor {

// declarations, friends, constructors, destructors

 public :
#if defined(AL_FLAT_MODEL) || defined(AL_FLAT_MODEL)
  AL_PROTO ALGlDecompressor(short int compression_level = AL_GREENLEAF_LEVEL_4);
#else
  AL_PROTO ALGlDecompressor(short int compression_level = AL_GREENLEAF_LEVEL_2);
#endif
  virtual AL_PROTO ~ALGlDecompressor();
#if defined(AL_USING_DLL) || defined(AL_BUILDING_DLL)
  void AL_DLL_FAR * AL_PROTO operator new(size_t size);
#endif

// The copy constructor and assignment operator do not exist.  I define
// them here to prevent the compiler from creating default versions.

 protected :
  AL_PROTO ALGlDecompressor(ALGlDecompressor AL_DLL_FAR &);
  ALGlDecompressor AL_DLL_FAR & AL_PROTO operator=(ALGlDecompressor AL_DLL_FAR & rhs);

// member functions

 protected :
  virtual int AL_PROTO WriteEngineData(ALStorage AL_DLL_FAR * archive);
  virtual int AL_PROTO ReadEngineData(ALStorage AL_DLL_FAR * archive);

 public :
  virtual ALDecompressor AL_DLL_FAR * AL_PROTO Clone(int engine_type) const;
  virtual int AL_PROTO Decompress(ALStorage AL_DLL_FAR &input,
                                  ALStorage AL_DLL_FAR &output,
                                  long compressed_length = -1L);
  short int AL_PROTO CompressionLevel();

// data members

 public :
  short int miCompressionLevel;
  AL_CLASS_TAG(_ALGlDecompressorTag);
};

#include "glengn.inl"

extern "C" {

#endif // #ifdef __cplusplus

AL_LINKAGE short int AL_FUNCTION ALGlCompressorLevel(hALCompressor this_object);
AL_LINKAGE short int AL_FUNCTION ALGlDecompressorLevel(hALDecompressor this_object);
AL_LINKAGE hALCompressor AL_FUNCTION newALGlCompressor(short int compression_level,
                                                       short int fail_uncompressible);
AL_LINKAGE hALDecompressor AL_FUNCTION newALGlDecompressor(short int compression_level);

#ifdef __cplusplus
};
#endif

#endif // #ifndef __GRENENGN_H__

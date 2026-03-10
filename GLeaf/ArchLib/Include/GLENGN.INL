/*= Archive Library v2.12 ================================================
 |
 |  H\GLENGN.INL
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  Inline functions for class ALGlCompressor and ALGlDecompressor.
 |
 +- Functions ------------------------------------------------------------
 |  ALGlCompressor::CompressionLevel()
 |  ALGlDecompressor::CompressionLevel()
 |
 +=======================================================================*/

/*- Function Name --------------------------------------------------------
 |  ALGlCompressor::CompressionLevel()
 |
 +- Version --------------------------------------------------------------
 |  2.11.00 01 Oct 1996 12:00
 |
 +- Description ----------------------------------------------------------
 |  The Greenleaf Compressor has a compression level member that
 |  dictates how much memory the compressor will devote to its
 |  job. The more memory, the better compression you get.  The number
 |  returned here will normally be between AL_GREENLEAF_LEVEL_0 and
 |  AL_GREENLEAF_LEVEL_4.  A value of AL_GREENLEAF_COPY can also be
 |  used, which gives no compressino at all.
 |
 |  This is a real simple function, so it will normally be implemented as
 |  an inline function.  The rest of the supported languages don't have
 |  this luxury.
 |
 +- Arguments ------------------------------------------------------------
 |  None
 |
 +- Return Value ---------------------------------------------------------
 |  A short integer that contains the current compression level for the
 |  Greenleaf Engine.  If a compression pass fails because a file is
 |  incompressible, this guy will return AL_GREENLEAF_COPY, otherwise
 |  it should normally be one of AL_GREENLEAF_LEVEL_*.
 |
 +- Revision History -----------------------------------------------------
 |
 +-----------------------------------------------------------------------*/

// Tag public function
inline short int AL_INLINE_PROTO
ALGlCompressor::CompressionLevel()
{
 return miCompressionLevel;
}

/*- Function Name --------------------------------------------------------
 |  ALGlDecompressor::CompressionLevel()
 |
 +- Version --------------------------------------------------------------
 |  2.11.00 01 Oct 1996 12:00
 |
 +- Description ----------------------------------------------------------
 |  The Greenleaf Compressor has a compression level member that
 |  dictates how much memory the compressor will devote to its
 |  job. The more memory, the better compression you get.  The number
 |  returned here will normally be between AL_GREENLEAF_LEVEL_0 and
 |  AL_GREENLEAF_LEVEL_4.  A value of AL_GREENLEAF_COPY can also be
 |  used, which gives no compressino at all.
 |
 |  When you create a decompressor, you need to match up the compression
 |  level in the decompressor with the level that the compressor used to
 |  create the data.  In ArchiveLib, this is done automatically for all
 |  Archiving functions, as well as the Compressed object.  If you are
 |  performing raw compression on blocks of data, you will need to ensure
 |  manually that the two levels are the same when creating the engine.
 |
 |  This is a real simple function, so it will normally be implemented as
 |  an inline function.  The rest of the supported languages don't have
 |  this luxury.
 |
 +- Arguments ------------------------------------------------------------
 |  None
 |
 +- Return Value ---------------------------------------------------------
 |  A short integer that contains the current compression level for the
 |  Greenleaf Engine.  If a compression pass fails because a file is
 |  incompressible, this guy will return AL_GREENLEAF_COPY, otherwise
 |  it should normally be one of AL_GREENLEAF_LEVEL_*.
 |
 +- Revision History -----------------------------------------------------
 |
 +-----------------------------------------------------------------------*/

// Tag public function
inline short int AL_INLINE_PROTO
ALGlDecompressor::CompressionLevel()
{
 return miCompressionLevel;
}

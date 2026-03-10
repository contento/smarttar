/*= Archive Library v2.12 ================================================
 |
 |  H\STOR.INL
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  Inline functions for the ALStorage class.
 |
 +- Functions ------------------------------------------------------------
 |  ALStorage::GetSize()
 |  ALStorage::IsOpen()
 |  ALStorage::ClearError()
 |  ALStorage::ReadChar()
 |  ALStorage::WriteChar()
 |
 +=======================================================================*/

/*- Function Name --------------------------------------------------------
 |  ALStorage::GetSize()
 |
 +- Version --------------------------------------------------------------
 |  2.11.00 01 Oct 1996 12:00
 |
 +- Description ----------------------------------------------------------
 |  Once an ALStorage object has been opened and closed, you can determine
 |  what size it was by looking at the mlSize member.  This member is
 |  protected so people can't jack with it.
 |
 |  Be sure to note that just creating a storage object doesn't initialize
 |  the mlSize member to its correct value.   You have to open the file and
 |  close it. If you are creating the file, you need to do all your output
 |  as well.
 |
 |  There is an important exception to this, however.  When you read in
 |  a directory from an Archive, the size (and other stats) of the stored
 |  files are loaded and ready for perusal.
 |
 +- Arguments ------------------------------------------------------------
 |  None
 |
 +- Return Value ---------------------------------------------------------
 |  A long integer containing the file length.
 |
 +- Revision History -----------------------------------------------------
 |
 +-----------------------------------------------------------------------*/

inline long AL_INLINE_PROTO
// Tag public function
ALStorage::GetSize() const
{
 return mlSize;
}

/*- Function Name --------------------------------------------------------
 |  ALStorage::IsOpen()
 |
 +- Version --------------------------------------------------------------
 |  2.11.00 01 Oct 1996 12:00
 |
 +- Description ----------------------------------------------------------
 |  Sometimes you might want to know if a storage object is open.  This
 |  function does just that.  It relies on the fact that an open storage
 |  object has its mpcBuffer allocated to determine this status.
 |
 +- Arguments ------------------------------------------------------------
 |  None
 |
 +- Return Value ---------------------------------------------------------
 |  A boolean value telling whether the file is open.
 |
 +- Revision History -----------------------------------------------------
 |
 +-----------------------------------------------------------------------*/

inline int AL_INLINE_PROTO
// Tag public function
ALStorage::IsOpen()
{
 return mpcBuffer != 0;
}

/*- Function Name --------------------------------------------------------
 |  ALStorage::ClearError()
 |
 +- Version --------------------------------------------------------------
 |  2.11.00 01 Oct 1996 12:00
 |
 +- Description ----------------------------------------------------------
 |  An ALStorage object carries around a status object in its mStatus
 |  member.  For various reasons, this member might get set to an error
 |  condition.  Error conditions aren't cleared automatically by the library,
 |  so the user will have to manually clear it with a call to this function.
 |
 |  This is a real simple function, so in C++ it will be implemented as
 |  an inline function.  The rest of the supported languages don't have
 |  this luxury.
 |
 +- Arguments ------------------------------------------------------------
 |  None
 |
 +- Return Value ---------------------------------------------------------
 |  None
 |
 +- Revision History -----------------------------------------------------
 |
 +-----------------------------------------------------------------------*/

inline void AL_INLINE_PROTO
// Tag public function
ALStorage::ClearError()
{
 mStatus.SetError(AL_SUCCESS, 0);
}

/*- Function Name --------------------------------------------------------
 |  ALStorage::ReadChar()
 |
 +- Version --------------------------------------------------------------
 |  2.11.00 01 Oct 1996 12:00
 |
 +- Description ----------------------------------------------------------
 |  This is an inline function that is able to quickly do buffered I/O.
 |  By utilizing an I/O buffer we can make this routine very fast, since
 |  it doesn't have to call a virtual function.  The virtual function
 |  only has to be called when LoadBuffer() gets called.
 |
 |  Different compilers have different abilities to make this code inline,
 |  so sometimes it needs to be tinkered with.  If you see anything in here
 |  that looks funny, that probably explains why.
 |
 +- Arguments ------------------------------------------------------------
 |  None
 |
 +- Return Value ---------------------------------------------------------
 |  Either the next character available from the I/O buffer, or
 |  AL_END_OF_FILE.
 |
 +- Revision History -----------------------------------------------------
 |
 +-----------------------------------------------------------------------*/

inline int AL_INLINE_PROTO
// Tag public function
ALStorage::ReadChar()
{
 int result;

// Can't read if I've done a write!
 AL_ASSERT(muWriteIndex == 0, "ReadChar(): Attempt to read while in write mode");
 result = muBufferValidData - muReadIndex;
 if (result <= 0)
  result = LoadBuffer(mlFilePointer);
// Potential disaster.
 AL_ASSERT(mpcBuffer != 0, "ReadChar(): Attempt to read from closed file");
 if (result < 0)
  return result;
 else
  return mpcBuffer[muReadIndex++] & 0xff;
}

/*- Function Name --------------------------------------------------------
 |  ALStorage::WriteChar()
 |
 +- Version --------------------------------------------------------------
 |  2.11.00 01 Oct 1996 12:00
 |
 +- Description ----------------------------------------------------------
 |  This is an inline function that is able to quickly do buffered I/O.
 |  By utilizing an I/O buffer we can make this routine very fast, since
 |  it doesn't have to call a virtual function.  The virtual function
 |  only has to be called when FlushBuffer() gets called.
 |
 |  Different compilers have different abilities to make this code inline,
 |  so sometimes it needs to be tinkered with.  If you see anything in here
 |  that looks funny, that probably explains why.
 |
 +- Arguments ------------------------------------------------------------
 |  char             -  The character that is going to be written.
 |
 +- Return Value ---------------------------------------------------------
 |  Either the character that we just wrote out, or an error < AL_SUCCESS.
 |
 +- Revision History -----------------------------------------------------
 |
 +-----------------------------------------------------------------------*/

inline int AL_INLINE_PROTO
// Tag public function
ALStorage::WriteChar(int c)
{
 int result;

 AL_ASSERT(mpcBuffer != 0, "WriteChar(): Attempt to write to closed file");
 result = muBufferSize - muWriteIndex;
 if (result <= 0)
  result = FlushBuffer();
 if (result < 0)
  return mStatus;
 else
  return mpcBuffer[muWriteIndex++] = (char) c;
}

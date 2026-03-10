/*= Archive Library v2.12 ================================================
 |
 |  H\ARCENTRY.INL
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  Inline functions for class ALEntry.
 |
 +- Functions ------------------------------------------------------------
 |  ALEntry::GetCompressedSize()
 |  ALEntry::GetCrc32()
 |  ALEntry::GetComment()
 |  ALEntry::SetMark()
 |  ALEntry::ClearMark()
 |  ALEntry::SetMarkState()
 |  ALEntry::GetMark()
 |  ALEntry::GetCompressedObjectPosition()
 |  ALEntry::GetEncrypted()
 |  ALEntry::SetEncrypted()
 |  ALEntry::ClearEncrypted()
 |
 +=======================================================================*/

/*- Function Name --------------------------------------------------------
 |  ALEntry::GetCompressedSize()
 |
 +- Version --------------------------------------------------------------
 |  2.11.00 01 Oct 1996 12:00
 |
 +- Description ----------------------------------------------------------
 |  GetCompressedSize() is used to determine the compressed size of an
 |  object referrred to in an ALEntry object.  Note that the compressed
 |  size will only be available after executing an archive function that
 |  stores this object in an archive, or after reading the directory.  You
 |  cannot use it to predict a compressed size in advance of the actual
 |  compression procedure!
 |
 |  This is a real simple function, so in C++ it will normally be implemented
 |  as inline.  The other supported languages will still have to call the
 |  normal C function, found in CXL_ENTR.CPP.
 |
 +- Arguments ------------------------------------------------------------
 |  None
 |
 +- Return Value ---------------------------------------------------------
 |  A long value, indicating the compressed size.  Note that this information
 |  will only be available after reading a directory, or performing an
 |  operation that compresses the file.
 |
 +- Revision History -----------------------------------------------------
 |
 +-----------------------------------------------------------------------*/

// Tag public function
inline long AL_INLINE_PROTO
ALEntry::GetCompressedSize() const
{
 return mlCompressedSize;
}

/*- Function Name --------------------------------------------------------
 |  ALEntry::GetCrc32()
 |
 +- Version --------------------------------------------------------------
 |  2.11.00 01 Oct 1996 12:00
 |
 +- Description ----------------------------------------------------------
 |  GetCrc32() is used to read the previously calculated CRC-32 of an
 |  object referrred to in an ALEntry object.  Note that the CRC will only
 |  be available after executing an archive function that stores this
 |  object in an archive, or after reading the directory.
 |
 |  This is a real simple function, so in C++ it will normally be implemented
 |  as inline.  The other supported languages will still have to call the
 |  normal C function, found in CXL_ENTR.CPP.
 |
 +- Arguments ------------------------------------------------------------
 |  None
 |
 +- Return Value ---------------------------------------------------------
 |  A long value, containing the CRC-32.  Note that this information
 |  will only be available after reading a directory, or performing an
 |  operation that compresses the file.
 |
 +- Revision History -----------------------------------------------------
 |
 +-----------------------------------------------------------------------*/

// Tag public function
inline long AL_INLINE_PROTO
ALEntry::GetCrc32() const
{
 return mlCrc32;
}

/*- Function Name --------------------------------------------------------
 |  ALEntry::GetComment()
 |
 +- Version --------------------------------------------------------------
 |  2.11.00 01 Oct 1996 12:00
 |
 +- Description ----------------------------------------------------------
 |  GetComment() is used to retrieve the comment associated with an ALEntry
 |  object.  Comments can be set manually at run time with the SetComment()
 |  function, or they can be read into the list when reading the directory.
 |
 |  This is a real simple function, so in C++ it will normally be implemented
 |  as inline.  The other supported languages will still have to call the
 |  normal C function, found in CXL_ENTR.CPP.
 |
 +- Arguments ------------------------------------------------------------
 |  None
 |
 +- Return Value ---------------------------------------------------------
 |  A string containing the comment.  For C++, C, and Delphi programmers,
 |  this will be a pointer to a null-terminated C-Style character array.
 |  The string itself is actually stored in the ALEntry object, and YOU
 |  MUST NOT MODIFY IT!!!  C++ will prevent you from making this horrible
 |  faux pas, but C and Delphi programs are simply on their honor.
 |
 |  C-style character strings aren't very fun to work with in VB, so the
 |  VB version of this function actually returns a true VB string.  Since
 |  this VB string has no connection to the ALEntry object, you are free
 |  to jack with it to your heart's delight.
 |
 +- Revision History -----------------------------------------------------
 |
 +-----------------------------------------------------------------------*/

// Tag public function
inline const char AL_DLL_FAR * AL_INLINE_PROTO
ALEntry::GetComment()
{
 return mszComment;
}

/*- Function Name --------------------------------------------------------
 |  ALEntry::SetMark()
 |
 +- Version --------------------------------------------------------------
 |  2.11.00 01 Oct 1996 12:00
 |
 +- Description ----------------------------------------------------------
 |  Every ALEntry object has a mark associated with it.  Archiving
 |  functions such as Create(), Delete(), etc., only perform their
 |  operations on entries whose marks are set.  By default, entries
 |  are created with their marks set, but various operations such
 |  as ClearMark() can be used to clear them.  After it's been cleared,
 |  you might want to use this function to set it again.
 |
 |  This is a real simple function, so in C++ it will normally be implemented
 |  as inline.  The other supported languages will still have to call the
 |  normal C function, found in CXL_ENTR.CPP.
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

// Tag public function
inline void AL_INLINE_PROTO
ALEntry::SetMark()
{
 miMark = 1;
}

/*- Function Name --------------------------------------------------------
 |  ALEntry::SetEncrypted()
 |
 +- Version --------------------------------------------------------------
 |  2.11.00 01 Oct 1996 12:00
 |
 +- Description ----------------------------------------------------------
 |  Every ALEntry object has an encrypted bit associated with it.
 |  As the archive directory is read in, each entry will have the
 |  encrypted bit set if the file is encrypted.
 |
 |  This is a real simple function, so in C++ it will normally be implemented
 |  as inline.  This function lets you modify the bit.  Is this a useful
 |  thing?  Not sure.
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

// Tag public function
inline void AL_INLINE_PROTO
ALEntry::SetEncrypted()
{
 miEncrypted = 1;
}

/*- Function Name --------------------------------------------------------
 |  ALEntry::ClearMark()
 |
 +- Version --------------------------------------------------------------
 |  2.11.00 01 Oct 1996 12:00
 |
 +- Description ----------------------------------------------------------
 |  Every ALEntry object has a mark associated with it.  Archiving
 |  functions such as Create(), Delete(), etc., only perform their
 |  operations on entries whose marks are set.  By default, entries
 |  are created with their marks set.  If you want to exclude an entry
 |  from some archiving operation, you can call this function to
 |  clear its mark.
 |
 |  This is a real simple function, so in C++ it will normally be implemented
 |  as inline.  The other supported languages will still have to call the
 |  normal C function, found in CXL_ENTR.CPP.
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

// Tag public function
inline void AL_INLINE_PROTO
ALEntry::ClearMark()
{
 miMark = 0;
}

/*- Function Name --------------------------------------------------------
 |  ALEntry::ClearEncrypted()
 |
 +- Version --------------------------------------------------------------
 |  2.11.00 01 Oct 1996 12:00
 |
 +- Description ----------------------------------------------------------
 |  Every ALEntry object has an encrypted bit associated with it.
 |  As the archive directory is read in, each entry will have the
 |  encrypted bit set if the file is encrypted.
 |
 |  This is a real simple function, so in C++ it will normally be implemented
 |  as inline.  This function lets you modify the bit.  Is this a useful
 |  thing?  Not sure.
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

// Tag public function
inline void AL_INLINE_PROTO
ALEntry::ClearEncrypted()
{
 miEncrypted = 0;
}

/*- Function Name --------------------------------------------------------
 |  ALEntry::SetMarkState()
 |
 +- Version --------------------------------------------------------------
 |  2.11.00 01 Oct 1996 12:00
 |
 +- Description ----------------------------------------------------------
 |  Every ALEntry object has a mark associated with it.  Archiving
 |  functions such as Create(), Delete(), etc., only perform their
 |  operations on entries whose marks are set.  By default, entries
 |  are created with their marks set.  This function can be used to
 |  either set or clear the mark associated with an ALEntry object.
 |
 |  This is a real simple function, so in C++ it will normally be implemented
 |  as inline.  The other supported languages will still have to call the
 |  normal C function, found in CXL_ENTR.CPP.
 |
 +- Arguments ------------------------------------------------------------
 |  int         -   The desired new state of the ALEntry object.  0 is used
 |                  to clear the mark, and any non-zero value is used to
 |                  set it.
 |
 +- Return Value ---------------------------------------------------------
 |  None
 |
 +- Revision History -----------------------------------------------------
 |
 +-----------------------------------------------------------------------*/

// Tag public function
inline void AL_INLINE_PROTO
ALEntry::SetMarkState(short int new_state)
{
 miMark = new_state;
}

/*- Function Name --------------------------------------------------------
 |  ALEntry::GetMark()
 |
 +- Version --------------------------------------------------------------
 |  2.11.00 01 Oct 1996 12:00
 |
 +- Description ----------------------------------------------------------
 |  Every ALEntry object has a mark associated with it.  Archiving
 |  functions such as Create(), Delete(), etc., only perform their
 |  operations on entries whose marks are set.  By default, entries
 |  are created with their marks set.  This function can be used to
 |  either set or clear the mark associated with an ALEntry object.
 |
 |  This is a real simple function, so in C++ it will normally be implemented
 |  as inline.  The other supported languages will still have to call the
 |  normal C function, found in CXL_ENTR.CPP.
 |
 +- Arguments ------------------------------------------------------------
 |  None
 |
 +- Return Value ---------------------------------------------------------
 |  Value of miMark.
 |
 +- Revision History -----------------------------------------------------
 |
 +-----------------------------------------------------------------------*/

// Tag public function
inline int AL_INLINE_PROTO
ALEntry::GetMark()
{
 return miMark;
}

/*- Function Name --------------------------------------------------------
 |  ALEntry::GetEncrypted()
 |
 +- Version --------------------------------------------------------------
 |  2.11.00 01 Oct 1996 12:00
 |
 +- Description ----------------------------------------------------------
 |  Every ALEntry object has an encrypted bit associated with it.
 |  As the archive directory is read in, each entry will have the
 |  encrypted bit set if the file is encrypted.
 |
 |  This is a real simple function, so in C++ it will normally be implemented
 |  as inline.  This function lets you modify the bit.  Is this a useful
 |  thing?  Not sure.
 |
 +- Arguments ------------------------------------------------------------
 |  None
 |
 +- Return Value ---------------------------------------------------------
 |  Value of miEncrypted.
 |
 +- Revision History -----------------------------------------------------
 |
 +-----------------------------------------------------------------------*/

// Tag public function
inline int AL_INLINE_PROTO
ALEntry::GetEncrypted()
{
 return miEncrypted;
}

/*- Function Name --------------------------------------------------------
 |  ALEntry::GetCompressedObjectPosition()
 |
 +- Version --------------------------------------------------------------
 |  2.11.00 01 Oct 1996 12:00
 |
 +- Description ----------------------------------------------------------
 |  GetCompressedObjectPosition() is used to determine the position of an
 |  object in an archive object.  Note that the compressed object position
 |  will only be available after executing an archive function that stores
 |  this object in an archive, or after reading the directory.
 |
 |  This is a real simple function, so in C++ it will normally be implemented
 |  as inline.  The other supported languages will still have to call the
 |  normal C function, found in CXL_ENTR.CPP.
 |
 +- Arguments ------------------------------------------------------------
 |  None
 |
 +- Return Value ---------------------------------------------------------
 |  A long value, indicating the compressed size.  Note that this information
 |  will only be available after reading a directory, or performing an
 |  operation that compresses the file.
 |
 +- Revision History -----------------------------------------------------
 |
 +-----------------------------------------------------------------------*/

// Tag public function
inline long AL_INLINE_PROTO
ALEntry::GetCompressedObjectPosition() const
{
 return mlCompressedObjectPosition;
}

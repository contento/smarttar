/*= Archive Library v2.12 ================================================
 |
 |  H\FILEATTR.INL
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  Inline functions for class ALFileAttriutes.
 |
 +- Functions ------------------------------------------------------------
 |  ALFileAttributes::ReadOnly()
 |  ALFileAttributes::System()
 |  ALFileAttributes::Hidden()
 |  ALFileAttributes::Archive()
 |  ALFileAttributes::Directory()
 |
 +=======================================================================*/

/*- Function Name --------------------------------------------------------
 |  ALFileAttributes::ReadOnly()
 |
 +- Version --------------------------------------------------------------
 |  2.11.00 01 Oct 1996 12:00
 |
 +- Description ----------------------------------------------------------
 |  Every storage object has a set of attributes that correspond to the
 |  attributes kept by the file system.  These might not make sense for
 |  objects such as ALMemory, but they are kept on hand anyway.  This
 |  function is one of a collection that is used to read those attribute
 |  bits.
 |
 +- Arguments ------------------------------------------------------------
 |  None
 |
 +- Return Value ---------------------------------------------------------
 |  An integer that will be set to 0 if the ReadOnly bit of the storage
 |  object is clear, or 1 if it is set.
 |
 +- Revision History -----------------------------------------------------
 |
 +-----------------------------------------------------------------------*/

// Tag public function
inline int AL_INLINE_PROTO
ALFileAttributes::ReadOnly()
{
 return miReadOnly;
}

/*- Function Name --------------------------------------------------------
 |  ALFileAttributes::System()
 |
 +- Version --------------------------------------------------------------
 |  2.11.00 01 Oct 1996 12:00
 |
 +- Description ----------------------------------------------------------
 |  Every storage object has a set of attributes that correspond to the
 |  attributes kept by the file system.  These might not make sense for
 |  objects such as ALMemory, but they are kept on hand anyway.  This
 |  function is one of a collection that is used to read those attribute
 |  bits.
 |
 +- Arguments ------------------------------------------------------------
 |  None
 |
 +- Return Value ---------------------------------------------------------
 |  An integer that will be set to 0 if the System bit of the storage
 |  object is clear, or 1 if it is set.
 |
 +- Revision History -----------------------------------------------------
 |
 +-----------------------------------------------------------------------*/

// Tag public function
inline int AL_INLINE_PROTO
ALFileAttributes::System()
{
 return miSystem;
}

/*- Function Name --------------------------------------------------------
 |  ALFileAttributes::Hidden()
 |
 +- Version --------------------------------------------------------------
 |  2.11.00 01 Oct 1996 12:00
 |
 +- Description ----------------------------------------------------------
 |  Every storage object has a set of attributes that correspond to the
 |  attributes kept by the file system.  These might not make sense for
 |  objects such as ALMemory, but they are kept on hand anyway.  This
 |  function is one of a collection that is used to read those attribute
 |  bits.
 |
 +- Arguments ------------------------------------------------------------
 |  None
 |
 +- Return Value ---------------------------------------------------------
 |  An integer that will be set to 0 if the Hidden bit of the storage
 |  object is clear, or 1 if it is set.
 |
 +- Revision History -----------------------------------------------------
 |
 +-----------------------------------------------------------------------*/

// Tag public function
inline int AL_INLINE_PROTO
ALFileAttributes::Hidden()
{
 return miHidden;
}

/*- Function Name --------------------------------------------------------
 |  ALFileAttributes::Archive()
 |
 +- Version --------------------------------------------------------------
 |  2.11.00 01 Oct 1996 12:00
 |
 +- Description ----------------------------------------------------------
 |  Every storage object has a set of attributes that correspond to the
 |  attributes kept by the file system.  These might not make sense for
 |  objects such as ALMemory, but they are kept on hand anyway.  This
 |  function is one of a collection that is used to read those attribute
 |  bits.
 |
 +- Arguments ------------------------------------------------------------
 |  None
 |
 +- Return Value ---------------------------------------------------------
 |  An integer that will be set to 0 if the Archive bit of the storage
 |  object is clear, or 1 if it is set.
 |
 +- Revision History -----------------------------------------------------
 |
 +-----------------------------------------------------------------------*/

// Tag public function
inline int AL_INLINE_PROTO
ALFileAttributes::Archive()
{
 return miArchive;
}

/*- Function Name --------------------------------------------------------
 |  ALFileAttributes::Directory()
 |
 +- Version --------------------------------------------------------------
 |  2.11.00 01 Oct 1996 12:00
 |
 +- Description ----------------------------------------------------------
 |  Every storage object has a set of attributes that correspond to the
 |  attributes kept by the file system.  These might not make sense for
 |  objects such as ALMemory, but they are kept on hand anyway.  This
 |  function is one of a collection that is used to read those attribute
 |  bits.
 |
 |  This attribute bit is a little different than all the other bits
 |  in the attribute structure.  If an object has the directory bit set,
 |  it is more like a placeholder than anything else, and by definition
 |  it will have a length of 0.  When the expansion routines are running,
 |  they take special action when creating a file whose directory bit is
 |  set.
 |
 |  At this time, ArchiveLib won't create any entries in an archive with
 |  the directory bit set.  However, it will attempt to create directories
 |  it finds in PK archives.
 |
 +- Arguments ------------------------------------------------------------
 |  None
 |
 +- Return Value ---------------------------------------------------------
 |  An integer that will be set to 0 if the Directory bit of the storage
 |  object is clear, or 1 if it is set.
 |
 +- Revision History -----------------------------------------------------
 |
 +-----------------------------------------------------------------------*/

// Tag public function
inline int AL_INLINE_PROTO
ALFileAttributes::Directory()
{
 return miDirectory;
}

// The rest of these functions were in use once, when ArchiveLib briefly
// had a history on UNIX boxes.  Now, they are sad relics of greater times
// gone by.  As such, they aren't documented, and they aren't supported.
// Even worse, they don't have any C/VB/Delphi wrapper functions.

// Tag public function
inline int AL_INLINE_PROTO
ALFileAttributes::UnixBitsPresent()
{
 return miUnixBitsPresent;
}

// Tag public function
inline int AL_INLINE_PROTO
ALFileAttributes::UserRead()
{
 return miUserRead;
}

// Tag public function
inline int AL_INLINE_PROTO
ALFileAttributes::UserWrite()
{
 return miUserWrite;
}

// Tag public function
inline int AL_INLINE_PROTO
ALFileAttributes::UserExecute()
{
 return miUserExecute;
}

// Tag public function
inline int AL_INLINE_PROTO
ALFileAttributes::OtherRead()
{
 return miOtherRead;
}

// Tag public function
inline int AL_INLINE_PROTO
ALFileAttributes::OtherWrite()
{
 return miOtherWrite;
}

// Tag public function
inline int AL_INLINE_PROTO
ALFileAttributes::OtherExecute()
{
 return miOtherExecute;
}

// Tag public function
inline int AL_INLINE_PROTO
ALFileAttributes::GroupRead()
{
 return miGroupRead;
}

// Tag public function
inline int AL_INLINE_PROTO
ALFileAttributes::GroupWrite()
{
 return miGroupWrite;
}

// Tag public function
inline int AL_INLINE_PROTO
ALFileAttributes::GroupExecute()
{
 return miGroupExecute;
}

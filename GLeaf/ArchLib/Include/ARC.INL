/*= Archive Library v2.12 ================================================
 |
 |  H\ARC.INL
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  Inline functions for class ALArchive.
 |
 +- Functions ------------------------------------------------------------
 |  ALArchive::ClearError()
 |  ALArchive::GetComment()
 |  ALArchive::GetVersion()
 |  ALArchive::GetStorageObject()
 |
 +=======================================================================*/

/*- Function Name --------------------------------------------------------
 |  ALArchive::ClearError()
 |
 +- Version --------------------------------------------------------------
 |  2.11.00 01 Oct 1996 12:00
 |
 +- Description ----------------------------------------------------------
 |  An ALArchive object carries around a status object in its mStatus
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

// Tag public function
inline void AL_INLINE_PROTO
ALArchive::ClearError()
{
 mStatus.SetError(AL_SUCCESS, 0);
}

/*- Function Name --------------------------------------------------------
 |  ALArchive::GetComment()
 |
 +- Version --------------------------------------------------------------
 |  2.11.00 01 Oct 1996 12:00
 |
 +- Description ----------------------------------------------------------
 |  GetComment() is used to retrieve the comment associated with
 |  an ALArchive object.  Comments can be set manually at run time with
 |  the SetComment() function, or they can be read into the ALArchive
 |  object when reading the directory from the physical archive.
 |
 |  This is a real simple function, so in C++ it will normally be implemented
 |  as inline.  The other supported languages will still have to call the
 |  normal C function, found in CXL_ARCH.CPP.
 |
 +- Arguments ------------------------------------------------------------
 |  None
 |
 +- Return Value ---------------------------------------------------------
 |  A string containing the comment.  For C++, C, and Delphi programmers,
 |  this will be a pointer to a null-terminated C-Style character array.
 |  The string itself is actually stored in the ALEntry object, and YOU
 |  MUST NOT MODIFY IT!!! C++ will prevent you from doing this, but C
 |  and Delphi programs are simply on their honor.
 |
 |  C-style character strings aren't very fun to work with in VB, so the
 |  VB version of this function actually returns a true VB string.  Since
 |  this VB string has no connection to the ALArchive object, you are free
 |  to jack with it to your heart's delight.
 |
 +- Revision History -----------------------------------------------------
 |
 +-----------------------------------------------------------------------*/

// Tag public function
inline const char AL_DLL_FAR * AL_INLINE_PROTO
ALArchive::GetComment()
{
 return mComment;
}

/*- Function Name --------------------------------------------------------
 |  ALArchive::GetVersion()
 |
 +- Version --------------------------------------------------------------
 |  2.11.00 01 Oct 1996 12:00
 |
 +- Description ----------------------------------------------------------
 |  An ALArchive object carries around a version number in its miVersion
 |  member.  Sometimes we might like to see what that version number is,
 |  but it's a protected member, so we need an access function.  That's
 |  what this guy is.
 |
 |  This is a real simple function, so it will normally be implemented as
 |  inline.  The rest of the supported languages don't have
 |  this luxury.
 |
 +- Arguments ------------------------------------------------------------
 |  None
 |
 +- Return Value ---------------------------------------------------------
 |  At this time, all ALGlArchive objects are going to return 0x100 for
 |  their version number.  ALPkArchive objects should always return 0x20.
 |  Since their haven't been any upgrades of either format since ArchiveLib
 |  was first created, version numbers aren't too important yet.  As
 |  archive formats are updated, this will change.
 |
 +- Revision History -----------------------------------------------------
 |
 +-----------------------------------------------------------------------*/

// Tag public function
inline short AL_INLINE_PROTO
ALArchive::GetVersion()
{
 return miVersion;
}

/*- Function Name --------------------------------------------------------
 |  ALArchive::GetStorageObject()
 |
 +- Version --------------------------------------------------------------
 |  2.11.00 01 Oct 1996 12:00
 |
 +- Description ----------------------------------------------------------
 |  An ALArchive object always has a storage object underlying it.  The
 |  storage object is where the archived objects are actually stored.
 |  Sometimes you might want to check out this storage object, for various
 |  reasons.  You can get at it with this function.
 |
 |  This is a real simple function, so it will normally be implemented as
 |  an inline function.  The rest of the supported languages don't have
 |  this luxury.
 |
 +- Arguments ------------------------------------------------------------
 |  None
 |
 +- Return Value ---------------------------------------------------------
 |  A reference or pointer to the storage object underlying the archive.
 |
 +- Revision History -----------------------------------------------------
 |
 +-----------------------------------------------------------------------*/

// Tag public function
inline ALStorage AL_DLL_FAR * AL_INLINE_PROTO
ALArchive::GetStorageObject()
{
 return mpArchiveStorageObject;
}

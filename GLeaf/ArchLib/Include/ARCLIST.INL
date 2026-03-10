/*= Archive Library v2.12 ================================================
 |
 |  H\ARCLIST.INL
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  Inline functions for class ALEntryList.
 |
 +- Functions ------------------------------------------------------------
 |  ALEntryList::ClearError()
 |  ALEntryList::SetMarks()
 |  ALEntryList::ClearMarks()
 |
 +=======================================================================*/

/*- Function Name --------------------------------------------------------
 |  ALEntryList::ClearError()
 |
 +- Version --------------------------------------------------------------
 |  2.11.00 01 Oct 1996 12:00
 |
 +- Description ----------------------------------------------------------
 |  An ALEntryList object carries around a status object in its mStatus
 |  member.  For various reasons, this member might get set to an error
 |  condition.  Error conditions aren't cleared automatically by the library,
 |  so the user will have to manually clear it with a call to this function.
 |
 |  This is a real simple function, so it will normally be implemented as
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
ALEntryList::ClearError()
{
 mStatus.SetError(AL_SUCCESS, 0);
}

/*- Function Name --------------------------------------------------------
 |  ALEntryList::SetMarks()
 |
 +- Version --------------------------------------------------------------
 |  2.11.00 01 Oct 1996 12:00
 |
 +- Description ----------------------------------------------------------
 |  An ALEntryList is composed of a bunch of ALEntry objects.  Each ALEntry
 |  has a marker attached to it that can be set or cleared.  Archive
 |  operations such as Create(), Append(), and so on, only operate on
 |  ALEntry objects that have their mark set.  Marks are set by default when
 |  the object is created, but they can be cleared for various reasons.
 |
 |  SetMarks() is used to set the marks of multiple objects at once.
 |  It is done by simply checking the names of the storage objects in the
 |  ALEntry object.  If a wild card match is successful, the mark is set,
 |  otherwise it is left alone.
 |
 |  This is a real simple function, so it will normally be implemented as
 |  a C++ inline function. VB et. al. don't get this boon.
 |  this luxury.
 |
 +- Arguments ------------------------------------------------------------
 |  const char *  - A wild card pattern that is compared to the name of
 |                  each storage object in the entry list.  The wild card
 |                  matching in this case is not done according to the
 |                  rules of the operating system, but rather according to
 |                  ArchiveLib's match() function.
 |
 +- Return Value ---------------------------------------------------------
 |  An integer count detailing how many of the marks were set in the list.
 |
 +- Revision History -----------------------------------------------------
 |
 +-----------------------------------------------------------------------*/

// Tag public function
inline int AL_INLINE_PROTO
ALEntryList::SetMarks(const char AL_DLL_FAR *name)
{
 return SetMarkState(name, 1);
}

/*- Function Name --------------------------------------------------------
 |  ALEntryList::ClearMarks()
 |
 +- Version --------------------------------------------------------------
 |  2.11.00 01 Oct 1996 12:00
 |
 +- Description ----------------------------------------------------------
 |  An ALEntryList is composed of a bunch of ALEntry objects.  Each ALEntry
 |  has a marker attached to it that can be set or cleared.  Archive
 |  operations such as Create(), Append(), and so on, only operate on
 |  ALEntry objects that have their mark set.  Marks are set by default when
 |  the object is created, but they can be cleared for various reasons.
 |
 |  ClearMarks() is used to clear the marks of multiple objects at once.
 |  It is done by simply checking the names of the storage objects in the
 |  ALEntry object.  If a wild card match is successful, the mark is cleared,
 |  otherwise it is left alone.
 |
 |  This is a real simple function, so it will normally be implemented as
 |  a C++ inline function. VB et. al. don't get this boon.
 |  this luxury.
 |
 +- Arguments ------------------------------------------------------------
 |  const char *  - A wild card pattern that is compared to the name of
 |                  each storage object in the entry list.  The wild card
 |                  matching in this case is not done according to the
 |                  rules of the operating system, but rather according to
 |                  ArchiveLib's match() function.
 |
 +- Return Value ---------------------------------------------------------
 |  An integer count detailing how many of the marks were cleared in the
 |  list.
 |
 +- Revision History -----------------------------------------------------
 |
 +-----------------------------------------------------------------------*/

// Tag public function
inline int AL_INLINE_PROTO
ALEntryList::ClearMarks(const char AL_DLL_FAR *name)
{
 return SetMarkState(name, 0);
}

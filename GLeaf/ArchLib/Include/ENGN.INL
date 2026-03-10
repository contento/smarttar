/*= Archive Library v2.12 ================================================
 |
 |  H\ENGN.INL
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  Inline functions for class ALCompressedObject.
 |
 +- Functions ------------------------------------------------------------
 |  ALEngine::ClearError()
 |
 +=======================================================================*/

/*- Function Name --------------------------------------------------------
 |  ALEngine::ClearError()
 |
 +- Version --------------------------------------------------------------
 |  2.11.00 01 Oct 1996 12:00
 |
 +- Description ----------------------------------------------------------
 |  An ALEngine object carries around a status object in its mStatus
 |  member.  For various reasons, this member might get set to an error
 |  condition.  Error conditions aren't cleared automatically by the library,
 |  so the user will have to manually clear it with a call to this function.
 |
 |  This is a real simple function, so in C++ it will be implemented as
 |  an inline function.  The rest of the supported languages don't have
 |  this luxury.
 |
 +- Note -----------------------------------------------------------------
 |  There are two different versions of this function for C, VB, and Delphi.
 |  This was necessary to preserve some type safety in languages where we
 |  aren't supporting the concept of inheritance.
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
ALEngine::ClearError()
{
 mStatus.SetError(AL_SUCCESS, 0);
}

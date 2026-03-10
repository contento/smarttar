/*= Archive Library v2.12 ================================================
 |
 |  H\CMPOBJ.INL
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  Inline functions for class ALCompressedObject.
 |
 +- Functions ------------------------------------------------------------
 |  ALCompressedObject::ClearError()
 |
 +=======================================================================*/

/*- Function Name --------------------------------------------------------
 |  ALCompressedObject::ClearError()
 |
 +- Version --------------------------------------------------------------
 |  2.11.00 01 Oct 1996 12:00
 |
 +- Description ----------------------------------------------------------
 |  An ALCompressedObject carries around a status object in its mStatus
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
ALCompressedObject::ClearError()
{
 mStatus.SetError(AL_SUCCESS, 0);
}

/*= Archive Library v2.12 ================================================
 |
 |  H\TIMEDATE.INL
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Functions ------------------------------------------------------------
 |  ALTimeDate::Valid()
 |
 +=======================================================================*/

/*- Function Name --------------------------------------------------------
 |  ALTimeDate::Valid()
 |
 +- Version --------------------------------------------------------------
 |  2.11.00 01 Oct 1996 12:00
 |
 +- Description ----------------------------------------------------------
 |  ALStorage objects carry around a time date stamp.  The time date stamp
 |  isn't valid until you have a chance to either read it from the file
 |  system or from an archive.  But how do you tell if the time date stamp
 |  you have is valid?  Easy, just call this function.
 |
 |  BTW, the reason we don't have C/VB/Delphi wrapper functions is that
 |  those languages don't have immediate access to the mTimeDate member
 |  of the storage object.  So instead, they attach their Valid() function
 |  to a storage object.  The result is ALStorageTimeDateValid(), or
 |  something like that.
 |
 +- Arguments ------------------------------------------------------------
 |  None
 |
 +- Return Value ---------------------------------------------------------
 |  1 or 0
 |
 +- Revision History -----------------------------------------------------
 |
 +-----------------------------------------------------------------------*/

inline int AL_INLINE_PROTO
// Tag public function
ALTimeDate::Valid()
{
 return miYear != 0;
}

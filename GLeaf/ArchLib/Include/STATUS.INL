/*= Archive Library v2.12 ================================================
 |
 |  H\STATUS.INL
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  Inline functions for class ALStatus.
 |
 +- Functions ------------------------------------------------------------
 |  ALStatus::GetStatusCode()
 |  ALStatus::operator int()
 |  operator << (ostream& stream, const ALStatus &status)
 |
 +=======================================================================*/

/*- Function Name --------------------------------------------------------
 |  ALStatus::GetStatusCode()
 |
 +- Version --------------------------------------------------------------
 |  2.11.00 01 Oct 1996 12:00
 |
 +- Description ----------------------------------------------------------
 |  Lots of objects in ArchiveLib carry around a status member.  The
 |  status member has two parts.  First, it has a numeric value that
 |  is 0 when things are cool and < 0 when things aren't.  This function
 |  takes care of returning that value.  The second part is the text
 |  of the error message, and we don't worry about that here.
 |
 |  So why no C/VB/Delphi translation for this function?  To simplify things,
 |  C and other programs have individual customized routines that know how
 |  to get the status from ALArchive, ALCompressors, etc.  That way
 |  they don't have to go through a two step process of getting the
 |  status member of an object first, then getting the integer value
 |  out of that status object.
 |
 +- Arguments ------------------------------------------------------------
 |  None
 |
 +- Return Value ---------------------------------------------------------
 |  The integer value contained in the status member.
 |
 +- Revision History -----------------------------------------------------
 |
 +-----------------------------------------------------------------------*/

// Tag public function
inline int AL_INLINE_PROTO
ALStatus::GetStatusCode()
{
 return miStatus;
}

/*- Function Name --------------------------------------------------------
 |  ALStatus::operator int()
 |
 +- Version --------------------------------------------------------------
 |  2.11.00 01 Oct 1996 12:00
 |
 +- Description ----------------------------------------------------------
 |  Lots of objects in ArchiveLib carry around a status member.  The
 |  status member has two parts.  First, it has a numeric value that
 |  is 0 when things are cool and < 0 when things aren't.  This function
 |  takes care of returning that value.  The second part is the text
 |  of the error message, and we don't worry about that here.
 |
 |  This overloaded operator int is just the ticket for converting an
 |  ALStatus member to its integer whenever needed.
 |
 +- Arguments ------------------------------------------------------------
 |  None
 |
 +- Return Value ---------------------------------------------------------
 |  The integer value contained in the status member.
 |
 +- Revision History -----------------------------------------------------
 |
 +-----------------------------------------------------------------------*/

// Tag public function
inline AL_INLINE_PROTO
ALStatus::operator int()
{
 return miStatus;
}

/*- Function Name --------------------------------------------------------
 |  ostream& operator << (ostream& , const ALStatus &)
 |
 +- Version --------------------------------------------------------------
 |  2.11.00 01 Oct 1996 12:00
 |
 +- Description ----------------------------------------------------------
 |  Lots of objects in ArchiveLib carry around a status member.  The
 |  status member has two parts.  First, it has a numeric value that
 |  is 0 when things are cool and < 0 when things aren't.  Whoever
 |  set the message to a value less than zero is also responsible for
 |  adding a text description of the message to the status member.  That
 |  text description is what gets output to the stream in this function.
 |
 +- Arguments ------------------------------------------------------------
 |  None
 |
 +- Return Value ---------------------------------------------------------
 |  A reference to the output stream, so you can cascade the output.
 |
 +- Revision History -----------------------------------------------------
 |
 +-----------------------------------------------------------------------*/

// Tag public function
inline ostream&
operator<<(ostream& stream, const ALStatus AL_DLL_FAR &status)
{
#if defined(AL_USING_DLL) && !defined(AL_LARGE_MODEL) && !defined(AL_FLAT_MODEL)
 const char _far *p = status.GetStatusDetail();
 char *near_string = new char[_fstrlen(p) + 1];
 if (near_string) {
  _fstrcpy(near_string, p);
  stream << near_string;
  delete near_string;
 } else
  stream << "Memory allocation failure!";
 return stream;
#else
 return stream << status.GetStatusDetail();
#endif
}

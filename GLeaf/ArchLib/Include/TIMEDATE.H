/*= Archive Library v2.12 ================================================
 |
 |  H\TIMEDATE.H
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  TIMEDATE.H contains the class definition for ALTimeDate.
 |
 +- Classes --------------------------------------------------------------
 |  ALTimeDate
 |
 +=======================================================================*/

#ifndef __TIMEDATE_H__
#define __TIMEDATE_H__

#include <time.h>

/*- Class Name -----------------------------------------------------------
 |  ALTimeDate
 |
 +- Version --------------------------------------------------------------
 |  2.11.00 01 Oct 1996 12:00
 |
 +- Description ----------------------------------------------------------
 |  ALTimeDate is used strictly to keep track of the time/date stamp of an
 |  ALStorage object.  The only place this class appears is as the mTimeDate
 |  member of ALStorage.  It has a number of conversion utilities for
 |  loading and exporting its values in various formats.  Since most of the
 |  ways we have to set time stamps for objects are not ANSI standard, we
 |  end up with quite a few conversion utilities.
 |
 +- Data Members ---------------------------------------------------------
 |  miYear      -   The year just like you would expect, e.g 1996.  A value
 |                  of 0 in this field indicates an invalid time.
 |  miMonth     -   The month, 1-12.
 |  miDate      -   The date, 1-31.
 |  miHour      -   In 2400 format, 0 - 23.
 |  miMinute    -   0 - 59
 |  miSecond    -   0 - 59
 |
 +- Member Functions -----------------------------------------------------
 |  ALTimeDate()             -  The constructor, sets all members to 0.
 |  ~ALTimeDate()            -  The destructor, has no work to do.
 |  operator new()           -  The memory allocation operator, only used
 |                              when the library is inside a DLL.
 |  ToJulian()               -  Convert the internal m/d/y members to
 |                              a julian day number.
 |  FromJulian()             -  Convert a julian day number to internal
 |                              data members m/d/y.
 |  GetUnixTime()            -  Convert all members to a long in unix format,
 |                              total seconds since 1/1/1970.
 |  GetDosTime()             -  Convert h:m:s data members to the unsigned int
 |                              used in certain DOS commands.
 |  GetDosDate()             -  Convert m/d/y data members to the unsigned int
 |                              used in certain DOS commands.
 |  SetTimeDate(long)        -  Set internal data members from a UNIX long.
 |  SetTimeDate(struct tm*)  -  Set internal data members from a DOS
 |                              struct tm *.
 |  GetTimeDate()            -  Convert internal data members to a DOS
 |                              struct tm *.
 |  Valid()                  -  Indicate if a valid time has been set.
 |
 +- Revision History -----------------------------------------------------
 |
 +-----------------------------------------------------------------------*/

#ifdef __cplusplus

// Tag public class
class AL_CLASS_TYPE ALTimeDate {

// constructors, destructors, declarations, assignment operator

 public :
  AL_PROTO ALTimeDate();
  AL_PROTO ~ALTimeDate();
#if defined(AL_USING_DLL) || defined(AL_BUILDING_DLL)
  void AL_DLL_FAR * AL_PROTO operator new(size_t size);
#endif

// I usually hide the copy constructor and assignment operators,
// but in this case they are OK.

// member functions

 public :
  long AL_PROTO ToJulian();
  void AL_PROTO FromJulian(long jdn);
  long AL_PROTO GetUnixTime();
  unsigned short int AL_PROTO GetDosTime();
  unsigned short int AL_PROTO GetDosDate();
  void AL_PROTO SetTimeDate(long unix_time);
  void AL_PROTO SetTimeDate(struct tm AL_DLL_FAR *tblock);
  void AL_PROTO GetTimeDate(struct tm AL_DLL_FAR *tblock);
  int AL_PROTO Valid();

// Data members

 protected :
  short int miYear;       // e.g. 1996
  short int miMonth;      // 1-12
  short int miDate;       // 1-31
  short int miHour;       // 0-23
  short int miMinute;     // 0-59
  short int miSecond;     // 0-59
};

#include "timedate.inl"

#endif // #ifdef __cplusplus

#endif // #ifndef __TIMEDATE_H__

/*= Archive Library v2.12 ================================================
 |
 |  H\SPINNER.H
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  SPINNER.H contains the class definition for the ALMonitor derived
 |  object ALSpinner.
 |
 +- Classes --------------------------------------------------------------
 |  ALSpinner
 |
 +=======================================================================*/

#ifndef __SPINNER_H__
#define __SPINNER_H__

#include "arclib.h"

/*- Class Name -----------------------------------------------------------
 |  ALSpinner
 |
 +- Version --------------------------------------------------------------
 |  2.11.00 01 Oct 1996 12:00
 |
 +- Description ----------------------------------------------------------
 |  ALSpinner is a very simple monitor class, and is only useful under
 |  DOS, not Windows.  All ALSpinner does is spin a little propellor
 |  around while the file is being processed.  This lets you know
 |  that something is happening in an otherwise boring process.
 |
 +- Data Members ---------------------------------------------------------
 |  miSpinIndex     -  A Static variable that keeps track of which
 |                     position the propellor should be in.  The
 |                     propellor travels through one of four different
 |                     positions.
 |
 |  mrStream        -  A reference to the stream that the propellor is
 |                     going to be written on.
 |
 +- Member Functions -----------------------------------------------------
 |  ALSpinner()         - The one and only constructor.
 |  ~ALSpinner()        - The virtual destructor.
 |  Progress()          - The routine that gets called to make the
 |                        propellor twitch.
 |  ArchiveOperation()  - The routine that gets called when archiving
 |                        starts, stops, etc.
 |  VerboseMessage()    - Some additional messages that only get turned
 |                        on when a verbose mode is selected (at least that
 |                        is the case for the Greenleaf supplied classes)
 |
 +- Revision History -----------------------------------------------------
 |
 +-----------------------------------------------------------------------*/

// Microsoft refuses to define cout if you are building a
// windows DLL.  So, we work around that here by preventing
// anyone from using the bargraph class from a DLL.  That's
// okay anyway, since Microsoft doesn't have a 16 bit
// DLL RTL.

#if !defined(AL_MICROSOFT) || !defined(AL_BUILDING_DLL) || (defined(AL_MICROSOFT) && defined(AL_FLAT_MODEL))

#ifdef __cplusplus

// Tag public class
class AL_CLASS_TYPE ALSpinner : public ALMonitor {

// constructors, destructors, and friend classes

 public :
  AL_PROTO ALSpinner(ALMonitorType monitor_type, ostream AL_DLL_FAR & stream = cout);
  virtual AL_PROTO ~ALSpinner();
#if defined(AL_USING_DLL) || defined(AL_BUILDING_DLL)
 void AL_DLL_FAR * AL_PROTO operator new(size_t size);
#endif

//  The copy constructor and assignment operator do not exist.

 protected :
  AL_PROTO ALSpinner(const ALSpinner AL_DLL_FAR &);
  ALSpinner AL_DLL_FAR & AL_PROTO operator=(const ALSpinner AL_DLL_FAR &);

// member functions

 public :
  virtual void AL_PROTO Progress(long mlObjectSoFar, ALStorage AL_DLL_FAR & object);
  virtual void AL_PROTO ArchiveOperation(ALArchiveOperation operation,
                                         ALArchive AL_DLL_FAR *archive,
                                         ALEntry AL_DLL_FAR *job);
  virtual void AL_PROTO VerboseMessage(enum ALArchiveOperation operation,
                                       const char AL_DLL_FAR *fmt,
                                       ...);

// data Members

 protected :
  static int miSpinIndex;
  ostream AL_DLL_FAR & mrStream;

 public :
  AL_CLASS_TAG(_ALSpinnerTag);
};

extern "C" {

#endif  // #ifdef __cplusplus

AL_LINKAGE hALMonitor AL_FUNCTION newALSpinner(enum ALMonitorType monitor_type);

#ifdef __cplusplus
};
#endif

#endif // #if !defined(AL_MICROSOFT) || !defined(AL_BUILDING_DLL)

#endif // #ifndef __SPINNER_H__

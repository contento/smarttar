/*= Archive Library v2.12 ================================================
 |
 |  H\BARGRAPH.H
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  The class definition for ALBarGraph is found here.
 |
 +- Classes --------------------------------------------------------------
 |  ALBarGraph
 |
 +=======================================================================*/

#ifndef __BARGRAPH_H__
#define __BARGRAPH_H__

#include "arclib.h"

/*- Class Name -----------------------------------------------------------
 |  ALBarGraph
 |
 +- Version --------------------------------------------------------------
 |  2.11.00 01 Oct 1996 12:00
 |
 +- Description ----------------------------------------------------------
 |  This is a utility class.  The constructor opens a file for input,
 |  and keeps track of whether it was already open or not.  The destructor
 |  will automatically close the file if it was closed when the
 |  ctor was invoked.
 |
 +- Data Members ---------------------------------------------------------
 |  miCurrentOffset   - The current offset of the bargraph, in screen
 |                      units.  Usually the bar itself is 20 characters
 |                      long, in which case this value will be somewhere
 |                      between 0 and 19.
 |
 |  miBarLength       - The length of the bar, defined when the
 |                      constructor is called.  This is a const member,
 |                      which means we can leave it public.
 |
 |  mrStream          - Reference to an output stream.  This is the
 |                      stream where the bargraph gets drawn.
 |
 +- Member Functions -----------------------------------------------------
 |  ALBarGraph()        - The constructor.
 |  ~ALBarGraph()       - Virtual destructor.
 |  Progress()          - The progress routine, where the bargraph
 |                        gets updated.
 |  ArchiveOperation()  - The routine that gets called when files
 |                        are opened, closed, etc.
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
class AL_CLASS_TYPE ALBarGraph : public ALMonitor {

// constructors, destructors, and friend classes

 public :
  AL_PROTO ALBarGraph(ALMonitorType monitor_type,
                      ostream AL_DLL_FAR& stream = cout,
                      int bar_length = 25);
  virtual AL_PROTO ~ALBarGraph();
#if defined(AL_USING_DLL) || defined(AL_BUILDING_DLL)
  void AL_DLL_FAR * AL_PROTO operator new(size_t size);
#endif

// copy constructor and assignment operator do not exist.

 protected :
  AL_PROTO ALBarGraph(const ALBarGraph AL_DLL_FAR &);
  ALBarGraph AL_DLL_FAR & AL_PROTO operator=(const ALBarGraph AL_DLL_FAR &);

// member functions

 public :
  virtual void AL_PROTO Progress(long object_so_far,
                                 ALStorage AL_DLL_FAR & object);
  virtual void AL_PROTO ArchiveOperation(enum ALArchiveOperation operation,
                                         ALArchive AL_DLL_FAR *archive,
                                         ALEntry AL_DLL_FAR *job);
  virtual void AL_PROTO VerboseMessage(enum ALArchiveOperation operation,
                                       const char AL_DLL_FAR *fmt,
                                       ...);
// data members

 protected :
  int miCurrentOffset;
  const int miBarLength;
  ostream AL_DLL_FAR & mrStream;
 public :
  AL_CLASS_TAG(_ALBarGraphTag);
};

extern "C" {

#endif // #ifdef __cplusplus

AL_LINKAGE hALMonitor AL_FUNCTION newALBarGraph(enum ALMonitorType monitor_type);

#ifdef __cplusplus
}
#endif

#endif // #if !defined(AL_MICROSOFT) || !defined(AL_BUILDING_DLL)

#endif // #ifdef __BARGRAPH_H__

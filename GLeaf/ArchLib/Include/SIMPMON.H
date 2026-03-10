/*= Archive Library v2.12 ================================================
 |
 |  H\SIMPMON.H
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  SIMPMON.H contains the class definition for ALSimpleMonitor.
 |
 +- Classes --------------------------------------------------------------
 |  ALSimpleMonitor
 |
 +=======================================================================*/

#ifndef __SIMPMON_H__
#define __SIMPMON_H__

#include "monitor.h"

#ifdef __cplusplus

/*- Class Name -----------------------------------------------------------
 |  ALSimpleMonitor
 |
 +- Version --------------------------------------------------------------
 |  2.11.00 01 Oct 1996 12:00
 |
 +- Description ----------------------------------------------------------
 |  This class is used by the simplified interface functions as a monitor.
 |  For C and C++ programmers, the monitor simply calls a callback function
 |  supplied by the user.  VB and Delphi users see the monitor as an object
 |  that sends text messages to windows they are using as part of their
 |  user interface.
 |
 |  Regardless of the language you are using, this monitor class is hidden
 |  from you.  However, it is pretty simple, and the details of what it
 |  is doing may help you understand the simplified interfaces.
 |
 +- Data Members ---------------------------------------------------------
 |  mpCallbackFunction       -    A pointer to a callback function that
 |                                the monitor will call from both
 |                                Progress() and ArchiveOperation().
 |  mhTextWindow             -    The text window where file names will
 |                                be sent from ArchiveOperation().
 |  mhFileProgressWindow     -    The text window where progress ratios
 |                                will be sent from Progress().
 |  mhJobProgressWindow      -    The text window were job ratios will be
 |                                sent from Progress().
 |
 +- Member Functions -----------------------------------------------------
 |  ALSimpleMonitor()     -   The constructor, two versions
 |  ~ALSimpleMonitor()    -   The destructor.
 |  Progress()            -   The archiving file/job progress function
 |  ArchiveOperation()    -   The function called at key points in the
 |                            archiving routine.
 |  VerboseMessage()      -   The function called with verbose messages.
 |
 +- Revision History -----------------------------------------------------
 |
 +-----------------------------------------------------------------------*/

// Tag public class
class AL_CLASS_TYPE ALSimpleMonitor : public ALMonitor {

// constructors, destructors, and friend classes

 public :
  AL_PROTO ALSimpleMonitor(CALLBACK_FN fn);

// alternate constructor, used by VB and Delphi

#if defined(AL_VB) || defined(AL_VB32)
 AL_PROTO ALSimpleMonitor(HWND text_window, HWND file_progress_window, HWND job_progress_window);
#endif
 virtual AL_PROTO ~ALSimpleMonitor();
#if defined(AL_USING_DLL) || defined(AL_BUILDING_DLL)
 void AL_DLL_FAR * AL_PROTO operator new(size_t size);
#endif

// copy constructor and assignment operator do not exist.

 protected :
  AL_PROTO ALSimpleMonitor(const ALSimpleMonitor AL_DLL_FAR &);
  ALSimpleMonitor AL_DLL_FAR & AL_PROTO operator=(const ALSimpleMonitor AL_DLL_FAR &);

// member functions

 public    :
  virtual void AL_PROTO Progress(long object_so_far, ALStorage AL_DLL_FAR & object);
  virtual void AL_PROTO ArchiveOperation(ALArchiveOperation operation, ALArchive AL_DLL_FAR *archive, ALEntry AL_DLL_FAR *job);
  virtual void AL_PROTO VerboseMessage(enum ALArchiveOperation operation, const char AL_DLL_FAR *fmt, ...);

// data members

 protected :
  CALLBACK_FN mpCallbackFunction;
#if defined(AL_VB) || defined(AL_VB32)
  HWND mhTextWindow;
  HWND mhFileProgressWindow;
  HWND mhJobProgressWindow;
#endif
 public :
  AL_CLASS_TAG(_ALSimpleMonitorTag);
};
#endif

#endif // #ifndef __SIMPMON_H__

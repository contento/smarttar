/*= Archive Library v2.12 ================================================
 |
 |  H\WILDCARD.H
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  WILDCARD.H contains the class declaration for ALWildCardExpander,
 |  the class used to expand wildcard file specifications under DOS, Win32,
 |  and OS/2
 |
 +- Classes --------------------------------------------------------------
 |  ALWildCardExpander
 |
 +=======================================================================*/

#ifndef __WILDCARD_H__
#define __WILDCARD_H__

#include "arclib.h"

#ifdef __cplusplus

/*- Class Name -----------------------------------------------------------
 |  ALWildCardExpander
 |
 +- Version --------------------------------------------------------------
 |  2.11.00 01 Oct 1996 12:00
 |
 +- Description ----------------------------------------------------------
 |  This class is used to expand wild card specifications on a DOS, OS/2,
 |  or NT file system. Note that you can do exciting things with this, like
 |  traversing through subdirectories, and separate various specs using
 |  commas or white space.
 |
 +- Data Members ---------------------------------------------------------
 |  mState          - The current state of the expander.  This is a value
 |                    from ALExpander state that lets use keep track
 |                    of where we are between calls to the expander.
 |  mpNextExpander  - If we are traversing subdirectories, we will
 |                    open a new expander for each subdirectory.  This
 |                    is a pointer to any subdirectory we might already
 |                    have open for a search in progress.
 |  mInputLine      - The list of wildcard filespecs passed in as an
 |                    argument.  This gets whittled away, and will be
 |                    smaller and smaller as all the names are parsed out.
 |  mFullWildName   - The current wild card file spec that is being
 |                    expanded.
 |  mWildNameOnly   - The current wild name that is being expanded,
 |                    stripped of its drive and path information.
 |  mWildPathOnly   - The current drive and path being expanded, stripped
 |                    of its filename and extension.
 |  mResultFileName - The file name that is returned to the calling
 |                    program.
 |  miTraverseFlag  - This flag indicates whether the search should traverse
 |                    traverse through subdirectories or just search
 |                    in the current directory.
 |  mpOsData        - Pointer to OS specific data.  This is needed because
 |                    the O/S services needed to scan directories vary
 |                    widely between DOS, Win32, and OS/2.
 |  mpcDelimiters   - An array of characters that are delimiters in the
 |                    input file list.
 |  mCase           - Indicates whether file names should always be forced
 |                    to upper case, forced to lower case, or left mixed.
 |
 +- Member Functions -----------------------------------------------------
 |  ALWildCardExpander()    - Constructor, everything we need to know to
 |                            perform the search is defined here.
 |  ~ALWildCardExpander()   - Destructor.
 |  operator new()          - Memory allocation function, used when the
 |                            library is in a DLL.
 |  GetNextWildName()       - Protected routine to get the next wild name
 |                            from the input line.
 |
 |  ConstructOsData()       - There are different versions of this
 |                            function for each O/S.  It takes care of
 |                            allocating the data structures and services
 |                            needed to start a directory scan.
 |
 |  DestroyOsData()         - Likewise, there is a cleanup function for
 |                            each O/S as well.
 |
 |  IsDelimiter()           - A little function that is used internally
 |                            to test an individual character for delimiter
 |                            status.
 |
 |  GetNextFile()           - The function to get the next expanded file
 |                            name.  It keeps chunking out names until
 |                            the search is complete.
 |
 +- Revision History -----------------------------------------------------
 |
 +-----------------------------------------------------------------------*/

class AL_CLASS_TYPE ALWildCardOsData;
// An incomplete type.  I'll know what to do here
// in the expansion routines, which are O/S specfic

// Tag public class
class AL_CLASS_TYPE ALWildCardExpander {

// constructors, destructors, assignment operators, declarations

 protected :
  // Tag protected class
  enum ALExpanderState {
            GET_NEXT_WILD_NAME,
            GET_FIRST_FILE_NAME,
            GET_NEXT_FILE_NAME,
            GET_FIRST_DIRECTORY,
            GET_NEXT_DIRECTORY
  };
 public :
  AL_PROTO ALWildCardExpander(const char AL_DLL_FAR *file_list,
                              int traverse_flag = 0,
#if defined(AL_WIN32) || defined(AL_OS2)
                              ALCase name_case = AL_MIXED_UP);
#elif defined(AL_UNIX)
                              ALCase name_case = AL_MIXED);
#else
                              ALCase name_case = AL_LOWER);
#endif
  AL_PROTO ALWildCardExpander(const char AL_DLL_FAR *file_list,
                              const char AL_DLL_FAR *delimiters,
                              int traverse_flag = 0,
#if defined(AL_WIN32) || defined(AL_OS2)
                              ALCase name_case = AL_MIXED_UP);
#elif defined(AL_UNIX)
                              ALCase name_case = AL_MIXED);
#else
                              ALCase name_case = AL_LOWER);
#endif
  AL_PROTO ~ALWildCardExpander();
#if defined(AL_USING_DLL) || defined(AL_BUILDING_DLL)
  void AL_DLL_FAR * AL_PROTO operator new(size_t size);
#endif

// disable copy constructor and assignment operator

 protected :
  ALWildCardExpander AL_DLL_FAR & AL_PROTO operator=(ALWildCardExpander AL_DLL_FAR &);
  AL_PROTO ALWildCardExpander(ALWildCardExpander AL_DLL_FAR &);

// member functions

 protected :
  int AL_PROTO GetNextWildName();
  void AL_PROTO ConstructOsData();
  void AL_PROTO DestroyOsData();
  int AL_PROTO IsDelimiter(char c);
 public :
  char AL_DLL_FAR * AL_PROTO GetNextFile();

// data members

 protected :
  ALExpanderState mState;
  ALWildCardExpander AL_DLL_FAR *mpNextExpander;
  ALName mInputLine;
  ALName mFullWildName;
  ALName mWildNameOnly;
  ALName mWildPathOnly;
  ALName mResultFileName;
  int miTraverseFlag;
  ALWildCardOsData AL_DLL_FAR *mpOsData;
  const char AL_DLL_FAR *mpcDelimiters;
 public :
  const ALCase mCase;
  AL_CLASS_TAG(_ALWildCardExpanderTag);
};

extern "C" {

#endif  // #ifdef __cplusplus

AL_LINKAGE hALExpander AL_FUNCTION newALExpander(char AL_DLL_FAR *wild_file_list,
                                                 int traverse_flag,
                                                 enum ALCase name_case);
AL_LINKAGE hALExpander AL_FUNCTION newALExpanderWithDelimiters(char AL_DLL_FAR *wild_file_list,
                                                               char AL_DLL_FAR *delimiters,
                                                               int traverse_flag,
                                                               enum ALCase name_case);
AL_LINKAGE void AL_FUNCTION deleteALExpander(hALExpander this_object);
AL_LINKAGE char AL_DLL_FAR * AL_FUNCTION ALExpanderGetNextFile(hALExpander this_object);

#ifdef __cplusplus
};
#endif

#endif // #ifndef __WILDCARD_H__

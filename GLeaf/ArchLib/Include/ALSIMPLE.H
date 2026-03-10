/*= Archive Library v2.12 ================================================
 |
 |  H\ALSIMPLE.H
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  ALSIMPLE.H contains the definitions for the C/C++ simplified interface.
 |  This interface consists of eight functions and a structure definition,
 |  all of which are found here.  The idea is that this interface lets you
 |  manipulate PKZip archives containing files with little or not knowledge
 |  about the depths of ArchiveLib.
 |
 +- Classes --------------------------------------------------------------
 |  ALZipDir
 |  ALSimpleMonitor
 |
 +- Functions ------------------------------------------------------------
 |  ALFreeDir()
 |  ALReadDir()
 |  ALWriteDir()
 |  ALSetName()
 |  ALSetComment()
 |  ALCreate()
 |  ALAppend()
 |  ALExtract()
 |  ALDelete()
 |  ALTranslateError()
 |
 +=======================================================================*/

#ifndef __ALSIMPLE_H__
#define __ALSIMPLE_H__

#include "aldefs.h"

#ifndef __cplusplus
struct ALZipDir {
#else
struct AL_CLASS_TYPE ALZipDir {
#endif

 char AL_DLL_FAR *name;      // These two members are allocated using
 char AL_DLL_FAR *comment;   // new char[].  Let the library take care
                             // of allocation and deletion.
 long compressed_size;
 long compressed_position;
 long size;
 long crc;
 short int mark;
 short int encrypted;
 short int month;
 short int date;
 short int year;
 short int hour;
 short int minute;
 short int second;
 unsigned char r;
 unsigned char a;
 unsigned char s;
 unsigned char h;
 unsigned char d;
 unsigned char level;
};

typedef void (AL_EXPORT AL_DLL_FAR * CALLBACK_FN)(const char AL_DLL_FAR *file_name,
                                                  int file_ratio,
                                                  int job_ratio);

#ifdef __cplusplus
extern "C" {
#endif

AL_LINKAGE void AL_FUNCTION ALFreeDir(struct ALZipDir AL_DLL_FAR *z);
AL_LINKAGE struct ALZipDir AL_DLL_FAR * AL_FUNCTION ALReadDir(const char AL_DLL_FAR *filename,
                                                              int AL_DLL_FAR *count,
                                                              int AL_DLL_FAR *error);
AL_LINKAGE int AL_FUNCTION ALWriteDir(struct ALZipDir AL_DLL_FAR *z);
AL_LINKAGE void AL_FUNCTION ALSetName(struct ALZipDir AL_DLL_FAR *z,
                                             const char AL_DLL_FAR *name);
AL_LINKAGE void AL_FUNCTION ALSetComment(struct ALZipDir AL_DLL_FAR *z,
                                         const char AL_DLL_FAR *comment);
AL_LINKAGE int AL_FUNCTION ALCreate(const char AL_DLL_FAR *name,
                                    const char AL_DLL_FAR *file_list,
                                    int strip_path,
                                    CALLBACK_FN callback);
AL_LINKAGE int AL_FUNCTION ALAppend(const char AL_DLL_FAR *archive_name,
                                    const char AL_DLL_FAR *input_files,
                                    int strip_path,
                                    CALLBACK_FN callback);
AL_LINKAGE int AL_FUNCTION ALExtract(struct ALZipDir AL_DLL_FAR *z,
                                     int strip_path,
                                     CALLBACK_FN callback);
AL_LINKAGE int AL_FUNCTION ALDelete(struct ALZipDir AL_DLL_FAR *z,
                                    CALLBACK_FN callback);
AL_LINKAGE char AL_DLL_FAR * AL_FUNCTION ALTranslateError(int code);

#ifdef __cplusplus
};
#endif

#endif // #ifndef __ALSIMPLE_H__

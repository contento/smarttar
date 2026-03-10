/*= Archive Library v2.12 ================================================
 |
 |  H\_VBUTIL.H
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  _VBUTIL.H contains the prototypes used for VB interfacing.
 |
 +=======================================================================*/

#ifndef ___VBUTIL_H__
#define ___VBUTIL_H__

#ifndef AL_FLAT_MODEL

extern "C" long _far _pascal
ALCreateVBString(const char _far *string, unsigned short int l);

#endif // #ifndef AL_FLAT_MODEL

#endif // #ifndef __VBUTIL_H

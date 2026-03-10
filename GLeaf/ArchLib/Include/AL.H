/*= Archive Library v2.12 ================================================
 |
 |  H\AL.H
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  This is the master file that you can use to include all of the
 |  ArchiveLib headers.  If you are using precompiled headers with your
 |  compiler, this might be the file you want included first in all your
 |  source files.
 |
 +=======================================================================*/

#ifndef __AL_H__
#define __AL_H__

#include "arclib.h"

// derived classes

#include "filestor.h"
#include "fileattr.h"
#include "memstore.h"
#include "copyengn.h"
#include "glengn.h"
#include "glarc.h"
#include "pkengn.h"
#include "pkarc.h"

// demo classes and functions

#ifdef AL_WINDOWS
 #include "winmon.h"
 #include "algauge.h"
#elif defined AL_OS2_PM
 #include "pmmon.h"
#endif

#include "bargraph.h"
#include "spinner.h"

// additional classes

#include "wildcard.h"

#endif // #ifndef __AL_H__

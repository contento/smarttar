//	Zinc Interface Library - UI_ENV.HPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#ifndef UI_ENV_HPP
#define UI_ENV_HPP

#define ZIL_VERSION		350
#define ZIL_REVISION	4

// ----- Compiler/Environment Dependencies ----------------------------------

#define VOIDF(function)	(function)
#define VOIDP(pointer)	(pointer)

// Borland/Turbo C++
#if defined(__BCPLUSPLUS__) || defined(__TCPLUSPLUS__)
#	undef VOIDF
#	define VOIDF(function)	((void *)(function))
#	undef VOIDP
#	define VOIDP(pointer)	((void *)(pointer))
#	if defined(__OS2__)
#		define ZIL_OS2		20
#		define EXPORT
#		define ZIL_BITS32
#		define ZIL_LITTLEENDIAN
#	elif defined(_WINDOWS) || defined(_Windows)
#		define ZIL_MSWINDOWS	30
//#		ifdef __DLL__
//#			define EXPORT _export
//#		else
//#			define EXPORT huge
//#		endif
//#		define ZIL_BITS16
//#		define ZIL_LITTLEENDIAN
#		if defined(__WIN32__)
#			define WIN32
#		endif
#		define ZIL_MSWINDOWS	30
#		ifdef __DLL__
#				define EXPORT _export
#		else
#			if defined(WIN32)
#				define EXPORT
#			else
#				define EXPORT huge
#			endif
#		endif
#		if defined(WIN32)
#			define ZIL_BITS32
#			define ZIL_LITTLEENDIAN
#		else
#			define ZIL_BITS16
#			define ZIL_LITTLEENDIAN
#		endif
#	else
#		define ZIL_MSDOS	20
#		define EXPORT
#		define ZIL_BITS16
#		define ZIL_LITTLEENDIAN
#	endif
#	if __BORLANDC__ >= 0x0300
#		define ZIL_DELETE
#	endif
#endif

// Zortech C++
#if defined(__ZTC__)
#	define ZIL_LINKBUG
#	if defined(__OS2__)
#		define ZIL_OS2		20
#		define EXPORT
#		define ZIL_BITS32
#		define ZIL_LITTLEENDIAN
#	elif defined(_WINDOWS) || defined(_Windows)
#		define ZIL_MSWINDOWS	30
#		ifdef __DLL__
#			define EXPORT _export
#		else
#			define EXPORT
#		endif
#		define ZIL_BITS16
#		define ZIL_LITTLEENDIAN
#	else
#		define ZIL_MSDOS	20
#		define EXPORT
#		define ZIL_BITS16
#		define ZIL_LITTLEENDIAN
#	endif
#	if __ZTC__ >= 0x0310
#		define ZIL_DELETE
#	endif
#endif

// Microsoft C++ (7.0)
#if defined(_MSC_VER)
#	define ZIL_LINKBUG
#	if defined(_WINDOWS) || defined(_Windows) || defined(WIN32)
#		define ZIL_MSWINDOWS	30
#		ifdef __DLL__
#			if defined(WIN32)
#				define EXPORT __export
#			else
#				define EXPORT _export
#			endif
#		else
#			define EXPORT
#		endif
#		if defined(WIN32)
#			define ZIL_BITS32
#			define ZIL_LITTLEENDIAN
#		else
#			define ZIL_BITS16
#			define ZIL_LITTLEENDIAN
#		endif
#	else
#		define ZIL_MSDOS	20
#		define EXPORT
#		define ZIL_BITS16
#		define ZIL_LITTLEENDIAN
#	endif
#endif

// Watcom C++ (9.5)
#if defined(__WATCOMC__)
#	define ZIL_LINKBUG
#	define ZIL_DELETE
#	if defined(__OS2__)
#		define ZIL_OS2		20
#		define EXPORT
#		define ZIL_BITS32
#		define ZIL_LITTLEENDIAN
#	elif defined(__WINDOWS__) ||  defined(__NT__)
#		define ZIL_MSWINDOWS	30
#		define WINNT
#		ifdef __DLL__
#			if defined(WIN32)
#				define EXPORT __export
#			else
#				define EXPORT _export
#			endif
#		else
#			define EXPORT
#		endif
#		if defined(__WINDOWS_386__) || defined(WIN32)
#			define ZIL_BITS32
#			define ZIL_LITTLEENDIAN
#		else
#			define ZIL_BITS16
#			define ZIL_LITTLEENDIAN
#		endif
#	elif defined(__DVX__)
#		define EXPORT
#		define ZIL_BITS32
#		define ZIL_LITTLEENDIAN
#		define ZIL_LOAD_MOTIF	// Use ZIL_MOTIF not ZIL_LOAD_MOTIF in programs.

		struct _XDisplay;
		struct XKeytrans;
		struct _XrmHashBucketRec;
		struct _XSQEvent;
#	else
#		define ZIL_MSDOS	20
#		define EXPORT
#		define ZIL_BITS32
#		define ZIL_LITTLEENDIAN
#	endif
#endif

// DJGPP, GNU C++ port DOS (1.08)
#if defined(__GNUC__) && defined(MSDOS)
#	define ZIL_LINKBUG
#	define ZIL_DELETE
#	if defined(__DVX__)
#		define EXPORT
#		define ZIL_BITS32
#		define ZIL_LITTLEENDIAN
#		define ZIL_LOAD_MOTIF	// Use ZIL_MOTIF not ZIL_LOAD_MOTIF in programs.

		struct _XDisplay;
		struct XKeytrans;
		struct _XrmHashBucketRec;
		struct _XSQEvent;

		extern "C" {
#		define LC_ALL      0
#		define LC_COLLATE  1
#		define LC_CTYPE    2
#		define LC_MONETARY 3
#		define LC_NUMERIC  4
#		define LC_TIME     5
		struct lconv
		{
			char *decimal_point;
			char *thousands_sep;
			char *grouping;
			char *int_curr_symbol;
			char *currency_symbol;
			char *mon_decimal_point;
			char *mon_thousands_sep;
			char *mon_grouping;
			char *positive_sign;
			char *negative_sign;
			char int_frac_digits;
			char frac_digits;
			char p_cs_precedes;
			char p_sep_by_space;
			char n_cs_precedes;
			char n_sep_by_space;
			char p_sign_posn;
			char n_sign_posn;
		};

		struct lconv *localeconv( void );
		char *setlocale( int category, const char *locale );
		};
#	else
#		define ZIL_MSDOS	20
#		define EXPORT
#		define ZIL_BITS32
#		define ZIL_LITTLEENDIAN
#	endif
#endif

// Metaware C++ (3.0)
#if defined(__HIGHC__)
#	define ZIL_LINKBUG
#	define ZIL_DELETE
#	if defined(_WINDOWS) || defined(_Windows) || defined(WIN32)
#		define ZIL_MSWINDOWS	30
#		ifdef __DLL__
#			if defined(WIN32)
#				define EXPORT __export
#			else
#				define EXPORT _export
#			endif
#		else
#			define EXPORT
#		endif
#		if defined(WIN32)
#			define ZIL_BITS32
#			define ZIL_LITTLEENDIAN
#		else
#			define ZIL_BITS16
#			define ZIL_LITTLEENDIAN
#		endif
#	else
#		define ZIL_MSDOS	20
#		define EXPORT
#		define ZIL_BITS32
#		define ZIL_LITTLEENDIAN
#	endif
#endif

// IBM C++
#if defined(__IBMCPP__)
#	define ZIL_LINKBUG
#	define ZIL_DELETE
#	define ZIL_OS2		21
#	define EXPORT
#	define ZIL_BITS32
#	define ZIL_LITTLEENDIAN
#endif

// Computer Associates C++ (Glockenspiel)
#if defined(__GLOCKENSPIEL__)
#	define ZIL_LINKBUG
#	define ZIL_DELETE
#	define ZIL_OS2		21
#	define EXPORT
#	define ZIL_BITS32
#	define ZIL_LITTLEENDIAN
#endif

// HP-UX, CC (cfront from HP) and Motif
#if defined(__hpux)
#	define ZIL_POSIX
#	define EXPORT
#	define ZIL_BITS32
#	define ZIL_BIGENDIAN
#	define ZIL_LOAD_MOTIF	// Use ZIL_MOTIF not ZIL_LOAD_MOTIF in programs.
#endif

// Solaris 2.1, CC (cfront from SunPro) and Motif
#if defined(sun)
#	define ZIL_POSIX
#	define EXPORT
#	define ZIL_BITS32
#	define ZIL_BIGENDIAN
#	define ZIL_LOAD_MOTIF	// Use ZIL_MOTIF not ZIL_LOAD_MOTIF in programs.
#	if defined(_SUNOS4)
#		define XtPointer caddr_t
#		define XTFUNCPROTO
#		define CFRONT
#	endif
#endif

// Silicon Graphics and Motif
#if defined(__sgi)
#	define ZIL_POSIX
#	define EXPORT
#	define ZIL_BITS32
#	define ZIL_BIGENDIAN
#	define ZIL_LOAD_MOTIF	// Use ZIL_MOTIF not ZIL_LOAD_MOTIF in programs.
#	define XTFUNCPROTO
#endif

// IBM RS6000 and Motif
#if defined(_IBM_RS6000)
#	define ZIL_POSIX
#	define EXPORT
#	define ZIL_BITS32
#	define ZIL_BIGENDIAN
#	define ZIL_LOAD_MOTIF	// Use ZIL_MOTIF not ZIL_LOAD_MOTIF in programs.
#	define ZIL_DELETE
#endif

// MS-DOS, Quarterdeck DESQview/X with Motif, DJGPP G++
#if defined(QDeckX)
#	define ZIL_POSIX
#	define EXPORT
#	define ZIL_BITS32
#	define ZIL_LITTLEENDIAN
#	define ZIL_LOAD_MOTIF	// Use ZIL_MOTIF not ZIL_LOAD_MOTIF in programs.
#	define ZIL_DELETE
	extern "C" {
#		define LC_ALL      0
#		define LC_COLLATE  1
#		define LC_CTYPE    2
#		define LC_MONETARY 3
#		define LC_NUMERIC  4
#		define LC_TIME     5
	struct lconv
	{
		char *decimal_point;
		char *thousands_sep;
		char *grouping;
		char *int_curr_symbol;
		char *currency_symbol;
		char *mon_decimal_point;
		char *mon_thousands_sep;
		char *mon_grouping;
		char *positive_sign;
		char *negative_sign;
		char int_frac_digits;
		char frac_digits;
		char p_cs_precedes;
		char p_sep_by_space;
		char n_cs_precedes;
		char n_sep_by_space;
		char p_sign_posn;
		char n_sign_posn;
	};

	struct lconv *localeconv( void );
	char *setlocale( int category, const char *locale );
	};
#endif

// ----- General Definitions ------------------------------------------------

// Optimization switches for various compiler problems.
#define ZIL_PERSISTENCE		// Zinc persistence.
#define ZIL_STORAGE			// Zinc storage.
#define ZIL_OPTIMIZE		// Size & speed optimization for members.
#define ZIL_EDIT			// WOS_EDIT_MODE objects.

#if defined(ZIL_MSDOS)
#	define MAXPATHLEN	128
#	define ZIL_PATHSEP	';'
#	define ZIL_DIRSEP	'\\'
#	define ZIL_DIRSEPSTR	"\\"
	typedef char ichar_t;

	extern ichar_t _iCornerUL[];
	extern ichar_t _iCornerUR[];
	extern ichar_t _iCornerLL[];
	extern ichar_t _iCornerLR[];
	extern ichar_t _iHorizontal[];
	extern ichar_t _iVertical[];
	extern ichar_t _iLowerBox;
	extern ichar_t _iUpperBox;
	extern ichar_t _iFullBox;
	extern ichar_t *_iSBUpArrow;
	extern ichar_t *_iSBDownArrow;
	extern ichar_t *_iSBLeftArrow;
	extern ichar_t *_iSBRightArrow;
	extern ichar_t *_iSBMiddleArrow;
	extern ichar_t *_iWDownArrow;
	extern ichar_t *_iWNormalMin;
	extern ichar_t *_iWMinimized;
	extern ichar_t *_iWNormalMax;
	extern ichar_t *_iWMaximized;
	extern ichar_t *_iActiveCheck;
	extern ichar_t *_iInactiveCheck;
	extern ichar_t *_iActiveRadio;
	extern ichar_t *_iInactiveRadio;
	extern ichar_t *_iCheckMark;

	extern void I_ScreenOpen(int *, int *, int *);
	extern void I_ScreenClose(void);
	extern void I_ScreenPut(int, int, int, int, void *);
	extern void I_ScreenGet(int, int, int, int, void *);
	extern int I_ScreenColor(int);
	extern void I_CursorRemove(void);
	extern void I_CursorPosition(int, int, int);

	extern int I_MouseOpen(void);
	extern void I_MouseClose(void);
	extern void I_MouseHalt(int);

	extern void I_KeyboardOpen(void);
	extern void I_KeyboardClose(void);
	extern void I_KeyboardRead(unsigned *, unsigned *, unsigned *);
	extern int I_KeyboardQuery(unsigned *);

	extern void (*mouseQueueEvent)(unsigned, int *, int *);

#	define ZIL_EXT	".DAT"
#	define ZIL_BAK	".BK?"
#endif

#if defined(ZIL_MSWINDOWS)
extern "C" {																// BUG 1326
#	include <windows.h>
}
#	define MAXPATHLEN	128
#	define ZIL_PATHSEP	';'
#	define ZIL_DIRSEP	'\\'
#	define ZIL_DIRSEPSTR	"\\"
#	define ZIL_EXT	".DAT"
#	define ZIL_BAK	".BK?"
#endif

#if defined(ZIL_OS2)
#	define HIWORD(arg) (((ULONG)arg >> 16) & 0x0000FFFF)
#	define LOWORD(arg) ((ULONG)arg & 0x0000FFFF)
	extern "C"
	{
#	include <os2.h>
	}
#	define MAXPATHLEN	128
#	define ZIL_PATHSEP	';'
#	define ZIL_DIRSEP	'\\'
#	define ZIL_DIRSEPSTR	"\\"
#	define ZIL_EXT	".DAT"
#	define ZIL_BAK	".BK?"
#endif

#if defined(ZIL_POSIX)
	extern "C" {
		void cursorRemove(void);
		void cursorPosition(int, int, int);
	};
#	define MAXPATHLEN	1024
#	define ZIL_PATHSEP	':'
#	define ZIL_DIRSEP	'/'
#	define ZIL_DIRSEPSTR	"/"
#	define ZIL_EXT	".dat"
#	define ZIL_BAK	".bk?"
//  Compatibility with 3.5 Beta
#   define ZIL_UNIX
#endif

#if defined(__DVX__)
#	define ZIL_PATHSEP	';'
#	define ZIL_DIRSEP	'\\'
#	define ZIL_DIRSEPSTR	"\\"

#	define ZIL_EXT	".dat"
#	define ZIL_BAK	".bk?"
#endif

#if defined (ZIL_LOAD_MOTIF)
	extern "C"
	{
#	include <X11/Intrinsic.h>
#	include <Xm/Xm.h>
#include <stdio.h>
	}

#	if defined(XtSpecificationRelease)
#		define ZIL_X11		XtSpecificationRelease
#	else
#		define ZIL_X11		4
#	endif
#	if defined(XmVersion)
#		define ZIL_MOTIF	XmVersion
#	else
#		define ZIL_MOTIF	1001
#	endif
#	undef	ZIL_LOAD_MOTIF
#endif

#define ZIL_HOTMARK	'&'
#define ZIL_HOTMARKSTR	"&"

#endif	// UI_ENV_HPP

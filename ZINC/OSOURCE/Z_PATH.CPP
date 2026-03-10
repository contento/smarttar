//	Zinc Interface Library - PATH.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#if defined(__ZTC__) || defined(_MSC_VER) || defined(__WATCOMC__) || defined(__HIGHC__) || defined(__IBMCPP__)
#	include <direct.h>
#endif
#if defined(__BCPLUSPLUS__) | defined(__TCPLUSPLUS__)
#	include <dir.h>
#endif
#include <stdlib.h>
#include <string.h>
#include "ui_gen.hpp"
#if defined(ZIL_MSDOS) || defined(ZIL_MSWINDOWS) || defined(ZIL_OS2)
#	include <io.h>
#endif
#if defined(ZIL_POSIX)
#	include <unistd.h>
#endif
#if defined(_MSC_VER)
#	define getcwd	_getcwd
#	pragma hdrstop					// Microsoft pre-compiled header pragma.
#elif defined(__IBMCPP__)
#	define getcwd	_getcwd
#endif

static UI_PATH path;
UI_PATH *_path = &path;

// ----- UI_PATH_ELEMENT & UI_PATH ------------------------------------------

UI_PATH_ELEMENT::UI_PATH_ELEMENT(char *_pathName, int length)
{
	if (length == -1)
		length = ui_strlen(_pathName);
	pathName = new char[length+1];
	strncpy(pathName, _pathName, length);
	pathName[length] = '\0';
}

UI_PATH_ELEMENT::~UI_PATH_ELEMENT(void)
{
	delete pathName;
}

static void loadPath(UI_PATH *me, const char *envname)
{
	char *path;

	if ((path = getenv(envname)) != NULL)
	{
		for (int i = 0; path[i]; )	// Parse the directory tree.
		{
			for (int j = i; path[j] && path[j] != ZIL_PATHSEP; )
				j++;
			me->Add(NULL, new UI_PATH_ELEMENT(&path[i], j - i));
			i = path[j] ? j + 1 : j;
		}
	}
}


UI_PATH::UI_PATH(char *programPath, int rememberCWD)
{
	// Get the path names.
	if (rememberCWD)				// The current working directory.
	{
		char path[256];
		getcwd(path, sizeof(path));
		Add(NULL, new UI_PATH_ELEMENT(path));
	}

	if (programPath)				// The program path.
	{
		char path[256];
		strcpy(path, programPath);
		char *endPath = strrchr(path, ZIL_DIRSEP);
		if (endPath)
		{
#if defined(ZIL_MSDOS) || defined(ZIL_MSWINDOWS) || defined(ZIL_OS2)
			if (endPath > path && *(endPath - 1) == ':')
				endPath++;			// Leave in the slash if root dir.
#endif
			*endPath = '\0';
		}
		Add(NULL, new UI_PATH_ELEMENT(path));
	}
	loadPath(this, "ZINC_PATH");
	loadPath(this, "PATH");
}

UI_PATH::~UI_PATH(void)
{
	if (_path == this)
		_path = NULL;
}



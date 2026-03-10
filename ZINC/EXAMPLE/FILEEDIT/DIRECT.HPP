#ifndef DIRECT_HPP
#define DIRECT_HPP

// ----- General include directives for all environments.
#define INCL_DOSFILEMGR
#define INCL_DOSDEVICES
#define INCL_DOSERRORS
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ui_win.hpp>

// ----- Compiler specific files needed for the DIRECTORY_SERVICE class.
#if defined(__BCPLUSPLUS__) || defined(__TCPLUSPLUS__)
#	include <dos.h>
#	include <dir.h>
#	include <direct.h>
#elif defined(__ZTC__)
#	include <dos.h>
#	include <cerror.h>
#	include <direct.h>
#	define _dos_getdrive dos_getdrive 
#	define _dos_setdrive dos_setdrive 
#elif defined(_MSC_VER)
#	if !defined(WIN32)
#		include <dos.h>
#	endif
#	if defined(DOSX286)
#		define	_int86x	int86x
#		define	_int86	int86
#		define	_REGS	REGS
#		define	chdir	_chdir
#		define	unlink	_unlink
#	endif
#	include <direct.h>
#	define getcwd _getcwd
#elif defined(__WATCOMC__)
#	include <io.h>
#	include <dos.h>
#	include <direct.h>
#elif defined(__HIGHC__) || defined(__IBMCPP__)
#	include <direct.h>
#elif defined(ZIL_POSIX)
#	include <fcntl.h>
#	include <unistd.h>
#	include <dirent.h>
#	include <sys/stat.h>
#	include <sys/param.h>
#	if defined (_IBM_RS6000)
#		include <time.h>
#	endif
#endif

typedef unsigned DEVICE_CLASSIFICATION;
const DEVICE_CLASSIFICATION DEVICE_UNKNOWN			= 0x0000;
const DEVICE_CLASSIFICATION DEVICE_DRIVE			= 0x1000;
const DEVICE_CLASSIFICATION DEVICE_DRIVE_FIXED		= 0x1001;
const DEVICE_CLASSIFICATION DEVICE_DRIVE_REMOVABLE	= 0x1002;
const DEVICE_CLASSIFICATION DEVICE_DRIVE_REMOTE		= 0x1004;
const DEVICE_CLASSIFICATION DEVICE_DIRECTORY		= 0x2000;
const DEVICE_CLASSIFICATION DEVICE_FILE				= 0x4000;

class DEVICE_INFO
{
	friend class DIRECTORY_SERVICE;
public:
	DEVICE_CLASSIFICATION classification;
	char name[32];
	UI_BIGNUM size;
	UI_DATE date;
	UI_TIME time;

	int Inherited(DEVICE_CLASSIFICATION _classification)
		{ return (((classification & _classification) == _classification) ? TRUE : FALSE) ; }

private:
	int reserved;
#if defined(ZIL_POSIX)
	struct dirent *_reserved;
#elif defined(WIN32)
	WIN32_FIND_DATA _reserved;
#elif defined(ZIL_OS2)
	FILEFINDBUF3 _reserved;
#elif defined(__BCPLUSPLUS__) | defined(__TCPLUSPLUS__)
	struct ffblk _reserved;
#elif defined(_MSC_VER) || defined(__WATCOMC__)
	struct _find_t _reserved;
#elif defined(__ZTC__)
	struct FIND *_reserved;
#endif
};

class DIRECTORY_SERVICE : public UIW_WINDOW
{
public:
	EVENT_TYPE command;
	UI_WINDOW_OBJECT *requestor;

	DIRECTORY_SERVICE(UI_WINDOW_OBJECT *requestor);
	~DIRECTORY_SERVICE(void);
	EVENT_TYPE Event(const UI_EVENT &event);

	int DeviceInfo(char *pathName, DEVICE_INFO &info);
	void DeviceName(char *deviceName);
	void PathName(char *pathName);

private:
	int originalDrive;
	int currentDrive;
	char originalPath[MAXPATHLEN];
	char currentPath[MAXPATHLEN];

	static void Close(void);
	static int DriveError(int drive);
	static int FindFirst(const char *pathName, DEVICE_INFO &info, DEVICE_CLASSIFICATION classification);
	static int GetDrive(void);
	static int GetPath(char *pathName);
	static int FindNext(DEVICE_INFO &info);
	static void Open(void);
	static int SetDrive(int drive);
	static int SetPath(char *pathName);

	void DirectoryUpdate(void);
	void DriveUpdate(void);
	void FileUpdate(void);
};

#endif // DIRECT_HPP

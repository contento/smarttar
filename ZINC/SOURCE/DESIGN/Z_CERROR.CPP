//	Zinc Interface Library Designer - Z_ERROR.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include <string.h>
#include <stdlib.h>
#include "ui_dsn.hpp"
#if defined(ZIL_MSDOS) || defined(ZIL_MSWINDOWS) || defined(ZIL_OS2) || defined(__DVX__)
#	if defined(__ZTC__)
#		include <cerror.h>
#	elif defined(_MSC_VER) || defined(__WATCOMC__)
#		include <new.h>
#	elif defined(__BCPLUSPLUS__) || defined(__TCPLUSPLUS__)
#		include <dir.h>
#	endif
#	include <direct.h>
#	if !defined(__IBMCPP__)
#		include <dos.h>
#	endif
#elif defined(ZIL_UNIX)
#	include <unistd.h>
#endif
#if defined(_MSC_VER)
#	pragma hdrstop				// Microsoft pre-compiled header pragma.
#endif

#define IGNORE			0
#define RETRY			1
#define ABORT			2

static DOS dummy;  // Force constructor / destructor calls.
int DOS::criticalError = FALSE;
int DOS::errorCode;
int DOS::errorDrive;
char *DOS::errorMemory = NULL;
#if defined(ZIL_MSDOS) || defined(ZIL_MSWINDOWS) || defined(ZIL_OS2)
int DOS::startupDrive;
#endif
char DOS::startupDir[MAX_PATH];

#if defined(ZIL_MSDOS)
// Error messages used by the error handler.
static char *errorMessage[] =
{
	"Disk write protect error",
	"Unknown disk drive",
	"Drive not ready error",
	"Unknown command error",
	"CRC data error",
	"Bad drive request structure length",
	"Disk seek error",
	"Unknown disk format error",
	"Disk sector not found",
	"Printer out of paper",
	"Unable to write data",
	"Unable to read data",
	"General hardware error",
	"Unknown hardware error",
	"Unknown hardware error",
	"Invalid disk change"
};
#endif

#if defined(ZIL_MSDOS) && defined(_MSC_VER)
//extern "C" void __cdecl xCriticalErrorHandler(unsigned deverror, unsigned errcode)
extern "C" void __cdecl xCriticalErrorHandler(void)
{
	DOS::CriticalErrorHandler(0, 0); //deverror, errcode);
}
extern "C" int __cdecl xNewHandler(size_t size)
{
	return DOS::NewHandler(size);
}
#endif

DOS::DOS(void)
{
#if defined(ZIL_MSDOS)
	errorMemory = new char[8192];
#if defined(__BCPLUSPLUS__) | defined(__TCPLUSPLUS__)
	extern void (*_new_handler)(void);
	_new_handler = DOS::NewHandler;
   	harderr(DOS::CriticalErrorHandler);
#elif defined(__ZTC__)
	extern void (*_new_handler)(void);
	_new_handler = NewHandler;
	_cerror_handler = DOS::CriticalErrorHandler;
	cerror_open();
#elif defined(_MSC_VER)
	_set_new_handler(xNewHandler);
	_harderr(xCriticalErrorHandler);
#elif defined(__WATCOMC__)
	extern void (*_new_handler)(void);
	set_new_handler(DOS::NewHandler);
	_harderr(DOS::CriticalErrorHandler);
#endif
#endif
#if defined(ZIL_MSDOS) || defined(ZIL_MSWINDOWS) || defined(ZIL_OS2)
#if defined(__BCPLUSPLUS__) | defined(__TCPLUSPLUS__)
	startupDrive = getdisk();
#elif defined(__ZTC__)
	dos_getdrive(&(unsigned)startupDrive);
#endif
#endif
	getcwd(startupDir, MAX_PATH);
}

DOS::~DOS(void)
{
#if defined(ZIL_MSDOS)
	if (errorMemory)
		delete errorMemory;
#endif
#if defined(ZIL_MSDOS) || defined(ZIL_MSWINDOWS) || defined(ZIL_OS2)
#if defined(__BCPLUSPLUS__) | defined(__TCPLUSPLUS__)
	setdisk(startupDrive);
#elif defined(__ZTC__)
	unsigned temp;
	dos_setdrive((unsigned)startupDrive, &temp);
#endif
#endif
	chdir(startupDir);
}

#if defined(ZIL_MSDOS)
// Set up critical error handler.
#if defined(__ZTC__)
int _cdecl DOS::CriticalErrorHandler(int *ax, int *di)
{
	criticalError = TRUE;
	errorDrive = *ax & 0x00FF;
	errorCode = *di & 0x00FF;
	*ax = ABORT;
	return ABORT;
}
#elif defined(_MSC_VER)
void DOS::CriticalErrorHandler(unsigned deverror, unsigned errcode)
{
	criticalError = TRUE;
	errorDrive = deverror & 0x00FF;
	errorCode = errcode & 0x00FF;
	_hardretn(ABORT);
	return;
}
#elif defined(__WATCOMC__)
int DOS::CriticalErrorHandler(unsigned deverr, unsigned errcode, unsigned far *)
{
	criticalError = TRUE;
	errorDrive = deverr & 0x00FF;
	errorCode = errcode & 0x00FF;
#if !defined(__386__)
	_hardretn(ABORT);
#endif
	return (ABORT);
}
#elif defined(__BCPLUSPLUS__) || defined(__TCPLUSPLUS__)
int DOS::CriticalErrorHandler(int errval, int ax, int, int)
{
	criticalError = TRUE;
	errorDrive = ax & 0x00FF;
	errorCode = errval & 0x00FF;
#if !defined(DOSX286)
	hardretn(ABORT);
#endif
	return ABORT;
}
#endif
#endif

// Get error responce from user.
EVENT_TYPE DOS::ErrorResponce()
{
#if defined(ZIL_MSDOS)
	DIALOG_WINDOW dialog("Disk Error!", "EXCLAMATION", DIF_ABORT | DIF_RETRY,
		"%s on drive %c.", errorMessage[errorCode], 'A' + errorDrive);
	return dialog.Responce();
#elif defined(ZIL_MSWINDOWS) || defined(ZIL_OS2) || defined(ZIL_MOTIF)
	return DIF_ABORT;
#endif
}

#if defined(ZIL_MSDOS)
// Setup free-store exception handler.
#if defined(__ZTC__) || defined(__BCPLUSPLUS__) | defined(__TCPLUSPLUS__)
void DOS::NewHandler(void)
#elif defined(_MSC_VER)
int DOS::NewHandler(size_t size)
#elif defined(__WATCOMC__)
void DOS::NewHandler(void)
#endif
{
	// Free enouph memory for reporting error and saving file.
	delete errorMemory;

	// Report out of memory error.
	_errorSystem->Beep();
	char path[128];
	char *errstr;
	int doOk;
	if (CONTROL_WINDOW::storage)
	{
		CONTROL_WINDOW::storage->StorageName(path);
		errstr = "Out of memory!\r\n\r\nZinc designer will terminate.\r\n\r\nSave file %s before termination?";
		doOk = DIF_YES | DIF_NO;
	}
	else
	{
		errstr = "Out of memory!\r\n\r\nZinc designer will terminate.";
		doOk = DIF_OK;
	}
	DIALOG_WINDOW dialog("Memory Error!", NULL, doOk, errstr, path);

	if (dialog.Responce() == DIALOG_YES)
	{
		// Free memory occupied by un-needed objects.
		UI_WINDOW_OBJECT *object = UI_WINDOW_OBJECT::windowManager->First();
		while (object)
		{
			UI_WINDOW_OBJECT *tObject = object->Next();
			if (!FlagSet(object->woStatus, WOS_EDIT_MODE))
			{
				*UI_WINDOW_OBJECT::windowManager - object;
				delete object;
			}
			object = tObject;
		}

		// Save the file.
		CONTROL_WINDOW::SaveFile(NULL, TRUE);
	}

	// Emergency clean up and exit.
	UI_EVENT_MANAGER *eventManager = UI_WINDOW_OBJECT::eventManager;
	UI_DISPLAY *display = UI_WINDOW_OBJECT::display;
	delete UI_WINDOW_OBJECT::windowManager;
	delete eventManager;
	delete display;
	exit(0);
#if defined(_MSC_VER)
	return 0;
#endif
}
#endif

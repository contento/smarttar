//	DIRECT.CPP (DIRECT) - File directory service object.
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA
//  May be freely copied, used and distributed.

// For information about the directory service class please refer to the
// proper section of the class.  The following DIRECTORY_SERVICE sections 
// are contained in this file:
// ---	DIRECTORY_SERVICE
// ---	directory support functions
// ---	drive support functions
// ---	file support functions
// ---	filter support functions
// ---	miscellaneous compiler-dependent functions

#define USE_DRIVE_ERROR
#define USE_DIRECTORY_SERVICE
#include "direct.hpp"
#include "dir_win.hpp"

#define CURRENT_DIRECTORY			"."
#define PARENT_DIRECTORY			".."
static char *_tResourceName	 		= "DIRECTORY_SERVICE_TXT";
#if defined(ZIL_MSDOS) || defined(ZIL_MSWINDOWS) || defined(__DVX__)
#	define ALL_DEVICES				"*.*"
	static char *_separator			= "\\";
	static char *_gResourceName	 	= "DIRECTORY_SERVICE";
#	if defined(WIN32)
#		define TIME_DAY 			8640000L
		static HANDLE _directory	= 0;
#	endif
#elif defined(ZIL_OS2)
#	define ALL_DEVICES				"*.*"
	static char *_separator			= "\\";
	static HDIR _directory			= HDIR_SYSTEM;
	static char *_gResourceName	 	= "DIRECTORY_SERVICE_OS2";
#elif defined(ZIL_POSIX)
#	define ALL_DEVICES				"*"
	static char *_separator			= "/";
	static DIR *_directory			= NULL;
	static char *_gResourceName	 	= "DIRECTORY_SERVICE_MTF";
#endif

static NUMBERID DriveCharToId(char drive) { return (10100 + drive - 'a'); }
static NUMBERID DriveIntToId(int drive) { return (10100 + drive); }
static int DriveCharToInt(char drive) { return (tolower(drive) - 'a'); }
static char DriveIntToChar(int drive) { return (drive + 'a'); }

// ----- DIRECTORY_SERVICE --------------------------------------------------

// This section contains all the public members of the directory service 
// class. Communication with applications is done by setting the 
// directory->command variable to a non-zero application based message
// (e.g. application specified OPTION_OPEN, OPTION_SAVE_AS, OPTION_DELETE,
// and OPTION_STATISTICS) and then adding the directory service to the 
// window manager.  When a file is selected the directory service responds 
// by sending the directory->command value back to the requestor via the
// requestor's Event() function.  The selected file can be retrieved by 
// calling directory->PathName().  Notice that the directory class is set 
// up as a no-destroy window.  This means the application must create and 
// destroy the directory service class.  This allows us to maintain directory 
// and file information throughout the scope of an application.
// ---	DIRECTORY_SERVICE::DIRECTORY_SERVICE - creates the directory service
//		class.  This class reads the window from the defaultStorage data file.
//		The name of the window depends on the run time environment as 
//		specified by the _tResourceName or _gResourceName variable.
// ---	DIRECTORY_SERVICE::~DIRECTORY_SERVICE - destroys the directory 
//		service class.  This member restores the original drive and path.
// ---	DeviceName - retreives the name of the current device selected in 
//		the class.
// ---	Event - Processes the window events dispatced from the windowManager.
//		In addition to normal window messages, this function also processes
//		those messages passed by the drive, directory, file and filter 
//		callback functions.
// ---	PathName - retreives the full path-name of the current device 
//		selected in the class.

const EVENT_TYPE OPTION_DIRECTORY_OK	= 10001;
const EVENT_TYPE OPTION_DIRECTORY_CANCEL= 10002;
const EVENT_TYPE OPTION_DIRECTORY_HELP	= 10003;
const EVENT_TYPE OPTION_RESET_DRIVE		= 10004;
const EVENT_TYPE OPTION_RESET_DIRECTORY	= 10005;
const EVENT_TYPE OPTION_RESET_FILE		= 10006;
const EVENT_TYPE OPTION_RESET_FILTER	= 10007;

DIRECTORY_SERVICE::DIRECTORY_SERVICE(UI_WINDOW_OBJECT *_requestor) :
	UIW_WINDOW((display && display->isText) ? _tResourceName : _gResourceName, defaultStorage),
	requestor(_requestor), command(0)
{
	if (windowManager)
		windowManager->Center(this);
	originalDrive = currentDrive = GetDrive();
	GetPath(originalPath);

	DEVICE_INFO device;
	UI_EVENT event(S_ADD_OBJECT);
	woAdvancedFlags |= WOAF_NO_DESTROY | WOAF_MODAL;
	strcpy(currentPath, originalPath);

	// Set up the directory and filter strings.
	UI_WINDOW_OBJECT *directory = Get(DIRECTORY_STRING);
	if (directory)
		directory->Information(SET_TEXT, currentPath);

	UI_WINDOW_OBJECT *file = Get(FILE_STRING);
	if (file)
		file->Information(SET_TEXT, ALL_DEVICES);

	UI_WINDOW_OBJECT *filter = Get(FILTER_LIST);
	if (filter)
		filter->Information(SET_TEXT, ALL_DEVICES);

	// Set up the drive, directory and file fields.
	DriveUpdate();
	DirectoryUpdate();
	FileUpdate();
}

DIRECTORY_SERVICE::~DIRECTORY_SERVICE(void)
{
	// Restore the original drive and directory.
	SetDrive(originalDrive);
	SetPath(originalPath);
}

void DIRECTORY_SERVICE::DeviceName(char *deviceName)
{
	// Retrieve the current device name.
	char *name;
	UI_WINDOW_OBJECT *object;
	object = Get(FILE_STRING);
	object->Information(GET_TEXT, &name);
	strcpy(deviceName, name);
}

EVENT_TYPE DIRECTORY_SERVICE::Event(const UI_EVENT &event)
{
	EVENT_TYPE ccode = event.type;
	switch (ccode)
	{
	case OPTION_DIRECTORY_CANCEL:
		eventManager->Put(S_CLOSE); 	// Close the window.
		break;

	case OPTION_DIRECTORY_OK:
		requestor->Event(command); // Send a response to the requestor.
		break;

	case OPTION_RESET_DRIVE:
	case OPTION_RESET_DIRECTORY:
		{
		char *data = (char *)event.data;
		if (ccode == OPTION_RESET_DRIVE)
		{
			if (SetDrive(DriveCharToInt(*data)))
			{
				UI_EVENT event(S_ADD_OBJECT);
				event.data = Get(DriveIntToId(currentDrive));
				UI_WINDOW_OBJECT *drive = Get(DRIVE_LIST);
				if (drive && event.data)
					drive->Event(event); // Restore the previous drive.
				break;
			}
		}
		else
		{
			if (SetPath(data))
				break;
		}
		currentDrive = GetDrive();
		GetPath(currentPath);
		UI_WINDOW_OBJECT *directory = Get(DIRECTORY_STRING);
		if (directory)
			directory->Information(SET_TEXT, currentPath);
		DirectoryUpdate();
		FileUpdate();
		}
		break;

	case OPTION_RESET_FILE:
		{
		UI_WINDOW_OBJECT *file = Get(FILE_STRING);
		if (file)
			file->Information(SET_TEXT, event.data);
		}
		break;

	case OPTION_RESET_FILTER:
		{
		FileUpdate();
		char *filterString = ALL_DEVICES;
		UI_WINDOW_OBJECT *filter = Get(FILTER_LIST);
		if (filter)
			filter->Information(GET_TEXT, &filterString);
		UI_WINDOW_OBJECT *file = Get(FILE_STRING);
		if (file)
			file->Information(SET_TEXT, filterString);
		}
		break;

	case OPTION_DIRECTORY_HELP:
		if (helpSystem)
			helpSystem->DisplayHelp(windowManager, helpContext);
		break;

	default:
		ccode = UIW_WINDOW::Event(event);
		break;
	}

	return (ccode);
}

void DIRECTORY_SERVICE::PathName(char *pathName)
{
	// Retrieve the current directory and device name.
	char *name;
	UI_WINDOW_OBJECT *object;
	object = Get(FILE_STRING);
	object->Information(GET_TEXT, &name);
	int length = ui_strlen(currentPath);
	if (!length || currentPath[length-1] != *_separator)
		sprintf(pathName, "%s%s%s", currentPath, _separator, name);
	else
		sprintf(pathName, "%s%s", currentPath, name);
}

// ----- directory support functions ----------------------------------------

// The directory support functions are used to reset the current directory
// and the sort directory entries.
// ---	DirectoryCallback - sends a message to the DIRECTORY_SERVICE::Event()
//		to reset the current directory.  The name of the directory is 
//		contained in event.data.
// ---	DirectoryCompare - compares the two directory entries.  If the object
//		is a DEVICE_DIRECTORY it does not change its list position.  Otherwise
//		this function calls the standard string compare on the objects.
// ---	ParentDirectoryCallback - concatenates the directory paths to form
//		a new directory.  This function differs from DirectoryCallback because
//		it must create a full path-name to the directory based on its entry
//		and all associated parent directories.
// ---	DirectoryUpdate - updates the list of directory entries based on the
//		current directory.

EVENT_TYPE DirectoryCallback(UI_WINDOW_OBJECT *object, UI_EVENT &event, EVENT_TYPE ccode)
{
	if (ccode == L_DOUBLE_CLICK || object->LogicalEvent(event) == L_SELECT)
	{
		event.type = OPTION_RESET_DIRECTORY;
		event.data = object->Information(GET_TEXT, NULL);
		object->eventManager->Put(event);
	}
	return (ccode);
}

int DirectoryCompare(void *object1, void *object2)
{
	if (((UI_WINDOW_OBJECT *)object1)->userFlags)
		return (-1);
	else if (((UI_WINDOW_OBJECT *)object2)->userFlags)
		return (1);
	return (UIW_WINDOW::StringCompare(object1, object2));
}

EVENT_TYPE ParentDirectoryCallback(UI_WINDOW_OBJECT *object, UI_EVENT &event, EVENT_TYPE ccode)
{
	static char _fullPath[MAXPATHLEN];
	if (ccode == L_DOUBLE_CLICK || object->LogicalEvent(event) == L_SELECT)
	{
		event.type = OPTION_RESET_DIRECTORY;
		UI_WINDOW_OBJECT *tObject = object;
		while (tObject->Previous())
			tObject = tObject->Previous();
		*_fullPath = NULL;
		while (tObject != object)
		{
			strcat(_fullPath, (char *)tObject->Information(GET_TEXT, NULL));
			strcat(_fullPath, _separator);
			tObject = tObject->Next();
		}
		strcat(_fullPath, (char *)object->Information(GET_TEXT, NULL));
		if (!object->Previous())
			strcat(_fullPath, _separator);
		event.data = _fullPath;
		object->eventManager->Put(event);
	}
	return (ccode);
}

void DIRECTORY_SERVICE::DirectoryUpdate(void)
{
	DEVICE_INFO device;
	UI_EVENT event(S_ADD_OBJECT);

	// Set up the Directory field.
	UI_WINDOW_OBJECT *directory = Get(DIRECTORY_LIST);
	if (directory)
	{
		directory->Information(DESTROY_LIST, NULL);

		int indentation = 0;
		UIW_BUTTON *cDirectoryField = NULL;
		if (*currentPath)
		{
			char subString[MAXPATHLEN];
			strcpy(subString, currentPath);
			int length = ui_strlen(subString);
			ui_strrepc(subString, *_separator, NULL);
			for (int i = 0; i < length; )
			{
				int j = i + ui_strlen(&subString[i]) + 1;
				cDirectoryField = new UIW_BUTTON(indentation++, 0, 22, 
					&subString[i], BTF_NO_3D | BTF_DOUBLE_CLICK,
					WOF_NO_FLAGS, ParentDirectoryCallback, 0,
					(j < length) ? "parentDirectory" : "currentDirectory");
				if (j >= length)
					cDirectoryField->woStatus |= WOS_SELECTED;
				cDirectoryField->userFlags = DEVICE_DIRECTORY;
				event.data = cDirectoryField;
				directory->Event(event);
				i = j;
			}
		}

		Open();
		for (int error = FindFirst(ALL_DEVICES, device, DEVICE_DIRECTORY); !error; error = FindNext(device))
			if (device.Inherited(DEVICE_DIRECTORY) &&
				strcmp(device.name, CURRENT_DIRECTORY) &&
				strcmp(device.name, PARENT_DIRECTORY))
			{
				event.data = new UIW_BUTTON(indentation, 0, 22, device.name,
					BTF_NO_3D | BTF_DOUBLE_CLICK, WOF_NO_FLAGS,
					DirectoryCallback, 0, "childDirectory");
				directory->Event(event);
			}
		Close();

		if (cDirectoryField)
		{
			event.data = cDirectoryField;
			directory->Event(event);
		}
		if (directory->screenID)
			directory->Event(S_REDISPLAY);
	}
}

// ----- drive support functions --------------------------------------------

// The drive support functions are used to reset the current drive (if any).
// These functions are stubbed for systems that do not support drives.
// ---	DriveCallback - sends a message to the DIRECTORY_SERVICE::Event()
//		to reset the current drive.  The name of the drive is contained 
//		in event.data.
// ---	DriveUpdate - updates the list of drive entries available to the
//		application.

EVENT_TYPE DriveCallback(UI_WINDOW_OBJECT *object, UI_EVENT &event, EVENT_TYPE ccode)
{
	if (ccode == L_SELECT)
	{
		event.type = OPTION_RESET_DRIVE;
		event.data = object->Information(GET_TEXT, NULL);
		object->eventManager->Put(event);
	}
	return (ccode);
}

void DIRECTORY_SERVICE::DriveUpdate(void)
{
	DEVICE_INFO device;
	UI_EVENT event(S_ADD_OBJECT);

	// Set up the Drive field.
	UI_WINDOW_OBJECT *drive = Get(DRIVE_LIST);
	if (drive)
	{
		drive->Information(DESTROY_LIST, NULL);

		Open();
		void *cDriveField = NULL;
		for (int error = FindFirst(ALL_DEVICES, device, DEVICE_DRIVE); !error; error = FindNext(device))
		{
			char *bitmapName = "hardDrive";
			if (device.Inherited(DEVICE_DRIVE_REMOTE))
				bitmapName = "networkDrive";
			else if (device.Inherited(DEVICE_DRIVE_REMOVABLE))
				bitmapName = "softDrive";
			UIW_BUTTON *button = new UIW_BUTTON(0, 0, 22, device.name,
				BTF_NO_3D, WOF_NO_FLAGS, DriveCallback, 0, bitmapName);
			button->NumberID(DriveCharToId(*device.name));
			event.data = button;
			if (DriveCharToInt(*device.name) == currentDrive)
				cDriveField = event.data;
			drive->Event(event);
		}
		Close();

		if (cDriveField)
		{
			event.data = cDriveField;
			drive->Event(event);
		}
		if (drive->screenID)
			drive->Event(S_REDISPLAY);
	}
}

// ----- file support functions ---------------------------------------------

// The file support functions are used to reset the files available in the
// current directory.
// ---	FileCallback - sends a message to the DIRECTORY_SERVICE::Event()
//		to reset the current file.  The name of the file is contained 
//		in event.data.  If the field is double-click'd this function
//		tells the directory service class to select the file and pass
//		back completion to the applcation.
// ---	FileUpdate - updates the list of file entries available to the
//		application based on the current directory.

static EVENT_TYPE FileCallback(UI_WINDOW_OBJECT *object, UI_EVENT &event, EVENT_TYPE ccode)
{
	if (ccode == L_DOUBLE_CLICK || object->LogicalEvent(event) == L_SELECT)
	{
		event.type = OPTION_RESET_FILE;
		event.data = object->Information(GET_TEXT, NULL);
		object->eventManager->Put(event);
		event.type = OPTION_DIRECTORY_OK;
		object->eventManager->Put(event);
	}

	return (ccode);
}

void DIRECTORY_SERVICE::FileUpdate(void)
{
	DEVICE_INFO device;
	UI_EVENT event(S_ADD_OBJECT);

	// Set up the File field.
	UI_WINDOW_OBJECT *file = Get(FILE_LIST);
	if (file)
	{
		file->Information(DESTROY_LIST, NULL);

		char *filterString = ALL_DEVICES;
		UI_WINDOW_OBJECT *filter = Get(FILTER_LIST);
		if (filter)
			filter->Information(GET_TEXT, &filterString);

		Open();
		for (int error = FindFirst(ALL_DEVICES, device, DEVICE_FILE); !error; error = FindNext(device))
			if (device.Inherited(DEVICE_FILE) &&
				!ui_WildStrcmp(device.name, filterString))
			{
				event.data = new UIW_BUTTON(0, 0, 22, device.name,
					BTF_NO_3D | BTF_DOUBLE_CLICK, WOF_NO_FLAGS, FileCallback);
				file->Event(event);
			}
		Close();

		if (file->screenID)
			file->Event(S_REDISPLAY);
	}
}

// ----- filter support functions -------------------------------------------

// The filter support functions are used to reset the filters (wild-card 
// matching).
// ---	FilterCallback - sends a message to the DIRECTORY_SERVICE::Event()
//		to reset the wild-card file matching pattern.  The name of the filter
//		is contained in event.data.

EVENT_TYPE FilterCallback(UI_WINDOW_OBJECT *object, UI_EVENT &event, EVENT_TYPE ccode)
{
	if (ccode == L_SELECT)
	{
		event.type = OPTION_RESET_FILTER;
		event.data = object->Information(GET_TEXT, NULL);
		object->eventManager->Put(event);
	}
	return (ccode);
}

// ----- filename support function ------------------------------------------

// The filename support function is used to select the specified file
// for the application.
// ---	FilenameCallback - sends a message to the DIRECTORY_SERVICE::Event()
//		to select the current specified file.

EVENT_TYPE FilenameCallback(UI_WINDOW_OBJECT *object, UI_EVENT &event, EVENT_TYPE ccode)
{
	if (ccode == L_SELECT)
	{
		event.type = OPTION_DIRECTORY_OK;
		object->eventManager->Put(event);
	}
	return (ccode);
}

// ----- miscellaneous compiler-dependent functions -------------------------

// The miscellaneous compiler-dependent functions are used to get system
// information for the application.
// ---	systemerror - is a DOS hardware dependent function that detects drive
//		errors.
// ---	ioctl - is a DOS hardware dependent function that indicates the
//		validity and type of drive available on the system.
// ---	drivetype - is a hardware dependent function that returns the type
//		of drive based on the specified drive value.
// ---	Close - is used to close the current directory.  This function
//		is a stub for most operating systems.
// ---	DeviceInfo - retreives the device information for the current device
//		selected in the class.
// ---	DriveError - is used to display an error window when a drive error
//		is detected.  This function only displays a window in the DOS 
//		environment.  All other environments have operating system level
//		support for drive errors.
// ---	FindFirst - is used to find the first drive, directory or file given
//		a specified path.
// ---	FindNext - is used to find the next drive, directory or file based
//		on the results of FindFirst.
// ---	GetDrive - gets the current drive in the application.
// ---	GetPath - gets the current working directory in the application.
// ---	Open - opens the current directory for read capabilities.
// ---	SetDrive - sets the current drive in the application.
// ---	SetPath - sets the current working directory in the application.

#if defined(ZIL_MSDOS) || defined(__DVX__)

#define DRIVE_ABORT 		2

#define IOCTL_CHANGEABLE	8
#define IOCTL_LOCAL_REMOTE	9

#if defined(__BCPLUSPLUS__) | defined(__TCPLUSPLUS__)
static int systemerror(int, int, int, int)
{
#if !defined(DOSX286)
	hardretn(DRIVE_ABORT);
#endif
	return (DRIVE_ABORT);
}
#elif defined(_MSC_VER)
static void __cdecl systemerror(void)
{
	_hardretn(DRIVE_ABORT);
}
#elif defined(__WATCOMC__)
static int __far systemerror(unsigned, unsigned, unsigned far *)
{
	return (DRIVE_ABORT);
}
#elif defined(__ZTC__)
static int __cdecl systemerror(int *, int *)
{
	return (DRIVE_ABORT);
}
#endif

static int ioctl(int handle, int functionID)
{
#if defined(_MSC_VER)
	union _REGS regs;
#else
	union REGS regs;
#endif

	// Check for a valid function number (8 or 9).
	if (functionID != 8 && functionID != 9)
		return (-1);
	regs.h.ah = 0x44;
	regs.h.al = functionID;
	regs.h.bl = handle;
#if defined(__ZTC__) && defined(DOS386)
	int86_real(0x21, &regs, &regs);
	return (regs.x.cflag ? -1 : (functionID == 8 ? regs.x.ax : regs.x.dx));
#elif defined(_MSC_VER)
	_int86(0x21, &regs, &regs);
	return (regs.x.cflag ? -1 : (functionID == 8 ? regs.x.ax : regs.x.dx));
#elif defined(__WATCOMC__)
	int386(0x21, &regs, &regs);
	return (regs.x.cflag ? -1 : (functionID == 8 ? regs.w.ax : regs.w.dx));
#else
	int86(0x21, &regs, &regs);
	return (regs.x.cflag ? -1 : (functionID == 8 ? regs.x.ax : regs.x.dx));
#endif
}

static DEVICE_CLASSIFICATION drivetype(int drive)
{
	// Set the critical error handler.
	static int seterror = TRUE;
	if (seterror)
	{
#if defined(__BCPLUSPLUS__) | defined(__TCPLUSPLUS__)
		harderr(systemerror);
#elif defined(_MSC_VER) || defined(__WATCOMC__)
		_harderr(systemerror);
#elif defined(__ZTC__)
		_cerror_handler = systemerror;
		cerror_open();
#endif
		seterror = FALSE;
	}

	// Determine the drive type.
	DEVICE_CLASSIFICATION driveType = DEVICE_UNKNOWN;
	int error = ioctl(drive + 1, IOCTL_LOCAL_REMOTE);
	if (error == -1)
		;
	else if (FlagSet(error, 0x1000))
		driveType = DEVICE_DRIVE_REMOTE;
	else if (ioctl(drive + 1, IOCTL_CHANGEABLE) == 1)
		driveType = DEVICE_DRIVE_FIXED;
	else
		driveType = DEVICE_DRIVE_REMOVABLE;
	return (driveType);
}
#elif defined(ZIL_MSWINDOWS)
static DEVICE_CLASSIFICATION drivetype(int drive)
{
#if defined(WIN32)
	char sDrive[MAXPATHLEN];
	sprintf(sDrive, "%c:%s", DriveIntToChar(drive), _separator);
	DEVICE_CLASSIFICATION driveType = GetDriveType(sDrive);
#else
	DEVICE_CLASSIFICATION driveType = GetDriveType(drive);
#endif
	if (driveType == DRIVE_FIXED)
		driveType = DEVICE_DRIVE_FIXED;
	else if (driveType == DRIVE_REMOVABLE)
		driveType = DEVICE_DRIVE_REMOVABLE;
	else if (driveType == DRIVE_REMOTE)
		driveType = DEVICE_DRIVE_REMOTE;
	else
		driveType = DEVICE_UNKNOWN;
	return (driveType);
}
#elif defined(ZIL_OS2)
static DEVICE_CLASSIFICATION drivetype(int drive)
{
	static BYTE floppyDrives = 0, hardDrives = 0;
	if (!floppyDrives && !hardDrives)
	{
		DosDevConfig(&floppyDrives, DEVINFO_FLOPPY);
		hardDrives = 'f' - 'a' + 1;
	}

	DEVICE_CLASSIFICATION driveType = DEVICE_UNKNOWN;
	ULONG temp, logical;
	DosQueryCurrentDisk(&temp, &logical);
	if (DosSetDefaultDisk(drive + 1) != ERROR_INVALID_DRIVE)
		DosSetDefaultDisk(temp);
	else
		return (driveType);
	if (drive < floppyDrives)
		driveType = DEVICE_DRIVE_REMOVABLE;
	else if (drive < hardDrives)
		driveType = DEVICE_DRIVE_FIXED;
	else
		driveType = DEVICE_DRIVE_REMOTE;
	return (driveType);
}
#elif defined(ZIL_POSIX)
static DEVICE_CLASSIFICATION drivetype(int)
{
	// POSIX systems do not have drives.
	return (DEVICE_UNKNOWN);
}
#endif

void DIRECTORY_SERVICE::Close(void)
{
#if defined(ZIL_POSIX)
	closedir(_directory);
#elif defined(ZIL_OS2)
	DosFindClose(_directory);
#endif
}

int DIRECTORY_SERVICE::DeviceInfo(char *fullName, DEVICE_INFO &info)
{
	// Check for NULL pathnames.
	char pathName[MAXPATHLEN];
	if (!fullName)
	{
		PathName(pathName);
		fullName = pathName;
	}

#if defined(ZIL_POSIX)
	struct stat statx;
	int error = stat(fullName, &statx);
	if (!error)
	{
		info.classification = S_ISDIR(statx.st_mode) ? DEVICE_DIRECTORY : DEVICE_FILE;
		UI_STORAGE::StripFullPath(fullName, NULL, info.name, NULL, NULL);
		info.size.Import(statx.st_size);
		struct tm *time = localtime(&statx.st_mtime);
		info.date.Import(1900 + time->tm_year, time->tm_mon + 1, time->tm_mday);
		info.time.Import(time->tm_hour, time->tm_min, time->tm_sec);
		return (TRUE);
	}
#elif defined(ZIL_OS2)
	_directory = HDIR_SYSTEM;
	ULONG oneBlock = 1;
	int error = DosFindFirst((PSZ)fullName, &_directory, FILE_DIRECTORY,
		&info._reserved, sizeof(info._reserved), &oneBlock, FIL_STANDARD) ? -1 : 0;
	if (!error)
	{
		info.classification = (info._reserved.attrFile == FILE_DIRECTORY) ? DEVICE_DIRECTORY : DEVICE_FILE;
		strcpy(info.name, info._reserved.achName);
		ui_strlwr(info.name);
		info.size.Import((long)info._reserved.cbFile);
		info.date.Import(1980 + info._reserved.fdateLastWrite.year,
			info._reserved.fdateLastWrite.month,
			info._reserved.fdateLastWrite.day);
		info.time.Import(info._reserved.ftimeLastWrite.hours,
			info._reserved.ftimeLastWrite.minutes,
			info._reserved.ftimeLastWrite.twosecs * 2);
	}
#elif defined(WIN32)
	_directory = FindFirstFile(fullName, &info._reserved);
	int error = (_directory == INVALID_HANDLE_VALUE) ? -1 : 0;
	if (!error)
	{
		info.classification = (info._reserved.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY) ? DEVICE_DIRECTORY : DEVICE_FILE;
		strcpy(info.name, info._reserved.cFileName);
		info.size.Import((long)info._reserved.nFileSizeLow);
		ui_strlwr(info.name);
		WORD dateCode, timeCode;
		FileTimeToDosDateTime(&info._reserved.ftLastWriteTime, &dateCode, &timeCode);

		// Adjust for GMT.
		TIME_ZONE_INFORMATION tZInfo;
		GetTimeZoneInformation(&tZInfo);
		info.date.Import(dateCode);
		info.time.Import(timeCode);
		UI_TIME bias;
		int cYear, cMonth, cDay;
		UI_DATE cDate;
		cDate.Export(&cYear, &cMonth, &cDay);
		if ((cMonth > tZInfo.DaylightDate.wMonth ||
			cMonth == tZInfo.DaylightDate.wMonth && cDay >= tZInfo.DaylightDate.wDay) &&
			(cMonth < tZInfo.StandardDate.wMonth ||
			cMonth == tZInfo.StandardDate.wMonth && cDay < tZInfo.StandardDate.wDay))
				tZInfo.Bias += tZInfo.DaylightBias;
		else
			tZInfo.Bias += tZInfo.StandardBias;
		if (tZInfo.Bias < 0)
		{
			UI_TIME diff;
			bias.Import(-tZInfo.Bias / 60, -tZInfo.Bias % 60);
			diff = UI_TIME(23, 59, 59, 99) - bias;
			if (diff < info.time)
			{
				++info.date;
				info.time -= TIME_DAY;
			}
			info.time = info.time + bias;
		}
		else
		{
			bias.Import(tZInfo.Bias / 60, tZInfo.Bias % 60);
			if (bias > info.time)
			{
				--info.date;
				info.time += TIME_DAY;
			}
			info.time = info.time - bias;
		}
	}
#elif defined(__BCPLUSPLUS__) | defined(__TCPLUSPLUS__)
	int error = ::findfirst(fullName, &info._reserved, FA_DIREC);
	if (!error)
	{
		info.classification = (info._reserved.ff_attrib == FA_DIREC) ? DEVICE_DIRECTORY : DEVICE_FILE;
		strcpy(info.name, info._reserved.ff_name);
		ui_strlwr(info.name);
		info.size.Import(info._reserved.ff_fsize);
		info.date.Import(info._reserved.ff_fdate);
		info.time.Import(info._reserved.ff_ftime);
	}
#elif defined(_MSC_VER) || defined(__WATCOMC__)
	int error = _dos_findfirst(fullName, _A_SUBDIR, &info._reserved) ? -1 : 0;
	if (!error)
	{
		info.classification = (info._reserved.attrib == _A_SUBDIR) ? DEVICE_DIRECTORY : DEVICE_FILE;
		strcpy(info.name, info._reserved.name);
		ui_strlwr(info.name);
		info.size.Import((long)info._reserved.size);
		info.date.Import(info._reserved.wr_date);
		info.time.Import(info._reserved.wr_time);
	}
#elif defined(__ZTC__)
	info._reserved = ::findfirst(fullName, FA_DIREC);
	int error = info._reserved ? 0 : -1;
	if (!error)
	{
		info.classification = (info._reserved->attribute == FA_DIREC) ? DEVICE_DIRECTORY : DEVICE_FILE;
		strcpy(info.name, info._reserved->name);
		ui_strlwr(info.name);
		info.size.Import((long)info._reserved->size);
		info.date.Import(info._reserved->date);
		info.time.Import(info._reserved->time);
	}
#endif

	return (error);
}

int DIRECTORY_SERVICE::DriveError(int drive)
{
#if defined(ZIL_MSDOS) || defined(__DVX__)
	const EVENT_TYPE ERROR_OPTION_RETRY		= 10001;
	const EVENT_TYPE ERROR_OPTION_CANCEL	= 10002;

	UIW_WINDOW *window = new UIW_WINDOW("DRIVE_ERROR", windowManager->defaultStorage);

	UI_WINDOW_OBJECT *object = window->Get(ERROR_STRING);
	char message[128];
	sprintf(message, "The specified drive \"%c\" cannot be read.", DriveIntToChar(drive));
	object->Information(SET_TEXT, message);

	// Wait for user responce.
	int errorStatus = TRUE;
	UI_EVENT event;
	windowManager->errorSystem->Beep();
	windowManager->Center(window);
	*windowManager + window;
	do
	{
		eventManager->Get(event);
		if (event.type == ERROR_OPTION_CANCEL)
			break;
		else if (event.type == ERROR_OPTION_RETRY)
		{
			if (!SetDrive(drive))
				errorStatus = FALSE;
			else
				windowManager->errorSystem->Beep();
		}
		else
			windowManager->Event(event);
	} while (errorStatus && windowManager->Index(window) != -1);
	if (windowManager->Index(window) != -1)
		*windowManager - window;
	delete window;

	return (errorStatus);
#else
	// All other operating systems have their own drive error handling.
	return (TRUE);
#endif
}

int DIRECTORY_SERVICE::FindFirst(const char *fullName, DEVICE_INFO &info, DEVICE_CLASSIFICATION classification)
{
	info.reserved = 0;
	info.classification = classification;
#if defined(__ZTC__)
	info._reserved = NULL;
#endif

	// ----- drive code. -----
	if (info.Inherited(DEVICE_DRIVE))
		return (FindNext(info));

	// ----- directory and file code. -----
#if defined(ZIL_POSIX)
	int error = 0;
#elif defined(ZIL_OS2)
	ULONG oneBlock = 1;
	int error = DosFindFirst((PSZ)fullName, &_directory, FILE_DIRECTORY,
		&info._reserved, sizeof(info._reserved), &oneBlock, FIL_STANDARD) ? -1 : 0;
#elif defined(WIN32)
	_directory = FindFirstFile((char *)fullName, &info._reserved);
	int error = (_directory == INVALID_HANDLE_VALUE) ? -1 : 0;
#elif defined(__BCPLUSPLUS__) | defined(__TCPLUSPLUS__)
	int error = ::findfirst(fullName, &info._reserved, FA_DIREC);
#elif defined(_MSC_VER) || defined(__WATCOMC__)
	int error = _dos_findfirst(fullName, _A_SUBDIR, &info._reserved) ? -1 : 0;
#elif defined(__ZTC__)
	info._reserved = ::findfirst(fullName, FA_DIREC);
	int error = info._reserved ? 0 : -1;
#endif

	if (!error)
		return (FindNext(info));
	return (error);
}

int DIRECTORY_SERVICE::FindNext(DEVICE_INFO &info)
{
	const int LAST_DRIVE = 25;

	// ----- drive code. -----
	if (info.Inherited(DEVICE_DRIVE))
	{
#if defined(ZIL_POSIX)
		int drive = LAST_DRIVE + 1;
#elif defined(ZIL_OS2)
		int drive = info.reserved ? (int)info._reserved.cbFile : 0;
#elif defined(WIN32)
		int drive = info.reserved ? (int)info._reserved.nFileSizeLow : 0;
#elif defined(__BCPLUSPLUS__) | defined(__TCPLUSPLUS__)
		int drive = info.reserved ? (int)info._reserved.ff_fsize : 0;
#elif defined(_MSC_VER) || defined(__WATCOMC__)
		int drive = info.reserved ? (int)info._reserved.size : 0;
#elif defined(__ZTC__)
		static struct FIND __reserved;
		int drive = info._reserved ? (int)info._reserved->size : 0;
		info._reserved = &__reserved;
#endif

		DEVICE_CLASSIFICATION driveType = drivetype(drive);
		while (!driveType && drive <= LAST_DRIVE)
		{
			drive++;
			driveType = drivetype(drive);
		}
		if (driveType)
		{
			info.reserved++;
			info.classification = driveType;
			sprintf(info.name, "%c:", DriveIntToChar(drive));
			info.date.Import();
			info.time.Import();
			info.size.Import(0L);
#if defined(ZIL_POSIX)
			// Stub for POSIX systems.
#elif defined(ZIL_OS2)
			info._reserved.cbFile = drive + 1;
#elif defined(WIN32)
			info._reserved.nFileSizeLow = drive + 1;
#elif defined(__BCPLUSPLUS__) | defined(__TCPLUSPLUS__)
			info._reserved.ff_fsize = drive + 1;
#elif defined(_MSC_VER) || defined(__WATCOMC__)
			info._reserved.size = drive + 1;
#elif defined(__ZTC__)
			info._reserved->size = drive + 1;
#endif
		}

		return (driveType ? 0 : -1);
	}

	// ----- directory and file code. -----
#if defined(ZIL_POSIX)
	info._reserved = readdir(_directory);
	int error = info._reserved ? 0 : -1;
	if (!error)
	{
		info.reserved++;
		struct stat statx;
		stat(info._reserved->d_name, &statx);
		info.classification = S_ISDIR(statx.st_mode) ? DEVICE_DIRECTORY : DEVICE_FILE;
		strcpy(info.name, info._reserved->d_name);
	}
#elif defined(ZIL_OS2)
	ULONG oneBlock = 1;
	int error = info.reserved ? DosFindNext(_directory, &info._reserved, sizeof(info._reserved), &oneBlock) : 0;
	if (!error)
	{
		info.reserved++;
		info.classification = (info._reserved.attrFile == FILE_DIRECTORY) ? DEVICE_DIRECTORY : DEVICE_FILE;
		strcpy(info.name, info._reserved.achName);
		ui_strlwr(info.name);
	}
#elif defined(WIN32)
	int error = info.reserved ? (FindNextFile(_directory, &info._reserved) ? 0 : -1) : 0;
	if (!error)
	{
		info.reserved++;
		info.classification = (info._reserved.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY) ? DEVICE_DIRECTORY : DEVICE_FILE;
		strcpy(info.name, info._reserved.cFileName);
		ui_strlwr(info.name);
	}
#elif defined(__BCPLUSPLUS__) | defined(__TCPLUSPLUS__)
	int error = info.reserved ? ::findnext(&info._reserved) : 0;
	if (!error)
	{
		info.reserved++;
		info.classification = (info._reserved.ff_attrib == FA_DIREC) ? DEVICE_DIRECTORY : DEVICE_FILE;
		strcpy(info.name, info._reserved.ff_name);
		ui_strlwr(info.name);
	}
#elif defined(_MSC_VER) || defined(__WATCOMC__)
	int error = info.reserved ? (::_dos_findnext(&info._reserved) ? -1 : 0) : 0;
	if (!error)
	{
		info.reserved++;
		info.classification = (info._reserved.attrib == _A_SUBDIR) ? DEVICE_DIRECTORY : DEVICE_FILE;
		strcpy(info.name, info._reserved.name);
		ui_strlwr(info.name);
	}
#elif defined(__ZTC__)
	if (info.reserved)
		info._reserved = ::findnext();
	int error = info._reserved ? 0 : -1;
	if (!error)
	{
		info.reserved++;
		info.classification = (info._reserved->attribute == FA_DIREC) ? DEVICE_DIRECTORY : DEVICE_FILE;
		strcpy(info.name, info._reserved->name);
		ui_strlwr(info.name);
	}
#endif

	return (error);
}

int DIRECTORY_SERVICE::GetDrive(void)
{
#if defined(ZIL_POSIX)
	return (-1);
#elif defined(ZIL_OS2)
	ULONG drive, logical;
	DosQueryCurrentDisk(&drive, &logical);
	return (drive - 1);
#elif defined(WIN32)
	char currentDrive[MAXPATHLEN];
	GetCurrentDirectory(MAXPATHLEN, currentDrive);
	return (DriveCharToInt(*currentDrive));
#else
	unsigned drive;
	_dos_getdrive(&drive);
	return (drive - 1);
#endif
}

int DIRECTORY_SERVICE::GetPath(char *pathName)
{
	int error = getcwd(pathName, MAXPATHLEN) ? 0 : -1;
#if !defined(ZIL_POSIX)
	if (!error)
		ui_strlwr(pathName);
#endif
	return (error);
}

void DIRECTORY_SERVICE::Open(void)
{
#if defined(ZIL_POSIX)
	_directory = opendir(".");
#elif defined(ZIL_OS2)
	_directory = HDIR_SYSTEM;
#endif
}

int DIRECTORY_SERVICE::SetDrive(int drive)
{
#if defined(ZIL_POSIX)
	return (-1);
#elif defined(ZIL_OS2)
	ULONG temp, logical;
	DosQueryCurrentDisk(&temp, &logical);
	if (DosSetDefaultDisk(drive + 1) == ERROR_INVALID_DRIVE)
	{
		DosSetDefaultDisk(temp);
		return (-1);
	}
	return (0);
#elif defined(WIN32)
	char newDrive[MAXPATHLEN];
	sprintf(newDrive, "%c:", DriveIntToChar(drive));
	SetCurrentDirectory(newDrive);
	GetCurrentDirectory(MAXPATHLEN, newDrive);
	if (tolower(*newDrive) == drive)
		return (0);
	return (-1);
#else
	char tempPath[MAXPATHLEN];
	unsigned temp, oldDrive;
	_dos_getdrive(&oldDrive);
	_dos_setdrive(drive + 1, &temp);
	_dos_getdrive(&temp);

	static int internalOperation = FALSE;
	if ((temp - 1 == drive) && getcwd(tempPath, MAXPATHLEN))
		return (0);
	else if (internalOperation)
		return (-1);
	else
	{
		internalOperation = TRUE;
		int error = DriveError(drive);
		internalOperation = FALSE;
		if (error)
		{
			_dos_setdrive(oldDrive, &temp);
			return (-1);
		}
	}
	return (0);
#endif
}

int DIRECTORY_SERVICE::SetPath(char *pathName)
{
	return (chdir(pathName));
}


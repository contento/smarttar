//	Zinc Interface Library Designer - Z_FILE.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#define INCL_DOSFILEMGR		// OS/2 file directives
#define INCL_DOSDEVICES		// OS/2 file directives
#define INCL_DOSERRORS		// OS/2 file directives
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include "ui_dsn.hpp"
#if defined(ZIL_MSDOS) || defined(ZIL_MSWINDOWS) || defined(ZIL_OS2) || defined(__DVX__)
#	if defined(__ZTC__) || defined(_MSC_VER) || defined(__WATCOMC__) || defined(__HIGHC__) || defined(__IBMCPP__)
#		include <direct.h>
#	else
#		include <dir.h>
#	endif
#	if !defined(__IBMCPP__)
#		include <dos.h>
#		include <io.h>
#	endif
#endif
#if defined(ZIL_POSIX)
#	include <unistd.h>
#	include <dirent.h>
#	include <sys/stat.h>
#	include <errno.h>
#endif
#if defined(__DVX__)
#	include <errno.h>
#endif
#if defined(_MSC_VER)
#	if defined(DOSX286)
#		define	_int86x	int86x
#		define	_int86	int86
#		define	_REGS	REGS
#	endif
#	pragma hdrstop					// Microsoft pre-compiled header pragma.
#elif defined(__IBMCPP__) 
#	define chdir	_chdir
#	define getcwd	_getcwd
#endif

static char *_dirMask = "*.*";

EVENT_TYPE FileFieldFunction(UI_WINDOW_OBJECT *object, UI_EVENT &event, EVENT_TYPE ccode)
{
	// Perform action on keyboard select only.
	if (object->LogicalEvent(event, ID_WINDOW_OBJECT) == L_SELECT)
	{
		char *path;
		char fileName[MAX_PATH];
		object->Information(GET_TEXT, &path);
		UI_STORAGE::StripFullPath(path, NULL, fileName);
		if (*fileName && !strchr(fileName, '*') && !strchr(fileName, '?'))
			object->eventManager->Put(UI_EVENT(OPTION_OK, 0));
		else if (*path)
			object->eventManager->Put(UI_EVENT(FILE_CHANGE_PATH, 0));
	}
	return (ccode);
}

static EVENT_TYPE FileItemFunction(UI_WINDOW_OBJECT *object, UI_EVENT &event, EVENT_TYPE ccode)
{
	// Copy text into fileField.
	if (ccode == L_SELECT)
		object->eventManager->Put(UI_EVENT(FILE_SET_NAME, 0));

	// Select the file.
	if (ccode == L_DOUBLE_CLICK || event.type == L_SELECT)
		object->eventManager->Put(UI_EVENT(OPTION_OK, 0));

	return (ccode);
}

#if (defined(ZIL_MSDOS) || (defined(ZIL_MSWINDOWS)) && !defined(WIN32)) || defined(__DVX__)
int zIoctl(int handle, int func)
{
#if defined(_MSC_VER)
	union _REGS regs;
#else
	union REGS regs;
#endif

	if (func != 8 && func != 9)
		return (-1);
	regs.h.ah = 0x44;
	regs.h.al = func;
	regs.h.bl = handle;
#if defined(__ZTC__) && defined(DOS386)
	int86_real(0x21, &regs, &regs);
	return (regs.x.cflag ? -1 : (func == 8 ? regs.x.ax : regs.x.dx));
#elif defined(__WATCOMC__) && defined(__386__)
	int386(0x21, &regs, &regs);
	return (regs.x.cflag ? -1 : (func == 8 ? regs.w.ax : regs.w.dx));
#elif defined(_MSC_VER)
	_int86(0x21, &regs, &regs);
	return (regs.x.cflag ? -1 : (func == 8 ? regs.x.ax : regs.x.dx));
#else
	int86(0x21, &regs, &regs);
	return (regs.x.cflag ? -1 : (func == 8 ? regs.x.ax : regs.x.dx));
#endif
}
#endif

FILE_WINDOW::FILE_WINDOW(const char *name, char *title, char *_fileMask) :
	UIW_WINDOW(name, defaultStorage)
{

	windowManager->Center(this);
	Information(SET_TEXT, title);
	strcpy(fileMask, _fileMask ? _fileMask : _dirMask);
	fileField = (UIW_STRING *)Information(GET_STRINGID_OBJECT, "NAME_FIELD");
	fileField->Information(SET_TEXT, fileMask);
	directoryField = (UIW_STRING *)Information(GET_STRINGID_OBJECT, "DIRECTORY_FIELD");

#if defined(ZIL_POSIX)
	//	Remove the drive window and its prompt.
	(void *)&(*this
		- (UI_WINDOW_OBJECT *)Information(GET_STRINGID_OBJECT, "FIELD_4")
		- (UI_WINDOW_OBJECT *)Information(GET_STRINGID_OBJECT, "DRIVE_LIST"));
#else
	driveList = (UIW_VT_LIST *)Information(GET_STRINGID_OBJECT, "DRIVE_LIST");
#endif
	directoryList = (UIW_VT_LIST *)Information(GET_STRINGID_OBJECT, "DIRECTORY_LIST");
	fileList = (UIW_VT_LIST *)Information(GET_STRINGID_OBJECT, "FILE_LIST");
	helpBar = (HELP_BAR *)Information(GET_STRINGID_OBJECT, "HELP_BAR");

	// Get the available drives.
#if defined(ZIL_OS2)
	if (driveList)
	{
		char *bitmapName;
		BYTE floppyDrives;
		ULONG i, j;
		DosQueryCurrentDisk(&j, &i);
		DosDevConfig(&floppyDrives, DEVINFO_FLOPPY);
		for (i = 1; i <= 26; i++)
		{
			if (DosSetDefaultDisk(i) != ERROR_INVALID_DRIVE)
			{
				char drive[5];
				sprintf(drive, "%c:", 'a' + i - 1);
				char *bitmapName = "networkDrive";
				if (i <= floppyDrives)
					bitmapName = "softDrive";
				else if (i < 'f' - 'a' + 1)
					bitmapName = "hardDrive";
				*driveList + new UIW_BUTTON(0, 0, 22, drive,
					BTF_NO_3D | BTF_NO_TOGGLE | BTF_SEND_MESSAGE | BTF_DOUBLE_CLICK, 
					WOF_NO_FLAGS, NULL, FILE_CHANGE_DRIVE, bitmapName);
			}
		}
		DosSetDefaultDisk(j);
	}
#elif defined(ZIL_MSDOS) || defined(ZIL_MSWINDOWS) || defined(__DVX__)
#if defined(WIN32)
	if (driveList)
	{
		for (int i = 0; i < 26; i++)
		{
			char d[16];
			sprintf(d, "%c:\\", i + 'a');
			DWORD type = GetDriveType(d);
			if (type != 1)
			{
				char drive[5];
				sprintf(drive, "%c:", i + 'a');
				char *bitmapName = "hardDrive";
				if (type == DRIVE_REMOTE)
					bitmapName = "networkDrive";
				else if (type == DRIVE_REMOVABLE)
					bitmapName = "softDrive";
				*driveList
					+ new UIW_BUTTON(0, 0, 22, drive,
						BTF_NO_3D | BTF_NO_TOGGLE | BTF_SEND_MESSAGE | BTF_DOUBLE_CLICK,
						WOF_NO_FLAGS, NULL, FILE_CHANGE_DRIVE, bitmapName);
			}
		}
	}
#else
	if (driveList)
	{
		for (int i = 'A'; i <= 'Z'; i++)
		{
			char *bitmapName = NULL;
			int ioValue = zIoctl(i - 'A' + 1, 9);
			if (ioValue == -1)
				;
			else if (FlagSet(ioValue, 0x1000))
				bitmapName = "networkDrive";
			else
			{
				switch (zIoctl(i - 'A' + 1, 8))
				{
				case 0:
					bitmapName = "softDrive";
					break;
				case 1:
					bitmapName = "hardDrive";
					break;
				}
			}
			if (bitmapName)
			{
				char drive[5];
				sprintf(drive, "%c:", i);
				*driveList + new UIW_BUTTON(0, 0, 22, drive,
					BTF_NO_3D | BTF_NO_TOGGLE | BTF_SEND_MESSAGE | BTF_DOUBLE_CLICK,
					WOF_NO_FLAGS, NULL, FILE_CHANGE_DRIVE, bitmapName);
			}
		}
	}
#endif
#elif defined(ZIL_POSIX)
#endif
}

EVENT_TYPE FILE_WINDOW::Event(const UI_EVENT &event)
{
	EVENT_TYPE ccode = event.type;
	switch (ccode)
	{
	case OPTION_OK:
		{
		char *path;
		char fileName[MAX_PATH];
		fileField->Information(GET_TEXT, &path);
	 	UI_STORAGE::StripFullPath(path, NULL, fileName);
		if (*fileName)
			eventManager->Put(UI_EVENT(FILE_SELECTED, 0));
		else
			errorSystem->Beep();
		}
		break;

	case OPTION_CANCEL:
		eventManager->Put(UI_EVENT(S_CLOSE, 0));
		break;

	case FILE_SET_NAME:
		fileField->Information(SET_TEXT, fileList->Current()->Information(GET_TEXT, NULL));
		break;

	case S_INITIALIZE:
	case FILE_CHANGE_PATH:
	case FILE_CHANGE_DRIVE:
	case FILE_CHANGE_DIRECTORY:
#if defined(ZIL_MSWINDOWS) && defined(WIN32)
		{
		char newDrive[MAX_PATH];
		char oldDrive[MAX_PATH];
		GetCurrentDirectory(MAX_PATH, oldDrive);
		if (ccode == FILE_CHANGE_DRIVE)
		{
			char *drive;
			driveList->Information(GET_TEXT, &drive);
			sprintf(newDrive, "%c:", drive[0]);
			SetCurrentDirectory(newDrive);
			GetCurrentDirectory(MAX_PATH, newDrive);
			if (newDrive[0] == oldDrive[0])
				break;
		}
		else if (ccode == FILE_CHANGE_DIRECTORY)
		{
			char *directory;
			directoryList->Information(GET_TEXT, &directory);
			SetCurrentDirectory(directory);
		}
		else if (ccode == FILE_CHANGE_PATH)
		{
			char *fullPath;
			char path[MAX_PATH];
			char fileName[MAX_PATH];
			fileField->Information(GET_TEXT, &fullPath);
		 	UI_STORAGE::StripFullPath(fullPath, path, fileName);
			if (strchr(path, ':') == path + 1)
			{
				SetCurrentDirectory(path);
				GetCurrentDirectory(MAX_PATH, newDrive);
			}
			if (*fileName)
				strcpy(fileMask, fileName);
			fileField->DataSet(fileMask);
		}

		// Read the current path.
		char path[MAX_PATH];
		GetCurrentDirectory(MAX_PATH, path);
		ui_strlwr(path);
		directoryField->Information(SET_TEXT, path);

		// Clear the file and directory lists.
		directoryList->Destroy();
		fileList->Destroy();

		// Add the parent directory.
		int indentation = 0;
		if (strrchr(path, ':')[2] != '\0')
			*directoryList
				+ new UIW_BUTTON(indentation++, 0, 22, "..",
					BTF_NO_3D | BTF_NO_TOGGLE | BTF_DOUBLE_CLICK | BTF_SEND_MESSAGE, WOF_NO_FLAGS,
					NULL, FILE_CHANGE_DIRECTORY, "parentDirectory");

		// Add the sub-directories.
		WIN32_FIND_DATA fileBlock;
		HANDLE fileHandle = FindFirstFile("*.*", &fileBlock);
		BOOL empty = fileHandle == INVALID_HANDLE_VALUE;
		while (!empty)
		{
			if (fileBlock.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY &&
				strncmp(".", fileBlock.cFileName, 1))
					*directoryList
						+ new UIW_BUTTON(indentation, 0, 22, strlwr(fileBlock.cFileName),
							BTF_NO_3D | BTF_NO_TOGGLE | BTF_DOUBLE_CLICK | BTF_SEND_MESSAGE,
							WOF_NO_FLAGS, NULL, FILE_CHANGE_DIRECTORY, "childDirectory");
			empty = !FindNextFile(fileHandle, &fileBlock);
		}
		FindClose(fileHandle);
	
		// Get the files
		fileHandle = FindFirstFile(fileMask, &fileBlock);
		empty = fileHandle == INVALID_HANDLE_VALUE;
		while (!empty)
		{
			if (fileBlock.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)
				*fileList
//					+ new UIW_STRING(0, 0, 22, strlwr(fileBlock.cFileName),
//						-1, STF_NO_FLAGS, WOF_NO_FLAGS, FileItemFunction);
					+ new UIW_BUTTON(0, 0, 22, strlwr(fileBlock.cFileName),
						BTF_NO_3D | BTF_DOUBLE_CLICK, WOF_NO_FLAGS, FileItemFunction);
			empty = !FindNextFile(fileHandle, &fileBlock);
		}
		FindClose(fileHandle);
		}
#elif defined(ZIL_OS2)
		{
		ULONG oldDrive, driveMap, newDrive;
		DosQueryCurrentDisk(&oldDrive, &driveMap);
		if (ccode == FILE_CHANGE_DRIVE)
		{
			const char *drive;
			driveList->Current()->Information(GET_TEXT, &drive);
			DosSetDefaultDisk(drive[0] - 'a' + 1);
			DosQueryCurrentDisk(&newDrive, &driveMap);
			if (oldDrive == newDrive)
				break;
			DOS::errorDrive = drive[0] - 'a';
			DOS::errorCode = 2;
		}
		else if (ccode == FILE_CHANGE_DIRECTORY)
		{
			char *directory;
			directoryList->Current()->Information(GET_TEXT, &directory);
			chdir(directory);
		}
		else if (ccode == FILE_CHANGE_PATH)
		{
			char *fullPath;
			char path[MAX_PATH];
			char fileName[MAX_PATH];
			fileField->Information(GET_TEXT, &fullPath);
		 	UI_STORAGE::StripFullPath(fullPath, path, fileName);
			if (strchr(path, ':') == path + 1)
			{
				DosSetDefaultDisk(path[0] - 'a' + 1);
				DosQueryCurrentDisk(&newDrive, &driveMap);
				if (newDrive != toupper(path[0]) - 'A')
					break;
				chdir(path + 2);
			}
			if (*fileName)
				strcpy(fileMask, fileName);
			fileField->DataSet(fileMask);
		}

		// Read the current path.
		char *path, pathBuffer[MAX_PATH];
		do
		{
			DOS::criticalError = FALSE;
			path = getcwd(pathBuffer, MAX_PATH);
		} while ((!path || DOS::criticalError) && DOS::ErrorResponce() == DIALOG_RETRY);
		if (!path || DOS::criticalError)
		{
			DosSetDefaultDisk(oldDrive);
			break;
		}
		ui_strlwr(path);
		directoryField->Information(SET_TEXT, path);

		// Clear the file and directory lists.
		directoryList->Destroy();
		fileList->Destroy();

		// Add the parent directory.
		int indentation = 0;
		if (strrchr(path, ':')[2] != '\0')
			*directoryList + new UIW_BUTTON(indentation++, 0, 22, "..",
				BTF_NO_3D | BTF_NO_TOGGLE | BTF_DOUBLE_CLICK | BTF_SEND_MESSAGE, WOF_NO_FLAGS,
				NULL, FILE_CHANGE_DIRECTORY, "parentDirectory");

		// Add the sub-directories.
		FILEFINDBUF3 fileBlock;
		ULONG oneBlock = 1;
		APIRET done;
		HDIR dirHandle = HDIR_SYSTEM;
		do
		{
			DOS::criticalError = FALSE;
			done = DosFindFirst((PSZ)"*", &dirHandle, FILE_DIRECTORY,
				&fileBlock, sizeof(fileBlock), &oneBlock, FIL_STANDARD);
		} while (DOS::criticalError && DOS::ErrorResponce() == DIALOG_RETRY);
		if (DOS::criticalError)
			return S_ERROR;
		while (!done)
		{
			if (FlagSet(fileBlock.attrFile, FILE_DIRECTORY) &&
				strcmp(".", fileBlock.achName) &&
				strcmp("..", fileBlock.achName))
			{
//				ui_strlwr(fileBlock.achName);
				*directoryList + new UIW_BUTTON(indentation, 0, 22,
					fileBlock.achName, BTF_NO_3D | BTF_NO_TOGGLE |
					BTF_DOUBLE_CLICK | BTF_SEND_MESSAGE, WOF_NO_FLAGS,
					NULL, FILE_CHANGE_DIRECTORY, "childDirectory");
			}
			do
			{
				DOS::criticalError = FALSE;
				done = DosFindNext(dirHandle, &fileBlock, sizeof(fileBlock), &oneBlock);
			} while (DOS::criticalError && DOS::ErrorResponce() == DIALOG_RETRY);
			if (DOS::criticalError)
				return S_ERROR;
		}

		// Add the files.
		oneBlock = 1;
		do
		{
			DOS::criticalError = FALSE;
			done = DosFindFirst((PSZ)fileMask, &dirHandle, FILE_DIRECTORY, &fileBlock, sizeof(fileBlock), &oneBlock, FIL_STANDARD);
		} while (DOS::criticalError && DOS::ErrorResponce() == DIALOG_RETRY);
		if (DOS::criticalError)
			return S_ERROR;
		while (!done)
		{
			if (!FlagSet(fileBlock.attrFile, FILE_DIRECTORY))
			{
//				ui_strlwr(fileBlock.achName);
//				*fileList + new UIW_STRING(0, 0, 22, fileBlock.achName,
//					-1, STF_NO_FLAGS, WOF_NO_FLAGS, FileItemFunction);
				*fileList + new UIW_BUTTON(0, 0, 22, fileBlock.achName,
						BTF_NO_3D | BTF_DOUBLE_CLICK, WOF_NO_FLAGS, FileItemFunction);
			}
			do
			{
				DOS::criticalError = FALSE;
				done = DosFindNext(dirHandle, &fileBlock, sizeof(fileBlock), &oneBlock);
			} while (DOS::criticalError && DOS::ErrorResponce() == DIALOG_RETRY);
			if (DOS::criticalError)
				return S_ERROR;
		}
		}
#elif defined(ZIL_MOTIF)
		if (ccode == FILE_CHANGE_DIRECTORY)
		{
			char *directory;
			directoryList->Current()->Information(GET_TEXT, &directory);
			chdir(directory);
		}
		else if (ccode == FILE_CHANGE_PATH)
		{
			char *fullPath;
			char path[MAX_PATH];
			char fileName[MAX_PATH];
			fileField->Information(GET_TEXT, &fullPath);
		 	UI_STORAGE::StripFullPath(fullPath, path, fileName);
			if (*fileName)
				strcpy(fileMask, fileName);
			fileField->DataSet(fileMask);
		}

		{
		// Read the current path.
		char path[MAX_PATH];
		getcwd(path, MAX_PATH);
/*		strcpy(path, (char *)directoryField->Information(GET_TEXT, NULL)); */
		directoryField->Information(SET_TEXT, path);

		// Clear the file and directory lists.
		directoryList->Destroy();
		fileList->Destroy();

		// Add the parent directory.
		int indentation = 0;
		(void *)&(*directoryList + new UIW_BUTTON(indentation++, 0, 22, "..",
			BTF_NO_3D | BTF_NO_TOGGLE | BTF_DOUBLE_CLICK | BTF_SEND_MESSAGE,
				WOF_NO_FLAGS, NULL, FILE_CHANGE_DIRECTORY, "parentDirectory"));

		DIR *directory = opendir(path);
		struct dirent *dirEntry;
		errno = 0;
		while ((dirEntry = readdir(directory)))
		{
			struct stat fileStat;
			stat(dirEntry->d_name, &fileStat);
			if (S_ISDIR(fileStat.st_mode))
			{
				if (!strcmp(dirEntry->d_name, "..") ||
					!strcmp(dirEntry->d_name, "."))
					continue;

				// Add it to the directory list.
				(void *)&(*directoryList + new UIW_BUTTON(indentation, 0, 22,
					dirEntry->d_name, BTF_NO_3D | BTF_NO_TOGGLE |
					BTF_DOUBLE_CLICK | BTF_SEND_MESSAGE, WOF_NO_FLAGS, NULL,
					FILE_CHANGE_DIRECTORY, "childDirectory"));
			}
			else if (S_ISREG(fileStat.st_mode) &&
				!ui_WildStrcmp(dirEntry->d_name, fileMask))
			{
				// Add it to the file list.
//				(void *)&(*fileList + new UIW_STRING(0, 0, 22, dirEntry->d_name,
//					-1, STF_NO_FLAGS, WOF_NO_FLAGS, FileItemFunction));
				(void *)&(*fileList + new UIW_BUTTON(0, 0, 22, dirEntry->d_name,
					BTF_NO_3D | BTF_DOUBLE_CLICK, WOF_NO_FLAGS, FileItemFunction));
			}
		}
		closedir(directory);
		}
#elif defined(__BCPLUSPLUS__) || defined(__TCPLUSPLUS__)
		{
		int oldDrive = getdisk();
		if (ccode == FILE_CHANGE_DRIVE)
		{
			const char *drive;
			driveList->Current()->Information(GET_TEXT, &drive);
			setdisk(drive[0] - 'A');
			if (getdisk() == oldDrive)
				break;
			DOS::errorDrive = drive[0] - 'A';
			DOS::errorCode = 2;
		}
		else if (ccode == FILE_CHANGE_DIRECTORY)
		{
			char *directory;
			directoryList->Current()->Information(GET_TEXT, &directory);
			chdir(directory);
		}
		else if (ccode == FILE_CHANGE_PATH)
		{
			char *fullPath;
			char path[MAX_PATH];
			char fileName[MAX_PATH];
			fileField->Information(GET_TEXT, &fullPath);
		 	UI_STORAGE::StripFullPath(fullPath, path, fileName);
			if (strchr(path, ':') == path + 1)
			{
				setdisk(toupper(path[0]) - 'A');
				if (getdisk() != toupper(path[0]) - 'A')
					break;
				chdir(path + 2);
			}
			if (*fileName)
				strcpy(fileMask, fileName);
			fileField->DataSet(fileMask);
		}

		// Read the current path.
		char *path, pathBuffer[MAX_PATH];
		do
		{
			DOS::criticalError = FALSE;
			path = getcwd(pathBuffer, MAX_PATH);
		} while ((!path || DOS::criticalError) && DOS::ErrorResponce() == DIALOG_RETRY);
		if (!path || DOS::criticalError)
		{
			setdisk(oldDrive);
			break;
		}
		ui_strlwr(path);
		directoryField->Information(SET_TEXT, path);

		// Clear the file and directory lists.
		directoryList->Destroy();
		fileList->Destroy();

		// Add the parent directory.
		int indentation = 0;
		if (strrchr(path, ':')[2] != '\0')
			*directoryList + new UIW_BUTTON(indentation++, 0, 22, "..",
				BTF_NO_3D | BTF_NO_TOGGLE | BTF_DOUBLE_CLICK | BTF_SEND_MESSAGE, WOF_NO_FLAGS,
				NULL, FILE_CHANGE_DIRECTORY, "parentDirectory");

		// Add the sub-directories.
		int done;
		struct ffblk fileBlock;
		do
		{
			DOS::criticalError = FALSE;
			done = findfirst(_dirMask, &fileBlock, FA_DIREC);
		} while (DOS::criticalError && DOS::ErrorResponce() == DIALOG_RETRY);
		if (DOS::criticalError)
			return S_ERROR;
		while (!done)
		{
			if (fileBlock.ff_attrib == FA_DIREC &&
				strcmp(".", fileBlock.ff_name) &&
				strcmp("..", fileBlock.ff_name))
			{
				ui_strlwr(fileBlock.ff_name);
				*directoryList + new UIW_BUTTON(indentation, 0, 22,
					fileBlock.ff_name, BTF_NO_3D | BTF_NO_TOGGLE |
					BTF_DOUBLE_CLICK | BTF_SEND_MESSAGE, WOF_NO_FLAGS, NULL, FILE_CHANGE_DIRECTORY,
					"childDirectory");
			}
			do
			{
				DOS::criticalError = FALSE;
				done = findnext(&fileBlock);
			} while (DOS::criticalError && DOS::ErrorResponce() == DIALOG_RETRY);
			if (DOS::criticalError)
				return S_ERROR;
		}

		// Add the files.
		do
		{
			DOS::criticalError = FALSE;
			done = findfirst(fileMask, &fileBlock, FA_DIREC);
		} while (DOS::criticalError && DOS::ErrorResponce() == DIALOG_RETRY);
		if (DOS::criticalError)
			return S_ERROR;
		while (!done)
		{
			if (fileBlock.ff_attrib != FA_DIREC)
			{
				ui_strlwr(fileBlock.ff_name);
//				*fileList + new UIW_STRING(0, 0, 22, fileBlock.ff_name,
//					-1, STF_NO_FLAGS, WOF_NO_FLAGS, FileItemFunction);
				*fileList + new UIW_BUTTON(0, 0, 22, fileBlock.ff_name,
					BTF_NO_3D | BTF_DOUBLE_CLICK, WOF_NO_FLAGS, FileItemFunction);
			}
			do
			{
				DOS::criticalError = FALSE;
				done = findnext(&fileBlock);
			} while (DOS::criticalError && DOS::ErrorResponce() == DIALOG_RETRY);
			if (DOS::criticalError)
				return S_ERROR;
		}
		}
#elif defined(__ZTC__)
		{
		unsigned oldDrive;
		unsigned newDrive;
		unsigned temp;
		dos_getdrive(&oldDrive);
		if (ccode == FILE_CHANGE_DRIVE)
		{
			const char *drive;
			driveList->Current()->Information(GET_TEXT, &drive);
			dos_setdrive(drive[0] - 'A' + 1, &temp);
			dos_getdrive(&newDrive);
			if (newDrive == oldDrive)
				break;
		}
		else if (ccode == FILE_CHANGE_DIRECTORY)
		{
			char *directory;
			directoryList->Current()->Information(GET_TEXT, &directory);
			chdir(directory);
		}
		else if (ccode == FILE_CHANGE_PATH)
		{
			char *fullPath;
			char path[MAX_PATH];
			char fileName[MAX_PATH];
			fileField->Information(GET_TEXT, &fullPath);
		 	UI_STORAGE::StripFullPath(fullPath, path, fileName);
			if (strchr(path, ':') == path + 1)
			{
				dos_setdrive(toupper(path[0]) - 'A' + 1, &temp);
				dos_getdrive(&newDrive);
				if (newDrive != toupper(path[0]) - 'A')
					break;
				chdir(path + 2);
			}
			if (*fileName)
				strcpy(fileMask, fileName);
			fileField->DataSet(fileMask);
		}

		// Read the current path.
		char path[MAX_PATH];
		do
		{
			DOS::criticalError = FALSE;
			getcwd(path, MAX_PATH);
		} while (DOS::criticalError && DOS::ErrorResponce() == DIALOG_RETRY);
	    if (DOS::criticalError)
		{
			dos_setdrive(oldDrive, &temp);
			break;
		}
		ui_strlwr(path);
		directoryField->Information(SET_TEXT, path);

		// Clear the file and directory lists.
		directoryList->Destroy();
		fileList->Destroy();

		// Add the parent directory.
		int indentation = 0;
		if (strrchr(path, ':')[2] != '\0')
			*directoryList + new UIW_BUTTON(indentation++, 0, 22, "..",
				BTF_NO_3D | BTF_NO_TOGGLE | BTF_DOUBLE_CLICK | BTF_SEND_MESSAGE, WOF_NO_FLAGS,
				NULL, FILE_CHANGE_DIRECTORY, "parentDirectory");

		// Add the sub-directories.
		struct FIND *fileBlock;
		do
		{
			DOS::criticalError = FALSE;
			fileBlock = findfirst("*.*", FA_DIREC);
		} while (DOS::criticalError && DOS::ErrorResponce() == DIALOG_RETRY);
		if (DOS::criticalError)
			return S_ERROR;
		while (fileBlock)
		{
			if (fileBlock->attribute == FA_DIREC &&
				strcmp(".", fileBlock->name) &&
				strcmp("..", fileBlock->name))
			{
				ui_strlwr(fileBlock->name);
				*directoryList + new UIW_BUTTON(indentation, 0, 22,
					fileBlock->name, BTF_NO_3D | BTF_NO_TOGGLE |
					BTF_DOUBLE_CLICK | BTF_SEND_MESSAGE, WOF_NO_FLAGS, NULL, FILE_CHANGE_DIRECTORY,
					"childDirectory");
			}
			do
			{
				DOS::criticalError = FALSE;
				fileBlock = findnext();
			} while (DOS::criticalError && DOS::ErrorResponce() == DIALOG_RETRY);
			if (DOS::criticalError)
				return S_ERROR;
		}

		// Add the files.
		do
		{
			DOS::criticalError = FALSE;
			fileBlock = findfirst(fileMask, FA_DIREC);
		} while (DOS::criticalError && DOS::ErrorResponce() == DIALOG_RETRY);
		if (DOS::criticalError)
			return S_ERROR;
		while (fileBlock)
		{
			if (fileBlock->attribute != FA_DIREC)
			{
				ui_strlwr(fileBlock->name);
//				*fileList + new UIW_STRING(0, 0, 22, fileBlock->name,
//					-1, STF_NO_FLAGS, WOF_NO_FLAGS, FileItemFunction);
				*fileList + new UIW_BUTTON(0, 0, 22, fileBlock->name,
					BTF_NO_3D | BTF_DOUBLE_CLICK, WOF_NO_FLAGS, FileItemFunction);
			}
			do
			{
				DOS::criticalError = FALSE;
				fileBlock = findnext();
			} while (DOS::criticalError && DOS::ErrorResponce() == DIALOG_RETRY);
			if (DOS::criticalError)
				return S_ERROR;
		}
		}
#elif defined(_MSC_VER)
		{
		unsigned oldDrive;
		unsigned newDrive;
		unsigned temp;
		_dos_getdrive(&oldDrive);
		if (ccode == FILE_CHANGE_DRIVE)
		{
			char *drive;
			driveList->Current()->Information(GET_TEXT, &drive);
			_dos_setdrive(drive[0] - 'A' + 1, &temp);
			_dos_getdrive(&newDrive);
			if (newDrive == oldDrive)
				break;
		}
		else if (ccode == FILE_CHANGE_DIRECTORY)
		{
			char *directory;
			directoryList->Current()->Information(GET_TEXT, &directory);
			chdir(directory);
		}
		else if (ccode == FILE_CHANGE_PATH)
		{
			char *fullPath;
			char path[MAX_PATH];
			char fileName[MAX_PATH];
			fileField->Information(GET_TEXT, &fullPath);
		 	UI_STORAGE::StripFullPath(fullPath, path, fileName);
			if (strchr(path, ':') == path + 1)
			{
				_dos_setdrive(toupper(path[0]) - 'A' + 1, &temp);
				_dos_getdrive(&newDrive);
				if (newDrive != toupper(path[0]) - 'A')
					break;
				chdir(path + 2);
			}
			if (*fileName)
				strcpy(fileMask, fileName);
			fileField->DataSet(fileMask);
		}

		// Read the current path.
		char path[MAX_PATH];
		do
		{
			DOS::criticalError = FALSE;
			getcwd(path, MAX_PATH);
		} while (DOS::criticalError && DOS::ErrorResponce() == DIALOG_RETRY);
	    if (DOS::criticalError)
		{
			_dos_setdrive(oldDrive, &temp);
			break;
		}
		ui_strlwr(path);
		directoryField->Information(SET_TEXT, path);

		// Clear the file and directory lists.
		directoryList->Destroy();
		fileList->Destroy();

		// Add the parent directory.
		int indentation = 0;
		if (strrchr(path, ':')[2] != '\0')
			*directoryList + new UIW_BUTTON(indentation++, 0, 22, "..",
				BTF_NO_3D | BTF_NO_TOGGLE | BTF_DOUBLE_CLICK | BTF_SEND_MESSAGE, WOF_NO_FLAGS,
				NULL, FILE_CHANGE_DIRECTORY, "parentDirectory");

		// Add the sub-directories.
		int empty;
		struct _find_t fileBlock;
		do
		{
			DOS::criticalError = FALSE;
			empty = _dos_findfirst("*.*", _A_SUBDIR, &fileBlock);
		} while (DOS::criticalError && DOS::ErrorResponce() == DIALOG_RETRY);
		if (DOS::criticalError)
			return S_ERROR;
		while (!empty)
		{
			if (fileBlock.attrib == _A_SUBDIR &&
				strcmp(".", fileBlock.name) &&
				strcmp("..", fileBlock.name))
			{
				ui_strlwr(fileBlock.name);
				*directoryList + new UIW_BUTTON(indentation, 0, 22,
					fileBlock.name, BTF_NO_3D | BTF_NO_TOGGLE |
					BTF_DOUBLE_CLICK | BTF_SEND_MESSAGE, WOF_NO_FLAGS, NULL, FILE_CHANGE_DIRECTORY,
					"childDirectory");
			}
			do
			{
				DOS::criticalError = FALSE;
				empty = _dos_findnext(&fileBlock);
			} while (DOS::criticalError && DOS::ErrorResponce() == DIALOG_RETRY);
			if (DOS::criticalError)
				return S_ERROR;
		}

		// Add the files.
		do
		{
			DOS::criticalError = FALSE;
			empty = _dos_findfirst(fileMask, _A_SUBDIR, &fileBlock);
		} while (DOS::criticalError && DOS::ErrorResponce() == DIALOG_RETRY);
		if (DOS::criticalError)
			return S_ERROR;
		while (!empty)
		{
			if (fileBlock.attrib != _A_SUBDIR)
			{
				ui_strlwr(fileBlock.name);
//				*fileList + new UIW_STRING(0, 0, 22, fileBlock.name,
//					-1, STF_NO_FLAGS, WOF_NO_FLAGS, FileItemFunction);
				*fileList + new UIW_BUTTON(0, 0, 22, fileBlock.name,
					BTF_NO_3D | BTF_DOUBLE_CLICK, WOF_NO_FLAGS, FileItemFunction);
			}
			do
			{
				DOS::criticalError = FALSE;
				empty = _dos_findnext(&fileBlock);
			} while (DOS::criticalError && DOS::ErrorResponce() == DIALOG_RETRY);
			if (DOS::criticalError)
				return S_ERROR;
		}
		}
#elif defined(__WATCOMC__)
		{
		unsigned oldDrive;
		unsigned newDrive;
		unsigned temp;
		_dos_getdrive(&oldDrive);
		if (ccode == FILE_CHANGE_DRIVE)
		{
			char *drive;
			driveList->Current()->Information(GET_TEXT, &drive);
			_dos_setdrive(drive[0] - 'A' + 1, &temp);
			_dos_getdrive(&newDrive);
			if (newDrive == oldDrive)
				break;
		}
		else if (ccode == FILE_CHANGE_DIRECTORY)
		{
			char *directory;
			directoryList->Current()->Information(GET_TEXT, &directory);
			chdir(directory);
		}
		else if (ccode == FILE_CHANGE_PATH)
		{
			char *fullPath;
			char path[MAX_PATH];
			char fileName[MAX_PATH];
			fileField->Information(GET_TEXT, &fullPath);
		 	UI_STORAGE::StripFullPath(fullPath, path, fileName);
			if (strchr(path, ':') == path + 1)
			{
				_dos_setdrive(toupper(path[0]) - 'A' + 1, &temp);
				_dos_getdrive(&newDrive);
				if (newDrive != toupper(path[0]) - 'A')
					break;
				chdir(path + 2);
			}
			if (*fileName)
				strcpy(fileMask, fileName);
			fileField->DataSet(fileMask);
		}

		// Read the current path.
		char path[MAX_PATH];
		do
		{
			DOS::criticalError = FALSE;
			getcwd(path, MAX_PATH);
		} while (DOS::criticalError && DOS::ErrorResponce() == DIALOG_RETRY);
	    if (DOS::criticalError)
		{
			_dos_setdrive(oldDrive, &temp);
			break;
		}
		ui_strlwr(path);
		directoryField->Information(SET_TEXT, path);

		// Clear the file and directory lists.
		directoryList->Destroy();
		fileList->Destroy();

		// Add the parent directory.
		int indentation = 0;
		if (strrchr(path, ':')[2] != '\0')
			*directoryList + new UIW_BUTTON(indentation++, 0, 22, "..",
				BTF_NO_3D | BTF_NO_TOGGLE | BTF_DOUBLE_CLICK | BTF_SEND_MESSAGE, WOF_NO_FLAGS,
				NULL, FILE_CHANGE_DIRECTORY, "parentDirectory");

		// Add the sub-directories.
		int empty;
		struct find_t fileBlock;
		do
		{
			DOS::criticalError = FALSE;
			empty = _dos_findfirst("*.*", _A_SUBDIR, &fileBlock);
		} while (DOS::criticalError && DOS::ErrorResponce() == DIALOG_RETRY);
		if (DOS::criticalError)
			return S_ERROR;
		while (!empty)
		{
			if (fileBlock.attrib == _A_SUBDIR &&
				strcmp(".", fileBlock.name) &&
				strcmp("..", fileBlock.name))
			{
				ui_strlwr(fileBlock.name);
				*directoryList + new UIW_BUTTON(indentation, 0, 22,
					fileBlock.name, BTF_NO_3D | BTF_NO_TOGGLE |
					BTF_DOUBLE_CLICK | BTF_SEND_MESSAGE, WOF_NO_FLAGS, NULL, FILE_CHANGE_DIRECTORY,
					"childDirectory");
			}
			do
			{
				DOS::criticalError = FALSE;
				empty = _dos_findnext(&fileBlock);
			} while (DOS::criticalError && DOS::ErrorResponce() == DIALOG_RETRY);
			if (DOS::criticalError)
				return S_ERROR;
		}

		// Add the files.
		do
		{
			DOS::criticalError = FALSE;
			empty = _dos_findfirst(fileMask, _A_SUBDIR, &fileBlock);
		} while (DOS::criticalError && DOS::ErrorResponce() == DIALOG_RETRY);
		if (DOS::criticalError)
			return S_ERROR;
		while (!empty)
		{
			if (fileBlock.attrib != _A_SUBDIR)
			{
				ui_strlwr(fileBlock.name);
//				*fileList + new UIW_STRING(0, 0, 22, fileBlock.name,
//					-1, STF_NO_FLAGS, WOF_NO_FLAGS, FileItemFunction);
				*fileList + new UIW_BUTTON(0, 0, 22, fileBlock.name,
					BTF_NO_3D | BTF_DOUBLE_CLICK, WOF_NO_FLAGS, FileItemFunction);
			}
			do
			{
				DOS::criticalError = FALSE;
				empty = _dos_findnext(&fileBlock);
			} while (DOS::criticalError && DOS::ErrorResponce() == DIALOG_RETRY);
			if (DOS::criticalError)
				return S_ERROR;
		}
		}
#endif

		// Initialize or redisplay.
		if (ccode == S_INITIALIZE)
			ccode = UIW_WINDOW::Event(event);
		else
		{
			directoryList->Event(UI_EVENT(S_REDISPLAY, 0));
			fileList->Event(UI_EVENT(S_REDISPLAY, 0));
		}
		break;

	case OPTION_HELP:
		helpSystem->DisplayHelp(windowManager, helpContext);
		break;

	default:
		ccode = UIW_WINDOW::Event(event);
		helpBar->Update(Current());
		break;
	}

	// Return the control code.
	return (ccode);
}


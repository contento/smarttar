//	Zinc Interface Library - Z_STORE.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#define DOCACHEPTR	5
#ifndef _STORE_HDR
#	include <stdio.h>
#	include <stdlib.h>
#	include <string.h>
#	include <time.h>
#	include <fcntl.h>
#	include <errno.h>
#	if defined(__BCPLUSPLUS__) | defined(__TCPLUSPLUS__)
#		include <mem.h>
#		include <dir.h>
#		include <direct.h>
#	endif
#	if defined(__ZTC__) | defined(_MSC_VER) | defined(__WATCOMC__) || defined(__HIGHC__) || defined(__IBMCPP__)
#		include <direct.h>
#	endif
#	include "ui_gen.hpp"
#	include <sys/types.h>
#	include <sys/stat.h>
#	if defined(ZIL_POSIX)
#		include <sys/param.h>
#		include <unistd.h>
#		define O_BINARY	0
#	endif
#	if defined(ZIL_MSDOS) || defined(ZIL_MSWINDOWS) || defined(ZIL_OS2) || defined(__DVX__)
#		include <io.h>
#		if !defined(__IBMCPP__)
#			include <dos.h>
#		endif
#	endif
#	if defined(__GNUC__)
#		define SEEK_SET	0
#		define SEEK_CUR	1
#		define SEEK_END	2
#	endif
#	if defined(_MSC_VER) || defined(__IBMCPP__) 
#		define open		_open
#		define close	_close
#		define read		_read
#		define write	_write
#		define lseek	_lseek
#		define unlink	_unlink
#		define getcwd	_getcwd
#		if defined(_MSC_VER)
#			define stat		_stat
#			pragma hdrstop			// Microsoft pre-compiled header pragma.
#		endif
#	elif defined(__ZTC__)
#		define _dos_getdrive dos_getdrive
#	elif defined(__WATCOMC__) || defined(__HIGHC__)
#		define dos_getdrive _dos_getdrive
#		define dos_setdrive _dos_setdrive
#	endif

#	ifdef TESTING
#		ifndef MAKESTRLEN
#			define MAKESTRLEN
#		endif
#		define CheckStorageError(x)	if (storageError != 0) fileAbort(-1);
#		define CheckObjectError(x)	if (objectError != 0) fileAbort(-1);
#		define CheckObjectErrorNull()	if (objectError != 0) fileAbort(-1);
#	else
#		define CheckStorageError(x)	if (storageError != 0) return x;
#		define CheckObjectError(x)	if (objectError != 0) return x;
#		define CheckObjectErrorNull()	if (objectError != 0) return;
#	endif	// TESTING
#endif	// _STORE_HDR

#ifdef MAKESTRLEN
#define ui_strlen	strlen
#endif

#define DOT		"."
#define DOTDOT		".."
#define DIRECTORY_SEPARATOR	'~'
#define ROOT		"~"
#define ZIL_IREAD		0
#define ZIL_IWRITE		1

#define MAGIC_NUMBER	0x05AF
#define MAJOR_VERSION	3
#define MINOR_VERSION	1
#define DIRECTORY_TAG	0x8000

#define NEW_INC		5

#define BYTES_PER_DATA_BLOCK		256
#define DISK_ADDR_PER_DATA_BLOCK	(BYTES_PER_DATA_BLOCK / sizeof(diskAddr))
#define BYTES_PER_INODE_BLOCK		1024
#define DISK_ADDR_PER_INODE_BLOCK	(BYTES_PER_INODE_BLOCK / sizeof(diskAddr))
#define INODES_PER_INODE_BLOCK		(BYTES_PER_INODE_BLOCK / sizeof(struct inode))
#define BYTES_PER_BLOCK			(BYTES_PER_DATA_BLOCK)
#define COPY_BYTES			4096

#define LENGTHOF(x)	(sizeof(x)/sizeof((x)[0]))
#define stMIN(x, y)	(x < y ? x : y)
#define setFragmentWithInum(index, block, inum) \
		(index) = (USHORT)((inum) & 0xFFFF), \
		(block) = (USHORT)(((inum) >> 16) & 0xFFFF);
#define setInumWithFragment(inum, index, block) \
		(inum) = ((inum_t)(block) << 16) | (index);


struct ZINC_SIGNATURE {
	char copyrightNotice[64];
  	UCHAR majorVersion;
	UCHAR minorVersion;
	USHORT magicNumber;
};

static struct ZINC_SIGNATURE _signature =
	{ "Zinc Data File Version 3.1\032", MAJOR_VERSION, MINOR_VERSION, MAGIC_NUMBER };

struct inode
{
	long size;
	long createTime;
	long modifyTime;
	diskAddr direct[21];
	diskAddr sIndirect;
	diskAddr dIndirect;
	diskAddr fragmentBlock;	// this is used as a free list pointer
	USHORT useCount;	// if this is 0 then this inode is unused
	USHORT fragmentIndex;	// this is also used as the free list
};

struct directoryEntry
{
	inum_t inum;
	OBJECTID objectID;
	USHORT revision;
	USHORT country;
	char stringID[MAX_NAME_LENGTH];
};

struct superBlock
{
	struct ZINC_SIGNATURE signature;
	time_t createTime;
	time_t modifyTime;
	ULONG unused;
	USHORT freeInodeListHi;
	USHORT revision;
	USHORT freeInodeListLo;
	diskAddr freeDataList;
	diskAddr inodeDirect[82];
	diskAddr inodeSIndirect;
	diskAddr inodeDIndirect;
};

struct freeListBlock
{
	diskAddr freeBlocks[DISK_ADDR_PER_DATA_BLOCK-1];
	diskAddr next;
};

#ifndef _STORE_HDR
struct openFile
{
	int openCount;		// Number of times the file is open
	int modified;		// if the modifyTime needs updating
	inum_t inum;		// Inode number
	struct inode inode;	// Inode of this file
	USHORT revision;
	USHORT country;
};

struct cacheData
{
	USHORT pos;			// position of data buffer
	diskAddr blknum;		// absolute block number on disk
	UCHAR dirty;			// 1 if block needs to be written
	UCHAR used;			// 1 if this block is being used
};
#endif	// _STORE_HDR


// --- support -------------------------------------------------------------

#ifdef TESTING
static void fileAbort(int i)
{
	printf("error abort: %d\n", i);
	perror("");
	abort();
}
#endif

#if defined(ZIL_BIGENDIAN)
static void swapDiskAddr(diskAddr *datum)
{
	union {
		diskAddr a;
		UCHAR b[sizeof(diskAddr)];
	} tmp;
	UCHAR tmpb;

	tmp.a = *datum;
	for (int j=0; j < sizeof(*datum) / 2; j++)
	{
		tmpb = tmp.b[j];
		tmp.b[j] = tmp.b[sizeof(*datum)-1-j];
		tmp.b[sizeof(*datum)-1-j] = tmpb;
	}
	*datum = tmp.a;
}

static void swapUshort(USHORT *datum)
{
	union {
		USHORT a;
		UCHAR b[sizeof(USHORT)];
	} tmp;
	UCHAR tmpb;

	tmp.a = *datum;
	for (int j=0; j < sizeof(*datum) / 2; j++)
	{
		tmpb = tmp.b[j];
		tmp.b[j] = tmp.b[sizeof(*datum)-1-j];
		tmp.b[sizeof(*datum)-1-j] = tmpb;
	}
	*datum = tmp.a;
}

static void swapLong(long *datum)
{
	union {
		long a;
		UCHAR b[sizeof(long)];
	} tmp;
	UCHAR tmpb;

	tmp.a = *datum;
	for (int j=0; j < sizeof(*datum) / 2; j++)
	{
		tmpb = tmp.b[j];
		tmp.b[j] = tmp.b[sizeof(*datum)-1-j];
		tmp.b[sizeof(*datum)-1-j] = tmpb;
	}
	*datum = tmp.a;
}

static void swapTimet(time_t *datum)
{
	union {
		time_t a;
		UCHAR b[sizeof(time_t)];
	} tmp;
	UCHAR tmpb;

	tmp.a = *datum;
	for (int j=0; j < sizeof(*datum) / 2; j++)
	{
		tmpb = tmp.b[j];
		tmp.b[j] = tmp.b[sizeof(*datum)-1-j];
		tmp.b[sizeof(*datum)-1-j] = tmpb;
	}
	*datum = tmp.a;
}

static void swapInumt(inum_t *datum)
{
	union {
		inum_t a;
		UCHAR b[sizeof(inum_t)];
	} tmp;
	UCHAR tmpb;

	tmp.a = *datum;
	for (int j=0; j < sizeof(*datum) / 2; j++)
	{
		tmpb = tmp.b[j];
		tmp.b[j] = tmp.b[sizeof(*datum)-1-j];
		tmp.b[sizeof(*datum)-1-j] = tmpb;
	}
	*datum = tmp.a;
}

static void swapInode(inode *di)
{
	int i;

	swapLong(&di->size);
	swapLong(&di->createTime);
	swapLong(&di->modifyTime);
	for (i=0; i < LENGTHOF(di->direct); i++)
		swapDiskAddr(&di->direct[i]);
	swapDiskAddr(&di->sIndirect);
	swapDiskAddr(&di->dIndirect);
	swapUshort(&di->useCount);
	swapDiskAddr(&di->fragmentBlock);
	swapUshort(&di->fragmentIndex);
}

static void swapSuperBlock(superBlock *ds)
{
	int i;

	swapDiskAddr(&ds->signature.magicNumber);
	swapTimet(&ds->createTime);
	swapTimet(&ds->modifyTime);
	swapUshort(&ds->revision);
	swapUshort(&ds->freeInodeListHi);
	swapUshort(&ds->freeInodeListLo);
	swapDiskAddr(&ds->freeDataList);
	for (i=0; i < LENGTHOF(ds->inodeDirect); i++)
		swapDiskAddr(&ds->inodeDirect[i]);
	swapDiskAddr(&ds->inodeSIndirect);
	swapDiskAddr(&ds->inodeDIndirect);
}

#else
#define swapDiskAddr(x)
#define swapUshort(x)
#define swapInode(x)
#define swapSuperBlock(x)
#endif

#ifndef _STORE_HDR
static int writeAt(int fd, long pos, void *buf, int len)
{
	if (lseek(fd, pos, SEEK_SET) != pos)
	{
#ifdef TESTING
		fileAbort(errno);
#endif
		return errno;
	}
	if (write(fd, (char *)buf, len) != len)
	{
#ifdef TESTING
		fileAbort(errno);
#endif
		return errno;
	}
	return 0;
}

static int readAt(int fd, long pos, void *buf, int len)
{
	if (lseek(fd, pos, SEEK_SET) != pos)
	{
#ifdef TESTING
		fileAbort(errno);
#endif
		return errno;
	}
#if defined(_SUNOS4)
	if (read(fd, (char *)buf, len) != len)
#else
	if (read(fd, buf, len) != len)
#endif
	{
#ifdef TESTING
		fileAbort(errno);
#endif
		return errno;
	}
	return 0;
}

static void makeFullPath(char *tmppath)						// BUG.7178
{
#if defined(ZIL_UNIX)
	if (tmppath[0] != ZIL_DIRSEP)
	{
		char save[MAXPATHLEN];
		strcpy(save, tmppath);
		getcwd(tmppath, MAXPATHLEN);
		UI_STORAGE::AppendFullPath(tmppath, tmppath, save, NULL);
	}
#else
	if (tmppath[0] == ZIL_DIRSEP)
	{
		memmove(&tmppath[2], &tmppath[0], (strlen(tmppath)+1) * sizeof(tmppath[0]));
		tmppath[1] = ':';
#if defined(ZIL_MSWINDOWS) && defined(WIN32)
		unsigned drive;
		char currentDrive[MAX_PATH];
		GetCurrentDirectory(MAX_PATH, currentDrive);
		strupr(currentDrive);
		drive = currentDrive[0] - 'A' + 1;
		tmppath[0] = drive - 1 + 'A';
#elif defined(ZIL_OS2)
		ULONG pDrive, lDrive;
		DosQueryCurrentDisk(&pDrive, &lDrive);
		tmppath[0] = pDrive - 1 + 'A';
#elif defined(ZIL_MSDOS) || defined(ZIL_MSWINDOWS) || defined(__DVX__)
		unsigned drive;
		_dos_getdrive(&drive);
		tmppath[0] = drive - 1 + 'A';
#else
		????;   This is an error;
#endif
	}
	else if (tmppath[1] == ':' && tmppath[2] != ZIL_DIRSEP)
	{
		char save[MAXPATHLEN];
		strcpy(save, &tmppath[2]);
#if defined(ZIL_MSWINDOWS) && defined(WIN32)
		char currentDrive[MAX_PATH], newDrive[MAX_PATH];
		GetCurrentDirectory(MAX_PATH, currentDrive);
		sprintf(newDrive, "%c:", tmppath[0]);
		SetCurrentDirectory(newDrive);
		GetCurrentDirectory(MAXPATHLEN, &tmppath[2]);
		SetCurrentDirectory(currentDrive);
#elif defined(ZIL_OS2)
		ULONG drive, xxx;
		DosQueryCurrentDisk(&drive, &xxx);
		DosSetDefaultDisk(tmppath[1] - 'a' + 1);
		getcwd(&tmppath[2], CCHMAXPATH);
		DosSetDefaultDisk(drive);
#elif defined(ZIL_MSDOS) || defined(ZIL_MSWINDOWS) || defined(__DVX__)
#	if defined(__ZTC__) || defined(__WATCOMC__) || defined(__HIGHC__)
		unsigned drive, xxx;
		dos_getdrive(&drive);
		dos_setdrive(tmppath[0] - 'A' + 1, &xxx);
		getcwd(&tmppath[2], MAXPATHLEN-2);
		dos_setdrive(drive, &xxx);
#	else
		_getdcwd(tmppath[0] - 'A' + 1, &tmppath[2], MAXPATHLEN-2);
#	endif
#else
		????; This is an error;
#endif
		UI_STORAGE::AppendFullPath(tmppath, tmppath, save, NULL);
	}
	else if (!(tmppath[1] == ':' && tmppath[2] == ZIL_DIRSEP))
	{
		char save[MAXPATHLEN];
		strcpy(save, tmppath);
		getcwd(tmppath, MAXPATHLEN);
		UI_STORAGE::AppendFullPath(tmppath, tmppath, save, NULL);
	}
#endif
}

// --- UI_STORAGE statics --------------------------------------------------

void UI_STORAGE::AppendFullPath(char *fullPath, const char *pathName,
	const char *fileName, const char *extension)
{
	if (pathName != fullPath)
		strcpy(fullPath, pathName);
	if (fullPath[0] != '\0' && fullPath[ui_strlen(fullPath)-1]!= ZIL_DIRSEP
#if defined(ZIL_MSDOS) || defined(ZIL_MSWINDOWS) || defined(ZIL_OS2) || defined(__DVX__)
		&& fullPath[ui_strlen(fullPath)-1] != ':'
#endif
	   )
		strcat(fullPath, ZIL_DIRSEPSTR);
	strcat(fullPath, fileName);
	if (extension)
		ChangeExtension(fullPath, extension);
#if defined(ZIL_MSDOS) || defined(ZIL_MSWINDOWS)
	ui_strupr(fullPath);
#endif
}

void UI_STORAGE::ChangeExtension(char *pathName, const char *newExtension)
{
	char *oldExtension = strrchr(pathName, '.');
	if (oldExtension)
		*oldExtension = '\0';
	if (newExtension)
		strcat(pathName, newExtension);
}

int UI_STORAGE::ValidName(const char *name, int createStorage)
// Implements the following TRUTH table:					// BUG.1604
// createStorage =	TRUE	FALSE
// invalid path		FALSE	FALSE
// valid path/no file	TRUE	FALSE
// valid path/file RW	TRUE	TRUE
// valid path/file RO	FALSE	TRUE
// valid path/protected	FALSE	FALSE
{
	int fd = open((char *)name, O_RDWR|O_CREAT|O_EXCL, S_IREAD|S_IWRITE);
	if (fd < 0)
	{
		if (errno != EEXIST)
			return (FALSE);		// invalid path
		fd = open((char *)name, O_RDWR);
		if (fd >= 0)
		{
			close(fd);
			return (TRUE);		// valid path/file RW
		}
		fd = open((char *)name, O_RDONLY);
		if (fd < 0)
			return (FALSE);		// valid path/file protected
		close(fd);
		return (!createStorage);	// valid path/file RO
	}
	close(fd);
	unlink(name);
	return createStorage;			// valid path/no file
}

void UI_STORAGE::StripFullPath(const char *fullPath, char *pathName,
	char *fileName, char *objectName, char *objectPathName)
{
	int i;

	// Determine the path/file split area.
	const char *endPath = strrchr(fullPath, ZIL_DIRSEP);
#if defined(ZIL_MSDOS) || defined(ZIL_MSWINDOWS) || defined(ZIL_OS2)
	if (!endPath)
		endPath = strrchr(fullPath, ':');
#endif
	if (!endPath)
		endPath = fullPath;
#if defined(ZIL_MSDOS) || defined(ZIL_MSWINDOWS) || defined(ZIL_OS2)
	if (*endPath == ':')
		endPath++;
	if (*endPath == '\\' && endPath-fullPath == 2 && endPath[-1] == ':')
		endPath++;
#endif
	// Determine the file/object split area.
	const char *endFile = strchr(endPath, DIRECTORY_SEPARATOR);
	if (!endFile)
		endFile = &endPath[ui_strlen(endPath)];

	// find the end of the object path name
	const char *endOPath = strrchr(endFile, DIRECTORY_SEPARATOR);
	if (!endOPath)
		endOPath = &endFile[ui_strlen(endFile)];

	// Construct the path name.
	if (pathName)
	{
		i = (int)(endPath - fullPath);
		strncpy(pathName, fullPath, i);
		pathName[i] = '\0';
#if defined(ZIL_MSDOS) || defined(ZIL_MSWINDOWS)
		ui_strupr(pathName);
#endif
	}
	if (*endPath == ZIL_DIRSEP)
		endPath++;

	// Construct the file name.
	if (fileName)
	{
		i = (int)(endFile - endPath);
		strncpy(fileName, endPath, i);
		fileName[i] = '\0';
#if defined(ZIL_MSDOS) || defined(ZIL_MSWINDOWS)
		ui_strupr(fileName);
#endif
	}

	// Construct the object path name.
	if (objectPathName)
	{
		i = (int)(endOPath - endFile);
		strncpy(objectPathName, endFile, i);
		objectPathName[i] = '\0';
	}
	if (*endOPath == DIRECTORY_SEPARATOR) endOPath++;

	// Construct the object name.
	if (objectName)
		strcpy(objectName, endOPath);
}


// --- UI_STORAGE privates -------------------------------------------------

int UI_STORAGE::deallocateSpace(struct inode *ientry)
{
	int j, i;
	diskAddr *iblk1, *iblk2;

	for (i=0; i < LENGTHOF(ientry->direct); i++)
	{
		freeData(ientry->direct[i]);
		ientry->direct[i] = 0;
	}
	if (ientry->sIndirect != 0)
	{
		iblk1 = (diskAddr *)readData(ientry->sIndirect);
		if (iblk1 == NULL)
			return TRUE;
		for (i=0; i < DISK_ADDR_PER_DATA_BLOCK; i++)
			freeData(iblk1[i]);
		freeData(ientry->sIndirect);
		ientry->sIndirect = 0;
	}
	if (ientry->dIndirect != 0)
	{
		iblk1 = (diskAddr *)readData(ientry->dIndirect);
		if (iblk1 == NULL)
			return TRUE;
		for (i=0; i < DISK_ADDR_PER_DATA_BLOCK; i++)
		{
			if (iblk1[i] == 0)
				continue;
			iblk2 = (diskAddr *)readData(iblk1[i]);
			if (iblk2 == NULL)
				return TRUE;
			for (j=0; j < DISK_ADDR_PER_DATA_BLOCK; j++)
				freeData(iblk2[j]);
			freeData(iblk1[i]);
		}
		freeData(ientry->dIndirect);
		ientry->dIndirect = 0;
	}
	return FALSE;
}

int UI_STORAGE::checkOpen(inum_t inum)
{
	int i;

	for (i=0; i < openLen; i++)
		if (openFiles[i].openCount && openFiles[i].inum == inum)
			return i;
	storageError = EACCES;
	return -1;
}

UI_STORAGE_OBJECT *UI_STORAGE::walkPath(const char *name, int stripLast)
{
	char *strt, *stop, *tmpname;
	UI_STORAGE_OBJECT *startdir;

	if (!name || ! *name)
	{
//		storageError = EINVAL;
		return NULL;
	}
	startdir = currentDirectory;
	tmpname = new char[ui_strlen(name)+2];
	strt = stop = strcpy(tmpname, name);
	tmpname[ui_strlen(name)+1] = '\0';
	// if (stripLast) then don't try to get into the last part of the path
	if (stripLast)
	{
		int i = ui_strlen(tmpname);
		while (i && tmpname[i] != DIRECTORY_SEPARATOR)
			i--;
		if (i == 0 && tmpname[0] == DIRECTORY_SEPARATOR)
			i++;
		tmpname[i] = '\0';
		tmpname[i+1] = '\0';
	}
	if (!*strt)
	{
#if defined(ZIL_DELETE)
		delete []tmpname;
#else
		delete [ui_strlen(name)+2]tmpname;				// BUG.internal
#endif
		return NULL;
	}
	for (;;)
	{
		// strip out a name
		while (*stop && *stop != DIRECTORY_SEPARATOR)
			stop++;
		*stop = '\0';
		if (stop == strt)
		{
			// null name, start from root
			if (openFiles[startdir->inodeIndex].inum != 0)
			{
				static struct directoryEntry dentry = { 0, 0, 0, 0, "" };
				struct inode ientry;

				if (rwInode((inum_t)0, &ientry, ZIL_IREAD) < 0)
					break;
				if (startdir != currentDirectory)
					// No one does this right yet...
					// startdir->~UI_STORAGE_OBJECT();
					startdir->partialDestruct();
				else
					startdir = new UI_STORAGE_OBJECT();
				startdir->flags = currentDirectory->flags;
				startdir->openTheFile(*this, &dentry, &ientry,
						      FALSE);
			}
			stop++;
			if (*stop == '\0')
			{
#if defined(ZIL_DELETE)
				delete []tmpname;
#else
				delete [ui_strlen(name)+1]tmpname;
#endif
				return startdir;
			}
			strt = stop;
		}
		else if (! *strt)
		{
//			storageError = EINVAL;
			break;
		}
		else if (!startdir->findName(strt))
		{
			// it doesn't exist
//			storageError = ENOENT;
			break;
		}
		else
		{
			UI_STORAGE_OBJECT *dir = new UI_STORAGE_OBJECT();
			struct directoryEntry dentry;
			struct inode ientry;
			int i;

			startdir->Load(&dentry);
			dir->objectID = dentry.objectID;
			(void) strcpy(dir->stringID, dentry.stringID);
			dir->flags = startdir->flags;
			if ((i = checkOpen(dentry.inum)) < 0)
			{
				storageError = 0;	// not an error yet
				if (rwInode(dentry.inum, &ientry, ZIL_IREAD) < 0)
				{
					delete dir;
					break;
				}
			}
			else
				ientry = openFiles[i].inode;
			dir->openTheFile(*this, &dentry, &ientry, FALSE);
			if (!(openFiles[dir->inodeIndex].inode.useCount &
			      DIRECTORY_TAG))
			{
				delete dir;
//				storageError = ENOENT;
				break;
			}
			if (currentDirectory != startdir)
				delete startdir;
			startdir = dir;
			stop++;
			if (*stop == '\0')
			{
#if defined(ZIL_DELETE)
				delete []tmpname;
#else
				delete [ui_strlen(name)+1]tmpname;
#endif
				return startdir;
			}
			strt = stop;
		}
	}
	if (currentDirectory != startdir)
		delete startdir;
#if defined(ZIL_DELETE)
	delete []tmpname;
#else
	delete [ui_strlen(name)+1]tmpname;
#endif
	return NULL;
}

void UI_STORAGE::walkPartialPath(const char *pathname,
				 UI_STORAGE_OBJECT **parentdir,
				 const char **filename)
{
	*parentdir = walkPath(pathname, TRUE);
	*filename = pathname + ui_strlen(pathname);
	while (*filename != pathname && **filename != DIRECTORY_SEPARATOR)
		(*filename)--;
	if (**filename == DIRECTORY_SEPARATOR)
		(*filename)++;
	if (*parentdir == NULL)
		*parentdir = currentDirectory;
}

void UI_STORAGE::freeData(unsigned int blknum)
{
	int i;
	struct freeListBlock *flblk, *flist;

	if (blknum == 0)
		return;
	flblk = (freeListBlock *)readData(blknum);
	if (flblk == NULL)
		return;
	for (i=0; i < DISK_ADDR_PER_DATA_BLOCK-1; i++)
		flblk->freeBlocks[i] = 0;
	if (sb->freeDataList == 0)
	{
		flblk->next = 0;
		sb->freeDataList = blknum;
	}
	else
	{
		flist = (freeListBlock *)readData(sb->freeDataList);
		if (flist == NULL)
			return;
		for (i=0; i < DISK_ADDR_PER_DATA_BLOCK-1; i++)
			if (flist->freeBlocks[i] == 0)
				break;
		if (i >= DISK_ADDR_PER_DATA_BLOCK-1)
		{
			flblk->next = sb->freeDataList;
			swapDiskAddr(&flblk->next);
			sb->freeDataList = blknum;
		}
		else
		{
			flist->freeBlocks[i] = blknum;
			swapDiskAddr(&flist->freeBlocks[i]);
		}
		writeData(flist, TRUE);
	}
	writeData(flblk, TRUE);
}

diskAddr UI_STORAGE::allocData(void)
{
	diskAddr blkptr;

	if (sb->freeDataList == 0)
	{
		int i;
		long pos;
		char *buff;

		// extend the file
		pos = lseek(fd, 0L, SEEK_END);
		if (pos < 0L)
		{
#ifdef TESTING
			fileAbort(errno);
#endif
			storageError = errno;
			return 0;
		}
		blkptr = (USHORT)(pos / BYTES_PER_BLOCK);
		if (pos % BYTES_PER_BLOCK != 0) blkptr++;
		buff = new char[BYTES_PER_BLOCK];
		memset(buff, 0, BYTES_PER_BLOCK);
		i = write(fd, buff, BYTES_PER_BLOCK);
#if defined(ZIL_DELETE)
		delete []buff;
#else
		delete [BYTES_PER_BLOCK]buff;
#endif
		if (i != BYTES_PER_BLOCK)
		{
#ifdef TESTING
			fileAbort(errno);
#endif
			storageError = errno;
			return 0;
		}
	}
	else
	{
		int i;
		struct freeListBlock *flblk;

		flblk = (freeListBlock *)readData(sb->freeDataList);
		if (flblk == NULL)
			return 0;
		for (i=0; i < DISK_ADDR_PER_DATA_BLOCK-1; i++)
			if (flblk->freeBlocks[i] != 0)
				break;
		if (i < DISK_ADDR_PER_DATA_BLOCK-1)
		{
			blkptr = flblk->freeBlocks[i];
			flblk->freeBlocks[i] = 0;
			swapDiskAddr(&blkptr);
		}
		else
		{
			blkptr = sb->freeDataList;
			sb->freeDataList = flblk->next;
			swapDiskAddr(&sb->freeDataList);
		}
		writeData(flblk, i < DISK_ADDR_PER_DATA_BLOCK-1);
	}
	return blkptr;
}

diskAddr UI_STORAGE::appendInode(inum_t inum)
{
	int i, j;
	struct inode *ipage, *tmp;
	long pos;

	tmp = ipage = new struct inode[INODES_PER_INODE_BLOCK];
	// append another block to file and make them free
	for (i=0; i < INODES_PER_INODE_BLOCK; i++, tmp++)
	{
		tmp->size = 0;
		tmp->createTime = 0;
		tmp->modifyTime = 0;
		for (j=0; j < LENGTHOF(tmp->direct); j++)
			tmp->direct[j] = 0;
		tmp->sIndirect = 0;
		tmp->dIndirect = 0;
		tmp->useCount = 0;
		setFragmentWithInum(tmp->fragmentIndex, tmp->fragmentBlock,
				    inum + i + 1);
		swapInode(tmp);
	}
	pos = lseek(fd, 0L, SEEK_END);
	if (pos < 0L)
		return 0;
	if (pos % BYTES_PER_BLOCK != 0)
	{
		pos = lseek(fd, BYTES_PER_BLOCK - (pos % BYTES_PER_BLOCK), SEEK_CUR);
		if (pos < 0L)
			return 0;
	}
	i = write(fd, (char *)ipage, BYTES_PER_INODE_BLOCK);
#if defined(ZIL_DELETE)
	delete []ipage;
#else
	delete [INODES_PER_INODE_BLOCK]ipage;
#endif
	if (i != BYTES_PER_INODE_BLOCK)
		return 0;
	if (inum)
		setFragmentWithInum(sb->freeInodeListLo, sb->freeInodeListHi,
				    inum);
	return (USHORT)(pos / BYTES_PER_BLOCK);
}

inum_t UI_STORAGE::allocInode(void)
// returns 0 on failure or end of inodes.  This is safe because
// inode 0 is always hard assigned to the root directory.
{
	inum_t i;
	struct inode ibuf;

	setInumWithFragment(i, sb->freeInodeListLo, sb->freeInodeListHi);
	if (rwInode(i, &ibuf, ZIL_IREAD) < 0)
		return 0;
	sb->freeInodeListLo = ibuf.fragmentIndex;
	sb->freeInodeListHi = ibuf.fragmentBlock;
	return i;
}

#define INODE_INDIRECT_SIZE	(DISK_ADDR_PER_INODE_BLOCK*sizeof(*iblk))

int UI_STORAGE::rwInode(inum_t inum, inode *ientry, int direction)
{
	diskAddr iindex;
	diskAddr inumblk, inumbyt;
	long pos;

//	iindex = inum / INODES_PER_INODE_BLOCK;
//	inumbyt = inum % INODES_PER_INODE_BLOCK;
	iindex = (USHORT)(inum / INODES_PER_INODE_BLOCK);
	inumbyt = (USHORT)(inum % INODES_PER_INODE_BLOCK);

	// direct blocks
	if (iindex < LENGTHOF(sb->inodeDirect))
	{
		if (sb->inodeDirect[iindex] == 0)
			sb->inodeDirect[iindex] = appendInode(inum);
		inumblk = sb->inodeDirect[iindex];
	}
	else
	{
		// for single indirect blocks
		iindex -= LENGTHOF(sb->inodeDirect);
		// try to do the first stage of double indirect blocks
		diskAddr *iblk = new diskAddr[DISK_ADDR_PER_INODE_BLOCK];
		if (iindex >= DISK_ADDR_PER_INODE_BLOCK)
		{
			iindex -= DISK_ADDR_PER_INODE_BLOCK;
			if (iindex >= DISK_ADDR_PER_INODE_BLOCK)
			{
#if defined(ZIL_DELETE)
				delete []iblk;
#else
				delete [DISK_ADDR_PER_INODE_BLOCK]iblk;
#endif
#ifdef TESTING
				fileAbort(EINVAL);
#endif
				storageError = EINVAL;
				return -1;
			}
			if (sb->inodeDIndirect == 0)
				sb->inodeDIndirect = appendInode(0);
			pos = sb->inodeDIndirect * BYTES_PER_BLOCK;
			storageError = readAt(fd, pos, iblk,
					      INODE_INDIRECT_SIZE);
			diskAddr blkind = iindex % DISK_ADDR_PER_INODE_BLOCK;
			diskAddr blknum = iindex / DISK_ADDR_PER_INODE_BLOCK;
			iindex = iblk[blknum];
			if (storageError == 0 && iindex == 0)
			{
				iindex = iblk[blknum] = appendInode(0);
				swapDiskAddr(&iblk[blknum]);
				storageError = writeAt(fd, pos, iblk,
						       INODE_INDIRECT_SIZE);
			}
			else
				swapDiskAddr(&iindex);
			pos = iindex * BYTES_PER_BLOCK;
			iindex = blkind;
			if (storageError != 0)
			{
#if defined(ZIL_DELETE)
				delete []iblk;
#else
				delete [DISK_ADDR_PER_INODE_BLOCK]iblk;
#endif
				return -1;
			}
		}
		else
		{
			if (sb->inodeSIndirect == 0)
				sb->inodeSIndirect = appendInode(0);
			pos = sb->inodeSIndirect * BYTES_PER_BLOCK;
		}
		storageError = readAt(fd, pos, iblk, INODE_INDIRECT_SIZE);
		inumblk = iblk[iindex];
		if (storageError == 0 && inumblk == 0)
		{
			inumblk = iblk[iindex] = appendInode(inum);
			swapDiskAddr(&iblk[iindex]);
			storageError = writeAt(fd, pos, iblk,
					       INODE_INDIRECT_SIZE);
		}
		else
			swapDiskAddr(&inumblk);
#if defined(ZIL_DELETE)
		delete []iblk;
#else
		delete [DISK_ADDR_PER_INODE_BLOCK]iblk;
#endif
		if (storageError != 0)
			return -1;
	}
	pos = (long)inumblk*BYTES_PER_BLOCK + inumbyt*sizeof(inode);
	switch (direction)
	{
	case ZIL_IREAD:
		if ((storageError = readAt(fd, pos, ientry, sizeof(*ientry))) != 0)
			return -1;
		swapInode(ientry);
		return 0;
	case ZIL_IWRITE:
		swapInode(ientry);
		if ((storageError = writeAt(fd, pos, ientry, sizeof(*ientry))) != 0)
			return -1;
		swapInode(ientry);
		return 0;
	}
#ifdef TESTING
	fileAbort(EINVAL);
#endif
	storageError = EINVAL;
	return -1;
}

int UI_STORAGE::findSlot(void)
{
	int i;

	for (i=0; i < openLen; i++)
		if (openFiles[i].openCount <= 0) break;
	if (i >= openLen)
	{
		openFile *nopen;

		nopen = new openFile[openLen+NEW_INC];
		if (nopen == NULL)
		{
#ifdef TESTING
			fileAbort(ENOMEM);
#endif
			storageError = ENOMEM;
			return -1;
		}
		for (i=0; i < openLen; i++)
			nopen[i] = openFiles[i];
		for (i=openLen; i < openLen+NEW_INC; i++)
			nopen[i].openCount = 0;
#if defined(ZIL_DELETE)
		delete []openFiles;
#else
		delete [openLen]openFiles;
#endif
		openFiles = nopen;
		i = openLen;
		openLen += NEW_INC;
	}
	return i;
}


void *UI_STORAGE::readData(diskAddr blknum)
{
	int i, j, cpos;
	long dpos;
	struct cacheData tmpcd;

	if (blknum == 0)
		return NULL;
	for (i=0; i < cacheLen; i++)
	{
		if (cd[i].blknum == blknum)
		{
			tmpcd = cd[i];
			tmpcd.used++;
			for (j=i; j > 0; j--)
				cd[j] = cd[j-1];
			cd[0] = tmpcd;
			return &cache[tmpcd.pos*BYTES_PER_BLOCK];
		}
	}
	tmpcd = cd[cacheLen-1];
	for (j=cacheLen-1; j > 0; j--)
		cd[j] = cd[j-1];
	cpos = tmpcd.pos * BYTES_PER_BLOCK;
	if (tmpcd.dirty)
	{
		dpos = (long)tmpcd.blknum * BYTES_PER_BLOCK;
		if ((storageError = writeAt(fd, dpos, &cache[cpos], BYTES_PER_BLOCK)) != 0)
			return NULL;
		tmpcd.dirty = 0;
	}
	tmpcd.blknum = blknum;
	tmpcd.used = 1;
	cd[0] = tmpcd;
	dpos = (long)blknum * BYTES_PER_BLOCK;
	if ((storageError = readAt(fd, dpos, &cache[cpos], BYTES_PER_BLOCK)) != 0)
		return NULL;
	return &cache[cpos];
}

void UI_STORAGE::writeData(void *data, int modified)
{
	USHORT i;
	int j;

	if ((char *)data < cache || (char *)data >= &cache[cacheLen*BYTES_PER_BLOCK])
	{
		storageError = ERANGE;
		abort();
	}
	i = ((USHORT)((char *)data - cache) / BYTES_PER_BLOCK);
	// This is probably always 0 (or 1)
	for (j=0; j < cacheLen; j++)
		if (cd[j].pos == i)
		{
			// FlagSet() is probably overkill.
			cd[j].dirty |= FlagSet(flags, UIS_READWRITE) && modified;
			cd[j].used = 0;
			return;
		}
	// storageError = ERANGE;
#if !defined(ZIL_MSWINDOWS)
	printf("Fatal internal UI_STORAGE error.\n");
#endif
	abort();
}

// --- UI_STORAGE publics --------------------------------------------------

UI_STORAGE::UI_STORAGE(const char *name, UIS_FLAGS pflags)
{
#if defined(ZIL_LINKBUG)
	extern void z_store_dummy(void);		// Bug fix for Zortech & Microsoft linkers.
	z_store_dummy();
#endif

	int i;

	storageError = 0;
	openLen = 0;
	openFiles = NULL;
	currentDirectory = NULL;
	firstTime = 1;
	fd = -1;
	sb = NULL;
	cacheLen = 0;
	cd = NULL;
	cache = NULL;

	flags = pflags;
	StripFullPath(name, pname, fname);
	if (!FlagSet(flags, UIS_CREATE))
	{
		struct stat statb;
		char tmp[MAXPATHLEN];
		const char *path = "";
		int first = 1;

		while (path != NULL)
		{
			AppendFullPath(tmp, path, pname, NULL);
			AppendFullPath(tmp, tmp, fname, NULL);
		if (stat(tmp, &statb) >= 0) break;
			path = searchPath ? 
				(first ? searchPath->FirstPathName() : searchPath->NextPathName()) : NULL;
			first = 0;
		}
		if (path != NULL)
		{
			AppendFullPath(tmp, path, pname, NULL);
			(void) strcpy(pname, tmp);
		}
		makeFullPath(pname);
		if (FlagSet(flags, UIS_OPENCREATE))
		{
			flags |= UIS_READWRITE;
			if (path == NULL)
				flags |= UIS_CREATE;
		}
	}
	else
		makeFullPath(pname);						// BUG.7178
	// create the file
	if (FlagSet(flags, UIS_READWRITE))
	{
#if defined(ZIL_UNIX)
		tmpnam(tname);
#else
		char tmppath[MAXPATHLEN];
		tmpnam(tmppath);
		makeFullPath(tmppath);						// BUG.7178
		strcpy(tname, tmppath);
#endif
		fd = open(tname, O_BINARY|O_CREAT|O_TRUNC|O_RDWR, S_IREAD|S_IWRITE);
	}
	else
	{
		char tmp[MAXPATHLEN];
		tname[0] = '\0';
		AppendFullPath(tmp, pname, fname, NULL);
		fd = open(tmp, O_BINARY|O_RDONLY);
	}
	if (fd < 0)
	{
#ifdef TESTING_ALL
		fileAbort(errno);
#endif
		storageError = errno;
		return;
	}
	sb = new superBlock;
	if (FlagSet(flags, UIS_CREATE))
	{
		// write the super block
		memset(sb, 0, sizeof(*sb));
		sb->signature = _signature;
		(void) time(&sb->createTime);
		sb->revision = 0;
		sb->unused = 0;
		if (Flush() < 0)
			return;
	}
	else if (FlagSet(flags, UIS_READWRITE))
	{
		// copy the file across
		int fd1, i;
		char *buff;
		char tmp[MAXPATHLEN];

		AppendFullPath(tmp, pname, fname, NULL);
		fd1 = open(tmp, O_BINARY|O_RDONLY);
		buff = new char[COPY_BYTES];
		lseek(fd, 0L, SEEK_SET);
		while ((i = read(fd1, buff, COPY_BYTES)) > 0)
			write(fd, buff, i);
#if defined(ZIL_DELETE)
		delete []buff;
#else
		delete [COPY_BYTES]buff;
#endif
		close(fd1);
	}
	storageError = readAt(fd, 0L, sb, sizeof(*sb));
	swapSuperBlock(sb);
	if (storageError == 0 && sb->signature.majorVersion == 3 &&
	    sb->signature.minorVersion == 0)
	{
		sb->freeInodeListHi = 0;
		sb->signature.majorVersion = MAJOR_VERSION;
		sb->signature.minorVersion = MINOR_VERSION;
	}
	if (storageError != 0 ||
	    sb->signature.magicNumber != MAGIC_NUMBER ||
	    sb->signature.majorVersion < MAJOR_VERSION)
	{
		storageError = EINVAL;
		if (tname[0])
			unlink(tname);
		close(fd);
		fd = -1;
		delete sb;
		return;
	}
	sb->revision++;
	cacheLen = cacheSize;
#if defined(DOSX286)
	// The DOS Extender can butt this up against the end of a segment
	// thereby creating a non-ANSI program (we can't compare a pointer
	// against the address following "cache" because it wraps around
	// to 0x0000).  This will fix it!!!
	cache = new char[cacheLen * BYTES_PER_DATA_BLOCK + 4] + 2;
#else
	cache = new char[cacheLen * BYTES_PER_DATA_BLOCK];
#endif
	cd = new cacheData[cacheLen];
	if (cache == NULL || cd == NULL)
	{
#if defined(ZIL_DELETE)
#if defined(DOSX286)
		delete [](cache - 2);
#else
		delete []cache;
#endif
		delete []cd;
#else
#if defined(DOSX286)
		delete [cacheLen * BYTES_PER_DATA_BLOCK + 4](cache - 2);
#else
		delete [cacheLen * BYTES_PER_DATA_BLOCK]cache;
#endif
		delete [cacheLen]cd;
#endif
		cd = NULL;
		cache = NULL;
		cacheLen = 0;
		storageError = ENOMEM;
		close(fd);
		fd = -1;
		return;
	}
	for (i=0; i < cacheLen; i++)
	{
		cd[i].pos = i;
		cd[i].blknum = 0;
		cd[i].dirty = 0;
		cd[i].used = 0;
	}
	// initialize the open file table to all unused
	openLen = NEW_INC;
	openFiles = new openFile[openLen];
	for (i=0; i < openLen; i++)
		openFiles[i].openCount = 0;
	if (FlagSet(flags, UIS_CREATE))
	{
		if (MkDir(ROOT) < 0)
			return;
	}
	else
	{
		// this is start up code
		currentDirectory = new UI_STORAGE_OBJECT();
		currentDirectory->file = this;
		currentDirectory->inodeIndex = 0;
		currentDirectory->position = 0;
		currentDirectory->flags = flags;
		openFiles[0].inum = 0;
		openFiles[0].openCount = 1;
		openFiles[0].modified = 0;
		if (rwInode((inum_t)0, &openFiles[0].inode, ZIL_IREAD) < 0)
			return;
	}
	storageError = 0;
}

UI_STORAGE::~UI_STORAGE(void)
{
	if (fd >= 0)
	{
		Flush();
		delete currentDirectory;
		close(fd);
		if (tname[0])
			unlink(tname);
		fd = -1;
#if defined(ZIL_DELETE)
#	if defined(DOSX286)
		delete [](cache - 2);
#	else
		delete []cache;
#	endif
		delete []cd;
		delete []openFiles;
#else
#	if defined(DOSX286)
		delete [cacheLen * BYTES_PER_DATA_BLOCK + 4](cache - 2);
#	else
		delete [cacheLen * BYTES_PER_DATA_BLOCK]cache;
#	endif
		delete [cacheLen]cd;
		delete [openLen]openFiles;
#endif
		delete sb;
	}
}

int UI_STORAGE::Save(int revisions)
{
	char tmp[MAXPATHLEN];

	AppendFullPath(tmp, pname, fname, NULL);
	CheckStorageError(-1);
	if (!FlagSet(flags, UIS_READWRITE))
	{
		storageError = EACCES;
		return -1;
	}
	if (modified)
		(void) time(&sb->modifyTime);
	if (Flush() < 0)
		return -1;
	if (firstTime)
	{
		char tmp1[MAXPATHLEN], tmp2[MAXPATHLEN];

		AppendFullPath(tmp1, pname, fname, ZIL_BAK);
		(void) strcpy(tmp2, tmp1);
		tmp1[ui_strlen(tmp1)-1] = revisions + '0';
		unlink(tmp1);
		for (int j=revisions-1; j > 0; j--)
		{
			tmp2[ui_strlen(tmp2)-1] = j + '0';
			rename(tmp2, tmp1);
			tmp1[ui_strlen(tmp1)-1] = j + '0';
		}
		if (revisions) rename(tmp, tmp1);
	}
	int fd1, i;
	char *buff;

	fd1 = open(tmp, O_BINARY|O_CREAT|O_TRUNC|O_RDWR, S_IREAD|S_IWRITE);
	lseek(fd, 0L, SEEK_SET);
	buff = new char[COPY_BYTES];
	while ((i = read(fd, buff, COPY_BYTES)) > 0)
		if (write(fd1, buff, i) != i)
			return -1;
	if (i < 0)
		return -1;
#if defined(ZIL_DELETE)
	delete []buff;
#else
	delete [COPY_BYTES]buff;
#endif
	close(fd1);
	firstTime = 0;
	return 0;
}

int UI_STORAGE::SaveAs(const char *newName, int revisions)
{
	CheckStorageError(-1);
	if (!FlagSet(flags, UIS_READWRITE))
	{
		storageError = EACCES;
		return -1;
	}
	if (modified)
		(void) time(&sb->modifyTime);
	if (Flush() < 0)
		return -1;
	StripFullPath(newName, pname, fname);
	firstTime = 1;
	return Save(revisions);
}

int UI_STORAGE::Version(void)
{
	return 100*sb->signature.majorVersion + sb->signature.minorVersion;
}

int UI_STORAGE::Flush(void)
{
	int i;
	long pos;

	CheckStorageError(-1);
	for (i=0; i < cacheLen; i++)
		if (cd[i].dirty)
		{
			pos = (long)cd[i].blknum * BYTES_PER_BLOCK;
			if ((storageError = writeAt(fd, pos, &cache[cd[i].pos * BYTES_PER_BLOCK], BYTES_PER_BLOCK)) != 0)
				return -1;
			cd[i].dirty = 0;
		}
	swapSuperBlock(sb);
	if ((storageError = writeAt(fd, 0L, sb, sizeof(*sb))) != 0)
		return -1;
	swapSuperBlock(sb);
	for (i=0; i < openLen; i++)
		if (openFiles[i].openCount > 0)
			if (rwInode(openFiles[i].inum, &openFiles[i].inode, ZIL_IWRITE) < 0)
				return -1;
#if 0
	extern unsigned int _version;
	if (_version >= 0x0330)
		bdos(0x68, 
#endif
	return 0;
}

int UI_STORAGE::RenameObject(const char *oldObject, const char *newName)
{
	long delentry;
	struct directoryEntry dentry, oldentry;

	CheckStorageError(-1);
	if (currentDirectory->findName(newName))
	{
		// if it does exist
		storageError = EACCES;
		return -1;
	}
	if (!currentDirectory->findName(oldObject))
	{
		// if it doesn't exist
		storageError = ENOENT;
		return -1;
	}
	delentry = currentDirectory->position;
	currentDirectory->Load(&oldentry);
	// if it is already open, abort
	if (checkOpen(oldentry.inum) >= 0)
		return -1;
	storageError = 0;
	struct openFile *o = &openFiles[currentDirectory->inodeIndex];
	long pos = currentDirectory->position;

	while (pos < o->inode.size)
	{
		currentDirectory->position = pos;
		currentDirectory->Load(&dentry);
		pos = currentDirectory->position;
 
		currentDirectory->position = delentry;
		currentDirectory->Store(&dentry);
		delentry = currentDirectory->position;
	}
	(void) strcpy(oldentry.stringID, newName);
	currentDirectory->Store(&oldentry);
	o->inode.size = currentDirectory->position;
	return 0;
}

int UI_STORAGE::DestroyObject(const char *name)
{
	long delentry;
	struct directoryEntry dentry;
	struct inode ientry;
	UI_STORAGE_OBJECT *parentdir;
	const char *fname;
	int retval;

	retval = -1;
	CheckStorageError(-1);
	walkPartialPath(name, &parentdir, &fname);
	if (!parentdir->findName(fname))
	{
		// if it doesn't exist
#ifdef TESTING_ALL
		fileAbort(ENOENT);
#endif
		storageError = ENOENT;
		goto EXIT;
	}
	delentry = parentdir->position;
	parentdir->Load(&dentry);
	// if it is already open, abort
	if (checkOpen(dentry.inum) >= 0)
	{
#ifdef TESTING_ALL
		fileAbort(storageError);
#endif
		goto EXIT;
	}
	if (rwInode(dentry.inum, &ientry, ZIL_IREAD) < 0)
	{
#ifdef TESTING_ALL
		fileAbort(storageError);
#endif
		goto EXIT;
	}
	// if it is a directory, abort
	if (ientry.useCount & DIRECTORY_TAG)
	{
#ifdef TESTING_ALL
		fileAbort(EACCES);
#endif
		storageError = EACCES;
		goto EXIT;
	}
	// if this isn't really a file return error.
	if (ientry.useCount <= 0)
	{
#ifdef TESTING_ALL
		fileAbort(EINVAL);
#endif
		storageError = EINVAL;
		goto EXIT;
	}

	{
		// Delete the entry out of the directory
		struct openFile *o = &openFiles[parentdir->inodeIndex];
		long pos = parentdir->position;
		struct directoryEntry tmpdir;

		while (pos < o->inode.size)
		{
			parentdir->position = pos;
			parentdir->Load(&tmpdir);
			pos = parentdir->position;
 
			parentdir->position = delentry;
			parentdir->Store(&tmpdir);
			delentry = parentdir->position;
		}
		o->inode.size = parentdir->position = delentry;
	}
	ientry.useCount--;
	if (ientry.useCount > 0)
	{
		if (parentdir != currentDirectory)
			delete parentdir;
		return rwInode(dentry.inum, &ientry, ZIL_IWRITE);
	}
	ientry.useCount = 0;
	if (deallocateSpace(&ientry))
	{
#ifdef TESTING_ALL
		fileAbort(storageError);
#endif
		goto EXIT;
	}
	ientry.fragmentIndex = sb->freeInodeListLo;
	ientry.fragmentBlock = sb->freeInodeListHi;
	setFragmentWithInum(sb->freeInodeListLo, sb->freeInodeListHi,
			    dentry.inum);
	if (rwInode(dentry.inum, &ientry, ZIL_IWRITE) >= 0)
		retval = 0;
#ifdef TESTING_ALL
	else
		fileAbort(storageError);
#endif
EXIT:
	if (parentdir != currentDirectory)
		delete parentdir;
	return retval;
}

int UI_STORAGE::MkDir(const char *name)
{
	int i, retval;
	struct directoryEntry dentry;
	struct inode ientry;
	UI_STORAGE_OBJECT *dir;
	UI_STORAGE_OBJECT *parentdir;
	const char *fname;

	retval = -1;
	CheckStorageError(-1);
#ifdef TESTING_ALL
	memset(&dentry, 0, sizeof(dentry));
#endif
	if (!FlagSet(flags, UIS_READWRITE))
		return -1;
	// this is ROOT
	if (strcmp(name, ROOT) == 0)
	{
		dentry.inum = allocInode();	// this will be a 0!!
		fname = name;
		parentdir = NULL;
	}
	else
	{
		walkPartialPath(name, &parentdir, &fname);
		// if it does exist
		if (parentdir->findName(fname))
		{
#ifdef TESTING_ALL
			fileAbort(EEXIST);
#endif
//			storageError = EEXIST;
			goto EXIT;
		}
		// allocate an inode
		if ((dentry.inum = allocInode()) == 0)
		{
#ifdef TESTING
			fileAbort(storageError);
#endif
			goto EXIT;
		}
	}
	dentry.objectID = 0;
	dentry.revision = 0;
	dentry.country = 0;

	ientry.useCount = 1;
	ientry.size = 0;
	for (i=0; i < LENGTHOF(ientry.direct); i++)
		ientry.direct[i] = 0;
	ientry.sIndirect = 0;
	ientry.dIndirect = 0;
	ientry.fragmentBlock = 0;
	ientry.fragmentIndex = 0;
	(void) time((time_t *)&ientry.createTime);
	modified = 0;
	sb->modifyTime = ientry.modifyTime = ientry.createTime;

	if ((i = findSlot()) >= 0)
	{
		struct openFile *o = &openFiles[i];

		o->inum = dentry.inum;
		o->openCount = 1;
		o->inode = ientry;
		o->modified = 0;

		dir = new UI_STORAGE_OBJECT();
		dir->inodeIndex = i;
		dir->file = this;
		dir->position = 0;
		dir->flags = UIS_READWRITE;

		if (currentDirectory == NULL)
			currentDirectory = dir;

		// Write the "." (myself) entry
		(void) strcpy(dentry.stringID, DOT);
		dir->Store(&dentry);
		if (dentry.inum != 0)
		{
			struct openFile *p = &openFiles[parentdir->inodeIndex];
			// Write my name in my parent
			(void) strcpy(dentry.stringID, fname);
			// write name to parent directory
			parentdir->position = p->inode.size;
			parentdir->Store(&dentry);
			// mark a pointer to ..
			dentry.inum = p->inum;
			p->inode.useCount++;
		}
		o->inode.useCount++;
		// Point me at my parent
		(void) strcpy(dentry.stringID, DOTDOT);
		dir->Store(&dentry);
		o->inode.useCount |= DIRECTORY_TAG;
		if (o->inum != 0)
			delete dir;
		retval = 0;
	}
EXIT:
	if (parentdir != currentDirectory)
		delete parentdir;
	return retval;
}

int UI_STORAGE::RmDir(const char *name)
{
	struct inode ientry;
	struct directoryEntry dentry;
	UI_STORAGE_OBJECT *parentdir;
	const char *fname;

	CheckStorageError(-1);

	walkPartialPath(name, &parentdir, &fname);
	if (!parentdir->findName(fname))
	{
		// if it doesn't exist
#ifdef TESTING_ALL
		fileAbort(ENOENT);
#endif
//		storageError = ENOENT;
		return -1;
	}
	parentdir->Load(&dentry);
	if (checkOpen(dentry.inum) >= 0)
		return -1;
	if (rwInode(dentry.inum, &ientry, ZIL_IREAD) < 0)
		return -1;
	// Make sure this is really a directory and it is only used 2 times
	// i.e. empty (except for "." and "..")
	if (ientry.useCount != (DIRECTORY_TAG|2))
	{
#ifdef TESTING_ALL
		fileAbort(EACCES);
#endif
//		storageError = EACCES;
		return -1;
	}
	// and not currentDirectory
	if (dentry.inum == openFiles[currentDirectory->inodeIndex].inum)
	{
#ifdef TESTING_ALL
		fileAbort(EACCES);
#endif
//		storageError = EACCES;
		return -1;
	}
	// deallocate it
	ientry.useCount &= ~DIRECTORY_TAG;
	ientry.useCount--;
// ????? what if useCount doesn't go to 0
	if (rwInode(dentry.inum, &ientry, ZIL_IWRITE) < 0)
		return -1;
	openFiles[parentdir->inodeIndex].inode.useCount--;
	if (parentdir != currentDirectory)
		delete parentdir;
	return DestroyObject(name);
}

int UI_STORAGE::ChDir(const char *name)
{
	UI_STORAGE_OBJECT *startdir;

	CheckStorageError(-1);
	startdir = walkPath(name, FALSE);
	if (startdir == NULL)
		return -1;
	if (startdir != currentDirectory)
		delete currentDirectory;
	currentDirectory = startdir;
	return 0;
}

UI_STATS_INFO *UI_STORAGE::Stats(void)
{
	static UI_STATS_INFO stats;

	stats.createTime = sb->createTime;
	stats.modifyTime = sb->modifyTime;
	stats.size = lseek(fd, 0L, SEEK_END);
	stats.useCount = 1;		// Unix will change this
	stats.revision = sb->revision;
	stats.countryID = 0;
	stats.inum = 0;			// Unix will change this
	return &stats;
}

// --- FindFirst -----------------------------------------------------------

static char searchInfo[MAX_NAME_LENGTH];

static const char *searchPat;
static long patPosition;
static OBJECTID searchID;
static long IDPosition;

static int matchString(const char *pattern, const char *item)
{
	while (*pattern)
	{
		if (*pattern == *item || *pattern == '?')
		{
			pattern++;
			item++;
			continue;
		}
		if (*pattern != '*')
			return FALSE;
		if (!pattern[1])
			return TRUE;
#if defined(__WATCOMC__)
		int i;
		for (i = 0; item[i]; i++)
			if (matchString(pattern+1, item+i))
				return TRUE;
#else
		for (int i = 0; item[i]; i++)
			if (matchString(pattern+1, item+i))
				return TRUE;
#endif
		return FALSE;
	}
	return !*item;
}

char *UI_STORAGE::FindFirstObject(const char *pattern)
{
	CheckStorageError(NULL);
	patPosition = 0;
	searchPat = pattern;
	return FindNextObject();
}

char *UI_STORAGE::FindNextObject(void)
{
	struct directoryEntry dentry;

	CheckStorageError(NULL);
	currentDirectory->position = patPosition;
	do {
		if (currentDirectory->position >= openFiles[currentDirectory->inodeIndex].inode.size)
			return NULL;
		currentDirectory->Load(&dentry);
	} while (!matchString(searchPat, dentry.stringID));
	patPosition = currentDirectory->position;
	(void) strcpy(searchInfo, dentry.stringID);
	return searchInfo;
}

char *UI_STORAGE::FindFirstID(OBJECTID nobjectID)
{
	CheckStorageError(NULL);
	IDPosition = 0;
	searchID = nobjectID;
	return FindNextID();
}

char *UI_STORAGE::FindNextID(void)
{
	struct directoryEntry dentry;

	CheckStorageError(NULL);
	currentDirectory->position = IDPosition;
	do {
		if (currentDirectory->position >= openFiles[currentDirectory->inodeIndex].inode.size)
			return NULL;
		currentDirectory->Load(&dentry);
	} while (dentry.objectID != searchID);
	IDPosition = currentDirectory->position;
	(void) strcpy(searchInfo, dentry.stringID);
	return searchInfo;
}

// --- UI_STORAGE_OBJECT privates ------------------------------------------

int UI_STORAGE_OBJECT::findName(const char *name)
{
	long i;
	struct directoryEntry dentry;

	// Assume this is a directory
	position = 0;
	while (position < file->openFiles[inodeIndex].inode.size)
	{
		i = position;
		if (Load(&dentry) < 0)
			return FALSE;
		if (strcmp(name, dentry.stringID) == 0)
		{
			position = i;
			return TRUE;
		}
	}
	return FALSE;
}


openFile *UI_STORAGE_OBJECT::checkObject(void)
{
	openFile *o;

	if (file == NULL)
	{
#ifdef TESTING
		fileAbort(ENOENT);
#endif
		objectError = ENOENT;
		return NULL;
	}
	if (inodeIndex < 0 || inodeIndex >= file->openLen)
	{
#ifdef TESTING
		fileAbort(EBADF);
#endif
		objectError = EBADF;
		return NULL;
	}
	o = &file->openFiles[inodeIndex];
	if (o->openCount < 0)
	{
#ifdef TESTING
		fileAbort(EBADF);
#endif
		objectError = EBADF;
		return NULL;
	}
	return o;
}

diskAddr UI_STORAGE_OBJECT::getBlockPtr(diskAddr blknum)
{
	unsigned int blkptr;
	diskAddr *iblk, retval;
	struct inode *inode;

#ifdef DOCACHEPTR
	for (int i=0; i < DOCACHEPTR; i++)
		if (cachedBlkptr[i] != 0 && blknum == cachedBlknum[i])
		{
			diskAddr tmpNum = cachedBlknum[i];
			diskAddr tmpPtr = cachedBlkptr[i];
			for (int j=i; j > 0; j--)
			{
				cachedBlknum[j] = cachedBlknum[j-1];
				cachedBlkptr[j] = cachedBlkptr[j-1];
			}
			cachedBlknum[0] = tmpNum;
			cachedBlkptr[0] = tmpPtr;
			return tmpPtr;
		}
	for (i=DOCACHEPTR-1; i > 0; i--)
	{
		cachedBlknum[i] = cachedBlknum[i-1];
		cachedBlkptr[i] = cachedBlkptr[i-1];
	}
	cachedBlknum[0] = blknum;
#endif
	// direct blocks
	inode = &file->openFiles[inodeIndex].inode;
	if (blknum < LENGTHOF(inode->direct))
	{
		if (inode->direct[blknum] == 0)
			inode->direct[blknum] = file->allocData();
#ifdef DOCACHEPTR
		cachedBlkptr[0] = inode->direct[blknum];
		return cachedBlkptr[0];
#else
		return inode->direct[blknum];
#endif
	}
	blknum -= LENGTHOF(inode->direct);
	// single indirect blocks
	if (blknum < DISK_ADDR_PER_DATA_BLOCK)
	{
		if (inode->sIndirect == 0)
			inode->sIndirect = file->allocData();
		iblk = (diskAddr *)file->readData(inode->sIndirect);
		if (iblk == NULL)
			return 0;
		retval = iblk[blknum];
		int modified = (retval == 0);					// BUG.internal
		if (modified)
		{
			retval = iblk[blknum] = file->allocData();
			swapDiskAddr(&iblk[blknum]);
		}
		else
			swapDiskAddr(&retval);
		file->writeData(iblk, modified);
#ifdef DOCACHEPTR
		cachedBlkptr[0] = retval;
#endif
		return retval;
	}
	blknum -= DISK_ADDR_PER_DATA_BLOCK;
	// double indirect blocks
	blkptr = blknum % DISK_ADDR_PER_DATA_BLOCK;
	blknum /= DISK_ADDR_PER_DATA_BLOCK;
	if (blknum < DISK_ADDR_PER_DATA_BLOCK)
	{
		if (inode->dIndirect == 0)
			inode->dIndirect = file->allocData();
		iblk = (diskAddr *)file->readData(inode->dIndirect);
		if (iblk == NULL)
			return 0;
		retval = iblk[blknum];
		int modified = (retval == 0);					// BUG.internal
		if (modified)
		{
			retval = iblk[blknum] = file->allocData();
			swapDiskAddr(&iblk[blknum]);
		}
		else
			swapDiskAddr(&retval);
		file->writeData(iblk, modified);
		iblk = (diskAddr *)file->readData(retval);
		if (iblk == NULL)
			return 0;
		retval = iblk[blkptr];
		modified = (retval == 0);					// BUG.internal
		if (modified)
		{
			retval = iblk[blkptr] = file->allocData();
			swapDiskAddr(&iblk[blkptr]);
		}
		else
			swapDiskAddr(&retval);
		file->writeData(iblk, modified);
#ifdef DOCACHEPTR
		cachedBlkptr[0] = retval;
#endif
		return retval;
	}
	return 0;		// error
}

void UI_STORAGE_OBJECT::openTheFile(UI_STORAGE &pfile,
				    struct directoryEntry *dentry,
				    struct inode *ientry, int truncate)
{
	int i;

	// if it is already open, dup the file
	if ((i = pfile.checkOpen(dentry->inum)) >= 0)
		pfile.openFiles[i].openCount++;
	else
	{
		pfile.storageError = 0;		// not an error yet
		// find an empty slot
		if ((i = pfile.findSlot()) < 0)
		{
			objectError = ENOMEM;
			return;
		}
		pfile.openFiles[i].inum = dentry->inum;
		pfile.openFiles[i].openCount = 1;
		pfile.openFiles[i].inode = *ientry;
		pfile.openFiles[i].modified = 0;
		pfile.openFiles[i].revision = dentry->revision;
		pfile.openFiles[i].country = dentry->country;
	}
	// This next line will be changed in 3.1 to get rid of space used
	if (truncate)
		pfile.openFiles[i].inode.size = 0;
	inodeIndex = i;
	position = 0;
	file = &pfile;
}

int UI_STORAGE_OBJECT::rwData(void *_buffer, unsigned N, int direction)
{
//	int i, cnt, cpy, pos;
	int i;
	unsigned cnt, cpy, pos;
	char *bblk;
	openFile *o;

	char *buffer = (char *)_buffer;

	CheckObjectError(-1);
	if (direction == ZIL_IWRITE && !FlagSet(flags, UIS_READWRITE))
		return -1;
	if ((o = checkObject()) == NULL)
		return -1;
	if (N <= 0)
		return 0;

	// Write the first partial block
//	pos = (short)(position % BYTES_PER_DATA_BLOCK);
	pos = (unsigned)(position % BYTES_PER_DATA_BLOCK);
	cpy = BYTES_PER_DATA_BLOCK - pos;
	i = (short)(position / BYTES_PER_DATA_BLOCK);
	cnt = 0;

//	if (direction == ZIL_IREAD && o->inode.size < position + N)
	if (direction == ZIL_IREAD && o->inode.size < (long)(position + N))
		N = (short)(o->inode.size - position);
	if (cpy > N) cpy = N;
	// write blocks until done
	while (cnt < N)
	{
		bblk = (char *)file->readData(getBlockPtr(i));
		if (bblk == NULL)
			return -1;
		if (direction == ZIL_IWRITE)
			memcpy(&bblk[pos], buffer, cpy);
		else
			memcpy(buffer, &bblk[pos], cpy);
		file->writeData(bblk, direction == ZIL_IWRITE);
		buffer += cpy;
		i++;
		cnt += cpy;
		pos = 0;
		cpy = stMIN(BYTES_PER_DATA_BLOCK, N-cnt);
	}
	position += cnt;
	if (direction == ZIL_IWRITE)
	{
		if (o->inode.size < position)
			o->inode.size = position;
		o->modified = 1;
		file->modified = 1;
	}
	return cnt;
}

// --- UI_STORAGE_OBJECT publics -------------------------------------------

UI_STORAGE_OBJECT::UI_STORAGE_OBJECT()
{
#ifdef DOCACHEPTR
	cachedBlknum = new diskAddr[DOCACHEPTR];
	cachedBlkptr = new diskAddr[DOCACHEPTR];
	for (int i=0; i < DOCACHEPTR; i++)
	{
		cachedBlknum[i] = 0;
		cachedBlkptr[i] = 0;
	}
#endif
	objectError = 0;
	file = NULL;
}

UI_STORAGE_OBJECT::UI_STORAGE_OBJECT(UI_STORAGE &pfile, const char *name,
				     OBJECTID nobjectID, UIS_FLAGS pflags)
{
	int i, found;
	struct inode ientry;
	struct directoryEntry dentry;
	UI_STORAGE_OBJECT *parentdir;
	const char *fname;

	objectError = 0;
	file = NULL;
	inodeIndex = -1;
	position = -1;
	cachedBlknum = cachedBlkptr = NULL;

	if (name == NULL)
	{
#ifdef TESTING
		fileAbort(EINVAL);
#endif
		objectError = EINVAL;
		return;
	}

	flags = pflags;
	if (!FlagSet(pfile.flags, UIS_READWRITE))
	{
		flags |= UIS_READ;
		flags &= ~UIS_READWRITE;
	}

	pfile.walkPartialPath(name, &parentdir, &fname);
	found = parentdir->findName(fname);
	if (FlagSet(flags, UIS_CREATE))
	{
		if (found)
			parentdir->file->DestroyObject(fname);
		found = FALSE;
	}

	if (!found)
	{
		// if it doesn't exist
		if (!FlagSet(flags, UIS_CREATE) &&
		    !FlagSet(flags, UIS_OPENCREATE))
		{
#ifdef TESTING_ALL
			fileAbort(ENOENT);
#endif
			objectError = ENOENT;
			goto EXIT;
		}
		// We ARE going to be able to write this object!!!
		flags |= UIS_READWRITE;
#ifdef TESTING_ALL
		memset(&dentry, 0, sizeof(dentry));
#endif
		if ((dentry.inum = pfile.allocInode()) == 0)
		{
#ifdef TESTING
			fileAbort(pfile.storageError);
#endif
			objectError = pfile.storageError;
			pfile.storageError = 0;
			goto EXIT;
		}
		objectID = dentry.objectID = nobjectID;
		(void) strcpy(dentry.stringID, fname);
		(void) strcpy(stringID, fname);
		dentry.revision = 0;
		dentry.country = 0;
		parentdir->Store(&dentry);

		ientry.useCount = 1;
		ientry.size = 0;
		for (i=0; i < LENGTHOF(ientry.direct); i++)
			ientry.direct[i] = 0;
		ientry.sIndirect = 0;
		ientry.dIndirect = 0;
		ientry.fragmentBlock = 0;
		ientry.fragmentIndex = 0;
	}
	else	// findName()
	{
		// if it does exist
		parentdir->Load(&dentry);
		objectID = dentry.objectID;
		(void) strcpy(stringID, dentry.stringID);
		if ((i = pfile.checkOpen(dentry.inum)) < 0) {
			pfile.storageError = 0;		// not an error yet
			if (pfile.rwInode(dentry.inum, &ientry, ZIL_IREAD) < 0)
			{
				objectError = pfile.storageError;
				pfile.storageError = 0;
				goto EXIT;
			}
		}
		else
			ientry = pfile.openFiles[i].inode;
		if (ientry.useCount == 0)
		{
#ifdef TESTING
			fileAbort(ENOENT);
#endif
			objectError = ENOENT;
			goto EXIT;
		}
	}
	if (FlagSet(flags, UIS_CREATE))
	{
		flags |= UIS_READWRITE;
		(void) time((time_t *)&ientry.createTime);
		pfile.modified = 0;
		pfile.sb->modifyTime = ientry.modifyTime =
			ientry.createTime;
	}
#ifdef DOCACHEPTR
	cachedBlknum = new diskAddr[DOCACHEPTR];
	cachedBlkptr = new diskAddr[DOCACHEPTR];
	for (i=0; i < DOCACHEPTR; i++)
	{
		cachedBlknum[i] = 0;
		cachedBlkptr[i] = 0;
	}
#endif
	openTheFile(pfile, &dentry, &ientry, FlagSet(flags, UIS_CREATE));
EXIT:
	if (parentdir != pfile.currentDirectory)
		delete parentdir;
}

void UI_STORAGE_OBJECT::partialDestruct(void)
// This is really the complete Destructor, but since we can't call
// it directly (some compilers won't allow it) and I need to call it,
// here it is with this name.
{
	openFile *o;

#ifdef DOCACHEPTR
#	if defined(ZIL_DELETE)
	delete []cachedBlknum;
	delete []cachedBlkptr;
#	else
	delete [DOCACHEPTR]cachedBlknum;
	delete [DOCACHEPTR]cachedBlkptr;
#	endif
#endif
	if ((o = checkObject()) == NULL)
		return;
	o->openCount--;
	if (o->openCount <= 0 && FlagSet(flags, UIS_READWRITE))
	{
		if (o->modified)
			(void) time((time_t *)&o->inode.modifyTime);
//		if (FlagSet(flags, UIS_TEMPORARY))
//			????;
		if (file->rwInode(o->inum, &o->inode, ZIL_IWRITE) < 0)
		{
			objectError = file->storageError;
			file->storageError = 0;
		}
	}
	inodeIndex = -1;
}

void UI_STORAGE_OBJECT::Touch(void)
{
	openFile *o;

	CheckObjectErrorNull();
	if ((o = checkObject()) == NULL)
		return;
	(void) time((time_t *)&o->inode.modifyTime);
	file->sb->modifyTime = o->inode.modifyTime;
	o->modified = file->modified = 0;
}

UI_STATS_INFO *UI_STORAGE_OBJECT::Stats(void)
{
	openFile *o;
	static UI_STATS_INFO stats;

	if ((o = checkObject()) == NULL)
		return NULL;
	stats.createTime = o->inode.createTime;
	stats.modifyTime = o->inode.modifyTime;
	stats.size = o->inode.size;
	stats.useCount = o->inode.useCount;
	stats.revision = o->revision;
	stats.countryID = o->country;
	stats.inum = o->inum;
	return &stats;
}


// --- Loads/Stores --------------------------------------------------------

#if defined(ZIL_BIGENDIAN)

int UI_STORAGE_OBJECT::Load(short *value)
{
	int i, j;
	UCHAR buff[sizeof(*value)];

	if ((i = rwData(buff, sizeof(*value), ZIL_IREAD)) > 0)
	{
		*value = 0;
		for (j=0; j < sizeof(*value); j++)
			*value |= (buff[j] << 8*j);
	}
	return i;
}

int UI_STORAGE_OBJECT::Load(USHORT *value)
{
	int i, j;
	UCHAR buff[sizeof(*value)];

	if ((i = rwData(buff, sizeof(*value), ZIL_IREAD)) > 0)
	{
		*value = 0;
		for (j=0; j < sizeof(*value); j++)
			*value |= (buff[j] << 8*j);
	}
	return i;
}

int UI_STORAGE_OBJECT::Load(long *value)
{
	int i, j;
	UCHAR buff[sizeof(*value)];

	if ((i = rwData(buff, sizeof(*value), ZIL_IREAD)) > 0)
	{
		*value = 0;
		for (j=0; j < sizeof(*value); j++)
			*value |= (buff[j] << 8*j);
	}
	return i;
}

int UI_STORAGE_OBJECT::Load(ULONG *value)
{
	int i, j;
	UCHAR buff[sizeof(*value)];

	if ((i = rwData(buff, sizeof(*value), ZIL_IREAD)) > 0)
	{
		*value = 0;
		for (j=0; j < sizeof(*value); j++)
			*value |= (buff[j] << 8*j);
	}
	return i;
}

int UI_STORAGE_OBJECT::Store(short value)
{
	int j;
	UCHAR buff[sizeof(value)];

	for (j=0; j < sizeof(value); j++)
		buff[j] = (UCHAR)((value >> 8*j) & 0xff);
	return rwData(buff, sizeof(value), ZIL_IWRITE);
}

int UI_STORAGE_OBJECT::Store(USHORT value)
{
	int j;
	UCHAR buff[sizeof(value)];

	for (j=0; j < sizeof(value); j++)
		buff[j] = (UCHAR)((value >> 8*j) & 0xff);
	return rwData(buff, sizeof(value), ZIL_IWRITE);
}

int UI_STORAGE_OBJECT::Store(long value)
{
	int j;
	UCHAR buff[sizeof(value)];

	for (j=0; j < sizeof(value); j++)
		buff[j] = (UCHAR)((value >> 8*j) & 0xff);
	return rwData(buff, sizeof(value), ZIL_IWRITE);
}

int UI_STORAGE_OBJECT::Store(ULONG value)
{
	int j;
	UCHAR buff[sizeof(value)];

	for (j=0; j < sizeof(value); j++)
		buff[j] = (UCHAR)((value >> 8*j) & 0xff);
	return rwData(buff, sizeof(value), ZIL_IWRITE);
}

#endif // ZIL_BIGENDIAN

int UI_STORAGE_OBJECT::Load(directoryEntry *dirent)
{
	int i, j;
	USHORT sinum;

	CheckObjectError(-1);
	if ((i = Load(&sinum)) < 0)
		return -1;
	if (sinum == 0xffff)
	{
		if ((j = Load(&dirent->inum)) < 0)
			return -1;
		i += j;
	}
	else
		dirent->inum = sinum;
	if ((j = Load(&dirent->objectID)) < 0)
		return -1;
	i += j;
	if ((j = Load(&dirent->revision)) < 0)
		return -1;
	i += j;
	if ((j = Load(&dirent->country)) < 0)
		return -1;
	i += j;
	if ((j = Load(dirent->stringID, sizeof(dirent->stringID))) < 0)
		return -1;
	return i + j;
}

int UI_STORAGE_OBJECT::Store(const directoryEntry *dirent)
{
	int i, j;

	CheckObjectError(-1);
	if (dirent->inum >= 0xFFFF)
	{
		if ((i = Store((USHORT)0xFFFF)) < 0)
			return -1;
		if ((j = Store(dirent->inum)) < 0)
			return -1;
		i += j;
	}
	else
	{
		if ((i = Store((USHORT)dirent->inum)) < 0)
			return -1;
	}
	if ((j = Store(dirent->objectID)) < 0)
		return -1;
	i += j;
	if ((j = Store(dirent->revision)) < 0)
		return -1;
	i += j;
	if ((j = Store(dirent->country)) < 0)
		return -1;
	i += j;
	if ((j = Store(dirent->stringID)) < 0)
		return -1;
	return i + j;
}

int UI_STORAGE_OBJECT::Load(char *string, int len)
{
	USHORT size;
	int cnt1, cnt2;

	CheckObjectError(-1);
	*string = '\0';
	cnt1 = Load(&size);
	if (cnt1 != sizeof(size))
		return cnt1;
	if (size > len)
	{
		position -= cnt1;
		return -1;
	}
	cnt2 = 0;
	if (size)
		cnt2 = rwData(string, size, ZIL_IREAD);
	string[size] = '\0';
	return (cnt2 != size ? cnt2 : cnt1 + cnt2);
}

int UI_STORAGE_OBJECT::Load(char **string)
{
	USHORT size;
	int cnt1, cnt2;

	CheckObjectError(-1);
	cnt1 = Load(&size);
	cnt2 = 0;
	if (cnt1 == sizeof(size) && size)
	{
		*string = new char[size + 1];
		cnt2 = rwData(*string, size, ZIL_IREAD);
		(*string)[size] = '\0';
	}
	else
		*string = NULL;
	return (cnt2 != size ? cnt2 : cnt1 + cnt2);
}

int UI_STORAGE_OBJECT::Store(const char *string)
{
	USHORT size;
	int cnt1, cnt2;

	CheckObjectError(-1);
	size = (string == NULL ? 0 : ui_strlen(string));
	cnt1 = Store(size);
	if (cnt1 != sizeof(size))
		return cnt1;
	cnt2 = (size == 0 ? 0 : rwData((void *)string, size, ZIL_IWRITE));
	return (cnt2 != size ? cnt2 : cnt1 + cnt2);
}

int UI_STORAGE::Link(const char *path1, const char *path2)
{
	const char *fname1, *fname2;
	UI_STORAGE_OBJECT *dir;
	struct inode ientry;
	struct directoryEntry dentry;

	CheckStorageError(-1);
	walkPartialPath(path1, &dir, &fname1);
	if (dir == NULL)
		return -1;
	if (!dir->findName(fname1))
	{
		// if it doesn't exist
		if (dir != currentDirectory)
			delete dir;
		storageError = ENOENT;
		return -1;
	}
	// get the inode of fname1;
	dir->Load(&dentry);
	rwInode(dentry.inum, &ientry, ZIL_IREAD);
	if (dir != currentDirectory)
		delete dir;
	// make sure it isn't a directory;
	if ((ientry.useCount & DIRECTORY_TAG) != 0)
	{
		storageError = EACCES;
		return -1;
	}
	walkPartialPath(path2, &dir, &fname2);
	if (dir == NULL)
		return -1;
	if (dir->findName(fname2))
	{
		if (dir != currentDirectory)
			delete dir;
		// if it does exist
		storageError = EACCES;
		return -1;
	}
	// create the entry fname2 in dir;
	strcpy(dentry.stringID, fname2);
	dir->Store(&dentry);
	if (dir != currentDirectory)
		delete dir;
	// update the inode to an extra entry
	ientry.useCount++;
	return rwInode(dentry.inum, &ientry, ZIL_IWRITE);
}
#endif	// _STORE_HDR

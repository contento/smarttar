//	ICO2DAT.CPP - MS Windows .ICO icon to Zinc .DAT icon format converter.
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

// Current limitations:
// 1 - Only 16 colors.
// 2 - Only 32x32 icons

#define VERSION	"3.50"

#include <io.h>
#include <fcntl.h>
//#include <sys\stat.h>
#include <stdio.h>
#include <string.h>
#include <ui_gen.hpp>

#if defined(_MSC_VER)
#	if defined(WIN32)
#		define open _lopen
#		define read (int)_lread
#		define write (int)_lwrite
#		define lseek _llseek
#		define close _lclose
#	else
#		define open	_open
#		define close	_close
#		define read	_read
#		define write	_write
#		define lseek	_lseek
#		define unlink	_unlink
#		define stat	_stat
#	endif
#	pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

#define BYTE	UCHAR
#define WORD	USHORT
#define DWORD	ULONG

#if defined(ZIL_MSDOS) || defined(ZIL_OS2)
struct RGBQUAD
{
	BYTE	rgbBlue;
	BYTE	rgbGreen;
	BYTE	rgbRed;
	BYTE	rgbReserved;
};

struct BITMAPINFOHEADER
{
	DWORD	biSize;
	DWORD	biWidth;
	DWORD	biHeight;
	WORD	biPlanes;
	WORD	biBitCount;
	DWORD	biCompression;
	DWORD	biSizeImage;
	DWORD	biXPelsPerMeter;
	DWORD	biYPelsPerMeter;
	DWORD	biClrUsed;
	DWORD	biClrImportant;
};
#endif

struct ICO_HEADER
{
	WORD	icoReserved;
	WORD	icoResourceType;
	WORD	icoResourceCount;
};

struct ICO_DESCRIPTOR
{
	BYTE	width;
	BYTE	height;
	BYTE	colorCount;
	BYTE	reserved1;
	WORD	reserved2;
	WORD	reserved3;
	DWORD	icoDIBSize;
	DWORD	icoDIBOffset;
};

// Map for matching RGB values to an index into _colorMap[16].
static RGBQUAD zincRGBValues[] =
{
	{ 0x00, 0x00, 0x00 },   // 0 - BLACK		
	{ 0x80, 0x00, 0x00 },	// 1 - BLUE		
	{ 0x00, 0x80, 0x00 },	// 2 - GREEN		
	{ 0x80, 0x80, 0x00 },	// 3 - CYAN		
	{ 0x00, 0x00, 0x80 },	// 4 - RED			
	{ 0x80, 0x00, 0x80 },	// 5 - MAGENTA		
	{ 0x00, 0x80, 0x80 },	// 6 - BROWN		
	{ 0xC0, 0xC0, 0xC0 },	// 7 - LIGHTGRAY	
	{ 0x80, 0x80, 0x80 },	// 8 - DARKGRAY	
	{ 0xFF, 0x00, 0x00 },	// 9 - LIGHTBLUE	
	{ 0x00, 0xFF, 0x00 },	// 10 - LIGHTGREEN	
	{ 0xFF, 0xFF, 0x00 },	// 11 - LIGHTCYAN	
	{ 0x00, 0x00, 0xFF },	// 12 - LIGHTRED	
	{ 0xFF, 0x00, 0xFF },	// 13 - LIGHTMAGENTA
	{ 0x00, 0xFF, 0xFF },	// 14 - YELLOW		
	{ 0xFF, 0xFF, 0xFF }	// 15 - WHITE		
};

main(int argc, char *argv[])
{
#if defined(ZIL_MSDOS)
	char *exeName = "ico2dat";
#elif defined(ZIL_MSWINDOWS)
	char *exeName = "wico2dat";
#elif defined(ZIL_OS2)
	char *exeName = "oico2dat";
#elif defined(ZIL_MOTIF)
	char *exeName = "ico2dat";
#endif

	ICO_HEADER header;
	ICO_DESCRIPTOR descriptor;
	RGBQUAD rgbColor;
	UCHAR zincRGBIndex[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	UCHAR *zincBitmap;
	
	int fileHandle;
	BYTE *xorArray;
	BYTE *andArray;
	int i, j, k;
	int XORSize, ANDSize;
	int icoIndex;
	int zincIndex;

	// Display version.
	printf("%s Version %s Copyright (C) 1991-1993 Zinc Software Incorporated.\n", exeName, VERSION);

	// Check for command line arguments.
	if (argc < 2)
	{
		printf("Usage: %s <file[.ICO]> [file[.DAT]] [resource name]\n", exeName);
		return (1);
	}

	// Set up .ICO file name.
	char fileName[128];
	strcpy(fileName, argv[1]);
	UI_STORAGE::ChangeExtension(fileName, ".ico");
	if (!UI_STORAGE::ValidName(fileName, FALSE) || 
		(fileHandle = open(fileName, O_RDONLY | O_BINARY)) == -1)
	{
		printf("Error opening the icon file < %s >.\n", fileName);
		return (1);
	}

	// Check icon file header.
	read(fileHandle, &header, sizeof(header));
	if (header.icoReserved != 0 || header.icoResourceType != 1)
	{
		printf("File < %s > is not a valid icon file.\n", fileName);
		return (1);
	}

	// Set up .DAT file name.
	char dataFileName[128];
	if (argc < 3)
		strcpy(dataFileName, fileName);
	else
		strcpy(dataFileName, argv[2]);
	UI_STORAGE::ChangeExtension(dataFileName, ".dat");

	// Set up resource name of icon.
	char resourceName[128];
	if (argc < 4)
		resourceName[0] = '\0';
	else
		strcpy(resourceName, argv[3]);

	// Read each icon resource.
	for (i = 0; i < header.icoResourceCount; i++)
	{
		// Find and read next resource descriptor.
		lseek(fileHandle, sizeof(header) + sizeof(descriptor) * i, SEEK_SET);
		read(fileHandle, &descriptor, sizeof(descriptor));

		if (descriptor.colorCount == 16 && descriptor.width == 32 && descriptor.height == 32)
		{
			lseek(fileHandle, descriptor.icoDIBOffset + sizeof(BITMAPINFOHEADER), SEEK_SET);

			// Match up the palette maps.
			for (j = 0; j < descriptor.colorCount; j++)
			{
				read(fileHandle, &rgbColor, sizeof(RGBQUAD));
				for (k = 0; k < descriptor.colorCount; k++)
					if (zincRGBValues[k].rgbRed == rgbColor.rgbRed &&
						zincRGBValues[k].rgbGreen == rgbColor.rgbGreen &&
						zincRGBValues[k].rgbBlue == rgbColor.rgbBlue)
					{
						zincRGBIndex[j] = k;
						break;
					}
			}

			// Read the ICO format arrays from disk.
			XORSize = 512;
			ANDSize = 128;

			xorArray = new BYTE [XORSize];
			zincBitmap = new UCHAR [descriptor.width * descriptor.height];
			read(fileHandle, xorArray, XORSize);
			andArray = new BYTE [ANDSize];
			read(fileHandle, andArray, ANDSize);

			// Convert the ICO format array to Zinc format icon.
			zincIndex = 0;
			for (j = 31; j >= 0; j--)
			{
				for (k = 0; k < 16; k++)
				{
					// Map the corresponding color into the bitmap array.
					icoIndex = j * 16 + k;
					zincBitmap[zincIndex++] = zincRGBIndex[(xorArray[icoIndex] >> 4) & 0xF];
					zincBitmap[zincIndex++] = zincRGBIndex[xorArray[icoIndex] & 0xF];
					
					// If the pixel is transparent (AND array) then set to white.
					if (andArray[(j * 4 + k / 4)] & (0x80 >> ((k % 4) * 2)))
						zincBitmap[zincIndex - 2] = 15;
					if (andArray[(j * 4 + k / 4)] & (0x80 >> ((k % 4) * 2 + 1)))
						zincBitmap[zincIndex - 1] = 15;
				}
			}

			// Create storage class.
			UI_STORAGE *storage = new UI_STORAGE(dataFileName, UIS_OPENCREATE | UIS_READWRITE);
			if (storage->storageError)
			{
				printf("Error opening the data file < %s >.\n", dataFileName);
				return (1);
			}
			else
			{
				storage->MkDir("UIW_WINDOW");
				storage->MkDir("UI_BITMAP");
				storage->MkDir("UI_ICON");
				storage->MkDir("UI_HELP");
				storage->MkDir("UI_HPP");
				storage->storageError = 0;
				storage->ChDir("UI_ICON");
			}

			// Get resource name from icon file name if not explicitly given.
			if (resourceName[0] == '\0')
			{
				UI_STORAGE::ChangeExtension(fileName, "");
				char pathName[MAXPATHLEN];
//				strcpy(resourceName, fileName);								// BUG.2125
				UI_STORAGE::StripFullPath(fileName, pathName, resourceName);
				for (int i = 1; storage->FindFirstObject(resourceName); i++)
					sprintf(resourceName, "%s_%d", fileName, i);
			}
			strupr(resourceName);

			UI_STORAGE_OBJECT iconImage(*storage, resourceName, ID_ICON_IMAGE, UIS_CREATE | UIS_READWRITE);
			if (!iconImage.objectError)
			{
				iconImage.Store((USHORT)descriptor.width);
				iconImage.Store((USHORT)descriptor.height);
				iconImage.Store(zincBitmap, descriptor.width, descriptor.height);
			}
			storage->Save(0);

			// Clean up.
			delete storage;
			UI_STORAGE::ChangeExtension(fileName, ".ico");
			printf("File < %s > has been converted to < %s~%s >.\n", fileName,
				dataFileName, resourceName);
			break;
		}
	}

	// Report if not a 32x32 icon.
	if (i >= header.icoResourceCount)
		printf("The icon < %s > is not a 32x32 pixel, 16 color icon.\n", fileName);

	close(fileHandle);
 	return (0);
}

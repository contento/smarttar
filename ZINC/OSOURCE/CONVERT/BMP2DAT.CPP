//	Zinc Interface Library Designer - Z_IMAGE2.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

// Current limitations:
// 1 - Only 16 colors.
// 2 - No RLE

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

#if !defined(MAX_PATH)
#define MAX_PATH	128
#endif

#define VERSION	"3.50"

#if defined(ZIL_MSDOS) || defined(ZIL_OS2)
#define BI_RGB	0
#define BYTE	UCHAR
#define WORD	USHORT
#define DWORD	ULONG

struct RGBQUAD
{
	BYTE	rgbBlue;
	BYTE	rgbGreen;
	BYTE	rgbRed;
	BYTE	rgbReserved;
};

struct BITMAPFILEHEADER
{
    WORD	bfType;
    DWORD	bfSize;
    WORD	bfReserved1;
    WORD	bfReserved2;
    DWORD	offBits;
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

struct BITMAPINFO
{
  	BITMAPINFOHEADER	bmiHeader;
	RGBQUAD				bmiColors[1];
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

UCHAR GetNearestZincColor(RGBQUAD rgbColor)
{
    for (UCHAR colorIndex = 0; colorIndex < 16 &&
        (zincRGBValues[colorIndex].rgbRed != rgbColor.rgbRed ||
		zincRGBValues[colorIndex].rgbGreen != rgbColor.rgbGreen ||
		zincRGBValues[colorIndex].rgbBlue != rgbColor.rgbBlue);
		colorIndex++)
    ;
    if (colorIndex == 16)
        colorIndex = 0xFF;
    return colorIndex;
}

#define		BITMAP_OK					0
#define		BITMAP_INVALID_FILE			1
#define		BITMAP_INVALID_FILE_TYPE	2
#define		BITMAP_TOO_LARGE			3
#define		BITMAP_COMPRESSED			4

static int Bmp2Dat(char *fileName, USHORT *_width, USHORT *_height, UCHAR **_bitmapArray)
{
	BITMAPFILEHEADER    bmiFileHeader;
   	BITMAPINFOHEADER    bmiInfoHeader;
   	RGBQUAD             *bmiColors;

	int fileHandle;
	if ((fileHandle = open(fileName, O_RDONLY | O_BINARY)) == -1)
		return BITMAP_INVALID_FILE;

	// Check the bitmap file header.
	read(fileHandle, &bmiFileHeader, sizeof(bmiFileHeader));
   	if (bmiFileHeader.bfType != *(WORD *)"BM")
		return BITMAP_INVALID_FILE_TYPE;

	read(fileHandle, &bmiInfoHeader, sizeof(bmiInfoHeader));
   	DWORD colors = bmiInfoHeader.biClrUsed;
   	if (colors == 0 && bmiInfoHeader.biBitCount < 24)
   	{
       	colors = 2;
       	for (int exp = 1; exp < bmiInfoHeader.biBitCount; exp++)
           	colors *= 2;
   	}

   	bmiColors = new RGBQUAD[(int)colors];
	read(fileHandle, bmiColors, (int)colors * sizeof(RGBQUAD));
   	UCHAR *zincPalette = new UCHAR[(int)colors];
   	for (int colorIndex = 0; colorIndex < colors; colorIndex++)
   	{
       	UCHAR matchColor = GetNearestZincColor(bmiColors[colorIndex]);
       	zincPalette[colorIndex] = matchColor;
   	}
	delete bmiColors;

   	unsigned width = (unsigned)bmiInfoHeader.biWidth;
   	unsigned height = (unsigned)bmiInfoHeader.biHeight;

	if ((long)width * (long)height > 0x7FFF)
		return BITMAP_TOO_LARGE;

	*_width = width;
	*_height = height;

   	UCHAR *zincBitmap = new UCHAR[(unsigned)width * (unsigned)height];

	//
	UCHAR startMask = ~(0xFF / colors);
	int tempColors = (int)colors / 2;
	int shiftValue = 0;
	while (tempColors)
	{
		shiftValue++;
		tempColors /= 2;
	}

   	if (bmiInfoHeader.biCompression == BI_RGB)
   	{
		UCHAR mask = startMask;
		UCHAR colorIndex;
		read(fileHandle, &colorIndex, sizeof(colorIndex));
		int shift= 8 - shiftValue;
		for (int y = height - 1; y >= 0; y--)
			for (int x = 0; x < width; x++)
			{
				zincBitmap[y * width + x] =
					zincPalette[(colorIndex & mask) >> shift];
				mask = mask >> shiftValue;
				shift -= shiftValue;
				if (mask == 0)
				{
					read(fileHandle, &colorIndex, sizeof(colorIndex));
					mask = startMask;
					shift = 8 - shiftValue;
				}
			}
		delete zincPalette;
   	}
	else
	{
		delete zincPalette;
		close(fileHandle);
		return BITMAP_COMPRESSED;
	}
	*_bitmapArray = zincBitmap;
	close(fileHandle);
	return BITMAP_OK;
}

int main(int argv, char *argc[])
{
#if defined(ZIL_MSDOS)
	char *exeName = "bmp2dat";
#elif defined(ZIL_MSWINDOWS)
	char *exeName = "wbmp2dat";
#elif defined(ZIL_OS2)
	char *exeName = "obmp2dat";
#elif defined(ZIL_MOTIF)
	char *exeName = "bmp2dat";
#endif

	// Display version.
	printf("%s Version %s Copyright (C) 1991-1993 Zinc Software Incorporated.\n", exeName, VERSION);

	if (argv < 3)
	{
		printf("Usage:  %s <filename[.bmp]> <filename[.dat]> [imagename]\n", exeName);
		return 1;
	}

	char sourcePath[MAX_PATH];
	strcpy(sourcePath, argc[1]);
	if (!strchr(sourcePath, '.'))
		strcat(sourcePath, ".bmp");

	char destPath[MAX_PATH];
	strcpy(destPath, argc[2]);
	if (!strchr(destPath, '.'))
		strcat(destPath, ".dat");

	char imageName[64];
	if (argv >= 4)
		strcpy(imageName, argc[3]);
	else
	{
		UI_STORAGE::StripFullPath(sourcePath, NULL, imageName);
		*strchr(imageName, '.') = '\0';
	}
	ui_strupr(imageName);
	
	USHORT colorWidth, colorHeight, monoWidth, monoHeight;
	UCHAR *colorArray = NULL, *monoArray = NULL;
	int convertError = Bmp2Dat(sourcePath, &colorWidth, &colorHeight, &colorArray);
	if (convertError)
	{
		char *errString = "Error converting filer: %s";
		switch (convertError)
		{
		case BITMAP_INVALID_FILE:
			errString = "Error opening file: %s";
			break;

		case BITMAP_INVALID_FILE_TYPE:
			errString = "\"%s\" is not a Windows bitmap file.";
			break;

		case BITMAP_TOO_LARGE:
			errString = "\"%s\" is larger than 32K.";
			break;

		case BITMAP_COMPRESSED:
			errString = "Cannot uncompress file: %s";
			break;
		}
		printf(errString, sourcePath);
		return convertError;
	}

	char monoPath[MAX_PATH];
	char fileName[MAX_PATH];
	UI_STORAGE::StripFullPath(sourcePath, monoPath, fileName);
	strcat(monoPath, "_");
	strcat(monoPath, fileName);

	if (!Bmp2Dat(monoPath, &monoWidth, &monoHeight, &monoArray) &&
		monoWidth == colorWidth && monoHeight == colorHeight)
	{
		for (int x = 0; x < colorWidth; x++)
			for (int y = 0; y < colorHeight; y++)
				if (monoArray[y * colorWidth + x])
					colorArray[y * colorWidth + x] = 0xFF;
		delete monoArray;
	}

	UI_STORAGE *storage = new UI_STORAGE(destPath, UIS_READWRITE);
	if (storage->storageError)
	{
		delete storage;
		storage = new UI_STORAGE(destPath, UIS_OPENCREATE);
		if (storage->storageError)
		{
			printf("\nError (%d) opening output file: %s", storage->storageError, destPath);
			return 1;
		}
		storage->MkDir("UIW_WINDOW");
		storage->MkDir("UI_BITMAP");
		storage->MkDir("UI_ICON");
		storage->MkDir("UI_HELP");
	}

	storage->ChDir("~UI_BITMAP");
	{
	UI_STORAGE_OBJECT sObject(*storage, imageName, 0, UIS_READWRITE | UIS_CREATE);
	sObject.Store(colorWidth);
	sObject.Store(colorHeight);
	sObject.Store(colorArray, colorWidth, colorHeight);
	}
	delete colorArray;

	storage->Save();
	delete storage;

 	return 0;
}

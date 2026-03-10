//	Zinc Interface Library Designer - Z_IMAGE2.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

// Current limitations:
// 1 - Only 16 colors.
// 2 - Only 32x32 icons

#include <io.h>
#include <fcntl.h>
//#include <sys\stat.h>
#include <stdio.h>
#include <string.h>
#include "ui_dsn.hpp"

#if defined(ZIL_MSDOS) || defined(ZIL_OS2) || defined(__DVX__)
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

static int Bmp2Dat(char *fileName, int *_width, int *_height, UCHAR **_bitmapArray)
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
//				if (mask == 0)												// BUG.1615
				if (mask == 0 || (x == width - 1 && width % 2))
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

int IMAGE_EDITOR::Import(char *imagePath)
{
	char *extention = strchr(imagePath, '.');
	ui_strupr(extention);
	if (!strcmp(extention, ".ICO"))
	{
		ICO_HEADER header;
		ICO_DESCRIPTOR descriptor;
		RGBQUAD rgbColor;
		UCHAR zincRGBIndex[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
		UCHAR *zincBitmap;
	
		int fileHandle;
		BYTE *xorArray;
		BYTE *andArray;
		int j, k;
		int XORSize, ANDSize;
		int icoIndex;
		int zincIndex;


		// Set up .ICO file name.
		char fileName[128];
		strcpy(fileName, imagePath);
		if ((fileHandle = open(fileName, O_RDONLY | O_BINARY)) == -1)
		{
			errorSystem->ReportError(windowManager,	WOS_NO_STATUS,
				"Error opening File: %s", fileName);
			return FALSE;
		}

		// Check icon file header.
		read(fileHandle, &header, sizeof(header));
		if (header.icoReserved != 0 || header.icoResourceType != 1)
		{
			errorSystem->ReportError(windowManager,	WOS_NO_STATUS,
				"\"%s\" is not a Windows icon file.", fileName);
			return FALSE;
		}

		// Read first icon descriptor.
		lseek(fileHandle, sizeof(header), SEEK_SET);
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

			imageWidth = editBitmap->width = viewBitmap->width = descriptor.width;
			imageHeight = editBitmap->height = viewBitmap->height = descriptor.height;
			if (imageArray)
				delete imageArray;
			if (undoArray)
				delete undoArray;
			imageArray = editBitmap->bitmapArray = viewBitmap->bitmapArray = zincBitmap;
			undoArray = new UCHAR[imageHeight * imageWidth];
			UIW_WINDOW::Event(S_SIZE);
			UIW_WINDOW::Event(S_REDISPLAY);
		}
		else
		{
			errorSystem->ReportError(windowManager,	WOS_NO_STATUS,
				"File \"%s\" is not of a compatible format.", fileName);
			return FALSE;
		}
		close(fileHandle);
	}
	else if (!strcmp(extention, ".BMP"))
	{
		int colorWidth, colorHeight, monoWidth, monoHeight;
		UCHAR *colorArray = NULL, *monoArray = NULL;

		char *errString = NULL;
		switch (Bmp2Dat(imagePath, &colorWidth, &colorHeight, &colorArray))
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
		if (errString)
	 	{
			errorSystem->ReportError(windowManager,	WOS_NO_STATUS,
				errString, imagePath);
			return FALSE;
		}

		char monoPath[MAX_PATH];
		char fileName[MAX_PATH];
		UI_STORAGE::StripFullPath(imagePath, monoPath, fileName);
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
		imageWidth = editBitmap->width = viewBitmap->width = colorWidth;
		imageHeight = editBitmap->height = viewBitmap->height = colorHeight;
		if (imageArray)
			delete imageArray;
		if (undoArray)
			delete undoArray;
		editBitmap->bitmapArray = viewBitmap->bitmapArray = imageArray = colorArray;
		undoArray = new UCHAR[imageWidth * imageHeight];
		UIW_WINDOW::Event(S_SIZE);
		UIW_WINDOW::Event(S_REDISPLAY);
 		return TRUE;
	}
	else
		return FALSE;
	return TRUE;
}

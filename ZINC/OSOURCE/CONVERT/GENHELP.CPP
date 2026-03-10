//	Zinc Interface Library help generator - GENHELP.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include <stdio.h>
#include <string.h>
#include <ui_gen.hpp>
#if defined(_MSC_VER)
#	pragma hdrstop					// Microsoft pre-compiled header pragma.
#elif defined(ZIL_POSIX)
#	include <unistd.h>
#endif

#define VERSION	"3.50"
#define PrintStatus printf

static void WriteHelpConstant(UI_STORAGE_OBJECT *element, const char *helpName,
	OBJECTID helpID, char *title)
{
	// Write out the help constant.
	char buffer[256];
	sprintf(buffer, "const UI_HELP_CONTEXT %-32s = 0x%04X; // %s", helpName, helpID, title);
	element->Store(buffer);
}

static void WriteHelpMessage(FILE *textFile, UI_STORAGE_OBJECT *element,
	long beginOffset, long endOffset)
{
	// Write out the help message.
	long originalOffset = ftell(textFile);
	if (endOffset != beginOffset)
	{
		short bufferSize = (short)(endOffset - beginOffset);
		char *buffer = new char[bufferSize];
		fseek(textFile, beginOffset, SEEK_SET);
		fread(buffer, bufferSize, 1, textFile);
		for (short i = 0; i < bufferSize; )
			if (buffer[i] == '\\' && buffer[i+1] == '\r')
			{
				bufferSize -= 1;
				memcpy(&buffer[i], &buffer[i+1], bufferSize - i);
				i += 2;
			}
			else if (buffer[i] == '\\' && buffer[i+1] == '\n')
			{
				bufferSize -= 1;
				memcpy(&buffer[i], &buffer[i+1], bufferSize - i);
				i += 1;
			}
			else if (buffer[i] == '\r')
			{
				if (buffer[i+2] != '\r')
				{
					if (buffer[i-1] != ' ')
					{
						buffer[i++] = ' ';
						bufferSize -= 1;
						memcpy(&buffer[i], &buffer[i+1], bufferSize - i);
					}
					else
					{
						bufferSize -= 2;
						memcpy(&buffer[i], &buffer[i+2], bufferSize - i);
					}
				}

				// Check for blank line.
				while (buffer[i] == '\r' || buffer[i] == '\n')
					i++;
			}
			else if (buffer[i] == '\n')
			{
				if (buffer[i+1] != '\n')
				{
					if (buffer[i-1] != ' ')
						buffer[i] = ' ';
					else
					{
						bufferSize -= 1;
						memcpy(&buffer[i], &buffer[i+1], bufferSize - i);
					}
				}

				// Check for blank line.
				while (buffer[i] == '\n')
					i++;
			}
			else
				i++;
		element->Store(bufferSize);
		if (bufferSize)
			element->Store(buffer, sizeof(char), bufferSize);
		delete buffer;
	}
	else
	{
		short bufferSize = 0;
		element->Store(bufferSize);
	}
	fseek(textFile, originalOffset, SEEK_SET);
}

#if !defined(ZIL_MSWINDOWS)
#define PrintStatus printf
#endif

void Z_GenerateHeaderFile(UI_STORAGE *directory, int printStatus)
{
	// Make sure there is an UI_HPP directory.
	if (directory->ChDir("~UI_HPP"))
		return;

	// Generate the HPP file.
	char line[256], fileName[256];
	directory->StorageName(fileName);
	UI_STORAGE::ChangeExtension(fileName, ".hpp");
#if !defined(ZIL_MSWINDOWS)
	if (printStatus)
		PrintStatus("GENERATING %s:\n", fileName);
#endif
	FILE *includeFile = fopen(fileName, "w+");
	for (char *entry = directory->FindFirstObject("*"); entry; entry = directory->FindNextObject())
		if (strcmp(entry, ".") && strcmp(entry, ".."))
		{
#if !defined(ZIL_MSWINDOWS)
			if (printStatus)
				PrintStatus("  USE_%s\n", entry);
#endif
			fprintf(includeFile, "#ifdef USE_%s\n", entry);
			UI_STORAGE_OBJECT hppEntry(*directory, entry, 0, UIS_READ);
			for (hppEntry.Load(line, 256); strcmp(line, ""); hppEntry.Load(line, 256))
				fprintf(includeFile, "%s\n", line);
			fprintf(includeFile, "#endif // USE_%s\n\n", entry);
		}
	fclose(includeFile);
}

main(int argc, char *argv[])
{
#if defined(ZIL_MSDOS)
	char *exeName = "genhelp";
#elif defined(ZIL_MSWINDOWS)
	char *exeName = "wgenhelp";
#elif defined(ZIL_OS2)
	char *exeName = "ogenhelp";
#elif defined(ZIL_MOTIF)
	char *exeName = "genhelp";
#endif

	// Display version.
	printf("%s Version %s Copyright (C) 1991-1993 Zinc Software Incorporated.\n", exeName, VERSION);

	// Make sure there is a specified text file.
	if (argc < 2)
	{
		printf("Usage: %s <file[.TXT]> [file[.DAT]]\n", exeName);
		return (1);
	}

	// Open the text file.
	char textFileName[128];
	char storageFileName[128];
	strcpy(textFileName, argv[1]);
	UI_STORAGE::ChangeExtension(textFileName, ".txt");
	FILE *textFile = fopen(textFileName, "rb");
	if (!textFile)
	{
		printf("Error opening the text file < %s >.\n", textFileName);
		return (1);
	}

	// Open the data file.
	if (argc > 2 && argv[2][0] != '/')
		strcpy(storageFileName, argv[2]);
	else
		strcpy(storageFileName, argv[1]);
	UI_STORAGE::ChangeExtension(storageFileName, ".dat");
	UI_STORAGE *helpFile = new UI_STORAGE(storageFileName, UIS_OPENCREATE | UIS_READWRITE);
	if (helpFile->storageError)
	{
		delete helpFile;
		printf("Error opening/creating the data file < %s >.\n", storageFileName);
		return (1);
	}

	// Create the help directory.
	helpFile->MkDir("UI_HPP");
	helpFile->MkDir("UI_HELP");

	// Print genhelp status.
	PrintStatus("PROCESSING %s to %s:\n", textFileName, storageFileName);


	OBJECTID newHelpID = 0;
	helpFile->ChDir("~UI_HELP");
	for (char *objectName = helpFile->FindFirstObject("*"); objectName;
		objectName = helpFile->FindNextObject())
	{
		UI_STORAGE_OBJECT sObject(*helpFile, objectName, 0,	UIS_READ);
		if (sObject.objectID >= newHelpID)
	 		newHelpID = sObject.objectID + 1;
	}

	// Generate the HPP directory.
	helpFile->ChDir("~UI_HPP");
	UI_STORAGE_OBJECT *hppElement = new UI_STORAGE_OBJECT(*helpFile, "HELP_CONTEXTS", ID_HELP_CONTEXT, UIS_OPENCREATE | UIS_READWRITE);

	// Generate the help contexts.
	helpFile->ChDir("~UI_HELP");
	char line[256], title[256];
	long beginOffset = 0L;
	long endOffset = 0L;
	UI_STORAGE_OBJECT *helpElement = NULL;
	while (textFile && fgets(line, sizeof(line), textFile))
	{
		char helpName[64];
//		if (sscanf(line, "--- %[^\r]", helpName) == 1)						// BUG.1961
		if (sscanf(line, "--- %[^\r\n]", helpName) == 1)
		{
			// Save the previous help message.
			if (helpElement)
			{
				WriteHelpMessage(textFile, helpElement, beginOffset, endOffset);
				delete helpElement;
			}

			OBJECTID helpID;
			if (helpFile->FindFirstObject(helpName))
			{
				UI_STORAGE_OBJECT sObject(*helpFile, helpName, 0, UIS_READ);
				helpID = sObject.objectID;
			}
			else
				helpID = newHelpID++;

			// Get the new help storage element.
			helpElement = new UI_STORAGE_OBJECT(*helpFile, helpName, helpID, UIS_CREATE | UIS_READWRITE);

			// Get and save the new help title.
			if (!fgets(title, sizeof(title), textFile))
				title[0] = '\0';
			ui_strrepc(title, '\r', '\0');
			ui_strrepc(title, '\n', '\0');
			helpElement->Store(title);

			// Print genhelp status.
			PrintStatus("  %s\n", title);

			// Print the HPP information.
			WriteHelpConstant(hppElement, helpName, helpID, title);

			// Get the starting position of the help message.
			beginOffset = endOffset = ftell(textFile);
		}
		else
			endOffset = ftell(textFile);
	}

	// Save the last help message.
	if (helpElement)
	{
		endOffset = ftell(textFile);
		WriteHelpMessage(textFile, helpElement, beginOffset, endOffset);
		delete helpElement;
	}
	if (textFile)
		fclose(textFile);
	hppElement->Store("");	// End of HPP string entries.
	delete hppElement;
	helpFile->Save();

	// Generate the hpp file.
	Z_GenerateHeaderFile(helpFile, TRUE);

	// Clean up the file.
	PrintStatus("DONE!\n");
	delete helpFile;
	return (0);
}

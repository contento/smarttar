//	PERIODIC.CPP (PERIODIC) - Periodic table of the elements example program.
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA
//  May be freely copied, used and distributed.

#include <stdio.h>
#include <ui_win.hpp>

// The information file "elements" really contains fixed length records.
// The elements are listed in order of atomic number, full name first
// (in a 16 character, space filled field), followed by the atomic weight
// (in a 7 character, space filled field).  The file is formatted as a
// text file, so each operating system has its own representation of 
// the end-of-line.

#if defined(ZIL_POSIX)
#	define EOL_LENGTH	1
#	include <unistd.h>
#elif defined(ZIL_MSDOS) || defined(ZIL_MSWINDOWS) || defined(ZIL_OS2) || defined(__DVX__)
#	define EOL_LENGTH	2
#endif

#define ELEMENT_NAME_LENGTH		16
#define ATOMIC_WEIGHT_LENGTH	7

#define FILENAME		"elements"

char *elementFunction = "Elements";

int ReadElementInfo(EVENT_TYPE atomicNumber, char *elementNameString, char *atomicWeightString)
{
	FILE *file = fopen(FILENAME, "rb");
	if (!file)
		return (-1);

	long offset = (ELEMENT_NAME_LENGTH + ATOMIC_WEIGHT_LENGTH + EOL_LENGTH) * atomicNumber;
	fseek(file, offset, SEEK_SET);
	fread(elementNameString, sizeof(char), ELEMENT_NAME_LENGTH, file);
	fread(atomicWeightString, sizeof(char), ATOMIC_WEIGHT_LENGTH, file);
	elementNameString[ELEMENT_NAME_LENGTH - 1] = '\0';
	atomicWeightString[ATOMIC_WEIGHT_LENGTH - 1] = '\0';
	if (file)
		fclose(file);

	return (0);
}

EVENT_TYPE Elements(UI_WINDOW_OBJECT *object, UI_EVENT &, EVENT_TYPE ccode)
{
	if (ccode != L_SELECT)
		return (ccode);

	// Get the string object pointers.
	UIW_STRING *elementName = (UIW_STRING *)object->parent->Information(GET_STRINGID_OBJECT, "ELEMENT_NAME");
	UIW_INTEGER *atomicNumber = (UIW_INTEGER *)object->parent->Information(GET_STRINGID_OBJECT, "ATOMIC_NUMBER");
	UIW_STRING *atomicWeight = (UIW_STRING *)object->parent->Information(GET_STRINGID_OBJECT, "ATOMIC_WEIGHT");

	// Read element name and weight.
	char elementNameString[24];
	char atomicWeightString[12];
	if (ReadElementInfo(((UIW_BUTTON *)object)->value, elementNameString,
		atomicWeightString) != 0)
		return (ccode);

	// Update the strings.
	elementName->DataSet(elementNameString);
	atomicNumber->DataSet((int *)&((UIW_BUTTON *)object)->value);
	atomicWeight->DataSet(atomicWeightString);

	return (ccode);
}

int UI_APPLICATION::Main(void)
{
	// The UI_APPLICATION constructor automatically initializes the 
	// display, eventManager, and windowManager variables.

	// This line fixes linkers that don't look for main in the .LIBs.
	UI_APPLICATION::LinkMain();

	// Load the window and set current item.
	UIW_WINDOW *window = (UIW_WINDOW *)UI_WINDOW_OBJECT::New("per_win.dat~PERIODIC_TABLE");
	*window +
		(UI_WINDOW_OBJECT *)window->Information(GET_STRINGID_OBJECT, "ZN");

	// Add the window to the window manager.
	*windowManager + window;

	// Process user responses.
	UI_APPLICATION::Control();

	return (0);
}

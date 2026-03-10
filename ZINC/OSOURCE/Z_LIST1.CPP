//	Zinc Interface Library - Z_LIST1.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "ui_gen.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

// ----- UI_LIST_BLOCK ------------------------------------------------------

UI_LIST_BLOCK::UI_LIST_BLOCK(int _noOfElements,
	int (*_compareFunction)(void *element1, void *element2)) :
	UI_LIST(_compareFunction), noOfElements(_noOfElements),
	elementArray(NULL), freeList()
{
	// No other list block information is initialized.
}

UI_LIST_BLOCK::~UI_LIST_BLOCK(void)
{
	// Establish null lists so it doesn't free up an unknown block.
	first = last = NULL;
	freeList.first = freeList.last = NULL;
}

UI_ELEMENT *UI_LIST_BLOCK::Add(UI_ELEMENT *element)
{
	UI_ELEMENT *newElement = freeList.first;

	// Add a new list element to the use list at specified location.
	if (newElement)
	{
		freeList.Subtract(newElement);
		UI_LIST::Add(element, newElement);
	}

	// Return a pointer to the new element.
	return (newElement);
}

UI_ELEMENT *UI_LIST_BLOCK::Add(void)
{
	UI_ELEMENT *newElement = freeList.first;

	// Add a new list element to the use list according to compareFunction.
	if (newElement)
	{
		freeList.Subtract(newElement);
		UI_LIST::Add(newElement);
	}

	// Return a pointer to the new element.
	return (newElement);
}

UI_ELEMENT *UI_LIST_BLOCK::Subtract(UI_ELEMENT *element)
{
	UI_ELEMENT *nextElement = element->Next();

	// Delete the element from the use list.
	UI_LIST::Subtract(element);
	freeList.Add(NULLP(UI_ELEMENT), element);

	// Return the next element.
	return (nextElement);
}

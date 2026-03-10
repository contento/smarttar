//	LIST.CPP (WORD) - List and element classes. (from UI_GEN.HPP)
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA
//  May be freely copied, used and distributed.

#include <ui_env.hpp>
#include "list.hpp"

UI_ELEMENT *UI_LIST::Add(UI_ELEMENT *newElement)
{
	// See if it is a default insertion.
	if (!first || !compareFunction)
		return (UI_LIST::Add(NULL, newElement));

	// Get the beginning positions.
	int weight;
	UI_ELEMENT *element1, *element2;
	UI_ELEMENT *beginElement = first;
	UI_ELEMENT *endElement = last;

	// Shortcuts for elements in special orders.
	if ((*compareFunction)(endElement, newElement) <= 0)
		beginElement = endElement;
	else if ((*compareFunction)(beginElement, newElement) >= 0)
		beginElement = endElement = NULL;

	// Find the proper element position.
	while (beginElement != endElement)
	{
		// Get the middle element - The order here is important!.
		element1 = beginElement;
		for (element2 = endElement; element1 != element2;
			element2 = element2->previous)
			if (element1->next != element2)
				element1 = element1->next;

		// Increment the proper element position.
		weight = (*compareFunction)(element1, newElement);
		if (weight > 0 && endElement != element1)
			endElement = element1;
		else if (weight < 0 && beginElement != element1)
			beginElement = element1;
		else
			beginElement = endElement = element1;
	}

	// Place the element in the sorted list.
	if (endElement)					// Insert after endElement.
	{
		newElement->previous = endElement;
		newElement->next = endElement->next;
		if (newElement->next)
			newElement->next->previous = newElement;
		endElement->next = newElement;
		if (last == endElement)
			last = newElement;
	}
	else							// Insert at beginning of the list.
	{
		newElement->previous = NULL;
		newElement->next = first;
		first->previous = newElement;
		first = newElement;
	}

	// Return a pointer to the element.
	return (newElement);
}

UI_ELEMENT *UI_LIST::Add(UI_ELEMENT *element, UI_ELEMENT *newElement)
{
	// Add the element to the list.
	if (!first)
	{
		newElement->previous = newElement->next = NULL;
		first = last = newElement;
	}
	else if (!element)
	{
		newElement->previous = last;
		newElement->next = NULL;
		last->next = newElement;
		last = newElement;
	}
	else
	{
		newElement->previous = element->previous;
		newElement->next = element;
		if (!element->previous)
			first = newElement;
		else
			element->previous->next = newElement;
		element->previous = newElement;
	}

	// Return a pointer to the element.
	return (newElement);
}

int UI_LIST::Count(void)
{
	int count = 0;

	// Get the element index.
	for (UI_ELEMENT *element = first; element; element = element->next)
		count++;

	// Return the count.
	return (count);
}

UI_ELEMENT *UI_LIST::Subtract(UI_ELEMENT *element)
{
	if (!element)
		return(NULL);

	// Make sure the element is in the list.
	UI_ELEMENT *tElement = element;
	while (tElement->previous)
		tElement = tElement->previous;
	if (tElement != first)
		return (element);

	// Delete the specified element from the list.
	if (element->previous)
		element->previous->next = element->next;
	else
		first = element->next;
	if (element->next)
		element->next->previous = element->previous;
	else
		last = element->previous;
	if (current == element)
		current = NULL;

	// Return the next element.
	tElement = element->next;
	element->next = element->previous = NULL;
	return (tElement);
}

void UI_LIST::Destroy(void)
{
	UI_ELEMENT *tElement;

	// Delete all the elements in the list.
	for (UI_ELEMENT *element = first; element; )
	{
		tElement = element;
		element = element->next;
		delete tElement;
	}
	first = last = current = NULL;
}

UI_ELEMENT *UI_LIST::Get(int index)
{
	// Get the list element.
	for (UI_ELEMENT *element = first; index > 0 && element; element = element->next)
		index--;

	// Return the element.
	return (element);
}

UI_ELEMENT *UI_LIST::Get(int (*findFunction)(void *element1, void *matchData), void *matchData)
{
	// See if the current element matches.
	if (current && !(*findFunction)(current, matchData))
		return (current);

	// Try to find the list element.
	for (UI_ELEMENT *element = first;
		element && (*findFunction)(element, matchData);
		element = element->next)
		;

	// Return the element.
	return (element);
}

int UI_LIST::Index(const UI_ELEMENT *element)
{
	// Make sure there is an element.
	if (!element)
		return (-1);

	// Get the element index.
	int index = 0;
	for (UI_ELEMENT *tElement = first; tElement && tElement != element; 
		tElement = tElement->next)
		index++;

	// Return the index.
	if (tElement)
		return (index);
	return (-1);
}

void UI_LIST::Sort(void)
{
	// Make sure there are elements in the list and a comparison routine.
	if (!first || !compareFunction)
		return;

	// Start the list with the first element.
	UI_ELEMENT *element;
	UI_ELEMENT *addElement = first;
	first = last = NULL;

	// Sort the list of elements using the flat binary insertion.
	while (addElement)
	{
		element = addElement->next;
		UI_LIST::Add(addElement);
		addElement = element;
	}
}

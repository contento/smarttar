//	MEMWIN.CPP (MEMORY_WINDOW) - Example of a derived device.
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA
#include <alloc.h>
#include <stdio.h>
#include <ui_win.hpp>

const int E_MEMORY_WINDOW = 10000;
const EVENT_TYPE SAVE_FREE_MEMORY = 10001;

class MEMORY_WINDOW : public UI_DEVICE, public UIW_WINDOW
{
public:
	UI_DEVICE *device;
	UIW_WINDOW *window;

	MEMORY_WINDOW(int left, int top);
	~MEMORY_WINDOW(void) {}

	static EVENT_TYPE Save(UI_WINDOW_OBJECT *object, UI_EVENT &, EVENT_TYPE ccode);

private:
	UI_BIGNUM farCoreMemory;
	UI_BIGNUM freeMemory;
	UIW_BIGNUM *farCoreLeftField;
	UIW_BIGNUM *freeMemoryField;
	UIW_BIGNUM *saveField;

	EVENT_TYPE Event(const UI_EVENT &event);
	void Poll(void);
};

long FarCoreLeft()
{
	return (farcoreleft());
}

long MemoryFree()
{
#if !defined(DOSX286)
	if (heapcheck() != _HEAPOK)
		return (-1L);
#endif

	long freeMem = FarCoreLeft();

#if !defined(DOSX286)
	struct heapinfo heapInfo;
	heapInfo.ptr = 0;
	while (heapwalk(&heapInfo) == _HEAPOK)
	{
		if (!heapInfo.in_use)
			freeMem += heapInfo.size;
	}
#endif
	return (freeMem);
}

MEMORY_WINDOW::MEMORY_WINDOW(int left, int top):
	UI_DEVICE(E_MEMORY_WINDOW, D_ON),
	UIW_WINDOW(left, top, 32, 5, WOF_NO_FLAGS, WOAF_NO_SIZE | WOAF_NO_DESTROY)
{
	// Setup the values.
	farCoreLeftField = new UIW_BIGNUM(13, 0, 15, &freeMemory, "", NMF_DECIMAL(0) | NMF_COMMAS,
		WOF_JUSTIFY_RIGHT | WOF_NON_SELECTABLE | WOF_NO_ALLOCATE_DATA);

	freeMemoryField = new UIW_BIGNUM(13, 1, 15, &farCoreMemory, "", NMF_DECIMAL(0) | NMF_COMMAS,
		WOF_JUSTIFY_RIGHT | WOF_NON_SELECTABLE | WOF_NO_ALLOCATE_DATA);

	saveField = new UIW_BIGNUM(13, 2, 15, &freeMemory, "", NMF_DECIMAL(0) | NMF_COMMAS,
		WOF_JUSTIFY_RIGHT | WOF_NON_SELECTABLE | WOF_BORDER);


	// Set this object pointer to the window pointer and add objects.
	window = this;
	*window
		+ new UIW_BORDER
		+ new UIW_TITLE("Free Memory")
		+ new UIW_PROMPT(0, 0, 12, "Core Left:", WOF_JUSTIFY_RIGHT)
		+ farCoreLeftField
		+ new UIW_PROMPT(0, 1, 12, "Total Left:", WOF_JUSTIFY_RIGHT)
		+ freeMemoryField
		+ new UIW_PROMPT(0, 2, 7, "(Save):", WOF_JUSTIFY_RIGHT)
		+ new UIW_BUTTON(8, 2, 5, "->", BTF_NO_TOGGLE, WOF_JUSTIFY_CENTER,
			MEMORY_WINDOW::Save)
		+ saveField;

	// Set this object pointer to the device pointer.
	device = this;
}

EVENT_TYPE MEMORY_WINDOW::Event(const UI_EVENT &event)
{
	EVENT_TYPE ccode = event.type;

	// Switch on the event type.
	switch (ccode)
	{
	case E_DEVICE:
	case E_MEMORY_WINDOW:
		// Turn the clock on or off.
		switch (event.rawCode)
		{
		case D_OFF:
		case D_ON:
			state = event.rawCode;
			break;
		}
		ccode = state;
		break;

	case SAVE_FREE_MEMORY:
		saveField->DataSet(&freeMemory);
		break;

	default:
		// Process window messages.
		ccode = UIW_WINDOW::Event(event);
	}

	return(ccode);
}

void MEMORY_WINDOW::Poll(void)
{
	// Check to see if it is on.
	if (state != D_ON)
		return;

	// See how much far core memory is left.
	char buffer[64];
	sprintf(buffer, "%ld", FarCoreLeft());
	UI_BIGNUM coreFree(buffer);

	// Check to see if core memory amount has changed.
	if (farCoreMemory != coreFree)
	{
		farCoreMemory.Import(coreFree);
		farCoreLeftField->DataSet(&farCoreMemory);
	}

	// See how much free memory there is.
	sprintf(buffer, "%ld", MemoryFree());

	UI_BIGNUM amountFree(buffer);

	// Check to see if amount of memory has changed.
	if (freeMemory != amountFree)
	{
		freeMemory.Import(amountFree);
		freeMemoryField->DataSet(&freeMemory);
	}
}

EVENT_TYPE MEMORY_WINDOW::Save(UI_WINDOW_OBJECT *object, UI_EVENT &, EVENT_TYPE ccode)
{
	if (ccode !=L_SELECT)
		return(0);

	for (UI_WINDOW_OBJECT *parentWindow = object; parentWindow->parent;
		parentWindow = parentWindow->parent)
		;
	parentWindow->Event(UI_EVENT(SAVE_FREE_MEMORY));

	return (0);
}


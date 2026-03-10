//	Zinc Interface Library - Z_EVENT.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "ui_evt.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

static int _userDevices = 0;

UI_EVENT_MANAGER::UI_EVENT_MANAGER(UI_DISPLAY *_display, int _noOfElements) :
	UI_LIST(UI_DEVICE::CompareDevices), queueBlock(_noOfElements), level(0),
	qFlags(0)
{
	// Initialize the event manager information.
	display = _display;
	UI_DEVICE::display = display;
	UI_DEVICE::eventManager = this;
#if defined(ZIL_OS2)
	hmq = WinCreateMsgQueue(display->hab, _noOfElements);
#endif
}

UI_EVENT_MANAGER::~UI_EVENT_MANAGER(void)
{
#if defined(ZIL_OS2)
	if (hmq)
	  WinDestroyMsgQueue(hmq);
#endif
}

void UI_EVENT_MANAGER::Add(UI_DEVICE *device)
{
	// Add the device to the object list.
	if (device && UI_LIST::Index(device) == -1)
	{
		UI_LIST::Add(device);
		device->Event(UI_EVENT(S_INITIALIZE, 0));
		if (device->type != E_KEY && device->type != E_MOUSE && device->type != E_CURSOR)
			_userDevices++;
	}
}

EVENT_TYPE UI_EVENT_MANAGER::DevicePosition(DEVICE_TYPE deviceType, int column, int line)
{
	// Send the position information to the affected device.
	UI_EVENT event(S_POSITION, 0);
	event.position.column = column;
	event.position.line = line;
	for (UI_DEVICE *device = First(); device; device = device->Next())
		if (deviceType == device->type)
		 	return (device->Event(event));
	return (event.type);
}

EVENT_TYPE UI_EVENT_MANAGER::DeviceState(DEVICE_TYPE deviceType, DEVICE_STATE deviceState)
{
	// Send the state information to the affected device.
	UI_EVENT event(deviceState, 0);
	for (UI_DEVICE *device = First(); device; device = device->Next())
		if (deviceType == device->type)
		 	return (device->Event(event));
	return (event.type);
}

EVENT_TYPE UI_EVENT_MANAGER::DeviceImage(DEVICE_TYPE deviceType, DEVICE_IMAGE deviceImage)
{
	// Send the image information to the affected device.
	UI_EVENT event(deviceImage, 0);
	for (UI_DEVICE *device = First(); device; device = device->Next())
		if (deviceType == device->type)
		 	return (device->Event(event));
	return (event.type);
}

EVENT_TYPE UI_EVENT_MANAGER::Event(const UI_EVENT &event, DEVICE_TYPE deviceType)
{
	// Process the event.
	EVENT_TYPE ccode = S_UNKNOWN;
	if (event.type == S_RESET_DISPLAY && !event.data)
	{
		UI_EVENT tEvent(S_DEINITIALIZE, 0);
		for (UI_DEVICE *device = First(); device; device = device->Next())
		 	ccode = device->Event(tEvent);
	}
	else if (event.type == S_RESET_DISPLAY)
	{
		display = (UI_DISPLAY *)event.data;
		UI_DEVICE::display = display;
		UI_DEVICE *tDevice = First();
		first = current = last = NULL;
		while (tDevice)
		{
			UI_DEVICE *device = tDevice;
			tDevice = device->Next();
			UI_EVENT_MANAGER::Add(device);
		}
		ccode = S_RESET_DISPLAY;
	}
	else
	{
		for (UI_DEVICE *device = First(); device; device = device->Next())
			if (deviceType == device->type || deviceType == E_DEVICE)
			 	ccode = device->Event(event);
	}

	// Return the associated response code.
	return (ccode);
}

int UI_EVENT_MANAGER::Get(UI_EVENT &event, Q_FLAGS flags)
{
	// Initialize the variables.
	if (++level == 1)
		qFlags = flags;

	UI_DEVICE *device;
	UI_QUEUE_ELEMENT *element;
	int error = -1;

	// Stay in loop while no event conditions are met.
	do
	{
		// Call all the polled devices.
		if (!FlagSet(flags, Q_NO_POLL))
		{
			// Check for a user-device to do special message polling.
#if defined(ZIL_MSWINDOWS)
			MSG message;
			if ((!FlagSet(flags, Q_NO_BLOCK) && !queueBlock.First() && !_userDevices) ||	// BUG.1641 and BUG.2217
				(!queueBlock.Full() && PeekMessage(&message, 0, 0, 0, PM_NOREMOVE)))
			{
				GetMessage(&message, 0, 0, 0);
				UI_EVENT event(E_MSWINDOWS, message.hwnd, message.message,
					message.wParam, message.lParam);
				event.message = message;
				Put(event, Q_BEGIN);
			}
#elif defined(ZIL_OS2)
			QMSG message;
			if ((!FlagSet(flags, Q_NO_BLOCK) && !queueBlock.First() && !_userDevices) ||	// BUG.1641 and BUG.2217
				(!queueBlock.Full() && WinPeekMsg(display->hab, &message, 0, 0, 0, PM_NOREMOVE)))
			{
				WinGetMsg(display->hab, &message, 0, 0, 0);
				UI_EVENT event(E_OS2, message.hwnd, message.msg, message.mp1, message.mp2);
				event.message = message;
				Put(event, Q_BEGIN);
			}
#elif defined(ZIL_MOTIF)
			if ((!FlagSet(flags, Q_NO_BLOCK) && !queueBlock.First() && !_userDevices) ||	// BUG.1641 and BUG.2217
				(!queueBlock.Full() && XtAppPending(display->appContext)))
			{
				MSG message;
				XtAppNextEvent(display->appContext, &message);
				UI_EVENT event(E_MOTIF, message);
				Put(event, Q_BEGIN);
			}
#endif

			for (device = First(); device; device = device->Next())
			 	device->Poll();
		}

		// Get the event.
		element = FlagSet(flags, Q_END) ? queueBlock.Last() : queueBlock.First();
		if (element)
		{
			event = element->event;
			if (!FlagSet(flags, Q_NO_DESTROY))
				queueBlock.Subtract((UI_ELEMENT *)element);
			error = 0;		// Normal operation of the event manager.
		}
		else if (FlagSet(flags, Q_NO_BLOCK))
		{
			if (--level == 0)
				qFlags = 0;
			return (-2);	// No events were in queue.
		}
	} while (error);

	// Return the error status.
	if (--level == 0)
		qFlags = 0;
	return (error);
}

void UI_EVENT_MANAGER::Put(const UI_EVENT &event, Q_FLAGS flags)
{
	// Place the event back in the event queue.
	UI_QUEUE_ELEMENT *element;
	if (FlagSet(flags, Q_END))
		element = (UI_QUEUE_ELEMENT *)queueBlock.Add(NULLP(UI_QUEUE_ELEMENT));
	else
		element = (UI_QUEUE_ELEMENT *)queueBlock.Add(queueBlock.First());
	if (element)
		element->event = event;
}

void UI_EVENT_MANAGER::Subtract(UI_DEVICE *device)
{
	// Remove the device from the object list.
	if (UI_LIST::Index(device) != -1)
	{
		if (device->type != E_KEY && device->type != E_MOUSE && device->type != E_CURSOR)
			_userDevices--;
		device->Event(UI_EVENT(S_DEINITIALIZE, 0));
		UI_LIST::Subtract(device);
	}
}

UI_QUEUE_ELEMENT::UI_QUEUE_ELEMENT(void) :
	UI_ELEMENT()
{
}

UI_QUEUE_ELEMENT::~UI_QUEUE_ELEMENT(void)
{
}

UI_QUEUE_BLOCK::UI_QUEUE_BLOCK(int _noOfElements) :
	UI_LIST_BLOCK(_noOfElements)
{
	// Initialize the queue block.
	UI_QUEUE_ELEMENT *queueBlock = new UI_QUEUE_ELEMENT[_noOfElements];
	elementArray = queueBlock;
	for (int i = 0; i < _noOfElements; i++)
		freeList.Add(NULLP(UI_ELEMENT), &queueBlock[i]);
}

UI_QUEUE_BLOCK::~UI_QUEUE_BLOCK(void)
{
	// Free the queue block.
	UI_QUEUE_ELEMENT *queueBlock = (UI_QUEUE_ELEMENT *)elementArray;

	// Early C++ versions require explicit number of elements.
#if defined(ZIL_DELETE)
	delete [] queueBlock;
#else
	delete [noOfElements]queueBlock;
#endif
}


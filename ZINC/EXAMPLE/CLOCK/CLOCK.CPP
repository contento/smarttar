//	CLOCK.CPP (CLOCK) - Example of a derived device.
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA
//  May be freely copied, used and distributed.

#include <ui_win.hpp>

// Define the clock device type.
const DEVICE_TYPE E_CLOCK = 80;

class CLOCK : public UI_DEVICE, public UIW_WINDOW
{
public:
	UI_DEVICE *device;
	UIW_WINDOW *window;

	CLOCK(int left, int top);

private:
	UI_TIME time;
	UIW_TIME *timeField;

	EVENT_TYPE Event(const UI_EVENT &event);
	void Poll(void);
};

CLOCK::CLOCK(int left, int top):
	UI_DEVICE(E_CLOCK, D_ON),
	UIW_WINDOW(left, top, 14, 3, WOF_NO_FLAGS, WOAF_NO_SIZE | WOAF_NO_DESTROY)
{
	// Setup the time.
	timeField = new UIW_TIME(1, 0, 10, &time, "",
		TMF_SECONDS | TMF_COLON_SEPARATOR | TMF_TWENTY_FOUR_HOUR | TMF_ZERO_FILL,
		WOF_JUSTIFY_CENTER | WOF_NON_SELECTABLE | WOF_NO_ALLOCATE_DATA);

	// Set this object pointer to the window pointer and add objects.
	window = this;
	*window
		+ new UIW_BORDER
		+ &(*new UIW_SYSTEM_BUTTON
			+ new UIW_POP_UP_ITEM("&Move", MNIF_MOVE)
			+ new UIW_POP_UP_ITEM("&Close\tAlt+F4", MNIF_CLOSE))
		+ new UIW_TITLE("Clock")
		+ timeField;

	// Set this object pointer to the device pointer.
	device = this;
}

EVENT_TYPE CLOCK::Event(const UI_EVENT &event)
{
	// Switch on the event type.
	EVENT_TYPE ccode = event.type;
	switch (ccode)
	{
	case E_DEVICE:
	case E_CLOCK:
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

	default:
		// Process window messages.
		ccode = UIW_WINDOW::Event(event);
	}

	// Return the control code.
	return (ccode);
}

void CLOCK::Poll(void)
{
	// Check to see if the clock is on.
	if (state != D_ON)
		return;

	UI_TIME newTime;
	int hour, minute, second;
	int oldHour, oldMinute, oldSecond;

	// Check to see if the time has changed.
	newTime.Export(&hour, &minute, &second);
	time.Export(&oldHour, &oldMinute, &oldSecond);
	if (oldSecond != second || oldMinute != minute)
	{
		time.Import();
		timeField->DataSet(&time);
	}
}

int UI_APPLICATION::Main(void)
{
	// The UI_APPLICATION constructor automatically initializes the 
	// display, eventManager, and windowManager variables.

	// This line fixes linkers that don't look for main in the .LIBs.
	UI_APPLICATION::LinkMain();

	// Add the clock to the window and event managers.
	CLOCK *clock = new CLOCK(0, 0);
	*windowManager + clock->window;
	*eventManager + clock->device;

	// Wait for user response.
 	UI_EVENT event;
	EVENT_TYPE ccode;
	do
	{
		// Get input from the user.
		if (!eventManager->Get(event, Q_NO_BLOCK | Q_BEGIN | Q_DESTROY | Q_POLL))
			ccode = windowManager->Event(event);
	} while (ccode != L_EXIT && ccode != S_NO_OBJECT);

	// Clean up.
	*eventManager - clock->device;
	*windowManager - clock->window;

	return (0);
}

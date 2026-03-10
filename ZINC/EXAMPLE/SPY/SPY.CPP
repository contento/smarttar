//	SPY.CPP (SPY) - An event monitor example program.
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA
//  May be freely copied, used and distributed.

// This is an simple program that catches all messages put on the event
// queue except mouse movement events.  Many of the events listed below in
// the spy class member functions are not seen often because they are
// interpreted by the window manager or window objects from the keyboard
// or mouse events and used directly.
//
// To use SPY just add a new SPY to the event manager.  It automatically
// creates a TTY_WINDOW window (see tty.cpp) and adds it to the window
// manager.  Puting an event of type E_SPY and rawCode D_OFF on the event
// queue will disable the SPY.  Likewise, an event of type E_SPY and
// rawCode D_ON will enable it.
//
// The spy remains in the event manager's device list until the programmer
// subtracts it from the event manager or the event manager is deleted.  To
// redisplay the spy window after the user has closed it send a message to 
// the spy device with event.type = E_SPY and event.rawCode = D_ON.  The 
// spy should be created and added to the event manager only once.
//
// Many advanced features can be added to this SPY if the programmer takes
// the initiative.  These could include disabling all mouse events (currently
// only mouse move events are disabled) or setting a breakpoint on a certain event.

#include <ui_win.hpp>
#include "tty.hpp"

class SPY : public UI_DEVICE
{
public:
	SPY(UI_WINDOW_MANAGER *_windowManager);
	~SPY(void) { delete spyWindow; }

	EVENT_TYPE Event(const UI_EVENT &event);

protected:
	void Poll(void);

private:
	UI_WINDOW_MANAGER *windowManager;
	UIW_WINDOW *spyWindow;
	TTY_WINDOW *ttyWindow;
	int lastMouseEvent;
};

SPY::SPY(UI_WINDOW_MANAGER *_windowManager) :
	UI_DEVICE(E_DEVICE, D_ON)
{
	// Initialize the member variables.
	lastMouseEvent = 0;
	windowManager = _windowManager;

	// Create the window to display events in. (see tty.cpp)
	spyWindow = UIW_WINDOW::Generic(40, 0, 60, 14, "Spy Window");
	*spyWindow
		+ (ttyWindow = new TTY_WINDOW);

	// The spy should delete the spy window and not the window manager.
	spyWindow->woAdvancedFlags |= WOAF_NO_DESTROY;
}

EVENT_TYPE SPY::Event(const UI_EVENT &event)
{
	// Process events for this device.
	switch (event.type)
	{
	case S_INITIALIZE:
	case D_ON:
		// Add the spy window to the window manager.
		*windowManager + spyWindow;
		state = D_ON;
		break;

	case D_OFF:
		// Subtract the spy window from the window manager.
		*windowManager - spyWindow;
		state = D_OFF;
		break;
	}

	return(state);
}

void SPY::Poll(void)
{
	char *typeName;

	if (state != D_ON)
		return;

	// Get next event on the queue (Only if not NO_DESTROY).
	//  Return if nothing there.
	UI_EVENT event;
	if (FlagSet(eventManager->QFlags(), Q_NO_DESTROY) ||
		eventManager->Get(event, Q_NO_BLOCK | Q_NO_DESTROY | Q_NO_POLL))
			return;

	// Add the event strings to the spy event list.
	switch (event.type)
	{
#if defined(ZIL_MSWINDOWS)
	case E_MSWINDOWS:
		typeName = "E_MSWINDOWS";
		if (event.message.message == WM_KEYDOWN)
			ttyWindow->Printf("WM_KEYDOWN");
		if (event.message.message == WM_KEYUP)
			ttyWindow->Printf("WM_KEYUP");
		break;
#elif defined(ZIL_OS2)
	case E_OS2:
		typeName = "E_OS2";
		if (event.message.msg == WM_CHAR)
			ttyWindow->Printf("WM_CHAR");
		break;
#elif defined(ZIL_MOTIF)
	case E_MOTIF:
		typeName = "E_MOTIF";
		if (event.message.type == KeyPress)
			ttyWindow->Printf("KeyPress");
		if (event.message.type == KeyRelease)
			ttyWindow->Printf("KeyRelease");
		break;
#endif

	case E_KEY:
		typeName = "E_KEY";
		ttyWindow->Printf(" %s   %04x %02x  %c", typeName, event.rawCode, event.key.shiftState, event.key.value);
		break;

	case E_MOUSE:
		typeName = "E_MOUSE";
		if (lastMouseEvent == event.rawCode || !event.rawCode)
			return;
		lastMouseEvent = event.rawCode;
		ttyWindow->Printf(" %s %04x %03d %03d", typeName, event.rawCode, event.position.column, event.position.line);
		break;

	default:
		typeName = "<Unknown>";
		ttyWindow->Printf(" %s", typeName);
	}
}

int UI_APPLICATION::Main(void)
{
	// The UI_APPLICATION constructor automatically initializes the 
	// display, eventManager, and windowManager variables.

	// This line fixes linkers that don't look for main in the .LIBs.
	UI_APPLICATION::LinkMain();

	// Add the spy to the event manager.
	*eventManager + new SPY(windowManager);

	// Create sample window.
	UIW_WINDOW *window = UIW_WINDOW::Generic(3, 5, 40, 6, "Text window for SPY!");
	*window
		+ new UIW_TEXT(0, 0, 0, 0, "Sample Text!", 256, WNF_NO_FLAGS,
			WOF_NON_FIELD_REGION);

	// Add the window the window manager.
	*windowManager + window;

	// Process user responses.
	UI_APPLICATION::Control();

	return (0);
}

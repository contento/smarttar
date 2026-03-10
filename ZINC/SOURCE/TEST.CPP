//	HELLO1.CPP (HELLO) - Simple "Hello, World!" in a window.
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA
//  May be freely copied, used and distributed.

#include <ui_win.hpp>

#if defined(ZIL_MSDOS)
main()
{
	// Create the MSDOS display.
	UI_DISPLAY *display = new UI_GRAPHICS_DISPLAY;
	if (!display->installed)
	{
		delete display;
		display = new UI_TEXT_DISPLAY;
	}
#elif defined(ZIL_MSWINDOWS)
int PASCAL WinMain(HANDLE hInstance, HANDLE hPrevInstance, LPSTR, int nCmdShow)
{
	// Create the Windows display.
	UI_DISPLAY *display = new UI_MSWINDOWS_DISPLAY(hInstance, hPrevInstance, nCmdShow);
#elif defined(ZIL_OS2)
main()
{
	// Create the OS/2 display.
	UI_DISPLAY *display = new UI_OS2_DISPLAY;
#elif defined(ZIL_MOTIF)
main(int argc, char **argv)
{
	// Create the Motif display and initialize XtIntrinsics using application
	// class "ZincApp".
	UI_DISPLAY *display = new UI_MOTIF_DISPLAY(&argc, argv, "ZincApp");
#endif

	// Make sure the display installed correctly.
	if (!display || !display->installed)
	{
		delete display;
		return (0);
	}

	// Create the event manager and add devices.
	UI_EVENT_MANAGER *eventManager = new UI_EVENT_MANAGER(display);
	*eventManager
		+ new UID_KEYBOARD
		+ new UID_MOUSE
		+ new UID_CURSOR;

	// Create the window manager.
	UI_WINDOW_MANAGER *windowManager = new UI_WINDOW_MANAGER(display, eventManager);

	// Create the hello world window.
	UIW_WINDOW *window = new UIW_WINDOW(5, 5, 40, 6);

	// Add the window objects to the window.
	*window
		+ new UIW_BORDER
		+ new UIW_MAXIMIZE_BUTTON
		+ new UIW_MINIMIZE_BUTTON
		+ new UIW_SYSTEM_BUTTON(SYF_GENERIC)
		+ new UIW_TITLE("Hello World Window")
		+ new UIW_TEXT(0, 0, 0, 0, "Hello, World!", 256, WNF_NO_FLAGS,
			WOF_NON_FIELD_REGION);

	// Add the window to the window manager.
	*windowManager + window;

	// Wait for user response.
	UI_EVENT event;
	EVENT_TYPE ccode;
	do
	{
		// Get input from the user.
		eventManager->Get(event);

		// Send event information to the window manager.
		ccode = windowManager->Event(event);
	} while (ccode != L_EXIT && ccode != S_NO_OBJECT);

	// Clean up.
	delete windowManager;
	delete eventManager;
	delete display;

	return (0);
}

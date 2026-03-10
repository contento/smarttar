//  MESSAGES.CPP (MESSAGES) - User defined message handling.
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//  Zinc Software Incorporated.  Pleasant Grove, Utah  USA
//  May be freely copied, used and distributed.

#include <string.h>
#include <ui_win.hpp>

// User defined event type.
const EVENT_TYPE U_SET = 12001;

// MY_BUTTON is derived from UIW_BUTTON that understands U_SET event.
class EXPORT MY_BUTTON : public UIW_BUTTON
{
public:
	MY_BUTTON(int left, int top, int width, char *_string, BTF_FLAGS _btFlags,
		WOF_FLAGS _woFlags, USER_FUNCTION _userFunction) : UIW_BUTTON(left, top, width, _string, _btFlags,
		_woFlags, _userFunction) {}

	virtual EVENT_TYPE Event(const UI_EVENT &event);
	static EVENT_TYPE LeaveMessage(UI_WINDOW_OBJECT *object, UI_EVENT &event,
		EVENT_TYPE ccode);
	static EVENT_TYPE UserFunction(UI_WINDOW_OBJECT *object, UI_EVENT &event,
		EVENT_TYPE ccode);
};

// Event function for MY_BUTTON that understands U_SET event.
EVENT_TYPE MY_BUTTON::Event(const UI_EVENT &event)
{
	if (event.type == U_SET)
	{
		Information(SET_TEXT, (char *)event.data, ID_BUTTON);
		delete event.data;

		// Leave message to redisplay button after DataSet.
		return (UIW_BUTTON::Event(UI_EVENT(S_REDISPLAY, 0, true)));
	}
	else
		return (UIW_BUTTON::Event(event));
}

EVENT_TYPE MY_BUTTON::LeaveMessage(UI_WINDOW_OBJECT *object, UI_EVENT &event,
	EVENT_TYPE ccode)
{
	if (ccode != L_SELECT)
		return (ccode);

	// Leave a message to close current window;
	event.type = S_CLOSE;
	object->eventManager->Put(event);

	// Leave a User Defined message for current object to DataSet itself.
	event.type = U_SET;
	event.data = ui_strdup((char *)object->Information(GET_TEXT, NULL, ID_BUTTON));
	object->eventManager->Put(event);

	return (ccode);
}

// User Funtion for buttons that will receive message.
EVENT_TYPE MY_BUTTON::UserFunction(UI_WINDOW_OBJECT *object, UI_EVENT &,
	EVENT_TYPE ccode)
{
	if (ccode != L_SELECT)
		return (ccode);

	// Create a list of buttons with possible entries in them.
	UIW_VT_LIST *list = new UIW_VT_LIST(0, 0, 0, 0, NULL,
		WNF_NO_WRAP, WOF_NON_FIELD_REGION | WOF_BORDER, WOAF_NO_FLAGS);
	*list	  
		+ new MY_BUTTON(0, 0, 10, "OPTION 1", BTF_NO_3D | BTF_NO_TOGGLE,
			WOF_JUSTIFY_CENTER, LeaveMessage)
		+ new MY_BUTTON(0, 0, 10, "OPTION 2", BTF_NO_3D | BTF_NO_TOGGLE,
			WOF_JUSTIFY_CENTER, LeaveMessage)
		+ new MY_BUTTON(0, 0, 10, "OPTION 3", BTF_NO_3D | BTF_NO_TOGGLE,
			WOF_JUSTIFY_CENTER, LeaveMessage)
		+ new MY_BUTTON(0, 0, 10, "OPTION 4", BTF_NO_3D | BTF_NO_TOGGLE,
			WOF_JUSTIFY_CENTER, LeaveMessage)
		+ new MY_BUTTON(0, 0, 10, "OPTION 5", BTF_NO_3D | BTF_NO_TOGGLE,
			WOF_JUSTIFY_CENTER, LeaveMessage)
		+ new MY_BUTTON(0, 0, 10, "OPTION 6", BTF_NO_3D | BTF_NO_TOGGLE,
			WOF_JUSTIFY_CENTER, LeaveMessage)
		+ new MY_BUTTON(0, 0, 10, "OPTION 7", BTF_NO_3D | BTF_NO_TOGGLE,
			WOF_JUSTIFY_CENTER, LeaveMessage)
		+ new MY_BUTTON(0, 0, 10, "OPTION 8", BTF_NO_3D | BTF_NO_TOGGLE,
			WOF_JUSTIFY_CENTER, LeaveMessage)
		+ new MY_BUTTON(0, 0, 10, "OPTION 9", BTF_NO_3D | BTF_NO_TOGGLE,
			WOF_JUSTIFY_CENTER, LeaveMessage)
		+ new MY_BUTTON(0, 0, 10, "OPTION 10", BTF_NO_3D | BTF_NO_TOGGLE,
			WOF_JUSTIFY_CENTER, LeaveMessage);

	// Create a window with the list.
	UIW_WINDOW *window = new UIW_WINDOW(40, 5, 20, 14, WOF_NO_FLAGS,
		WOAF_NO_MOVE | WOAF_NO_SIZE | WOAF_MODAL);
	*window
		+ new UIW_BORDER
		+ new UIW_TITLE("OPTIONS")
		+ list;

	// Add the window to the window manager.
	*object->windowManager + window;

	return (ccode);
}

int UI_APPLICATION::Main(void)
{
	// The UI_APPLICATION constructor automatically initializes the 
	// display, eventManager, and windowManager variables.

	// This line fixes linkers that don't look for main in the .LIBs.
	UI_APPLICATION::LinkMain();

	UIW_WINDOW *mainWindow = UIW_WINDOW::Generic(5, 5, 22, 6, "MAIN");
	*mainWindow
		+ new MY_BUTTON(1, 1, 18, "Press Me", BTF_NO_TOGGLE, WOF_JUSTIFY_CENTER, MY_BUTTON::UserFunction)
		+ new MY_BUTTON(1, 3, 18, "Press Me Too", BTF_NO_TOGGLE, WOF_JUSTIFY_CENTER, MY_BUTTON::UserFunction);

	*windowManager
		+ mainWindow;

	// Process user responses.
	UI_APPLICATION::Control();

	return (0);
}

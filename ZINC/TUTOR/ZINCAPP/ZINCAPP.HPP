//	ZINCAPP.HPP (ZINCAPP) - This file contains the zinc application control and class definitions.
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA
//  May be freely copied, used and distributed.

// Display options.
const RAW_CODE MSG_DISPLAY			= 10001;
const RAW_CODE MSG_25x40_MODE		= 10002;
const RAW_CODE MSG_25x80_MODE		= 10003;
const RAW_CODE MSG_43x80_MODE		= 10004;
const RAW_CODE MSG_GRAPHICS_MODE	= 10005;
const RAW_CODE MSG_WINDOWS_MODE		= 10006;

// Window options.
const RAW_CODE MSG_WINDOW			= 10100;
const RAW_CODE MSG_GENERIC_WINDOW	= 10101;
const RAW_CODE MSG_DATE_WINDOW		= 10102;
const RAW_CODE MSG_TIME_WINDOW		= 10103;
const RAW_CODE MSG_NUMBER_WINDOW	= 10104;
const RAW_CODE MSG_STRING_WINDOW	= 10105;
const RAW_CODE MSG_TEXT_WINDOW		= 10106;
const RAW_CODE MSG_MENU_WINDOW		= 10107;
const RAW_CODE MSG_LIST_WINDOW		= 10108;
const RAW_CODE MSG_ICON_WINDOW		= 10109;
const RAW_CODE MSG_BUTTON_WINDOW	= 10110;
const RAW_CODE MSG_TOOL_BAR_WINDOW	= 10111;
const RAW_CODE MSG_COMBO_BOX_WINDOW	= 10112;
const RAW_CODE MSG_MDI_WINDOW		= 10113;

// Event options.
const RAW_CODE MSG_EVENT			= 10200;
const RAW_CODE MSG_EVENT_MONITOR	= 10201;

// Help options.
const RAW_CODE MSG_HELP				= 10300;
const RAW_CODE MSG_HELP_KEYBOARD	= 10302;
const RAW_CODE MSG_HELP_MOUSE		= 10303;
const RAW_CODE MSG_HELP_COMMANDS	= 10304;
const RAW_CODE MSG_HELP_PROCEDURES	= 10305;
const RAW_CODE MSG_HELP_OBJECTS		= 10306;
const RAW_CODE MSG_HELP_HELP		= 10307;
const RAW_CODE MSG_HELP_ZINCAPP		= 10308;

// Highest and lowest messages for ZINCAPP.
const RAW_CODE MSG_FIRST			= 10000;
const RAW_CODE MSG_LAST				= 20000;

class ZINCAPP_WINDOW_MANAGER : public UI_WINDOW_MANAGER
{
public:
	ZINCAPP_WINDOW_MANAGER(UI_DISPLAY *display, UI_EVENT_MANAGER *eventManager) :
		UI_WINDOW_MANAGER(display, eventManager, ZINCAPP_WINDOW_MANAGER::ExitFunction) { }
	virtual EVENT_TYPE Event(const UI_EVENT &event);

private:
	static EVENT_TYPE ExitFunction(UI_DISPLAY *display,
		UI_EVENT_MANAGER *eventManager, UI_WINDOW_MANAGER *windowManager);
};

class CONTROL_WINDOW : public UIW_WINDOW
{
public:
	CONTROL_WINDOW(void);
	virtual EVENT_TYPE Event(const UI_EVENT &event);
	static EVENT_TYPE Message(UI_WINDOW_OBJECT *object, UI_EVENT &event, 
		EVENT_TYPE ccode);

protected:
	void OptionDisplay(EVENT_TYPE item);
	void OptionEvent(EVENT_TYPE item);
	void OptionHelp(EVENT_TYPE item);
	void OptionWindow(EVENT_TYPE item);

	UI_DEVICE *EventMonitor(void);
};

class EVENT_MONITOR : public UIW_WINDOW
{
public:
	EVENT_MONITOR(void);
	EVENT_TYPE Event(const UI_EVENT &event);

private:
#if defined(ZIL_MSDOS)
	UIW_STRING *keyboard[3];
	UI_EVENT kEvent;
	UIW_STRING *mouse[3];
	UI_EVENT mEvent;
#elif defined(ZIL_MSWINDOWS)
	UIW_STRING *windowsMessage[5];
	MSG wMsg;
#elif defined(ZIL_OS2)
	UIW_STRING *windowsMessage[5];
	QMSG oMsg;
#elif defined(ZIL_MOTIF)
	UIW_STRING *motifMessage[3];
	XEvent xEvt;
#endif
	UIW_STRING *system;
	UI_EVENT sEvent;
};

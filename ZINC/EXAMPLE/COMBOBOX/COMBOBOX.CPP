//	COMBOBOX.CPP (COMBOBOX) - Combo box example program.
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA
//  May be freely copied, used and distributed.

#include <string.h>
#include <ui_win.hpp>

#define Z_COM_PORT1			1
#define Z_COM_PORT2			2
#define Z_STOP_BITS_1		3
#define Z_STOP_BITS_2		4
#define Z_PARITY_NONE		5
#define Z_PARITY_EVEN		6
#define Z_PARITY_ODD		7
#define Z_PARITY_STICKY		8
#define Z_DATA_BITS_7		9
#define Z_DATA_BITS_8    	10
#define Z_BAUD_300       	11
#define Z_BAUD_1200      	12
#define Z_BAUD_2400      	13
#define Z_BAUD_9600      	14
#define Z_BAUD_19200     	15
#define Z_BAUD_38400     	16
#define Z_BAUD_115K      	17

class EXPORT COMPORT_SETUP : public UIW_WINDOW
{
public:
	UIW_STRING *currentPort;
	UIW_STRING *currentBaud;
	UIW_STRING *currentParity;
	UIW_STRING *currentDataBits;
	UIW_STRING *currentStopBits;

	UIW_COMBO_BOX *portField;
	UIW_COMBO_BOX *baudField;
	UIW_COMBO_BOX *parityField;
	UIW_COMBO_BOX *dataBitsField;
	UIW_COMBO_BOX *stopBitsField;
	UIW_GROUP *currentSettings;

	COMPORT_SETUP(void);

private:
	UIW_BUTTON *defaultPort;
	UIW_BUTTON *defaultBaud;
	UIW_BUTTON *defaultParity;
	UIW_BUTTON *defaultDataBits;
	UIW_BUTTON *defaultStopBits;

	static EVENT_TYPE ResetDefaults(UI_WINDOW_OBJECT *object, UI_EVENT &event,
		EVENT_TYPE ccode);
	static EVENT_TYPE NewSettings(UI_WINDOW_OBJECT *object, UI_EVENT &event,
		EVENT_TYPE ccode);
};

EVENT_TYPE COMPORT_SETUP::ResetDefaults(UI_WINDOW_OBJECT *object, UI_EVENT &,
	EVENT_TYPE ccode)
{
	if (ccode != L_SELECT)
		return (ccode);

	for (UI_WINDOW_OBJECT *window = object; window->parent;
		window = window->parent)
			;

	COMPORT_SETUP *parentWindow = (COMPORT_SETUP *)window;

	// Reset current settings.
	parentWindow->currentPort->Information(SET_TEXT, "Com port 1");
	parentWindow->currentStopBits->Information(SET_TEXT, "1 stop bit");
	parentWindow->currentParity->Information(SET_TEXT, "No parity");
	parentWindow->currentDataBits->Information(SET_TEXT, "8 data bits");
	parentWindow->currentBaud->Information(SET_TEXT, "9600 baud");

	// Reset the combo box's default current items.
	UI_EVENT tEvent(S_ADD_OBJECT);
	tEvent.data = (void *)parentWindow->defaultPort;
	parentWindow->portField->Event(tEvent);
	parentWindow->portField->Information(SET_TEXT, "Com port 1");
	tEvent.data = (void *)parentWindow->defaultBaud;
	parentWindow->baudField->Event(tEvent);
	parentWindow->baudField->Information(SET_TEXT, "9600 baud");
	tEvent.data = (void *)parentWindow->defaultParity;
	parentWindow->parityField->Event(tEvent);
	parentWindow->parityField->Information(SET_TEXT, "No parity");
	tEvent.data = (void *)parentWindow->defaultDataBits;
	parentWindow->dataBitsField->Event(tEvent);
	parentWindow->dataBitsField->Information(SET_TEXT, "8 data bits");
	tEvent.data = (void *)parentWindow->defaultStopBits;
	parentWindow->stopBitsField->Event(tEvent);
	parentWindow->stopBitsField->Information(SET_TEXT, "1 stop bit");

	return (ccode);
}

EVENT_TYPE COMPORT_SETUP::NewSettings(UI_WINDOW_OBJECT *object, UI_EVENT &,
	EVENT_TYPE ccode)
{
	if (ccode != L_SELECT)
		return (ccode);

	for (UI_WINDOW_OBJECT *window = object; window->parent;
		window = window->parent)
			;

	COMPORT_SETUP *parentWindow = (COMPORT_SETUP *)window;
	switch (((UIW_BUTTON *)object)->value)
	{
	case Z_COM_PORT1:
		parentWindow->currentPort->Information(SET_TEXT, "Com port 1");
		break;

	case Z_COM_PORT2:
		parentWindow->currentPort->Information(SET_TEXT, "Com port 2");
		break;

	case Z_STOP_BITS_1:
		parentWindow->currentStopBits->Information(SET_TEXT, "1 stop bit");
		break;

	case Z_STOP_BITS_2:
		parentWindow->currentStopBits->Information(SET_TEXT, "2 stop bits");
		break;

	case Z_PARITY_NONE:
		parentWindow->currentParity->Information(SET_TEXT, "No parity");
		break;

	case Z_PARITY_EVEN:
		parentWindow->currentParity->Information(SET_TEXT, "Even parity");
		break;

	case Z_PARITY_ODD:
		parentWindow->currentParity->Information(SET_TEXT, "Odd parity");
		break;

	case Z_PARITY_STICKY:
		parentWindow->currentParity->Information(SET_TEXT, "Sticky parity");
		break;

	case Z_DATA_BITS_7:
		parentWindow->currentDataBits->Information(SET_TEXT, "7 data bits");
		break;

	case Z_DATA_BITS_8:
		parentWindow->currentDataBits->Information(SET_TEXT, "8 data bits");
		break;

	case Z_BAUD_300:
		parentWindow->currentBaud->Information(SET_TEXT, "300 baud");
		break;

	case Z_BAUD_1200:
		parentWindow->currentBaud->Information(SET_TEXT, "1200 baud");
		break;

	case Z_BAUD_2400:
		parentWindow->currentBaud->Information(SET_TEXT, "2400 baud");
		break;

	case Z_BAUD_9600:
		parentWindow->currentBaud->Information(SET_TEXT, "9600 baud");
		break;

	case Z_BAUD_19200:
		parentWindow->currentBaud->Information(SET_TEXT, "19.2 Kbaud");
		break;

	case Z_BAUD_38400:
		parentWindow->currentBaud->Information(SET_TEXT, "38.4 Kbaud");
		break;

	case Z_BAUD_115K:
		parentWindow->currentBaud->Information(SET_TEXT, "115 Kbaud");
		break;
	}

	return (ccode);
}

COMPORT_SETUP::COMPORT_SETUP(void) : UIW_WINDOW(0, 0, 60, 10)
{
	portField = &(*new UIW_COMBO_BOX(15, 1, 20, 5, NULL, WNF_NO_WRAP, WOF_VIEW_ONLY)
		+ (defaultPort = new UIW_BUTTON(0,0,0, "Com port 1", BTF_NO_3D | BTF_NO_TOGGLE,
			WOF_NO_FLAGS, COMPORT_SETUP::NewSettings, Z_COM_PORT1))
		+ new UIW_BUTTON(0,0,0, "Com port 2", BTF_NO_3D | BTF_NO_TOGGLE,
			WOF_NO_FLAGS, COMPORT_SETUP::NewSettings, Z_COM_PORT2));

	baudField = &(*new UIW_COMBO_BOX(15, 2, 20, 5, NULL, WNF_NO_WRAP, WOF_VIEW_ONLY)
		+ new UIW_BUTTON(0,0,0, "300 baud", BTF_NO_3D | BTF_NO_TOGGLE,
			WOF_NO_FLAGS, COMPORT_SETUP::NewSettings, Z_BAUD_300)
		+ new UIW_BUTTON(0,0,0, "1200 baud", BTF_NO_3D | BTF_NO_TOGGLE,
			WOF_NO_FLAGS, COMPORT_SETUP::NewSettings, Z_BAUD_1200)
		+ new UIW_BUTTON(0,0,0, "2400 baud", BTF_NO_3D | BTF_NO_TOGGLE,
			WOF_NO_FLAGS, COMPORT_SETUP::NewSettings, Z_BAUD_2400)
		+ (defaultBaud = new UIW_BUTTON(0,0,0, "9600 baud", BTF_NO_3D | BTF_NO_TOGGLE,
			WOF_NO_FLAGS, COMPORT_SETUP::NewSettings, Z_BAUD_9600))
		+ new UIW_BUTTON(0,0,0, "19.2 Kbaud", BTF_NO_3D | BTF_NO_TOGGLE,
			WOF_NO_FLAGS, COMPORT_SETUP::NewSettings, Z_BAUD_19200)
		+ new UIW_BUTTON(0,0,0, "38.4 Kbaud", BTF_NO_3D | BTF_NO_TOGGLE,
			WOF_NO_FLAGS, COMPORT_SETUP::NewSettings, Z_BAUD_38400)
		+ new UIW_BUTTON(0,0,0, "115 Kbaud", BTF_NO_3D | BTF_NO_TOGGLE,
			WOF_NO_FLAGS, COMPORT_SETUP::NewSettings, Z_BAUD_115K));

	// Set the default baud rate.
	UI_EVENT tEvent;
	tEvent.type = S_ADD_OBJECT;
	tEvent.data = (void *)defaultBaud;
	baudField->Event(tEvent);

	parityField = &(*new UIW_COMBO_BOX(15, 3, 20, 5, NULL, WNF_NO_WRAP, WOF_VIEW_ONLY)
		+ (defaultParity = new UIW_BUTTON(0,0,0, "No parity", BTF_NO_3D | BTF_NO_TOGGLE,
			WOF_NO_FLAGS, COMPORT_SETUP::NewSettings, Z_PARITY_NONE))
		+ new UIW_BUTTON(0,0,0, "Even parity", BTF_NO_3D | BTF_NO_TOGGLE,
			WOF_NO_FLAGS, COMPORT_SETUP::NewSettings, Z_PARITY_EVEN)
		+ new UIW_BUTTON(0,0,0, "Odd parity", BTF_NO_3D | BTF_NO_TOGGLE,
			WOF_NO_FLAGS, COMPORT_SETUP::NewSettings, Z_PARITY_ODD)
		+ new UIW_BUTTON(0,0,0, "Sticky parity", BTF_NO_3D | BTF_NO_TOGGLE,
			WOF_NO_FLAGS, COMPORT_SETUP::NewSettings, Z_PARITY_STICKY));

	dataBitsField = &(*new UIW_COMBO_BOX(15, 4, 20, 5, NULL, WNF_NO_WRAP, WOF_VIEW_ONLY)
		+ new UIW_BUTTON(0,0,0, "7 data bits", BTF_NO_3D | BTF_NO_TOGGLE,
			WOF_NO_FLAGS, COMPORT_SETUP::NewSettings, Z_DATA_BITS_7)
		+ (defaultDataBits = new UIW_BUTTON(0,0,0, "8 data bits", BTF_NO_3D | BTF_NO_TOGGLE,
			WOF_NO_FLAGS, COMPORT_SETUP::NewSettings, Z_DATA_BITS_8)));

	// Set the default data bits.
	tEvent.data = (void *)defaultDataBits;
	dataBitsField->Event(tEvent);

	stopBitsField = &(*new UIW_COMBO_BOX(15, 5, 20, 5, NULL, WNF_NO_WRAP, WOF_VIEW_ONLY)
		+ (defaultStopBits = new UIW_BUTTON(0,0,0, "1 stop bit", BTF_NO_3D | BTF_NO_TOGGLE,
			WOF_NO_FLAGS, COMPORT_SETUP::NewSettings, Z_STOP_BITS_1))
		+ new UIW_BUTTON(0,0,0, "2 stop bits", BTF_NO_3D | BTF_NO_TOGGLE,
			WOF_NO_FLAGS, COMPORT_SETUP::NewSettings, Z_STOP_BITS_2));

	currentSettings = new UIW_GROUP(37, 0, 20, 6, "Settings:", WNF_NO_FLAGS, WOF_NON_SELECTABLE);
	*currentSettings
		+ (currentPort = new UIW_STRING(1, 1, 15, "Com port 1", 20, STF_NO_FLAGS, WOF_NON_SELECTABLE))
		+ (currentBaud = new UIW_STRING(1, 2, 15, "9600 baud", 20, STF_NO_FLAGS, WOF_NON_SELECTABLE))
		+ (currentParity = new UIW_STRING(1, 3, 15, "No parity", 20, STF_NO_FLAGS, WOF_NON_SELECTABLE))
		+ (currentDataBits = new UIW_STRING(1, 4, 15, "8 data bits", 20, STF_NO_FLAGS, WOF_NON_SELECTABLE))
		+ (currentStopBits = new UIW_STRING(1, 5, 15, "1 stop bit", 20, STF_NO_FLAGS, WOF_NON_SELECTABLE));

	*this
		+ new UIW_BORDER
		+ new UIW_MAXIMIZE_BUTTON
		+ new UIW_MINIMIZE_BUTTON
		+ new UIW_SYSTEM_BUTTON(SYF_GENERIC)
		+ new UIW_TITLE("Communications Setup")
		+ new UIW_PROMPT(1, 1, "Port in use:")
		+ portField
		+ new UIW_PROMPT(1, 2, "Baud rate:")
		+ baudField
		+ new UIW_PROMPT(1, 3, "Parity:")
		+ parityField
		+ new UIW_PROMPT(1, 4, "Data bits:")
		+ dataBitsField
		+ new UIW_PROMPT(1, 5, "Stop bits:")
		+ stopBitsField
		+ new UIW_BUTTON(37, 7, 18, "&Default Settings", BTF_NO_TOGGLE, WOF_NO_FLAGS,
			COMPORT_SETUP::ResetDefaults)
		+ currentSettings;
}

int UI_APPLICATION::Main(void)
{
	// The UI_APPLICATION constructor automatically initializes the 
	// display, eventManager, and windowManager variables.

	// This line fixes linkers that don't look for main in the .LIBs.
	UI_APPLICATION::LinkMain();

	// Add the window to the window manager.
	*windowManager + new COMPORT_SETUP();

	// Process user responses.
	UI_APPLICATION::Control();

	return (0);
}

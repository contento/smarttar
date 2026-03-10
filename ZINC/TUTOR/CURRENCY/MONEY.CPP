//	MONEY.CPP (MONEY) - International currency class.
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

// NOTE: This is program makes use of international characters.  By default
//       the library uses the ISO8859-1 standard.  What characters appear
//       on the screen depend on the font used.  Fonts included in the 
//       libraries installed make use of ISO8859-1 standard.  Mapping is
//       provided for text mode for code page 850.  In order to have the 
//       proper characters appear in the text mode version of this program
//       you must have code page 850 on your machine.

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define USE_RAW_KEYS
#include <ui_win.hpp>

#include "money.hpp"
#if defined(_MSC_VER)
#	pragma hdrstop
#endif

#if defined (ZIL_MSDOS)
static UI_EVENT_MAP _eventTable[] =
{
	{ ID_WINDOW_OBJECT, 	L_SELECT,			E_KEY, 		ENTER },
	{ ID_WINDOW_OBJECT, 	L_SELECT,			E_KEY, 		GRAY_ENTER },
	{ ID_WINDOW_OBJECT, 	L_CANCEL,			E_KEY, 		ESCAPE },
	{ ID_WINDOW_OBJECT, 	L_VIEW,				E_MOUSE, 	0 },
	{ ID_WINDOW_OBJECT, 	L_BEGIN_SELECT,		E_MOUSE, 	M_LEFT | M_LEFT_CHANGE },
	{ ID_WINDOW_OBJECT, 	L_BEGIN_SELECT,		E_MOUSE, 	M_LEFT | M_RIGHT | M_LEFT_CHANGE | M_RIGHT_CHANGE },
	{ ID_WINDOW_OBJECT, 	L_CONTINUE_SELECT,	E_MOUSE, 	M_LEFT },
	{ ID_WINDOW_OBJECT, 	L_CONTINUE_SELECT,	E_MOUSE, 	M_LEFT | M_RIGHT },
	{ ID_WINDOW_OBJECT, 	L_END_SELECT,		E_MOUSE, 	M_LEFT_CHANGE },
	{ ID_WINDOW_OBJECT, 	L_END_SELECT,		E_MOUSE, 	M_RIGHT | M_LEFT_CHANGE },
	{ ID_WINDOW_OBJECT, 	L_UP,				E_KEY, 		GRAY_UP_ARROW },
	{ ID_WINDOW_OBJECT, 	L_UP,				E_KEY, 		WHITE_UP_ARROW },
	{ ID_WINDOW_OBJECT, 	L_DOWN,				E_KEY, 		GRAY_DOWN_ARROW },
	{ ID_WINDOW_OBJECT, 	L_DOWN,				E_KEY, 		WHITE_DOWN_ARROW },
	{ ID_WINDOW_OBJECT, 	L_LEFT,				E_KEY, 		GRAY_LEFT_ARROW },
	{ ID_WINDOW_OBJECT, 	L_LEFT,				E_KEY, 		WHITE_LEFT_ARROW },
	{ ID_WINDOW_OBJECT, 	L_RIGHT,			E_KEY, 		GRAY_RIGHT_ARROW },
	{ ID_WINDOW_OBJECT, 	L_RIGHT,			E_KEY, 		WHITE_RIGHT_ARROW },
	{ ID_WINDOW_OBJECT, 	L_PREVIOUS,			E_KEY, 		BACKTAB },
	{ ID_WINDOW_OBJECT, 	L_PREVIOUS,			E_KEY, 		SHIFT_F6 },
	{ ID_WINDOW_OBJECT, 	L_NEXT,				E_KEY, 		TAB },
	{ ID_WINDOW_OBJECT, 	L_NEXT,				E_KEY, 		F6 },
	{ ID_WINDOW_OBJECT,		L_HELP,				E_KEY, 		F1 },
	{ ID_WINDOW_OBJECT, 	L_HELP,				E_KEY, 		ALT_F1 },

	{ ID_STRING, 			L_MARK,				E_KEY,		CTRL_F5},
	{ ID_STRING, 			L_BEGIN_MARK,		E_MOUSE, 	M_LEFT | M_LEFT_CHANGE },
	{ ID_STRING, 			L_CONTINUE_MARK,	E_MOUSE, 	M_LEFT },
	{ ID_STRING, 			L_END_MARK,			E_MOUSE, 	M_LEFT_CHANGE },
	{ ID_STRING, 			L_CUT,				E_KEY,		CTRL_F6},
	{ ID_STRING, 			L_COPY_MARK,		E_KEY,		CTRL_F7},
	{ ID_STRING, 			L_PASTE,			E_KEY,		CTRL_F8},
	{ ID_STRING, 			L_BACKSPACE,		E_KEY,		BACKSPACE},
	{ ID_STRING, 			L_DELETE,			E_KEY,		WHITE_DELETE},
	{ ID_STRING, 			L_DELETE,			E_KEY,		GRAY_DELETE},
	{ ID_STRING, 			L_DELETE_EOL,		E_KEY,		CTRL_WHITE_DELETE},
	{ ID_STRING, 			L_DELETE_EOL,		E_KEY,		CTRL_GRAY_DELETE},
	{ ID_STRING, 			L_WORD_LEFT,		E_KEY,		CTRL_WHITE_LEFT_ARROW},
	{ ID_STRING, 			L_WORD_LEFT,		E_KEY,		CTRL_GRAY_LEFT_ARROW},
	{ ID_STRING, 			L_WORD_LEFT,		E_KEY,		ALT_GRAY_LEFT_ARROW},
	{ ID_STRING, 			L_WORD_RIGHT,		E_KEY,		CTRL_WHITE_RIGHT_ARROW},
	{ ID_STRING, 			L_WORD_RIGHT,		E_KEY,		CTRL_GRAY_RIGHT_ARROW},
	{ ID_STRING, 			L_WORD_RIGHT,		E_KEY,		ALT_GRAY_RIGHT_ARROW},
	{ ID_STRING, 			L_BOL,				E_KEY,		WHITE_HOME},
	{ ID_STRING, 			L_BOL,				E_KEY,		GRAY_HOME},
	{ ID_STRING, 			L_BOL,				E_KEY,		CTRL_WHITE_HOME},
	{ ID_STRING, 			L_BOL,				E_KEY,		CTRL_GRAY_HOME},
	{ ID_STRING, 			L_EOL,				E_KEY,		WHITE_END},
	{ ID_STRING, 			L_EOL,				E_KEY,		GRAY_END},
	{ ID_STRING, 			L_EOL,				E_KEY,		CTRL_WHITE_END},
	{ ID_STRING, 			L_EOL,				E_KEY,		CTRL_GRAY_END},
	{ ID_STRING, 			L_INSERT_TOGGLE,	E_KEY,		WHITE_INSERT},
	{ ID_STRING, 			L_INSERT_TOGGLE,	E_KEY,		GRAY_INSERT},
	
	{ ID_INTL_CURRENCY,		L_PGUP,				E_KEY,		GRAY_PGUP },
	{ ID_INTL_CURRENCY,		L_PGUP,				E_KEY,		WHITE_PGUP },
	{ ID_INTL_CURRENCY,		L_PGDN,				E_KEY,		GRAY_PGDN },
	{ ID_INTL_CURRENCY,		L_PGDN,				E_KEY,		WHITE_PGDN },

	// End of array.
	{ ID_END, 0, 0, 0 }
};
#elif defined(ZIL_MSWINDOWS)
static UI_EVENT_MAP _eventTable[] =
{
	{ ID_WINDOW_OBJECT, 	L_UP,				WM_KEYDOWN, 	GRAY_UP_ARROW },
	{ ID_WINDOW_OBJECT, 	L_UP,				WM_KEYDOWN, 	WHITE_UP_ARROW },
	{ ID_WINDOW_OBJECT, 	L_DOWN,				WM_KEYDOWN, 	GRAY_DOWN_ARROW },
	{ ID_WINDOW_OBJECT, 	L_DOWN,				WM_KEYDOWN, 	WHITE_DOWN_ARROW },
	{ ID_WINDOW_OBJECT, 	L_LEFT,				WM_KEYDOWN, 	GRAY_LEFT_ARROW },
	{ ID_WINDOW_OBJECT, 	L_LEFT,				WM_KEYDOWN, 	WHITE_LEFT_ARROW },
	{ ID_WINDOW_OBJECT, 	L_RIGHT,			WM_KEYDOWN, 	GRAY_RIGHT_ARROW },
	{ ID_WINDOW_OBJECT, 	L_RIGHT,			WM_KEYDOWN, 	WHITE_RIGHT_ARROW },
	{ ID_WINDOW_OBJECT, 	L_PREVIOUS,			WM_CHAR, 		BACKTAB },
	{ ID_WINDOW_OBJECT, 	L_NEXT,				WM_CHAR, 		TAB },
	{ ID_WINDOW_OBJECT, 	L_SELECT,			WM_CHAR, 		ENTER },
	{ ID_WINDOW_OBJECT,		L_HELP,				WM_KEYUP, 		F1 },
	{ ID_WINDOW_OBJECT, 	L_HELP,				WM_KEYUP, 		ALT_F1 },

	{ ID_STRING, 			L_BACKSPACE,		WM_KEYDOWN, 	BACKSPACE },
	{ ID_STRING, 			L_DELETE,			WM_KEYDOWN, 	GRAY_DELETE },
	{ ID_STRING, 			L_DELETE,			WM_KEYDOWN, 	WHITE_DELETE },
	{ ID_STRING, 			L_DELETE_EOL,		WM_KEYDOWN,		CTRL_WHITE_DELETE },
	{ ID_STRING, 			L_DELETE_EOL,		WM_KEYDOWN,		CTRL_GRAY_DELETE },

	{ ID_INTL_CURRENCY,		L_PGUP,				WM_KEYDOWN,		GRAY_PGUP },
	{ ID_INTL_CURRENCY,		L_PGUP,				WM_KEYDOWN,		WHITE_PGUP },
	{ ID_INTL_CURRENCY,		L_PGDN,				WM_KEYDOWN,		GRAY_PGDN },
	{ ID_INTL_CURRENCY,		L_PGDN,				WM_KEYDOWN,		WHITE_PGDN },

	// End of array.
	{ ID_END, 0, 0, 0 }
};
#elif defined(ZIL_OS2)
static UI_EVENT_MAP _eventTable[] =
{
	{ ID_WINDOW_OBJECT, 	L_UP,				WM_CHAR, 	GRAY_UP_ARROW },
	{ ID_WINDOW_OBJECT, 	L_UP,				WM_CHAR, 	WHITE_UP_ARROW },
	{ ID_WINDOW_OBJECT, 	L_DOWN,				WM_CHAR, 	GRAY_DOWN_ARROW },
	{ ID_WINDOW_OBJECT, 	L_DOWN,				WM_CHAR, 	WHITE_DOWN_ARROW },
	{ ID_WINDOW_OBJECT, 	L_LEFT,				WM_CHAR, 	GRAY_LEFT_ARROW },
	{ ID_WINDOW_OBJECT, 	L_LEFT,				WM_CHAR, 	WHITE_LEFT_ARROW },
	{ ID_WINDOW_OBJECT, 	L_RIGHT,			WM_CHAR, 	GRAY_RIGHT_ARROW },
	{ ID_WINDOW_OBJECT, 	L_RIGHT,			WM_CHAR, 	WHITE_RIGHT_ARROW },
	{ ID_WINDOW_OBJECT, 	L_PREVIOUS,			WM_CHAR, 	BACKTAB },
	{ ID_WINDOW_OBJECT, 	L_NEXT,				WM_CHAR, 	TAB },
	{ ID_WINDOW_OBJECT, 	L_SELECT,			WM_CHAR, 	ENTER },
	{ ID_WINDOW_OBJECT,		L_HELP,				WM_CHAR, 	F1 },
	{ ID_WINDOW_OBJECT, 	L_HELP,				WM_CHAR, 	ALT_F1 },

	{ ID_STRING, 			L_BACKSPACE,		WM_CHAR, 	BACKSPACE },
	{ ID_STRING, 			L_DELETE,			WM_CHAR, 	GRAY_DELETE },
	{ ID_STRING, 			L_DELETE,			WM_CHAR, 	WHITE_DELETE },
	{ ID_STRING, 			L_DELETE_EOL,		WM_CHAR,	CTRL_WHITE_DELETE },
	{ ID_STRING, 			L_DELETE_EOL,		WM_CHAR,	CTRL_GRAY_DELETE },

	{ ID_INTL_CURRENCY,		L_PGUP,				WM_CHAR,	GRAY_PGUP },
	{ ID_INTL_CURRENCY,		L_PGUP,				WM_CHAR,	WHITE_PGUP },
	{ ID_INTL_CURRENCY,		L_PGDN,				WM_CHAR,	GRAY_PGDN },
	{ ID_INTL_CURRENCY,		L_PGDN,				WM_CHAR,	WHITE_PGDN },

	// End of array.
	{ ID_END, 0, 0, 0 }
};
#elif defined(ZIL_MOTIF)
static UI_EVENT_MAP _eventTable[] =
{
	{ ID_WINDOW_OBJECT,		L_BEGIN_SELECT,		ButtonPress,	Button1 },
	{ ID_WINDOW_OBJECT,		L_CONTINUE_SELECT,	MotionNotify,	M_LEFT },
	{ ID_WINDOW_OBJECT,		L_END_SELECT,		ButtonRelease,	Button1 },
	{ ID_WINDOW_OBJECT,		L_BEGIN_ESCAPE,		ButtonPress,	Button3 },
	{ ID_WINDOW_OBJECT,		L_CONTINUE_ESCAPE,	MotionNotify,	M_RIGHT },
	{ ID_WINDOW_OBJECT,		L_END_ESCAPE,		ButtonRelease,	Button3 },
	{ ID_WINDOW_OBJECT,		L_SELECT,			KeyPress,		XK_Return },
	{ ID_WINDOW_OBJECT,		L_SELECT,			KeyPress,		XK_KP_Enter },
	{ ID_WINDOW_OBJECT,		S_SIZE,				ConfigureNotify,0 },
#if defined(__hpux)
	{ ID_WINDOW_OBJECT, 	L_UP,				KeyPress, 		osfXK_Up },
	{ ID_WINDOW_OBJECT, 	L_DOWN,				KeyPress, 		osfXK_Down },
	{ ID_WINDOW_OBJECT, 	L_LEFT,				KeyPress, 		osfXK_Left },
	{ ID_WINDOW_OBJECT, 	L_RIGHT,			KeyPress, 		osfXK_Right },
	{ ID_WINDOW_OBJECT, 	L_PGUP,				KeyPress, 		osfXK_PageUp },
	{ ID_WINDOW_OBJECT, 	L_PGDN,				KeyPress, 		osfXK_PageDown },
#endif
	{ ID_WINDOW_OBJECT, 	L_UP,				KeyPress, 		XK_Up },
	{ ID_WINDOW_OBJECT, 	L_DOWN,				KeyPress, 		XK_Down },
	{ ID_WINDOW_OBJECT, 	L_LEFT,				KeyPress, 		XK_Left },
	{ ID_WINDOW_OBJECT, 	L_RIGHT,			KeyPress, 		XK_Right },
	{ ID_WINDOW_OBJECT, 	L_PGUP,				KeyPress, 		XK_Prior },
	{ ID_WINDOW_OBJECT, 	L_PGDN,				KeyPress, 		XK_Next },

#if defined(__hpux)
	{ ID_INTL_CURRENCY, 	L_PGUP,				KeyPress, 		osfXK_PageUp },
	{ ID_INTL_CURRENCY, 	L_PGDN,				KeyPress, 		osfXK_PageDown },
#endif
	{ ID_INTL_CURRENCY, 	L_PGUP,				KeyPress, 		XK_Prior },
	{ ID_INTL_CURRENCY, 	L_PGDN,				KeyPress, 		XK_Next},

	// End of array.
	{ ID_END, 0, 0, 0 }
};
#endif

// The maximum langth of the UIW_INTL_CURRENCY object.
const int _maxLength = NUMBER_WHOLE + NUMBER_DECIMAL + 15;			// BUG.1991

// _exchangeRate is based on currency exchange rates on 8/12/92.
// $1 = DM .654000
// $1 = Pts .009850
// $1 = L 1.867700
// $1 = Y .007540

double _exchangeRate[MAX_COUNTRIES] =
{
	1.000000, 0.654000, 0.009850, 1.867700, 0.007540
};

// Country-specific currency formatting information.
countryInfo _currencyInfo[MAX_COUNTRIES] =
{
	// Chars are in the ISO8859-1 standard
	{ "$", ',', '.' },			// U.S. dollar
	{ "DM",	'.', ',' },			// German mark
	{ "Pt ", '.', ',' }, 		// Spanish peseta
	{ "\xA3", ',', '.' },		// UK pound
	{ "\xA5", ',', '.' }, 		// Japanese yen
};				 

UIW_INTL_CURRENCY::UIW_INTL_CURRENCY(int left, int top, int width,
	UI_BIGNUM *_number, NMF_FLAGS _nmFlags, WOF_FLAGS woFlags,
	USER_FUNCTION userFunction, int _countrySetting) : UIW_STRING(left,
	top, width, NULL, _maxLength, STF_NO_FLAGS, woFlags, userFunction),
	countryTableEntry(_countrySetting), number(NULL), nmFlags(_nmFlags)
{
	UIW_INTL_CURRENCY::Information(INITIALIZE_CLASS, NULL);
	UIW_INTL_CURRENCY::DataSet(_number);
	eventMapTable = _eventTable;
}

EVENT_TYPE UIW_INTL_CURRENCY::Event(const UI_EVENT &event)
{
	EVENT_TYPE ccode = LogicalEvent(event, ID_INTL_CURRENCY);
	switch (ccode)
	{
	case L_SELECT:
	case S_NON_CURRENT:
		if (!FlagSet(woStatus, WOS_INTERNAL_ACTION))
		{
			UI_BIGNUM tNumber = *number;		// Keep a temporary number in case of error.
			DataGet();

			// Update text.
			char newString[NUMBER_WHOLE+NUMBER_DECIMAL+15];
			strcpy(newString, text);

			ccode = UIW_STRING::Event(event);	// Call the user or validate function.
			if (ccode == -1)
				*number = tNumber;				// An error occurred, restore the old number.
			else
				woStatus &= ~WOS_UNANSWERED;
			number->Export(text, nmFlags);
			DataSet(number);
		}
		else
			ccode = UIW_STRING::Event(event);
		break;

	case L_PGUP:
		{
			int countryTableEntry = GetCountryCode();
			countryTableEntry--;
			if (countryTableEntry == -1)
				countryTableEntry = MAX_COUNTRIES-1;
			SetCountryCode(countryTableEntry);
		}
		break;

	case L_PGDN:
		{
			int countryTableEntry = GetCountryCode();
			countryTableEntry++;
			if (countryTableEntry == MAX_COUNTRIES)
				countryTableEntry = 0;
			SetCountryCode(countryTableEntry);
		}
		break;

	case L_NEW_VALUE:
		{
		rbignum value = 0;
		((UI_BIGNUM *)event.data)->Export(&value);

		// Convert from US to current country.
		value /= _exchangeRate[countryTableEntry];

		number->Import(value);
		DataSet(number);
		}
		break;

	case L_TRANSLATE_TO_US:
		SetCountryCode(ENTRY_US);
		break;

	case L_TRANSLATE_TO_GERMAN:
		SetCountryCode(ENTRY_GERMAN);
		break;

	case L_TRANSLATE_TO_SPANISH:
		SetCountryCode(ENTRY_SPANISH);
		break;

	case L_TRANSLATE_TO_BRITISH:
		SetCountryCode(ENTRY_BRITISH);
		break;

	case L_TRANSLATE_TO_JAPANESE:
		SetCountryCode(ENTRY_JAPANESE);
		break;

	default:
		ccode = UIW_STRING::Event(event);
		break;
	}
	return ccode;
}

void UIW_INTL_CURRENCY::ConvertToSystemSettings(char *string)
{
	char newString[NUMBER_WHOLE+NUMBER_DECIMAL+15];
	strcpy(newString, UI_INTERNATIONAL::currencySymbol);

	char *ch = string;
	int i = strlen(newString);
	int j;
	while(*ch != '\0')
	{
//		if (isdigit(*ch) || *ch == '-')										// BUG.2043
		if ((*ch >= '0' && *ch <= '9') || *ch == '-')
			newString[i++] = *ch;
		else if (*ch == _currencyInfo[countryTableEntry].decimalSeparator)
			for (j = 0; UI_INTERNATIONAL::decimalSeparator[j] != '\0'; j++)
				newString[i++] = UI_INTERNATIONAL::decimalSeparator[j];
		else if (*ch == _currencyInfo[countryTableEntry].thousandsSeparator)
			for (j = 0; UI_INTERNATIONAL::thousandsSeparator[j] != '\0'; j++)
				newString[i++] = UI_INTERNATIONAL::thousandsSeparator[j];
		ch++;
	}
	newString[i++] = '\0';

	strcpy(string, newString);
}

void UIW_INTL_CURRENCY::ConvertToLocalSettings(char *string)
{
	char newString[NUMBER_WHOLE+NUMBER_DECIMAL+15];
	strcpy(newString, _currencyInfo[countryTableEntry].currencySymbol);

	char *ch = string;
	int i = strlen(newString);
	while(*ch != '\0')
	{
//		if (isdigit(*ch) || *ch == '-')										// BUG.2043
		if ((*ch >= '0' && *ch <= '9') || *ch == '-')
			newString[i++] = *ch;
		else if (*ch == UI_INTERNATIONAL::decimalSeparator[0])
			newString[i++] = _currencyInfo[countryTableEntry].decimalSeparator;
		else if (*ch == UI_INTERNATIONAL::thousandsSeparator[0])
			newString[i++] = _currencyInfo[countryTableEntry].thousandsSeparator;
		ch++;
	}
	newString[i++] = '\0';

	strcpy(string, newString);
}

UI_BIGNUM *UIW_INTL_CURRENCY::DataGet(void)
{
	// Return the number.
	UIW_STRING::DataGet();

	// Convert to current country info.
	char newString[NUMBER_WHOLE+NUMBER_DECIMAL+15];
	strcpy(newString, text);
	ConvertToSystemSettings(newString);

	number->Import(newString);
	return (number);
}

void UIW_INTL_CURRENCY::DataSet(UI_BIGNUM *_number)
{
	// Reset the number.
	if (number == _number || FlagSet(woFlags, WOF_NO_ALLOCATE_DATA))
		number = _number;
	else if (_number)
		number = new UI_BIGNUM(*_number);
	else
		number = new UI_BIGNUM(0L);

	*number = round(*number, 2);

	// Get the text associated with the number.
	number->Export(text, nmFlags | NMF_COMMAS);

	char newString[NUMBER_WHOLE+NUMBER_DECIMAL+15];
	strcpy(newString, text);
	ConvertToLocalSettings(newString);
	strcpy(text, newString);

	UIW_STRING::DataSet(text);
}

void UIW_INTL_CURRENCY::SetCountryCode(int _countryEntry)
{
	// Do Currency translation.
	UI_BIGNUM *ammount = DataGet();
	rbignum value = 0;
	ammount->Export(&value);

	// Convert from US to current country.
	value *= _exchangeRate[countryTableEntry];
	value /= _exchangeRate[_countryEntry];

	ammount->Import(value);

	countryTableEntry = _countryEntry;
	DataSet(ammount);
}

int UIW_INTL_CURRENCY::GetCountryCode()
{
	return countryTableEntry;
}

void *UIW_INTL_CURRENCY::Information(INFO_REQUEST request, void *data, OBJECTID objectID)
{
	// Switch on the request.
	if (!objectID) objectID = ID_INTL_CURRENCY;
	switch (request)
	{
	case INITIALIZE_CLASS:
		searchID = windowID[0] = ID_INTL_CURRENCY;
		windowID[1] = ID_NUMBER;
		windowID[2] = ID_STRING;
		break;

	case CHANGED_FLAGS:
		// Check the object and base class flag settings.
		if (request == CHANGED_FLAGS)
			UIW_STRING::Information(CHANGED_FLAGS, data, ID_INTL_CURRENCY);
		// See if the field needs to be re-computed.
		if (screenID && request == CHANGED_FLAGS && objectID == ID_INTL_CURRENCY)
			Event(UI_EVENT(S_CREATE, 0));
		break;

	case GET_FLAGS:
	case SET_FLAGS:
	case CLEAR_FLAGS:
		if (objectID && objectID != ID_INTL_CURRENCY)
			data = UIW_STRING::Information(request, data, objectID);
		else if (request == GET_FLAGS && !data)
			data = &nmFlags;
		else if (request == GET_FLAGS)
			*(NMF_FLAGS *)data = nmFlags;
		else if (request == SET_FLAGS)
			nmFlags |= *(NMF_FLAGS *)data;
		else
			nmFlags &= ~(*(NMF_FLAGS *)data);
		break;

	default:
		data = UIW_STRING::Information(request, data, objectID);
		break;
	}

	// Return the information.
	return (data);
}

static EVENT_TYPE UpdateCurrency(UI_WINDOW_OBJECT *object, UI_EVENT &,
	EVENT_TYPE ccode)
{
	if ((ccode == L_SELECT) || (ccode == S_NON_CURRENT))
	{
		// Get a pointer to the currency field object.
		UIW_INTL_CURRENCY *currencyField = (UIW_INTL_CURRENCY *)object;

		// Get a pointer to the parent window.
		for (UI_WINDOW_OBJECT *parentWindow = object->parent;
			parentWindow->parent; parentWindow = parentWindow->parent)
			;

		// Get the value of the currency string.
		UI_BIGNUM newValue(*currencyField->DataGet());
		rbignum value = 0;
		newValue.Export(&value);

		// Do conversion to US dollars.
		int currentCountry = currencyField->GetCountryCode();
		value *= _exchangeRate[currentCountry];
		newValue.Import(value);

		// Set up the event.
		UI_EVENT tEvent(L_NEW_VALUE);
		tEvent.data = (void *) &newValue;

		for (UI_WINDOW_OBJECT *number = ((UIW_WINDOW *)parentWindow)->First();
			number; number = number->Next())
		{
			if ((number != object) && (object->Inherited(ID_INTL_CURRENCY)))
				number->Event(tEvent);
		}
	}

	return ccode;
}

int UI_APPLICATION::Main(void)
{
	// The UI_APPLICATION constructor automatically initializes the 
	// display, eventManager, and windowManager variables.

	// This line fixes linkers that don't look for main in the .LIBs.
	UI_APPLICATION::LinkMain();

	UIW_WINDOW *window = UIW_WINDOW::Generic(0, 0, 72, 10,
		"International Currency");

	rbignum value = 100;
	UI_BIGNUM *value1 = new UI_BIGNUM(value / _exchangeRate[ENTRY_US]);
	UI_BIGNUM *value2 = new UI_BIGNUM(value / _exchangeRate[ENTRY_GERMAN]);
	UI_BIGNUM *value3 = new UI_BIGNUM(value / _exchangeRate[ENTRY_JAPANESE]);
	UI_BIGNUM *value4 = new UI_BIGNUM(value / _exchangeRate[ENTRY_SPANISH]);
	UI_BIGNUM *value5 = new UI_BIGNUM(value / _exchangeRate[ENTRY_BRITISH]);
	UI_BIGNUM *value6 = new UI_BIGNUM(value / _exchangeRate[ENTRY_US]);
 
	*window
		+ new UIW_PROMPT(1, 1, "Enter an amount:")
		+ new UIW_INTL_CURRENCY(1, 2, 20, value1,
			NMF_CURRENCY | NMF_DECIMAL(2), WOF_BORDER | WOF_AUTO_CLEAR,
			UpdateCurrency)
		+ new UIW_PROMPT(25, 1, "The exchange value is:")
		+ new UIW_PROMPT(25, 2, "Germany:")
		+ new UIW_INTL_CURRENCY(43, 2, 25, value2,
			NMF_CURRENCY | NMF_DECIMAL(2), WOF_BORDER | WOF_NON_SELECTABLE,
			NULL, ENTRY_GERMAN)
		+ new UIW_PROMPT(25, 3, "Japan:")
		+ new UIW_INTL_CURRENCY(43, 3, 25, value3,
			NMF_CURRENCY | NMF_DECIMAL(2), WOF_BORDER | WOF_NON_SELECTABLE,
			NULL, ENTRY_JAPANESE)
		+ new UIW_PROMPT(25, 4, "Spain:")
		+ new UIW_INTL_CURRENCY(43, 4, 25, value4,
			NMF_CURRENCY | NMF_DECIMAL(2), WOF_BORDER | WOF_NON_SELECTABLE,
			NULL, ENTRY_SPANISH)
		+ new UIW_PROMPT(25, 5, "United Kingdom:")
		+ new UIW_INTL_CURRENCY(43, 5, 25, value5,
			NMF_CURRENCY | NMF_DECIMAL(2), WOF_BORDER | WOF_NON_SELECTABLE,
			NULL, ENTRY_BRITISH)
		+ new UIW_PROMPT(25, 6, "United States:")
		+ new UIW_INTL_CURRENCY(43, 6, 25, value6,
			NMF_CURRENCY | NMF_DECIMAL(2), WOF_BORDER | WOF_NON_SELECTABLE,
			NULL, ENTRY_US);

	*windowManager + window;

	// Process user responses.
	UI_APPLICATION::Control();

	return (0);
}

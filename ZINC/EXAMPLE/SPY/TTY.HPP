//	TTY.HPP (SPY) - Output window.
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA
//  May be freely copied, used and distributed.

// This MUST be greater than the length of the formatted string sent to ttyWindow->Printf() !!!!!
const int MAX_TTY_STRING_LENGTH = 80;

class TTY_ELEMENT : public UIW_STRING
{
	friend class TTY_WINDOW;
public:
	void DataSet(TTY_ELEMENT *element) { UIW_STRING::DataSet((char *)element->DataGet()); }
	void DataSet(char *string) { UIW_STRING::DataSet(string); }
	virtual EVENT_TYPE Event(const UI_EVENT &event);
	TTY_ELEMENT *Next(void) { return (TTY_ELEMENT *)next; }
	TTY_ELEMENT *Previous(void) { return (TTY_ELEMENT *)previous; }

private:
	int height;

	TTY_ELEMENT(int left, int top, int width, int _height, int length);
};

class TTY_WINDOW : public UIW_WINDOW
{
public:
	TTY_WINDOW(void) :
		UIW_WINDOW(0, 0, 0, 0, WOF_NON_FIELD_REGION), numberShowing(0) { }
	~TTY_WINDOW(void) { }

	virtual EVENT_TYPE Event(const UI_EVENT &event);

	TTY_ELEMENT *Current(void) { return (TTY_ELEMENT *)current; }
	TTY_ELEMENT *First(void) { return (TTY_ELEMENT *)first; }
	TTY_ELEMENT *Last(void) { return (TTY_ELEMENT *)last; }

	void Printf(char *format, ...);

private:
	int numberShowing;
};

//	VLIST.HPP (VLIST) - This file contains the virtual list class definitions.
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA
//  May be freely copied, used and distributed.

class VIRTUAL_ELEMENT : public UIW_STRING
{
	friend class VIRTUAL_LIST;
public:
	int recordNumber;

	void DataSet(VIRTUAL_ELEMENT *element)
		{ UIW_STRING::DataSet(element->DataGet()); recordNumber = element->recordNumber; }
	void DataSet(int _recordNumber, char *string)
		{ UIW_STRING::DataSet(string); recordNumber = _recordNumber; }
	virtual EVENT_TYPE Event(const UI_EVENT &event);
	VIRTUAL_ELEMENT *Next(void) { return (VIRTUAL_ELEMENT *)next; }
	VIRTUAL_ELEMENT *Previous(void) { return (VIRTUAL_ELEMENT *)previous; }

private:
	int height;

	VIRTUAL_ELEMENT(int left, int top, int width, int _height, int length) :
//		UIW_STRING(left, top, width, "", length, STF_NO_FLAGS, WOF_NO_FLAGS), height(_height) { }
		UIW_STRING(left, top, width, "", length, STF_NO_FLAGS, WOF_VIEW_ONLY), height(_height) { } // BUG.7029
};

class VIRTUAL_LIST : public UIW_WINDOW
{
public:
	VIRTUAL_LIST(const char *fileName, int _recordLength);
	~VIRTUAL_LIST(void);

	virtual EVENT_TYPE Event(const UI_EVENT &event);

	VIRTUAL_ELEMENT *Current(void) { return (VIRTUAL_ELEMENT *)current; }
	VIRTUAL_ELEMENT *First(void) { return (VIRTUAL_ELEMENT *)first; }
	VIRTUAL_ELEMENT *Last(void) { return (VIRTUAL_ELEMENT *)last; }

	void LoadRecord(VIRTUAL_ELEMENT *element, int recordNumber);

private:
	FILE *file;
	const int recordLength;
	int numberOfRecords;
	int topRecordShowing;
	int numberShowing;
};

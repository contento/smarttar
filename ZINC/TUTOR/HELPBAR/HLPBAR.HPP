//	HLPBAR.HPP (HELPBAR) - This file contains the help bar class definition.
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA
//  May be freely copied, used and distributed.

// Help bar objectID.
const OBJECTID ID_HELP_BAR			= 3005;

class HELP_BAR : public UI_WINDOW_OBJECT
{
public:
	HELP_BAR(char *text = NULL);
	~HELP_BAR(void);
	virtual EVENT_TYPE Event(const UI_EVENT &event);

	virtual void *Information(INFO_REQUEST request, void *data, OBJECTID objectID = 0);
	static UI_WINDOW_OBJECT *New(const char *, UI_STORAGE *, UI_STORAGE_OBJECT *)
	{ return new HELP_BAR(); }
	virtual void Store(const char *, UI_STORAGE *, UI_STORAGE_OBJECT *)
	{}

protected:
	char *text;

	virtual EVENT_TYPE DrawItem(const UI_EVENT &event, EVENT_TYPE ccode);
};

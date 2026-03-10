#ifndef __DB_VIEW_H
#define __DB_VIEW_H

#if !defined(UI_WIN_HPP)
#include <ui_win.hpp>
#endif

#if !defined(__ST_DEFS_H)
#include <st_defs.h>
#endif

class EXPORT DBView : public UIW_WINDOW
{
public:
	DBView(BOOL bFromTurn = FALSE);
	~DBView();

	virtual EVENT_TYPE Event(const UI_EVENT &event);

private:
	static EVENT_TYPE ProcessTurn(UI_WINDOW_OBJECT *object, UI_EVENT &event, EVENT_TYPE ccode);
	static EVENT_TYPE ProcessNumber(UI_WINDOW_OBJECT *object, UI_EVENT &event, EVENT_TYPE ccode);
	static EVENT_TYPE ProcessNumbers(UI_WINDOW_OBJECT *object, UI_EVENT &event, EVENT_TYPE ccode);

	void ShowRecord(long nNumber);

private:
	UIW_DATE	*m_pwDate;
	UIW_INTEGER *m_pwTurn;
	UIW_BIGNUM  *m_pwNumber;
	UIW_VT_LIST *m_pwNumbers;

	BOOL 		m_bFromTurn;
	BOOL		m_bArchive;
};

#endif // __DB_VIEW_H

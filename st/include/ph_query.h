#ifndef __PH_QUERY
#define __PH_QUERY

#if !defined(UI_WIN_HPP)
#include <ui_win.hpp>
#endif

#if !defined(__ST_DEFS_H)
#include <st_defs.h>
#endif

class EXPORT PhoneQueryWindow : public UIW_WINDOW
{
public:
	PhoneQueryWindow();
	virtual EVENT_TYPE Event(const UI_EVENT &event);

private:
	static EVENT_TYPE ProcessSource(UI_WINDOW_OBJECT *object, UI_EVENT &event, EVENT_TYPE ccode);
	static EVENT_TYPE ProcessPlaces(UI_WINDOW_OBJECT *object, UI_EVENT &event, EVENT_TYPE ccode);
	static EVENT_TYPE ProcessPlace (UI_WINDOW_OBJECT *object, UI_EVENT &event, EVENT_TYPE ccode);
	static EVENT_TYPE ProcessTime  (UI_WINDOW_OBJECT *object, UI_EVENT &event, EVENT_TYPE ccode);

	BOOL Recalc();

private:
	UIW_VT_LIST *m_pwPlaces;
	UIW_STRING  *m_pwPlace;
	UIW_STRING  *m_pwCurrentPlace;
	UIW_BIGNUM  *m_pwCost;
	UIW_BIGNUM  *m_pwTime;
	UIW_BIGNUM  *m_pwTotal;
};

#endif // __PH_QUERY

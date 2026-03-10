#ifndef __PLAIN_PR_H
#define __PLAIN_PR_H

#if !defined(UI_WIN_HPP)
#include <ui_win.hpp>
#endif

#if !defined(__DSTORAGE_H)
#include <dstorage.h>
#endif

class EXPORT PLAIN_PRINTOUT : public UIW_WINDOW
{
public:
	PLAIN_PRINTOUT
	(
		DWORD from,
		DWORD numOfReceipts,
		SHORT boothNum = -1,
		BOOL fromExt = FALSE,
		DWORD virtualFrom = 0
	);
	virtual EVENT_TYPE Event(const UI_EVENT &event);
private:
    DWORD From;
    DWORD NumOfReceipts;
    WORD  BoothNum;
    BOOL  FromExt;
    DWORD VirtualFrom;
    DWORD Count;
    //
    void PrintConfig(void);
    void PrintHeader(void);
	void PrintData(Receipt& receipt);
	void PrintFX(Receipt& receipt);
	void PrintFXTel(Receipt& receipt);
	void PrintFXSTel(Receipt& receipt);
	void PrintFXFax(Receipt& receipt);
	void PrintFXTelex(Receipt& receipt);
	void PrintFXMCard(Receipt& receipt);
	void PrintFXOther(Receipt& receipt);
	void PrintTM(Receipt& receipt);
	void PrintTMTel(Receipt& receipt);
	void PrintTMSTel(Receipt& receipt);
	void PrintTMFax(Receipt& receipt);
	void PrintTMTelex(Receipt& receipt);
	void PrintTMMCard(Receipt& receipt);
	void PrintTMOther(Receipt& receipt);
    void PrintFF(void);
};

#endif // __PLAIN_PR_H
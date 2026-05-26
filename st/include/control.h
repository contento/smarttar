#ifndef __CONTROL_H
#define __CONTROL_H

#if !defined(UI_ENV_HPP)
#include <ui_env.hpp>
#endif

#if !defined(__PH_ENG_H)
#include <ph_eng.h>
#endif

#if !defined(__RT_ENG_H)
#include <rt_eng.h>
#endif

#if !defined(__VIEW_H)
#include <view.h>
#endif

#if !defined(__BDISPLAY_H)
#include <bdisplay.h>
#endif

#if !defined(__UTIL__) && !defined(__TEST__) // v.220 __TEST__
#if !defined(__PRN_FMT_H)
#include <prn_fmt.h>
#endif
#endif

class CONTROLLER : public UI_DEVICE
{
    friend UIW_MANUAL;  // access to Receipts
	friend DB_ENGINE;   // access to Receipts

public:

	CONTROLLER(UI_EVENT_MANAGER *eventManager, UI_WINDOW_MANAGER *windowManager);
	~CONTROLLER();

	enum RECEIPT_TAG { RECEIPT_ON = 0x01, ACCUM_ON = 0x02 };

	static BOOL RTEngineIsBusy(void);
	static BOOL RTEngineIsBoothBusy(WORD cNum, WORD bNum);

	static WORD RTEngineGetNumOfCalls(WORD cNum, WORD bNum);

	static void RTEngineSetPreValue(WORD cNum, WORD bNum, double value);
	static double RTEngineGetPreValue(WORD cNum, WORD bNum);

	static void RTEngineSetFirstPreValue(WORD cNum, WORD bNum, double value);
	static double RTEngineGetFirstPreValue(WORD cNum, WORD bNum);

	static void RTEngineSetPrePaid(WORD cNum, WORD bNum, BOOL value);
	static BOOL RTEngineGetPrePaid(WORD cNum, WORD bNum);

	static DWORD  RTEngineGetPreTime(WORD cNum, WORD bNum);
	static void   RTEngineSetPreTime(WORD cNum, WORD bNum, DWORD time);

	void RefreshView(void);
	void RefreshBoothDisplay(void);

private:
	static ENGINE       			   *RTEngine;
	static CIRCULAR_QUEUE<DynamicReceipt> *PRNReceipts;
	static CIRCULAR_QUEUE<DynamicReceipt> *Receipts;

	struct ManualInfoItem
	{ // v.2.20.2 booth display info
		WORD   numOfCalls;
		double totalCost;
	};
	static ManualInfoItem *manualInfo;
	UI_EVENT_MANAGER *EventManager;
	static UIW_VIEW *View;

	BoothDisplay *boothDisplay; // v2.18
	BoothDisplay::Info *m_pDisplayInfos; // 2.21.1 Build 5

	BOOL CashReq;
	BOOL (*Alarms)[CLUSTER_SIZE];
	BOOL Refresh;
	BOOL watchDog;
	char watchDogMessage[80];
#if !defined(__UTIL__) && !defined(__TEST__) // v.220 __TEST__
    PrnFormatter *prnFormatter;
#endif
    //
    EVENT_TYPE  Event(const UI_EVENT &event);
    void        Poll(void);
    //
    void UnloadRTEngine (void);
    void ProcessReceipts(void);
    void ProcessExtensions(void);

	void ProcessPrepaid();
	void UpdateStatusBar(void);
    //
    void cookReceipts(void);
	void CookReceipt(DynamicReceipt& receipt);
    //
    void CookViewData       (void);
    void cookViewPhoneInfo  (int cNum, int bNum);
	void cookViewElapsedTime(int cNum, int bNum);
	void checkWatchDog(int cNum, int bNum);
    void cookArea     (int cNum, int bNum, CALL_ACCESS_HEADER accessHeader);
    void cookTariff   (int cNum, int bNum, WORD tariffNum);
    void cookCity     (int cNum, int bNum, CITY_NAME *city);
    void cookCallType (int cNum, int bNum, UINT callType);

	inline void refreshStat (WORD cNum, WORD bNum);
    inline void refreshArea (WORD cNum, WORD bNum);
    inline void refreshPhone(WORD cNum, WORD bNum);
    inline void refreshCity (WORD cNum, WORD bNum);
    inline void refreshTar  (WORD cNum, WORD bNum);
    inline void refreshETime(WORD cNum, WORD bNum);
    inline void refreshValue(WORD cNum, WORD bNum);
    inline void refreshCalls(WORD cNum, WORD bNum);
    //
	BOOL PrintReceipt(DynamicReceipt& receipt);
    //
	BOOL PrintNReceipt  (DynamicReceipt& receipt);
	void PrintNLINEAL_80(DynamicReceipt& receipt, const char *fmt);

	void PrintRem       (DynamicReceipt& receipt);
	void PrintNData     (DynamicReceipt& receipt);
	void PrintSummary   (DynamicReceipt& receipt);

	BOOL PrintSReceipt (DynamicReceipt& receipt);
	void PrintSTelData (DynamicReceipt& receipt);
	void PrintFaxData  (DynamicReceipt& receipt);
	void PrintTelexData(DynamicReceipt& receipt);
	void PrintMagneticCardData(DynamicReceipt& receipt);
	void PrintOtherData(DynamicReceipt& receipt);
    //
	void PrintSTelLINEAL_80 (DynamicReceipt& receipt, const char *fmt);
	void PrintFaxLINEAL_80  (DynamicReceipt& receipt, const char *fmt);
	void PrintTelexLINEAL_80(DynamicReceipt& receipt, const char *fmt);
	void PrintMagneticCardLINEAL_80(DynamicReceipt& receipt, const char *fmt);
	void PrintOtherLINEAL_80(DynamicReceipt& receipt, const char *fmt);
    //
    void displayStatistics(void *window, WORD type, BOOL fromTurn = TRUE);
    void printStatistics(WORD type, BOOL fromTurn = TRUE);
    void printStatData(WORD type, BOOL fromTurn = TRUE);
    void printPrePaidReceipt(WORD boothCount, double value);
    void printPrePaidData(WORD boothCount, double value);
    //
    void PrintConfig(BYTE channel, BOOL fromStatistics = FALSE);
	void PrintHeader(DynamicReceipt& receipt, BOOL fromStatistics = FALSE);
	void PrintShortHeader(DynamicReceipt& receipt);
    void PrintFooter(BYTE channel, BOOL fromStatistics = FALSE);
    void PrintFF(BYTE channel, BOOL fromStatistics = FALSE); // go to the next begin of page
    void PrintLF(BYTE channel); // go to the next but not necesarily begin of page
    void printLog(void);
    //
    void activateCash(UI_TIME& lastTime, WORD lastHundreth, WORD currentHundreth);
#ifdef DOSX286
    // the new ISR to catch exceptions
    static void interrupt far NewGPFHandler(EXCEP_FRAME eFrame);
    static PEHANDLER OldGPFHandler;
    static void 		 NewHandler(void);

#if !defined(__DEMO__)
	static void Dump();
	static void ReplaceDump();
#endif // #if !defined(__DEMO__)
#endif
};

#endif // __CONTROL_H

#ifndef __VIEW_H
#define __VIEW_H

#if !defined(UI_WIN_HPP)
#define USE_RAW_KEYS
#include <ui_win.hpp>
#endif

#if !defined(__PH_ENG_H)
#include <ph_eng.h>
#endif

#if !defined(__ST_DEFS_H)
#include <st_defs.h>
#endif

#if !defined (__W_STATBR_H)
#include <w_statbr.h>
#endif

#if !defined(__W_SGROUP_H)
#include <w_sgroup.h>
#endif

class EXPORT UIW_VIEW : public UIW_WINDOW
{
public:
	UIW_VIEW(int numOfClusters=1, int numOfGroups=9);
	~UIW_VIEW();

	void GetDumpData(void * & ptr, int &size);
	void SetDumpData(void *   ptr, int  size);

	void resetData(UINT clusterNum, UINT pIBooth);
	virtual EVENT_TYPE Event(const UI_EVENT &pEvent);
private:
	friend CONTROLLER;

	struct STATE
	{
		char  *BitmapName;
		char  *Text;
		UCHAR *Bitmap;
	};
	static	STATE      *States;
	static  HELP_INFO  *HelpInfo;

	struct REGION
	{
		int X;
		int Y;
		int W;
		int H;
	};

	UIW_INTEGER        *WBNumber;

	UIW_PULL_DOWN_ITEM *WFileMenu;
	UIW_PULL_DOWN_ITEM *WConfigMenu;
	UIW_POP_UP_ITEM    *WChangePasswd;
	UIW_POP_UP_ITEM    *WActivateConfig;
	UIW_POP_UP_ITEM    *WActivateExt;
	UIW_PULL_DOWN_ITEM *WInfoMenu;
	UIW_PULL_DOWN_ITEM *WExtMenu;
	UIW_PULL_DOWN_ITEM *WPrintMenu;
	UIW_PULL_DOWN_ITEM *WHelpMenu;

	// ---- config data
	int NumOfBooths;
	int NumOfGroups;
	int NumOfClusters;
	int BoothNumber; // manual
	// ---- Window Objects
	UIW_PULL_DOWN_MENU *WMenu;
	UIW_TOOL_BAR       *WToolBar;
	UIW_STAT_BAR       *WStatBar;

	UIW_BUTTON   *(*WBoothNumbers)[CLUSTER_SIZE];
	UIW_BUTTON   *(*WStates)[CLUSTER_SIZE];
	UIW_STRING   *(*WPhones)[CLUSTER_SIZE];
	UIW_STRING   *(*WCities)[CLUSTER_SIZE];
	UIW_STRING   *(*WAreas)[CLUSTER_SIZE];
	UIW_REAL     *(*WElapsedTimes)[CLUSTER_SIZE];
	UIW_INTEGER  *(*WTariffs)[CLUSTER_SIZE];
	UIW_BIGNUM   *(*WValues)[CLUSTER_SIZE];
	UIW_INTEGER  *(*WNumOfCalls)[CLUSTER_SIZE];

	// ---- items to store values ...
	int (*ToneFSs)[CLUSTER_SIZE];
	int (*PulseFSs)[CLUSTER_SIZE];

	PH_ENGINE::CallInfo (*m_callInfo)[CLUSTER_SIZE];

	void loadStatBMP   (void);
	void addStatBar    (void);
	void addMenu       (void);
	void addToolbar    (void);
	void initPos       (REGION *groupRegions);
	void addTitleGroups(REGION *groupRegions);
	void addTable      (REGION *groupRegions);
	BOOL NormalizeRowPitch(void);      // Zinc 3.5 minicell rounding: row-tear fix
	BOOL NormalizeColumnBorders(void); // Zinc 3.5 minicell rounding: thick-line fix
	UI_WINDOW_OBJECT *colCell(int c, int b, int k); // booth cell by column index
	//
	static EVENT_TYPE processBooth(UI_WINDOW_OBJECT *object, UI_EVENT &, EVENT_TYPE ccode);
};

#endif // __VIEW_H

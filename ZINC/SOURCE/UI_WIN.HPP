//	Zinc Interface Library - UI_WIN.HPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#if !defined(UI_WIN_HPP)
#	define UI_WIN_HPP
#	if !defined(UI_EVT_HPP)
#		if defined(__OS2__)
#			if !defined(INCL_WINSYS)
#				define INCL_WINSYS
#			endif
#			if !defined(INCL_WINFRAMEMGR)
#				define INCL_WINFRAMEMGR
#			endif
#			if !defined(INCL_WINWINDOWMGR)
#				define INCL_WINWINDOWMGR
#			endif
#		endif
#		include <ui_evt.hpp>
#	endif

// --- Version 2.0 and 1.0 compatibility ---
#define UIW_NUMBER				UIW_BIGNUM
#define UI_HELP_WINDOW_SYSTEM	UI_HELP_SYSTEM
#define UI_ERROR_WINDOW_SYSTEM	UI_ERROR_SYSTEM
#define _helpSystem				UI_WINDOW_OBJECT::helpSystem
#define _errorSystem			UI_WINDOW_OBJECT::errorSystem

// --- numberID ---
typedef USHORT NUMBERID;
const NUMBERID NUMID_BORDER				= 0xFFFF;
const NUMBERID NUMID_MAXIMIZE			= 0xFFFE;
const NUMBERID NUMID_MINIMIZE			= 0xFFFD;
const NUMBERID NUMID_SYSTEM				= 0xFFFC;
const NUMBERID NUMID_TITLE				= 0xFFFB;

const NUMBERID NUMID_OPT_RESTORE		= 0xFFEF;
const NUMBERID NUMID_OPT_MOVE			= 0xFFEE;
const NUMBERID NUMID_OPT_SIZE			= 0xFFED;
const NUMBERID NUMID_OPT_MINIMIZE		= 0xFFEC;
const NUMBERID NUMID_OPT_MAXIMIZE		= 0xFFEB;
const NUMBERID NUMID_OPT_CLOSE			= 0xFFEA;
const NUMBERID NUMID_OPT_SWITCH			= 0xFFE9;

// ----- UI_ITEM ------------------------------------------------------------

struct EXPORT UI_ITEM
{
	EVENT_TYPE value;
	void *data;
	char *text;
	UIF_FLAGS flags;
};

// ----- UI_PALETTE_MAP -----------------------------------------------------

typedef LOGICAL_EVENT LOGICAL_PALETTE;
const LOGICAL_PALETTE PM_ANY			= 0x0000;
const LOGICAL_PALETTE PM_ACTIVE			= 0x0001;
const LOGICAL_PALETTE PM_INACTIVE		= 0x0002;
const LOGICAL_PALETTE PM_CURRENT		= 0x0004;
const LOGICAL_PALETTE PM_SELECTED		= 0x0008;
const LOGICAL_PALETTE PM_NON_SELECTABLE	= 0x0010;
const LOGICAL_PALETTE PM_HOT_KEY		= 0x0020;
const LOGICAL_PALETTE PM_SPECIAL		= 0x0040;	// Special mode palettes (WOAF_DIALOG_OBJECT border)

struct EXPORT UI_PALETTE_MAP
{
	OBJECTID objectID;
	LOGICAL_PALETTE logicalPalette;
	UI_PALETTE palette;

	static UI_PALETTE *MapPalette(UI_PALETTE_MAP *mapTable,
		LOGICAL_PALETTE logicalPalette, OBJECTID id1 = ID_WINDOW_OBJECT,
		OBJECTID id2 = ID_WINDOW_OBJECT, OBJECTID id3 = ID_WINDOW_OBJECT,
		OBJECTID id4 = ID_WINDOW_OBJECT, OBJECTID id5 = ID_WINDOW_OBJECT);
};

// ----- UI_EVENT_MAP -------------------------------------------------------

struct EXPORT UI_EVENT_MAP
{
	OBJECTID objectID;
	LOGICAL_EVENT logicalValue;
	EVENT_TYPE eventType;
	RAW_CODE rawCode;
	RAW_CODE modifiers;

	static LOGICAL_EVENT MapEvent(UI_EVENT_MAP *mapTable,
		const UI_EVENT &event, OBJECTID id1 = ID_WINDOW_OBJECT,
		OBJECTID id2 = ID_WINDOW_OBJECT, OBJECTID id3 = ID_WINDOW_OBJECT,
		OBJECTID id4 = ID_WINDOW_OBJECT, OBJECTID id5 = ID_WINDOW_OBJECT);
};

// ----- UI_WINDOW_OBJECT ---------------------------------------------------

// --- WOF_FLAGS ---
typedef UIF_FLAGS WOF_FLAGS;
const WOF_FLAGS WOF_NO_FLAGS			= 0x0000;
const WOF_FLAGS WOF_JUSTIFY_CENTER		= 0x0001;	// center justify data
const WOF_FLAGS WOF_JUSTIFY_RIGHT		= 0x0002;	// right justify data
const WOF_FLAGS WOF_BORDER				= 0x0004;	// draw a border
const WOF_FLAGS WOF_NO_ALLOCATE_DATA	= 0x0008;	// do not allocate the data buffer
const WOF_FLAGS WOF_VIEW_ONLY			= 0x0010;	// object can be viewed, but not edited
const WOF_FLAGS WOF_SUPPORT_OBJECT		= 0x0020;	// place object in the support list
const WOF_FLAGS WOF_MINICELL			= 0x0040;	// original object region is in mini-cell coordinates
const WOF_FLAGS WOF_UNANSWERED			= 0x0080;	// Sets initial status bit to "unanswered"
const WOF_FLAGS WOF_INVALID				= 0x0100;	// Sets invalid status
const WOF_FLAGS WOF_NON_FIELD_REGION	= 0x0200;	// object does not share screen space
const WOF_FLAGS WOF_NON_SELECTABLE		= 0x0400;	// object cannot be selected
const WOF_FLAGS WOF_AUTO_CLEAR			= 0x0800;	// auto clear option for edit fields

// --- WOAF_FLAGS --- 
typedef UIF_FLAGS WOAF_FLAGS;
const WOAF_FLAGS WOAF_NO_FLAGS			= 0x0000;
const WOAF_FLAGS WOAF_OUTSIDE_REGION	= 0x0001;	// object occupies space outside the true region
const WOAF_FLAGS WOAF_NON_CURRENT		= 0x0002;	// object cannot be made current
const WOAF_FLAGS WOAF_TEMPORARY			= 0x0004;	// object is only temporarily on the display
const WOAF_FLAGS WOAF_NO_DESTROY		= 0x0010;	// don't destroy the object
const WOAF_FLAGS WOAF_NORMAL_HOT_KEYS	= 0x0020;	// look for hot keys on normal input.
const WOAF_FLAGS WOAF_NO_SIZE			= 0x0040;	// object cannot be sized
const WOAF_FLAGS WOAF_NO_MOVE			= 0x0080;	// object cannot be moved
const WOAF_FLAGS WOAF_MODAL				= 0x0100;	// object is modal (gets all events)
const WOAF_FLAGS WOAF_LOCKED			= 0x0200;	// object cannot be removed from the window manager

const WOAF_FLAGS WOAF_MDI_OBJECT		= 0x4000;	// object is an mdi window
const WOAF_FLAGS WOAF_DIALOG_OBJECT		= 0x8000;	// object is a dialog window

// --- WOS_STATUS ---
typedef UIS_STATUS WOS_STATUS;
const WOS_STATUS WOS_NO_STATUS			= 0x0000;
const WOS_STATUS WOS_GRAPHICS			= 0x0001;	// object region is in graphics coordinates
const WOS_STATUS WOS_CURRENT			= 0x0002;	// object is current
const WOS_STATUS WOS_CHANGED			= 0x0004;	// object data has changed
const WOS_STATUS WOS_SELECTED			= 0x0008;	// object has been selected
const WOS_STATUS WOS_UNANSWERED			= 0x0010;	// object data is answered
const WOS_STATUS WOS_INVALID			= 0x0020;	// object has invalid data
const WOS_STATUS WOS_MAXIMIZED			= 0x0040;	// object is maximized
const WOS_STATUS WOS_MINIMIZED			= 0x0080;	// object is minimized
const WOS_STATUS WOS_REDISPLAY			= 0x0100;	// data needs updating
const WOS_STATUS WOS_READ_ERROR			= 0x0200;	// read error occurred

// Private status flags - DO NOT USE!
const WOS_STATUS WOS_WINDOWS_ACTION		= 0x0800;
const WOS_STATUS WOS_OWNERDRAW			= 0x1000;
const WOS_STATUS WOS_EDIT_MODE			= 0x2000;
const WOS_STATUS WOS_INTERNAL_ACTION	= 0x4000;
const WOS_STATUS WOS_DESTROY_ICON		= 0x8000;	// for UIW_ICON only
const WOS_STATUS WOS_AUTO_MARK			= 0x8000;	// for UIW_STRING and UIW_TEXT only
const WOS_STATUS WOS_ACTIVE_MDI			= 0x8000;	// for UIW_WINDOW only

// --- INFO_REQUEST ---
const INFO_REQUEST GET_NUMBERID_OBJECT	= 0x0001;
const INFO_REQUEST GET_STRINGID_OBJECT	= 0x0002;
const INFO_REQUEST GET_FLAGS			= 0x0003;
const INFO_REQUEST SET_FLAGS			= 0x0004;
const INFO_REQUEST CLEAR_FLAGS			= 0x0005;
const INFO_REQUEST CHANGED_FLAGS		= 0x0006;
const INFO_REQUEST GET_STATUS			= 0x0007;
const INFO_REQUEST SET_STATUS			= 0x0008;
const INFO_REQUEST CLEAR_STATUS			= 0x0009;
const INFO_REQUEST CHANGED_STATUS		= 0x000A;
const INFO_REQUEST GET_TEXT				= 0x000B;
const INFO_REQUEST SET_TEXT				= 0x000C;
const INFO_REQUEST GET_TEXT_LENGTH		= 0x000D;
const INFO_REQUEST SET_TEXT_LENGTH		= 0x000E;
const INFO_REQUEST COPY_TEXT			= 0x000F;

#if defined(ZIL_MSWINDOWS)
const INFO_REQUEST GET_DWSTYLE			= 0x0051;	// Windows only requests.
const INFO_REQUEST SET_DWSTYLE			= 0x0052;
const INFO_REQUEST CLEAR_DWSTYLE		= 0x0053;
const INFO_REQUEST CHANGED_DWSTYLE		= 0x0054;
const INFO_REQUEST HIDE_SUBWINDOW		= 0x0055;
const INFO_REQUEST SET_TASKID 			= 0x0056;
#elif defined(ZIL_OS2)
const INFO_REQUEST GET_FLSTYLE 			= 0x0051;	// OS/2 only requests.
const INFO_REQUEST SET_FLSTYLE 			= 0x0052;
const INFO_REQUEST CLEAR_FLSTYLE		= 0x0053;
const INFO_REQUEST CHANGED_FLSTYLE 		= 0x0054;
const INFO_REQUEST GET_FLFLAG			= 0x0055;
const INFO_REQUEST SET_FLFLAG			= 0x0056;
const INFO_REQUEST CLEAR_FLFLAG			= 0x0057;
const INFO_REQUEST CHANGED_FLFLAG		= 0x0058;
#elif defined(ZIL_MOTIF)
const INFO_REQUEST GET_SYSTEM_OPTIONS	= 0x0051;	// MOTIF only requests.
const INFO_REQUEST SET_SYSTEM_OPTIONS	= 0x0052;
const INFO_REQUEST GET_DECORATIONS		= 0x0053;
const INFO_REQUEST SET_DECORATIONS		= 0x0054;
#endif

// Private class messages --- DO NOT USE!
const INFO_REQUEST INITIALIZE_CLASS		= 0x0000;	// Special initialization
const INFO_REQUEST PRINT_INFORMATION	= 0x0080;	// Special hpp print
const INFO_REQUEST PRINT_USER_FUNCTION	= 0x0081;	// Special cpp print
const INFO_REQUEST PRINT_COMPARE_FUNCTION = 0x0082;	// Special cpp print

// --- UI_HELP_CONTEXT ---
typedef int UI_HELP_CONTEXT;
const UI_HELP_CONTEXT NO_HELP_CONTEXT	= 0x0000;

// --- Underline character information ---
#if defined(ZIL_MSDOS) || defined(ZIL_MSWINDOWS)
const char UNDERLINE_CHARACTER 			= '&';
const char OLD_UNDERLINE_CHARACTER 		= '~';
#elif defined(ZIL_OS2) || defined(ZIL_MOTIF)
const char UNDERLINE_CHARACTER 			= '~';
const char OLD_UNDERLINE_CHARACTER 		= '&';
#endif

#if defined(ZIL_MOTIF)
typedef Widget (*MOTIF_CONVENIENCE_FUNCTION)(Widget parent, char *name,
	ArgList args, Cardinal count);
#elif defined(ZIL_OS2)
	extern MRESULT EXPENTRY ObjectJumpProcedure(HWND, ULONG, MPARAM, MPARAM);
#endif

class EXPORT UI_WINDOW_OBJECT : public UI_ELEMENT
{
	friend class EXPORT UIW_WINDOW;
	friend class EXPORT UIW_COMBO_BOX;
	friend class EXPORT UIF_CONTROL;
	friend class EXPORT UIF_WINDOW_OBJECT;
public:
	// Foreward declaration of classes used by UI_WINDOW_OBJECT.
	friend class EXPORT UI_WINDOW_MANAGER;
	friend class EXPORT UI_ERROR_SYSTEM;
	friend class EXPORT UI_HELP_SYSTEM;

	static int repeatRate;
	static int doubleClickRate;
	static WOS_STATUS defaultStatus;
	static UI_DISPLAY *display;
	static UI_EVENT_MANAGER *eventManager;
	static UI_WINDOW_MANAGER *windowManager;
	static UI_ERROR_SYSTEM *errorSystem;
	static UI_HELP_SYSTEM *helpSystem;
	static UI_STORAGE *defaultStorage;
	static UI_ITEM *objectTable;
	static UI_ITEM *userTable;

	UI_EVENT_MAP *eventMapTable;
	UI_EVENT_MAP *hotKeyMapTable;
	UI_PALETTE_MAP *paletteMapTable;

	SCREENID screenID;
	WOF_FLAGS woFlags;
	WOAF_FLAGS woAdvancedFlags;
#if defined (ZIL_EDIT)
	WOAF_FLAGS designerAdvancedFlags;
#endif
	WOS_STATUS woStatus;
	UI_REGION true;
	UI_REGION relative;
	UI_WINDOW_OBJECT *parent;
	UI_HELP_CONTEXT helpContext;

	UIF_FLAGS userFlags;
	UIS_STATUS userStatus;
	void *userObject;
	EVENT_TYPE (*userFunction)(UI_WINDOW_OBJECT *object, UI_EVENT &event, EVENT_TYPE ccode);
	EVENT_TYPE UserFunction(const UI_EVENT &event, EVENT_TYPE ccode);

	virtual ~UI_WINDOW_OBJECT(void);
	virtual EVENT_TYPE Event(const UI_EVENT &event);
	UI_WINDOW_OBJECT *Get(const char *name) { return ((UI_WINDOW_OBJECT *)Information(GET_STRINGID_OBJECT, (void *)name, 0)); }
	UI_WINDOW_OBJECT *Get(NUMBERID numberID) { return ((UI_WINDOW_OBJECT *)Information(GET_NUMBERID_OBJECT, (void *)&numberID, 0)); }
	unsigned HotKey(unsigned hotKey = 0);
	unsigned HotKey(char *text);
	virtual void *Information(INFO_REQUEST request, void *data, OBJECTID objectID = 0);
	int Inherited(OBJECTID matchID)
		{ return(matchID == windowID[0] || matchID == windowID[1] ||
		  matchID == windowID[2] || matchID == windowID[3] ||
		  matchID == windowID[4]); }
	EVENT_TYPE LogicalEvent(const UI_EVENT &event, OBJECTID currentID = 0);
	UI_PALETTE *LogicalPalette(LOGICAL_EVENT logicalEvent, OBJECTID currentID = 0);
	NUMBERID NumberID(NUMBERID numberID = 0);
	void RegionConvert(UI_REGION &region, int absolute);
	OBJECTID SearchID(void) { return (searchID); }
	char *StringID(const char *stringID = NULL);
#if defined(ZIL_MOTIF)
	char *StripHotKeyMark(char *text);
	SCREENID TopWidget(void);
	Widget shell;
#endif
	virtual int Validate(int processError = TRUE);

#if defined(ZIL_PERSISTENCE)
	static UI_WINDOW_OBJECT *New(const char *name, UI_STORAGE *file = NULL, UI_STORAGE_OBJECT *object = NULL);
	UI_WINDOW_OBJECT(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object);
	virtual void Load(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object);
	virtual void Store(const char *name, UI_STORAGE *file = NULL, UI_STORAGE_OBJECT *object = NULL);
#endif

	// List members.
	UI_WINDOW_OBJECT *Next(void) { return((UI_WINDOW_OBJECT *)next); }
	UI_WINDOW_OBJECT *Previous(void) { return((UI_WINDOW_OBJECT *)previous); }

protected:
	OBJECTID searchID;
	NUMBERID numberID;
	char stringID[32];
	OBJECTID windowID[5];

	unsigned hotKey;
	LOGICAL_FONT font;
	UI_PALETTE *lastPalette;
	char *userObjectName;				// Used for storage purposes only.
	char *userFunctionName;				// Used for storage purposes only.
	UI_REGION clip;
#if defined(ZIL_MSDOS)
	static char *pasteBuffer;			// There is only one global paste buffer.
	static int pasteLength;
#elif defined(ZIL_MSWINDOWS) && defined(WIN32)
	DWORD dwStyle;
	WNDPROC defaultCallback;
	void RegisterObject(char *className, char *winClassName,
		WNDPROC *defProcInstance, char *title = NULL, HMENU menu = 0);
#elif defined(ZIL_MSWINDOWS)
	DWORD dwStyle;
	FARPROC defaultCallback;

	void RegisterObject(char *className, char *winClassName, int *offset,
		FARPROC *procInstance, FARPROC *defProcInstance, char *title = NULL,
		HMENU menu = 0);
#elif defined(ZIL_OS2)
	ULONG flStyle;
	ULONG flFlag;
	PFNWP defaultCallback;

	void RegisterObject(char *className, PSZ os2ClassName,
		PFNWP *baseCallback, char *title);
#elif defined(ZIL_MOTIF)
	static Arg args[50];
	static int nargs;
	void RegisterObject(WidgetClass widgetClass, 
		MOTIF_CONVENIENCE_FUNCTION convenienceFunction, EVENT_TYPE ccode,
		int useArgs = FALSE, int manage = TRUE, SCREENID _parent = NULL);
#endif

	UI_WINDOW_OBJECT(int left, int top, int width, int height,
		WOF_FLAGS woFlags, WOAF_FLAGS woAdvancedFlags);
	EVENT_TYPE DrawBorder(SCREENID screenID, UI_REGION &region, int fillRegion, EVENT_TYPE ccode);
	virtual EVENT_TYPE DrawItem(const UI_EVENT &event, EVENT_TYPE ccode);
	EVENT_TYPE DrawShadow(SCREENID screenID, UI_REGION &region, int depth, int fillRegion, EVENT_TYPE ccode);
	EVENT_TYPE DrawText(SCREENID screenID, UI_REGION &region, const char *text, UI_PALETTE *palette, int fillRegion, EVENT_TYPE ccode);
	void Modify(const UI_EVENT &event);
	int NeedsUpdate(const UI_EVENT &event, EVENT_TYPE ccode);
	void RegisterObject(char *name);
	virtual void RegionMax(UI_WINDOW_OBJECT *object);
};

typedef UI_WINDOW_OBJECT *(*NEW_FUNCTION)(const char *name, UI_STORAGE *storage, UI_STORAGE_OBJECT *object);
typedef EVENT_TYPE (*USER_FUNCTION)(UI_WINDOW_OBJECT *object, UI_EVENT &event, EVENT_TYPE ccode);
typedef int (*COMPARE_FUNCTION)(void *element1, void *element2);

// ----- UIW_WINDOW ---------------------------------------------------------

// --- WNF_FLAGS ---
typedef UIF_FLAGS WNF_FLAGS;
const WNF_FLAGS WNF_NO_FLAGS			= 0x0000;
const WNF_FLAGS WNF_NO_WRAP				= 0x0001;
const WNF_FLAGS WNF_SELECT_MULTIPLE		= 0x0002;
const WNF_FLAGS WNF_BITMAP_CHILDREN		= 0x0004;	// Option used for list, matrix, combo.
const WNF_FLAGS WNF_AUTO_SORT			= 0x0008;
const WNF_FLAGS WNF_CONTINUE_SELECT		= 0x0010;
const WNF_FLAGS WNF_AUTO_SELECT			= 0x0020;	// Automatically select when item is current.
const WNF_FLAGS WNF_MOVE_FORWARD		= 0x0040;
const WNF_FLAGS WNF_MOVE_REVERSE		= 0x0080;

// --- INFO_REQUEST ---
const INFO_REQUEST SET_VSCROLL			= 0x0085;
const INFO_REQUEST SET_HSCROLL			= 0x0086;
const INFO_REQUEST CHECK_SELECTION		= 0x0087;
const INFO_REQUEST GET_CLIPREGION       = 0x0088;
const INFO_REQUEST DESTROY_LIST       	= 0x008E;

// Private class messages --- DO NOT USE!
const INFO_REQUEST GET_MENU				= 0x0089;
const INFO_REQUEST GET_MDIHANDLE		= 0x008A;
const INFO_REQUEST SET_TITLE			= 0x008B;
const INFO_REQUEST SET_MENU				= 0x008C;
const INFO_REQUEST SET_ICON				= 0x008D;

class EXPORT UIW_WINDOW : public UI_WINDOW_OBJECT, public UI_LIST
{
	friend class UI_WINDOW_MANAGER;
	friend class EXPORT UIF_WINDOW;
public:
	WNF_FLAGS wnFlags;
	UI_LIST support;

	UIW_WINDOW(int left, int top, int width, int height,
		WOF_FLAGS woFlags = WOF_NO_FLAGS, WOAF_FLAGS woAdvancedFlags = WOAF_NO_FLAGS,
		UI_HELP_CONTEXT helpContext = NO_HELP_CONTEXT, UI_WINDOW_OBJECT *minObject = NULL);
	virtual ~UIW_WINDOW(void);
	virtual void Destroy(void);
	virtual EVENT_TYPE Event(const UI_EVENT &event);
	static UIW_WINDOW *Generic(int left, int top, int width, int height,
		char *title, UI_WINDOW_OBJECT *minObject = NULL,
		WOF_FLAGS woFlags = WOF_NO_FLAGS, WOAF_FLAGS woAdvancedFlags = WOAF_NO_FLAGS,
		UI_HELP_CONTEXT helpContext = NO_HELP_CONTEXT);
	UI_WINDOW_OBJECT *Get(const char *name) { return ((UI_WINDOW_OBJECT *)Information(GET_STRINGID_OBJECT, (void *)name, 0)); }
	UI_WINDOW_OBJECT *Get(NUMBERID numberID) { return ((UI_WINDOW_OBJECT *)Information(GET_NUMBERID_OBJECT, (void *)&numberID, 0)); }
	virtual void *Information(INFO_REQUEST request, void *data, OBJECTID objectID = 0);
	static int StringCompare(void *object1, void *object2);

#if defined(ZIL_PERSISTENCE)
	static UI_WINDOW_OBJECT *New(const char *name, UI_STORAGE *file = NULL, UI_STORAGE_OBJECT *object = NULL)
		{ return (new UIW_WINDOW(name, file, object)); }
	UIW_WINDOW(const char *name, UI_STORAGE *file = NULL, UI_STORAGE_OBJECT *object = NULL);
	virtual void Load(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object);
	virtual void Store(const char *name, UI_STORAGE *file = NULL, UI_STORAGE_OBJECT *object = NULL);
#endif

	// List members.
	UI_WINDOW_OBJECT *Add(UI_WINDOW_OBJECT *object);
	UI_WINDOW_OBJECT *Current(void) { return((UI_WINDOW_OBJECT *)current); }
	UI_WINDOW_OBJECT *First(void) { return((UI_WINDOW_OBJECT *)first); }
	UI_WINDOW_OBJECT *Last(void) { return((UI_WINDOW_OBJECT *)last); }
	UI_WINDOW_OBJECT *Subtract(UI_WINDOW_OBJECT *object);
	UIW_WINDOW &operator+(UI_WINDOW_OBJECT *object) { Add(object); return(*this); }
	UIW_WINDOW &operator-(UI_WINDOW_OBJECT *object) { Subtract(object); return(*this); }

protected:
	UI_REGION scroll;
	UI_REGION_LIST clipList;
	UI_WINDOW_OBJECT *vScroll;
	UI_WINDOW_OBJECT *hScroll;
	char *compareFunctionName;			// Used for storage purposes only.

	void CheckSelection(void);
	virtual EVENT_TYPE DrawItem(const UI_EVENT &event, EVENT_TYPE ccode);
	virtual void RegionMax(UI_WINDOW_OBJECT *object);

#ifdef ZIL_MOTIF
	long supportDecorations;
#endif

private:
#if defined(ZIL_MSDOS)
	UI_WINDOW_OBJECT *icon;
	UI_REGION restore;
#elif defined(ZIL_MSWINDOWS)
	UI_WINDOW_OBJECT *title;
	SCREENID mdiClient;
	HMENU menu;
	HICON icon;
	long (FAR PASCAL *mdiCallback)(HWND, HWND, WORD, WORD, LONG);
	UI_REGION restore;
#elif defined(ZIL_OS2)
	UI_WINDOW_OBJECT *title;
	UI_REGION restore;
#elif defined(ZIL_MOTIF)
	static void ExitShell(Widget w, XtPointer closure, XtPointer call_data);
	void ShellRegion(UI_REGION &);
	UI_REGION GetMotifRegion(void);
#endif

	void Error(UIS_STATUS status, const char *format, ...);
};

// ----- UI_WINDOW_MANAGER --------------------------------------------------

class EXPORT UI_WINDOW_MANAGER : public UIW_WINDOW
{
	friend class UI_WINDOW_OBJECT;
public:
	EVENT_TYPE (*exitFunction)(UI_DISPLAY *display, UI_EVENT_MANAGER *eventManager, UI_WINDOW_MANAGER *windowManager);

	UI_WINDOW_MANAGER(UI_DISPLAY *display, UI_EVENT_MANAGER *eventManager,
		EVENT_TYPE (*exitFunction)(UI_DISPLAY *display, UI_EVENT_MANAGER *eventManager, UI_WINDOW_MANAGER *windowManager) = NULL);
	virtual ~UI_WINDOW_MANAGER(void);
	void Center(UI_WINDOW_OBJECT *object);
	virtual EVENT_TYPE Event(const UI_EVENT &event);
	virtual void *Information(INFO_REQUEST request, void *data, OBJECTID objectID = 0);

	// List members.
	UI_WINDOW_OBJECT *Add(UI_WINDOW_OBJECT *object);
	UI_WINDOW_OBJECT *Subtract(UI_WINDOW_OBJECT *object);
	UI_WINDOW_MANAGER &operator+(UI_WINDOW_OBJECT *object) { Add(object); return(*this); }
	UI_WINDOW_MANAGER &operator-(UI_WINDOW_OBJECT *object) { Subtract(object); return(*this); };

private:
#if defined(ZIL_MSDOS)
	static SCREENID currentScreenID;
#elif defined(ZIL_MSWINDOWS) || defined(ZIL_OS2) || defined(ZIL_MOTIF)
	static SCREENID taskID;
#endif
};

// ----- UIW_BORDER ---------------------------------------------------------

// --- BDF_FLAGS ---
typedef UIF_FLAGS BDF_FLAGS;
const BDF_FLAGS BDF_NO_FLAGS			= 0x0000;

class EXPORT UIW_BORDER : public UI_WINDOW_OBJECT
{
	friend class EXPORT UIF_BORDER;
public:
	static int width;
	BDF_FLAGS bdFlags;

	UIW_BORDER(BDF_FLAGS bdFlags = BDF_NO_FLAGS);
	virtual ~UIW_BORDER(void);
	int DataGet(void);
	void DataSet(int width);
	virtual EVENT_TYPE Event(const UI_EVENT &event);
	virtual void *Information(INFO_REQUEST request, void *data, OBJECTID objectID = 0);

#if defined(ZIL_PERSISTENCE)
	static UI_WINDOW_OBJECT *New(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object)
		{ return (new UIW_BORDER(name, file, object)); }
	UIW_BORDER(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object);
	virtual void Load(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object);
	virtual void Store(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object);
#endif

protected:
	virtual EVENT_TYPE DrawItem(const UI_EVENT &event, EVENT_TYPE ccode);
};

// ----- UIW_PROMPT ---------------------------------------------------------

class EXPORT UIW_PROMPT : public UI_WINDOW_OBJECT
{
	friend class EXPORT UIF_PROMPT;
public:
	UIW_PROMPT(int left, int top, char *text, WOF_FLAGS woFlags = WOF_NO_FLAGS);
	UIW_PROMPT(int left, int top, int width, char *text, WOF_FLAGS woFlags = WOF_NO_FLAGS);
	virtual ~UIW_PROMPT(void);
	char *DataGet(void);
	void DataSet(char *text);
	virtual EVENT_TYPE Event(const UI_EVENT &event);
	virtual void *Information(INFO_REQUEST request, void *data, OBJECTID objectID = 0);

#if defined(ZIL_PERSISTENCE)
	static UI_WINDOW_OBJECT *New(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object)
		{ return (new UIW_PROMPT(name, file, object)); }
	UIW_PROMPT(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object);
	virtual void Load(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object);
	virtual void Store(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object);
#endif

protected:
	char *text;

	virtual EVENT_TYPE DrawItem(const UI_EVENT &event, EVENT_TYPE ccode);
};

// ----- UIW_BUTTON ---------------------------------------------------------

// --- BTF_FLAGS ---
typedef UIF_FLAGS BTF_FLAGS;
const BTF_FLAGS BTF_NO_FLAGS			= 0x0000;
const BTF_FLAGS BTF_NO_TOGGLE			= 0x0001;	// don't toggle (select) the button
const BTF_FLAGS BTF_DOWN_CLICK			= 0x0002;	// call user-function on down click
const BTF_FLAGS BTF_REPEAT				= 0x0008;	// repeat the down-click action
const BTF_FLAGS BTF_DOUBLE_CLICK		= 0x0010;	// perform the action on a double-mouse click.
const BTF_FLAGS BTF_AUTO_SIZE			= 0x0020;	// size according to mode of operation
const BTF_FLAGS BTF_NO_3D				= 0x0040;	// do not shadow button
const BTF_FLAGS BTF_CHECK_BOX			= 0x0080;	// display a check box
const BTF_FLAGS BTF_RADIO_BUTTON		= 0x0100;	// display a radio button
const BTF_FLAGS BTF_SEND_MESSAGE		= 0x0200;	// use the Message function
const BTF_FLAGS BTF_STATIC_BITMAPARRAY	= 0x0400;	// do not destroy the bitmap array.

// --- BTS_STATUS ---
typedef UIS_STATUS BTS_STATUS;
const BTS_STATUS BTS_NO_STATUS			= 0x0000;
const BTS_STATUS BTS_DEPRESSED			= 0x0001;	// button is depressed

// --- INFO_REQUEST ---
const INFO_REQUEST GET_VALUE			= 0x0101;
const INFO_REQUEST SET_VALUE			= 0x0102;
const INFO_REQUEST GET_BITMAP_WIDTH		= 0x0103;
const INFO_REQUEST SET_BITMAP_WIDTH		= 0x0104;
const INFO_REQUEST GET_BITMAP_HEIGHT	= 0x0105;
const INFO_REQUEST SET_BITMAP_HEIGHT	= 0x0106;
const INFO_REQUEST GET_BITMAP_ARRAY		= 0x0107;
const INFO_REQUEST SET_BITMAP_ARRAY		= 0x0108;

class EXPORT UIW_BUTTON : public UI_WINDOW_OBJECT
{
	friend class EXPORT UIF_BUTTON;
public:
	BTF_FLAGS btFlags;
	EVENT_TYPE value;

	UIW_BUTTON(int left, int top, int width, char *text,
		BTF_FLAGS btFlags = BTF_NO_TOGGLE | BTF_AUTO_SIZE,
		WOF_FLAGS woFlags = WOF_JUSTIFY_CENTER,
		USER_FUNCTION userFunction = NULL, EVENT_TYPE value = 0,
		char *bitmapName = NULL);
	virtual ~UIW_BUTTON(void);
	virtual EVENT_TYPE Event(const UI_EVENT &event);
	char *DataGet(int stripText = FALSE);
	void DataSet(char *text);
	virtual void *Information(INFO_REQUEST request, void *data, OBJECTID objectID = 0);
	static EVENT_TYPE Message(UI_WINDOW_OBJECT *object, UI_EVENT &event, EVENT_TYPE ccode);

#if defined(ZIL_PERSISTENCE)
	static UI_WINDOW_OBJECT *New(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object)
		{ return (new UIW_BUTTON(name, file, object)); }
	UIW_BUTTON(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object);
	virtual void Load(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object);
	virtual void Store(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object);
#endif

protected:
	BTS_STATUS btStatus;
	int depth;
	char *text;
	char *bitmapName;
	int bitmapWidth;
	int bitmapHeight;
	UCHAR *bitmapArray;
	HBITMAP colorBitmap, monoBitmap;
#if defined(ZIL_MOTIF)
	Pixmap pixmap;
#endif

	virtual EVENT_TYPE DrawItem(const UI_EVENT &event, EVENT_TYPE ccode);
};

// ----- UIW_TITLE ----------------------------------------------------------

class EXPORT UIW_TITLE : public UIW_BUTTON
{
	friend class EXPORT UIF_TITLE;
public:
	UIW_TITLE(char *text, WOF_FLAGS woFlags = WOF_BORDER | WOF_JUSTIFY_CENTER);
	virtual ~UIW_TITLE(void);
	char *DataGet(void);
	void DataSet(char *text);
	virtual EVENT_TYPE Event(const UI_EVENT &event);
	virtual void *Information(INFO_REQUEST request, void *data, OBJECTID objectID = 0);

#if defined(ZIL_PERSISTENCE)
	static UI_WINDOW_OBJECT *New(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object)
		{ return (new UIW_TITLE(name, file, object)); }
	UIW_TITLE(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object);
	virtual void Load(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object);
	virtual void Store(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object);
#endif
};

// ----- UIW_MAXIMIZE_BUTTON ------------------------------------------------

class EXPORT UIW_MAXIMIZE_BUTTON : public UIW_BUTTON
{
	friend class EXPORT UIF_MAXIMIZE_BUTTON;
public:
	UIW_MAXIMIZE_BUTTON(void);
	virtual ~UIW_MAXIMIZE_BUTTON(void);
	virtual EVENT_TYPE Event(const UI_EVENT &event);
	virtual void *Information(INFO_REQUEST request, void *data, OBJECTID objectID = 0);

#if defined(ZIL_PERSISTENCE)
	static UI_WINDOW_OBJECT *New(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object)
		{ return (new UIW_MAXIMIZE_BUTTON(name, file, object)); }
	UIW_MAXIMIZE_BUTTON(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object);
	virtual void Load(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object);
	virtual void Store(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object);
#endif
};

// ----- UIW_MINIMIZE_BUTTON ------------------------------------------------

class EXPORT UIW_MINIMIZE_BUTTON : public UIW_BUTTON
{
	friend class EXPORT UIF_MINIMIZE_BUTTON;
public:
	UIW_MINIMIZE_BUTTON(void);
	virtual ~UIW_MINIMIZE_BUTTON(void);
	virtual EVENT_TYPE Event(const UI_EVENT &event);
	virtual void *Information(INFO_REQUEST request, void *data, OBJECTID objectID = 0);

#if defined(ZIL_PERSISTENCE)
	static UI_WINDOW_OBJECT *New(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object)
		{ return (new UIW_MINIMIZE_BUTTON(name, file, object)); }
	UIW_MINIMIZE_BUTTON(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object);
	virtual void Load(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object);
	virtual void Store(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object);
#endif
};

// ----- UIW_ICON -----------------------------------------------------------

// --- ICF_FLAGS ---
typedef UIF_FLAGS ICF_FLAGS;
const ICF_FLAGS ICF_NO_FLAGS			= 0x0000;
const ICF_FLAGS ICF_DOUBLE_CLICK		= 0x0001;
const ICF_FLAGS ICF_MINIMIZE_OBJECT		= 0x0002;
const ICF_FLAGS ICF_STATIC_ICONARRAY	= 0x0400;	// do not destroy the icon array.

// --- INFO_REQUEST ---
const INFO_REQUEST GET_ICON_WIDTH		= GET_BITMAP_WIDTH;
const INFO_REQUEST GET_ICON_HEIGHT		= GET_BITMAP_HEIGHT;
const INFO_REQUEST GET_ICON_ARRAY		= GET_BITMAP_ARRAY;
const INFO_REQUEST SET_ICON_ARRAY		= SET_BITMAP_ARRAY;

class EXPORT UIW_ICON : public UI_WINDOW_OBJECT
{
	friend class EXPORT UIF_ICON;
public:
	ICF_FLAGS icFlags;

	UIW_ICON(int left, int top, char *iconName, char *title = NULL,
		ICF_FLAGS icFlags = ICF_NO_FLAGS,
		WOF_FLAGS woFlags = WOF_JUSTIFY_CENTER | WOF_NON_SELECTABLE,
		USER_FUNCTION userFunction = NULL);
	virtual ~UIW_ICON(void);
	char *DataGet(void);
	void DataSet(char *text);
	virtual EVENT_TYPE Event(const UI_EVENT &event);
	virtual void *Information(INFO_REQUEST request, void *data, OBJECTID objectID = 0);

#if defined(ZIL_PERSISTENCE)
	static UI_WINDOW_OBJECT *New(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object)
		{ return (new UIW_ICON(name, file, object)); }
	UIW_ICON(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object);
	virtual void Load(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object);
	virtual void Store(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object);
#endif

protected:
	char *title;
	char *iconName;
	int iconWidth;
	int iconHeight;
	UCHAR *iconArray;
	HICON icon;
#if defined(ZIL_MSDOS)
	UI_REGION iconRegion;
	UI_REGION titleRegion;
#endif

	virtual EVENT_TYPE DrawItem(const UI_EVENT &event, EVENT_TYPE ccode);

private:
#if defined(ZIL_MSWINDOWS) || defined(ZIL_OS2)
	BTS_STATUS icStatus;
#endif
};

// ----- UIW_POP_UP_MENU ----------------------------------------------------

class EXPORT UIW_POP_UP_MENU : public UIW_WINDOW
{
	friend class EXPORT UIF_POP_UP_MENU;
public:
	UIW_POP_UP_MENU(int left, int top, WNF_FLAGS wnFlags,
		WOF_FLAGS woFlags = WOF_BORDER, WOAF_FLAGS woAdvancedFlags = WOAF_NO_FLAGS);
	UIW_POP_UP_MENU(int left, int top, WNF_FLAGS wnFlags, UI_ITEM *item);
	virtual ~UIW_POP_UP_MENU(void);
	virtual EVENT_TYPE Event(const UI_EVENT &event);
	virtual void *Information(INFO_REQUEST request, void *data, OBJECTID objectID = 0);

#if defined(ZIL_PERSISTENCE)
	static UI_WINDOW_OBJECT *New(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object)
		{ return (new UIW_POP_UP_MENU(name, file, object)); }
	UIW_POP_UP_MENU(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object);
	virtual void Load(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object);
	virtual void Store(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object);
#endif
};

// ----- UIW_POP_UP_ITEM ----------------------------------------------------

// --- MNIF_FLAGS ---
typedef UIF_FLAGS MNIF_FLAGS;
const MNIF_FLAGS MNIF_NO_FLAGS			= 0x0000;
const MNIF_FLAGS MNIF_SEPARATOR			= 0x0001;	// Menu item seperator
const MNIF_FLAGS MNIF_MAXIMIZE			= 0x0002;	// Maximize window
const MNIF_FLAGS MNIF_MINIMIZE			= 0x0004;	// Minimize window
const MNIF_FLAGS MNIF_MOVE				= 0x0008;	// Move window
const MNIF_FLAGS MNIF_SIZE				= 0x0010;	// Size window
const MNIF_FLAGS MNIF_SWITCH			= 0x0020;	// Switch to another window.
const MNIF_FLAGS MNIF_RESTORE			= 0x0040;	// Restore window size
const MNIF_FLAGS MNIF_CLOSE				= 0x0080;	// Close window
const MNIF_FLAGS MNIF_CHECK_MARK		= 0x0100;	// Show check-mark when selected.
const MNIF_FLAGS MNIF_SEND_MESSAGE		= 0x0200;	// use the Message function
const MNIF_FLAGS MNIF_NON_SELECTABLE	= 0x0400;	// item cannot be selected (grayed)

class EXPORT UIW_POP_UP_ITEM : public UIW_BUTTON
{
	friend class EXPORT UIF_POP_UP_ITEM;
public:
	MNIF_FLAGS mniFlags;
	UIW_POP_UP_MENU menu;

	UIW_POP_UP_ITEM(void);
	UIW_POP_UP_ITEM(char *text, MNIF_FLAGS mniFlags = MNIF_NO_FLAGS,
		BTF_FLAGS btFlags = BTF_NO_3D, WOF_FLAGS woFlags = WOF_NO_FLAGS,
		USER_FUNCTION userFunction = NULL, EVENT_TYPE value = 0);
	UIW_POP_UP_ITEM(int left, int top, int width, char *text,
		MNIF_FLAGS mniFlags = MNIF_NO_FLAGS, BTF_FLAGS btFlags = BTF_NO_FLAGS,
		WOF_FLAGS woFlags = WOF_NO_FLAGS, USER_FUNCTION userFunction = NULL,
		EVENT_TYPE value = 0);
	virtual ~UIW_POP_UP_ITEM(void);
	virtual EVENT_TYPE Event(const UI_EVENT &event);
	virtual void *Information(INFO_REQUEST request, void *data, OBJECTID objectID = 0);

#if defined(ZIL_PERSISTENCE)
	static UI_WINDOW_OBJECT *New(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object)
		{ return (new UIW_POP_UP_ITEM(name, file, object)); }
	UIW_POP_UP_ITEM(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object);
	virtual void Load(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object);
	virtual void Store(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object);
#endif

	// List members.
	UI_WINDOW_OBJECT *Add(UI_WINDOW_OBJECT *object) { return(menu.Add(object)); }
	UI_WINDOW_OBJECT *Subtract(UI_WINDOW_OBJECT *object) { return(menu.Subtract(object)); }
	UIW_POP_UP_ITEM &operator+(UI_WINDOW_OBJECT *object) { menu.Add(object); return(*this); }
	UIW_POP_UP_ITEM &operator-(UI_WINDOW_OBJECT *object) { menu.Subtract(object); return(*this); }

protected:
	virtual EVENT_TYPE DrawItem(const UI_EVENT &event, EVENT_TYPE ccode);
};

// ----- UIW_PULL_DOWN_MENU -------------------------------------------------

class EXPORT UIW_PULL_DOWN_MENU : public UIW_WINDOW
{
	friend class EXPORT UIF_PULL_DOWN_MENU;
public:
	UIW_PULL_DOWN_MENU(int indentation = 0,
		WOF_FLAGS woFlags = WOF_BORDER | WOF_NON_FIELD_REGION | WOF_SUPPORT_OBJECT,
		WOAF_FLAGS woAdvancedFlags = WOAF_NO_FLAGS);
	UIW_PULL_DOWN_MENU(int indentation, UI_ITEM *item);
	virtual ~UIW_PULL_DOWN_MENU(void);
	virtual EVENT_TYPE Event(const UI_EVENT &event);
	virtual void *Information(INFO_REQUEST request, void *data, OBJECTID objectID = 0);

#ifdef ZIL_PERSISTENCE
	static UI_WINDOW_OBJECT *New(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object)
		{ return (new UIW_PULL_DOWN_MENU(name, file, object)); }
	UIW_PULL_DOWN_MENU(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object);
	virtual void Load(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object);
	virtual void Store(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object);
#endif

protected:
	int indentation;
};

// ----- UIW_PULL_DOWN_ITEM -------------------------------------------------

class EXPORT UIW_PULL_DOWN_ITEM : public UIW_BUTTON
{
	friend class EXPORT UIF_PULL_DOWN_ITEM;
public:
	UIW_POP_UP_MENU menu;

	UIW_PULL_DOWN_ITEM(char *text, WNF_FLAGS wnFlags = WNF_NO_FLAGS,
		USER_FUNCTION userFunction = NULL, EVENT_TYPE value = 0);
	UIW_PULL_DOWN_ITEM(char *text, WNF_FLAGS wnFlags, UI_ITEM *item);
	virtual ~UIW_PULL_DOWN_ITEM(void);
	virtual EVENT_TYPE Event(const UI_EVENT &event);
	virtual void *Information(INFO_REQUEST request, void *data, OBJECTID objectID = 0);

#ifdef ZIL_PERSISTENCE
	static UI_WINDOW_OBJECT *New(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object)
		{ return (new UIW_PULL_DOWN_ITEM(name, file, object)); }
	UIW_PULL_DOWN_ITEM(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object);
	virtual void Load(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object);
	virtual void Store(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object);
#endif

	// List members.
	UI_WINDOW_OBJECT *Add(UI_WINDOW_OBJECT *object) { return(menu.Add(object)); }
	UI_WINDOW_OBJECT *Subtract(UI_WINDOW_OBJECT *object) { return(menu.Subtract(object)); }
	UIW_PULL_DOWN_ITEM &operator+(UI_WINDOW_OBJECT *object) { menu.Add(object); return(*this); }
	UIW_PULL_DOWN_ITEM &operator-(UI_WINDOW_OBJECT *object) { menu.Subtract(object); return(*this); }

protected:
	virtual EVENT_TYPE DrawItem(const UI_EVENT &event, EVENT_TYPE ccode);
};

// ----- UIW_SYSTEM_BUTTON --------------------------------------------------

// --- SYF_FLAGS ---
typedef UIF_FLAGS SYF_FLAGS;
const SYF_FLAGS SYF_NO_FLAGS			= 0x0000;
const SYF_FLAGS SYF_GENERIC				= 0x0001;	// Generic system menu

class EXPORT UIW_SYSTEM_BUTTON : public UIW_BUTTON
{
	friend class EXPORT UIF_SYSTEM_BUTTON;
public:
	SYF_FLAGS syFlags;
	UIW_POP_UP_MENU menu;
	static char **sysPrompts;

	UIW_SYSTEM_BUTTON(SYF_FLAGS syFlags = SYF_NO_FLAGS);
	UIW_SYSTEM_BUTTON(UI_ITEM *item);
	virtual ~UIW_SYSTEM_BUTTON(void);
	virtual EVENT_TYPE Event(const UI_EVENT &event);
	static UIW_SYSTEM_BUTTON *Generic(void);
	virtual void *Information(INFO_REQUEST request, void *data, OBJECTID objectID = 0);

#if defined(ZIL_PERSISTENCE)
	static UI_WINDOW_OBJECT *New(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object)
		{ return (new UIW_SYSTEM_BUTTON(name, file, object)); }
	UIW_SYSTEM_BUTTON(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object);
	virtual void Load(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object);
	virtual void Store(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object);
#endif

	// List members.
	UI_WINDOW_OBJECT *Add(UI_WINDOW_OBJECT *object) { return(menu.Add(object)); }
	UI_WINDOW_OBJECT *Subtract(UI_WINDOW_OBJECT *object) { return(menu.Subtract(object)); }
	UIW_SYSTEM_BUTTON &operator+(UI_WINDOW_OBJECT *object) { menu.Add(object); return(*this); }
	UIW_SYSTEM_BUTTON &operator-(UI_WINDOW_OBJECT *object) { menu.Subtract(object); return(*this); }
};

// ----- UIW_STRING ---------------------------------------------------------

// --- STF_FLAGS ---
typedef UIF_FLAGS STF_FLAGS;
const STF_FLAGS STF_NO_FLAGS			= 0x0000;
const STF_FLAGS STF_VARIABLE_NAME		= 0x0001;
const STF_FLAGS STF_LOWER_CASE			= 0x0002;
const STF_FLAGS STF_UPPER_CASE			= 0x0004;
const STF_FLAGS STF_PASSWORD			= 0x0008;
const STF_FLAGS STF_SUBSTRING			= 0x8000;	// sub-string (used only for UIW_TEXT)

class EXPORT UIW_STRING : public UI_WINDOW_OBJECT
{
	friend class EXPORT UIW_TEXT;
	friend class EXPORT UIF_STRING;
public:
	STF_FLAGS stFlags;
	int insertMode;

	UIW_STRING(int left, int top, int width, char *text, int maxLength = -1,
		STF_FLAGS stFlags = STF_NO_FLAGS,
		WOF_FLAGS woFlags = WOF_BORDER | WOF_AUTO_CLEAR,
		USER_FUNCTION userFunction = NULL);
	virtual ~UIW_STRING(void);
	virtual EVENT_TYPE Event(const UI_EVENT &event);
	char *DataGet(void);
	void DataSet(char *text, int maxLength = -1);
	virtual void *Information(INFO_REQUEST request, void *data, OBJECTID objectID = 0);

#if defined(ZIL_PERSISTENCE)
	static UI_WINDOW_OBJECT *New(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object)
		{ return (new UIW_STRING(name, file, object)); }
	UIW_STRING(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object);
	virtual void Load(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object);
	virtual void Store(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object);
#endif

protected:
	int maxLength;
	char *text;

	virtual EVENT_TYPE DrawItem(const UI_EVENT &event, EVENT_TYPE ccode);
	char *ParseRange(char *buffer, char *minValue, char *maxValue);

private:
#if defined(ZIL_MSDOS)
	int length;
	int leftClip;
	int lineWidth;
	UI_REGION active;
	int cursor, beginMark, endMark;
	int scrollCount;

	char CharacterConvert(char key);
	int CursorUpdate(int newOffset, int scrollCount, int showCursor);
	UIW_STRING *NextString(void) { return ((UIW_STRING *)next); }
	UIW_STRING *PreviousString(void) { return ((UIW_STRING *)previous); }
	void StringUpdate(int insertCount, int deleteCount, int scrollCount);
#elif defined(ZIL_MOTIF)
	Pixel colorForeground, colorBackground;
#endif
};

// ----- UIW_DATE -----------------------------------------------------------

class EXPORT UIW_DATE : public UIW_STRING
{
	friend class EXPORT UIF_DATE;
public:
	static DTF_FLAGS rangeFlags;
	DTF_FLAGS dtFlags;
	static UI_ITEM *errorTable;

	UIW_DATE(int left, int top, int width, UI_DATE *date, 
		const char *range = NULL, DTF_FLAGS dtFlags = DTF_NO_FLAGS,
		WOF_FLAGS woFlags = WOF_BORDER | WOF_AUTO_CLEAR,
		USER_FUNCTION userFunction = NULL);
	virtual ~UIW_DATE(void);
	virtual EVENT_TYPE Event(const UI_EVENT &event);
	UI_DATE *DataGet(void);
	void DataSet(UI_DATE *date);
	virtual void *Information(INFO_REQUEST request, void *data, OBJECTID objectID = 0);
	virtual int Validate(int processError = TRUE);

#if defined(ZIL_PERSISTENCE)
	static UI_WINDOW_OBJECT *New(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object)
		{ return (new UIW_DATE(name, file, object)); }
	UIW_DATE(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object);
	virtual void Load(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object);
	virtual void Store(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object);
#endif

protected:
	UI_DATE *date;
	char *range;
};

// ----- UIW_FORMATTED_STRING -----------------------------------------------

// --- FMI_RESULT ---
enum FMI_RESULT
{
	FMI_OK = 0,				// Formatted string successfully imported.
	FMI_INVALID_CHARACTERS	// Formatted string has invalid characters.
};

class EXPORT UIW_FORMATTED_STRING : public UIW_STRING
{
	friend class EXPORT UIF_FORMATTED_STRING;
public:
	UIW_FORMATTED_STRING(int left, int top, int width, char *compressedText,
		char *editMask, char *deleteText,
		WOF_FLAGS woFlags = WOF_BORDER | WOF_AUTO_CLEAR,
		USER_FUNCTION userFunction = NULL);
	virtual ~UIW_FORMATTED_STRING(void);
	char *DataGet(int compressedText = FALSE);
	void DataSet(char *text);
	virtual EVENT_TYPE Event(const UI_EVENT &event);
	void Export(char *destination, int expanded);
	FMI_RESULT Import(char *source);
	virtual void *Information(INFO_REQUEST request, void *data, OBJECTID objectID = 0);

#if defined(ZIL_PERSISTENCE)
	static UI_WINDOW_OBJECT *New(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object)
		{ return (new UIW_FORMATTED_STRING(name, file, object)); }
	UIW_FORMATTED_STRING(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object);
	virtual void Load(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object);
	virtual void Store(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object);
#endif

protected:
	char *compressedText;
	char *editMask;
	char *deleteText;
};

// ----- UIW_BIGNUM ---------------------------------------------------------

class EXPORT UIW_BIGNUM : public UIW_STRING
{
	friend class EXPORT UIF_BIGNUM;
public:
	static NMF_FLAGS rangeFlags;
	NMF_FLAGS nmFlags;
	static UI_ITEM *errorTable;

	UIW_BIGNUM(int left, int top, int width, UI_BIGNUM *value,
		const char *range = NULL, NMF_FLAGS nmFlags = NMF_NO_FLAGS,
		WOF_FLAGS woFlags = WOF_BORDER | WOF_AUTO_CLEAR,
		USER_FUNCTION userFunction = NULL);
	virtual ~UIW_BIGNUM(void);
	UI_BIGNUM *DataGet(void);
	void DataSet(UI_BIGNUM *value);
	virtual EVENT_TYPE Event(const UI_EVENT &event);
	virtual void *Information(INFO_REQUEST request, void *data, OBJECTID objectID = 0);
	virtual int Validate(int processError = TRUE);

#if defined(ZIL_PERSISTENCE)
	static UI_WINDOW_OBJECT *New(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object)
		{ return (new UIW_BIGNUM(name, file, object)); }
	UIW_BIGNUM(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object);
	virtual void Load(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object);
	virtual void Store(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object);
#endif

protected:
	UI_BIGNUM *number;
	char *range;
};

// ----- UIW_INTEGER ---------------------------------------------------------

class EXPORT UIW_INTEGER : public UIW_STRING
{
	friend class EXPORT UIF_INTEGER;
public:
	NMF_FLAGS nmFlags;
	static UI_ITEM *errorTable;

	UIW_INTEGER(int left, int top, int width, int *value, 
		const char *range = NULL, NMF_FLAGS nmFlags = NMF_NO_FLAGS,
		WOF_FLAGS woFlags = WOF_BORDER | WOF_AUTO_CLEAR,
		USER_FUNCTION userFunction = NULL);
	virtual ~UIW_INTEGER(void);
	int DataGet(void);
	void DataSet(int *value);
	virtual EVENT_TYPE Event(const UI_EVENT &event);
	virtual void *Information(INFO_REQUEST request, void *data, OBJECTID objectID = 0);
	virtual int Validate(int processError = TRUE);

#if defined(ZIL_PERSISTENCE)
	static UI_WINDOW_OBJECT *New(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object)
		{ return (new UIW_INTEGER(name, file, object)); }
	UIW_INTEGER(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object);
	virtual void Load(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object);
	virtual void Store(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object);
#endif

protected:
	int *number;
	char *range;
};

// ----- UIW_REAL ---------------------------------------------------------

class EXPORT UIW_REAL : public UIW_STRING
{
	friend class EXPORT UIF_REAL;
public:
	NMF_FLAGS nmFlags;
	static UI_ITEM *errorTable;

	UIW_REAL(int left, int top, int width, double *value, 
		const char *range = NULL, NMF_FLAGS nmFlags = NMF_NO_FLAGS,
		WOF_FLAGS woFlags = WOF_BORDER | WOF_AUTO_CLEAR,
		USER_FUNCTION userFunction = NULL);
	virtual ~UIW_REAL(void);
	double DataGet(void);
	void DataSet(double *value);
	virtual EVENT_TYPE Event(const UI_EVENT &event);
	virtual void *Information(INFO_REQUEST request, void *data, OBJECTID objectID = 0);
	virtual int Validate(int processError = TRUE);

#if defined(ZIL_PERSISTENCE)
	static UI_WINDOW_OBJECT *New(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object)
		{ return (new UIW_REAL(name, file, object)); }
	UIW_REAL(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object);
	virtual void Load(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object);
	virtual void Store(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object);
#endif

protected:
	double *number;
	char *range;
};

// ----- UIW_TIME -----------------------------------------------------------

class EXPORT UIW_TIME : public UIW_STRING
{
	friend class EXPORT UIF_TIME;
public:
	static TMF_FLAGS rangeFlags;
	TMF_FLAGS tmFlags;
	static UI_ITEM *errorTable;

	UIW_TIME(int left, int top, int width, UI_TIME *time,
		const char *range = NULL, TMF_FLAGS tmFlags = TMF_NO_FLAGS,
		WOF_FLAGS woFlags = WOF_BORDER | WOF_AUTO_CLEAR,
		USER_FUNCTION userFunction = NULL);
	virtual ~UIW_TIME(void);
	virtual EVENT_TYPE Event(const UI_EVENT &event);
	UI_TIME *DataGet(void);
	void DataSet(UI_TIME *time);
	virtual void *Information(INFO_REQUEST request, void *data, OBJECTID objectID = 0);
	virtual int Validate(int processError = TRUE);

#if defined(ZIL_PERSISTENCE)
	static UI_WINDOW_OBJECT *New(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object)
		{ return (new UIW_TIME(name, file, object)); }
	UIW_TIME(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object);
	virtual void Load(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object);
	virtual void Store(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object);
#endif

protected:
	UI_TIME *time;
	char *range;
};

// ----- UIW_TEXT -----------------------------------------------------------

class EXPORT UIW_TEXT : public UIW_WINDOW
{
	friend class EXPORT UIF_TEXT;
public:
	int insertMode;

	UIW_TEXT(int left, int top, int width, int height, char *text,
		int maxLength = -1, WNF_FLAGS wnFlags = WNF_NO_WRAP,
		WOF_FLAGS woFlags = WOF_BORDER | WOF_AUTO_CLEAR,
		USER_FUNCTION userFunction = NULL);
	virtual ~UIW_TEXT(void);
	virtual EVENT_TYPE Event(const UI_EVENT &event);
	char *DataGet(void);
	void DataSet(char *text, int maxLength = -1);
	virtual void *Information(INFO_REQUEST request, void *data, OBJECTID objectID = 0);

#if defined(ZIL_PERSISTENCE)
	static UI_WINDOW_OBJECT *New(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object)
		{ return (new UIW_TEXT(name, file, object)); }
	UIW_TEXT(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object);
	virtual void Load(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object);
	virtual void Store(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object);
#endif

protected:
	int maxLength;
	char *text;

	virtual EVENT_TYPE DrawItem(const UI_EVENT &event, EVENT_TYPE ccode);

private:
#if defined(ZIL_MSDOS)
	char *topLine;
	UIW_STRING iString;

	UIW_STRING *Current(void) { return ((UIW_STRING *)current); }
	UIW_STRING *First(void) { return ((UIW_STRING *)first); }
	UIW_STRING *Last(void) { return ((UIW_STRING *)last); }
	void RecomputeFields(int updateFields, int updateStrings);
	int ResetMark(void);
#elif defined(ZIL_MOTIF)
	int parentManaged;
#endif
};

// ----- UIW_GROUP ----------------------------------------------------------

class EXPORT UIW_GROUP : public UIW_WINDOW
{
	friend class EXPORT UIF_GROUP;
public:
	UIW_GROUP(int left, int top, int width, int height, char *text,
		WNF_FLAGS wnFlags = WNF_AUTO_SELECT, WOF_FLAGS woFlags = WOF_NO_FLAGS);
	virtual ~UIW_GROUP(void);
	virtual EVENT_TYPE Event(const UI_EVENT &event);
	char *DataGet(void);
	void DataSet(char *text);
	virtual void *Information(INFO_REQUEST request, void *data, OBJECTID objectID = 0);

#if defined(ZIL_MOTIF)
	Widget labelWidget;
#endif
#if defined(ZIL_PERSISTENCE)
	static UI_WINDOW_OBJECT *New(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object)
		{ return (new UIW_GROUP(name, file, object)); }
	UIW_GROUP(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object);
	virtual void Load(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object);
	virtual void Store(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object);
#endif

protected:
	char *text;

#if defined(ZIL_MOTIF)
	virtual void RegionMax(UI_WINDOW_OBJECT *object);
#endif
};

// ----- UIW_VT_LIST --------------------------------------------------------

class EXPORT UIW_VT_LIST : public UIW_WINDOW
{
	friend class EXPORT UIW_COMBO_BOX;
	friend class EXPORT UIF_VT_LIST;
public:
	UIW_VT_LIST(int left, int top, int width, int height,
		int (*compareFunction)(void *element1, void *element2) = NULL, 
		WNF_FLAGS wnFlags = WNF_NO_WRAP, WOF_FLAGS woFlags = WOF_BORDER,
		WOAF_FLAGS woAdvancedFlags = WOAF_NO_FLAGS);
	UIW_VT_LIST(int left, int top, int width, int height,
		int (*compareFunction)(void *element1, void *element2), 
		WOF_FLAGS flagSetting, UI_ITEM *item);
	virtual ~UIW_VT_LIST(void);
	virtual void Destroy(void);
	virtual EVENT_TYPE Event(const UI_EVENT &event);
	virtual void *Information(INFO_REQUEST request, void *data, OBJECTID objectID = 0);
#if defined(ZIL_MOTIF)
	virtual void RegionMax(UI_WINDOW_OBJECT *object);
#endif

#if defined(ZIL_PERSISTENCE)
	static UI_WINDOW_OBJECT *New(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object)
		{ return (new UIW_VT_LIST(name, file, object)); }
	UIW_VT_LIST(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object);
	virtual void Load(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object);
	virtual void Store(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object);
#endif
};

// ----- UIW_HZ_LIST ---------------------------------------------------------

class EXPORT UIW_HZ_LIST : public UIW_WINDOW
{
	friend class EXPORT UIF_HZ_LIST;
public:
	UIW_HZ_LIST(int left, int top, int width, int height,
		int cellWidth, int cellHeight,
		int (*compareFunction)(void *element1, void *element2) = NULL,
		WNF_FLAGS wnFlags = WNF_NO_WRAP, WOF_FLAGS woFlags = WOF_BORDER,
		WOAF_FLAGS woAdvancedFlags = WOAF_NO_FLAGS);
	UIW_HZ_LIST(int left, int top, int width, int height,
		int (*compareFunction)(void *element1, void *element2),
		WOF_FLAGS flagSetting, UI_ITEM *item);
	virtual ~UIW_HZ_LIST(void);
	virtual void Destroy(void);
	virtual EVENT_TYPE Event(const UI_EVENT &event);
	virtual void *Information(INFO_REQUEST request, void *data, OBJECTID objectID = 0);

#if defined(ZIL_PERSISTENCE)
	static UI_WINDOW_OBJECT *New(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object)
		{ return (new UIW_HZ_LIST(name, file, object)); }
	UIW_HZ_LIST(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object);
	virtual void Load(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object);
	virtual void Store(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object);
#endif

protected:
	int cellWidth;
	int cellHeight;
};

// ----- UIW_COMBO_BOX ------------------------------------------------------

// --- INFO_REQUEST ---
const INFO_REQUEST RESET_SELECTION		= 0x0200;

class EXPORT UIW_COMBO_BOX : public UIW_WINDOW
{
	friend class EXPORT UIF_COMBO_BOX;
public:
	UIW_VT_LIST list;

	UIW_COMBO_BOX(int left, int top, int width, int height,
		int (*compareFunction)(void *element1, void *element2) = NULL, 
		WNF_FLAGS wnFlags = WNF_NO_WRAP,
		WOF_FLAGS woFlags = WOF_NO_FLAGS,
		WOAF_FLAGS woAdvancedFlags = WOAF_NO_FLAGS);
	UIW_COMBO_BOX(int left, int top, int width, int height,
		int (*compareFunction)(void *element1, void *element2), 
		WOF_FLAGS flagSetting, UI_ITEM *item);
	virtual ~UIW_COMBO_BOX(void);
	virtual void Destroy(void);
	virtual EVENT_TYPE Event(const UI_EVENT &event);
	virtual void *Information(INFO_REQUEST request, void *data, OBJECTID objectID = 0);

#if defined(ZIL_PERSISTENCE)
	static UI_WINDOW_OBJECT *New(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object)
		{ return (new UIW_COMBO_BOX(name, file, object)); }
	UIW_COMBO_BOX(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object);
	virtual void Load(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object);
	virtual void Store(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object);
#endif

	// List members.
	UI_WINDOW_OBJECT *Add(UI_WINDOW_OBJECT *object) { return (list.Add(object)); }
	int Count(void) { return (list.Count()); };
	UI_WINDOW_OBJECT *Current(void) { return ((UI_WINDOW_OBJECT *)list.Current()); }
	UI_WINDOW_OBJECT *First(void) { return ((UI_WINDOW_OBJECT *)list.First()); }
	UI_WINDOW_OBJECT *Get(int index) { return ((UI_WINDOW_OBJECT *)list.UI_LIST::Get(index)); }
	int Index(UI_WINDOW_OBJECT const *element) { return (list.Index(element)); }
	UI_WINDOW_OBJECT *Last(void) { return((UI_WINDOW_OBJECT *)list.Last()); }
	UI_WINDOW_OBJECT *Subtract(UI_WINDOW_OBJECT *object) { return(list.Subtract(object)); }
	UIW_COMBO_BOX &operator+(UI_WINDOW_OBJECT *object) { list.Add(object); return(*this); }
	UIW_COMBO_BOX &operator-(UI_WINDOW_OBJECT *object) { list.Subtract(object); return(*this); }

private:
#if defined(ZIL_MSDOS)
	UIW_STRING *string;
	UIW_BUTTON *button;
#endif
	char *text;
};

#if defined(ZIL_MSDOS)
// Private DOS class definitions for combo support objects.
class UIW_COMBO_BUTTON : public UIW_BUTTON
{
public:
	UIW_COMBO_BUTTON(void) : UIW_BUTTON(0, 0, 0, NULL, BTF_NO_TOGGLE,
		WOF_JUSTIFY_CENTER | WOF_BORDER)
		{ woAdvancedFlags |= WOAF_NON_CURRENT; depth = 1; }
	~UIW_COMBO_BUTTON(void) { text = NULL; bitmapArray = NULL; }
	virtual EVENT_TYPE Event(const UI_EVENT &event);
};

class UIW_COMBO_STRING : public UIW_STRING
{
public:
	UIW_COMBO_STRING(void) : UIW_STRING(0, 0, 0, NULL, 128, STF_NO_FLAGS, WOF_BORDER) { }
	virtual EVENT_TYPE Event(const UI_EVENT &event);
};
#endif

// ----- UIW_SCROLL_BAR -----------------------------------------------------

// --- sbFlags ---
typedef UIF_FLAGS SBF_FLAGS;
const SBF_FLAGS SBF_NO_FLAGS			= 0x0000;
const SBF_FLAGS SBF_CORNER				= 0x0001;
const SBF_FLAGS SBF_VERTICAL			= 0x0002;
const SBF_FLAGS SBF_HORIZONTAL			= 0x0004;
const SBF_FLAGS SBF_SLIDER				= 0x0008;
const SBF_FLAGS SBF_SCALE				= 0x0008;

class EXPORT UIW_SCROLL_BAR : public UIW_WINDOW
{
	friend class EXPORT UIF_SCROLL_BAR;
public:
	SBF_FLAGS sbFlags;

	UIW_SCROLL_BAR(int left, int top, int width, int height,
		SBF_FLAGS sbFlags = SBF_VERTICAL,
		WOF_FLAGS woFlags = WOF_BORDER | WOF_SUPPORT_OBJECT | WOF_NON_FIELD_REGION);
	UIW_SCROLL_BAR(int left, int top, int width, int height,
		UI_SCROLL_INFORMATION *scroll, SBF_FLAGS sbFlags = SBF_VERTICAL,
		WOF_FLAGS woFlags = WOF_BORDER, WOAF_FLAGS woAdvancedFlags = WOAF_NON_CURRENT,
		USER_FUNCTION userFunction = NULL);
	virtual ~UIW_SCROLL_BAR(void);
	virtual EVENT_TYPE Event(const UI_EVENT &event);
	virtual void *Information(INFO_REQUEST request, void *data, OBJECTID objectID = 0);

#if defined(ZIL_PERSISTENCE)
	static UI_WINDOW_OBJECT *New(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object)
		{ return (new UIW_SCROLL_BAR(name, file, object)); }
	UIW_SCROLL_BAR(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object);
	virtual void Load(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object);
	virtual void Store(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object);
#endif

protected:
	UI_SCROLL_INFORMATION scroll;

private:
#if defined(ZIL_MSDOS)
	UI_WINDOW_OBJECT *lButton, *mButton, *rButton;
#endif
};

#if defined(ZIL_MSDOS)
// Private DOS class definitions for scroll support objects.
typedef UIF_FLAGS BUTTON_TYPE;
const BUTTON_TYPE VTOP_BUTTON			= 0x0001;
const BUTTON_TYPE VMIDDLE_BUTTON		= 0x0002;
const BUTTON_TYPE VBOTTOM_BUTTON		= 0x0004;
const BUTTON_TYPE HLEFT_BUTTON			= 0x0010;
const BUTTON_TYPE HMIDDLE_BUTTON		= 0x0020;
const BUTTON_TYPE HRIGHT_BUTTON			= 0x0040;

const BUTTON_TYPE VBUTTON				= 0x000F;
const BUTTON_TYPE HBUTTON				= 0x00F0;
const BUTTON_TYPE LTBUTTON				= 0x0011;
const BUTTON_TYPE MBUTTON				= 0x0022;
const BUTTON_TYPE RBBUTTON				= 0x0044;

class UIW_SCROLL_BUTTON : public UIW_BUTTON
{
	friend class EXPORT UIW_SCROLL_BAR;
public:
	BUTTON_TYPE btType;

	UIW_SCROLL_BUTTON(BUTTON_TYPE btType);
	~UIW_SCROLL_BUTTON(void);
	virtual EVENT_TYPE Event(const UI_EVENT &event);
};
#endif

// ----- UIW_TOOL_BAR -------------------------------------------------------

class EXPORT UIW_TOOL_BAR : public UIW_WINDOW
{
	friend class EXPORT UIF_TOOL_BAR;
public:
	UIW_TOOL_BAR(int left, int top, int width, int height,
		WNF_FLAGS wnFlags = WNF_NO_FLAGS, WOF_FLAGS woFlags = WOF_BORDER | WOF_NON_FIELD_REGION,
		WOAF_FLAGS woAdvancedFlags = WOAF_NO_FLAGS);
	virtual ~UIW_TOOL_BAR(void);
	virtual EVENT_TYPE Event(const UI_EVENT &event);
	virtual void *Information(INFO_REQUEST request, void *data, OBJECTID objectID = 0);

#if defined(ZIL_PERSISTENCE)
	static UI_WINDOW_OBJECT *New(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object)
		{ return (new UIW_TOOL_BAR(name, file, object)); }
	UIW_TOOL_BAR(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object);
	virtual void Load(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object);
	virtual void Store(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object);
#endif
};

// ----- UI_ERROR_SYSTEM ----------------------------------------------------

class EXPORT UI_ERROR_SYSTEM
{
public:
	static char **errorPrompts;
	static char **statusPrompts;
	UI_ERROR_SYSTEM(void);
	virtual ~UI_ERROR_SYSTEM(void);
	static void Beep(void);
#if defined(__WATCOMC__)
	UIS_STATUS ReportError(UI_WINDOW_MANAGER *windowManager,
		UIS_STATUS errorStatus, char *format, ...);
#else
	virtual UIS_STATUS ReportError(UI_WINDOW_MANAGER *windowManager,
		UIS_STATUS errorStatus, char *format, ...);
#endif
};

// ----- UI_HELP_SYSTEM -----------------------------------------------------

class EXPORT UI_HELP_SYSTEM
{
public:
	static char **helpPrompts;
	UI_HELP_SYSTEM(char *fileName, UI_WINDOW_MANAGER *windowManager = NULL,
		UI_HELP_CONTEXT helpContext = NO_HELP_CONTEXT);
	virtual ~UI_HELP_SYSTEM(void);
	virtual void DisplayHelp(UI_WINDOW_MANAGER *windowManager,
		UI_HELP_CONTEXT helpContext = NO_HELP_CONTEXT);

protected:
	UI_STORAGE *storage;
	UIW_WINDOW *helpWindow;
	UIW_TITLE *titleField;
	UIW_TEXT *messageField;
	UI_HELP_CONTEXT defaultHelpContext;
};

// ----- UI_APPLICATION -----------------------------------------------------

#if !defined(NO_ZIL_APP)
class EXPORT UI_APPLICATION
{
public:
	UI_DISPLAY *display;
	UI_EVENT_MANAGER *eventManager;
	UI_WINDOW_MANAGER *windowManager;
	UI_PATH *searchPath;

#if defined(ZIL_MSWINDOWS)
	HANDLE hInstance;
	HANDLE hPrevInstance;
	LPSTR lpszCmdLine;
	int nCmdShow;
	UI_APPLICATION(HANDLE hInstance, HANDLE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow);
#else
	int argc;
	char **argv;
	UI_APPLICATION(int argc, char **argv);
#endif	
	~UI_APPLICATION(void);
	int Main(void);
	EVENT_TYPE Control(void);
	void LinkMain(void);
#if defined(ZIL_MSDOS)
	static char **textSwitches;
#endif
};
#endif

#endif // UI_WIN_HPP

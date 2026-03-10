//	Zinc Interface Library Designer - UI_DSN.HPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#ifndef UI_DSN_HPP
#define UI_DSN_HPP

// Give frame class access to protected members.
#define UIF_WINDOW						UIW_FRAME_WINDOW
#define UIF_BORDER						UIW_FRAME_WINDOW
#define UIF_PROMPT						UIW_FRAME_WINDOW
#define UIF_BUTTON						UIW_FRAME_WINDOW
#define UIF_TITLE						UIW_FRAME_WINDOW
#define UIF_MAXIMIZE_BUTTON				UIW_FRAME_WINDOW
#define UIF_MINIMIZE_BUTTON				UIW_FRAME_WINDOW
#define UIF_ICON						UIW_FRAME_WINDOW
#define UIF_POP_UP_MENU					UIW_FRAME_WINDOW
#define UIF_POP_UP_ITEM					UIW_FRAME_WINDOW
#define UIF_PULL_DOWN_MENU				UIW_FRAME_WINDOW
#define UIF_PULL_DOWN_ITEM				UIW_FRAME_WINDOW
#define UIF_SYSTEM_BUTTON				UIW_FRAME_WINDOW
#define UIF_STRING						UIW_FRAME_WINDOW
#define UIF_DATE						UIW_FRAME_WINDOW
#define UIF_FORMATTED_STRING			UIW_FRAME_WINDOW
#define UIF_BIGNUM						UIW_FRAME_WINDOW
#define UIF_INTEGER						UIW_FRAME_WINDOW
#define UIF_REAL						UIW_FRAME_WINDOW
#define UIF_TIME						UIW_FRAME_WINDOW
#define UIF_TEXT						UIW_FRAME_WINDOW
#define UIF_GROUP						UIW_FRAME_WINDOW
#define UIF_VT_LIST						UIW_FRAME_WINDOW
#define UIF_HZ_LIST						UIW_FRAME_WINDOW
#define UIF_COMBO_BOX					UIW_FRAME_WINDOW
#define UIF_SCROLL_BAR					UIW_FRAME_WINDOW
#define UIF_TOOL_BAR					UIW_FRAME_WINDOW
#define UIF_WINDOW_OBJECT				UIW_FRAME_WINDOW

#ifndef UI_WIN_HPP
#include <ui_win.hpp>
#endif
#if !defined(ZIL_MSWINDOWS) || !defined(WIN32)
#	define MAX_PATH		128
#endif


// Special object ID's for designer.
const OBJECTID ID_RADIO_BUTTON			= 3000;
const OBJECTID ID_CHECK_BOX				= 3001;
const OBJECTID ID_HSCROLL				= 3002;
const OBJECTID ID_VSCROLL				= 3003;
const OBJECTID ID_CSCROLL				= 3004;
const OBJECTID ID_HELP_BAR				= 3005;
const OBJECTID ID_OBJECT_LIST			= 3006;
const OBJECTID ID_STORAGE_COMBO_BOX		= 3007;
const OBJECTID ID_FLAG_FIELD			= 3008;
const OBJECTID ID_DERIVE_OBJECT			= 3009;
const OBJECTID ID_BITMAP_OBJECT			= 3010;
const OBJECTID ID_AUTO_PROMPT			= 3011;
const OBJECTID ID_HELP_TEXT				= 3012;
const OBJECTID ID_DERIVE_START			= 3500;
const OBJECTID ID_DERIVE_END			= -2;

// Designer event types.
const USER_EVENT FILE_FIRST			  	= 10000;
const USER_EVENT FILE_NEW			  	= 10000;
const USER_EVENT FILE_FOPEN			  	= 10001;
const USER_EVENT FILE_SAVE			  	= 10002;
const USER_EVENT FILE_SAVE_AS		  	= 10003;
const USER_EVENT FILE_CHANGE_DRIVE	  	= 10004;
const USER_EVENT FILE_CHANGE_DIRECTORY	= 10005;
const USER_EVENT FILE_CHANGE_FILE		= 10006;
const USER_EVENT FILE_DELETE			= 10007;
const USER_EVENT FILE_PREFERENCES		= 10009;
const USER_EVENT FILE_EXIT				= 10011;
const USER_EVENT FILE_SET_NAME			= 10012;
const USER_EVENT FILE_CHANGE_PATH	  	= 10013;
const USER_EVENT FILE_SELECTED		  	= 10014;
const USER_EVENT FILE_LAST			  	= 10014;

const USER_EVENT EDIT_FIRST				= 10100;
const USER_EVENT EDIT_OBJECT			= 10100;
const USER_EVENT EDIT_CUT				= 10101;
const USER_EVENT EDIT_COPY				= 10102;
const USER_EVENT EDIT_PASTE				= 10103;
const USER_EVENT EDIT_DELETE			= 10104;
const USER_EVENT EDIT_MOVE				= 10105;
const USER_EVENT EDIT_SIZE				= 10106;
const USER_EVENT EDIT_ADVANCED			= 10107;
const USER_EVENT EDIT_LAST				= 10107;

const USER_EVENT RESOURCE_FIRST		 	= 10200;
const USER_EVENT RESOURCE_CREATE	 	= 10200;
const USER_EVENT RESOURCE_LOAD		 	= 10201;
const USER_EVENT RESOURCE_SELECT		= 10202;
const USER_EVENT RESOURCE_ADD			= 10203;
const USER_EVENT RESOURCE_EDIT		 	= 10204;
const USER_EVENT RESOURCE_CLEAR			= 10205;
const USER_EVENT RESOURCE_DELETE	 	= 10206;
const USER_EVENT RESOURCE_TEST		 	= 10207;
const USER_EVENT RESOURCE_STORE		 	= 10208;
const USER_EVENT RESOURCE_STORE_AS	 	= 10209;
const USER_EVENT RESOURCE_END_TEST	 	= 10210;
const USER_EVENT RESOURCE_SELECTED	 	= 10211;
const USER_EVENT RESOURCE_IMPORT	 	= 10212;
const USER_EVENT RESOURCE_LAST		 	= 10212;

const USER_EVENT UTIL_FIRST				= 10300;
const USER_EVENT UTIL_BITMAP			= 10300;
const USER_EVENT BITMAP_RESIZE			= 10301;
const USER_EVENT IMAGE_LIST				= 10302;
const USER_EVENT IMAGE_SELECT			= 10303;
const USER_EVENT IMAGE_DISPLAY			= 10304;
const USER_EVENT UTIL_HELP				= 10305;
const USER_EVENT CONTEXT_LIST			= 10306;
const USER_EVENT CONTEXT_SELECT			= 10307;
const USER_EVENT UTIL_LAST				= 10307;
const USER_EVENT IMAGE_SELECTED			= 10308;
const USER_EVENT IMAGE_SET_TYPE			= 10309;
const USER_EVENT IMAGE_SET_NAME			= 10310;
const USER_EVENT IMAGE_SAVE_UNDO		= 10311;
const USER_EVENT IMAGE_LIST_BITMAPS		= 10312;
const USER_EVENT IMAGE_LIST_ICONS		= 10313;

const USER_EVENT OPTION_NEW				= 10400;
const USER_EVENT OPTION_LOAD			= 10401;
const USER_EVENT OPTION_SAVE			= 10402;
const USER_EVENT OPTION_SAVE_AS			= 10403;
const USER_EVENT OPTION_DELETE			= 10404;
const USER_EVENT OPTION_EXIT			= 10405;
const USER_EVENT OPTION_OK				= 10406;
const USER_EVENT OPTION_CANCEL			= 10407;
const USER_EVENT OPTION_HELP			= 10408;
const USER_EVENT OPTION_ADD_ITEM		= 10410;
const USER_EVENT OPTION_IMPORT			= 10411;
const USER_EVENT OPTION_EXPORT			= 10412;

const USER_EVENT DIALOG_FIRST			= 10700;
const USER_EVENT DIALOG_OK				= 10700;
const USER_EVENT DIALOG_CANCEL			= 10701;
const USER_EVENT DIALOG_YES				= 10702;
const USER_EVENT DIALOG_NO				= 10703;
const USER_EVENT DIALOG_ABORT			= 10704;
const USER_EVENT DIALOG_RETRY			= 10705;
const USER_EVENT DIALOG_LAST			= 10706;

const USER_EVENT IMAGE_PENCIL			= 10800;
const USER_EVENT IMAGE_BRUSH			= 10801;
const USER_EVENT IMAGE_LINE				= 10802;
const USER_EVENT IMAGE_RECTANGLE		= 10803;
const USER_EVENT IMAGE_FILL_RECTANGLE	= 10804;
const USER_EVENT IMAGE_ELLIPSE			= 10805;
const USER_EVENT IMAGE_FILL_ELLIPSE		= 10806;
const USER_EVENT IMAGE_FILL				= 10807;
const USER_EVENT IMAGE_CLEAR			= 10808;
const USER_EVENT IMAGE_UNDO				= 10809;
const USER_EVENT IMAGE_GRID				= 10810;
const USER_EVENT IMAGE_BRUSH_SIZE		= 10811;

const USER_EVENT D_SET_TEXT				= 10800;
const USER_EVENT D_DELETE_OBJECT		= 10802;
const USER_EVENT D_DRAG_UP 				= 10803;
const USER_EVENT D_DRAG_DOWN			= 10804;

const USER_EVENT D_CONFIG_CHANGED		= 9999;
const USER_EVENT D_UPDATE_STATUS		= 9998;

// Support Objects ------------------------------------------------------------

class HELP_BAR : public UI_WINDOW_OBJECT
{
public:
	HELP_BAR(void);

	virtual EVENT_TYPE DrawItem(const UI_EVENT &event, EVENT_TYPE ccode);
	EVENT_TYPE Event(const UI_EVENT &event);
	void Update(UI_WINDOW_OBJECT *helpObject);

	static UI_WINDOW_OBJECT *New(const char *, UI_STORAGE *, UI_STORAGE_OBJECT *)
	{ return new HELP_BAR(); }
	virtual void Store(const char *, UI_STORAGE *, UI_STORAGE_OBJECT *)
	{}

protected:
	UI_HELP_CONTEXT helpContext;
	char *text;
};

// Control Window  ------------------------------------------------------------

class EXPORT CONTROL_WINDOW : public UIW_WINDOW
{
public:
	static int testMode;
	static int saveNeeded;
	static UI_STORAGE *storage;
	static UI_ITEM *deriveTable;
	static void SaveFile(char *newName, int memErr = FALSE);

	CONTROL_WINDOW(char *fileName);
	~CONTROL_WINDOW(void);
	virtual EVENT_TYPE Event(const UI_EVENT &event);

protected:
	// Child objects.
	UIW_PULL_DOWN_MENU *mainMenu;
	UIW_TOOL_BAR *toolBar;
	UIW_TOOL_BAR *statusBar;
	UIW_STRING *objectString;
	UIW_STRING *idString;
	UIW_STRING *posString;
	UIW_STRING *sizeString;
	UIW_STRING *placeString;
	HELP_BAR *helpBar;

	// Editing related data.
	int paste;
	int place;
	OBJECTID placeID;
	UI_WINDOW_OBJECT *currentObject;

	// Event handling functons.
	void FileOption(const UI_EVENT &event);
	void EditOption(const UI_EVENT &event);
	void ResourceOption(const UI_EVENT &event);
	void UtilOption(const UI_EVENT &event);

	// File related functions.
	void FileAction(EVENT_TYPE action, char *fileName = NULL);
	void ResourceAction(EVENT_TYPE action);
	void LoadDeriveTable(void);

	// Update function.
	int BuildWindow(void);
	void UpdateStatusBar(void);
	void UpdatePlaceString(OBJECTID objectID);
};

class EXPORT FILE_WINDOW : public UIW_WINDOW
{
public:
	FILE_WINDOW(const char *name, char *title, char *_fileMask = NULL);
	virtual EVENT_TYPE Event(const UI_EVENT &event);
	const char *FileName(void)
	{ return (char *)fileField->Information(GET_TEXT, NULL); }

protected:
	char fileMask[MAX_PATH];
	UIW_STRING *fileField;
	UIW_STRING *directoryField;
	UIW_VT_LIST *driveList;
	UIW_VT_LIST *directoryList;
	UIW_VT_LIST *fileList;
	HELP_BAR *helpBar;
};

class RESOURCE_WINDOW : public UIW_WINDOW
{
public:
  	RESOURCE_WINDOW(char *title);
	EVENT_TYPE Event(const UI_EVENT &event);

	char *ResourceName(void)
	{ return nameField->DataGet(); }
protected:
	UIW_STRING *nameField;
	UIW_HZ_LIST *resourceList;
	HELP_BAR *helpBar;
};

// Configuration (preferences) --------------------------------------------

const USHORT TOOL_BITMAP 				= 1;
const USHORT TOOL_TEXT 					= 2;
const USHORT TOOL_BITMAP_AND_TEXT 		= 3;

const USHORT OPTION_TOOL_BAR			= 0x0001;
const USHORT OPTION_STATUS_BAR			= 0x0002;
const USHORT OPTION_HELP_BAR			= 0x0004;
const USHORT OPTION_MENU_BITMAPS		= 0x0008;

struct ZINC_SIGNATURE
{
	char copyrightNotice[64];
  	UCHAR majorVersion;
	UCHAR minorVersion;
	USHORT magicNumber;
};

struct TOOL_BUTTON_DATA
{
	char objectName[32];
  	char bitmapName[32];
	USHORT createID;
  	int used;
};

class TOOL_BUTTON_ELEMENT : public UI_ELEMENT
{
public:
	TOOL_BUTTON_ELEMENT(TOOL_BUTTON_DATA &_data)
	{ data = _data; }
	TOOL_BUTTON_DATA data;
};


struct CONFIG_DATA
{
	ZINC_SIGNATURE signature;
	USHORT options;
	USHORT backups;
	USHORT numeratorX;
	USHORT denominatorX;
	USHORT numeratorY;
	USHORT denominatorY;
	USHORT toolBar;
	UI_LIST toolList;
};

class PREFERENCES_WINDOW : public UIW_WINDOW
{
public:
	PREFERENCES_WINDOW(void);
	EVENT_TYPE Event(const UI_EVENT &event);
	UIW_INTEGER *backupsField;
	UIW_BUTTON *bitmapButton;
	UIW_BUTTON *textButton;
	UIW_BUTTON *bothButton;
	UIW_BUTTON *toolBarButton;
	UIW_BUTTON *statusBarButton;
	UIW_BUTTON *helpBarButton;
	UIW_BUTTON *bitmapsButton;
	UIW_HZ_LIST *objectsList;
	UIW_INTEGER *numeratorX;
	UIW_INTEGER *denominatorX;
	UIW_INTEGER *numeratorY;
	UIW_INTEGER *denominatorY;

protected:
	HELP_BAR *helpBar;
};


// Image Editor ---------------------------------------------------------
typedef UIF_FLAGS BMF_FLAGS;
const BMF_FLAGS BMF_COLOR_BITMAP		= 0x0001;
const BMF_FLAGS BMF_EDIT_BITMAP			= 0x0002;
const BMF_FLAGS BMF_VIEW_BITMAP			= 0x0004;
const BMF_FLAGS BMF_SCREEN_BITMAP		= 0x0008;
const BMF_FLAGS BMF_LT_RT_BITMAP		= 0x0010;
const BMF_FLAGS BMF_GRID				= 0x0020;
const BMF_FLAGS BMF_POS_RIGHT			= 0x0040;
const BMF_FLAGS BMF_POS_BOTTOM			= 0x0080;

class BITMAP_OBJECT;
class IMAGE_EDITOR : public UIW_WINDOW
{
	friend class IMAGE_WINDOW;
	friend class BITMAP_OBJECT;
	friend class AUTO_PROMPT;
public:
	IMAGE_EDITOR(void);
	~IMAGE_EDITOR(void);
	virtual EVENT_TYPE Event(const UI_EVENT &event);

protected:
	UI_REGION client;
	UI_STORAGE *storage;
	OBJECTID imageID;
	char imageName[MAX_PATH];
	USHORT imageWidth, imageHeight;
  	UCHAR *imageArray, *undoArray;
	int brushSize;
	EVENT_TYPE drawMode;
	UCHAR leftRightColors[2];
	UCHAR screenColor;
	BITMAP_OBJECT *colorBitmap;
	BITMAP_OBJECT *editBitmap;
	BITMAP_OBJECT *leftRightBitmap;
	BITMAP_OBJECT *screenBitmap;
	BITMAP_OBJECT *viewBitmap;

	void ImageAction(EVENT_TYPE action);
	int Import(char *fileName);
};

class BITMAP_OBJECT : public UI_WINDOW_OBJECT
{
	friend class IMAGE_EDITOR;
public:
	static BITMAP_OBJECT *New(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object)
		{ return (new BITMAP_OBJECT(name, file, object)); }

	BITMAP_OBJECT(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object);
	virtual EVENT_TYPE Event(const UI_EVENT &event);
	virtual EVENT_TYPE DrawItem(const UI_EVENT &event, EVENT_TYPE ccode);
	UI_POSITION GetRegion(int x, int y, EVENT_TYPE drawMode);
	void Pixel(int column, int row, COLOR color);
	void Line(int x1, int y1, int x2, int y2, COLOR color);
	void Rectangle(int left, int top, int right, int bottom, COLOR color, int fill);
	void Ellipse(int left, int top, int right, int bottom, COLOR color, int fill);
	void Fill(int column, int line, COLOR newColor);

protected:
	UI_REGION client;
	IMAGE_EDITOR *editor;
	USHORT width, height;
	USHORT ratioWidth, ratioHeight;
	UCHAR *bitmapArray;
};

class AUTO_PROMPT : public UIW_PROMPT
{
public:
	AUTO_PROMPT(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object) :
		UIW_PROMPT(name, file, object)
		{}
	virtual EVENT_TYPE Event(const UI_EVENT &event);
	static AUTO_PROMPT *New(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object)
		{ return (new AUTO_PROMPT(name, file, object)); }

protected:
	IMAGE_EDITOR *editor;
};

class IMAGE_WINDOW : public UIW_WINDOW
{
public:
  	IMAGE_WINDOW(char *title, IMAGE_EDITOR *editor, EVENT_TYPE action);
	EVENT_TYPE Event(const UI_EVENT &event);
	OBJECTID ImageID(void)
	{ return imageID; }
	char *ImageName(void)
	{ return nameField->DataGet(); }
	USHORT ImageWidth(void)
	{ return (USHORT)widthField->DataGet(); }
	USHORT ImageHeight(void)
	{ return (USHORT)heightField->DataGet(); }

protected:
	OBJECTID imageID;
	UIW_COMBO_BOX *typeField;
	UIW_STRING *nameField;
	UIW_HZ_LIST *imageList;
	UIW_INTEGER *widthField;
	UIW_INTEGER *heightField;
	HELP_BAR *helpBar;
};

// Help Editor -----------------------------------------------------------------

class HELP_EDITOR : public UIW_WINDOW
{
	friend class CONTEXT_WINDOW;
public:
  	HELP_EDITOR(char *_helpName = NULL);
	virtual EVENT_TYPE Event(const UI_EVENT &event);

protected:
	char *helpName;
	UIW_STRING *titleField;
	UIW_TEXT *textField;

	void HELP_EDITOR::LoadContext(void);
};

class CONTEXT_WINDOW : public UIW_WINDOW
{
public:
  	CONTEXT_WINDOW(USER_EVENT _mode, HELP_EDITOR *_editor);
	EVENT_TYPE Event(const UI_EVENT &event);
	UIW_STRING *nameField;
	UIW_HZ_LIST *contextMatrix;
	USER_EVENT mode;
	HELP_EDITOR *editor;
	HELP_BAR *helpBar;
};

// Dialog Window --------------------------------------------------------------
typedef UIF_FLAGS DIF_FLAGS;
const DIF_FLAGS DIF_OK					= 0x0001;
const DIF_FLAGS DIF_YES					= 0x0002;
const DIF_FLAGS DIF_NO					= 0x0004;
const DIF_FLAGS DIF_CANCEL				= 0x0008;
const DIF_FLAGS DIF_ABORT				= 0x0010;
const DIF_FLAGS DIF_RETRY				= 0x0020;

class DIALOG_WINDOW : public UIW_WINDOW
{
public:
	DIALOG_WINDOW(char *title, char *icon, DIF_FLAGS diFlags, char *format, ...);
	EVENT_TYPE Responce(void);
};

// Object Editing classes -----------------------------------------------------

class UIW_FRAME_WINDOW : public UIW_WINDOW
{
public:
	UIW_FRAME_WINDOW(char *name, UI_WINDOW_OBJECT *editObject,
		UI_WINDOW_OBJECT *listItem = NULL);

	EVENT_TYPE Event(const UI_EVENT &event);

protected:
	UI_WINDOW_OBJECT *editObject;
	UI_STORAGE *storage;
	UI_WINDOW_OBJECT *listItem;
	UIF_FLAGS oldFlags;
	HELP_BAR *helpBar;
};

class IMPORT_WINDOW : public UIW_WINDOW
{
public:
	IMPORT_WINDOW(UI_STORAGE *destStorage, UI_STORAGE *sourceStorage);
	~IMPORT_WINDOW(void);
	virtual EVENT_TYPE Event(const UI_EVENT &event);

protected:
	UI_STORAGE *destStorage;
	UI_STORAGE *sourceStorage;
	UIW_VT_LIST *importList;
};

class UIW_ADVANCED_FRAME : public UIW_WINDOW
{
public:
	UIW_ADVANCED_FRAME(UI_WINDOW_OBJECT *editObject);

	EVENT_TYPE Event(const UI_EVENT &event);

protected:
	UI_WINDOW_OBJECT *editObject;
};

class EXPORT UIW_DERIVE_OBJECT : public UI_WINDOW_OBJECT
{
public:
	UIW_DERIVE_OBJECT(int left, int top, int width, int height) :
		UI_WINDOW_OBJECT(left, top, width, height, WOF_NO_FLAGS, WOAF_NO_FLAGS)
		{ Information(INITIALIZE_CLASS, NULL); }
	UIW_DERIVE_OBJECT(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object) :
		UI_WINDOW_OBJECT(0, 0, 0, 0, WOF_NO_FLAGS, WOAF_NO_FLAGS)
		{ UI_WINDOW_OBJECT::Load(name, file, object); Information(INITIALIZE_CLASS, NULL); }
	virtual EVENT_TYPE Event(const UI_EVENT &event);
	virtual void *Information(INFO_REQUEST request, void *data, OBJECTID objectID = 0);

	static UI_WINDOW_OBJECT *New(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object)
		{ return (new UIW_DERIVE_OBJECT(name, file, object)); }

protected:
	EVENT_TYPE DrawItem(const UI_EVENT &event, EVENT_TYPE ccode);
};

class EXPORT FLAG_FIELD : public UIW_STRING
{
public:
	FLAG_FIELD(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object) :
		UIW_STRING(name, file, object)
		{}
	UIF_FLAGS DataGet(void);
	void DataSet(UIF_FLAGS flags);

	static UI_WINDOW_OBJECT *New(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object)
		{ return (new FLAG_FIELD(name, file, object)); }
};

class OBJECT_LIST : public UIW_VT_LIST
{
public:
	OBJECT_LIST(int left, int top, int width, int height);
	virtual EVENT_TYPE Event(const UI_EVENT &event);
	void GetObjects(UIW_WINDOW *window);
	void SaveObjects(UIW_WINDOW *window);
	void NewObject(UI_WINDOW_OBJECT *object);

	OBJECT_LIST(const char *name, UI_STORAGE *file = NULL, UI_STORAGE_OBJECT *object = NULL);
	static UI_WINDOW_OBJECT *New(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object)
		{ return (new OBJECT_LIST(name, file, object)); }

protected:
	UI_LIST deleteList;
	UI_LIST addList;
	int orderChanged;

	static EVENT_TYPE SelectChild(UI_WINDOW_OBJECT *object, UI_EVENT &event,
		EVENT_TYPE ccode);
};

class STORAGE_COMBO_BOX : public UIW_COMBO_BOX
{
public:
	STORAGE_COMBO_BOX(const char *name, UI_STORAGE *file = NULL, UI_STORAGE_OBJECT *object = NULL);
	static UI_WINDOW_OBJECT *New(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object)
		{ return (new STORAGE_COMBO_BOX(name, file, object)); }
	virtual void Store(const char *name, UI_STORAGE *file = NULL, UI_STORAGE_OBJECT *object = NULL);
	void LoadBox(char *currentName);
	void LoadBox(UI_HELP_CONTEXT helpNumber);

	char *GetName(void);
	int GetID(void);

protected:
	OBJECTID childID;
};

// ----------------------------------------------------------------------------

class DOS
{
	friend class CONTROL_WINDOW;
	friend class FILE_WINDOW;
public:
	static int criticalError;
#if defined(ZIL_MSDOS)
#if defined(__BCPLUSPLUS__) | defined(__TCPLUSPLUS__)
	static int CriticalErrorHandler(int errval, int ax, int, int);
	static void NewHandler(void);
#elif defined(__ZTC__)
	static int _cdecl CriticalErrorHandler(int *ax, int *di);
	static void NewHandler(void);
#elif defined(_MSC_VER)
//	static void CriticalErrorHandler(unsigned deverror, unsigned errcode, unsigned *);
	static void CriticalErrorHandler(unsigned deverror, unsigned errcode);
	static int NewHandler(unsigned size);
#elif defined(__WATCOMC__)
	static int CriticalErrorHandler(unsigned deverr, unsigned errcode, unsigned far *devhdr);
	static void NewHandler(void);
#endif
#endif
	static EVENT_TYPE ErrorResponce();

	DOS(void);
	~DOS(void);

protected:
	static int errorCode;
	static int errorDrive;
	static char *errorMemory;
#if defined(ZIL_MSDOS) || defined(ZIL_MSWINDOWS) || defined(ZIL_OS2)
	static int startupDrive;
#endif
	static char startupDir[MAX_PATH];
};

// Global variables -----------------------------------------------------------
extern CONFIG_DATA *_config;
extern UI_PALETTE_MAP _textPaletteTable[];
extern UI_ITEM _toolData[];

// Utilities  -----------------------------------------------------------------
int AddAllowed(UI_WINDOW_OBJECT *parent, UI_WINDOW_OBJECT *child);
UI_WINDOW_OBJECT *CreateObject(OBJECTID objectID, UIF_FLAGS flags = 0);
void GetDecimalFlags(UIW_VT_LIST *flagsList, NMF_FLAGS *nmFlags);
EVENT_TYPE ErrorResponce();
void GetFlags(UIW_VT_LIST *flagsList, UIF_FLAGS *objectFlag[], UIW_WINDOW *window = NULL);
char *HelpBarMessage(UI_HELP_CONTEXT helpContext);
void ReadConfiguration(void);
void ReplaceString(char **oldString, char *newString);
void SaveDecimalFlags(UIW_VT_LIST *flagsList, NMF_FLAGS *nmFlags);
void SaveFile(char *newName = NULL, int memErr = FALSE);
void SaveFlags(UIW_VT_LIST *flagsList, UIF_FLAGS *objectFlag[], UIW_WINDOW *window = NULL);

#if defined (_MSC_VER)
#define read _read
#define lseek _lseek
#define write _write
#define open _open
#define chdir _chdir
#define close _close
#define getcwd _getcwd
#endif

#endif // UI_DSN_HPP

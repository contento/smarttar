//	SKETCH.HPP (DRAW) - Sketch pad example program.
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA
//  May be freely copied, used and distributed.

const USER_EVENT CLEAR_BITMAP			  	= 10000;
const USER_EVENT SAVE_BITMAP			  	= 10001;
const USER_EVENT LOAD_BITMAP			  	= 10002;
const USER_EVENT IMAGE_PENCIL				= 10003;

const INFO_REQUEST FLOOD_COLOR				= 0x010B;

enum BITMAP_ID
{
	ID_COLOR_BITMAP,
	ID_EDIT_BITMAP,
	ID_VIEW_BITMAP,
  	ID_SCREEN_BITMAP,
	ID_LEFT_BITMAP,
	ID_RIGHT_BITMAP
};

class UIW_BITMAP : public UI_WINDOW_OBJECT
{
	friend class SKETCH_PAD;
public:
	UIW_BITMAP *leftObject;
	UIW_BITMAP *rightObject;
	UIW_BITMAP *screenObject;
	UIW_BITMAP *viewObject;
	UIW_BITMAP *editObject;

	UIW_BITMAP(int left, int top, char *bitmapName, BITMAP_ID bitmapID,
		int ratioWidth = 1, int ratioHeight = 1);
	virtual ~UIW_BITMAP(void);
	virtual EVENT_TYPE Event(const UI_EVENT &event);
	virtual void *Information(INFO_REQUEST request, void *data, OBJECTID objectID = 0);

protected:
	BITMAP_ID bitmapID;
	EVENT_TYPE drawStyle;
	char *bitmapName;
	int bitmapWidth;
	int bitmapHeight;
	UCHAR *bitmapArray;
	HBITMAP colorBitmap, monoBitmap;
	int ratioWidth, ratioHeight;
	static COLOR leftColor, rightColor, screenColor;

	virtual EVENT_TYPE DrawItem(const UI_EVENT &event, EVENT_TYPE ccode);
	void Pixel(int x, int y, UCHAR color);
};

class SKETCH_PAD : public UIW_WINDOW
{
	friend class IMAGE_WINDOW;
public:
  	SKETCH_PAD(int top, int left);
	virtual EVENT_TYPE Event(const UI_EVENT &event);

protected:
	int bitmapWidth, bitmapHeight;
	UCHAR *bitmapArray;
	UIW_BITMAP *colorBitmap;
	UIW_BITMAP *editBitmap;
	UIW_BITMAP *editBackground;
	UIW_BITMAP *viewBitmap;
	UIW_BITMAP *viewBackground;
	UIW_BITMAP *screenBitmap;
	UIW_BITMAP *leftColor; 
	UIW_BITMAP *rightColor;

	void LoadImage(char *storageName, char *bitmapName);
	void SaveImage(char *storageName, char *bitmapName);
};


//	WORD3.HPP (WORD) - Dictionary tutorial, using the data file.
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA
//  May be freely copied, used and distributed.

#define L_DCT_DELETE 10000
#define L_DCT_LOOKUP 10001
#define L_DCT_SAVE   10002

#define ID_DICTIONARY_ENTRY 20000

class D_ENTRY
{
public:
	int wasLoaded;
	char *word;
	char *definition;
	char *antonym;
	char *synonym;

	D_ENTRY(const char *name, UI_STORAGE *file, UIS_FLAGS sFlags = UIS_READ);
	~D_ENTRY();

	static D_ENTRY *New(const char *name, UI_STORAGE *file, UIS_FLAGS sFlags = UIS_READ);
	void Save();

private:
	UI_STORAGE_OBJECT *object;
};

class DICTIONARY : public UI_STORAGE
{
public:
	DICTIONARY(char *name) : UI_STORAGE(name, UIS_OPENCREATE | UIS_READWRITE){};
	~DICTIONARY() { Save(1); };

	D_ENTRY *Get(const char *word);
};

class DICTIONARY_WINDOW : public UIW_WINDOW
{
public:
 	DICTIONARY_WINDOW(char *dictionaryName);
	~DICTIONARY_WINDOW() { if (dictionary) delete dictionary;};

	EVENT_TYPE Event(const UI_EVENT &event);

private:
	DICTIONARY *dictionary;
	UIW_STRING *inputField;
	UIW_TEXT *definitionField;
	UIW_STRING *antonymField;
	UIW_STRING *synonymField;

	static EVENT_TYPE ButtonFunction(UI_WINDOW_OBJECT *object, UI_EVENT &event, 
       EVENT_TYPE ccode);
};

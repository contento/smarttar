//	WORD1B.HPP (WORD) - Dictionary tutorial, C to C++.
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA
//  May be freely copied, used and distributed.

class WORD : public UI_ELEMENT
{
public:
	char *string;

	WORD(const char *_string) { string = ui_strdup(_string); }
	~WORD(void) { delete string; }

	WORD *Next(void) { return ((WORD *)next); }
};

class WORD_LIST : public UI_LIST
{
public:
	WORD *First(void) { return ((WORD *)first); }
};

class D_WORD : public UI_ELEMENT
{
public:
	char *string;
	char *definition;
	WORD_LIST antonymList;
	WORD_LIST synonymList;

	D_WORD(FILE *file);
	~D_WORD(void) { delete string; delete definition; }

	D_WORD *Next(void) { return ((D_WORD *)next); }
	void Print(void);
};

class D_WORD_LIST : public UI_LIST
{
public:
	static int FindWord(void *element, void *matchData)
		{ return(ui_stricmp(((D_WORD *)element)->string, (char *)matchData)); }

	D_WORD *First(void) { return ((D_WORD *)first); }
};

class DICTIONARY : public D_WORD_LIST
{
public:
	int found;

	DICTIONARY(void);

	D_WORD *Get(const char *word) { return((D_WORD *)UI_LIST::Get(D_WORD_LIST::FindWord, (void *)word)); }
};

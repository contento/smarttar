//	LIST.HPP (WORD) - List and element class definitions. (from UI_GEN.HPP)
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA
//  May be freely copied, used and distributed.

#if !defined(TRUE)
	const int TRUE = 1;
	const int FALSE = 0;
#endif

#if !defined(NULL)
#	if defined(__TINY__) || defined(__SMALL__) || defined(__MEDIUM__)
#		define	NULL	0
#	else
#		define	NULL	0L
#	endif
#endif

class EXPORT UI_ELEMENT
{
	friend class EXPORT UI_LIST;
public:
	virtual ~UI_ELEMENT(void) { }
	UI_ELEMENT *Next(void) { return (next); }
	UI_ELEMENT *Previous(void) { return (previous); }

protected:
	UI_ELEMENT *previous, *next;

	UI_ELEMENT(void) : previous(NULL), next(NULL) { }
};

class EXPORT UI_LIST
{
	friend class EXPORT UI_LIST_BLOCK;
public:
	int (*compareFunction)(void *element1, void *element2);

	UI_LIST(int (*_compareFunction)(void *element1, void *element2) = NULL) :
		first(NULL), last(NULL), current(NULL), compareFunction(_compareFunction) { }
	virtual ~UI_LIST(void) { UI_LIST::Destroy(); }
	UI_ELEMENT *Add(UI_ELEMENT *newElement);
	UI_ELEMENT *Add(UI_ELEMENT *element, UI_ELEMENT *newElement);
	int Count(void);
	UI_ELEMENT *Current(void) { return (current); }
	virtual void Destroy(void);
	UI_ELEMENT *First(void) { return (first); }
	UI_ELEMENT *Get(int index);
	UI_ELEMENT *Get(int (*findFunction)(void *element1, void *matchData), void *matchData);
	int Index(UI_ELEMENT const *element);
	UI_ELEMENT *Last(void) { return (last); }
	void Sort(void);
	UI_ELEMENT *Subtract(UI_ELEMENT *element);
	UI_LIST &operator+(UI_ELEMENT *element) { Add(element); return (*this); }
	UI_LIST &operator-(UI_ELEMENT *element) { Subtract(element); return (*this); };

protected:
	UI_ELEMENT *first, *last, *current;
};

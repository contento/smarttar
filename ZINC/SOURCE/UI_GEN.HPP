//	Zinc Interface Library - UI_GEN.HPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#if !defined(UI_GEN_HPP)
#	define UI_GEN_HPP
#	if !defined(UI_ENV_HPP)
#		include <ui_env.hpp>
#	endif

// --- Version 2.0 and 1.0 compatibility ---
#define UI_NUMBER				UI_BIGNUM

// --- Conversion hooks for version 2.0 databases ---

#if defined(ZIL200_PERSISTENCE)
#include <z_store.hpp>
UI_ZIL200_STORAGE *_v20file = NULL;
#endif

// --- basic typedef declarations ---

#if !defined(UCHAR)
	typedef unsigned char UCHAR;
#endif
#if !defined(USHORT)
	typedef unsigned short USHORT;
#endif
#if !defined(ULONG)
	typedef unsigned long ULONG;
#endif

// --- NULL ---

#if defined(NULL)
#undef NULL
#endif
#define	NULL		0
#define NULLP(type)	((type *)0)
#define NULLF(type)	((type *)0)

// --- TRUE/FALSE ---

#if !defined(TRUE)
	const int TRUE = 1;
	const int FALSE = 0;
#endif

// --- UIF_FLAGS ---
typedef USHORT UIF_FLAGS;
const UIF_FLAGS UIF_NO_FLAGS			= 0x0000;

// --- UIS_STATUS ---
typedef USHORT UIS_STATUS;
const UIS_STATUS UIS_NO_FLAGS			= 0x0000;

// --- OBJECTID --- THESE NUMBERS CANNOT BE CHANGED! ---

typedef short OBJECTID;
const OBJECTID ID_END					= -1;	// Special value
const OBJECTID ID_BORDER				= 1;	// Simple objects: 1 through 999
const OBJECTID ID_BUTTON				= 2;
const OBJECTID ID_DATE					= 3;
const OBJECTID ID_FORMATTED_STRING		= 4;
const OBJECTID ID_ICON					= 5;
const OBJECTID ID_INTEGER				= 6;
const OBJECTID ID_MAXIMIZE_BUTTON		= 7;
const OBJECTID ID_MINIMIZE_BUTTON		= 8;
const OBJECTID ID_NUMBER				= 9;
const OBJECTID ID_PROMPT				= 11;
const OBJECTID ID_REAL					= 12;
const OBJECTID ID_STRING				= 13;
const OBJECTID ID_TEXT					= 15;
const OBJECTID ID_TIME					= 16;
const OBJECTID ID_TITLE					= 17;
const OBJECTID ID_WINDOW_OBJECT			= 18;
const OBJECTID ID_POP_UP_ITEM			= 19;
const OBJECTID ID_HELP_CONTEXT			= 20;
const OBJECTID ID_BITMAP_IMAGE			= 21;
const OBJECTID ID_ICON_IMAGE			= 22;
const OBJECTID ID_GROUP					= 23;
const OBJECTID ID_BIGNUM				= 24;

const OBJECTID ID_MATRIX				= 1000;		// Complex objects: 1000 through 1999
const OBJECTID ID_LIST					= 1001;
const OBJECTID ID_POP_UP_MENU			= 1002;
const OBJECTID ID_PULL_DOWN_ITEM		= 1003;
const OBJECTID ID_PULL_DOWN_MENU		= 1004;
const OBJECTID ID_SCROLL_BAR			= 1005;
const OBJECTID ID_SYSTEM_BUTTON			= 1006;
const OBJECTID ID_WINDOW				= 1007;
const OBJECTID ID_COMBO_BOX				= 1008;
const OBJECTID ID_TOOL_BAR				= 1009;
const OBJECTID ID_VT_LIST				= 1010;
const OBJECTID ID_VLIST					= 1010;
const OBJECTID ID_HZ_LIST				= 1011;
const OBJECTID ID_HLIST					= 1011;

const OBJECTID ID_DISPLAY				= 2000;		// Special types: 2000 through 2999
const OBJECTID ID_EVENT_MANAGER			= 2001;
const OBJECTID ID_WINDOW_MANAGER		= 2002;
const OBJECTID ID_OUTLINE				= 2100;
const OBJECTID ID_WHITE_SHADOW			= 2101;
const OBJECTID ID_LIGHT_SHADOW			= 2102;
const OBJECTID ID_DARK_SHADOW			= 2103;
const OBJECTID ID_BLACK_SHADOW			= 2104;
const OBJECTID ID_MENU					= 2105;
const OBJECTID ID_MENU_ITEM				= 2106;
const OBJECTID ID_HOT_KEY				= 2107;

// Designer types: 3000 through 3999

// Composite class types: 4000 through 4999

// User defined class types: 5000+

// --- INFO_REQUEST ---
typedef unsigned INFO_REQUEST;

// ----- UI_ELEMENT ---------------------------------------------------------

class EXPORT UI_ELEMENT
{
	friend class EXPORT UI_LIST;
public:
	virtual ~UI_ELEMENT(void);
	virtual void *Information(INFO_REQUEST request, void *data, OBJECTID objectID = 0);
	int ListIndex(void);
	UI_ELEMENT *Next(void) { return (next); }
	UI_ELEMENT *Previous(void) { return (previous); }

protected:
	UI_ELEMENT *previous, *next;

	UI_ELEMENT(void);
};

// ----- UI_LIST ------------------------------------------------------------

class EXPORT UI_LIST
{
	friend class EXPORT UI_LIST_BLOCK;
public:
	int (*compareFunction)(void *element1, void *element2);

	UI_LIST(int (*_compareFunction)(void *element1, void *element2) = NULL);
	virtual ~UI_LIST(void);
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
	void SetCurrent(UI_ELEMENT *element) { current = element; }
	void Sort(void);
	UI_ELEMENT *Subtract(UI_ELEMENT *element);
	UI_LIST &operator+(UI_ELEMENT *element) { Add(element); return (*this); }
	UI_LIST &operator-(UI_ELEMENT *element) { Subtract(element); return (*this); };

protected:
	UI_ELEMENT *first, *last, *current;
};

// ----- UI_LIST_BLOCK ------------------------------------------------------

class EXPORT UI_LIST_BLOCK : public UI_LIST
{
public:
	virtual ~UI_LIST_BLOCK(void);
	UI_ELEMENT *Add(void);
	UI_ELEMENT *Add(UI_ELEMENT *element);
	int Full(void) { return (freeList.First() ? FALSE : TRUE); }
	UI_ELEMENT *Subtract(UI_ELEMENT *element);

protected:
	int noOfElements;
	void *elementArray;
	UI_LIST freeList;

	UI_LIST_BLOCK(int noOfElements, int (*compareFunction)(void *element1, void *element2) = NULL);
};

// ----- UI_INTERNATIONAL ---------------------------------------------------

const int UII_MAXPARAM				= 512;

class EXPORT UI_INTERNATIONAL
{
	friend class EXPORT UI_ERROR_SYSTEM;
public:
	static int initialized;
#if defined(ZIL_MSDOS) || defined(ZIL_MSWINDOWS) || defined(ZIL_OS2)
	static int countryCode;
#endif

	static char languageName[64];
	static char localeName[64];
	static char decimalSeparator[4];
	static char monDecimalSeparator[4];
	static char thousandsSeparator[4];
	static char monThousandsSeparator[4];
	static char currencySymbol[8];
	static char grouping[10];
	static char monGrouping[10];
	static char intCurrencySymbol[8];
	static int posCurrencyPrecedes;
	static int negCurrencyPrecedes;
	static int fractionDigits;
	static int intFractionDigits;
	static char positiveSign[4];
	static int posSignPrecedes;
	static int posSpaceSeparation;
	static char negativeSign[4];
	static int negSignPrecedes;
	static int negSpaceSeparation;

	static int dateFormat;
	static char dateSeparator[4];

	static int timeFormat;
	static char timeSeparator[4];

	static void Initialize(int forceInitialization = FALSE);
	static int sprintf(char *dst, const char *format, ...);
	static int vsprintf(char *dst, const char *format, void *args);
protected:
	static void rearrangeArgs(const char *format, const void *args,
				   char *newFormat, void *newArgs, void *toRet);
};

// ----- UI_BIGNUM ----------------------------------------------------------

// --- NMF_FLAGS ---
typedef UIF_FLAGS NMF_FLAGS;
const NMF_FLAGS NMF_NO_FLAGS			= 0x0000;
const NMF_FLAGS NMF_CURRENCY 			= 0x0002;
const NMF_FLAGS NMF_CREDIT 			= 0x0004;
const NMF_FLAGS NMF_COMMAS 			= 0x0008;
const NMF_FLAGS NMF_PERCENT 			= 0x0010;
const NMF_FLAGS NMF_SCIENTIFIC 			= 0x0020;
const NMF_FLAGS NMF_DECIMAL_FLAGS		= 0xFF00;

#define	NMF_DECIMAL(decimal)			(((decimal) + 1) << 8) 
#define	NMF_DIGITS(flags)			((((flags) >> 8) & 0xFF) - 1)

// --- NMI_RESULT ---
enum NMI_RESULT
{
	NMI_OK = 0,				// Number successfully imported.
	NMI_OUT_OF_RANGE,  		// Number out of range.
	NMI_INVALID				// Invalid number.
};

#define NUMBER_WHOLE	30
#define NUMBER_DECIMAL	8
// Warning, if DIGITS is not 4 or 8, you must rewrite "*" and "/" and "%".
#if defined(ZIL_BITS32)
#define DIGITS		8
#else
#define DIGITS		4
#endif
typedef long   ibignum;
typedef double rbignum;
#if DIGITS == 4
typedef unsigned int nm_number;
#elif DIGITS == 8
typedef unsigned long nm_number;
#endif

class EXPORT UI_BIGNUM : public UI_INTERNATIONAL
{
	friend UI_BIGNUM &abs(const UI_BIGNUM &number);
	friend UI_BIGNUM &ceil(const UI_BIGNUM &number) { return number.floor_ceil(0); }
	friend UI_BIGNUM &floor(const UI_BIGNUM &number) { return number.floor_ceil(1); }
	friend UI_BIGNUM &round(const UI_BIGNUM &number, int places = 0);
	friend UI_BIGNUM &truncate(const UI_BIGNUM &number, int places = 0);

public:
	UI_BIGNUM(void) { zero(); }
	UI_BIGNUM(ibignum value) { Import(value); }
	UI_BIGNUM(rbignum value) { Import(value); }
	UI_BIGNUM(const char *string) { Import(string, NULLP(const char), NULLP(const char)); }
	UI_BIGNUM(const UI_BIGNUM &number) { *this = number; }
	virtual ~UI_BIGNUM(void) { }
	void Export(ibignum *value);
	void Export(rbignum *value);
	void Export(char *string, NMF_FLAGS nmFlags);
	NMI_RESULT Import(ibignum value);
	NMI_RESULT Import(rbignum value);
	NMI_RESULT Import(const UI_BIGNUM &number);
	NMI_RESULT Import(const char *string, const char *decimalString = NULL,
		const char *signString = NULL);

	UI_BIGNUM &operator=(const UI_BIGNUM &number);
	UI_BIGNUM &operator+(const UI_BIGNUM &number);
	UI_BIGNUM &operator-(const UI_BIGNUM &number);
	UI_BIGNUM &operator*(const UI_BIGNUM &number);
	UI_BIGNUM &operator++(void);
	UI_BIGNUM &operator--(void);
	UI_BIGNUM &operator+=(const UI_BIGNUM &number) { return *this = *this + number; }
	UI_BIGNUM &operator-=(const UI_BIGNUM &number) { return *this = *this - number; }
	int operator==(const UI_BIGNUM &number) { return compare(number, FALSE, FALSE, TRUE); }
	int operator!=(const UI_BIGNUM &number) { return compare(number, TRUE, TRUE, FALSE); }
	int operator>(const UI_BIGNUM &number) { return compare(number, TRUE, FALSE, FALSE); }
	int operator>=(const UI_BIGNUM &number) { return compare(number, TRUE, FALSE, TRUE); }
	int operator<(const UI_BIGNUM &number) { return compare(number, FALSE, TRUE, FALSE); }
	int operator<=(const UI_BIGNUM &number) { return compare(number, FALSE, TRUE, TRUE); }

private:
	int sign;
	nm_number num[(NUMBER_WHOLE+NUMBER_DECIMAL+DIGITS-1)/DIGITS];

	int do_add_loop(const nm_number *aw, int carry);
	void nines_complement(void);
	void zero(void);
	int add_same_sign(const UI_BIGNUM &b, UI_BIGNUM &result);
	int add_diff_sign(const UI_BIGNUM &b, UI_BIGNUM &result);
	UI_BIGNUM &floor_ceil(int sign) const;
	int compare(const UI_BIGNUM &b, int gt, int ls, int eq);
	void checkZero(void);
	void itob(ibignum value);
	void rtob(rbignum value);
};

// ----- UI_DATE ------------------------------------------------------------

// --- DTF_FLAGS ---
typedef UIF_FLAGS DTF_FLAGS;
const DTF_FLAGS DTF_NO_FLAGS			= 0x0000;
const DTF_FLAGS DTF_US_FORMAT		 	= 0x0001;
const DTF_FLAGS DTF_EUROPEAN_FORMAT		= 0x0002;
const DTF_FLAGS DTF_JAPANESE_FORMAT		= 0x0004;
const DTF_FLAGS DTF_MILITARY_FORMAT		= 0x0008;
const DTF_FLAGS DTF_DASH				= 0x0010;
const DTF_FLAGS DTF_SLASH				= 0x0020;
const DTF_FLAGS DTF_ALPHA_MONTH			= 0x0040;
const DTF_FLAGS DTF_DAY_OF_WEEK			= 0x0080;
const DTF_FLAGS DTF_UPPER_CASE			= 0x0100;
const DTF_FLAGS DTF_SHORT_YEAR			= 0x0200;
const DTF_FLAGS DTF_SHORT_MONTH			= 0x0400;
const DTF_FLAGS DTF_SHORT_DAY			= 0x0800;
const DTF_FLAGS DTF_ZERO_FILL			= 0x1000;
const DTF_FLAGS DTF_SYSTEM				= 0x4000;

// --- DTI_RESULT ---
enum DTI_RESULT
{
	DTI_OK = 0,				// Date successfully imported.
	DTI_INVALID,			// Invalid date value or format.
	DTI_AMBIGUOUS,			// Ambiguous month name.
	DTI_INVALID_NAME,		// Invalid month or day-of-week name.
	DTI_VALUE_MISSING, 		// Required date value not present.
	DTI_OUT_OF_RANGE  		// Date out of range.
};

class EXPORT UI_DATE : public UI_INTERNATIONAL
{
public:
	static char **monthTable;
	static char **dayTable;

	UI_DATE(void) { Import(); }
	UI_DATE(const UI_DATE &date) { Import(date); }
	UI_DATE(int year, int month, int day) { Import(year, month, day); }
	UI_DATE(const char *string, DTF_FLAGS dtFlags = DTF_NO_FLAGS) { Import(string, dtFlags); }
	UI_DATE(int packedDate) { Import(packedDate); }
	virtual ~UI_DATE(void) { }
	int DayOfWeek(void);
	int DaysInMonth(void);
	int DaysInYear(void);
	void Export(int *year, int *month, int *day, int *dayOfWeek = NULL);
	void Export(char *string, DTF_FLAGS dtFlags);
	void Export(int *packedDate);
	DTI_RESULT Import(void);
	DTI_RESULT Import(const UI_DATE &date);
	DTI_RESULT Import(int year, int month, int day);
	DTI_RESULT Import(const char *string, DTF_FLAGS dtFlags = DTF_NO_FLAGS);
	DTI_RESULT Import(int packedDate);

	long operator=(long days) { value = days; return (value); }
	long operator=(const UI_DATE &date) { value = date.value; return (value); }
	long operator+(long days) { return (value + days); }
	long operator-(long days) { return (value - days); }
	long operator-(const UI_DATE &date) { return (value - date.value); }
	long operator++(void) { value++; return (value); }
	long operator--(void) { value--; return (value); }
	void operator+=(long days) { value += days; }
	void operator-=(long days) { value -= days; }
	int operator==(const UI_DATE& date) { return (value == date.value); }
	int operator!=(const UI_DATE& date) { return (value != date.value); }
	int operator>(const UI_DATE &date) { return (value > date.value); }
	int operator>=(const UI_DATE &date) { return (value >= date.value); }
	int operator<(const UI_DATE &date) { return (value < date.value); }
	int operator<=(const UI_DATE &date) { return (value <= date.value); }

private:
	long value;
};

// ----- UI_TIME ------------------------------------------------------------

// --- TMF_FLAGS ---
typedef UIF_FLAGS TMF_FLAGS;
const TMF_FLAGS TMF_NO_FLAGS			= 0x0000;
const TMF_FLAGS TMF_SECONDS 			= 0x0001;
const TMF_FLAGS TMF_HUNDREDTHS			= 0x0002;
const TMF_FLAGS TMF_NO_HOURS			= 0x0004;
const TMF_FLAGS TMF_NO_MINUTES			= 0x0008;
const TMF_FLAGS TMF_TWELVE_HOUR			= 0x0010;
const TMF_FLAGS TMF_TWENTY_FOUR_HOUR	= 0x0020;
const TMF_FLAGS TMF_ZERO_FILL			= 0x0040;
const TMF_FLAGS TMF_COLON_SEPARATOR		= 0x0080;
const TMF_FLAGS TMF_NO_SEPARATOR		= 0x0100;
const TMF_FLAGS TMF_UPPER_CASE			= 0x0200;
const TMF_FLAGS TMF_LOWER_CASE			= 0x0400;
const TMF_FLAGS TMF_SYSTEM				= 0x0800;

// --- TMI_RESULT ---
enum TMI_RESULT
{
	TMI_OK = 0,				// Time successfully imported.
	TMI_INVALID,			// Invalid time value or format.
	TMI_OUT_OF_RANGE,		// Time out of range (used by UIW_TIME)
	TMI_VALUE_MISSING		// Required value not present.
};

class EXPORT UI_TIME : public UI_INTERNATIONAL
{
public:
	static char *amPtr;
	static char *pmPtr;

	UI_TIME(void) { Import(); }
	UI_TIME(const UI_TIME &time) { Import(time); }
	UI_TIME(int hour, int minute, int second = 0, int hundredth = 0) { Import(hour, minute, second, hundredth); }
	UI_TIME(const char *string, TMF_FLAGS tmFlags = TMF_NO_FLAGS) { Import(string, tmFlags); }
	UI_TIME(int packedTime) { Import(packedTime); }
	virtual ~UI_TIME(void) { }
 	void Export(int *hour, int *minute, int *second = NULL, int *hundredth = NULL);
	void Export(char *string, TMF_FLAGS tmFlags);
	void Export(int *packedTime);
	TMI_RESULT Import(void);
	TMI_RESULT Import(const UI_TIME &time);
 	TMI_RESULT Import(int hour, int minute, int second = 0, int hundredth = 0);
	TMI_RESULT Import(const char *string, TMF_FLAGS tmFlags);
	TMI_RESULT Import(int packedTime);

	long operator=(long hundredths) { value = hundredths; return (value); }
	long operator=(const UI_TIME &time) { value = time.value; return (value); }
	long operator+(long hundredths) { return (value + hundredths); }
	long operator+(const UI_TIME &time) { return (value + time.value); }
	long operator-(long hundredths) { return (value - hundredths); }
	long operator-(const UI_TIME &time) { return (value - time.value); }
	long operator++(void) { value++; return (value); }
	long operator--(void) { value--; return (value); }
	void operator+=(long hundredths) { value += hundredths; }
	void operator-=(long hundredths) { value -= hundredths; }
	int operator==(UI_TIME& time) { return (value == time.value); }
	int operator!=(UI_TIME& time) { return (value != time.value); }
	int operator>(UI_TIME& time) { return (value > time.value); }
	int operator>=(UI_TIME& time) { return (value >= time.value); }
	int operator<(UI_TIME& time) { return (value < time.value); }
	int operator<=(UI_TIME& time) { return (value <= time.value); }

private:
	long value;
};

// ----- UI_PATH_ELEMENT & UI_PATH ------------------------------------------

class EXPORT UI_PATH_ELEMENT : public UI_ELEMENT
{
public:
	char *pathName;

	UI_PATH_ELEMENT(char *pathName, int length = -1);
	~UI_PATH_ELEMENT(void);

	// List members.
	UI_PATH_ELEMENT *Next(void) { return((UI_PATH_ELEMENT *)next); }
	UI_PATH_ELEMENT *Previous(void) { return((UI_PATH_ELEMENT *)previous); }
};

class EXPORT UI_PATH : public UI_LIST
{
public:
	UI_PATH(char *programPath = NULL, int rememberCWD = TRUE);
	~UI_PATH(void);
	const char *FirstPathName(void)
		{ current = first; return (current ? (const char *)Current()->pathName : NULLP(const char)); };
	const char *NextPathName(void)
		{ current = current ? Current()->Next() : NULLP(UI_ELEMENT);
		  return (current ? Current()->pathName : NULLP(const char)); };

	// List members
	UI_PATH_ELEMENT *Current(void) { return((UI_PATH_ELEMENT *)current); }
	UI_PATH_ELEMENT *First(void) { return((UI_PATH_ELEMENT *)first); }
	UI_PATH_ELEMENT *Last(void) { return((UI_PATH_ELEMENT *)last); }
};

// ----- UI_STORAGE_OBJECT & UI_STORAGE -------------------------------------

// --- UIS_FLAGS ---
typedef UIF_FLAGS UIS_FLAGS;
const UIS_FLAGS UIS_READ			= 0x0001;
const UIS_FLAGS UIS_READWRITE			= 0x0002;
const UIS_FLAGS UIS_CREATE			= 0x0004;
const UIS_FLAGS UIS_OPENCREATE			= 0x0008;
const UIS_FLAGS UIS_TEMPORARY			= 0x0010;

typedef USHORT diskAddr;
#define MAX_NAME_LENGTH			64

typedef ULONG inum_t;

struct EXPORT UI_STATS_INFO
{
public:
	long size;
	long createTime;
	long modifyTime;
	USHORT useCount;
	USHORT revision;
	USHORT countryID;
	inum_t inum;
};

class EXPORT UI_STORAGE
{
	friend struct inode;
	friend struct directoryEntry;
	friend struct openFile;
	friend struct superBlock;
	friend struct cacheData;
	friend class EXPORT UI_STORAGE_OBJECT;
	friend class EXPORT UI_STORAGE_SUPPORT;
public:
	static int cacheSize;
	static UI_PATH *searchPath;
	int storageError;

	UI_STORAGE(void) { storageError = 0; fd = -1; }
	UI_STORAGE(const char *name, UIS_FLAGS pFlags = UIS_READWRITE);
	~UI_STORAGE(void);
	static void AppendFullPath(char *fullPath, const char *pathName = NULL,
		const char *fileName = NULL, const char *extension = NULL);
	static void ChangeExtension(char *name, const char *newExtension);
	int ChDir(const char *newName);
	int RenameObject(const char *oldObject, const char *newName);
	int DestroyObject(const char *name);
	char *FindFirstObject(const char *pattern);
	char *FindNextObject(void);
	char *FindFirstID(OBJECTID nObjectID);
	char *FindNextID(void);
	int Flush(void);
	int MkDir(const char *newName);
	int Save(int revisions = 0);
	int SaveAs(const char *newName, int revisions = 0);
	UI_STATS_INFO *Stats(void);
	static void StripFullPath(const char *fullPath, char *pathName = NULL,
		char *fileName = NULL, char *objectName = NULL, char *objectPathName = NULL);
	static int ValidName(const char *name, int createStorage = FALSE);
	int Version(void);
	int RmDir(const char *name);
	void StorageName(char *buff) { 	AppendFullPath(buff, pname, fname, NULL); }
	int Link(const char *path1, const char *path2);

private:
	int openLen;
	struct openFile *openFiles;
	UI_STORAGE_OBJECT *currentDirectory;
	int fd;
	char pname[MAXPATHLEN + 1];
	char fname[MAXPATHLEN + 1];
	char tname[MAXPATHLEN + 1];
	int firstTime;
	int flags;
	int modified;
	struct superBlock *sb;
	int cacheLen;
	struct cacheData *cd;
	char *cache;

	int findSlot(void);
	diskAddr allocData(void);
	void freeData(unsigned int blknum);
	inum_t allocInode(void);
	diskAddr appendInode(inum_t inum);
	int rwInode(inum_t inum, struct inode *ientry, int direction);
	void *readData(diskAddr blknum);
	void writeData(void *data, int modified);
	int checkOpen(inum_t inum);
	UI_STORAGE_OBJECT *walkPath(const char *name, int stripLast);
	void walkPartialPath(const char *pathname,
			     UI_STORAGE_OBJECT **parentdir,
			     const char **filename);
	int deallocateSpace(struct inode *ientry);
};

class EXPORT UI_STORAGE_OBJECT
{
	friend struct inode;
	friend struct directoryEntry;
	friend class EXPORT UI_STORAGE;
	friend class EXPORT UI_STORAGE_SUPPORT;
public:
	int objectError;
	OBJECTID objectID;
	char stringID[32];

	UI_STORAGE_OBJECT(void);
	UI_STORAGE_OBJECT(UI_STORAGE &file, const char *name,
			  OBJECTID nObjectID, UIS_FLAGS pFlags = UIS_READWRITE);
	~UI_STORAGE_OBJECT(void) { partialDestruct(); }
	void Touch(void);
	UI_STATS_INFO *Stats(void);
	UI_STORAGE *Storage(void) { return file; }

#if defined(ZIL_BIGENDIAN)
	int Load(short *value);
	int Load(USHORT *value);
	int Load(long *value);
	int Load(ULONG *value);
	int Store(short value);
	int Store(USHORT value);
	int Store(long value);
	int Store(ULONG value);
#else
	int Load(short *value) { return rwData(value, sizeof(*value), 0); }
	int Load(USHORT *value) { return rwData(value, sizeof(*value), 0); }
	int Load(long *value) { return rwData(value, sizeof(*value), 0); }
	int Load(ULONG *value) { return rwData(value, sizeof(*value), 0); }
	int Store(short value) { return rwData(&value, sizeof(value), 1); }
	int Store(USHORT value) { return rwData(&value, sizeof(value), 1); }
	int Store(long value) { return rwData(&value, sizeof(value), 1); }
	int Store(ULONG value) { return rwData(&value, sizeof(value), 1); }
#endif
	int Load(char *value) { return rwData(value, sizeof(*value), 0); }
	int Load(UCHAR *value) { return rwData(value, sizeof(*value), 0); }
	int Load(void *buff, int size, int length) { return rwData(buff, size*length, 0); }
	int Load(char *string, int length);
	int Load(char **string);
	int Store(char value) { return rwData(&value, sizeof(value), 1); }
	int Store(UCHAR value) { return rwData(&value, sizeof(value), 1); }
	int Store(void *buff, int size, int length) { return rwData(buff, size*length, 1); }
	int Store(const char *string);

private:
	UI_STORAGE *file;
	int inodeIndex;
	long position;
	int flags;
	diskAddr *cachedBlknum;
	diskAddr *cachedBlkptr;

	int Load(struct directoryEntry *dirent);
	int Store(const struct directoryEntry *dirent);
	int findName(const char *name);
	diskAddr getBlockPtr(diskAddr blknum);
	int rwData(void *buffer, unsigned N, int direction);
	struct openFile *checkObject(void);
	void openTheFile(UI_STORAGE &pfile, struct directoryEntry *dentry,
		struct inode *ientry, int truncate);
	void partialDestruct(void);
};

// ----- Other misellaneous definitions -------------------------------------

// --- macros, miscellaneous fixups and generic functions ---
// --- macros ---
#define FlagSet(flag1, flag2) ((flag1) & (flag2))
#define FlagsSet(flag1, flag2) (((flag1) & (flag2)) == (flag2))
#define Max(arg1, arg2) (((arg1) > (arg2)) ? (arg1) : (arg2))
#define Min(arg1, arg2) (((arg1) < (arg2)) ? (arg1) : (arg2))
#define Abs(arg) ((arg > 0) ? arg : -arg)

extern char * EXPORT ui_strdup(const char *string);	// uses new instead of malloc.
extern int EXPORT ui_strlen(const char *string);	// checks for null.
extern void EXPORT ui_strrepc(char *str, int c, int repc);
extern void EXPORT ui_strstrip(char *string, int c);
extern char * EXPORT ui_itoa(int value, char *string, int radix);
extern int ui_WildStrcmp(char *str, char *pattern);
extern int ui_stricmp(const char *a, const char *b);
extern int ui_strnicmp(const char *a, const char *b, int n);
extern void ui_strupr(char *a);
extern void ui_strlwr(char *a);

#if defined(_SUNOS4)
	void *memmove(void *s1, const void *s2, size_t n);
#endif

#endif // UI_GEN_HPP

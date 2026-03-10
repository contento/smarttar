//	MONEY.HPP (MONEY) - International currency class.
//	COPYRIGHT (C) 1990-1992.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

// Currency class ID.
#define ID_INTL_CURRENCY 3000

// Currency update events.
#define L_NEW_VALUE					10000
#define L_TRANSLATE_TO_US           10001
#define L_TRANSLATE_TO_GERMAN       10002
#define L_TRANSLATE_TO_SPANISH      10003
#define L_TRANSLATE_TO_BRITISH      10004
#define L_TRANSLATE_TO_JAPANESE     10005

// Table entry country codes.
#define ENTRY_US         0
#define ENTRY_GERMAN     1
#define ENTRY_SPANISH    2
#define ENTRY_BRITISH    3
#define ENTRY_JAPANESE   4	 

#define MAX_COUNTRIES 5

typedef struct
{
	char *currencySymbol;
	char thousandsSeparator;
	char decimalSeparator;
} countryInfo;

class EXPORT UIW_INTL_CURRENCY : public UIW_STRING
{
public:
	static NMF_FLAGS rangeFlags;
	NMF_FLAGS nmFlags;

	UIW_INTL_CURRENCY(int left, int top, int width, UI_BIGNUM *value,
		NMF_FLAGS nmFlags = NMF_CURRENCY | NMF_DECIMAL(2),
		WOF_FLAGS woFlags = WOF_BORDER | WOF_AUTO_CLEAR,
		USER_FUNCTION userFunction = NULL, int _countrySetting = ENTRY_US);
	virtual ~UIW_INTL_CURRENCY(void) {};

	virtual EVENT_TYPE Event(const UI_EVENT &event);
	virtual void *Information(INFO_REQUEST request, void *data, OBJECTID objectID = 0);
	UI_BIGNUM *DataGet();
	void DataSet(UI_BIGNUM *_number);
	void SetCountryCode(int _countryTableEntry);
	int GetCountryCode();

	void ConvertToSystemSettings(char *string);
	void ConvertToLocalSettings(char *string);

protected:
	// Default current currency setting is US.
	int countryTableEntry;
	UI_BIGNUM *number;
};



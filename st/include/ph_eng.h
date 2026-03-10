#ifndef __PH_ENG_H
#define __PH_ENG_H

#if !defined(__STRING_H)
#include <string.h>
#endif

#if !defined(__FSTREAM_H)
#include <fstream.h>
#endif

#if !defined(__LISTIMP_H)
#include <classlib\listimp.h>
#endif

#if !defined(__ST_DEFS_H)
#include <st_defs.h>
#endif

#pragma hdrstop

#if !defined(__PARSER_H)
#include <parser.h>
#endif

#if !defined(__PH_DEFS_H)
#include <ph_defs.h>
#endif

class PH_ENGINE
{
public: // ------------------------------------------------------------------
    PH_ENGINE (void);
    ~PH_ENGINE(void);
    // Definitions
    class PLACE_ENTRY
    {
	public:
/*
	// 2.22.1. delete constructor
		PLACE_ENTRY(void)
		{
		}
*/
		//
		// CITY : TARIFF, MINUTES, PERCENTAGE = NUMBERS
        CITY_NAME    Place;
		WORD         TariffNum;
/* $$$230
		double		 m_minutes;    // 2.30
		double		 m_percentage; // 2.30
*/
		class NUMBER_ENTRY
        {
		public:
            WORD          Count;
            PHONE_NUMBER *Numbers;
		} NumEntries;
        //
        WORD operator ==(const PLACE_ENTRY& entry) const
        {
            return (labs(NumEntries.Numbers[0]) == labs(entry.NumEntries.Numbers[0]));
        }

        WORD operator  <(const PLACE_ENTRY& entry) const
        {
            return (labs(NumEntries.Numbers[0]) < labs(entry.NumEntries.Numbers[0]));
        }
    };
    typedef BI_SListImp<PLACE_ENTRY>        PLACE_ENTRY_LIST;
    typedef BI_ListIteratorImp<PLACE_ENTRY> PLACE_ENTRY_LIST_ITERATOR;
    //
    class TARIFF_ENTRY
    {
	public:
        double Value;
        double TaxPercent;
    };
    class SCHEDULE_ENTRY
    {
	public:
        WORD From;
        WORD To;
        WORD Percent;
    };
    class CALL_PARAMETERS
    {
	public:
		CALL_ACCESS_HEADER AccessHeader; // only access codes.
		CALL_AREA_CODE     AreaCode;     // the digit after access codes
		CALL_ATTR          Attr;
	};
	// Services
	BOOL     Inf2Dat     (void);
	BOOL     Dat2Inf     (void);
	BOOL     LoadFromInfs(void);
	BOOL     CreatePatch (void);
	BOOL     ApplyPatch  (void);
	BOOL     SaveToInfs  (void);
	BOOL     Load        (void);
	WORD     GetFileVersion(
#if (__FHEADER==2)
		void
#endif
#if (__FHEADER>=3)
		WORD& appMajor, WORD& appMinor, WORD& appUpgrade
#endif
	)
	{
#if (__FHEADER>=3)
		appMajor   = MajorAppVersion;
		appMinor   = MinorAppVersion;
		appUpgrade = UpgradeAppVersion;
#endif
		return FileVersion;
	}
	BOOL     Save        (void);
	BOOL     Search      (PHONE const & phone, PLACE_ENTRY& entry, CALL_PARAMETERS& parameters);
	BOOL     SearchPlace (CITY_NAME const & place, PLACE_ENTRY_LIST& placeList, int nFromSource);

	inline ostream& ToInfLine   (ostream& os, PLACE_ENTRY_LIST_ITERATOR it);
	inline ostream& ToInfLine   (ostream& os, PLACE_ENTRY const & entry);
	inline ostream& EntryToLine (ostream& os, PLACE_ENTRY const & entry);

	BOOL     Add         (int from, const char * line);
	inline   PLACE_ENTRY_LIST&  GetPlaceList(int from, int slot);
	//
	static inline BOOL GetCallAttr    (const PHONE& phone, CALL_ATTR& attr, int numOfDigits = 0);
	static inline BOOL GetCallParms   (const PHONE& phone, CALL_PARAMETERS& parameters);
	static inline BOOL IsLockable     (const PHONE& phone, int numOfDigits);
	static inline BOOL IsMaxDigits    (const PHONE& phone, int numOfDigits);
	static inline BOOL IsLockedNumber (const PHONE& phone);
	static inline BOOL GetLockedNumber(      PHONE& phone, int i);
	static inline BOOL SetLockedNumber(const PHONE& phone, int i);
	static inline BOOL IsAnswerable   (const PHONE& phone, int numOfDigits, BOOL isException);

public:
	struct CallInfo
	{
		CallInfo()
			:
			date(0),
			time(0),
			elapsedTime(0L),
			nTariff(0),
			taxPercent(0.0),
			callAttr(0),
			isExtension(FALSE),
			value(0.0),
			tax(0.0),
			rawMin(0.0),
			ceilMin(0.0),
			valuePerMin(0.0),
			paidPercent(0),
			nCalls(0),
			spyState(0)
		{
			strcpy(city, "");
			strcpy(phone, "");
			strcpy(area, "");
		}
		int					date;
		int 				time;
		long 				elapsedTime;
		int 				nTariff;
		double 				taxPercent;
		WORD 				callAttr;
		BOOL 				isExtension;
		PHONE  				phone;
		CITY_NAME 			city;
		CALL_ACCESS_HEADER 	area;
		int    				nCalls;
		UCHAR  				spyState;
		double 				value;
		double 				tax;
		double 				rawMin;
		double 				ceilMin;
		double 				valuePerMin;
		int    				paidPercent;
	};

	void CalcCallValues(CallInfo & info);

private:

	double CalcCeilMinutes
	(
		double 	rawMin,
		WORD 	schedule,
		WORD 	callAttr,
		BOOL 	isExtension
	);
	int CalcCallPercent
	(
		int  date,
		int  time,
		WORD schedule,
		WORD callAttr,
		BOOL isExtension
	);

public:

	inline TARIFF_ENTRY&   GetDDNTariff  (WORD tariffNum);
	inline SCHEDULE_ENTRY& GetDDNSchedule(WORD dayType, WORD number);
	inline BOOL            IsDDNReduced  (WORD dayType, WORD time, WORD& percent);
	inline WORD	           GetDDNPercent (WORD dayType, WORD time);
	//
	inline TARIFF_ENTRY&   GetDDITariff  (WORD tariffNum);
	inline SCHEDULE_ENTRY& GetDDISchedule(WORD dayType, WORD number);
	inline BOOL            IsDDIReduced  (WORD schedule, WORD dayType, WORD time, WORD& percent);
	inline WORD            GetDDIPercent (WORD schedule, WORD dayType, WORD time);
	//
	// some classes and definitions
	//
	// --- Type of days for national calls
	enum DDN_DAY_TAG
	{
		DDN_MONDAY_FRIDAY,
		DDN_WEEKEND,
		DDN_HOLLYDAY,
	};
	// --- Type of days for international calls
	enum DDI_DAY_TAG
	{
        DDI_MONDAY_FRIDAY,
        DDI_SATURDAY,
        DDI_SUNDAY_HOLLYDAY
    };
    // --- Type of reduced schedules for international calls
	enum DDI_SCHEDULE_TAG
	{
		DDI_REDUCED_USA,
		DDI_REDUCED_OTHER,
		DDI_REDUCED_BORDER,
		DDI_REDUCED_SUBMARINE
	};
	enum FROM_SOURCE
	{
		LOCAL_SOURCE,
		DDN_SOURCE,
		DDI_SOURCE,
		ALL_SOURCES // 2.22.1
    };
private: // -----------------------------------------------------------------
    WORD FileVersion;
#if (__FHEADER>=3)
    WORD MajorAppVersion, MinorAppVersion, UpgradeAppVersion;
#endif
    class PLACE_INFO
    {
	public:
        PLACE_INFO();
        ~PLACE_INFO();
        //
        class NUMBERS_PER_LINE
        {
            friend PLACE_INFO;
		private:
			WORD         Count;
			PHONE_NUMBER Numbers[MAX_NUMBERS_PER_LINE];
		};
		class INFO_ENTRY
		{
			friend PLACE_INFO;
			inline BOOL LoadCount(fstream& file);
			inline BOOL SaveCount(fstream& file);
		private:
			WORD             Count;
			PLACE_ENTRY_LIST Entries;
		};
		WORD       Count;
		INFO_ENTRY Info[MAX_INFO_SLOTS];
		//
		BOOL     LoadFromInf    (const char *filename);
		ostream& ToInfLine      (ostream& os, PLACE_ENTRY_LIST_ITERATOR it);
		ostream& ToInfLine      (ostream& os, PLACE_ENTRY const & entry);
		ostream& EntryToLine    (ostream& os, PLACE_ENTRY const & entry);
		ostream& FormatNumbers  (ostream& os, PLACE_ENTRY const & entry);
		BOOL     Load           (fstream& file);
		BOOL     Save           (fstream& file);
		BOOL     Add            (const char *line);
		BOOL     Search         (PHONE  const & phone, PLACE_ENTRY& entry);
		BOOL     SearchPlace    (CITY_NAME const & place, PLACE_ENTRY_LIST& placeList);
		void     Flush          (void);
        WORD     GetNumOfEntries(void);
        BOOL     SaveToInf      (const char *filename);
        inline   PLACE_ENTRY_LIST& GetPlaceList(int slot);
	private:
        BOOL PartialSearch   (PHONE_NUMBER number, WORD slot, PLACE_ENTRY& entry);

        BOOL Translate       (const char *line,
							  CITY_NAME& place,
							  WORD& tariffNum,
                              double& minutes,
                              double& percentage,
							  NUMBERS_PER_LINE *numbers);
		BOOL GetPlace        (Parser::Iterator& it, CITY_NAME& place);
		BOOL GetTariffNum    (Parser::Iterator& it, WORD& tariffNum);
		BOOL GetMinutes      (Parser::Iterator& it, double& minutes);
		BOOL GetPercentage   (Parser::Iterator& it, double& percentage);
		BOOL GetNumbers      (Parser::Iterator& it, NUMBERS_PER_LINE *numbers);
		BOOL TokenIs         (Parser::Iterator& it, const char *target);
		void SkipSpaces      (Parser::Iterator& it);
        BOOL IsValidPhoneItem(const String& line);
        //
        inline BOOL LoadCount       (fstream& file);
        inline BOOL LoadEntry       (fstream& file, PLACE_ENTRY& entry);
        inline BOOL SaveCount       (fstream& file);
        inline BOOL SaveEntry       (fstream& file, PLACE_ENTRY& entry);
        inline ostream& SetInfHeader       (ostream& os);
        inline ostream& SetInfSlotSeparator(ostream& os, int slot);
    };
    //
    // these are what we store
    //
    // locked numbers
    // 10 for list and ten for ranges (5 ranges)
    static PHONE   LockedNumbers[MAX_LOCKED_NUMBERS];
    // tariffs
    TARIFF_ENTRY	 DDNTariffs [MAX_DDN_TARIFF];
    TARIFF_ENTRY	 DDITariffs [MAX_DDI_TARIFF];
    // schedules
    SCHEDULE_ENTRY DDNSchedule[MAX_DDN_SCHEDULE][MAX_DDN_DAY_TYPE];
    SCHEDULE_ENTRY DDISchedule[MAX_DDI_SCHEDULE][MAX_DDI_DAY_TYPE];
    // places
    PLACE_INFO     *LocalPlaces;
    PLACE_INFO     *DDNPlaces;
    PLACE_INFO     *DDIPlaces;
    //
    // services
    //
    // .DAT
    BOOL LoadHeader             (fstream& file);
    BOOL SaveHeader             (fstream& file);
    BOOL LoadLockedNumbers      (fstream& file);
    BOOL SaveLockedNumbers      (fstream& file);
    BOOL LoadDDNTariffs         (fstream& file);
    BOOL SaveDDNTariffs         (fstream& file);
    BOOL LoadDDITariffs         (fstream& file);
    BOOL SaveDDITariffs         (fstream& file);
    BOOL LoadDDNSchedule        (fstream& file);
    BOOL SaveDDNSchedule        (fstream& file);
    BOOL LoadDDISchedule	      (fstream& file);
    BOOL SaveDDISchedule	      (fstream& file);
    void SetDefaultLockedNumbers(void);
    void SetDefaultDDNTariffs   (void);
    void SetDefaultDDITariffs   (void);
    void SetDefaultDDNSchedule  (void);
    void SetDefaultDDISchedule  (void);
    // util
    inline PLACE_INFO *GetPlaces(int from);
    //
    inline static BOOL AnalizeFirstDigit (const PHONE& phone, CALL_ATTR &attr);
    inline static BOOL AnalizeSecondDigit(const PHONE& phone, CALL_ATTR &attr);
    inline static BOOL AnalizeThirdDigit (const PHONE& phone, CALL_ATTR &attr);
#if defined(__EDA__)
    static BOOL AnalizeFourthDigit (const PHONE& phone, CALL_ATTR &attr);
#endif
};

inline BOOL PH_ENGINE::PLACE_INFO::TokenIs(Parser::Iterator& it, const char *target)
{
	return !strcmp(it.current(), target);
}

inline BOOL PH_ENGINE::PLACE_INFO::LoadCount(fstream& file)
{
	file.read((char *)&Count, sizeof(WORD));
	return file.gcount() == sizeof(WORD);
}

inline BOOL PH_ENGINE::PLACE_INFO::SaveCount(fstream& file)
{
	file.write((char *)&Count, sizeof(WORD));
	return TRUE;
}

inline BOOL PH_ENGINE::PLACE_INFO::INFO_ENTRY::LoadCount(fstream& file)
{
	file.read((char *)&Count, sizeof(WORD));
	return file.gcount() == sizeof(WORD);
}

inline BOOL PH_ENGINE::PLACE_INFO::INFO_ENTRY::SaveCount(fstream& file)
{
	file.write((char *)&Count, sizeof(WORD));
	return TRUE;
}

#endif // __PH_ENG_H

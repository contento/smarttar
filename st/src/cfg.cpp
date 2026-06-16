//
// [ CFG.CPP ]
//

#include "stdst.h"

#include <parser.h>
#include <info.h>

// Global variables
//
// v.2.20.7 uses tariff 9
char *_DEF_TAR_FMT = "A1(%d),A2(%d),A3(%d),A4(%d),A5(%d),A6(%d),A7(%d),A8(%d),A9(%d)";
char *_COM_FMT     = "%u %u %u %s %u";
//
// local variables
//
static const char *CFG_FILENAME = "ST.CFG";
static const char *INI_FILENAME = "ST.INI";

static const WORD MAX_ID_VALUES = 0x100; // !!!

static char *forms[] = {
	"80 col. doble recibo",
	"Doble recibo preimpreso",
	"80 col. lineal",
	"80 col. un recibo",
	"40 col. doble rollo",
	"40 col. rollo simple",
	"18 col. doble rollo",
	"28 col. rollo simple",
	"80 col. EMETEL",
    "80 col. media p�gina",
};

static char *_MCARD_PRINT_FMT  = "%.1lf,%.1lf,%.1lf,%.1lf";
static char *_MCARD_SCAN_FMT   = "%lf,%lf,%lf,%lf";

// ---------------------------------------------------------------------------
//     CFG
// ---------------------------------------------------------------------------

static WORD status = CFG::NO_CFG_FILE;

WORD CFG::Load(const char *path, BOOL)
{
    FILE_NAME filename;
    SetDefault(); // only change the parameters
    if (path)
        strcpy(filename, path);
    else
        _GetAppPath(filename); // NULL implies .EXE path
    strcat(filename, INI_FILENAME);
    ifstream file(filename, ios::in);
    ENTRY *table = new ENTRY[MAX_ID_VALUES];
    memset(table, 0, sizeof(ENTRY)*MAX_ID_VALUES);
    FillCfgTable(table);
    STR512 line;
    while (file.getline(line, sizeof(STR512)))
        Line2Entry(table, line);
    Adjust();
    delete [] table;

    // Date related fields
    // adjust turn day 2.20.7f || 2.20.9 Build 4
    WORD year, month, day;
    _GetSysDate(year, month, day);
    if (day != TURN_DAY)
    {
        TURN_DAY    = day;
        TURN_NUMBER = 1;
    }
    return (status = OK);
}

WORD CFG::GetStatus(void)
{
    return status;
}
WORD CFG::Save(const char *path, BOOL)
{
    FILE_NAME filename;
    if (path)
        strcpy(filename, path);
    else
        _GetAppPath(filename); // NULL implies .EXE path
    strcat(filename, INI_FILENAME);
    if (access(filename, 0x06) != 0)
        chmod(filename, S_IREAD|S_IWRITE);
    ofstream file(filename);
    ENTRY *table = new ENTRY[MAX_ID_VALUES];
    memset(table, 0, sizeof(ENTRY)*MAX_ID_VALUES);
    FillCfgTable(table);
    char strDate[0x10];
    char strTime[0x10];
    file << ";\n; Configuraci?n [" << _GetSysDate(strDate) << ' ' << _GetSysTime(strTime) << "]\n";
    file << ";\n;\n\n";
    STR512 line;
    for (WORD offset=0; table[offset].Id; offset++)
        if (Entry2Line(table, offset, line))
            file << line << '\n';
    delete [] table;
    return OK;
}

void CFG::AdjustFooter(void)
{
    if (!strlen(P_FOOTER1) && !strlen(P_FOOTER2))
    { // both are empty ?
        P_FOOTER[0] = '\0';
        return;
    }
    char footer[0x44];
    switch (FORM)
    {
    case DR_EME:
    case DR_HALF:
    case SR_80:
    case DR_80:
    case DR_PRE:
    case LINEAL_80:
        strcpy(P_FOOTER, P_FOOTER1);
        strcat(P_FOOTER, "\n\t");
        strcat(P_FOOTER, P_FOOTER2);
        break;
    case DR_40:
    case SR_40:
        strcpy(P_FOOTER, P_FOOTER1);
        if (strlen(P_FOOTER1) >= 38)
        {
            // adjust
            strcpy(&P_FOOTER[30], "\n\t");
            strcpy(&P_FOOTER[32], &P_FOOTER1[30]);
        }
        strcat(P_FOOTER, "\n\t");
        strcpy(footer, P_FOOTER2);
        if (strlen(P_FOOTER2) >= 38)
        {
            // adjust
            strcpy(&footer[30], "\n\t");
            strcpy(&footer[32], &P_FOOTER2[30]);
        }
        strcat(P_FOOTER, footer);
        break;
    case DR_18:
        strcpy(P_FOOTER, P_FOOTER1);
        if (strlen(P_FOOTER1) >= 16)
        {
            // adjust
            strcpy(&P_FOOTER[15], "\n");
            strcpy(&P_FOOTER[16], &P_FOOTER1[15]);
            if (strlen(P_FOOTER1) >= 30)
            {
                strcpy(&P_FOOTER[31], "\n");
                strcpy(&P_FOOTER[32], &P_FOOTER1[30]);
            }
            if (strlen(P_FOOTER1) >= 45)
            {
                strcpy(&P_FOOTER[47], "\n");
                strcpy(&P_FOOTER[48], &P_FOOTER1[45]);
            }
        }
        strcat(P_FOOTER, "\n");
        strcpy(footer, P_FOOTER2);
        if (strlen(P_FOOTER2) >= 16)
        {
            // adjust
            strcpy(&footer[15], "\n");
            strcpy(&footer[16], &P_FOOTER2[15]);
            if (strlen(P_FOOTER2) >= 30)
            {
                strcpy(&footer[31], "\n");
                strcpy(&footer[32], &P_FOOTER2[30]);
            }
            if (strlen(P_FOOTER2) >= 45)
            {
                strcpy(&footer[47], "\n");
                strcpy(&footer[48], &P_FOOTER2[45]);
            }
        }
        strcat(P_FOOTER, footer);
        break;
    case SR_28:
        strcpy(P_FOOTER, P_FOOTER1);
        if (strlen(P_FOOTER1) >= 26)
        {
            // adjust
            strcpy(&P_FOOTER[20], "\n\t");
            strcpy(&P_FOOTER[22], &P_FOOTER1[20]);
            if (strlen(P_FOOTER1) >= 40)
            {
                strcpy(&P_FOOTER[42], "\n\t");
                strcpy(&P_FOOTER[44], &P_FOOTER1[40]);
            }
        }
        strcat(P_FOOTER, "\n\t");
        strcpy(footer, P_FOOTER2);
        if (strlen(P_FOOTER2) >= 26)
        {
            // adjust
            strcpy(&footer[20], "\n\t");
            strcpy(&footer[22], &P_FOOTER2[20]);
            if (strlen(P_FOOTER2) >= 40)
            {
                strcpy(&footer[42], "\n\t");
                strcpy(&footer[44], &P_FOOTER2[40]);
            }
        }
        strcat(P_FOOTER, footer);
        break;
    }
}

void CFG::AdjustHeader(void)
{
	HEADER_LINE[0] = '\0';

    if (strlen(HEADER_LINE1))
    {
    	if (FORM == DR_80)
        	strcat(HEADER_LINE, "\t");
    	strcat(HEADER_LINE, HEADER_LINE1);
        strcat(HEADER_LINE, "\n");
    }
    if (strlen(HEADER_LINE2))
    {
    	if (FORM == DR_80)
        	strcat(HEADER_LINE, "\t");
    	strcat(HEADER_LINE, HEADER_LINE2);
        strcat(HEADER_LINE, "\n");
    }
    if (strlen(HEADER_LINE3))
    {
    	if (FORM == DR_80)
        	strcat(HEADER_LINE, "\t");

    	strcat(HEADER_LINE, HEADER_LINE3);
        strcat(HEADER_LINE, "\n");
    }
    if (strlen(HEADER_LINE4))
    {
    	if (FORM == DR_80)
        	strcat(HEADER_LINE, "\t");
    	strcat(HEADER_LINE, HEADER_LINE4);
    }
}

void CFG::AdjustForm(void)
{
    if (!strcmp(P_FORM, forms[DR_80]))     FORM = DR_80;
    else if (!strcmp(P_FORM, forms[DR_PRE]))    FORM = DR_PRE;
    else if (!strcmp(P_FORM, forms[LINEAL_80])) FORM = LINEAL_80;
    else if (!strcmp(P_FORM, forms[SR_80]))     FORM = SR_80;
    else if (!strcmp(P_FORM, forms[DR_40]))     FORM = DR_40;
    else if (!strcmp(P_FORM, forms[SR_40]))     FORM = SR_40;
    else if (!strcmp(P_FORM, forms[DR_18]))     FORM = DR_18;
    else if (!strcmp(P_FORM, forms[SR_28]))     FORM = SR_28;
    else if (!strcmp(P_FORM, forms[DR_EME]))    FORM = DR_EME;
    else if (!strcmp(P_FORM, forms[DR_HALF]))   FORM = DR_HALF;
}

//
// --- for password administration
//

void CFG::setDefaultPasswords(void)
{
	// V 2.33b3 changed it.
	// strcpy(Passwords[BACKDOOR]  , "\x4E\x74\x78\x12"); // "Adi" with seed = 15
	strcpy(Passwords[BACKDOOR]  , "\x3a\x3b\x76\x61\x13"); // "5+gs" with seed = 15
	_Decrypt(Passwords[BACKDOOR], sizeof(PASSWORD)); // to adjust
    strcpy(Passwords[SUPERVISOR], "Super");
    strcpy(Passwords[USER1]     , "User1");
    strcpy(Passwords[USER2]     , "User2");
    strcpy(Passwords[OPERATOR]  , "Oper");
    strcpy(Passwords[UTIL]      , "Util"); // v.220
}



BOOL CFG::SetCurrentPassword(const char *password)
{
    // is a valid password ?
    BOOL isOk = FALSE;
    for (WORD i=0; i<NOPWD2; i++)
        if (!strcmp(password, Passwords[i]))
            isOk = TRUE;
    if (!isOk)
        return FALSE;
    strcpy(Passwords[CURRENT], password);
    return TRUE;
}

BOOL CFG::ChangePassword(const char *oldPassword, const char *newPassword)
{
#define IsCurrent(s) (!strcmp(s, Passwords[CURRENT]))
    //
    // see the policies:
    // the backdoor and the supervisor can change supervisor, user1, or user2
    // but the user1 and user2 never can change theirselves
    //
    // to change the password, there must be a current one.
    if (!IsCurrent(Passwords[BACKDOOR]) && !IsCurrent(Passwords[SUPERVISOR]))
        return FALSE;
    if (PasswordIs(oldPassword, SUPERVISOR))
    {
        strcpy(Passwords[SUPERVISOR], newPassword);
        return TRUE;
    }
    if (PasswordIs(oldPassword, USER1))
    {
        strcpy(Passwords[USER1], newPassword);
        return TRUE;
    }
    if (PasswordIs(oldPassword, USER2))
    {
        strcpy(Passwords[USER2], newPassword);
        return TRUE;
    }
    if (PasswordIs(oldPassword, OPERATOR))
    {
        strcpy(Passwords[OPERATOR], newPassword);
        return TRUE;
    }
    if (PasswordIs(oldPassword, UTIL))
    {
        strcpy(Passwords[UTIL], newPassword);
        return TRUE;
    }
    return FALSE; // still here !!!
}

BOOL CFG::isUtilPassword(const char *password)
{
    return (
               PasswordIs(password, UTIL)      ||
               PasswordIs(password, BACKDOOR)  ||
               PasswordIs(password, SUPERVISOR)
           );
}

//
// --- for INI utilities ...
//

void CFG::FillCfgTable(ENTRY *table)
{
    // fill table using a macro to expand entry
#define Entry(entry, type) 			\
	if (offset < MAX_ID_VALUES) {   	\
	table[offset].Type  = type;   	\
	table[offset].Id    = #entry; 	\
	table[offset].Value = &entry; 	\
	}                             	\
	offset++;

	WORD offset = 0;

	// [ Sistema ]
    Entry(SysGroup, ENTRY::GROUP);
    Entry(COUNTRY  , ENTRY::STRING);
    Entry(CURRENCY , ENTRY::STRING);
    Entry(CITY     , ENTRY::STRING);
    Entry(COMPANY  , ENTRY::STRING);
    Entry(ID       , ENTRY::STRING);
    Entry(OPERATOR_NAME, ENTRY::STRING); // v2.16
    //
    Entry(DEALER     , ENTRY::INTEGER|ENTRY::UNSIGNED);
	Entry(TAX_NAME   , ENTRY::STRING);
	Entry(TAX_PERCENT, ENTRY::DOUBLE);
// 2.21.8	Entry(DDN_TAX    , ENTRY::DOUBLE); // Dialog Box
// 2.21.8	Entry(DDI_TAX    , ENTRY::DOUBLE); // Dialog Box
	Entry(CLUSTERS   , ENTRY::INTEGER|ENTRY::UNSIGNED);
	Entry(SS_ID      , ENTRY::INTEGER|ENTRY::UNSIGNED);
	Entry(SS_TIME    , ENTRY::INTEGER|ENTRY::UNSIGNED);
    //
    Entry(P_PORT     , ENTRY::STRING|ENTRY::LOWER);
    Entry(P_OPERATION, ENTRY::STRING|ENTRY::LOWER);
    Entry(P_FORM     , ENTRY::STRING);
    Entry(P_FOOTER1  , ENTRY::STRING);
    Entry(P_FOOTER2  , ENTRY::STRING);
    Entry(CASH       , ENTRY::STRING|ENTRY::LOWER);
    Entry(LPT        , ENTRY::INTEGER|ENTRY::UNSIGNED);
    Entry(DOUBLE_PRN , ENTRY::INTEGER|ENTRY::UNSIGNED);
    Entry(COM        , ENTRY::STRING|ENTRY::LOWER);
	Entry(M_ROUND    , ENTRY::DOUBLE);
	Entry(MCARDS     , ENTRY::STRING|ENTRY::UPPER|ENTRY::NO_SPACES);
	// 2.30	Entry(TLX_BASE   , ENTRY::DOUBLE);
	// 2.30	Entry(INTER_TLX_BASE , ENTRY::DOUBLE);
	//
	Entry(INTERNET_TAX   , ENTRY::DOUBLE);
	Entry(INTERNET_ROUND , ENTRY::DOUBLE);
	Entry(INTERNET_TARIFF, ENTRY::DOUBLE);

	// 2.33
	Entry(CELLULAR_TAX, ENTRY::DOUBLE);
	Entry(ENGINE_KIND, ENTRY::STRING|ENTRY::LOWER|ENTRY::NO_SPACES);
	//
	Entry(USA            , ENTRY::STRING);
    // [ Aplicacion ]
    Entry(AppGroup, ENTRY::GROUP);
    Entry(MANUAL_ANSWER  , ENTRY::INTEGER|ENTRY::UNSIGNED);
    Entry(MIN_NAL        , ENTRY::INTEGER|ENTRY::UNSIGNED);
	Entry(MIN_INTER      , ENTRY::INTEGER|ENTRY::UNSIGNED);
    Entry(MIN_BORDER     , ENTRY::INTEGER|ENTRY::UNSIGNED);
    Entry(MIN_USA        , ENTRY::INTEGER|ENTRY::UNSIGNED);
    Entry(MIN_CELLULAR   , ENTRY::INTEGER|ENTRY::UNSIGNED);
	Entry(CEIL_NAL       , ENTRY::DOUBLE);
	Entry(CEIL_INTER     , ENTRY::DOUBLE);
	Entry(CEIL_BORDER    , ENTRY::DOUBLE);
	Entry(CEIL_USA       , ENTRY::DOUBLE);
	Entry(CEIL_CELLULAR  , ENTRY::DOUBLE);
	//
    Entry(N_RECEIPT   , ENTRY::LONG);
    Entry(TURN_DAY		, ENTRY::INTEGER|ENTRY::UNSIGNED);
	Entry(TURN_NUMBER	, ENTRY::INTEGER|ENTRY::UNSIGNED);
	Entry(N_DIAL_ERR  , ENTRY::INTEGER|ENTRY::UNSIGNED);
    Entry(N_COM_ERR   , ENTRY::INTEGER|ENTRY::UNSIGNED);
    Entry(MAX_COM_ERR , ENTRY::INTEGER);
    Entry(MAX_DIAL_ERR, ENTRY::INTEGER);
    Entry(DEFAULT_TARIFFS         , ENTRY::STRING|ENTRY::UPPER|ENTRY::NO_SPACES);
    Entry(APPLY_DDN_SCHEDULE      , ENTRY::INTEGER|ENTRY::UNSIGNED);
    Entry(APPLY_DDI_SCHEDULE      , ENTRY::INTEGER|ENTRY::UNSIGNED);
	Entry(GENERATE_PREPAID_RECEIPT, ENTRY::INTEGER|ENTRY::UNSIGNED);
    Entry(DOUBLE_PREPAID_RECEIPT, ENTRY::INTEGER|ENTRY::UNSIGNED);
	Entry(CACHE_SIZE, ENTRY::INTEGER|ENTRY::UNSIGNED);
	Entry(CHECK_DUPS, ENTRY::INTEGER|ENTRY::UNSIGNED);
	Entry(MINIDB  , ENTRY::INTEGER|ENTRY::UNSIGNED);
	Entry(MULTIPLE_PREPAID_CALLS, ENTRY::INTEGER|ENTRY::UNSIGNED);
    Entry(CALL_ACTUAL_COST      , ENTRY::INTEGER|ENTRY::UNSIGNED);
    Entry(NO_SOUND_WHILE_SPY    , ENTRY::INTEGER|ENTRY::UNSIGNED);
    Entry(ACTIVATE_RELAY        , ENTRY::INTEGER|ENTRY::UNSIGNED);
	Entry(RELAY_NUMBER          , ENTRY::INTEGER|ENTRY::UNSIGNED);
	Entry(VIEW_REFRESH_TIME, ENTRY::INTEGER|ENTRY::UNSIGNED);
	Entry(VIEW_PHONE, ENTRY::INTEGER|ENTRY::UNSIGNED);
	Entry(VIEW_DECIMALS, ENTRY::INTEGER|ENTRY::UNSIGNED);
	Entry(CORRECTION_TIME, ENTRY::INTEGER|ENTRY::UNSIGNED);
	Entry(HEADER_LINE1, ENTRY::STRING);
	Entry(HEADER_LINE2, ENTRY::STRING);
	Entry(HEADER_LINE3, ENTRY::STRING);
    Entry(HEADER_LINE4, ENTRY::STRING);
	Entry(HEADER_PRINT_TAXNAME, ENTRY::INTEGER|ENTRY::UNSIGNED);
    Entry(HEADER_PRINT_RECNO, ENTRY::INTEGER|ENTRY::UNSIGNED);
	Entry(RECNO_LEADING_ZEROS, ENTRY::INTEGER|ENTRY::UNSIGNED);
	Entry(RECNO_DIGITS, ENTRY::INTEGER|ENTRY::UNSIGNED);
	Entry(RECNO_LABEL, ENTRY::STRING);
	Entry(SHORT_SERIAL, ENTRY::STRING);

	// [ telefonia ]
    Entry(PhoneGroup, ENTRY::GROUP);
    Entry(T_ON_HOOK      , ENTRY::INTEGER|ENTRY::UNSIGNED);
    Entry(T_INTER_RING   , ENTRY::INTEGER|ENTRY::UNSIGNED);
    Entry(T_OFF_HOOK     , ENTRY::INTEGER|ENTRY::UNSIGNED);
    Entry(T_BREAK        , ENTRY::INTEGER|ENTRY::UNSIGNED);
    Entry(T_MAKE         , ENTRY::INTEGER|ENTRY::UNSIGNED);
    Entry(T_MAKE_MARGIN  , ENTRY::INTEGER|ENTRY::UNSIGNED);
    Entry(T_INTERDIG     , ENTRY::INTEGER|ENTRY::UNSIGNED);
    Entry(T_DTMF_FLAG    , ENTRY::INTEGER|ENTRY::UNSIGNED);
	Entry(T_DTMF_INTERDIG, ENTRY::INTEGER|ENTRY::UNSIGNED);
	Entry(T_ANSWER       , ENTRY::INTEGER|ENTRY::UNSIGNED);
    Entry(T_TALK         , ENTRY::INTEGER|ENTRY::UNSIGNED);
	Entry(T_BIAS         , ENTRY::INTEGER|ENTRY::UNSIGNED);
    Entry(T_DIAL         , ENTRY::INTEGER|ENTRY::UNSIGNED);
    Entry(T_COM          , ENTRY::INTEGER|ENTRY::UNSIGNED);
	Entry(T_STORE        , ENTRY::INTEGER|ENTRY::UNSIGNED);
	Entry(T_LOCK         , ENTRY::INTEGER|ENTRY::UNSIGNED);
    Entry(SIGNAL         , ENTRY::STRING|ENTRY::LOWER);
    //
    Entry(DialingGroup, ENTRY::GROUP);
    Entry(ACCESS_LEVELS  , ENTRY::INTEGER|ENTRY::UNSIGNED);
    Entry(SPECIAL_ACCESS , ENTRY::INTEGER|ENTRY::UNSIGNED);
    Entry(ACCESS         , ENTRY::INTEGER|ENTRY::UNSIGNED);
    Entry(OPERATOR_ACCESS, ENTRY::INTEGER|ENTRY::UNSIGNED); // v2.16
    Entry(CELLULAR_ACCESS, ENTRY::INTEGER|ENTRY::UNSIGNED);
    Entry(CELLULAR_OPERATOR_ACCESS, ENTRY::INTEGER|ENTRY::UNSIGNED); // v2.16
    Entry(EDA_ACCESS     , ENTRY::INTEGER|ENTRY::UNSIGNED);
    Entry(BORDER_ACCESS  , ENTRY::INTEGER|ENTRY::UNSIGNED);
    Entry(INTER_ACCESS   , ENTRY::INTEGER|ENTRY::UNSIGNED);
	//
	Entry(INTER_DIGITS   , ENTRY::INTEGER|ENTRY::UNSIGNED);
    Entry(BORDER_DIGITS  , ENTRY::INTEGER|ENTRY::UNSIGNED);
	Entry(CELLULAR_DIGITS, ENTRY::INTEGER|ENTRY::UNSIGNED);
	Entry(NAL_DIGITS     , ENTRY::INTEGER|ENTRY::UNSIGNED);
    Entry(LOCAL_DIGITS   , ENTRY::INTEGER|ENTRY::UNSIGNED);
    Entry(SPECIAL_DIGITS , ENTRY::INTEGER|ENTRY::UNSIGNED);
    Entry(NAL_DIGITS_MARGIN  , ENTRY::INTEGER|ENTRY::UNSIGNED);
    Entry(LOCAL_DIGITS_MARGIN, ENTRY::INTEGER|ENTRY::UNSIGNED);
    Entry(INTER_DIGITS_MARGIN, ENTRY::INTEGER|ENTRY::UNSIGNED);
	Entry(INTER_DIGITS_NOT_INCLUDED, ENTRY::INTEGER|ENTRY::UNSIGNED);
	Entry(NAL_DIGITS_NOT_INCLUDED, ENTRY::INTEGER|ENTRY::UNSIGNED);
	Entry(LOCAL_DIGITS_NOT_INCLUDED, ENTRY::INTEGER|ENTRY::UNSIGNED);
	// [ Extension ]
    Entry(ExtGroup, ENTRY::GROUP);
    Entry(E_APPLY_DDN_SCHEDULE, ENTRY::INTEGER|ENTRY::UNSIGNED);
    Entry(E_APPLY_DDI_SCHEDULE, ENTRY::INTEGER|ENTRY::UNSIGNED);
	Entry(E_DISCOUNT     , ENTRY::DOUBLE);
	Entry(E_INSTALL_COST , ENTRY::DOUBLE);
    Entry(E_LINE_COST    , ENTRY::DOUBLE);
	Entry(E_MIN_AVAILABLE, ENTRY::DOUBLE);
	Entry(E_SHOW_PHONE   , ENTRY::INTEGER|ENTRY::UNSIGNED);
    Entry(E_FIRST_EXT    , ENTRY::INTEGER|ENTRY::UNSIGNED);
    Entry(E_N_RECEIPT    , ENTRY::LONG);
    Entry(E_MIN_NAL      , ENTRY::INTEGER|ENTRY::UNSIGNED);
    Entry(E_MIN_INTER    , ENTRY::INTEGER|ENTRY::UNSIGNED);
    Entry(E_MIN_USA      , ENTRY::INTEGER|ENTRY::UNSIGNED);
	Entry(E_MIN_BORDER   , ENTRY::INTEGER|ENTRY::UNSIGNED);
    Entry(E_MIN_CELLULAR , ENTRY::INTEGER|ENTRY::UNSIGNED);
	Entry(E_CEIL_NAL     , ENTRY::DOUBLE);
	Entry(E_CEIL_INTER   , ENTRY::DOUBLE);
	Entry(E_CEIL_USA     , ENTRY::DOUBLE);
	Entry(E_CEIL_BORDER  , ENTRY::DOUBLE);
	Entry(E_CEIL_CELLULAR, ENTRY::DOUBLE);
	Entry(E_APPLY_ROUND  , ENTRY::INTEGER|ENTRY::UNSIGNED);
    Entry(CriticalGroup, ENTRY::GROUP);
    Entry(CHECK_PAUSE_KEY    , ENTRY::INTEGER|ENTRY::UNSIGNED);
    Entry(IGNORE_EXTRA_DIGITS, ENTRY::INTEGER|ENTRY::UNSIGNED);
	Entry(EXCLUSIVE_SPY      , ENTRY::INTEGER|ENTRY::UNSIGNED);
    Entry(IGNORE_PRE_ANSWER  , ENTRY::INTEGER|ENTRY::UNSIGNED);
	Entry(DETECT_INCOME      , ENTRY::INTEGER|ENTRY::UNSIGNED);
	Entry(EXIST_FALSE_ONE    , ENTRY::INTEGER|ENTRY::UNSIGNED);
    Entry(ModemGroup, ENTRY::GROUP);
    Entry(MODEM_COM             , ENTRY::INTEGER|ENTRY::UNSIGNED);
    Entry(MODEM_BASE            , ENTRY::INTEGER|ENTRY::UNSIGNED);
    Entry(MODEM_IRQ             , ENTRY::INTEGER|ENTRY::UNSIGNED);
    Entry(MODEM_BAUDS           , ENTRY::LONG|ENTRY::UNSIGNED);
	Entry(MODEM_DIAL            , ENTRY::INTEGER|ENTRY::UNSIGNED);
    Entry(MODEM_SPEAKER         , ENTRY::INTEGER|ENTRY::UNSIGNED);
    Entry(MODEM_DELAY           , ENTRY::INTEGER|ENTRY::UNSIGNED);
    Entry(MODEM_RECEIVESENDDELAY, ENTRY::INTEGER|ENTRY::UNSIGNED);
    Entry(MODEM_ACKTIME         , ENTRY::INTEGER|ENTRY::UNSIGNED);
    Entry(MODEM_MAXTIME         , ENTRY::LONG|ENTRY::UNSIGNED);
    Entry(MODEM_PHONE           , ENTRY::STRING);
    Entry(DisplayGroup, ENTRY::GROUP);
	Entry(DISPLAY_ENABLE , ENTRY::INTEGER|ENTRY::UNSIGNED);
	Entry(DISPLAY_COM    , ENTRY::INTEGER|ENTRY::UNSIGNED);
	Entry(DISPLAY_BAUDS  , ENTRY::INTEGER|ENTRY::UNSIGNED);
	Entry(DISPLAY_DEFAULT_MESSAGE, ENTRY::STRING);

	if (offset > MAX_ID_VALUES)
	{
		_ES = 0xC0D0;
	}
}

BOOL CFG::Line2Entry(const ENTRY *table, const char *line)
{
	Parser::Tokens tokens;
	Parser parser(line, tokens);
#if (__BORLANDC__ > 0x410) // > borland C++ 3.1
	if (tokens.IsEmpty())
#else
	if (tokens.isEmpty())
#endif
		return FALSE;
	Parser::Iterator iterator(tokens);
	if (!iterator)
		return FALSE;
#if (__BORLANDC__ > 0x410) // > borland C++ 3.1
	if (!strcmp(iterator.Current().c_str(), ";")) // a comment
#else
	if (!strcmp(iterator.current(), ";")) // a comment
#endif
		return TRUE;
	int offset;
#if (__BORLANDC__ > 0x410) // > borland C++ 3.1
	if ((offset = SearchId(table, iterator.Current().c_str())) == -1) // token ?
#else
	if ((offset = SearchId(table, iterator.current())) == -1) // token ?
#endif
		return FALSE;
	// search for '=' and avoid spaces after '='
#if (__BORLANDC__ > 0x410) // > borland C++ 3.1
	while (iterator && strcmp(iterator.Current().c_str(), "="))
#else
	while (iterator && strcmp(iterator.current(), "="))
#endif
		iterator++;
	if (!iterator) // bad format
		return FALSE;
	iterator++; // after '='
#if (__BORLANDC__ > 0x410) // > borland C++ 3.1
	while (iterator && !strcmp(iterator.Current().c_str(), " "))
#else
	while (iterator && !strcmp(iterator.current(), " "))
#endif
		iterator++;
	if (!iterator) // bad format
		return FALSE;

	// build line until the EOL
	STR512 strLine;
	strLine[0] = '\0';
	while (iterator)
	{
#if (__BORLANDC__ > 0x410) // > borland C++ 3.1
		strcat(strLine, iterator.Current().c_str());
#else
		strcat(strLine, iterator.current());
#endif
		iterator++;
	}

	// process
	switch (table[offset].Type & 0x00FF)
	{
	case ENTRY::INTEGER:
		if (table[offset].Type & ENTRY::UNSIGNED)
			*(UINT *)table[offset].Value = atoi(strLine);
		else
			*(WORD *)table[offset].Value = atoi(strLine);
		break;
	case ENTRY::LONG:
		*(long *)table[offset].Value = atol(strLine);
		break;
	case ENTRY::FLOAT:
		break;
	case ENTRY::DOUBLE:
		*(double *)table[offset].Value = atof(strLine);
		break;
	case ENTRY::STRING:
		{
			if (table[offset].Type & ENTRY::LOWER)
				strlwr(strLine);
			if (table[offset].Type & ENTRY::UPPER)
				strupr(strLine);
			if (table[offset].Type & ENTRY::NO_SPACES)
			{
				for (WORD i=0; strLine[i]; i++)
					if (isspace(strLine[i]))
						memmove(&strLine[i], &strLine[i+1], strlen(&strLine[i+1]) + 1);
			}
			strcpy((char *)table[offset].Value, strLine);
			break;
		}
	}

	return TRUE;
}

BOOL CFG::Entry2Line(const ENTRY *table, WORD offset, char *line)
{
	if (!table[offset].Id)
		return FALSE;
    switch (table[offset].Type & 0x00FF)
    {
    case ENTRY::INTEGER:
        if (table[offset].Type & ENTRY::UNSIGNED)
            sprintf(line, "%s=%u", table[offset].Id, *(UINT *)table[offset].Value);
        else
            sprintf(line, "%s=%d", table[offset].Id, *(WORD *)table[offset].Value);
        break;
	case ENTRY::LONG:
        sprintf(line, "%s=%ld", table[offset].Id, *(long *)table[offset].Value);
        break;
    case ENTRY::FLOAT:
        break;
    case ENTRY::DOUBLE:
		sprintf(line, "%s=%.2lf", table[offset].Id, *(double *)table[offset].Value);
        break;
    case ENTRY::STRING:
        sprintf(line, "%s=%s", table[offset].Id, (char *)table[offset].Value);
        break;
    case ENTRY::GROUP:
        strcpy(line, (char *)table[offset].Value);
        break;
    }

	return TRUE;
}

int CFG::SearchId(const ENTRY *table, const char *id)
{
    // search for the id in the table
    UINT offset = 0;
    while (table[offset].Id != NULL)
    {
        if (!strcmp(table[offset].Id, id))
            return offset;   // return the position in the table
        offset++;
    }
    return -1;
}

//
// Calendar and holydays
//

static CALENDAR_ENTRY defaultHollydays[] = {
    {
        1995,
        {
			{  9,  0,  0,  0 }, // January
            {  0,  0,  0,  0 },
            { 20,  0,  0,  0 },
            { 13, 14,  0,  0 },
            {  1, 29,  0,  0 },
            { 19, 26,  0,  0 },
            {  3, 20,  0,  0 },
            {  7, 21,  0,  0 },
            {  0,  0,  0,  0 },
            { 16,  0,  0,  0 },
            {  6, 13,  0,  0 },
            {  8, 25,  0,  0 } // December
        }
    },
{
    1996,
    {
        {  1,  8,  0,  0 }, // January
        {  0,  0,  0,  0 },
        { 25,  0,  0,  0 },
		{  4,  5,  0,  0 },
        {  1, 20,  0,  0 },
        { 10, 17,  0,  0 },
        {  1, 20,  0,  0 },
        {  7, 19,  0,  0 },
        {  0,  0,  0,  0 },
        { 14,  0,  0,  0 },
        {  4, 11,  0,  0 },
        {  9, 25,  0,  0 }  // December
    }
},
{
    1997,
	{
        {  0,  0,  0,  0 }, // January
        {  0,  0,  0,  0 },
        {  0,  0,  0,  0 },
        {  0,  0,  0,  0 },
        {  0,  0,  0,  0 },
        {  0,  0,  0,  0 },
		{  0,  0,  0,  0 },
        {  0,  0,  0,  0 },
        {  0,  0,  0,  0 },
        {  0,  0,  0,  0 },
        {  0,  0,  0,  0 },
        {  0,  0,  0,  0 }  // December
    }
}
};

void CFG::SetDefault(BOOL setAll)
{
    if (!setAll)
    {
        // save
        // [ Passwords ]
        PASSWORD *passwords = new PASSWORD[NOPWD2+1]; // v.220
        memcpy(passwords, Passwords, (NOPWD2+1)*sizeof(PASSWORD));
        // [ informacion de las cabinas ]
        BOOTH_ENTRY *boothInfo = new BOOTH_ENTRY[MAX_BOOTH];
		memcpy(boothInfo, BoothInfo, sizeof(BOOTH_ENTRY)*MAX_BOOTH);
        // [ Dias festivos ]
        CALENDAR_ENTRY *hollydays = new CALENDAR_ENTRY[MAX_HOLLY_YEARS];
        memcpy(hollydays, Hollydays, sizeof(CALENDAR_ENTRY)*MAX_HOLLY_YEARS);
        // clear
        memset(this, 0, sizeof(*this));
        // restore
        memcpy(Passwords, passwords, sizeof(PASSWORD)*(NOPWD2+1));
        memcpy(BoothInfo, boothInfo, sizeof(BOOTH_ENTRY)*MAX_BOOTH);
        memcpy(Hollydays, hollydays, sizeof(CALENDAR_ENTRY)*MAX_HOLLY_YEARS);
        delete [] passwords;
        delete [] boothInfo;
        delete [] hollydays;
    }
    else
    {
        memset(this, 0, sizeof(*this));
        // [ Passwords ]
		setDefaultPasswords();
        // [ BoothInfo ]
		char alias[4];
        for (WORD i=0; i < MAX_BOOTH; i++)
        {
            strcpy(BoothInfo[i].Name, itoa(i+1, alias, 10));
            BoothInfo[i].Attr = 0;
        }
        // [ Dias festivos ]
        memcpy(Hollydays, defaultHollydays, sizeof(CALENDAR_ENTRY)*MAX_HOLLY_YEARS);
    }
    // [ Sistema ]
    strcpy(SysGroup, ";\n; [ Sistema ]\n;");
    strcpy(COUNTRY, "Colombia");
    strcpy(CURRENCY, "$");
    strcpy(CITY    , "Medell�n");
    strcpy(COMPANY , "TELECOM");
    strcpy(ID      , ""); // clear ID.  JEAM/GCC
    strcpy(OPERATOR_NAME, "TELECOM"); // v2.16
    DEALER         = 0; // MicroDise�o Ltda.
	CLUSTERS       = 2;
	ACTIVE_CLUSTERS = CLUSTERS; // 2.30
	VIEW_REFRESH_TIME = 500;
	VIEW_PHONE = 1;

	strcpy(TAX_NAME, "IVA");
	TAX_PERCENT    = 16;
	DDN_TAX        = 16;
	DDI_TAX        = 16;
    strcpy(OP_TITLE, "Se�ora");
    strcpy(OP_NAME , "Adriana Giraldo");
    SS_ID          = 0;
    SS_TIME        = 0;
    // [ Aplicacion ]
    strcpy(AppGroup, ";\n; [ Aplicacion ]\n;");
    MANUAL        = FALSE;
    MANUAL_ANSWER = FALSE;
    strcpy(P_OPERATION, "automatica");
    strcpy(COM, "1 2400 none 8 1");
    LPT	= 1;
    DOUBLE_PRN = FALSE;
    strcpy(P_PORT, "lpt");
    FORM = LINEAL_80;
    strcpy(P_FORM, forms[FORM]);
    P_FOOTER1[0] = '\0';
    P_FOOTER2[0] = '\0';
	strcpy(CASH, "prn");
	M_ROUND = 0.05F;
    strcpy(USA, "Estados Unidos");
    //
	MIN_NAL       = 1;
	MIN_INTER     = 1;
	MIN_USA       = 1;
	MIN_BORDER    = 1;
	MIN_CELLULAR  = 1;
	CEIL_NAL      = 0.0;
	CEIL_INTER    = 1.0;
	CEIL_USA      = 1.0;
	CEIL_BORDER   = 0.0;
	CEIL_CELLULAR = 1.0;
    GENERATE_PREPAID_RECEIPT = 1;
    DOUBLE_PREPAID_RECEIPT   = 0;
	CACHE_SIZE = 1024;
	CHECK_DUPS = 1;
	MINIDB   = 0;
	MULTIPLE_PREPAID_CALLS	 = 1;
    CALL_ACTUAL_COST         = 1;
    NO_SOUND_WHILE_SPY       = 1;
    ACTIVATE_RELAY           = 1;
	RELAY_NUMBER             = 1;
	VIEW_DECIMALS			 = 0; // 0 digits -> 2.21.8
	CORRECTION_TIME          = 0; // miliseconds
	TURN_DAY				 = 1;
    TURN_NUMBER              = 1;
    strcpy(HEADER_LINE1, "Recibo de pago de servicios p�blicos");
    strcpy(HEADER_LINE2, "");
    strcpy(HEADER_LINE3, "");
    strcpy(HEADER_LINE4, "Somos agente retenedor del");
	HEADER_PRINT_TAXNAME = 1; // use line 4
	HEADER_PRINT_RECNO   = 0; // use line 4

	RECNO_LEADING_ZEROS = FALSE;
	strcpy(RECNO_LABEL, "Recibo");
	strcpy(SHORT_SERIAL, "AA52048");

	MCARD[0] = 5000.0;
	MCARD[1] = 10000.0;
	MCARD[2] = 15000.0;
	MCARD[3] = 20000.0;
    sprintf(MCARDS, _MCARD_PRINT_FMT, MCARD[0], MCARD[1], MCARD[2], MCARD[3]);
// 2.30	TLX_BASE        = 566.4; // + IVA
// 2.30	INTER_TLX_BASE  = 0.0;   // + IVA
	INTERNET_TAX    = 16.0; // 2.30
	INTERNET_ROUND	= 15.0; // minutos
	INTERNET_TARIFF = 800.0; // pesos
	CELLULAR_TAX	= 20.0; // 2.33
#if defined(__DEMO__)
	strcpy(ENGINE_KIND, "demo"); // 2.50 -- dev / demo / training
#else
	strcpy(ENGINE_KIND, "real"); // 2.50 -- production hardware
#endif
	N_RECEIPT    = 0L;
	N_DIAL_ERR   = 0;
	N_COM_ERR    = 0;
    MAX_COM_ERR  = 4;
    MAX_DIAL_ERR = 4;
    // [ Telefonia ]
    strcpy(PhoneGroup, ";\n; [ Telefonia ]\n;");
    T_ON_HOOK       = 150;
    T_INTER_RING    = 2000;
	T_OFF_HOOK      = 300;
    T_BREAK         = 30;
    T_MAKE          = 20;
    T_MAKE_MARGIN   = 80;
    T_INTERDIG      = 200;
    T_DTMF_FLAG     = 40;
    T_DTMF_INTERDIG = 40;
    T_ANSWER        = 38000U;
    NAL_DIGITS_MARGIN   = 1;
    LOCAL_DIGITS_MARGIN = 1;
    INTER_DIGITS_MARGIN = 4;
	INTER_DIGITS_NOT_INCLUDED = 10;
	NAL_DIGITS_NOT_INCLUDED = 9;
	LOCAL_DIGITS_NOT_INCLUDED = 6;
	T_TALK          = 3000;
    T_BIAS          = 150;
    T_DIAL          = 30000U;
	T_COM           = T_ANSWER+1000;
	T_STORE			= 550; // ms
    T_LOCK          = 800;
    strcpy(SIGNAL, "inversion");
    ASIGNAL = S_BIAS;
    EXIST_FALSE_ONE = FALSE;
    //
	// [ Marcacion ]
    strcpy(DialingGroup, ";\n; [ Marcacion ]\n;");
    ACCESS_LEVELS    = 3;
    ACCESS           = 0;
    OPERATOR_ACCESS  = 9;
    CELLULAR_ACCESS  = 3;
    CELLULAR_OPERATOR_ACCESS = 3;
    BORDER_ACCESS    = 9;
    INTER_ACCESS     = 0;
    EDA_ACCESS       = 8;
    SPECIAL_ACCESS   = 1;
    //
    INTER_DIGITS     = 14;
    BORDER_DIGITS    = 14;
    CELLULAR_DIGITS  = 10;
    NAL_DIGITS       = 10;
    LOCAL_DIGITS     = 7;
    SPECIAL_DIGITS   = 3;
    int i;
    LOCK_OTHER_OPERATORS = 1;    // v2.16
	//
    for (i=0; i < MAX_DEFAULT_TARIFFS; i++)
        DEF_TARIFFS[i] = 0;     // tariff 0
	// maybe in the future we'll use DDI !!!
    // v.2.20.7 uses tariff 9
	sprintf(DEFAULT_TARIFFS, _DEF_TAR_FMT,
		/* DEF_TARIFFS[0x00], */ DEF_TARIFFS[0x01], DEF_TARIFFS[0x02], DEF_TARIFFS[0x03], DEF_TARIFFS[0x04],
		DEF_TARIFFS[0x05], DEF_TARIFFS[0x06], DEF_TARIFFS[0x07], DEF_TARIFFS[0x08], DEF_TARIFFS[0x09]
		);
    //
    APPLY_DDN_SCHEDULE = FALSE;
    APPLY_DDI_SCHEDULE = TRUE;
    //
    // [ Extensions ]
    //
    strcpy(ExtGroup, ";\n; [ Extensiones ]\n;");
    E_APPLY_DDN_SCHEDULE = TRUE;
    E_APPLY_DDI_SCHEDULE = TRUE;
    E_DISCOUNT      = 0;
    E_INSTALL_COST  = 0;
    E_LINE_COST     = 0;
	E_MIN_AVAILABLE = 1000;
    E_SHOW_PHONE    = FALSE;
    E_FIRST_EXT     = 0;
    E_N_RECEIPT     = 0L;
    E_MIN_NAL       = 3;
    E_MIN_INTER     = 1;
    E_MIN_USA       = 1;
    E_MIN_BORDER    = 1;
    E_MIN_CELLULAR  = 1;
	E_CEIL_NAL      = 0.0;
	E_CEIL_INTER    = 1.0;
	E_CEIL_USA      = 0.0;
	E_CEIL_BORDER   = 0.0;
	E_CEIL_CELLULAR = 1.0;
    E_APPLY_ROUND   = TRUE;
    //
    // [ Critical ]
    //
    strcpy(CriticalGroup, ";\n; [ Valores Cr�ticos ]\n;");
    CHECK_PAUSE_KEY     = TRUE;
	IGNORE_EXTRA_DIGITS = FALSE;
    EXCLUSIVE_SPY       = TRUE;
    IGNORE_PRE_ANSWER   = FALSE;
    DETECT_INCOME       = FALSE;
    //
    // [ Modem ]
    //
    strcpy(ModemGroup, ";\n; [ Modem ]\n;");
    MODEM_COM     = 1; // COM2
    MODEM_BASE    = 0x2F8;
    MODEM_IRQ     = 3;
    MODEM_BAUDS   = 2400L;
    MODEM_DIAL    = 9; // GCPP_PULSE
    MODEM_SPEAKER = 1; // ON
    MODEM_DELAY = 1000;
    MODEM_RECEIVESENDDELAY = 110; // 110 ms
    MODEM_ACKTIME = 60; // 60 seconds
    MODEM_MAXTIME = 40000L;
    strcpy(MODEM_PHONE, "3415600");
    strcpy(DisplayGroup, ";\n; [ Display ]\n;");
	DISPLAY_ENABLE  = 0; // disable
    DISPLAY_COM     = 1; // COM2
    DISPLAY_BAUDS   = 9600;
    strcpy(DISPLAY_DEFAULT_MESSAGE, APP_VER_NAME);
}

void CFG::Adjust(void)
{
    AdjustForm();
    AdjustFooter();
    //
    if      (!strcmp(SIGNAL,"inversion")) ASIGNAL = S_BIAS;
    else if (!strcmp(SIGNAL,"tiempo"))    ASIGNAL = S_TIME;
    else if (!strcmp(SIGNAL,"hilo c"))    ASIGNAL = S_THREAD;
    else if (!strcmp(SIGNAL,"tone"))      ASIGNAL = S_TONE;
    //
    MANUAL = (!strcmp(P_OPERATION, "automatica"))?FALSE:TRUE;
    // maybe in the future we'll use DDI !!!
	// v.2.20.7 uses tariff 9
	sscanf(DEFAULT_TARIFFS, _DEF_TAR_FMT,
           /* &DEF_TARIFFS[0x00], */ &DEF_TARIFFS[0x01], &DEF_TARIFFS[0x02], &DEF_TARIFFS[0x03],&DEF_TARIFFS[0x04],
		   &DEF_TARIFFS[0x05], &DEF_TARIFFS[0x06], &DEF_TARIFFS[0x07], &DEF_TARIFFS[0x08], &DEF_TARIFFS[0x09]
          );
    //
    sscanf(MCARDS, _MCARD_SCAN_FMT, &MCARD[0], &MCARD[1], &MCARD[2], &MCARD[3]);
    sprintf(MCARDS, _MCARD_PRINT_FMT, MCARD[0], MCARD[1], MCARD[2], MCARD[3]);
    //
    // to avoid big min
    const MAX_MIN = 5;
    if (MIN_NAL > MAX_MIN)      MIN_NAL      = MAX_MIN;
    if (MIN_INTER > MAX_MIN)    MIN_INTER    = MAX_MIN;
    if (MIN_USA > MAX_MIN)      MIN_USA      = MAX_MIN;
    if (MIN_BORDER > MAX_MIN)   MIN_BORDER   = MAX_MIN;
    if (MIN_CELLULAR > MAX_MIN) MIN_CELLULAR = MAX_MIN;
    // to avoid bad dealer
    if (DEALER > 2) // new dealer in Ecuador !!!
        DEALER = 0;
    // just 2 or 3 levels
    if (ACCESS_LEVELS != 2 && ACCESS_LEVELS != 3)
        ACCESS_LEVELS = 3;
    // to avoid bad access phone code
	if (ACCESS != 0 && ACCESS != 9)
        ACCESS = 0;
    //
    // Extensions
    //
    if (E_MIN_NAL > MAX_MIN)     E_MIN_NAL     = MAX_MIN;
    if (E_MIN_INTER > MAX_MIN)   E_MIN_INTER   = MAX_MIN;
    if (E_MIN_USA > MAX_MIN)     E_MIN_USA     = MAX_MIN;
    if (E_MIN_BORDER > MAX_MIN)  E_MIN_BORDER  = MAX_MIN;
    if (E_MIN_CELLULAR > MAX_MIN) E_MIN_CELLULAR = MAX_MIN;
	if (E_FIRST_EXT > CLUSTERS*CLUSTER_SIZE)
		E_FIRST_EXT = CLUSTERS*CLUSTER_SIZE;

	if (CORRECTION_TIME > 10000) // 2.34
		CORRECTION_TIME = 0;

	if (T_STORE < 110 || T_STORE > 1000) // 2..15 booth * 55 ms
		T_STORE = 550;

	if (CACHE_SIZE > 4096)
		CACHE_SIZE = 1024;
}

// [ Calendar ]

BOOL CFG::IsHollyday(WORD year, WORD month, WORD day)
{
    for (WORD i=0; i<MAX_HOLLY_YEARS; i++)
        if (Hollydays[i].Year == year)
		{
            for (WORD j=0; j<MAX_HOLLYDAYS; j++)
                if (Hollydays[i].Table[month-1][j] == day)
                    return TRUE;
            break;
        }
    return FALSE; // still here !!!
}

BOOL CFG::IsHollyday(WORD packedDate)
{
	WORD year, month, day;
    _UnpackDate(packedDate, year, month, day);
	return IsHollyday(year, month, day);
}

#if !defined(__TEST__) && !defined(__UTIL__)
BOOL CFG::IsExtension(WORD cNum, WORD bNum)
#else
BOOL CFG::IsExtension(WORD , WORD )
#endif
{
#if !defined(__TEST__) && !defined(__UTIL__)
	extern SUPER_APP_INFO g_superAppInfo;
    static WORD boothCount;
    boothCount = cNum*CLUSTER_SIZE+bNum;
	if (g_superAppInfo.Attr.STPro && E_FIRST_EXT && boothCount >= (E_FIRST_EXT-1))
        return TRUE;
    else
        return FALSE;
#else
	return FALSE;
#endif
}

void CFG::nextTurn(void)
{
    WORD year, month, day;
    _GetSysDate(year, month, day);
    if (day == TURN_DAY)
	{
        if (TURN_NUMBER > 0 && TURN_NUMBER < 99)
            TURN_NUMBER++;
        else
            TURN_NUMBER = 1;
    }
    else
    {
        TURN_DAY    = day;
        TURN_NUMBER = 1;
    }
}

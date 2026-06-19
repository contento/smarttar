//
// [ PH_TAR.CPP ]
//

#include "stdst.h"

#include <ph_eng.h>

extern CFG *g_cfg;

#if !defined(__EDA__)
static const PH_ENGINE::TARIFF_ENTRY defaultDDNEntries[] =
{
	{   0.00, 0 },
	{  80.00, 1 },
	{ 190.00, 1 },
	{ 280.00, 1 },
	{ 400.00, 1 },
	{ 400.00, 0 },
	{   0.00, 0 },
	{   0.00, 0 },
	{ 	0.00, 0 },
	{ 	0.00, 0 },
	{ 	0.00, 0 },
	{ 	0.00, 0 },
	{ 	0.00, 0 },
	{ 	0.00, 0 },
	{ 	0.00, 0 },
	{ 	0.00, 0 }
};

static const PH_ENGINE::TARIFF_ENTRY defaultDDIEntries[] =
{
	{    0.00, 0 },
	{ 1002.90, 1 },
	{ 1128.25, 1 },
	{ 1253.60, 1 },
	{ 1379.00, 1 },
	{ 1504.35, 1 },
	{ 1629.70, 1 },
	{ 1755.05, 1 },
	{ 2005.80, 1 },
	{ 2256.50, 1 },
	{ 2544.85, 1 },
	{ 2757.95, 1 },
	{ 3008.65, 1 },
	{ 3259.40, 1 },
	{    0.00, 0 },
	{ 	 0.00, 0 },
	{ 	 0.00, 0 },
	{ 	 0.00, 0 },
	{ 	 0.00, 0 },
	{ 	 0.00, 0 }
};
#else // EDA
static const PH_ENGINE::TARIFF_ENTRY defaultDDNEntries[] =
{
	{    0.00, 0 },
	{   57.00, 1 },
	{  130.00, 1 },
	{  191.00, 1 },
	{  226.00, 1 },
	{  332.00, 1 },
	{   80.00, 1 },
	{  190.00, 1 },
	{  400.00, 1 },
	{ 1149.00, 1 },
	{ 1129.30, 1 },
	{ 1109.30, 1 },
	{ 2900.00, 1 },
	{    0.00, 0 },
	{    0.00, 0 },
	{ 	 0.00, 0 }
};

static const PH_ENGINE::TARIFF_ENTRY defaultDDIEntries[] =
{
	{    0.00, 0 },
	{ 1114.30, 1 },
	{ 1253.60, 1 },
	{ 1392.90, 1 },
	{ 1532.20, 1 },
	{ 1671.50, 1 },
	{ 1810.75, 1 },
	{ 1950.05, 1 },
	{ 2228.65, 1 },
	{ 2507.20, 1 },
	{ 2827.60, 1 },
	{ 3064.40, 1 },
	{ 3342.95, 1 },
	{ 3621.55, 1 },
	{    0.00, 0 },
	{ 	 0.00, 0 },
	{ 	 0.00, 0 },
	{ 	 0.00, 0 },
	{ 	 0.00, 0 },
	{ 	 0.00, 0 }
};
#endif

struct DEFAULT_SCHEDULE_ENTRY
{ // just to store default schedule
    struct
    {
        WORD Hour;
        WORD Minute;
    }
    From;
    struct
    {
        WORD Hour;
        WORD Minute;
    }
    To;
    WORD  Percent;
};

static const DEFAULT_SCHEDULE_ENTRY defaultDDNSchedule[][MAX_DDN_DAY_TYPE] =
    {
        {
            {
                {
                    0, 0
                }
, { 0, 0 }, 0
            }
, { { 0,  0}, { 0, 0}, 0 }, { { 0, 0}, { 0, 0 }, 0}
        },
{ { { 0, 0}, { 0, 0 }, 0}, { { 0,  0}, { 0, 0}, 0 }, { { 0, 0}, { 0,  0}, 0} },
{ { { 0, 0}, { 0, 0 }, 0}, { { 0,  0}, { 0, 0}, 0 }, { { 0, 0}, { 0,  0}, 0} },
{ { { 0, 0}, { 0, 0 }, 0}, { { 0,  0}, { 0, 0}, 0 }, { { 0, 0}, { 0,  0}, 0} },
{ { { 0, 0}, { 0, 0 }, 0}, { { 0,  0}, { 0, 0}, 0 }, { { 0, 0}, { 0,  0}, 0} }
    };

// --- DDI schedule made up by:
// 		Schedule[i][0]
//    Schedule[i][1] for USA
//    Schedule[i][2]
//    Schedule[i][3] for Other countries
//    Schedule[i][4] for borderline countries.
//    Schedule[i][5] for submarines. GCC/gcc. Jun 13 95.
//
// see MAX_DDI_SCHEDULE if changes are needed. GCC/gcc !!!
//
static const DEFAULT_SCHEDULE_ENTRY defaultDDISchedule[][MAX_DDI_DAY_TYPE] =
    {
        {
            {
                {
                    20, 0
                }
, { 23, 59}, 75
            }
, { { 0,  0}, { 23, 59},  75}, { { 0,  0}, { 23, 59}, 75}
        }
, // USA
{
    {
        {
            0,  0
        }
        , { 7,  59}, 75
    }
    , { { 0,  0}, { 0,   0},  0 }, { { 0,  0}, { 0, 0}, 0 }
},
{ { { 20, 0}, { 23, 59}, 75 }, { { 0,  0}, { 23, 59},  75}, { { 0,  0}, { 23, 59}, 75} }, // Other countries
{ { { 0,  0}, {  7, 59}, 75 }, { { 0,  0}, { 0,   0},  0 }, { { 0,  0}, { 0, 0}, 0 }   },
{ { { 0,  0}, {  0,  0},  0 }, { { 0,  0}, { 0,   0},  0 }, { { 0,  0}, { 0, 0}, 0 }   }, // Border line
{ { { 0,  0}, {  0,  0},  0 }, { { 0,  0}, { 0,   0},  0 }, { { 0,  0}, { 0, 0}, 0 }   }  // Submarines
    }
;

// ------------------------------ DDN ----------------------------------------

void PH_ENGINE::SetDefaultDDNTariffs(void)
{
    memcpy(DDNTariffs, defaultDDNEntries, MAX_DDN_TARIFF*sizeof(TARIFF_ENTRY));
    for (int i=0; i < MAX_DDN_TARIFF; i++)
		DDNTariffs[i].TaxPercent *= g_cfg->DDN_TAX; // adjust iva percent
}



void PH_ENGINE::SetDefaultDDITariffs(void)
{
    memcpy(DDITariffs, defaultDDIEntries, MAX_DDI_TARIFF*sizeof(TARIFF_ENTRY));
    for (int i=0; i < MAX_DDI_TARIFF; i++)
        DDITariffs[i].TaxPercent *= g_cfg->DDI_TAX; // adjust iva percent
}



void PH_ENGINE::SetDefaultDDNSchedule(void)
{
    for (int i=0; i<MAX_DDN_SCHEDULE; i++)
        for (WORD j=0; j<MAX_DDN_DAY_TYPE; j++)
        {
            _PackTime(DDNSchedule[i][j].From, defaultDDNSchedule[i][j].From.Hour, defaultDDNSchedule[i][j].From.Minute);
            _PackTime(DDNSchedule[i][j].To, defaultDDNSchedule[i][j].To.Hour, defaultDDNSchedule[i][j].To.Minute);
            DDNSchedule[i][j].Percent = defaultDDNSchedule[i][j].Percent;
        }
}

void PH_ENGINE::SetDefaultDDISchedule(void)
{
    for (int i=0; i<MAX_DDI_SCHEDULE; i++)
        for (WORD j=0; j<MAX_DDI_DAY_TYPE; j++)
        {
            _PackTime(DDISchedule[i][j].From, defaultDDISchedule[i][j].From.Hour, defaultDDISchedule[i][j].From.Minute);
            _PackTime(DDISchedule[i][j].To, defaultDDISchedule[i][j].To.Hour, defaultDDISchedule[i][j].To.Minute);
            DDISchedule[i][j].Percent = defaultDDISchedule[i][j].Percent;
        }
}

PH_ENGINE::TARIFF_ENTRY& PH_ENGINE::GetDDNTariff(WORD tariffNum)
{
    return DDNTariffs[tariffNum];
}

PH_ENGINE::SCHEDULE_ENTRY& PH_ENGINE::GetDDNSchedule(WORD dayType, WORD number)
{
    return DDNSchedule[number][dayType];
}

BOOL PH_ENGINE::IsDDNReduced(WORD dayType, WORD time, WORD& percent)
{
    percent = 100; // !!! look out this the default
    for (WORD i=0; i<MAX_DDN_SCHEDULE; i++)
        if (DDNSchedule[i][dayType].From <= time && time <= DDNSchedule[i][dayType].To)
        {
            percent = DDNSchedule[i][dayType].Percent;
            return TRUE;
        }
    return FALSE;
}

WORD PH_ENGINE::GetDDNPercent(WORD dayType, WORD time)
{
    WORD percent;
    IsDDNReduced(dayType, time, percent);
    return percent;
}

// ------------------------------ DDI ----------------------------------------

PH_ENGINE::TARIFF_ENTRY& PH_ENGINE::GetDDITariff(WORD tariffNum)
{
    return DDITariffs[tariffNum];
}

PH_ENGINE::SCHEDULE_ENTRY& PH_ENGINE::GetDDISchedule(WORD dayType, WORD number)
{
    return DDISchedule[number][dayType];
}

BOOL PH_ENGINE::IsDDIReduced(WORD schedule, WORD dayType, WORD time, WORD& percent)
{
    percent = 100; // !!! look out with this default
    WORD offset = 0, len = 0;
    // adjust offsets and lengths into table
    switch (schedule)
    {
    case DDI_REDUCED_USA      :
        offset = 0;
        len = 2;
        break;
    case DDI_REDUCED_OTHER    :
        offset = 2;
        len = 2;
        break;
    case DDI_REDUCED_BORDER   :
        offset = 4;
        len = 1;
        break;
    case DDI_REDUCED_SUBMARINE:
        offset = 5;
        len = 1;
        break;
    }
    for (WORD i=offset; i<(offset+len); i++)
        if (DDISchedule[i][dayType].From <= time && time <= DDISchedule[i][dayType].To)
        {
            percent = DDISchedule[i][dayType].Percent;
            return TRUE;
        }
    return FALSE;
}

WORD PH_ENGINE::GetDDIPercent(WORD schedule, WORD dayType, WORD time)
{
    WORD percent;
    IsDDIReduced(schedule, dayType, time, percent);
    return percent;
}

BOOL PH_ENGINE::LoadDDNTariffs(fstream& file)
{
    file.read((char *)&DDNTariffs, MAX_DDN_TARIFF*sizeof(TARIFF_ENTRY));
    return file.gcount() == MAX_DDN_TARIFF*sizeof(TARIFF_ENTRY);
}

BOOL PH_ENGINE::SaveDDNTariffs(fstream& file)
{
    file.write((char *)&DDNTariffs, MAX_DDN_TARIFF*sizeof(TARIFF_ENTRY));
    return TRUE;
}

BOOL PH_ENGINE::LoadDDITariffs(fstream& file)
{
    file.read((char *)&DDITariffs, MAX_DDI_TARIFF*sizeof(TARIFF_ENTRY));
    return file.gcount() == MAX_DDI_TARIFF*sizeof(TARIFF_ENTRY);
}

BOOL PH_ENGINE::SaveDDITariffs(fstream& file)
{
    file.write((char *)&DDITariffs, MAX_DDI_TARIFF*sizeof(TARIFF_ENTRY));
    return TRUE;
}

BOOL PH_ENGINE::LoadDDNSchedule(fstream& file)
{
    file.read((char *)&DDNSchedule, MAX_DDN_SCHEDULE*MAX_DDN_DAY_TYPE*sizeof(SCHEDULE_ENTRY));
    return file.gcount() == MAX_DDN_SCHEDULE*MAX_DDN_DAY_TYPE*sizeof(SCHEDULE_ENTRY);
}

BOOL PH_ENGINE::SaveDDNSchedule(fstream& file)
{
    file.write((char *)&DDNSchedule, MAX_DDN_SCHEDULE*MAX_DDN_DAY_TYPE*sizeof(SCHEDULE_ENTRY));
    return TRUE;
}

BOOL PH_ENGINE::LoadDDISchedule(fstream& file)
{
    file.read((char *)&DDISchedule, MAX_DDI_SCHEDULE*MAX_DDI_DAY_TYPE*sizeof(SCHEDULE_ENTRY));
    return file.gcount() == MAX_DDI_SCHEDULE*MAX_DDI_DAY_TYPE*sizeof(SCHEDULE_ENTRY);
}

BOOL PH_ENGINE::SaveDDISchedule(fstream& file)
{
    file.write((char *)&DDISchedule, MAX_DDI_SCHEDULE*MAX_DDI_DAY_TYPE*sizeof(SCHEDULE_ENTRY));
    return TRUE;
}

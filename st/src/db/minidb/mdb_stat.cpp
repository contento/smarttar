//
// [ MINIDB_STATISTICS.CPP ]
//
// MiniDBStatistics — IStatisticsStorage backed by a MiniDB .db file.
// Statistics live on a dedicated page (PAGE_STATS) inside the same .db.
// The page payload holds serialised DS_ENTRY[], DS_DOUBLEPRNENTRY[],
// and DS_CELLULARENTRY[] arrays.  We keep local copies and flush back
// to the page on demand.
//


// Period constants (match DB_STATISTICS::TYPETAG values — bare names
// so the copied DB_STATISTICS logic compiles without scoping changes).
#define STAT_YEAR  0
#define STAT_MONTH 1
#define STAT_WEEK  2
#define STAT_DAY   3
#define STAT_TURN  4
// Map bare YEAR/MONTH/WEEK/DAY/TURN references to the STAT_* constants
// used by the MiniDBStatistics implementation.
#define YEAR  STAT_YEAR
#define MONTH STAT_MONTH
#define WEEK  STAT_WEEK
#define DAY   STAT_DAY
#define TURN  STAT_TURN
#include <string.h>
#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>

#include <cfg.h>
#include <st_util.h>

#include <minidb/page.h>
#include <minidb/cache.h>
#include <minidb/btree.h>
#include <mdb_stat.h>
#include <mdb_stor.h>

// ---------------------------------------------------------------------------
// External globals
// ---------------------------------------------------------------------------

extern CFG *g_cfg;

// Maximum receipt number for range-checking (same as DB_STORAGE)
static const long MINIDB_MAX_RECEIPTS = 100000000L;
// ---------------------------------------------------------------------------
// Offset helpers within the StatsPage::DSData[] payload
// ---------------------------------------------------------------------------
// Layout:  DS_ENTRY[DS_MAXENTRIES]
//          DS_DOUBLEPRNENTRY[DS_MAXDOUBLEPRNENTRIES]
//          DS_CELLULARENTRY[DS_MAXCELLULARENTRIES]

static inline BYTE *StatsDSData(BYTE *pageData)
{
    return pageData;
}

static DS_ENTRY          *StatsDSArray(BYTE *pageData)
{
    return (DS_ENTRY *)pageData;
}

static DS_DOUBLEPRNENTRY *StatsDPArray(BYTE *pageData)
{
    return (DS_DOUBLEPRNENTRY *)
        (pageData + DS_MAXENTRIES * sizeof(DS_ENTRY));
}

static DS_CELLULARENTRY  *StatsCEArray(BYTE *pageData)
{
    return (DS_CELLULARENTRY *)
        (pageData + DS_MAXENTRIES * sizeof(DS_ENTRY)
         + DS_MAXDOUBLEPRNENTRIES * sizeof(DS_DOUBLEPRNENTRY));
}

// ---------------------------------------------------------------------------
// Constructor / Destructor
// ---------------------------------------------------------------------------

MiniDBStatistics::MiniDBStatistics(MiniDBCache &cache, long statsPage)
    : m_cache(cache)
    , m_statsPage(statsPage)
    , m_status(0)
    , m_readOnly(FALSE)
{
    // Read the stats page from cache and copy data into local arrays
    BYTE *page = m_cache.GetPage(statsPage);
    if (page)
    {
        PageHeader *hdr = (PageHeader *)page;
        if (PageHdrIsValid(*hdr) && hdr->PageType == PAGE_STATS)
        {
            BYTE *dsData = ((StatsPage *)page)->DSData;
            memcpy(m_entries,        StatsDSArray(dsData),
                   DS_MAXENTRIES * sizeof(DS_ENTRY));
            memcpy(m_doublePrn,      StatsDPArray(dsData),
                   DS_MAXDOUBLEPRNENTRIES * sizeof(DS_DOUBLEPRNENTRY));
            memcpy(m_cellular,       StatsCEArray(dsData),
                   DS_MAXCELLULARENTRIES * sizeof(DS_CELLULARENTRY));
            m_status = 0;  // OK
        }
        else
        {
            // Invalid page — initialise empty
            InitAll();
            m_status = 1;  // BAD_FILE
        }
        m_cache.Release(statsPage);
    }
    else
    {
        // Page not found — initialise empty
        InitAll();
        m_status = 2;  // NO_FILE
    }
}

MiniDBStatistics::~MiniDBStatistics()
{
    // Flush is called by owner before destruction; but be safe
    Flush();
}

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

static inline void InitEntryArray(DS_ENTRY *arr, UINT count)
{
    for (UINT i = 0; i < count; i++)
        arr[i].Init();
}

static inline void InitDoublePrnArray(DS_DOUBLEPRNENTRY *arr, UINT count)
{
    for (UINT i = 0; i < count; i++)
        arr[i].Init();
}

static inline void InitCellularArray(DS_CELLULARENTRY *arr, UINT count)
{
    for (UINT i = 0; i < count; i++)
        arr[i].Init();
}

void MiniDBStatistics::InitAll()
{
    InitEntryArray(m_entries, DS_MAXENTRIES);
    InitDoublePrnArray(m_doublePrn, DS_MAXDOUBLEPRNENTRIES);
    InitCellularArray(m_cellular, DS_MAXCELLULARENTRIES);
}

// ---------------------------------------------------------------------------
// Accessors
// ---------------------------------------------------------------------------

DS_ENTRY *MiniDBStatistics::operator[](WORD type)
{
    if (type >= DS_MAXENTRIES)
        return &m_entries[0];  // safe fallback
    return &m_entries[type];
}

DS_DOUBLEPRNENTRY *MiniDBStatistics::GetDoublePrnEntry(WORD ofs)
{
    if (ofs >= DS_MAXDOUBLEPRNENTRIES)
        return &m_doublePrn[0];
    return &m_doublePrn[ofs];
}

DS_CELLULARENTRY *MiniDBStatistics::GetCellularEntry(WORD type)
{
    if (type >= DS_MAXCELLULARENTRIES)
        return &m_cellular[0];
    return &m_cellular[type];
}

WORD MiniDBStatistics::GetStatus()
{
    return m_status;
}

BOOL MiniDBStatistics::IsReadOnly()
{
    return m_readOnly;
}

// ---------------------------------------------------------------------------
// SetErrors / GetTelEntries
// ---------------------------------------------------------------------------

void MiniDBStatistics::SetErrors(WORD dialErrors, WORD commErrors)
{
    m_entries[TURN].DialErrors = dialErrors;
    m_entries[TURN].ComErrors  = commErrors;
}

long MiniDBStatistics::GetTelEntries()
{
    long number = m_entries[TURN].Tel.Inter.Receipts;
#if defined(__EDA__)
    number +=
        m_entries[TURN].Tel.EDA2EDA.Receipts +
        m_entries[TURN].Tel.EDA2EPM.Receipts +
        m_entries[TURN].Tel.EDA2TEL.Receipts;
#else
    number += m_entries[TURN].Tel.Nal.Receipts;
#endif
    return number;
}

// ---------------------------------------------------------------------------
// Flush — copy local arrays back to the stats page
// ---------------------------------------------------------------------------

void MiniDBStatistics::Flush()
{
    BYTE *page = m_cache.GetPageW(m_statsPage);
    if (!page)
        return;

    BYTE *dsData = ((StatsPage *)page)->DSData;
    memcpy(StatsDSArray(dsData),       m_entries,
           DS_MAXENTRIES * sizeof(DS_ENTRY));
    memcpy(StatsDPArray(dsData),       m_doublePrn,
           DS_MAXDOUBLEPRNENTRIES * sizeof(DS_DOUBLEPRNENTRY));
    memcpy(StatsCEArray(dsData),       m_cellular,
           DS_MAXCELLULARENTRIES * sizeof(DS_CELLULARENTRY));

    m_cache.MarkDirty(m_statsPage);
    m_cache.Release(m_statsPage);
}

// ---------------------------------------------------------------------------
// Archive — no-op (stats live inside .db archived by receipt storage)
// ---------------------------------------------------------------------------

BOOL MiniDBStatistics::Archive()
{
    return TRUE;
}

// ---------------------------------------------------------------------------
// Repair — rebuild statistics from receipt storage
// ---------------------------------------------------------------------------

// Static context pointer for EnumReceipts callback (no user-data parameter)
static MiniDBStatistics *g_repairTarget = NULL;

BOOL MiniDBStatistics::EnumAddHelper(Receipt const &receipt)
{
    if (g_repairTarget)
    {
        // Add the receipt — cast away const since Add modifies tracking fields
        g_repairTarget->Add((Receipt &)receipt, FALSE);  // FALSE = not a new receipt (don't update range)
    return TRUE;
    }
    return TRUE;
}

BOOL MiniDBStatistics::Repair(IReceiptStorage *receiptStorage, BOOL all)
{
    if (all)
    {
        InitAll();
    }
    else
    {
        // Just current turn
        m_entries[TURN].Init();
        m_doublePrn[0].Init();
        m_doublePrn[1].Init();
        m_cellular[TURN].Init();
    }

    // Iterate all receipts via callback (no context slot, use static ptr)
    g_repairTarget = this;
    receiptStorage->EnumReceipts(EnumAddHelper);
    g_repairTarget = NULL;

    Flush();
    return TRUE;
}

// ---------------------------------------------------------------------------
// Add — accumulate a receipt into TURN statistics
// ---------------------------------------------------------------------------

BOOL MiniDBStatistics::Add(Receipt &receipt, BOOL isNew)
{
    double decTime = g_Milisec2Time(receipt.ElapsedTime, g_cfg->CORRECTION_TIME);
    if (isNew)
    {
        if (!m_entries[TURN].From.Number)
        {
            m_entries[TURN].From.Number = receipt.Number;
            m_entries[TURN].From.Date   = receipt.Date;
            m_entries[TURN].From.Time   = receipt.Time;

            m_entries[TURN].To.Number   = receipt.Number;
            m_entries[TURN].To.Date     = receipt.Date;
            m_entries[TURN].To.Time     = receipt.Time;
        }
        else
        {
            // lower bound
            if (m_entries[TURN].From.Number > receipt.Number &&
                m_entries[TURN].From.Number < MINIDB_MAX_RECEIPTS - 1000)
            {
                m_entries[TURN].From.Number = receipt.Number;
                m_entries[TURN].From.Date   = receipt.Date;
                m_entries[TURN].From.Time   = receipt.Time;
            }
            // upper bound
            if (m_entries[TURN].To.Number < receipt.Number ||
                (m_entries[TURN].To.Number > MINIDB_MAX_RECEIPTS - 32 &&
                 receipt.Number < 32))
            {
                m_entries[TURN].To.Number = receipt.Number;
                m_entries[TURN].To.Date   = receipt.Date;
                m_entries[TURN].To.Time   = receipt.Time;
            }
        }
    }

    // Dual printer: odd and even booth numbers
    m_doublePrn[receipt.BoothNumber % DS_MAXDOUBLEPRNENTRIES] += receipt;

    switch (receipt.Tag)
    {
    case Receipt::TEL:
        {
            if (receipt.Stat.Paid & PAID_CALL)
            {
                if (receipt.Stat.CallAttr & INTERNATIONAL_CALL_MASK)
                {
                    m_entries[TURN].Tel.Inter.Receipts++;
                    m_entries[TURN].Tel.Inter.TalkMin  += decTime;
                    m_entries[TURN].Tel.Inter.PaidMin  += receipt.CeilMin;
                    m_entries[TURN].Tel.Inter.Value    += receipt.Value;
                    m_entries[TURN].Tel.Inter.Tax      += receipt.Tax;
                }
                else if (receipt.Stat.CallAttr == CELLULAR_CALL)
                {
                    m_cellular[TURN].Tel.Receipts++;
                    m_cellular[TURN].Tel.TalkMin += decTime;
                    m_cellular[TURN].Tel.PaidMin += receipt.CeilMin;
                    m_cellular[TURN].Tel.Value   += receipt.Value;
                    m_cellular[TURN].Tel.Tax     += receipt.Tax;
                }
                else
                {
                    m_entries[TURN].Tel.Nal.Receipts++;
                    m_entries[TURN].Tel.Nal.TalkMin += decTime;
                    m_entries[TURN].Tel.Nal.PaidMin += receipt.CeilMin;
                    m_entries[TURN].Tel.Nal.Value   += receipt.Value;
                    m_entries[TURN].Tel.Nal.Tax     += receipt.Tax;
#if defined(__EDA__)
                    switch (receipt.Stat.CallAttr & ~NOT_INCLUDED_CALL_MASK)
                    {
                    case EDA2EDA_CALL:
                        m_entries[TURN].Tel.EDA2EDA.Receipts++;
                        m_entries[TURN].Tel.EDA2EDA.TalkMin += decTime;
                        m_entries[TURN].Tel.EDA2EDA.PaidMin += receipt.CeilMin;
                        m_entries[TURN].Tel.EDA2EDA.Value   += receipt.Value;
                        m_entries[TURN].Tel.EDA2EDA.Tax     += receipt.Tax;
                        break;
                    case DDN_EDA2EPM_CALL:
                    case LOCAL_EDA2EPM_CALL:
                        m_entries[TURN].Tel.EDA2EPM.Receipts++;
                        m_entries[TURN].Tel.EDA2EPM.TalkMin += decTime;
                        m_entries[TURN].Tel.EDA2EPM.PaidMin += receipt.CeilMin;
                        m_entries[TURN].Tel.EDA2EPM.Value   += receipt.Value;
                        m_entries[TURN].Tel.EDA2EPM.Tax     += receipt.Tax;
                        break;
                    case DDN_EDA2TEL_CALL:
                        m_entries[TURN].Tel.EDA2TEL.Receipts++;
                        m_entries[TURN].Tel.EDA2TEL.TalkMin += decTime;
                        m_entries[TURN].Tel.EDA2TEL.PaidMin += receipt.CeilMin;
                        m_entries[TURN].Tel.EDA2TEL.Value   += receipt.Value;
                        m_entries[TURN].Tel.EDA2TEL.Tax     += receipt.Tax;
                        break;
                    }
#endif
                }
            }
            else
            {
                if (receipt.Stat.Paid == NOT_PAID_CALL)
                {
                    m_entries[TURN].NotPaid.Receipts++;
                    m_entries[TURN].NotPaid.Value   += receipt.Value;
                }
                else
                {
                    m_entries[TURN].TollFree.Receipts++;
                    m_entries[TURN].TollFree.Value   += receipt.Value;
                }
            }
            break;
        }

    case Receipt::SPECIAL_TEL:
        {
            if (receipt.Stat.Paid & PAID_CALL)
            {
                if (receipt.Stat.CallAttr & INTERNATIONAL_CALL_MASK)
                {
                    m_entries[TURN].SpecialTel.Inter.Receipts++;
                    m_entries[TURN].SpecialTel.Inter.TalkMin += decTime;
                    m_entries[TURN].SpecialTel.Inter.PaidMin += receipt.CeilMin;
                    m_entries[TURN].SpecialTel.Inter.Value   += receipt.Value;
                    m_entries[TURN].SpecialTel.Inter.Tax     += receipt.Tax;
                }
                else if (receipt.Stat.CallAttr == CELLULAR_CALL)
                {
                    m_cellular[TURN].SpecialTel.Receipts++;
                    m_cellular[TURN].SpecialTel.TalkMin += decTime;
                    m_cellular[TURN].SpecialTel.PaidMin += receipt.CeilMin;
                    m_cellular[TURN].SpecialTel.Value   += receipt.Value;
                    m_cellular[TURN].SpecialTel.Tax     += receipt.Tax;
                }
                else
                {
                    m_entries[TURN].SpecialTel.Nal.Receipts++;
                    m_entries[TURN].SpecialTel.Nal.TalkMin += decTime;
                    m_entries[TURN].SpecialTel.Nal.PaidMin += receipt.CeilMin;
                    m_entries[TURN].SpecialTel.Nal.Value   += receipt.Value;
                    m_entries[TURN].SpecialTel.Nal.Tax     += receipt.Tax;
#if defined(__EDA__)
                    switch (receipt.Stat.CallAttr & ~NOT_INCLUDED_CALL_MASK)
                    {
                    case EDA2EDA_CALL:
                        m_entries[TURN].SpecialTel.EDA2EDA.Receipts++;
                        m_entries[TURN].SpecialTel.EDA2EDA.TalkMin += decTime;
                        m_entries[TURN].SpecialTel.EDA2EDA.PaidMin += receipt.CeilMin;
                        m_entries[TURN].SpecialTel.EDA2EDA.Value   += receipt.Value;
                        m_entries[TURN].SpecialTel.EDA2EDA.Tax     += receipt.Tax;
                        break;
                    case DDN_EDA2EPM_CALL:
                    case LOCAL_EDA2EPM_CALL:
                        m_entries[TURN].SpecialTel.EDA2EPM.Receipts++;
                        m_entries[TURN].SpecialTel.EDA2EPM.TalkMin += decTime;
                        m_entries[TURN].SpecialTel.EDA2EPM.PaidMin += receipt.CeilMin;
                        m_entries[TURN].SpecialTel.EDA2EPM.Value   += receipt.Value;
                        m_entries[TURN].SpecialTel.EDA2EPM.Tax     += receipt.Tax;
                        break;
                    case DDN_EDA2TEL_CALL:
                        m_entries[TURN].SpecialTel.EDA2TEL.Receipts++;
                        m_entries[TURN].SpecialTel.EDA2TEL.TalkMin += decTime;
                        m_entries[TURN].SpecialTel.EDA2TEL.PaidMin += receipt.CeilMin;
                        m_entries[TURN].SpecialTel.EDA2TEL.Value   += receipt.Value;
                        m_entries[TURN].SpecialTel.EDA2TEL.Tax     += receipt.Tax;
                        break;
                    }
#endif
                }
            }
            else
            {
                if (receipt.Stat.Paid == NOT_PAID_CALL)
                {
                    m_entries[TURN].NotPaid.Receipts++;
                    m_entries[TURN].NotPaid.Value   += receipt.Value;
                }
                else
                {
                    m_entries[TURN].TollFree.Receipts++;
                    m_entries[TURN].TollFree.Value   += receipt.Value;
                }
            }
            break;
        }

    case Receipt::FAX:
        {
            if (receipt.Stat.Paid & PAID_CALL)
            {
                if (receipt.Stat.CallAttr & INTERNATIONAL_CALL_MASK)
                {
                    m_entries[TURN].Fax.Inter.Receipts++;
                    m_entries[TURN].Fax.Inter.Amount += receipt.Amount;
                    m_entries[TURN].Fax.Inter.Value  += receipt.Value;
                    m_entries[TURN].Fax.Inter.Tax    += receipt.Tax;
                }
                else
                {
                    m_entries[TURN].Fax.Nal.Receipts++;
                    m_entries[TURN].Fax.Nal.Amount += receipt.Amount;
                    m_entries[TURN].Fax.Nal.Value  += receipt.Value;
                    m_entries[TURN].Fax.Nal.Tax    += receipt.Tax;
#if defined(__EDA__)
                    switch (receipt.Stat.CallAttr & ~NOT_INCLUDED_CALL_MASK)
                    {
                    case CELLULAR_CALL:
                        break;
                    case EDA2EDA_CALL:
                        m_entries[TURN].Fax.EDA2EDA.Receipts++;
                        m_entries[TURN].Fax.EDA2EDA.Amount += receipt.Amount;
                        m_entries[TURN].Fax.EDA2EDA.Value  += receipt.Value;
                        m_entries[TURN].Fax.EDA2EDA.Tax    += receipt.Tax;
                        break;
                    case DDN_EDA2EPM_CALL:
                    case LOCAL_EDA2EPM_CALL:
                        m_entries[TURN].Fax.EDA2EPM.Receipts++;
                        m_entries[TURN].Fax.EDA2EPM.Amount += receipt.Amount;
                        m_entries[TURN].Fax.EDA2EPM.Value  += receipt.Value;
                        m_entries[TURN].Fax.EDA2EPM.Tax    += receipt.Tax;
                        break;
                    case DDN_EDA2TEL_CALL:
                        m_entries[TURN].Fax.EDA2TEL.Receipts++;
                        m_entries[TURN].Fax.EDA2TEL.Amount += receipt.Amount;
                        m_entries[TURN].Fax.EDA2TEL.Value  += receipt.Value;
                        m_entries[TURN].Fax.EDA2TEL.Tax    += receipt.Tax;
                        break;
                    }
#endif
                }
            }
            else
            {
                if (receipt.Stat.Paid == NOT_PAID_CALL)
                {
                    m_entries[TURN].NotPaid.Receipts++;
                    m_entries[TURN].NotPaid.Value   += receipt.Value;
                }
                else
                {
                    m_entries[TURN].TollFree.Receipts++;
                    m_entries[TURN].TollFree.Value   += receipt.Value;
                }
            }
            break;
        }

    case Receipt::TELEX:
        {
            if (receipt.Stat.Paid & PAID_CALL)
            {
                if (receipt.Stat.CallAttr & INTERNATIONAL_CALL_MASK)
                {
                    // 2.30 nothing to do
                }
                else
                {
                    m_entries[TURN].Internet.Nal.Receipts++;
                    m_entries[TURN].Internet.Nal.Minutes += receipt.Minutes;
                    m_entries[TURN].Internet.Nal.PaidMin += receipt.CeilMin;
                    m_entries[TURN].Internet.Nal.Value   += receipt.Value;
                    m_entries[TURN].Internet.Nal.Tax     += receipt.Tax;
#if defined(__EDA__)
                    switch (receipt.Stat.CallAttr & ~NOT_INCLUDED_CALL_MASK)
                    {
                    case CELLULAR_CALL:
                        break;
                    case EDA2EDA_CALL:
                        m_entries[TURN].Internet.EDA2EDA.Receipts++;
                        m_entries[TURN].Internet.EDA2EDA.Minutes += receipt.Minutes;
                        m_entries[TURN].Internet.EDA2EDA.PaidMin += receipt.CeilMin;
                        m_entries[TURN].Internet.EDA2EDA.Value   += receipt.Value;
                        m_entries[TURN].Internet.EDA2EDA.Tax     += receipt.Tax;
                        break;
                    case DDN_EDA2EPM_CALL:
                    case LOCAL_EDA2EPM_CALL:
                        m_entries[TURN].Internet.EDA2EPM.Receipts++;
                        m_entries[TURN].Internet.EDA2EPM.Minutes += receipt.Minutes;
                        m_entries[TURN].Internet.EDA2EPM.PaidMin += receipt.CeilMin;
                        m_entries[TURN].Internet.EDA2EPM.Value   += receipt.Value;
                        m_entries[TURN].Internet.EDA2EPM.Tax     += receipt.Tax;
                        break;
                    case DDN_EDA2TEL_CALL:
                        m_entries[TURN].Internet.EDA2TEL.Receipts++;
                        m_entries[TURN].Internet.EDA2TEL.Minutes += receipt.Minutes;
                        m_entries[TURN].Internet.EDA2TEL.PaidMin += receipt.CeilMin;
                        m_entries[TURN].Internet.EDA2TEL.Value   += receipt.Value;
                        m_entries[TURN].Internet.EDA2TEL.Tax     += receipt.Tax;
                        break;
                    }
#endif
                }
            }
            else
            {
                if (receipt.Stat.Paid == NOT_PAID_CALL)
                {
                    m_entries[TURN].NotPaid.Receipts++;
                    m_entries[TURN].NotPaid.Value   += receipt.Value;
                }
                else
                {
                    m_entries[TURN].TollFree.Receipts++;
                    m_entries[TURN].TollFree.Value   += receipt.Value;
                }
            }
            break;
        }

    case Receipt::CARD:
        {
            if (receipt.Stat.Paid & PAID_CALL)
            {
                m_entries[TURN].Cards.Receipts++;
                for (int i = 0; i < MAX_MAGNETIC_CARDS; i++)
                    m_entries[TURN].Cards.Cards[i] += receipt.Cards[i];
                m_entries[TURN].Cards.Value += receipt.Value;
                m_entries[TURN].Cards.Tax   += receipt.Tax;
            }
            else
            {
                if (receipt.Stat.Paid == NOT_PAID_CALL)
                {
                    m_entries[TURN].NotPaid.Receipts++;
                    m_entries[TURN].NotPaid.Value   += receipt.Value;
                }
                else
                {
                    m_entries[TURN].TollFree.Receipts++;
                    m_entries[TURN].TollFree.Value   += receipt.Value;
                }
            }
            break;
        }

    case Receipt::OTHER:
        {
            if (receipt.Stat.Paid & PAID_CALL)
            {
                m_entries[TURN].Other.Receipts++;
                m_entries[TURN].Other.Value += receipt.Value;
                m_entries[TURN].Other.Tax   += receipt.Tax;
            }
            else
            {
                if (receipt.Stat.Paid == NOT_PAID_CALL)
                {
                    m_entries[TURN].NotPaid.Receipts++;
                    m_entries[TURN].NotPaid.Value   += receipt.Value;
                }
                else
                {
                    m_entries[TURN].TollFree.Receipts++;
                    m_entries[TURN].TollFree.Value   += receipt.Value;
                }
            }
            break;
        }
    }

    // --- adjust total values ---
    m_entries[TURN].Total.Tel =
        m_entries[TURN].Tel.Nal.Value +
        m_cellular[TURN].Tel.Value +
        m_entries[TURN].Tel.Inter.Value;

    m_entries[TURN].Total.Special =
        m_entries[TURN].SpecialTel.Nal.Value +
        m_entries[TURN].SpecialTel.Inter.Value +
        m_cellular[TURN].SpecialTel.Value +
        m_entries[TURN].Fax.Nal.Value +
        m_entries[TURN].Fax.Inter.Value +
        m_entries[TURN].Internet.Nal.Value +
        m_entries[TURN].Cards.Value +
        m_entries[TURN].Other.Value;

#if defined(__EDA__)
    m_entries[TURN].Total.EDA2EDA =
        m_entries[TURN].Tel.EDA2EDA.Value +
        m_entries[TURN].SpecialTel.EDA2EDA.Value +
        m_entries[TURN].Fax.EDA2EDA.Value +
        m_entries[TURN].Internet.EDA2EDA.Value;

    m_entries[TURN].Total.EDA2EPM =
        m_entries[TURN].Tel.EDA2EPM.Value +
        m_entries[TURN].SpecialTel.EDA2EPM.Value +
        m_entries[TURN].Fax.EDA2EPM.Value +
        m_entries[TURN].Internet.EDA2EPM.Value;

    m_entries[TURN].Total.EDA2TEL =
        m_entries[TURN].Tel.EDA2TEL.Value +
        m_entries[TURN].SpecialTel.EDA2TEL.Value +
        m_entries[TURN].Fax.EDA2TEL.Value +
        m_entries[TURN].Internet.EDA2TEL.Value +
        m_entries[TURN].Tel.Inter.Value +
        m_entries[TURN].SpecialTel.Inter.Value +
        m_entries[TURN].Fax.Inter.Value;
#endif

    m_entries[TURN].Total.NotPaid =
        m_entries[TURN].NotPaid.Value + m_entries[TURN].TollFree.Value;
    m_entries[TURN].Total.General =
        m_entries[TURN].Total.Tel + m_entries[TURN].Total.Special;

    // Tax
    m_entries[TURN].Tax.Tel =
        m_entries[TURN].Tel.Nal.Tax +
        m_entries[TURN].Tel.Inter.Tax +
        m_cellular[TURN].Tel.Tax;

    m_entries[TURN].Tax.Special =
        m_entries[TURN].SpecialTel.Nal.Tax +
        m_entries[TURN].SpecialTel.Inter.Tax +
        m_cellular[TURN].SpecialTel.Tax +
        m_entries[TURN].Fax.Nal.Tax +
        m_entries[TURN].Fax.Inter.Tax +
        m_entries[TURN].Internet.Nal.Tax +
        m_entries[TURN].Cards.Tax +
        m_entries[TURN].Other.Tax;

#if defined(__EDA__)
    m_entries[TURN].Tax.EDA2EDA =
        m_entries[TURN].Tel.EDA2EDA.Tax +
        m_entries[TURN].SpecialTel.EDA2EDA.Tax +
        m_entries[TURN].Fax.EDA2EDA.Tax +
        m_entries[TURN].Internet.EDA2EDA.Tax;

    m_entries[TURN].Tax.EDA2EPM =
        m_entries[TURN].Tel.EDA2EPM.Tax +
        m_entries[TURN].SpecialTel.EDA2EPM.Tax +
        m_entries[TURN].Fax.EDA2EPM.Tax +
        m_entries[TURN].Internet.EDA2EPM.Tax;

    m_entries[TURN].Tax.EDA2TEL =
        m_entries[TURN].Tel.EDA2TEL.Tax +
        m_entries[TURN].SpecialTel.EDA2TEL.Tax +
        m_entries[TURN].Fax.EDA2TEL.Tax +
        m_entries[TURN].Internet.EDA2TEL.Tax +
        m_entries[TURN].Tel.Inter.Tax +
        m_entries[TURN].SpecialTel.Inter.Tax +
        m_entries[TURN].Fax.Inter.Tax;
#endif

    m_entries[TURN].Tax.General =
        m_entries[TURN].Tax.Tel + m_entries[TURN].Tax.Special;

    return TRUE;
}

// ---------------------------------------------------------------------------
// Subtract — reverse of Add
// ---------------------------------------------------------------------------

BOOL MiniDBStatistics::Subtract(Receipt &receipt)
{
    double decTime = g_Milisec2Time(receipt.ElapsedTime, g_cfg->CORRECTION_TIME);

    switch (receipt.Tag)
    {
    case Receipt::TEL:
        {
            if (receipt.Stat.Paid & PAID_CALL)
            {
                if (receipt.Stat.CallAttr & INTERNATIONAL_CALL_MASK)
                {
                    m_entries[TURN].Tel.Inter.Receipts--;
                    m_entries[TURN].Tel.Inter.TalkMin  -= decTime;
                    m_entries[TURN].Tel.Inter.PaidMin  -= receipt.CeilMin;
                    m_entries[TURN].Tel.Inter.Value    -= receipt.Value;
                    m_entries[TURN].Tel.Inter.Tax      -= receipt.Tax;
                }
                else if (receipt.Stat.CallAttr == CELLULAR_CALL)
                {
                    m_cellular[TURN].Tel.Receipts--;
                    m_cellular[TURN].Tel.TalkMin -= decTime;
                    m_cellular[TURN].Tel.PaidMin -= receipt.CeilMin;
                    m_cellular[TURN].Tel.Value   -= receipt.Value;
                    m_cellular[TURN].Tel.Tax     -= receipt.Tax;
                }
                else
                {
                    m_entries[TURN].Tel.Nal.Receipts--;
                    m_entries[TURN].Tel.Nal.TalkMin -= decTime;
                    m_entries[TURN].Tel.Nal.PaidMin -= receipt.CeilMin;
                    m_entries[TURN].Tel.Nal.Value   -= receipt.Value;
                    m_entries[TURN].Tel.Nal.Tax     -= receipt.Tax;
#if defined(__EDA__)
                    switch (receipt.Stat.CallAttr & ~NOT_INCLUDED_CALL_MASK)
                    {
                    case EDA2EDA_CALL:
                        m_entries[TURN].Tel.EDA2EDA.Receipts--;
                        m_entries[TURN].Tel.EDA2EDA.TalkMin -= decTime;
                        m_entries[TURN].Tel.EDA2EDA.PaidMin -= receipt.CeilMin;
                        m_entries[TURN].Tel.EDA2EDA.Value   -= receipt.Value;
                        m_entries[TURN].Tel.EDA2EDA.Tax     -= receipt.Tax;
                        break;
                    case DDN_EDA2EPM_CALL:
                    case LOCAL_EDA2EPM_CALL:
                        m_entries[TURN].Tel.EDA2EPM.Receipts--;
                        m_entries[TURN].Tel.EDA2EPM.TalkMin -= decTime;
                        m_entries[TURN].Tel.EDA2EPM.PaidMin -= receipt.CeilMin;
                        m_entries[TURN].Tel.EDA2EPM.Value   -= receipt.Value;
                        m_entries[TURN].Tel.EDA2EPM.Tax     -= receipt.Tax;
                        break;
                    case DDN_EDA2TEL_CALL:
                        m_entries[TURN].Tel.EDA2TEL.Receipts--;
                        m_entries[TURN].Tel.EDA2TEL.TalkMin -= decTime;
                        m_entries[TURN].Tel.EDA2TEL.PaidMin -= receipt.CeilMin;
                        m_entries[TURN].Tel.EDA2TEL.Value   -= receipt.Value;
                        m_entries[TURN].Tel.EDA2TEL.Tax     -= receipt.Tax;
                        break;
                    }
#endif
                }
            }
            else
            {
                if (receipt.Stat.Paid == NOT_PAID_CALL)
                {
                    m_entries[TURN].NotPaid.Receipts--;
                    m_entries[TURN].NotPaid.Value   -= receipt.Value;
                }
                else
                {
                    m_entries[TURN].TollFree.Receipts--;
                    m_entries[TURN].TollFree.Value   -= receipt.Value;
                }
            }
            break;
        }

    case Receipt::SPECIAL_TEL:
        {
            if (receipt.Stat.Paid & PAID_CALL)
            {
                if (receipt.Stat.CallAttr & INTERNATIONAL_CALL_MASK)
                {
                    m_entries[TURN].SpecialTel.Inter.Receipts--;
                    m_entries[TURN].SpecialTel.Inter.TalkMin -= decTime;
                    m_entries[TURN].SpecialTel.Inter.PaidMin -= receipt.CeilMin;
                    m_entries[TURN].SpecialTel.Inter.Value   -= receipt.Value;
                    m_entries[TURN].SpecialTel.Inter.Tax     -= receipt.Tax;
                }
                else if (receipt.Stat.CallAttr == CELLULAR_CALL)
                {
                    m_cellular[TURN].SpecialTel.Receipts--;
                    m_cellular[TURN].SpecialTel.TalkMin -= decTime;
                    m_cellular[TURN].SpecialTel.PaidMin -= receipt.CeilMin;
                    m_cellular[TURN].SpecialTel.Value   -= receipt.Value;
                    m_cellular[TURN].SpecialTel.Tax     -= receipt.Tax;
                }
                else
                {
                    m_entries[TURN].SpecialTel.Nal.Receipts--;
                    m_entries[TURN].SpecialTel.Nal.TalkMin -= decTime;
                    m_entries[TURN].SpecialTel.Nal.PaidMin -= receipt.CeilMin;
                    m_entries[TURN].SpecialTel.Nal.Value   -= receipt.Value;
                    m_entries[TURN].SpecialTel.Nal.Tax     -= receipt.Tax;
#if defined(__EDA__)
                    switch (receipt.Stat.CallAttr & ~NOT_INCLUDED_CALL_MASK)
                    {
                    case EDA2EDA_CALL:
                        m_entries[TURN].SpecialTel.EDA2EDA.Receipts--;
                        m_entries[TURN].SpecialTel.EDA2EDA.TalkMin -= decTime;
                        m_entries[TURN].SpecialTel.EDA2EDA.PaidMin -= receipt.CeilMin;
                        m_entries[TURN].SpecialTel.EDA2EDA.Value   -= receipt.Value;
                        m_entries[TURN].SpecialTel.EDA2EDA.Tax     -= receipt.Tax;
                        break;
                    case DDN_EDA2EPM_CALL:
                    case LOCAL_EDA2EPM_CALL:
                        m_entries[TURN].SpecialTel.EDA2EPM.Receipts--;
                        m_entries[TURN].SpecialTel.EDA2EPM.TalkMin -= decTime;
                        m_entries[TURN].SpecialTel.EDA2EPM.PaidMin -= receipt.CeilMin;
                        m_entries[TURN].SpecialTel.EDA2EPM.Value   -= receipt.Value;
                        m_entries[TURN].SpecialTel.EDA2EPM.Tax     -= receipt.Tax;
                        break;
                    case DDN_EDA2TEL_CALL:
                        m_entries[TURN].SpecialTel.EDA2TEL.Receipts--;
                        m_entries[TURN].SpecialTel.EDA2TEL.TalkMin -= decTime;
                        m_entries[TURN].SpecialTel.EDA2TEL.PaidMin -= receipt.CeilMin;
                        m_entries[TURN].SpecialTel.EDA2TEL.Value   -= receipt.Value;
                        m_entries[TURN].SpecialTel.EDA2TEL.Tax     -= receipt.Tax;
                        break;
                    }
#endif
                }
            }
            else
            {
                if (receipt.Stat.Paid == NOT_PAID_CALL)
                {
                    m_entries[TURN].NotPaid.Receipts--;
                    m_entries[TURN].NotPaid.Value   -= receipt.Value;
                }
                else
                {
                    m_entries[TURN].TollFree.Receipts--;
                    m_entries[TURN].TollFree.Value   -= receipt.Value;
                }
            }
            break;
        }

    case Receipt::FAX:
        {
            if (receipt.Stat.Paid & PAID_CALL)
            {
                if (receipt.Stat.CallAttr & INTERNATIONAL_CALL_MASK)
                {
                    m_entries[TURN].Fax.Inter.Receipts--;
                    m_entries[TURN].Fax.Inter.Amount -= receipt.Amount;
                    m_entries[TURN].Fax.Inter.Value  -= receipt.Value;
                    m_entries[TURN].Fax.Inter.Tax    -= receipt.Tax;
                }
                else
                {
                    m_entries[TURN].Fax.Nal.Receipts--;
                    m_entries[TURN].Fax.Nal.Amount -= receipt.Amount;
                    m_entries[TURN].Fax.Nal.Value  -= receipt.Value;
                    m_entries[TURN].Fax.Nal.Tax    -= receipt.Tax;
#if defined(__EDA__)
                    switch (receipt.Stat.CallAttr & ~NOT_INCLUDED_CALL_MASK)
                    {
                    case CELLULAR_CALL:
                        break;
                    case EDA2EDA_CALL:
                        m_entries[TURN].Fax.EDA2EDA.Receipts--;
                        m_entries[TURN].Fax.EDA2EDA.Amount -= receipt.Amount;
                        m_entries[TURN].Fax.EDA2EDA.Value  -= receipt.Value;
                        m_entries[TURN].Fax.EDA2EDA.Tax    -= receipt.Tax;
                        break;
                    case DDN_EDA2EPM_CALL:
                    case LOCAL_EDA2EPM_CALL:
                        m_entries[TURN].Fax.EDA2EPM.Receipts--;
                        m_entries[TURN].Fax.EDA2EPM.Amount -= receipt.Amount;
                        m_entries[TURN].Fax.EDA2EPM.Value  -= receipt.Value;
                        m_entries[TURN].Fax.EDA2EPM.Tax    -= receipt.Tax;
                        break;
                    case DDN_EDA2TEL_CALL:
                        m_entries[TURN].Fax.EDA2TEL.Receipts--;
                        m_entries[TURN].Fax.EDA2TEL.Amount -= receipt.Amount;
                        m_entries[TURN].Fax.EDA2TEL.Value  -= receipt.Value;
                        m_entries[TURN].Fax.EDA2TEL.Tax    -= receipt.Tax;
                        break;
                    }
#endif
                }
            }
            else
            {
                if (receipt.Stat.Paid == NOT_PAID_CALL)
                {
                    m_entries[TURN].NotPaid.Receipts--;
                    m_entries[TURN].NotPaid.Value   -= receipt.Value;
                }
                else
                {
                    m_entries[TURN].TollFree.Receipts--;
                    m_entries[TURN].TollFree.Value   -= receipt.Value;
                }
            }
            break;
        }

    case Receipt::TELEX:
        {
            if (receipt.Stat.Paid & PAID_CALL)
            {
                if (receipt.Stat.CallAttr & INTERNATIONAL_CALL_MASK)
                {
                    // 2.30 nothing to do
                }
                else
                {
                    m_entries[TURN].Internet.Nal.Receipts--;
                    m_entries[TURN].Internet.Nal.Minutes -= receipt.Minutes;
                    m_entries[TURN].Internet.Nal.PaidMin -= receipt.CeilMin;
                    m_entries[TURN].Internet.Nal.Value   -= receipt.Value;
                    m_entries[TURN].Internet.Nal.Tax     -= receipt.Tax;
#if defined(__EDA__)
                    switch (receipt.Stat.CallAttr & ~NOT_INCLUDED_CALL_MASK)
                    {
                    case CELLULAR_CALL:
                        break;
                    case EDA2EDA_CALL:
                        m_entries[TURN].Internet.EDA2EDA.Receipts--;
                        m_entries[TURN].Internet.EDA2EDA.Minutes -= receipt.Minutes;
                        m_entries[TURN].Internet.EDA2EDA.PaidMin -= receipt.CeilMin;
                        m_entries[TURN].Internet.EDA2EDA.Value   -= receipt.Value;
                        m_entries[TURN].Internet.EDA2EDA.Tax     -= receipt.Tax;
                        break;
                    case DDN_EDA2EPM_CALL:
                    case LOCAL_EDA2EPM_CALL:
                        m_entries[TURN].Internet.EDA2EPM.Receipts--;
                        m_entries[TURN].Internet.EDA2EPM.Minutes -= receipt.Minutes;
                        m_entries[TURN].Internet.EDA2EPM.PaidMin -= receipt.CeilMin;
                        m_entries[TURN].Internet.EDA2EPM.Value   -= receipt.Value;
                        m_entries[TURN].Internet.EDA2EPM.Tax     -= receipt.Tax;
                        break;
                    case DDN_EDA2TEL_CALL:
                        m_entries[TURN].Internet.EDA2TEL.Receipts--;
                        m_entries[TURN].Internet.EDA2TEL.Minutes -= receipt.Minutes;
                        m_entries[TURN].Internet.EDA2TEL.PaidMin -= receipt.CeilMin;
                        m_entries[TURN].Internet.EDA2TEL.Value   -= receipt.Value;
                        m_entries[TURN].Internet.EDA2TEL.Tax     -= receipt.Tax;
                        break;
                    }
#endif
                }
            }
            else
            {
                if (receipt.Stat.Paid == NOT_PAID_CALL)
                {
                    m_entries[TURN].NotPaid.Receipts--;
                    m_entries[TURN].NotPaid.Value   -= receipt.Value;
                }
                else
                {
                    m_entries[TURN].TollFree.Receipts--;
                    m_entries[TURN].TollFree.Value   -= receipt.Value;
                }
            }
            break;
        }

    case Receipt::CARD:
        {
            if (receipt.Stat.Paid & PAID_CALL)
            {
                m_entries[TURN].Cards.Receipts--;
                for (int i = 0; i < MAX_MAGNETIC_CARDS; i++)
                    m_entries[TURN].Cards.Cards[i] -= receipt.Cards[i];
                m_entries[TURN].Cards.Value -= receipt.Value;
                m_entries[TURN].Cards.Tax   -= receipt.Tax;
            }
            else
            {
                if (receipt.Stat.Paid == NOT_PAID_CALL)
                {
                    m_entries[TURN].NotPaid.Receipts--;
                    m_entries[TURN].NotPaid.Value   -= receipt.Value;
                }
                else
                {
                    m_entries[TURN].TollFree.Receipts--;
                    m_entries[TURN].TollFree.Value   -= receipt.Value;
                }
            }
            break;
        }

    case Receipt::OTHER:
        {
            if (receipt.Stat.Paid & PAID_CALL)
            {
                m_entries[TURN].Other.Receipts--;
                m_entries[TURN].Other.Value -= receipt.Value;
                m_entries[TURN].Other.Tax   -= receipt.Tax;
            }
            else
            {
                if (receipt.Stat.Paid == NOT_PAID_CALL)
                {
                    m_entries[TURN].NotPaid.Receipts--;
                    m_entries[TURN].NotPaid.Value   -= receipt.Value;
                }
                else
                {
                    m_entries[TURN].TollFree.Receipts--;
                    m_entries[TURN].TollFree.Value   -= receipt.Value;
                }
            }
            break;
        }
    }

    // --- adjust total values ---
    m_entries[TURN].Total.Tel =
        m_entries[TURN].Tel.Nal.Value +
        m_entries[TURN].Tel.Inter.Value +
        m_cellular[TURN].Tel.Value;

    m_entries[TURN].Total.Special =
        m_entries[TURN].SpecialTel.Nal.Value +
        m_entries[TURN].SpecialTel.Inter.Value +
        m_cellular[TURN].SpecialTel.Value +
        m_entries[TURN].Fax.Nal.Value +
        m_entries[TURN].Fax.Inter.Value +
        m_entries[TURN].Internet.Nal.Value +
        m_entries[TURN].Cards.Value +
        m_entries[TURN].Other.Value;

#if defined(__EDA__)
    m_entries[TURN].Total.EDA2EDA =
        m_entries[TURN].Tel.EDA2EDA.Value +
        m_entries[TURN].SpecialTel.EDA2EDA.Value +
        m_entries[TURN].Fax.EDA2EDA.Value +
        m_entries[TURN].Internet.EDA2EDA.Value;

    m_entries[TURN].Total.EDA2EPM =
        m_entries[TURN].Tel.EDA2EPM.Value +
        m_entries[TURN].SpecialTel.EDA2EPM.Value +
        m_entries[TURN].Fax.EDA2EPM.Value +
        m_entries[TURN].Internet.EDA2EPM.Value;

    m_entries[TURN].Total.EDA2TEL =
        m_entries[TURN].Tel.EDA2TEL.Value +
        m_entries[TURN].SpecialTel.EDA2TEL.Value +
        m_entries[TURN].Fax.EDA2TEL.Value +
        m_entries[TURN].Internet.EDA2TEL.Value +
        m_entries[TURN].Tel.Inter.Value +
        m_entries[TURN].SpecialTel.Inter.Value +
        m_entries[TURN].Fax.Inter.Value;
#endif

    m_entries[TURN].Total.NotPaid =
        m_entries[TURN].NotPaid.Value + m_entries[TURN].TollFree.Value;
    m_entries[TURN].Total.General =
        m_entries[TURN].Total.Tel + m_entries[TURN].Total.Special;

    // Tax
    m_entries[TURN].Tax.Tel =
        m_entries[TURN].Tel.Nal.Tax +
        m_entries[TURN].Tel.Inter.Tax +
        m_cellular[TURN].Tel.Tax;

    m_entries[TURN].Tax.Special =
        m_entries[TURN].SpecialTel.Nal.Tax +
        m_entries[TURN].SpecialTel.Inter.Tax +
        m_cellular[TURN].SpecialTel.Tax +
        m_entries[TURN].Fax.Nal.Tax +
        m_entries[TURN].Fax.Inter.Tax +
        m_entries[TURN].Internet.Nal.Tax +
        m_entries[TURN].Cards.Tax +
        m_entries[TURN].Other.Tax;

#if defined(__EDA__)
    m_entries[TURN].Tax.EDA2EDA =
        m_entries[TURN].Tel.EDA2EDA.Tax +
        m_entries[TURN].SpecialTel.EDA2EDA.Tax +
        m_entries[TURN].Fax.EDA2EDA.Tax +
        m_entries[TURN].Internet.EDA2EDA.Tax;

    m_entries[TURN].Tax.EDA2EPM =
        m_entries[TURN].Tel.EDA2EPM.Tax +
        m_entries[TURN].SpecialTel.EDA2EPM.Tax +
        m_entries[TURN].Fax.EDA2EPM.Tax +
        m_entries[TURN].Internet.EDA2EPM.Tax;

    m_entries[TURN].Tax.EDA2TEL =
        m_entries[TURN].Tel.EDA2TEL.Tax +
        m_entries[TURN].SpecialTel.EDA2TEL.Tax +
        m_entries[TURN].Fax.EDA2TEL.Tax +
        m_entries[TURN].Internet.EDA2TEL.Tax +
        m_entries[TURN].Tel.Inter.Tax +
        m_entries[TURN].SpecialTel.Inter.Tax +
        m_entries[TURN].Fax.Inter.Tax;
#endif

    m_entries[TURN].Tax.General =
        m_entries[TURN].Tax.Tel + m_entries[TURN].Tax.Special;

    // Dual printer: odd and even booth numbers
    m_doublePrn[receipt.BoothNumber % DS_MAXDOUBLEPRNENTRIES] -= receipt;

    return TRUE;
}

// ---------------------------------------------------------------------------
// Update — period-reset logic (mirrors DB_STATISTICS::Update)
// ---------------------------------------------------------------------------

BOOL MiniDBStatistics::Update()
{
    WORD sysYear, sysMonth, sysDay, sysDayOfWeek;
    _UnpackDate(_GetSysDate(), sysYear, sysMonth, sysDay, sysDayOfWeek);

    // Check year rollover
    WORD year, month, day, dayOfWeek;
    _UnpackDate(m_entries[YEAR].To.Date, year, month, day, dayOfWeek);
    if (year < sysYear)
    {
        m_entries[YEAR].Init();
        m_entries[MONTH].Init();
        m_entries[WEEK].Init();
        m_entries[DAY].Init();
        m_cellular[YEAR].Init();
        m_cellular[MONTH].Init();
        m_cellular[WEEK].Init();
        m_cellular[DAY].Init();
    }

    // Check month rollover
    _UnpackDate(m_entries[MONTH].To.Date, year, month, day, dayOfWeek);
    if (month < sysMonth)
    {
        m_entries[MONTH].Init();
        m_entries[WEEK].Init();
        m_entries[DAY].Init();
        m_cellular[MONTH].Init();
        m_cellular[WEEK].Init();
        m_cellular[DAY].Init();
    }

    // Check week rollover (last close between thu-sun, current open between mon-wed)
    dayOfWeek = _GetDayOfWeek(m_entries[WEEK].To.Date);
    if (
        (dayOfWeek == 5 || dayOfWeek == 6 || dayOfWeek == 7 || dayOfWeek == 1) &&
        (sysDayOfWeek == 2 || sysDayOfWeek == 3 || sysDayOfWeek == 4)
       )
    {
        m_entries[WEEK].Init();
        m_entries[DAY].Init();
        m_cellular[WEEK].Init();
        m_cellular[DAY].Init();
    }

    // Check day rollover
    if (m_entries[DAY].To.Date < _GetSysDate())
    {
        m_entries[DAY].Init();
        m_cellular[DAY].Init();
    }

    // Accumulate TURN into aggregated periods
    m_entries[DAY]   += m_entries[TURN];
    m_entries[WEEK]  += m_entries[TURN];
    m_entries[MONTH] += m_entries[TURN];
    m_entries[YEAR]  += m_entries[TURN];

    // Reset TURN
    m_entries[TURN].Init();

    // Cellular accumulation
    m_cellular[DAY]   += m_cellular[TURN];
    m_cellular[WEEK]  += m_cellular[TURN];
    m_cellular[MONTH] += m_cellular[TURN];
    m_cellular[YEAR]  += m_cellular[TURN];

    m_cellular[TURN].Init();

    // Reset double-prn entries
    for (int i = 0; i < DS_MAXDOUBLEPRNENTRIES; i++)
        m_doublePrn[i].Init();

    Flush();
    return TRUE;
}

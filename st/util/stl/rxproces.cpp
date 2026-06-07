//
// [ RXPROCESS.CPP ]
//
// Converts SmartTar receipt binary database records into human-readable
// plain text files (.LST for display, .PRN for printing with escape codes).
//
// Two conversion paths exist:
//   processDAT / dat2PlainFile — exports individual receipts (telephony,
//     telex, fax, card, other) with per-receipt detail lines.
//   processSTA / sta2PlainFile — exports aggregated statistics entries
//     (turn, day, week, month, year totals) with summary rows.
//
// Both paths read from the DB_ENGINE (current turn) or archived DB (arc),
// and produce output in either LST (no separator) or PRN (space-separated,
// with printer headers) format.
//
// Receipt binary format: each Receipt struct contains a Tag (receipt type),
// Number, Date/Time, Stat (payment status flags), Value/Tax amounts, and
// type-specific fields (phone, city, elapsed time, cards[], etc.).
//
// Filtering in listBooth(): receipts can be filtered by type (automatic vs
// special), payment status (not paid / toll-free), and optionally limited
// to telephony receipts when sorting by booth.
//
// NOTE: dat2PlainFile and sta2PlainFile share the same file-naming and
// stream-opening pattern (build path → open .lst or .prn → write header →
// iterate records). This is near-duplicated code; a shared helper could
// reduce the ~40 lines of overlapping boilerplate.
#include "stdst.h"

#include <rxproces.h>

static const char *LST_EXT         = ".lst";
static const char *PRN_EXT         = ".prn";
static const char *DATAFILE_EXT    = ".dat";
static const char *INDEXFILE_EXT   = ".idx";
static const char *LSTSEPARATOR    = "";
static const char *PRNSEPARATOR    = " ";
static const int   NUMINDENTSPACES = 5;

#define NODECIMALS(value, digits) \
(floor((value)*(pow(10, digits))))

extern CFG *g_cfg;

// processDAT — Export individual receipt records to a plain-text file.
// Loads the DAT database (current turn or archive by date/turn), then
// calls dat2PlainFile twice: once for .LST (display) and once for .PRN (print).
BOOL RXProcessor::processDAT(void)
{
    BOOL ok = TRUE;
    if (cmdOptions.currentTurn)
    { // archive ?
        ok = dat2PlainFile(LSTTARGET);
        if (ok)
        {
            ok = dat2PlainFile(PRNTARGET);
        }
    }
    else
    {
        if (dbEngine->LoadArcDB(cmdOptions.date, cmdOptions.turn))
        {
            ok = dat2PlainFile(LSTTARGET);
            if (ok)
            {
                ok = dat2PlainFile(PRNTARGET);
            }
            dbEngine->UnloadArcDB();
        }
        else
            ok = FALSE;
    }
    return ok;
}

// processSTA — Export aggregated statistics records to a plain-text file.
// Same load pattern as processDAT, but calls sta2PlainFile which writes
// summary entries (turn/day/week/month/year) instead of individual receipts.
BOOL RXProcessor::processSTA(void)
{
    BOOL ok = TRUE;
    if (cmdOptions.currentTurn)
    { // archive ?
        ok = sta2PlainFile(LSTTARGET);
        if (ok)
        {
            ok = sta2PlainFile(PRNTARGET);
        }
    }
    else
    {
        if (dbEngine->LoadArcDB(cmdOptions.date, cmdOptions.turn))
        {
            ok = sta2PlainFile(LSTTARGET);
            if (ok)
            {
                ok = sta2PlainFile(PRNTARGET);
            }
            dbEngine->UnloadArcDB();
        }
        else
            ok = FALSE;
    }
    return ok;
}

// dat2PlainFile — Write individual receipt records to an .LST or .PRN file.
//
// Builds the output filename from cmdOptions.lstPath + rxBaseFilename with
// a "dat"/"d" suffix (current turn vs archive) plus .lst or .prn extension.
// If extTarget is PRNTARGET, a printer header is written and the file may
// be appended to (if cmdOptions.appendDatResult is set).
//
// When sortByBooth is set, iterates booths 0..MAX_BOOTH and calls listBooth
// for each, producing a per-booth grouping. Otherwise, a single pass with
// boothNumber=-1 lists all receipts regardless of booth.
BOOL RXProcessor::dat2PlainFile(WORD extTarget)
{
    BOOL ok = TRUE;
    DWORD lowerNumber, numOfReceipts;
    if (cmdOptions.currentTurn)
    {
        lowerNumber = dbEngine->GetLowerNumber();
        numOfReceipts = dbEngine->GetEntries();
    }
    else
    {
        lowerNumber = dbEngine->GetArcLowerNumber();
        numOfReceipts = dbEngine->GetArcEntries();
    }
    // prepare stream
    FILE_NAME lstFilename;
    strncpy(lstFilename, cmdOptions.lstPath, sizeof(lstFilename)-1);
    lstFilename[sizeof(lstFilename)-1] = '\0';
    strncat(lstFilename, "\\", sizeof(lstFilename)-strlen(lstFilename)-1);
    strncat(lstFilename, cmdOptions.rxBaseFilename, sizeof(lstFilename)-strlen(lstFilename)-1);
    if (cmdOptions.currentTurn)
        strncat(lstFilename, "dat", sizeof(lstFilename)-strlen(lstFilename)-1);
    else
        strncat(lstFilename, "d", sizeof(lstFilename)-strlen(lstFilename)-1);
    ofstream lstStream;
    if (extTarget == LSTTARGET)
    {
        strncat(lstFilename, LST_EXT, sizeof(lstFilename)-strlen(lstFilename)-1);
        lstStream.open(lstFilename);
    }
    else
    {
        strncat(lstFilename, PRN_EXT, sizeof(lstFilename)-strlen(lstFilename)-1);
        if (cmdOptions.appendDatResult)
            lstStream.open(lstFilename, ios::out|ios::app);
        else
            lstStream.open(lstFilename);
    }
    if (lstStream)
    {
        if (extTarget == PRNTARGET)
        {
            setDatPrnHeader(lstStream, PRNSEPARATOR);
        }
        if (cmdOptions.sortByBooth)
        { // by booth
            for (int boothNumber = 0; boothNumber < MAX_BOOTH; boothNumber++)
            {
                listBooth(lstStream, extTarget, lowerNumber, numOfReceipts, boothNumber);
            }
        }
        else
        {
            listBooth(lstStream, extTarget, lowerNumber, numOfReceipts, -1); // indicate no booth
        }

        lstStream << endl;
    }
    else
    {
        ok = FALSE;
    }
    return ok;
}

// sta2PlainFile — Write aggregated statistics to an .LST or .PRN file.
//
// Near-duplicate of dat2PlainFile: same file-naming pattern (lstPath +
// rxBaseFilename, with "sta"/"s" suffix), same LST/PRN extension logic.
// Instead of iterating individual receipts, calls listEntry five times
// for TURN, DAY, WEEK, MONTH, and YEAR statistic periods.
//
// TODO: dat2PlainFile and sta2PlainFile share ~40 lines of identical
// file-naming/stream-opening code that could be extracted to a helper.
BOOL RXProcessor::sta2PlainFile(WORD extTarget)
{
    BOOL ok = TRUE;
    // prepare stream
    FILE_NAME lstFilename;
    strncpy(lstFilename, cmdOptions.lstPath, sizeof(lstFilename)-1);
    lstFilename[sizeof(lstFilename)-1] = '\0';
    strncat(lstFilename, "\\", sizeof(lstFilename)-strlen(lstFilename)-1);
    strncat(lstFilename, cmdOptions.rxBaseFilename, sizeof(lstFilename)-strlen(lstFilename)-1);
    if (cmdOptions.currentTurn)
        strncat(lstFilename, "sta", sizeof(lstFilename)-strlen(lstFilename)-1);
    else
        strncat(lstFilename, "s", sizeof(lstFilename)-strlen(lstFilename)-1);
    if (extTarget == LSTTARGET)
		strncat(lstFilename, LST_EXT, sizeof(lstFilename)-strlen(lstFilename)-1);
    else
        strncat(lstFilename, PRN_EXT, sizeof(lstFilename)-strlen(lstFilename)-1);
    ofstream lstStream(lstFilename);
    if (lstStream)
    {
        char *separator = (char *)LSTSEPARATOR;
        if (extTarget == PRNTARGET)
        {
            separator = (char *) PRNSEPARATOR;
            setStaPrnHeader(lstStream, PRNSEPARATOR);
        }
        listEntry(lstStream, extTarget, DB_STATISTICS::TURN, separator);
		listEntry(lstStream, extTarget, DB_STATISTICS::DAY, separator);
		listEntry(lstStream, extTarget, DB_STATISTICS::WEEK, separator);
		listEntry(lstStream, extTarget, DB_STATISTICS::MONTH, separator);
		listEntry(lstStream, extTarget, DB_STATISTICS::YEAR, separator);
	}
	return ok;
}

// listBooth — Iterate receipts and write accepted ones to the output stream.
//
// Walks the database iterator from lowerNumber for numReceipts entries.
// For each receipt, applies filters in order:
//   1. Type filter: onlySpecialReceipts keeps only TEL receipts;
//      onlyAutomaticReceipts keeps only non-TEL receipts.
//   2. Booth filter: when sortByBooth, only TEL and SPECIAL_TEL pass.
//   3. Payment filter: onlyNotPaid keeps NOT_PAID_CALL;
//      onlyTollFree keeps TOLL_FREE_CALL; both together exclude PAID_CALL.
//
// Accepted receipts are formatted via receipt2Line() using the appropriate
// separator (empty for LST, space for PRN).
void RXProcessor::listBooth(ostream& lstStream, WORD extTarget, DWORD lowerNumber, DWORD numReceipts, WORD boothNumber)
{
	DB_STORAGE::Iterator *pit = NULL;

	if (cmdOptions.currentTurn)
	{
		pit = new DB_STORAGE::Iterator(dbEngine->GetDBStorage());
	}
	else
	{
		pit = new DB_STORAGE::Iterator(dbEngine->GetArcDBStorage());
	}

	if (!pit)
	{
		return ;
	}

	lowerNumber = pit->Restart(lowerNumber);

	int i = 0;
	long number;

	BOOL accepted;
	BOOL found;
	Receipt receipt;

	while (*pit && i < numReceipts)
	{
		number = pit->Current();

		if (cmdOptions.currentTurn)
			found = dbEngine->Get(receipt, number, boothNumber);
		else
			found = dbEngine->GetArc(receipt, number, boothNumber);

		if (found)
		{
			accepted = TRUE;
			// filter automatic and special receipts
			if (receipt.Tag == Receipt::TEL)
			{
				accepted = !cmdOptions.onlySpecialReceipts;
			}
			else
			{
				accepted = !cmdOptions.onlyAutomaticReceipts;
			}
			// filter based on booth order
			if (accepted)
			{
				if (cmdOptions.sortByBooth)
				{
					accepted = receipt.Tag == Receipt::TEL || receipt.Tag == Receipt::SPECIAL_TEL;
				}
			}
			// filter NC y PR
			if (accepted)
			{
				if (cmdOptions.onlyNotPaid && cmdOptions.onlyTollFree)
				{
					accepted = receipt.Stat.Paid != PAID_CALL;
				}
				else if (cmdOptions.onlyNotPaid)
				{
					accepted = receipt.Stat.Paid == NOT_PAID_CALL;
				}
				else if (cmdOptions.onlyTollFree)
				{
					accepted = receipt.Stat.Paid == TOLL_FREE_CALL;
				}
			}
			// go
			if (accepted)
			{
				if (extTarget == LSTTARGET)
					receipt2Line(receipt, lstStream, LSTSEPARATOR);
				else
					receipt2Line(receipt, lstStream, PRNSEPARATOR);
			}
		}

		(*pit)++;
		i++;
	}

	delete pit;
}

// receipt2Line — Format a single receipt as one output line.
// Dispatches to receipt2LineCommon + receipt2LineTotals + a type-specific
// formatter (Tel/Telex/Fax/Card/Other) based on receipt.Tag.
void RXProcessor::receipt2Line(Receipt const & receipt, ostream& lstStream, const char *separator)
{
    // specific
    switch (receipt.Tag)
    {
	case Receipt::TEL:
        {
            receipt2LineCommon(receipt, lstStream, separator);
            receipt2LineTotals(receipt, lstStream, separator);
            receipt2LineTel(receipt, lstStream, separator);
            break;
        }
	case Receipt::SPECIAL_TEL:
        {
            receipt2LineCommon(receipt, lstStream, separator);
            receipt2LineTotals(receipt, lstStream, separator);
            receipt2LineTel(receipt, lstStream, separator);
            break;
        }
	case Receipt::TELEX:
        {
            receipt2LineCommon(receipt, lstStream, separator);
            receipt2LineTotals(receipt, lstStream, separator);
            receipt2LineTelex(receipt, lstStream, separator);
            break;
        }
	case Receipt::FAX:
        {
            receipt2LineCommon(receipt, lstStream, separator);
            receipt2LineTotals(receipt, lstStream, separator);
            receipt2LineFax(receipt, lstStream, separator);
            break;
        }
	case Receipt::CARD:
        {
            receipt2LineCommon(receipt, lstStream, separator);
            receipt2LineTotals(receipt, lstStream, separator);
            receipt2LineCard(receipt, lstStream, separator);
            break;
        }
	case Receipt::OTHER:
        {
            receipt2LineCommon(receipt, lstStream, separator);
            receipt2LineTotals(receipt, lstStream, separator);
            receipt2LineOther(receipt, lstStream, separator);
            break;
        }
    }
    lstStream << endl;
}

// receipt2LineCommon — Write the shared prefix fields for every receipt line:
// serial number, receipt number, tag, payment status (hex), date, and time.
void RXProcessor::receipt2LineCommon(Receipt const & receipt, ostream& lstStream, const char *separator)
{
    WORD day, month, year;
    _UnpackDate(receipt.Date, year, month, day);
    WORD hh, mm, ss;
    _UnpackTime(receipt.Time, hh, mm, ss);
    // common
    lstStream
        << setiosflags(ios::right|ios::dec)
        << setfill('0')
    ;
    lstStream
        << setw(4) << cmdOptions.shortSerial
        << separator
        << setw(8) << (DWORD) receipt.Number
        << separator
        << setw(1) << receipt.Tag
    ;
    lstStream
        << setiosflags(ios::right)
        << setw(4) << setfill('0') << setiosflags(ios::hex|ios::uppercase) << (*(WORD *)&receipt.Stat)
        << separator
        << setw(2) << dec << day
        << setw(2) << month
        << setw(4) << year
        << separator
        << setw(2) << hh
        << setw(2) << mm
        << setw(2) << ss
        << separator
    ;
}

// receipt2LineTotals — Write the value/tax columns: nominal value (before tax),
// tax percentage (with 0.20 rounding margin), tax amount, and total value.
void RXProcessor::receipt2LineTotals(Receipt const & receipt, ostream& lstStream, const char *separator)
{
    double nominalValue = (receipt.Value - receipt.Tax);
    double taxPercent = 0;
    if (receipt.Value)
    {
        taxPercent = ((receipt.Tax/nominalValue)*100.0); // to trim last decimal
        taxPercent = floor(taxPercent+0.20); // margin !!!
    }

    lstStream
        << setiosflags(ios::right)
        << setfill('0')
        << setw(9) << NODECIMALS(nominalValue,2)
        << separator
        << setw(4) << NODECIMALS(taxPercent, 2)
        << separator
        << setw(8) << NODECIMALS(receipt.Tax, 2)
        << separator
        << setw(9) << NODECIMALS(receipt.Value, 2)
        << separator
    ;
}

// setDatPrnHeader — Write the PRN header for receipt (DAT) output.
// Emits ESC/P printer control codes (draft 10 CPI), a timestamp with
// the command-line arguments, and column headers (Ser, Numero, T/E,
// Fecha, Hora, Valor, %IVA, IVA, Total, Especifico de Cada Recibo).
void RXProcessor::setDatPrnHeader(ostream& lstStream, const char *separator)
{
    lstStream
        << endl << endl << endl << endl
        << "\x1BP\x1B\x0F" // ESCP ESCSI(15) = Draft 10 CPI - 137 Character
    ;
    STR16 strTime, strDate;
    lstStream
        << "[" << _GetSysDate(strDate) << " " << _GetSysTime(strTime) << "]: "
    ;
    for (int i = 0; i< _argc; i++)
        lstStream << _argv[i] << " ";

    lstStream << endl << endl;
    lstStream
        << setfill('-')
        << setiosflags(ios::left)
        << setw(4) << "Ser."
        << separator
        << setw(8) << "N�mero"
        << separator
        << setw(5) << "T/E"
        << separator
        << setw(8) << "Fecha"
        << separator
        << setw(6) << "Hora"
        << separator
        << setw(9) << "Valor"
        << separator
        << setw(5) << "%IVA"
        << separator
        << setw(8) << "IVA"
        << separator
        << setw(9) << "Total"
        << separator
        << "Especifico de Cada Recibo"
        << endl
        << endl
    ;
}

// setStaPrnHeader — Write the PRN header for statistics (STA) output.
// Column headers: Ser, T, Desde (date/time/range), Hasta, N.C. (not paid
// count), P.R. (toll-free count), Subtotal, Impuesto, Total, Err.M., Err.C.
void RXProcessor::setStaPrnHeader(ostream& lstStream, const char *separator)
{
    lstStream
        << endl << endl << endl << endl
        << "\x1BP\x1B\x0F" // ESCP ESCSI(15) = Draft 10 CPI - 137 Character
    ;
    STR16 strTime, strDate;
    lstStream
        << "[" << _GetSysDate(strDate) << " " << _GetSysTime(strTime) << "]: "
    ;
    for (int i = 0; i< _argc; i++)
        lstStream << _argv[i] << " ";
    lstStream << endl << endl;
    lstStream
        << setfill('-')
        << setiosflags(ios::left)
        << setw(4) << "Ser."
        << separator
        << setw(1) << "T"
        << separator
        << setw(8) << "Desde"
        << separator
        << setw(6) << ""
        << separator
        << setw(8) << ""
        << separator
        << setw(8) << "Hasta"
        << separator
        << setw(6) << ""
        << separator
        << setw(8) << ""
        << separator
    ;
    lstStream
        << setw(6) << "N.C."
        << separator
        << setw(8) << ""
        << separator
        << setw(6) << "P.R."
        << separator
        << setw(8) << ""
        << separator
    ;
    lstStream
        << setw(10) << "Subtotal"
        << separator
        << setw(9) << "Impuesto"
        << separator
        << setw(10) << "Total"
        << separator
    ;
    lstStream
        << setw(6) << "Err.M."
        << separator
        << setw(6) << "Err.C."
        << separator
    ;
    lstStream << endl << endl;
}

// receipt2LineTel — Format the type-specific fields for telephone receipts:
// booth number, phone number, city, elapsed time (hh:mm:ss), billed minutes,
// per-minute rate, and percentage discount.
void RXProcessor::receipt2LineTel(Receipt const & receipt, ostream& lstStream, const char *separator)
{
	WORD time = g_Milisec2Time(receipt.ElapsedTime, 0); // 2.21.8
	WORD hh, mm, ss;
    _UnpackTime(time, hh, mm, ss);
    lstStream
        << setfill('0')
        << setiosflags(ios::right|ios::dec)
        << setw( 2) << (WORD) receipt.BoothNumber + 1
        << setfill(' ')
        << separator
        << setw(16) << setiosflags(ios::left) << receipt.Phone
        << setw(20) << receipt.City
    ;
    lstStream
        << setiosflags(ios::right)
        << setfill('0')
        << setw(2) << hh
        << setw(2) << mm
        << setw(2) << ss
        << separator
        << setw(5) << NODECIMALS(receipt.CeilMin, 1)
        << separator
        << setw(7) << NODECIMALS(receipt.ValuePerMin, 2)
        << separator
        << setw(5) << NODECIMALS(receipt.Percent, 2)
    ;
}

// receipt2LineTelex — Format the type-specific fields for telex receipts:
// city, amount (number of units), and unitary value.
void RXProcessor::receipt2LineTelex(Receipt const & receipt, ostream& lstStream, const char* separator)
{
    lstStream
        << setfill(' ')
        << setw (2)  << "" // whites
        << separator
        << setw (16) << "" // whites
        << setiosflags(ios::left)
        << setw(20) << receipt.City
        << setw(6)  << "" // whites
        << separator
    ;
    lstStream
        << setiosflags(ios::right)
        << setfill('0')
		<< setw(5) << (DWORD)(receipt.Amount)
        << separator
        << setw(7) << NODECIMALS(receipt.UnitaryValue, 2)
    ;
}

// receipt2LineFax — Format the type-specific fields for fax receipts:
// phone number, city, amount (pages), and unitary value.
void RXProcessor::receipt2LineFax(Receipt const & receipt, ostream& lstStream, const char* separator)
{
    lstStream
        << setfill(' ')
        << setw (2) << "" // whites
        << separator
        << setw(16) << setiosflags(ios::left) << receipt.Phone
        << setw(20) << receipt.City
        << setw(6)  << "" // whites
        << separator
    ;
    lstStream
        << setiosflags(ios::right)
        << setfill('0')
		<< setw(5) << (DWORD)(receipt.Amount)
        << separator
        << setw(7) << NODECIMALS(receipt.UnitaryValue, 2)
    ;
}

// receipt2LineCard — Format the type-specific fields for magnetic card
// receipts: four card types with their configured IDs (from g_cfg->MCARD)
// and usage counts, plus the total card count.
void RXProcessor::receipt2LineCard(Receipt const & receipt, ostream& lstStream, const char* separator)
{
    int amount = 0;
    for (int i=0; i < MAX_MAGNETIC_CARDS; i++)
        amount += receipt.Cards[i];
    lstStream
        << setfill(' ')
        << setw (2) << "" // whites
        << separator
        << setiosflags(ios::right)
        << setfill('0')
        << setw(6) << (DWORD)g_cfg->MCARD[0]
        << separator
        << setw(2) << receipt.Cards[0]
        << separator
        << setw(6) << (DWORD)g_cfg->MCARD[1]
        << separator
        << setw(2) << receipt.Cards[1]
        << separator
        << setw(6) << (DWORD)g_cfg->MCARD[2]
        << separator
        << setw(2) << receipt.Cards[2]
        << separator
        << setw(6) << (DWORD)g_cfg->MCARD[3]
        << separator
        << setw(2) << receipt.Cards[3]
        << separator
        << setfill(' ')
        << setw(2) << ""
        << separator
        << setfill('0')
        << setw(5) << amount
    ;
}

// receipt2LineOther — Format the type-specific fields for "other" receipts:
// motif (description), amount, and unitary value.
void RXProcessor::receipt2LineOther(Receipt const & receipt, ostream& lstStream, const char* separator)
{
    lstStream
        << setfill(' ')
        << setw (2) << "" // whites
        << separator
        << setw(16) << "" // whites
        << setw(20) << setiosflags(ios::left) << receipt.Motif
        << setw(6)  << "" // whites
        << separator
        << setiosflags(ios::right)
        << setfill('0')
		<< setw(5) << (DWORD)(receipt.Amount)
        << separator
        << setw(7) << NODECIMALS(receipt.UnitaryValue, 2)
	;
}

// listEntry — Write one aggregated statistics row for a given period type
// (TURN, DAY, WEEK, MONTH, YEAR). Outputs:
//   - Header: serial, period type, from/to date+time+receipt# range
//   - Payment summary: not-paid count/value, toll-free count/value
//   - Totals: general subtotal, tax, total, dial/com errors
//   - Telephony breakdown: national/cellular/inter by receipts, talk min,
//     paid min, value
//   - Special breakdown: special tel (national/cellular/inter), fax,
//     internet, magnetic cards, other
//   - Grand totals: special subtotal, tax, total
void RXProcessor::listEntry(ostream& lstStream, WORD extTarget, WORD type, const char* separator)
{
	DS_ENTRY *entry;
	DS_CELLULARENTRY *cellularEntry;
	if (cmdOptions.currentTurn)
	{
		entry = (*dbEngine)[type];
		cellularEntry = dbEngine->GetCellularEntry(type);
	}
	else
	{
		entry = dbEngine->GetArcStatistics(type);
		cellularEntry = dbEngine->GetArcCellularEntry(type);
	}
	lstStream
		<< setiosflags(ios::right|ios::dec)
		<< setfill('0');
	lstStream
		<< setw(4) << cmdOptions.shortSerial
		<< separator
		<< type
		<< separator;

	WORD day, month, year, hh, mm, ss;
	_UnpackDate(entry->From.Date, year, month, day);
	_UnpackTime(entry->From.Time, hh, mm, ss);

	lstStream
		<< setw(2) << day
		<< setw(2) << month
		<< setw(4) << year
		<< separator
		<< setw(2) << hh
		<< setw(2) << mm
		<< setw(2) << ss
		<< separator
		<< setw(8) << (DWORD)entry->From.Number
		<< separator;

	_UnpackDate(entry->To.Date, year, month, day);
	_UnpackTime(entry->To.Time, hh, mm, ss);

	lstStream
		<< setw(2) << day
		<< setw(2) << month
		<< setw(4) << year
		<< separator
		<< setw(2) << hh
		<< setw(2) << mm
		<< setw(2) << ss
		<< separator;

	lstStream
		<< setw(8) << (DWORD)entry->To.Number
		<< separator;

	lstStream
		<< setw(6) << entry->NotPaid.Receipts
		<< separator
		<< setw(8) << NODECIMALS(entry->NotPaid.Value, 0)
		<< separator
		<< setw(6) << entry->TollFree.Receipts
		<< separator
		<< setw(8) << NODECIMALS(entry->TollFree.Value, 0)
		<< separator;

	lstStream
		<< setw(10) << NODECIMALS((entry->Total.General-entry->Tax.General), 0)
		<< separator
		<< setw(10) << NODECIMALS(entry->Tax.General, 0)
		<< separator
		<< setw(10) << NODECIMALS(entry->Total.General, 0)
		<< separator;

	lstStream
		<< setw(6) << entry->DialErrors
		<< separator
		<< setw(6) << entry->ComErrors;

	if (extTarget == PRNTARGET)
		newLine(lstStream, NUMINDENTSPACES);

	lstStream
		<< setw(7) << entry->Tel.Nal.Receipts
		<< separator
		<< setw(8) << NODECIMALS(entry->Tel.Nal.TalkMin, 1)
		<< separator
		<< setw(8) << NODECIMALS(entry->Tel.Nal.PaidMin, 1)
		<< separator
		<< setw(9) << NODECIMALS(entry->Tel.Nal.Value, 0)
		<< separator;

	lstStream
		<< setw(7) << cellularEntry->Tel.Receipts
		<< separator
		<< setw(8) << NODECIMALS(cellularEntry->Tel.TalkMin, 1)
		<< separator
		<< setw(8) << NODECIMALS(cellularEntry->Tel.PaidMin, 1)
		<< separator
		<< setw(8) << NODECIMALS(cellularEntry->Tel.Value, 0)
		<< separator;

	lstStream
		<< setw(7) << entry->Tel.Inter.Receipts
		<< separator
		<< setw(8) << NODECIMALS(entry->Tel.Inter.TalkMin, 1)
		<< separator
		<< setw(8) << NODECIMALS(entry->Tel.Inter.PaidMin, 1)
		<< separator
		<< setw(9) << NODECIMALS(entry->Tel.Inter.Value, 0)
		<< separator;

    if (extTarget == PRNTARGET)
		newLine(lstStream, NUMINDENTSPACES);

	lstStream
		<< setw(10) << NODECIMALS((entry->Total.Tel-entry->Tax.Tel), 0)
		<< separator
		<< setw(10) << NODECIMALS(entry->Tax.Tel, 0)
		<< separator
		<< setw(10) << NODECIMALS(entry->Total.Tel, 0)
		<< separator;

	if (extTarget == PRNTARGET)
		newLine(lstStream, NUMINDENTSPACES);

	lstStream
		<< setw(5) << entry->SpecialTel.Nal.Receipts
		<< separator
		<< setw(6) << NODECIMALS(entry->SpecialTel.Nal.Value, 0)
		<< separator;

	lstStream
		<< setw(5) << cellularEntry->SpecialTel.Receipts
		<< separator
		<< setw(6) << NODECIMALS(cellularEntry->SpecialTel.Value, 0)
		<< separator;

	lstStream
		<< setw(5) << entry->Fax.Nal.Receipts
		<< separator
		<< setw(5) << entry->Fax.Nal.Amount
		<< separator
		<< setw(6) << NODECIMALS(entry->Fax.Nal.Value, 0)
		<< separator;

	lstStream
		<< setw(5) << entry->Internet.Nal.Receipts
		<< separator
		<< setw(5) << entry->Internet.Nal.PaidMin
		<< separator
		<< setw(6) << NODECIMALS(entry->Internet.Nal.Value, 0)
		<< separator;

	if (extTarget == PRNTARGET)
		newLine(lstStream, NUMINDENTSPACES);

	lstStream
		<< setw(5) << entry->SpecialTel.Inter.Receipts
		<< separator
		<< setw(6) << NODECIMALS(entry->SpecialTel.Inter.Value, 0)
		<< separator;
	lstStream
		<< setw(5) << entry->Fax.Inter.Receipts
		<< separator
		<< setw(5) << entry->Fax.Inter.Amount
		<< separator
		<< setw(6) << NODECIMALS(entry->Fax.Inter.Value, 0)
		<< separator;

	/*
	lstStream
		<< setw(5) << entry->Internet.Inter.Receipts
		<< separator
		<< setw(5) << entry->Internet.Inter.Amount
		<< separator
		<< setw(6) << NODECIMALS(entry->Internet.Inter.Value, 0)
		<< separator;
	*/
	//
	WORD mCards = 0;
	for (int i=0; i<MAX_MAGNETIC_CARDS; i++)
		mCards += entry->Cards.Cards[i];
	lstStream
		<< setw(5) << mCards
		<< separator
		<< setw(8) << NODECIMALS(entry->Cards.Value, 0)
		<< separator;

	lstStream
		<< setw(5) << entry->Other.Receipts
		<< separator
		<< setw(5) << NODECIMALS(entry->Other.Value, 0)
		<< separator;

	if (extTarget == PRNTARGET)
		newLine(lstStream, NUMINDENTSPACES);

	lstStream
		<< setw(10) << NODECIMALS((entry->Total.Special-entry->Tax.Special), 0)
		<< separator
		<< setw(10) << NODECIMALS(entry->Tax.Special, 0)
		<< separator
		<< setw(10) << NODECIMALS(entry->Total.Special, 0)
		<< separator;
	//
	lstStream << endl;
}

// newLine — Insert a line break followed by indent spaces (for PRN wrapping).
void RXProcessor::newLine(ostream& lstStream, WORD indent)
{
	lstStream
		<< endl
		<< setfill(' ')
		<< setw(indent) << " "
		<< setfill('0');
}

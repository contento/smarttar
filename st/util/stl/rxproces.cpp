//
// [ RXPROCESS.CPP ]
//
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
    strcpy(lstFilename, cmdOptions.lstPath);
    strcat(lstFilename, "\\");
    strcat(lstFilename, cmdOptions.rxBaseFilename);
    if (cmdOptions.currentTurn)
        strcat(lstFilename, "dat");
    else
        strcat(lstFilename, "d");
    ofstream lstStream;
    if (extTarget == LSTTARGET)
    {
        strcat(lstFilename, LST_EXT);
        lstStream.open(lstFilename);
    }
    else
    {
        strcat(lstFilename, PRN_EXT);
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

BOOL RXProcessor::sta2PlainFile(WORD extTarget)
{
    BOOL ok = TRUE;
    // prepare stream
    FILE_NAME lstFilename;
    strcpy(lstFilename, cmdOptions.lstPath);
    strcat(lstFilename, "\\");
    strcat(lstFilename, cmdOptions.rxBaseFilename);
    if (cmdOptions.currentTurn)
        strcat(lstFilename, "sta");
    else
        strcat(lstFilename, "s");
    if (extTarget == LSTTARGET)
		strcat(lstFilename, LST_EXT);
    else
        strcat(lstFilename, PRN_EXT);
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
        << setw(8) << "N£mero"
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

void RXProcessor::newLine(ostream& lstStream, WORD indent)
{
	lstStream
		<< endl
		<< setfill(' ')
		<< setw(indent) << " "
		<< setfill('0');
}

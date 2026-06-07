//
// [ DSTATIST.CPP ]
//

// statistical management

#include "stdst.h"

#if defined(__TEST__)
#include <conio.h>
#endif

#include <dstatist.h>

extern CFG 	*g_cfg;

static const char *STATISTICS_EXT = ".STA";

// ----------------------------------------------------------------------------
// [ DB_STATISTICS ]
// ----------------------------------------------------------------------------

DB_STATISTICS::DB_STATISTICS(const char *path, const char *name, WORD readOnly)
	:
	ReadOnly(readOnly),
	Status(OK)
{
    Header = new HEADER;
    Entries = new DS_ENTRY[DS_MAXENTRIES];
    DoublePRNEntries = new DS_DOUBLEPRNENTRY[DS_MAXDOUBLEPRNENTRIES];
    CellularEntries  = new DS_CELLULARENTRY[DS_MAXCELLULARENTRIES];
    //
    if (path)
        strcat(strcpy(Filename, path), "\\");
    else
        _GetAppPath(Filename); // NULL path implies .EXE path
    strcat(Filename, name);
    strcat(Filename, STATISTICS_EXT);
    WORD numOfReadBytes = 0;
    _fmode = O_BINARY;
    File = -1; // to check for error
    if (access(Filename, 0) != 0)
        Status |= NO_FILE;
    else
    {
        if (access(Filename, 6) != 0 && !ReadOnly)
            chmod(Filename, S_IREAD|S_IWRITE); // enable read/write
        File = open(Filename, O_RDWR|O_BINARY, S_IREAD|S_IWRITE);
        // Check datafile Header
        HEADER header;
        numOfReadBytes = read(File, &header, sizeof(HEADER));
        if (numOfReadBytes < sizeof(HEADER) || !header.FileHeader.IsValid())
            Status |= BAD_FILE;
        else
            *Header = header; // it's good, copy it
        if (!(Status & BAD_FILE))
        {
            // read Entries
            numOfReadBytes = read(File, Entries, sizeof(DS_ENTRY)*DS_MAXENTRIES);
            if (numOfReadBytes < sizeof(DS_ENTRY)*DS_MAXENTRIES)
                Status |= BAD_FILE;
            //
            // be careful no activate BAD_FILE for compatibility, just initialize !!!
            //
            // read double prn entries
            numOfReadBytes = read(File, DoublePRNEntries, sizeof(DS_DOUBLEPRNENTRY)*DS_MAXDOUBLEPRNENTRIES);
            if (numOfReadBytes < sizeof(DS_DOUBLEPRNENTRY)*DS_MAXDOUBLEPRNENTRIES)
                for (int i=0; i<DS_MAXDOUBLEPRNENTRIES; i++)
                    DoublePRNEntries[i].Init();
            // read cellular entries (may 1996)
            numOfReadBytes = read(File, CellularEntries, sizeof(DS_CELLULARENTRY)*DS_MAXCELLULARENTRIES);
            if (numOfReadBytes < sizeof(DS_CELLULARENTRY)*DS_MAXCELLULARENTRIES)
                for (int i=0; i<DS_MAXCELLULARENTRIES; i++)
                    CellularEntries[i].Init();
        }
    }
    if (!ReadOnly && ((Status & NO_FILE) || (Status & BAD_FILE)))
    {
        for (int i=0; i < DS_MAXENTRIES; i++)
            Entries[i].Init();
        File = creat(Filename, S_IREAD|S_IWRITE);
        if (File == -1)
            Status |= BAD_FILE; // disk-full / permission: do not write to fd -1
        else
        {
            write(File, Header, sizeof(HEADER));
            write(File, Entries, sizeof(DS_ENTRY)*DS_MAXENTRIES);
            write(File, DoublePRNEntries, sizeof(DS_DOUBLEPRNENTRY)*DS_MAXDOUBLEPRNENTRIES);
            write(File, CellularEntries, sizeof(DS_CELLULARENTRY)*DS_MAXCELLULARENTRIES);
            Status |= NEW;
        }
    }
}

DB_STATISTICS::~DB_STATISTICS(void)
{
    if (!ReadOnly)
		Flush();

	if (!(Status & NO_FILE) && File != -1)
		close(File);

    delete Header;
    delete [] Entries;
    delete [] DoublePRNEntries;
    delete [] CellularEntries;
}

void DB_STATISTICS::SetErrors(WORD dialErrors, WORD commErrors)
{
    Entries[TURN].DialErrors = dialErrors;
    Entries[TURN].ComErrors  = commErrors;
}

void DB_STATISTICS::Flush(void)
{
    if (lseek(File, sizeof(HEADER), SEEK_SET) != -1)
    {
        write(File, Entries, sizeof(DS_ENTRY)*DS_MAXENTRIES);
        write(File, DoublePRNEntries, sizeof(DS_DOUBLEPRNENTRY)*DS_MAXDOUBLEPRNENTRIES);
        write(File, CellularEntries, sizeof(DS_CELLULARENTRY)*DS_MAXCELLULARENTRIES);
    }
    WORD dupFile;
    dupFile = dup(File);
    close(dupFile);
}

BOOL DB_STATISTICS::Repair(DB_STORAGE *dBStorage, BOOL all)
{
    dBStorage->Flush(); // to be sure

	if (all)
    {
        // entries
        for (int i=0; i < DS_MAXENTRIES; i++)
            Entries[i].Init();
        // double prn
        for (i=0; i<DS_MAXDOUBLEPRNENTRIES; i++)
            DoublePRNEntries[i].Init();
        // cellular entries
        for (i=0; i < DS_MAXCELLULARENTRIES; i++)
			CellularEntries[i].Init();
    }
    else
    {
        // just current turn
        // entries
        Entries[TURN].Init();
        // double prn
		for (int i=0; i<DS_MAXDOUBLEPRNENTRIES; i++)
            DoublePRNEntries[i].Init();
        // cellular
        CellularEntries[TURN].Init();
	}
	close(File);
	if (access(Filename, 6) != 0)
		chmod(Filename, S_IREAD|S_IWRITE); // enable read/write
	File = creat(Filename, S_IREAD|S_IWRITE);
	write(File, Header, sizeof(HEADER));

#if defined(__TEST__)
#if !defined(__UTIL__)
	cprintf("\n\r- Statistics file, Record: ");
	short x = wherex(), y = wherey();
#endif
#endif

	DB_STORAGE::Iterator it(*dBStorage);
	it.Restart();
	while (it)
	{
		Receipt receipt;

		long number = it.Current();

		if (dBStorage->Get(receipt, number))
		{
			Add(receipt);
#if defined(__TEST__)
#if !defined(__UTIL__)
			gotoxy(x, y);
			cprintf("%ld", receipt.Number);
#endif
#endif
		}

		it++;
	}

	write(File, Entries, sizeof(DS_ENTRY)*DS_MAXENTRIES);
	write(File, DoublePRNEntries, sizeof(DS_DOUBLEPRNENTRY)*DS_MAXDOUBLEPRNENTRIES);
	write(File, CellularEntries, sizeof(DS_CELLULARENTRY)*DS_MAXCELLULARENTRIES);
	return TRUE;
}

BOOL DB_STATISTICS::Add(Receipt& receipt, BOOL newReceipt)
{
	double decTime = g_Milisec2Time(receipt.ElapsedTime, g_cfg->CORRECTION_TIME);
    if (newReceipt)
    {
        if (!Entries[TURN].From.Number)
        {
			Entries[TURN].From.Number = receipt.Number;
            Entries[TURN].From.Date   = receipt.Date;
            Entries[TURN].From.Time   = receipt.Time;
            //
            Entries[TURN].To.Number = receipt.Number;
            Entries[TURN].To.Date   = receipt.Date;
            Entries[TURN].To.Time   = receipt.Time;
        }
        else
        {
            // lower bound
            if (Entries[TURN].From.Number > receipt.Number &&
                    Entries[TURN].From.Number < DB_STORAGE::MAX_RECEIPTS-1000
               )
            {
                Entries[TURN].From.Number = receipt.Number;
                Entries[TURN].From.Date   = receipt.Date;
                Entries[TURN].From.Time   = receipt.Time;
			}
            // upper bound
            if (Entries[TURN].To.Number < receipt.Number ||
                    (Entries[TURN].To.Number > DB_STORAGE::MAX_RECEIPTS-32 && receipt.Number < 32)
               )
            {
                Entries[TURN].To.Number = receipt.Number;
                Entries[TURN].To.Date   = receipt.Date;
                Entries[TURN].To.Time   = receipt.Time;
            }
        }
    }
    //
    // Dual printer: odd and even booth numbers
    //
    DoublePRNEntries[receipt.BoothNumber%DS_MAXDOUBLEPRNENTRIES] += receipt;
    //
    switch (receipt.Tag)
	{
	case Receipt::TEL:
        {
            if (receipt.Stat.Paid & PAID_CALL)
            {
                if (receipt.Stat.CallAttr & INTERNATIONAL_CALL_MASK)
                {
                    // INTER is the same inter EDA2TEL
                    Entries[TURN].Tel.Inter.Receipts++;
                    Entries[TURN].Tel.Inter.TalkMin  += decTime;
					Entries[TURN].Tel.Inter.PaidMin  += receipt.CeilMin;
                    Entries[TURN].Tel.Inter.Value    += receipt.Value;
                    Entries[TURN].Tel.Inter.Tax      += receipt.Tax;
                }
                else if (receipt.Stat.CallAttr == CELLULAR_CALL)
                {
                    // Totals
                    CellularEntries[TURN].Tel.Receipts++;
					CellularEntries[TURN].Tel.TalkMin += decTime;
					CellularEntries[TURN].Tel.PaidMin += receipt.CeilMin;
                    CellularEntries[TURN].Tel.Value   += receipt.Value;
                    CellularEntries[TURN].Tel.Tax     += receipt.Tax;
                }
                else
                { // DDN and EDA
                    Entries[TURN].Tel.Nal.Receipts++;
                    Entries[TURN].Tel.Nal.TalkMin += decTime;
					Entries[TURN].Tel.Nal.PaidMin += receipt.CeilMin;
                    Entries[TURN].Tel.Nal.Value   += receipt.Value;
                    Entries[TURN].Tel.Nal.Tax     += receipt.Tax;
#if defined(__EDA__)
                    // becareful we must disable the NOT_INC mask
                    switch (receipt.Stat.CallAttr & ~NOT_INCLUDED_CALL_MASK)
                    {
                    case EDA2EDA_CALL:
                        Entries[TURN].Tel.EDA2EDA.Receipts++;
						Entries[TURN].Tel.EDA2EDA.TalkMin += decTime;
						Entries[TURN].Tel.EDA2EDA.PaidMin += receipt.CeilMin;
                        Entries[TURN].Tel.EDA2EDA.Value   += receipt.Value;
                        Entries[TURN].Tel.EDA2EDA.Tax     += receipt.Tax;
                        break;
                    case DDN_EDA2EPM_CALL:
                    case LOCAL_EDA2EPM_CALL:
                        Entries[TURN].Tel.EDA2EPM.Receipts++;
                        Entries[TURN].Tel.EDA2EPM.TalkMin += decTime;
						Entries[TURN].Tel.EDA2EPM.PaidMin += receipt.CeilMin;
                        Entries[TURN].Tel.EDA2EPM.Value   += receipt.Value;
                        Entries[TURN].Tel.EDA2EPM.Tax     += receipt.Tax;
                        break;
                    case DDN_EDA2TEL_CALL:
                        Entries[TURN].Tel.EDA2TEL.Receipts++;
                        Entries[TURN].Tel.EDA2TEL.TalkMin += decTime;
						Entries[TURN].Tel.EDA2TEL.PaidMin += receipt.CeilMin;
                        Entries[TURN].Tel.EDA2TEL.Value   += receipt.Value;
						Entries[TURN].Tel.EDA2TEL.Tax     += receipt.Tax;
                        break;
                        // default not implemented, see at PH_INFO.CPP into getCallAttr() !!!
                    }


#endif
                }
            }
            else
            {
                if (receipt.Stat.Paid == NOT_PAID_CALL)
                {
                    Entries[TURN].NotPaid.Receipts++;
                    // v.211	Entries[TURN].NotPaid.TalkMin += decTime;
					// v.211	Entries[TURN].NotPaid.PaidMin += receipt.CeilMin;
                    Entries[TURN].NotPaid.Value   += receipt.Value;
                }
				else
                { // toll free
                    Entries[TURN].TollFree.Receipts++;
                    // v.211	Entries[TURN].TollFree.TalkMin += decTime;
                    // v.211	Entries[TURN].TollFree.PaidMin += receipt.CeilMin;
                    Entries[TURN].TollFree.Value   += receipt.Value;
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
                    // INTER is the same inter EDA2TEL_CALL
                    Entries[TURN].SpecialTel.Inter.Receipts++;
					Entries[TURN].SpecialTel.Inter.TalkMin += decTime;
                    Entries[TURN].SpecialTel.Inter.PaidMin += receipt.CeilMin;
                    Entries[TURN].SpecialTel.Inter.Value   += receipt.Value;
                    Entries[TURN].SpecialTel.Inter.Tax += receipt.Tax;
                }
                else if (receipt.Stat.CallAttr == CELLULAR_CALL)
                {
                    // Totals
                    CellularEntries[TURN].SpecialTel.Receipts++;
                    CellularEntries[TURN].SpecialTel.TalkMin += decTime;
                    CellularEntries[TURN].SpecialTel.PaidMin += receipt.CeilMin;
                    CellularEntries[TURN].SpecialTel.Value   += receipt.Value;
                    CellularEntries[TURN].SpecialTel.Tax     += receipt.Tax;
                }
                else
                {
                    Entries[TURN].SpecialTel.Nal.Receipts++;
                    Entries[TURN].SpecialTel.Nal.TalkMin += decTime;
					Entries[TURN].SpecialTel.Nal.PaidMin += receipt.CeilMin;
                    Entries[TURN].SpecialTel.Nal.Value   += receipt.Value;
                    Entries[TURN].SpecialTel.Nal.Tax     += receipt.Tax;
                    // cellular entries (in the beginning just for Total)
#if defined(__EDA__)
                    // becareful we must disable the NOT_INC
                    switch (receipt.Stat.CallAttr & ~NOT_INCLUDED_CALL_MASK)
                    {
                    case EDA2EDA_CALL:
                        Entries[TURN].SpecialTel.EDA2EDA.Receipts++;
                        Entries[TURN].SpecialTel.EDA2EDA.TalkMin += decTime;
                        Entries[TURN].SpecialTel.EDA2EDA.PaidMin += receipt.CeilMin;
                        Entries[TURN].SpecialTel.EDA2EDA.Value   += receipt.Value;
                        Entries[TURN].SpecialTel.EDA2EDA.Tax     += receipt.Tax;
                        break;
                    case DDN_EDA2EPM_CALL:
                    case LOCAL_EDA2EPM_CALL:
                        Entries[TURN].SpecialTel.EDA2EPM.Receipts++;
						Entries[TURN].SpecialTel.EDA2EPM.TalkMin += decTime;
                        Entries[TURN].SpecialTel.EDA2EPM.PaidMin += receipt.CeilMin;
                        Entries[TURN].SpecialTel.EDA2EPM.Value   += receipt.Value;
                        Entries[TURN].SpecialTel.EDA2EPM.Tax     += receipt.Tax;
                        break;
                    case DDN_EDA2TEL_CALL:
                        Entries[TURN].SpecialTel.EDA2TEL.Receipts++;
                        Entries[TURN].SpecialTel.EDA2TEL.TalkMin += decTime;
                        Entries[TURN].SpecialTel.EDA2TEL.PaidMin += receipt.CeilMin;
                        Entries[TURN].SpecialTel.EDA2TEL.Value   += receipt.Value;
                        Entries[TURN].SpecialTel.EDA2TEL.Tax     += receipt.Tax;
                        break;
                        // default not implemented, see at PHONE_INFO::GetCallAttr()
                    }


#endif
                }
			}
            else
            {
                // v.211
                if (receipt.Stat.Paid == NOT_PAID_CALL)
                {
                    Entries[TURN].NotPaid.Receipts++;
                    Entries[TURN].NotPaid.Value   += receipt.Value;
                }
                else
                { // toll free
                    Entries[TURN].TollFree.Receipts++;
                    Entries[TURN].TollFree.Value   += receipt.Value;
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
                    // INTER is the same inter EDA2TEL
                    Entries[TURN].Fax.Inter.Receipts++;
                    Entries[TURN].Fax.Inter.Amount += receipt.Amount;
                    Entries[TURN].Fax.Inter.Value  += receipt.Value;
                    Entries[TURN].Fax.Inter.Tax    += receipt.Tax;
                }
                else
                {
                    Entries[TURN].Fax.Nal.Receipts++;
                    Entries[TURN].Fax.Nal.Amount += receipt.Amount;
                    Entries[TURN].Fax.Nal.Value  += receipt.Value;
                    Entries[TURN].Fax.Nal.Tax    += receipt.Tax;
#if defined(__EDA__)
					// becareful we must disable the NOT_INCLUDED_CALL_MASK
                    switch (receipt.Stat.CallAttr & ~NOT_INCLUDED_CALL_MASK)
                    {
                    case CELLULAR_CALL :
                        break; // nothing to do
                    case EDA2EDA_CALL:
                        Entries[TURN].Fax.EDA2EDA.Receipts++;
                        Entries[TURN].Fax.EDA2EDA.Amount += receipt.Amount;
                        Entries[TURN].Fax.EDA2EDA.Value  += receipt.Value;
                        Entries[TURN].Fax.EDA2EDA.Tax    += receipt.Tax;
                        break;
                    case DDN_EDA2EPM_CALL:
                    case LOCAL_EDA2EPM_CALL:
                        Entries[TURN].Fax.EDA2EPM.Receipts++;
                        Entries[TURN].Fax.EDA2EPM.Amount += receipt.Amount;
                        Entries[TURN].Fax.EDA2EPM.Value  += receipt.Value;
                        Entries[TURN].Fax.EDA2EPM.Tax    += receipt.Tax;
                        break;
					case DDN_EDA2TEL_CALL:
                        Entries[TURN].Fax.EDA2TEL.Receipts++;
                        Entries[TURN].Fax.EDA2TEL.Amount += receipt.Amount;
                        Entries[TURN].Fax.EDA2TEL.Value  += receipt.Value;
                        Entries[TURN].Fax.EDA2TEL.Tax    += receipt.Tax;
                        break;
                        // default not implemented, see at PHONE_INFO::GetCallAttr()
                    }


#endif
                }
            }
            else
            {
                // v.211
                if (receipt.Stat.Paid == NOT_PAID_CALL)
                {
					Entries[TURN].NotPaid.Receipts++;
                    Entries[TURN].NotPaid.Value   += receipt.Value;
                }
                else
                { // toll free
                    // this doesn't make sense !!!
                    Entries[TURN].TollFree.Receipts++;
                    Entries[TURN].TollFree.Value   += receipt.Value;
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
					// 2.30 removed this
				}
                else
                {
					Entries[TURN].Internet.Nal.Receipts++;
					Entries[TURN].Internet.Nal.Minutes += receipt.Minutes;
					Entries[TURN].Internet.Nal.PaidMin += receipt.CeilMin;
					Entries[TURN].Internet.Nal.Value  += receipt.Value;
					Entries[TURN].Internet.Nal.Tax    += receipt.Tax;
#if defined(__EDA__)
                    // becareful we must disable the NOT_INC and the CELL mask
                    switch (receipt.Stat.CallAttr & ~NOT_INCLUDED_CALL_MASK)
                    {
                    case CELLULAR_CALL:
                        break; // nothing to do
                    case EDA2EDA_CALL:
					 Entries[TURN].Internet.EDA2EDA.Receipts++;
					 Entries[TURN].Internet.EDA2EDA.Minutes += receipt.Minutes;
					 Entries[TURN].Internet.EDA2EDA.PaidMin += receipt.CeilMin;
					 Entries[TURN].Internet.EDA2EDA.Value   += receipt.Value;
					 Entries[TURN].Internet.EDA2EDA.Tax     += receipt.Tax;
					 break;
				 case DDN_EDA2EPM_CALL:
				 case LOCAL_EDA2EPM_CALL:
					 Entries[TURN].Internet.EDA2EPM.Receipts++;
					 Entries[TURN].Internet.EDA2EPM.Minutes += receipt.Minutes;
					 Entries[TURN].Internet.EDA2EPM.PaidMin += receipt.CeilMin;
					 Entries[TURN].Internet.EDA2EPM.Value   += receipt.Value;
					 Entries[TURN].Internet.EDA2EPM.Tax     += receipt.Tax;
					 break;
				 case DDN_EDA2TEL_CALL:
					 Entries[TURN].Internet.EDA2TEL.Receipts++;
					 Entries[TURN].Internet.EDA2TEL.Minutes += receipt.Minutes;
					 Entries[TURN].Internet.EDA2TEL.PaidMin += receipt.CeilMin;
					 Entries[TURN].Internet.EDA2TEL.Value   += receipt.Value;
					 Entries[TURN].Internet.EDA2TEL.Tax     += receipt.Tax;
					 break;
					 // default not implemented, see at PHONE_INFO::GetCallAttr()
					}
#endif
				}
			}
			else
			{
				// v.211
				if (receipt.Stat.Paid == NOT_PAID_CALL)
				{
					Entries[TURN].NotPaid.Receipts++;
					Entries[TURN].NotPaid.Value   += receipt.Value;
				}
				else
				{ // toll free
					// this doesn't make sense !!!
					Entries[TURN].TollFree.Receipts++;
					Entries[TURN].TollFree.Value   += receipt.Value;
				}
			}
			break;
		}
	case Receipt::CARD:
        {
            // nal because international not implemented !!!
            if (receipt.Stat.Paid & PAID_CALL)
            {
                Entries[TURN].Cards.Receipts++;
                for (int i=0; i<MAX_MAGNETIC_CARDS; i++)
                    Entries[TURN].Cards.Cards[i] += receipt.Cards[i];
                Entries[TURN].Cards.Value += receipt.Value;
                Entries[TURN].Cards.Tax   += receipt.Tax;
            }
            else
			{
                // v.211
                if (receipt.Stat.Paid == NOT_PAID_CALL)
                {
                    Entries[TURN].NotPaid.Receipts++;
                    Entries[TURN].NotPaid.Value   += receipt.Value;
                }
                else
                { // toll free
                    // this doesn't make sense !!!
                    Entries[TURN].TollFree.Receipts++;
                    Entries[TURN].TollFree.Value   += receipt.Value;
                }
            }
            break;
        }
	case Receipt::OTHER:
        {
			// nal because international not implemented !!!
            if (receipt.Stat.Paid & PAID_CALL)
            {
                Entries[TURN].Other.Receipts++;
                Entries[TURN].Other.Value += receipt.Value;
                Entries[TURN].Other.Tax   += receipt.Tax;
            }
            else
            {
                // v.211
                if (receipt.Stat.Paid == NOT_PAID_CALL)
                {
                    Entries[TURN].NotPaid.Receipts++;
                    Entries[TURN].NotPaid.Value   += receipt.Value;
                }
                else
                { // toll free
                    // this doesn't make sense !!!
					Entries[TURN].TollFree.Receipts++;
                    Entries[TURN].TollFree.Value   += receipt.Value;
                }
            }
            break;
        }
    }
    //
    // --- adjust total values
    //
    // total
    //
    Entries[TURN].Total.Tel =
        Entries[TURN].Tel.Nal.Value+
        //
        CellularEntries[TURN].Tel.Value+
        //
        Entries[TURN].Tel.Inter.Value
		;
    Entries[TURN].Total.Special =
        Entries[TURN].SpecialTel.Nal.Value+
		Entries[TURN].SpecialTel.Inter.Value+
        //
        CellularEntries[TURN].SpecialTel.Value+
        //
        Entries[TURN].Fax.Nal.Value+
        Entries[TURN].Fax.Inter.Value+
		Entries[TURN].Internet.Nal.Value+
		Entries[TURN].Cards.Value+
        Entries[TURN].Other.Value
        ;
#if defined(__EDA__)
    Entries[TURN].Total.EDA2EDA =
        Entries[TURN].Tel.EDA2EDA.Value +
        Entries[TURN].SpecialTel.EDA2EDA.Value +
		Entries[TURN].Fax.EDA2EDA.Value +
		Entries[TURN].Internet.EDA2EDA.Value
        ;
	Entries[TURN].Total.EDA2EPM =
        Entries[TURN].Tel.EDA2EPM.Value +
        Entries[TURN].SpecialTel.EDA2EPM.Value +
        Entries[TURN].Fax.EDA2EPM.Value +
		Entries[TURN].Internet.EDA2EPM.Value
        ;
    Entries[TURN].Total.EDA2TEL =
        Entries[TURN].Tel.EDA2TEL.Value +
        Entries[TURN].SpecialTel.EDA2TEL.Value +
        Entries[TURN].Fax.EDA2TEL.Value +
		Entries[TURN].Internet.EDA2TEL.Value+
        // remember all inter services are EDA2TEL
        Entries[TURN].Tel.Inter.Value      +
        Entries[TURN].SpecialTel.Inter.Value +
        Entries[TURN].Fax.Inter.Value 
		;
#endif
	Entries[TURN].Total.NotPaid = Entries[TURN].NotPaid.Value+Entries[TURN].TollFree.Value;
    Entries[TURN].Total.General = Entries[TURN].Total.Tel+Entries[TURN].Total.Special;
    //
    // Tax
    //
    Entries[TURN].Tax.Tel =
        Entries[TURN].Tel.Nal.Tax+
        Entries[TURN].Tel.Inter.Tax+
        //
        CellularEntries[TURN].Tel.Tax
        ;
    Entries[TURN].Tax.Special =
        Entries[TURN].SpecialTel.Nal.Tax+
        Entries[TURN].SpecialTel.Inter.Tax+
        //
		CellularEntries[TURN].SpecialTel.Tax+
        //
        Entries[TURN].Fax.Nal.Tax+
		Entries[TURN].Fax.Inter.Tax+
		Entries[TURN].Internet.Nal.Tax+
		Entries[TURN].Cards.Tax+
        Entries[TURN].Other.Tax
        ;
#if defined(__EDA__)
    // Tax instead of value
    Entries[TURN].Tax.EDA2EDA =
        Entries[TURN].Tel.EDA2EDA.Tax +
        Entries[TURN].SpecialTel.EDA2EDA.Tax +
        Entries[TURN].Fax.EDA2EDA.Tax +
		Entries[TURN].Internet.EDA2EDA.Tax
        ;
    Entries[TURN].Tax.EDA2EPM =
		Entries[TURN].Tel.EDA2EPM.Tax +
        Entries[TURN].SpecialTel.EDA2EPM.Tax +
        Entries[TURN].Fax.EDA2EPM.Tax +
		Entries[TURN].Internet.EDA2EPM.Tax
        ;
    Entries[TURN].Tax.EDA2TEL =
        Entries[TURN].Tel.EDA2TEL.Tax +
        Entries[TURN].SpecialTel.EDA2TEL.Tax +
        Entries[TURN].Fax.EDA2TEL.Tax +
		Entries[TURN].Internet.EDA2TEL.Tax +
        // remember all inter services are EDA2TEL
        Entries[TURN].Tel.Inter.Tax +
        Entries[TURN].SpecialTel.Inter.Tax +
        Entries[TURN].Fax.Inter.Tax 
		;
#endif
    Entries[TURN].Tax.General = Entries[TURN].Tax.Tel+Entries[TURN].Tax.Special;

	return TRUE;
}

BOOL DB_STATISTICS::Subtract(Receipt& receipt)
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
                    // INTER is the same inter EDA2TEL
                    Entries[TURN].Tel.Inter.Receipts--;
                    Entries[TURN].Tel.Inter.TalkMin  -= decTime;
                    Entries[TURN].Tel.Inter.PaidMin  -= receipt.CeilMin;
                    Entries[TURN].Tel.Inter.Value    -= receipt.Value;
                    Entries[TURN].Tel.Inter.Tax      -= receipt.Tax;
				}
                else if (receipt.Stat.CallAttr == CELLULAR_CALL)
                {
                    // Totals
                    CellularEntries[TURN].Tel.Receipts--;
                    CellularEntries[TURN].Tel.TalkMin -= decTime;
                    CellularEntries[TURN].Tel.PaidMin -= receipt.CeilMin;
                    CellularEntries[TURN].Tel.Value   -= receipt.Value;
                    CellularEntries[TURN].Tel.Tax     -= receipt.Tax;
                }
                else
                {
                    Entries[TURN].Tel.Nal.Receipts--;
                    Entries[TURN].Tel.Nal.TalkMin -= decTime;
                    Entries[TURN].Tel.Nal.PaidMin -= receipt.CeilMin;
                    Entries[TURN].Tel.Nal.Value   -= receipt.Value;
                    Entries[TURN].Tel.Nal.Tax     -= receipt.Tax;
#if defined(__EDA__)
					// becareful we must disable the NOT_INC mask
                    switch (receipt.Stat.CallAttr & ~NOT_INCLUDED_CALL_MASK)
                    {
                    case EDA2EDA_CALL:
                        Entries[TURN].Tel.EDA2EDA.Receipts--;
                        Entries[TURN].Tel.EDA2EDA.TalkMin -= decTime;
                        Entries[TURN].Tel.EDA2EDA.PaidMin -= receipt.CeilMin;
                        Entries[TURN].Tel.EDA2EDA.Value   -= receipt.Value;
                        Entries[TURN].Tel.EDA2EDA.Tax     -= receipt.Tax;
                        break;
                    case DDN_EDA2EPM_CALL:
                    case LOCAL_EDA2EPM_CALL:
                        Entries[TURN].Tel.EDA2EPM.Receipts--;
                        Entries[TURN].Tel.EDA2EPM.TalkMin -= decTime;
                        Entries[TURN].Tel.EDA2EPM.PaidMin -= receipt.CeilMin;
                        Entries[TURN].Tel.EDA2EPM.Value   -= receipt.Value;
                        Entries[TURN].Tel.EDA2EPM.Tax     -= receipt.Tax;
                        break;
					case DDN_EDA2TEL_CALL:
                        Entries[TURN].Tel.EDA2TEL.Receipts--;
                        Entries[TURN].Tel.EDA2TEL.TalkMin -= decTime;
                        Entries[TURN].Tel.EDA2TEL.PaidMin -= receipt.CeilMin;
                        Entries[TURN].Tel.EDA2TEL.Value   -= receipt.Value;
                        Entries[TURN].Tel.EDA2TEL.Tax     -= receipt.Tax;
                        break;
                        // default not implemented, see at PHONE_INFO::GetCallAttr()
                    }


#endif
                }
            }
            else
            {
                if (receipt.Stat.Paid == NOT_PAID_CALL)
                {
					Entries[TURN].NotPaid.Receipts--;
                    // v.211	Entries[TURN].NotPaid.TalkMin -= decTime;
                    // v.211	Entries[TURN].NotPaid.PaidMin -= receipt.CeilMin;
                    Entries[TURN].NotPaid.Value   -= receipt.Value;
                }
                else
                { // toll free
                    Entries[TURN].TollFree.Receipts--;
                    // v.211	Entries[TURN].TollFree.TalkMin -= decTime;
                    // v.211	Entries[TURN].TollFree.PaidMin -= receipt.CeilMin;
                    Entries[TURN].TollFree.Value   -= receipt.Value;
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
                    // INTER is the same inter EDA2TEL_CALL
                    Entries[TURN].SpecialTel.Inter.Receipts--;
                    Entries[TURN].SpecialTel.Inter.TalkMin -= decTime;
                    Entries[TURN].SpecialTel.Inter.PaidMin -= receipt.CeilMin;
                    Entries[TURN].SpecialTel.Inter.Value   -= receipt.Value;
                    Entries[TURN].SpecialTel.Inter.Tax -= receipt.Tax;
                }
                else if (receipt.Stat.CallAttr == CELLULAR_CALL)
                {
                    // Totals
                    CellularEntries[TURN].SpecialTel.Receipts--;
                    CellularEntries[TURN].SpecialTel.TalkMin -= decTime;
                    CellularEntries[TURN].SpecialTel.PaidMin -= receipt.CeilMin;
                    CellularEntries[TURN].SpecialTel.Value   -= receipt.Value;
                    CellularEntries[TURN].SpecialTel.Tax     -= receipt.Tax;
				}
                else
                {
                    Entries[TURN].SpecialTel.Nal.Receipts--;
                    Entries[TURN].SpecialTel.Nal.TalkMin -= decTime;
                    Entries[TURN].SpecialTel.Nal.PaidMin -= receipt.CeilMin;
                    Entries[TURN].SpecialTel.Nal.Value   -= receipt.Value;
                    Entries[TURN].SpecialTel.Nal.Tax     -= receipt.Tax;
#if defined(__EDA__)
                    // becareful we must disable the NOT_INC mask
                    switch (receipt.Stat.CallAttr & ~NOT_INCLUDED_CALL_MASK)
                    {
                    case EDA2EDA_CALL:
                        Entries[TURN].SpecialTel.EDA2EDA.Receipts--;
                        Entries[TURN].SpecialTel.EDA2EDA.TalkMin -= decTime;
                        Entries[TURN].SpecialTel.EDA2EDA.PaidMin -= receipt.CeilMin;
                        Entries[TURN].SpecialTel.EDA2EDA.Value   -= receipt.Value;
                        Entries[TURN].SpecialTel.EDA2EDA.Tax     -= receipt.Tax;
						break;
                    case DDN_EDA2EPM_CALL:
                    case LOCAL_EDA2EPM_CALL:
                        Entries[TURN].SpecialTel.EDA2EPM.Receipts--;
                        Entries[TURN].SpecialTel.EDA2EPM.TalkMin -= decTime;
                        Entries[TURN].SpecialTel.EDA2EPM.PaidMin -= receipt.CeilMin;
                        Entries[TURN].SpecialTel.EDA2EPM.Value   -= receipt.Value;
                        Entries[TURN].SpecialTel.EDA2EPM.Tax     -= receipt.Tax;
                        break;
                    case DDN_EDA2TEL_CALL:
                        Entries[TURN].SpecialTel.EDA2TEL.Receipts--;
                        Entries[TURN].SpecialTel.EDA2TEL.TalkMin -= decTime;
                        Entries[TURN].SpecialTel.EDA2TEL.PaidMin -= receipt.CeilMin;
                        Entries[TURN].SpecialTel.EDA2TEL.Value   -= receipt.Value;
                        Entries[TURN].SpecialTel.EDA2TEL.Tax     -= receipt.Tax;
                        break;
                        // default not implemented, see at PHONE_INFO::GetCallAttr()
                    }


#endif
                }
            }
            else
            {
                // v.211
                if (receipt.Stat.Paid == NOT_PAID_CALL)
                {
                    Entries[TURN].NotPaid.Receipts--;
                    Entries[TURN].NotPaid.Value   -= receipt.Value;
                }
                else
                { // toll free
                    Entries[TURN].TollFree.Receipts--;
                    Entries[TURN].TollFree.Value   -= receipt.Value;
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
                    // INTER is the same inter EDA2TEL
                    Entries[TURN].Fax.Inter.Receipts--;
                    Entries[TURN].Fax.Inter.Amount -= receipt.Amount;
                    Entries[TURN].Fax.Inter.Value  -= receipt.Value;
                    Entries[TURN].Fax.Inter.Tax    -= receipt.Tax;
                }
                else
                {
                    Entries[TURN].Fax.Nal.Receipts--;
					Entries[TURN].Fax.Nal.Amount -= receipt.Amount;
                    Entries[TURN].Fax.Nal.Value  -= receipt.Value;
                    Entries[TURN].Fax.Nal.Tax    -= receipt.Tax;
#if defined(__EDA__)
                    // becareful we must disable the NOT_INC and the CELL mask
                    switch (receipt.Stat.CallAttr & ~NOT_INCLUDED_CALL_MASK)
                    {
                    case CELLULAR_CALL :
                        break; // nothing to do
                    case EDA2EDA_CALL:
                        Entries[TURN].Fax.EDA2EDA.Receipts--;
                        Entries[TURN].Fax.EDA2EDA.Amount -= receipt.Amount;
                        Entries[TURN].Fax.EDA2EDA.Value  -= receipt.Value;
                        Entries[TURN].Fax.EDA2EDA.Tax    -= receipt.Tax;
                        break;
                    case DDN_EDA2EPM_CALL:
                    case LOCAL_EDA2EPM_CALL:
                        Entries[TURN].Fax.EDA2EPM.Receipts--;
						Entries[TURN].Fax.EDA2EPM.Amount -= receipt.Amount;
                        Entries[TURN].Fax.EDA2EPM.Value  -= receipt.Value;
                        Entries[TURN].Fax.EDA2EPM.Tax    -= receipt.Tax;
                        break;
                    case DDN_EDA2TEL_CALL:
                        Entries[TURN].Fax.EDA2TEL.Receipts--;
                        Entries[TURN].Fax.EDA2TEL.Amount -= receipt.Amount;
                        Entries[TURN].Fax.EDA2TEL.Value  -= receipt.Value;
                        Entries[TURN].Fax.EDA2TEL.Tax    -= receipt.Tax;
                        break;
                        // default not implemented, see at  PHONE_INFO::GetCallAttr()
                    }


#endif
                }
            }
            else
			{
                // v.211
                if (receipt.Stat.Paid == NOT_PAID_CALL)
                {
					Entries[TURN].NotPaid.Receipts--;
                    Entries[TURN].NotPaid.Value   -= receipt.Value;
                }
                else
                { // toll free
                    // this doesn't make sense !!!
                    Entries[TURN].TollFree.Receipts--;
                    Entries[TURN].TollFree.Value   -= receipt.Value;
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
					Entries[TURN].Internet.Nal.Receipts--;
					Entries[TURN].Internet.Nal.Minutes -= receipt.Minutes;
					Entries[TURN].Internet.Nal.PaidMin -= receipt.CeilMin;
					Entries[TURN].Internet.Nal.Value   -= receipt.Value;
					Entries[TURN].Internet.Nal.Tax     -= receipt.Tax;
#if defined(__EDA__)
                    // becareful we must disable the NOT_INC and the CELL mask
                    switch (receipt.Stat.CallAttr & ~NOT_INCLUDED_CALL_MASK)
					{
                    case CELLULAR_CALL :
                        break; // nothing to do
                    case EDA2EDA_CALL:
						Entries[TURN].Internet.EDA2EDA.Receipts--;
						Entries[TURN].Internet.EDA2EDA.Minutes -= receipt.Minutes;
						Entries[TURN].Internet.EDA2EDA.PaidMin -= receipt.CeilMin;
						Entries[TURN].Internet.EDA2EDA.Value   -= receipt.Value;
						Entries[TURN].Internet.EDA2EDA.Tax     -= receipt.Tax;
                        break;
                    case DDN_EDA2EPM_CALL:
                    case LOCAL_EDA2EPM_CALL:
						Entries[TURN].Internet.EDA2EPM.Receipts--;
						Entries[TURN].Internet.EDA2EPM.Minutes -= receipt.Minutes;
						Entries[TURN].Internet.EDA2EPM.PaidMin -= receipt.CeilMin;
						Entries[TURN].Internet.EDA2EPM.Value   -= receipt.Value;
						Entries[TURN].Internet.EDA2EPM.Tax     -= receipt.Tax;
						break;
					case DDN_EDA2TEL_CALL:
						Entries[TURN].Internet.EDA2TEL.Receipts--;
						Entries[TURN].Internet.EDA2TEL.Minutes -= receipt.Minutes;
						Entries[TURN].Internet.EDA2TEL.PaidMin -= receipt.CeilMin;
						Entries[TURN].Internet.EDA2TEL.Value   -= receipt.Value;
						Entries[TURN].Internet.EDA2TEL.Tax     -= receipt.Tax;
                        break;
                        // default not implemented, see at PH_INFO.CPP::GetCallAttr()
                    }
#endif
                }
            }
            else
            {
                // v.211
                if (receipt.Stat.Paid == NOT_PAID_CALL)
                {
                    Entries[TURN].NotPaid.Receipts--;
                    Entries[TURN].NotPaid.Value   -= receipt.Value;
				}
                else
                { // toll free
                    // this doesn't make sense !!!
                    Entries[TURN].TollFree.Receipts--;
                    Entries[TURN].TollFree.Value   -= receipt.Value;
                }
            }
            break;
        }
	case Receipt::CARD:
        {
            // nal because international not implemented !!!
            if (receipt.Stat.Paid & PAID_CALL)
            {
                Entries[TURN].Cards.Receipts--;
                for (int i=0; i<MAX_MAGNETIC_CARDS; i++)
                    Entries[TURN].Cards.Cards[i] -= receipt.Cards[i];
				Entries[TURN].Cards.Value -= receipt.Value;
                Entries[TURN].Cards.Tax   -= receipt.Tax;
            }
            else
            {
                // v.211
                if (receipt.Stat.Paid == NOT_PAID_CALL)
                {
                    Entries[TURN].NotPaid.Receipts--;
                    Entries[TURN].NotPaid.Value   -= receipt.Value;
                }
                else
                { // toll free
                    // this doesn't make sense !!!
                    Entries[TURN].TollFree.Receipts--;
                    Entries[TURN].TollFree.Value   -= receipt.Value;
                }
            }
			break;
        }
	case Receipt::OTHER:
        {
            // nal because international not implemented !!!
            if (receipt.Stat.Paid & PAID_CALL)
            {
                Entries[TURN].Other.Receipts--;
                Entries[TURN].Other.Value -= receipt.Value;
                Entries[TURN].Other.Tax   -= receipt.Tax;
            }
            else
            {
                // v.211
                if (receipt.Stat.Paid == NOT_PAID_CALL)
                {
                    Entries[TURN].NotPaid.Receipts--;
                    Entries[TURN].NotPaid.Value   -= receipt.Value;
				}
                else
                { // toll free
                    // this doesn't make sense !!!
                    Entries[TURN].TollFree.Receipts--;
                    Entries[TURN].TollFree.Value   -= receipt.Value;
                }
            }
            break;
        }
    }
    //
    // --- adjust total values
    //
    // total
    //
    Entries[TURN].Total.Tel =
        Entries[TURN].Tel.Nal.Value+
		Entries[TURN].Tel.Inter.Value+
        //
        CellularEntries[TURN].Tel.Value
        ;
    Entries[TURN].Total.Special =
        Entries[TURN].SpecialTel.Nal.Value+
        Entries[TURN].SpecialTel.Inter.Value+
        //
        CellularEntries[TURN].SpecialTel.Value+
        //
        Entries[TURN].Fax.Nal.Value+
        Entries[TURN].Fax.Inter.Value+
		Entries[TURN].Internet.Nal.Value+
		Entries[TURN].Cards.Value+
        Entries[TURN].Other.Value
        ;
#if defined(__EDA__)
	Entries[TURN].Total.EDA2EDA =
        Entries[TURN].Tel.EDA2EDA.Value +
        Entries[TURN].SpecialTel.EDA2EDA.Value +
        Entries[TURN].Fax.EDA2EDA.Value +
		Entries[TURN].Internet.EDA2EDA.Value
        ;
    Entries[TURN].Total.EDA2EPM =
        Entries[TURN].Tel.EDA2EPM.Value +
        Entries[TURN].SpecialTel.EDA2EPM.Value +
        Entries[TURN].Fax.EDA2EPM.Value +
		Entries[TURN].Internet.EDA2EPM.Value
        ;
    Entries[TURN].Total.EDA2TEL =
        Entries[TURN].Tel.EDA2TEL.Value +
        Entries[TURN].SpecialTel.EDA2TEL.Value +
        Entries[TURN].Fax.EDA2TEL.Value +
		Entries[TURN].Internet.EDA2TEL.Value+
        // remember all inter services are EDA2TEL
		Entries[TURN].Tel.Inter.Value      +
        Entries[TURN].SpecialTel.Inter.Value +
        Entries[TURN].Fax.Inter.Value 
		;
#endif
    Entries[TURN].Total.NotPaid = Entries[TURN].NotPaid.Value+Entries[TURN].TollFree.Value;
    Entries[TURN].Total.General = Entries[TURN].Total.Tel+Entries[TURN].Total.Special;
    //
    // Tax
    //
    Entries[TURN].Tax.Tel =
        Entries[TURN].Tel.Nal.Tax+
        Entries[TURN].Tel.Inter.Tax+
        //
        CellularEntries[TURN].Tel.Tax
        ;
    Entries[TURN].Tax.Special =
		Entries[TURN].SpecialTel.Nal.Tax+
        Entries[TURN].SpecialTel.Inter.Tax+
        //
        CellularEntries[TURN].SpecialTel.Tax+
        //
        Entries[TURN].Fax.Nal.Tax+
        Entries[TURN].Fax.Inter.Tax+
		Entries[TURN].Internet.Nal.Tax+
		Entries[TURN].Cards.Tax+
        Entries[TURN].Other.Tax
        ;
#if defined(__EDA__)
    // Tax instead of value
    Entries[TURN].Tax.EDA2EDA =
        Entries[TURN].Tel.EDA2EDA.Tax +
        Entries[TURN].SpecialTel.EDA2EDA.Tax +
        Entries[TURN].Fax.EDA2EDA.Tax +
		Entries[TURN].Internet.EDA2EDA.Tax
        ;
    Entries[TURN].Tax.EDA2EPM =
        Entries[TURN].Tel.EDA2EPM.Tax +
        Entries[TURN].SpecialTel.EDA2EPM.Tax +
        Entries[TURN].Fax.EDA2EPM.Tax +
		Entries[TURN].Internet.EDA2EPM.Tax
        ;
    Entries[TURN].Tax.EDA2TEL =
        Entries[TURN].Tel.EDA2TEL.Tax +
        Entries[TURN].SpecialTel.EDA2TEL.Tax +
        Entries[TURN].Fax.EDA2TEL.Tax +
		Entries[TURN].Internet.EDA2TEL.Tax+
        // remember all inter services are EDA2TEL
        Entries[TURN].Tel.Inter.Tax      +
        Entries[TURN].SpecialTel.Inter.Tax +
		Entries[TURN].Fax.Inter.Tax
		;
#endif
    Entries[TURN].Tax.General = Entries[TURN].Tax.Tel+Entries[TURN].Tax.Special;
    //
    // Dual printer: odd and even booth numbers
    //
    DoublePRNEntries[receipt.BoothNumber%DS_MAXDOUBLEPRNENTRIES] -= receipt; // only the cost
    //
	return TRUE;
}

BOOL DB_STATISTICS::Update(void)
{
    //
    // be careful with the order GCC/gcc !!!
    //
    WORD sysYear, sysMonth, sysDay, sysDayOfWeek;
    _UnpackDate(_GetSysDate(), sysYear, sysMonth, sysDay, sysDayOfWeek);
	//
    // is it time to reset statistics ?
    //
    WORD year, month, day, dayOfWeek;
    _UnpackDate(Entries[YEAR].To.Date, year, month, day, dayOfWeek);
    if (year < sysYear)
    {
        // create a valid general statistics
        // entries
        Entries[YEAR].Init();
        Entries[MONTH].Init();
        Entries[WEEK].Init();
        Entries[DAY].Init();
        // cellular entries
        CellularEntries[YEAR].Init();
        CellularEntries[MONTH].Init();
        CellularEntries[WEEK].Init();
        CellularEntries[DAY].Init();
	}
    _UnpackDate(Entries[MONTH].To.Date, year, month, day, dayOfWeek);
    if (month < sysMonth)
    {
        // entries
        Entries[MONTH].Init();
        Entries[WEEK].Init();
        Entries[DAY].Init();
        // cellular entries
        CellularEntries[MONTH].Init();
        CellularEntries[WEEK].Init();
        CellularEntries[DAY].Init();
    }
    dayOfWeek = _GetDayOfWeek(Entries[WEEK].To.Date);
	if
	(
		// lookout: last close between thursday and sunday
		(dayOfWeek==5 || dayOfWeek==6 || dayOfWeek==7 || dayOfWeek==1) &&
		// lookout: current open between monday and wednesday
		(sysDayOfWeek==2 || sysDayOfWeek==3 || sysDayOfWeek==4)
    )
    {
        // entries
        Entries[WEEK].Init();
        Entries[DAY].Init();
        // cellular entries
        CellularEntries[WEEK].Init();
        CellularEntries[DAY].Init();
    }

    if (Entries[DAY].To.Date < _GetSysDate())
    {
        // entries
        Entries[DAY].Init();
        // cellular entries
        CellularEntries[DAY].Init();
	}
    // entries
    Entries[DAY]   += Entries[TURN];
    Entries[WEEK]  += Entries[TURN];
    Entries[MONTH] += Entries[TURN];
    Entries[YEAR]  += Entries[TURN];

	// v.211 reset error counters
	g_cfg->N_DIAL_ERR = 0;
	g_cfg->N_COM_ERR  = 0;
	g_cfg->Save();

	//
    Entries[TURN].Init();

	// cellular entries
    CellularEntries[DAY]   += CellularEntries[TURN];
    CellularEntries[WEEK]  += CellularEntries[TURN];
	CellularEntries[MONTH] += CellularEntries[TURN];
    CellularEntries[YEAR]  += CellularEntries[TURN];
	CellularEntries[TURN].Init();

    // double prn
    for (int i=0; i<DS_MAXDOUBLEPRNENTRIES; i++)
		DoublePRNEntries[i].Init();

	return TRUE;
}

long DB_STATISTICS::GetTelEntries(void)
{
    long number = Entries[TURN].Tel.Inter.Receipts;
#if defined(__EDA__)
    number +=
        Entries[TURN].Tel.EDA2EDA.Receipts +
        Entries[TURN].Tel.EDA2EPM.Receipts +
		Entries[TURN].Tel.EDA2TEL.Receipts
        ;
#else
number += Entries[TURN].Tel.Nal.Receipts;
#endif
    return number;
}

BOOL DB_STATISTICS::Archive(void)
{
    if (ReadOnly)
		return FALSE;

	Flush();
	close(File);
	if (access(Filename, 6) != 0)
		chmod(Filename, S_IREAD|S_IWRITE); // enable read/write
	//
	_mkSysDateDir();
	FILE_NAME arcFilename;
	STR16 tmp;
	_getSysDatePath(arcFilename);
	_PrefixAppPath(arcFilename);
	WORD year, month, day;
	_GetSysDate(year, month, day);
	extern CFG *g_cfg;
	sprintf(tmp, "\\RX%02d_%02d%s", day, g_cfg->TURN_NUMBER, STATISTICS_EXT);
	strcat(arcFilename, tmp);
	if (access(arcFilename, 6) != 0)
		chmod(arcFilename, S_IREAD|S_IWRITE); // enable read/write
	unlink(arcFilename); // sorry !!!
	rename(Filename, arcFilename);
	File = open(Filename, O_RDWR|O_BINARY, S_IREAD|S_IWRITE);
	// Since the necesary files to repeat the status of the database are
	// stored we can now Update the statistics other than the turn.
	Update();

	return TRUE;
}

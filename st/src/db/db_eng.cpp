//
// [ DB_ENG.CPP ]
//

#include "stdst.h"

#include <control.h>
#include <db_eng.h>

#if !defined(__TEST__)
#include <mdb_stor.h>
#include <mdb_stat.h>
#endif

#if !defined(__TEST__)
#include <stm2.h>
#endif

#include <info.h>
#include <st_bids.h>

#if !defined(__TEST__)
#if !defined(__NOAPPINFO__)
extern SUPER_APP_INFO g_superAppInfo;
#endif
#endif

extern CFG *g_cfg;

static const char *DB_NAME     = "RX";
static const char *DB_EXT_NAME = "RXX";

// ----------------------------------------------------------------------------
// [ DB_ENGINE ]
// ----------------------------------------------------------------------------

#if !defined(__TEST__)
#if !defined(__NOSTM2__)
extern STM2 *g_STM2;
#endif
#endif

DB_ENGINE::DB_ENGINE(void)
{
	// current turn — select backend by config
#if !defined(__TEST__)
	if (g_cfg->MINIDB)
	{
		DBStorage    = new MiniDBReceiptStorage(NULL, DB_NAME, FALSE);
	}
	else
#endif
	{
		DBStorage    = new DB_STORAGE(NULL, DB_NAME, FALSE);
	}
	// Statistics always use FlatFile (.STA) regardless of MINIDB setting.
	// MiniDB replaces the receipt database only; statistics are simpler
	// and benefit from the existing .STA repair/recovery path.
	DBStatistics = new DB_STATISTICS(NULL, DB_NAME, FALSE);
	//
#if !defined(__TEST__)
#if !defined(__NOAPPINFO__)
	if (g_superAppInfo.Attr.STPro)
	{
		DBExtStorage    = new DB_STORAGE(NULL, DB_EXT_NAME, FALSE);
		DBExtStatistics = new DB_EXT_STATISTICS(NULL, DB_EXT_NAME, FALSE);
	}
#endif
#endif
	// another turn
	ArcDBStorage    = NULL;
	ArcDBStatistics = NULL;
#if !defined(__TEST__)
#if !defined(__NOSTM2__)
	if (g_STM2->getStatus() == STM2::BAD_SHUTDOWN)
		Recover();
	g_STM2->put(STM2::STATISTICSENTRIES, (*DBStatistics)[0]);
	g_STM2->put(STM2::STATISTICSDOUBLEPRNENTRIES, DBStatistics->GetDoublePrnEntry(0));
	g_STM2->put(STM2::STATISTICSCELLULARENTRIES, DBStatistics->GetCellularEntry(0));
#if !defined(__NOAPPINFO__)
	if (g_superAppInfo.Attr.STPro)
		g_STM2->put(STM2::EXTENSIONCRITICALSTATISTICS, DBExtStatistics->GetCritical());
#endif
#endif
#endif
}

DB_ENGINE::~DB_ENGINE(void)
{
    delete DBStatistics;
    delete DBStorage;
    //
#if !defined(__TEST__)
#if !defined(__NOAPPINFO__)
	if (g_superAppInfo.Attr.STPro)
    {
        delete DBExtStatistics;
		delete DBExtStorage;
    }
#endif
#endif
}

void DB_ENGINE::Flush(void)
{
    DBStorage->Flush();
    DBStatistics->Flush();
#if !defined(__TEST__)
#if !defined(__NOAPPINFO__)
	if (g_superAppInfo.Attr.STPro)
    {
        DBExtStorage->Flush();
        DBExtStatistics->Flush();
    }
#endif
#if !defined(__NOSTM2__)
	g_STM2->emptyReceipts(); // we assume the receipts are stored
#endif
#endif
}

BOOL DB_ENGINE::Repair(void)
{
	DBStorage->Repair();
	DBStatistics->Repair(DBStorage);
#if !defined(__TEST__)
#if !defined(__NOSTM2__)
	g_STM2->put(STM2::STATISTICSENTRIES, (*DBStatistics)[0]);
	g_STM2->put(STM2::STATISTICSDOUBLEPRNENTRIES, DBStatistics->GetDoublePrnEntry(0));
	g_STM2->put(STM2::STATISTICSCELLULARENTRIES, DBStatistics->GetCellularEntry(0));
#endif
#endif
	return TRUE;
}

BOOL DB_ENGINE::Archive(void)
{
	DBStorage->Archive();
	DBStatistics->Archive();

	// by repairing we force to re-Init the database files
	Repair();

#if !defined(__TEST__)
#if !defined(__NOSTM2__)
	// 2.21.1 Build 2
	g_STM2->emptyReceipts(); // we assume the receipts are stored
#endif
#endif

	return TRUE;
}

BOOL DB_ENGINE::ExtArchive(void)
{
#if !defined(__TEST__)
#if !defined(__NOAPPINFO__)
	if (!g_superAppInfo.Attr.STPro)
		return FALSE;
#endif
#endif
	DBExtStorage->Archive();
#if !defined(__TEST__)
#if !defined(__NOAPPINFO__)
	if (g_superAppInfo.Attr.STPro)
		DBExtStatistics->Archive();
	// by repairing we force to re-Init the database files
	if (g_superAppInfo.Attr.STPro)
		ExtRepair();
#endif
#endif

#if !defined(__TEST__)
#if !defined(__NOSTM2__)
	// 2.21.1 Build 2
	g_STM2->emptyReceipts(); // we assume the receipts are stored
#endif
#endif

	return TRUE;
}

BOOL DB_ENGINE::ExtRepair(void)
{
#if !defined(__TEST__)
#if !defined(__NOAPPINFO__)
	if (!g_superAppInfo.Attr.STPro)
        return FALSE;
    DBExtStorage->Repair();
	DBExtStatistics->Repair(DBExtStorage);
#endif
#if !defined(__NOSTM2__)
	g_STM2->put(STM2::EXTENSIONCRITICALSTATISTICS, DBExtStatistics->GetCritical());
#endif
#endif
	return TRUE;
}

void DB_ENGINE::Recover(void)
{
#if !defined(__TEST__)
	// Rebuild because the file system may will be corrupt
	DBStorage->Repair();

#if !defined(__NOAPPINFO__)
	if (g_superAppInfo.Attr.STPro)
		DBExtStorage->Repair();
#endif // !defined(__NOAPPINFO__)

	// now bring records from STM2 but taking care of repeated receipts.
#if !defined(__NOSTM2__)
	Receipt receipt;
	Receipt tmpReceipt;

	while (g_STM2->get(STM2::RECEIPTS, &receipt))
	{
		if (DBStorage->IsValid(receipt))
		{
			if (receipt.Stat.Extension)
			{
				if (g_superAppInfo.Attr.STPro)
				{
					if (!DBExtStorage->Get(tmpReceipt, receipt.Number)) // the dynReceipt is not in database
						DBExtStorage->Add(receipt);
				}
			}
			else
			{
				if (!DBStorage->Get(tmpReceipt, receipt.Number))  // the dynReceipt is not in database
					DBStorage->Add(receipt);
			}
		}
	}

	// in manual mode we need to recover the receipts for each booth
	// if the user change the mode while the system is down the dynReceipt
	// will be generated.

	LSDList lsdlManualReceipts[MAX_BOOTH];

	// Collect manual mode receipts

	DB_STORAGE::Iterator it(*DBStorage->GetConcreteStorage());
	it.Restart();

	long number;
	while (it)
	{
		number = it.Current();

		if (Get(receipt, number))
		{
			if (receipt.Stat.Manual && receipt.extendedStat.nonProcessed)
			{
				lsdlManualReceipts[receipt.BoothNumber].add(receipt.Number);
			}
		}

		it++;
	}

	// see if the receipts are valid for this turn

	for (int i = 0; i < MAX_BOOTH; ++i)
	{
		WORD cNum, bNum;
		cNum = i/CLUSTER_SIZE;
		bNum = i%CLUSTER_SIZE;

		int nCalls = 0;
		if (i < g_cfg->ACTIVE_CLUSTERS*CLUSTER_SIZE) // valid current booth?
		{
			nCalls = CONTROLLER::RTEngineGetNumOfCalls(cNum, bNum);
		}

		LSDListIterator it(lsdlManualReceipts[i]);

		// start from tail to include only recent valid receipts
		it.restartAtTail();
		int n = 0;
		while (it)
		{
			++n;

			if (!Get(receipt, it.current()))
			{
				continue ;  // !!!
			}

			DynamicReceipt dynReceipt;
			dynReceipt.m_r = receipt;

			if (i < g_cfg->ACTIVE_CLUSTERS*CLUSTER_SIZE) // valid current booth?
			{
				if (n <= nCalls)
				{
					// include in current list of manual receipts
					dynReceipt.m_r.Stat.Cooked     = FALSE; // force re-cook !!!
					//
					dynReceipt.Attr_.HeaderOn  = TRUE;
					dynReceipt.Attr_.FooterOn  = TRUE;
					dynReceipt.Attr_.SummaryOn = FALSE;
					dynReceipt.Attr_.Storable  = FALSE; // we suposse it
					dynReceipt.PreValue_       = 0;     // sorry we lost the pre-paid
					dynReceipt.Attr_.Countable = (g_cfg->IsExtension(cNum, bNum))?TRUE:!g_cfg->MANUAL;
					dynReceipt.Attr_.Printable = (g_cfg->IsExtension(cNum, bNum))?TRUE:!g_cfg->MANUAL;

					CONTROLLER::Receipts->Put(dynReceipt); // to continue processing
				}
				else
				{
					// fix old receipts to avoid reappearence
					dynReceipt.m_r.extendedStat.nonProcessed = FALSE; // fix it
					Update(dynReceipt);
				}
			}
			else
			{
				// old dynReceipt
				dynReceipt.m_r.extendedStat.nonProcessed = FALSE; // fix it
				Update(dynReceipt);
			}

			--it;
		}
	}

	// end 2.21.8

	//
	g_STM2->get(STM2::STATISTICSENTRIES, (*DBStatistics)[0]);
	g_STM2->get(STM2::STATISTICSDOUBLEPRNENTRIES, DBStatistics->GetDoublePrnEntry(0));
	g_STM2->get(STM2::STATISTICSCELLULARENTRIES, DBStatistics->GetCellularEntry(0));
	if (g_superAppInfo.Attr.STPro)
		g_STM2->get(STM2::EXTENSIONCRITICALSTATISTICS, DBExtStatistics->GetCritical());
#endif // !defined(__NOSTM2__)

	Flush();

#endif // !defined(__TEST__)
}

BOOL DB_ENGINE::Add(DynamicReceipt& dynReceipt)
{
	/////////////////////////////////////////////////////////////////
	//  Storage

	BOOL bStored = FALSE;

	if (dynReceipt.Attr_.Storable)
	{
		if (!dynReceipt.m_r.Stat.Extension)
		{
			bStored = DBStorage->Add(dynReceipt.m_r);
		}
		else
		{
#if !defined(__TEST__)
#if !defined(__NOAPPINFO__)
			if (g_superAppInfo.Attr.STPro)
			{
				bStored = DBExtStorage->Add(dynReceipt.m_r);
			}
#endif
#endif
		}

		if (!bStored)
		{
			return FALSE;
		}

		dynReceipt.Attr_.Storable = FALSE;

#if !defined(__TEST__)
#if !defined(__NOSTM2__)
		g_STM2->put(STM2::RECEIPTS, &dynReceipt.m_r);
#endif
#endif
	}

	/////////////////////////////////////////////////////////////////
	// statistics

	BOOL bCounted = FALSE;

	if (dynReceipt.Attr_.Countable)
	{
		if (!dynReceipt.m_r.Stat.Extension)
		{
			bCounted = DBStatistics->Add(dynReceipt.m_r);
#if !defined(__TEST__)
#if !defined(__NOSTM2__)
			g_STM2->put(STM2::STATISTICSENTRIES, (*DBStatistics)[0]);
			g_STM2->put(STM2::STATISTICSDOUBLEPRNENTRIES, DBStatistics->GetDoublePrnEntry(0));
			g_STM2->put(STM2::STATISTICSCELLULARENTRIES, DBStatistics->GetCellularEntry(0));
#endif
#endif
		}
		else
		{
#if !defined(__TEST__)
#if !defined(__NOSTM2__)
			if (g_superAppInfo.Attr.STPro)
			{
				bCounted = DBExtStatistics->Add(dynReceipt.m_r);
				if (bCounted)
				{
					g_STM2->put(STM2::EXTENSIONCRITICALSTATISTICS, DBExtStatistics->GetCritical());
				}
			}
#endif
#endif
		}

		dynReceipt.Attr_.Countable = !bCounted;
	}

	return bCounted;
}

// BOOL DB_ENGINE::operator <<(DynamicReceipt& dynReceipt)
// 2.21.8
BOOL DB_ENGINE::Update(DynamicReceipt& dynReceipt)
{
	BOOL bUpdated = FALSE;
	if (!dynReceipt.m_r.Stat.Extension)
	{
		bUpdated = DBStorage->Update(dynReceipt.m_r);
	}
	else
	{
#if !defined(__TEST__)
#if !defined(__NOAPPINFO__)
		if (g_superAppInfo.Attr.STPro)
			bUpdated = DBExtStorage->Update(dynReceipt.m_r);
#endif
#endif
	}

	return bUpdated;
}

BOOL DB_ENGINE::Update(long number, WORD modifier)
{
	//
	// let a dynReceipt to change its attributtes but not its contents.
	//
	// the dynReceipt will first Subtract from statistics and then
	// added again with the new attribute.
	//

	// Just for booth receipts, not for extensions !!!
	Receipt dynReceipt;

	// begin 2.21.8 build 6

	if (!DBStorage->Get(dynReceipt, number))
		return FALSE;

	if (dynReceipt.Stat.Paid == modifier)
		return TRUE; // nothing to do, but everything is ok

	if (!DBStatistics->Subtract(dynReceipt))
		return FALSE;

	dynReceipt.Stat.Paid = modifier;

	if (!DBStorage->Update(dynReceipt))
		return FALSE;

	if (!DBStatistics->Add(dynReceipt, FALSE))
		return FALSE;

#if !defined(__TEST__)
#if !defined(__NOSTM2__)
	g_STM2->put(STM2::STATISTICSENTRIES, (*DBStatistics)[0]);
	g_STM2->put(STM2::STATISTICSDOUBLEPRNENTRIES, DBStatistics->GetDoublePrnEntry(0));
	g_STM2->put(STM2::STATISTICSCELLULARENTRIES, DBStatistics->GetCellularEntry(0));
#endif
#endif

	// end 2.21.8 build 6

	return TRUE;
}

BOOL DB_ENGINE::LoadArcDB(WORD date, WORD turn)
{
    if (ArcDBStorage)
		return FALSE; // previous unloadded DB

	// archive Path
	FILE_NAME arcPath;
	WORD year, month, day;
	_UnpackDate(date, year, month, day);
	sprintf(arcPath, "%04d\\%02d", year, month);
	_PrefixAppPath(arcPath);
	// Archive name
	FILE_NAME arcName;
	sprintf(arcName, "%s%02d_%02d", DB_NAME, day, turn); // v.219d
	//
	ArcDBStorage = new DB_STORAGE(arcPath, arcName);
	if (ArcDBStorage->GetStatus() != DB_STORAGE::OK)
	{
		delete ArcDBStorage;
		ArcDBStorage = NULL;

		return FALSE;
	}

	ArcDBStatistics = new DB_STATISTICS(arcPath, arcName);

	return TRUE;
}

void DB_ENGINE::UnloadArcDB(void)
{
    if (ArcDBStatistics)
		delete ArcDBStatistics;

    if (ArcDBStorage)
		delete ArcDBStorage;

	ArcDBStorage    = NULL;
	ArcDBStatistics = NULL;
}


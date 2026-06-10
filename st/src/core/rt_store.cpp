//
// [ RT_STORE.CPP ]
//

#include "stdst.h"

#include <ph_eng.h>
#include <rt_eng.h>
#include <dstorage.h>

extern CFG 	*g_cfg;

void ENGINE::StoreReceipt(WORD cNum, WORD bNum)
{
#if defined (__TEST__)
    SetFSs(cNum, bNum, LOCK, LOCK);
#else
	{
		RTReceiptQueueMutex mutex;

		static BOOL isExtension;
		isExtension = g_cfg->IsExtension(cNum, bNum);
		if (!Clusters[cNum].NoStatistics[bNum])
		{
			if (isExtension)
			{
				g_cfg->E_N_RECEIPT = (g_cfg->E_N_RECEIPT+1)%BinStorage::MAX_RECEIPTS;
				if (!g_cfg->E_N_RECEIPT)
					g_cfg->E_N_RECEIPT++;
			}
			else
			{
				g_cfg->N_RECEIPT = (g_cfg->N_RECEIPT+1)%BinStorage::MAX_RECEIPTS;
				if (!g_cfg->N_RECEIPT)
					g_cfg->N_RECEIPT++;
			}
		}
		// count call. v.219
		Clusters[cNum].NumOfCalls[bNum]++;
		// erase num of calls v.219
		if (!g_cfg->MANUAL)
		{
			SetNumOfCalls(cNum, bNum, 0);
		}
		//
		static DynamicReceipt s_dynReceipt;
		static WORD s_boothCount;

		s_boothCount = cNum * CLUSTER_SIZE + bNum;
		// generate and store the receipt data
		s_dynReceipt.m_r.MagicNumber   = BinStorage::MAGIC_NUMBER;
		s_dynReceipt.m_r.Stat.Printed  = Clusters[cNum].NoReceipt[bNum];
		s_dynReceipt.m_r.Stat.Archived = Clusters[cNum].NoStatistics[bNum];
		s_dynReceipt.m_r.Stat.Paid     = PAID_CALL;
		// some kind of fields will be update or correct into DControl !!!
		s_dynReceipt.m_r.BoothNumber = s_boothCount;
		s_dynReceipt.m_r.Time        = Clusters[cNum].StartTimes[bNum];
		s_dynReceipt.m_r.Date        = Clusters[cNum].StartDates[bNum];
		s_dynReceipt.m_r.Number      = (isExtension)?g_cfg->E_N_RECEIPT:g_cfg->N_RECEIPT;
		strcpy(s_dynReceipt.m_r.Phone, Clusters[cNum].Phones[bNum]);
		s_dynReceipt.m_r.ElapsedTime = GetElapsedCount(cNum, bNum);
		SetFinalElapsedCount(cNum, bNum, GetElapsedCount(cNum, bNum)); // 2.21.1 B4
		s_dynReceipt.m_r.Tariff      = Clusters[cNum].Tariffs[bNum];
		s_dynReceipt.m_r.Stat.CallAttr = Clusters[cNum].CallAttrs[bNum];
		// lookout MANUAL doesn't affect extensions
		s_dynReceipt.m_r.Stat.Manual    = (isExtension)?FALSE:g_cfg->MANUAL;
		s_dynReceipt.m_r.Stat.Extension = isExtension;
		//
		s_dynReceipt.m_r.extendedStat.nonProcessed = TRUE; // only for recover in manual mode
		// MetaReceipt members
		s_dynReceipt.m_r.Tag             = Receipt::TEL;
		s_dynReceipt.m_r.Stat.Cooked     = FALSE;
		s_dynReceipt.Attr_.HeaderOn  = TRUE;
		s_dynReceipt.Attr_.FooterOn  = TRUE;
		s_dynReceipt.Attr_.SummaryOn = FALSE;
		s_dynReceipt.Attr_.Storable  = TRUE;
		s_dynReceipt.Attr_.Countable = (isExtension)?TRUE:!g_cfg->MANUAL;
		s_dynReceipt.Attr_.Printable = (isExtension)?TRUE:!g_cfg->MANUAL;
		// percent will be set
		// !!!
		// Be careful
		// we cannot use emulated floats into an interrupt because it uses an
		// interrupt. You have to use mem functions
		// repeat: don't use:
		//		dynReceipt.m_r.PreValue = 0.0F;
		// or another approach.
		// GCC/gcc
		// !!!
		memset(&s_dynReceipt.PreValue_, 0, sizeof(double));
		if (Clusters[cNum].PrePaid[bNum])
		{
			if (g_cfg->MANUAL && g_cfg->MULTIPLE_PREPAID_CALLS)
			{ // v.219
				if (GetFirstPreValue(cNum, bNum))
				{
					memcpy(&s_dynReceipt.PreValue_, &Clusters[cNum].PreValue[bNum], sizeof(double));
					SetFirstPreValue(cNum, bNum, FALSE);
				}
			}
			else
			{
				memcpy(&s_dynReceipt.PreValue_, &Clusters[cNum].PreValue[bNum], sizeof(double));
				SetPreValue(cNum, bNum, 0.0F); // to avoid confusion. v.218c
			}

			SetPreTime (cNum, bNum, 0U);
			SetPrePaid (cNum, bNum, FALSE);
			//
			Clusters[cNum].DataPort.Spy &= ~(1 << bNum);
			GeneralPort &= (~GP_BEEP);
		}
		if (!Receipts->Put(s_dynReceipt))
		{
			// I can't believe it !!!
			// we have to change the variable receipt for tmpDynReceipt
			// what a foolish player
			// GCC/gcc
			// sorry but I need space
			static DynamicReceipt s_tmpDynReceipt;
			Receipts->Get(s_tmpDynReceipt); // bye to a random receipt
			Receipts->Put(s_dynReceipt);
		}
	} // Mutex
	SetFSs(cNum, bNum, LOCK, LOCK);

	if (Clusters[cNum].Simula[bNum])
	{
		Clusters[cNum].Simula[bNum] = FALSE;
		if (!Clusters[cNum].Available)
			SetToneFS(cNum, bNum, NOPHONE);
	}
	/*
	// here we cop with a contention device situation
	// but if the finite state machine goes LOCK without
	// save the receipt this would be an error. We expect
	// that the other system clients loose control quickly.
	else
	{
		TraceInfo::s_nSemWait++;
		_ES = 0xC0C0; // JCAR/gcc trigger GP
	}
	*/

#endif
}

// -------------------------------------------------------------------------
// ForceStoreActiveCalls: 2.50 -- settle calls still connected (TALK) so a
// graceful stop / exit bills them instead of dropping them.  Enqueues a
// receipt for every booth past T_TALK (StoreReceipt locks the booth so it
// is not re-stored); shorter, pre-answer calls are dropped, matching
// DoStore's own threshold.  Safe to call when idle (no-op).
// -------------------------------------------------------------------------
void ENGINE::ForceStoreActiveCalls(void)
{
	for (WORD cNum = 0; cNum < g_cfg->ACTIVE_CLUSTERS; cNum++)
		for (WORD bNum = 0; bNum < CLUSTER_SIZE; bNum++)
		{
			if ((GetToneFS(cNum, bNum) == TALK || GetPulseFS(cNum, bNum) == TALK)
			    && GetElapsedCount(cNum, bNum) >= g_cfg->T_TALK)
				StoreReceipt(cNum, bNum);
		}
}

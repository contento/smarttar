//
// [ CONTROL.CPP ]
//

#include "stdst.h"

#include <control.h>
#include <db_eng.h>
#include <eng_fact.h>

#if !defined(__TEST__)
#include <stm2.h>
#endif

#include <spooler.h>
#include <menubar.h>
#include <toolbar.h>
#include <ph_eng.h>
#include <ssaver.h>
#include <events.h>

//
// --- Global Objects
//
CFG        *g_cfg 		= NULL;
PH_ENGINE  *g_phEngine 	= NULL;
SPOOLER    *g_spooler	= NULL;
SSAVER     *g_ssaver 	= NULL;
DB_ENGINE  *g_dbEngine 	= NULL;

extern STM2 *g_STM2;
// --- Global variables
BOOL   g_extAreChangeable = FALSE;

// ---------------------------------------------------------------------------
//     CONTROLLER
// ---------------------------------------------------------------------------

ENGINE                      	*CONTROLLER::RTEngine    = NULL;
CIRCULAR_QUEUE<DynamicReceipt> 	*CONTROLLER::Receipts    = NULL;
CIRCULAR_QUEUE<DynamicReceipt> 	*CONTROLLER::PRNReceipts = NULL;

static char *errorMemory = NULL;
static const WORD MAX_RECEIPTS     = 0x100;
static const WORD MAX_PRN_RECEIPTS = 0x40;

CONTROLLER::ManualInfoItem *CONTROLLER::manualInfo = NULL;
UIW_VIEW * CONTROLLER::View = NULL;

CONTROLLER::CONTROLLER(UI_EVENT_MANAGER *eventManager, UI_WINDOW_MANAGER *windowManager)
		: UI_DEVICE(E_CONTROLLER, D_ON),
		EventManager(eventManager),
		CashReq(FALSE),
		Refresh(FALSE),
		watchDog(FALSE)
{
#ifdef DOSX286
	DosSetExceptionHandler(0x0D, NewGPFHandler, &OldGPFHandler);
#endif
	//
	// Set up a free-store exception handler.
	errorMemory = new char[0x2000];
	extern void (*_new_handler)(void);
	_new_handler = NewHandler;
	//
	g_cfg = new CFG;
	g_cfg->Load();
	// Old __DEMO__-build behavior: bounds-check unconditionally.
	// Old real-build behavior: STM2 recovery on BAD_SHUTDOWN, then
	// bounds-check inside the same if-block.  Preserve both:
	BOOL doBoundsCheck = g_cfg->IsDemoMode();
	if (!g_cfg->IsDemoMode() && g_STM2->getStatus() == STM2::BAD_SHUTDOWN)
	{
		g_STM2->get(STM2::RECEIPTNUMBER, &g_cfg->N_RECEIPT);
		g_STM2->get(STM2::EXTENSIONRECEIPTNUMBER, &g_cfg->E_N_RECEIPT);
		doBoundsCheck = TRUE;
	}
	if (doBoundsCheck)
	{
		// avoid bad record number
		if (g_cfg->N_RECEIPT < 0 || g_cfg->N_RECEIPT >= DB_STORAGE::MAX_RECEIPTS)
			g_cfg->N_RECEIPT = 0;

		if (g_cfg->E_N_RECEIPT < 0 || g_cfg->E_N_RECEIPT >= DB_STORAGE::MAX_RECEIPTS)
			g_cfg->E_N_RECEIPT = 0;
	}

	g_phEngine = new PH_ENGINE;
	g_phEngine->Load();

	// RTEngine
	RTEngine   = MakeEngine(g_cfg->CLUSTERS);

	UI_DATE date;
	int intDate;
	date.Export(&intDate); // mark engine date
	RTEngine->SetCurrentDate(intDate);

	UI_TIME time;
	int intTime;
	time.Export(&intTime); // mark engine time
	RTEngine->SetCurrentTime(intTime);

	// --- look out the secuence must be kept !!!
	Receipts    = new CIRCULAR_QUEUE<DynamicReceipt>(MAX_RECEIPTS);
	PRNReceipts = new CIRCULAR_QUEUE<DynamicReceipt>(MAX_PRN_RECEIPTS);
	g_spooler  = new SPOOLER((!g_cfg->DOUBLE_PRN)?1:2);
	g_ssaver   = new SSAVER;
	g_dbEngine = new DB_ENGINE;
	//
	if (g_cfg->GetStatus() == CFG::OK)
    {
		g_cfg->Save();
	}

	prnFormatter = new PrnFormatter(g_cfg->FORM);
	//
	*EventManager
		+ g_spooler
		+ g_ssaver
	;

	Alarms = new BOOL[g_cfg->ACTIVE_CLUSTERS][CLUSTER_SIZE];
	View = new UIW_VIEW(g_cfg->CLUSTERS);
	*windowManager
		+ View;
	// This line assigns the exit function to be called before the main
	// Window is closed.  It MUST be after the Window is added to windowManager.
	windowManager->screenID = View->screenID;
	//
	manualInfo   = new ManualInfoItem[g_cfg->ACTIVE_CLUSTERS*CLUSTER_SIZE];
	memset(manualInfo, 0, sizeof(ManualInfoItem)*g_cfg->ACTIVE_CLUSTERS*CLUSTER_SIZE);
	//
	// booth display
	boothDisplay    = NULL;
	m_pDisplayInfos = NULL;
	if (g_cfg->DISPLAY_ENABLE)
	{
		boothDisplay = new BoothDisplay();
		if
		(
			!boothDisplay->install
			(
				g_cfg->DISPLAY_DEFAULT_MESSAGE,
				g_cfg->DISPLAY_COM,
				g_cfg->DISPLAY_BAUDS
			)
		)
		{
			delete boothDisplay;
			boothDisplay = NULL;
		}
		else
		{
			// to be sure !
			boothDisplay->setDefaultMessage(g_cfg->DISPLAY_DEFAULT_MESSAGE);

			// Keep display info 2.21.1 Build 5
			m_pDisplayInfos = new BoothDisplay::Info[g_cfg->ACTIVE_CLUSTERS*CLUSTER_SIZE];
			for (int i =  0; i < g_cfg->ACTIVE_CLUSTERS*CLUSTER_SIZE; i++)
			{
				m_pDisplayInfos[i].displayNum = i+1;
				m_pDisplayInfos[i].boothNum   = i+1;
			}
		}
	}

	if (!g_cfg->IsDemoMode() && TraceInfo::s_bTest)
	{
		ReplaceDump();
	}

	//
	// !!! to test GP Exception handler
	//_ES = 0xC0C0;
}

CONTROLLER::~CONTROLLER()
{
	// flush all receipts

	DynamicReceipt dynReceipt;
	while (RTEngine->GetReceipt(dynReceipt))
	{
		if (!dynReceipt.m_r.Stat.Printed && dynReceipt.Attr_.Printable)
			dynReceipt.m_r.Stat.Printed = PrintReceipt(dynReceipt);
		if (!dynReceipt.m_r.Stat.Archived)
		{
			dynReceipt.m_r.Stat.Archived = TRUE;
			dynReceipt.m_r.Stat.Archived = g_dbEngine->Add(dynReceipt); // 2.21.8
		}
	}

	if (g_cfg->GetStatus() == CFG::OK)
		g_cfg->Save();

	delete RTEngine;
	delete g_phEngine;
	delete g_cfg;
	delete Receipts;
	delete PRNReceipts;
	delete g_dbEngine;
	delete [] Alarms;
	delete prnFormatter;
	delete [] manualInfo;
	delete boothDisplay;
	delete [] m_pDisplayInfos;
	delete errorMemory;
	// never try to delete by hand g_dbEngine, DPrinter, View because
	// they must be deleted for the event or window manager !!!
#ifdef DOSX286
	DosSetExceptionHandler(0x0D, OldGPFHandler, NULL);
#endif
}

//
// here it's better to sacrifice the style instead of causing an overhead to the
// "Event Manager", this the reason because I prefer a pointer to the device
// rather than sending the characters to print into the message queue. GCC/gcc
// Sorry Mr. Zinc.
//
void CONTROLLER::Poll(void)
{
    if (state != D_ON)
		return;

    static UI_TIME lastTime, time;
    static SHORT hour, min, sec, hundreth;
    static SHORT lastHour, lastMin, lastSec, lastHundreth;
    time.Import();
    time.Export((int *)&hour, (int *)&min, (int *)&sec, (int *)&hundreth);
	lastTime.Export((int *)&lastHour, (int *)&lastMin, (int *)&lastSec, (int *)&lastHundreth);
	if (g_cfg->MANUAL && CashReq) // first because this processes hundreths
		activateCash(lastTime, lastHundreth, hundreth);

	// Raise resolution of View data 2.21.1 build 4
	// Parameterize on 2.30.1 build 13
	if (lastHundreth != hundreth)
	{
		static int s_refreshTime = 0;
		if (s_refreshTime >= g_cfg->VIEW_REFRESH_TIME)
		{
			s_refreshTime = 0;
			CookViewData();
		}
		else
		{
			s_refreshTime += 100;
		}
	}

	if (lastSec != sec)
	{
		// high priority tasks !!!
		UnloadRTEngine();
		cookReceipts();
		ProcessReceipts();

		// mark engine time
		int intTime;
		lastTime.Import();
		lastTime.Export(&intTime);
		RTEngine->SetCurrentTime(intTime);

		if (!g_cfg->IsDemoMode())
		{
			// recover info
			g_STM2->put(STM2::EXTENSIONRECEIPTNUMBER, &g_cfg->E_N_RECEIPT);
			g_STM2->put(STM2::RECEIPTNUMBER, &g_cfg->N_RECEIPT);

			if (!(sec%5))
			{
				static BoothCluster s_clusters[MAX_CLUSTER];
				RTEngine->GetClusters(s_clusters);
				g_STM2->put(STM2::BOOTHCLUSTERS, s_clusters);

				int date = RTEngine->GetCurrentDate();
				g_STM2->put(STM2::DATE, &date);

				int time = RTEngine->GetCurrentTime();
				g_STM2->put(STM2::TIME, &time);

				g_dbEngine->SetErrors(g_cfg->N_DIAL_ERR, g_cfg->N_COM_ERR);
			}
		}
		ProcessPrepaid(); // 2.21.8 build 16

		UpdateStatusBar(); // give a chance for the status bar

		if (g_cfg->E_FIRST_EXT)
            ProcessExtensions();
		if (!g_ssaver->IsActive())
		{
            View->WStatBar->Event(UI_EVENT(UE_REFRESH, 0));
        }
        // mark engine date
        if (lastMin != min)
        {
            UI_DATE date;
			int intDate;
			date.Export(&intDate); // mark engine date
			RTEngine->SetCurrentDate(intDate);

			g_dbEngine->Flush();

			if (TraceInfo::s_bTest)
			{
				DosMemAvail(&TraceInfo::s_nAvailableRAM);
			}
        }
	}
}

void CONTROLLER::ProcessExtensions(void)
{
	WORD cNum, bNum;
    double available = 0;
    SHORT pulseFS, toneFS;
	DXS_CRITICAL_ENTRY::ONLINE_ENTRY *onlineEntry;
	DXS_NON_CRITICAL_ENTRY *entry;
	for (SHORT i = g_cfg->E_FIRST_EXT-1; i < g_cfg->ACTIVE_CLUSTERS*CLUSTER_SIZE; i++)
	{
		cNum = i/CLUSTER_SIZE;
		bNum = i%CLUSTER_SIZE;
		toneFS  = RTEngine->GetToneFS(cNum, bNum);
		pulseFS = RTEngine->GetPulseFS(cNum, bNum);
		if
		(
			(toneFS > RT_ENGINE::OFFHOOK && toneFS != RT_ENGINE::NOPHONE) ||
			pulseFS > RT_ENGINE::OFFHOOK
		)
		{
			if (!(g_cfg->BoothInfo[i].Attr & CFG::ACTIVE_EXT))
			{
				RTEngine->SetToneFS(cNum, bNum, RT_ENGINE::LOCK);
				if (RTEngine->GetSimula(cNum, bNum))
				{
					RTEngine->SetSimula(cNum, bNum, FALSE);
					if (!RTEngine->IsAvailable(cNum))
					{
						RTEngine->SetToneFS(cNum, bNum, RT_ENGINE::NOPHONE);
					}
				}
				else
				{
					RTEngine->SetPulseFS(cNum, bNum, RT_ENGINE::LOCK);
				}
			}
			else
			{
				onlineEntry = &g_dbEngine->ExtGetCritical()->Online[i];
				entry = g_dbEngine->ExtGetNonCriticalEntry(i);
				for (SHORT j=0; j < 3; j++)
                {
					available += entry->Credits[j].Value;
                    available += entry->Debits[j].Value;
                    available -= entry->Others[j].Value;
                }
                available -= (onlineEntry->DDN.Cost + onlineEntry->DDI.Cost);
				available -= (g_cfg->E_INSTALL_COST + g_cfg->E_LINE_COST);
				available += ((g_cfg->E_DISCOUNT/100)*(onlineEntry->DDN.Cost + onlineEntry->DDI.Cost));
                //
				if (available < g_cfg->E_MIN_AVAILABLE)
                {
					RTEngine->SetToneFS(cNum, bNum, RT_ENGINE::LOCK);
					if (RTEngine->GetSimula(cNum, bNum))
                    {
						RTEngine->SetSimula(cNum, bNum, FALSE);
						if (!RTEngine->IsAvailable(cNum))
						{
							RTEngine->SetToneFS(cNum, bNum, RT_ENGINE::NOPHONE);
						}
                    }
					else
					{
						RTEngine->SetPulseFS(cNum, bNum, RT_ENGINE::LOCK);
					}
                }
			}
        }
    }
}

void CONTROLLER::UnloadRTEngine(void)
{
    // pass to local buffer
	DynamicReceipt dynReceipt, tmpDynReceipt;

	while (RTEngine->GetReceipt(dynReceipt))
	{
		if (!Receipts->Put(dynReceipt))
		{ // try
			Receipts->Get(tmpDynReceipt);   // bye to the oldest
			Receipts->Put(dynReceipt);
		}
	}
}

void CONTROLLER::cookReceipts(void)
{
	DynamicReceipt dynReceipt;
    UINT n = Receipts->GetCount();
    for (int i = 0; i < n; i++)
    {
		if (Receipts->Get(dynReceipt))
        {
			if (!dynReceipt.m_r.Stat.Cooked)
				CookReceipt(dynReceipt);
			Receipts->Put(dynReceipt);
		}
    }
}

void CONTROLLER::ProcessReceipts(void)
{
	DynamicReceipt dynReceipt, tmpDynReceipt;
	for (int i = 0; i < g_cfg->ACTIVE_CLUSTERS*CLUSTER_SIZE; i++)
	{
		// process dynReceipt queue
		if (Receipts->Get(dynReceipt))
		{
			// print
			if (!dynReceipt.m_r.Stat.Printed && dynReceipt.Attr_.Printable)
			{
				if (dynReceipt.m_r.Stat.Extension)
				{
					// not printed but the flag is set !!!
				}
				else
				{
					if (!PRNReceipts->Put(dynReceipt))
					{ // try
						PRNReceipts->Get(tmpDynReceipt); // bye to the oldest one
						// begin 2.21.8
						if (!g_dbEngine->Update(tmpDynReceipt))
						{
							UI_WINDOW_OBJECT::errorSystem->ReportError(
								UI_WINDOW_OBJECT::windowManager, WOS_NO_STATUS,
								"Recibo no actualizado (Despues PRNReceipts->Get())!\r\n");
						}
						// end 2.21.8
						PRNReceipts->Put(dynReceipt);
					}
				}
				dynReceipt.m_r.Stat.Printed = TRUE;
			}
			// archive
			if (!dynReceipt.m_r.Stat.Archived)
			{
				dynReceipt.m_r.Stat.Archived = TRUE;
				dynReceipt.m_r.Stat.Archived = g_dbEngine->Add(dynReceipt); // 2.21.8
			}

			// check to see if the record attributtes needs to be updated
			if (dynReceipt.m_r.Stat.Printed && dynReceipt.m_r.Stat.Archived)
			{
				// begin 2.21.8
				dynReceipt.m_r.extendedStat.nonProcessed = FALSE;
				if (dynReceipt.bFromTurn)
					dynReceipt.m_r.extendedStat.nonProcessed = !g_dbEngine->Update(dynReceipt);

				if (dynReceipt.m_r.extendedStat.nonProcessed)
				{
					if (TraceInfo::s_bTest)
					{
						 STR128 szMsg;
						 strcpy(szMsg, "Recibo no actualizado en ");
						 strcat(szMsg, __FILE__);
						 strcat(szMsg, "(");
						 STR16 szLine;
						 strcat(szMsg, itoa(__LINE__, szLine, 10));
						 strcat(szMsg, ")\r\n");

						 UI_WINDOW_OBJECT::errorSystem->ReportError(UI_WINDOW_OBJECT::windowManager,
							 WOS_NO_STATUS, szMsg);
					}
					// later processing
					Receipts->Put(dynReceipt);
				}
				// end 2.21.8
			}
			else // later processing
				Receipts->Put(dynReceipt);
		}
		// --- process printer queue
		if (PRNReceipts->Get2(dynReceipt))
		{ // try
			if (PrintReceipt(dynReceipt))
                PRNReceipts->Get(dynReceipt);
		}
	}
}

void CONTROLLER::UpdateStatusBar(void)
{
    static char msg[0x40];
	if (RTEngine->GetBadInterBooth() != -1)
	{
		sprintf(msg, "Llamada internacional en cabina: %s", g_cfg->BoothInfo[RTEngine->GetBadInterBooth()].Name);
		View->WStatBar->setMsg(msg, WHITE, LIGHTMAGENTA);
		RTEngine->SetBadInterBooth(-1); // reset !!!
	}
	else if (RTEngine->GetComErrBooth() != -1 && !View->WStatBar->PendingMsg())
	{
		sprintf(msg, "%d errores de comunicación en cabina: %s", g_cfg->MAX_COM_ERR, g_cfg->BoothInfo[RTEngine->GetComErrBooth()].Name);
		View->WStatBar->setMsg(msg, WHITE, LIGHTRED);
		RTEngine->SetComErrBooth(-1); // reset !!!
	}
	else if (RTEngine->GetDialErrBooth() != -1 && !View->WStatBar->PendingMsg())
	{
		sprintf(msg, "%d errores de marcación en cabina: %s", g_cfg->MAX_DIAL_ERR, g_cfg->BoothInfo[RTEngine->GetDialErrBooth()].Name);
		View->WStatBar->setMsg(msg, WHITE, LIGHTRED);
		RTEngine->SetDialErrBooth(-1); // reset !!!
	}
	else if (RTEngine->GetNotIncBooth() != -1 && !View->WStatBar->PendingMsg())
	{
		sprintf(msg, "Localidad no incluída en cabina: %s", g_cfg->BoothInfo[RTEngine->GetNotIncBooth()].Name);
		View->WStatBar->setMsg(msg, WHITE, LIGHTRED);
		RTEngine->SetNotIncBooth(-1); // reset !!!
	}
	else if (watchDog && !View->WStatBar->PendingMsg())
	{
		View->WStatBar->setMsg(watchDogMessage, WHITE, LIGHTRED);
		watchDog = FALSE; // reset !!!
	}
}

#ifdef DOSX286
PEHANDLER CONTROLLER::OldGPFHandler = NULL;
BOOL _exceptionIsOn = FALSE;

void interrupt far CONTROLLER::NewGPFHandler(EXCEP_FRAME eFrame)
{
	_exceptionIsOn = TRUE;

	if (!g_cfg->IsDemoMode() && TraceInfo::s_bTest)
	{
		Dump();
	}

	delete RTEngine;
	delete g_dbEngine;

	UI_DISPLAY *display = UI_WINDOW_OBJECT::display;
	delete UI_WINDOW_OBJECT::windowManager;
	delete display;
	delete Receipts;
	delete PRNReceipts;
	//
	DosSetExceptionHandler(0x0D, OldGPFHandler, NULL);
	cout.setf(ios::uppercase|ios::hex);
	cout
		<< "EXCEPCION ATRAPADA" << endl
		<< "------------------" << endl
		<< "C¢digo: " << eFrame.error_code << ' '
	;
	const char *msg = "Error en acceso a memoria general";
	switch (eFrame.error_code)
	{
	case 0xB0B0:
		msg = "Desbordamiento en ISR";
		break;
	case 0xC0C0:
		msg = "Contenci¢n en cola de recibos";
		break;
	case 0xC0D0:
		msg = "Desbordamiento del CFG";
		break;
	}
	cout
		<< '(' << msg << ')' << endl << endl
		<< "(Si desea intente con el comando:  \\st\\st <Enter>)" << endl
	;
	cout
		<< endl
		<< "Por favor comunicarse con Microdise¤o Ltda." << endl
		<< "Tels: (4) 341-5600" << endl
		<< " Fax: (4) 341-4629 Medell¡n Col." << endl
		<< endl
		<< "Ofrecemos disculpas por este molesto suceso." << endl
		<< "Gracias." << endl
		<< endl
		<< APP_VER_NAME  << endl
		<< APP_COPYRIGHT << endl
		<< "Porque el usuario es lo importante..." << endl
	;

	if (!TraceInfo::s_bTest)
		exit(1);
}


void CONTROLLER::Dump()
{
	/////////////////////////////////////////////////////////////////
	// Dump vital information
	FILE_NAME szFilename;

	WORD year, month, day, hour, minutes;
	_GetSysTime(hour, minutes);
	_GetSysDate(year, month, day);

	sprintf(szFilename, "%02d%02d%02d%02d.x", month, day, hour, minutes);
	_PrefixAppPath(szFilename);

	ofstream file(szFilename, ios::out|ios::binary);

	void 	*buffer;
	int   	size;
	STR32 	strLabel;
	STR16 	strSize;

	// RTEngine
	RTEngine->GetDumpData((void *)buffer, size);
	memset(strLabel, 0, 32);
	strcpy(strLabel, "RTEngine: "); strcat(strLabel, itoa(size, strSize, 10));
	file.write(strLabel, 32);
	file.write((char *)&size, sizeof(size));
	file.write((char *)buffer, size);

	// view
	View->GetDumpData((void *)buffer, size);
	memset(strLabel, 0, 32);
	strcpy(strLabel, "View: "); strcat(strLabel, itoa(size, strSize, 10));
	file.write(strLabel, 32);
	file.write((char *)&size, sizeof(size));
	file.write((char *)buffer, size);

	// stm2
	memset(strLabel, 0, 32);
	strcpy(strLabel, "STM2: "); strcat(strLabel, ultoa(STM2_BANKSIZE, strSize, 10));
	file.write(strLabel, 32);
	file.write((char *)&size, sizeof(size));
	buffer = new char[STM2_BANKSIZE/16]; // needs buffer
	for (int i=0; i<16; ++i)
	{
		g_STM2->GetDumpData(i*(STM2_BANKSIZE/16), buffer, STM2_BANKSIZE/16);
		file.write((char *)buffer, STM2_BANKSIZE/16);
	}
	delete [] buffer;
	buffer = NULL;
}

void CONTROLLER::ReplaceDump()
{
	FILE_NAME szFilename;

	/////////////////////////////////////////////////////////////////
	// test variable
	char *pszFilename = getenv("STDUMPFILE");
	if (!pszFilename)
	{
		return ;
	}
	strcpy(szFilename, pszFilename);
	_PrefixAppPath(szFilename);

	ifstream file(szFilename, ios::in|ios::binary);
	if (!file)
	{
		return ;
	}

	char 	*buffer = new char[STM2_BANKSIZE/4]; // needs buffer
	int   	size;
	STR32 	strLabel;

	// RTEngine
	file.read(strLabel, 32);
	file.read((char *)&size, sizeof(size));
	file.read((char *)buffer, size);
	RTEngine->SetDumpData(buffer, size);

	// view
	file.read(strLabel, 32);
	file.read((char *)&size, sizeof(size));
	file.read((char *)buffer, size);
	View->SetDumpData(buffer, size);

	// stm2
	file.read(strLabel, 32);
	file.read((char *)&size, sizeof(size));
	for (int i=0; i<4; ++i)
	{
		file.read((char *)buffer, STM2_BANKSIZE/4);
		g_STM2->SetDumpData(i*(STM2_BANKSIZE/4), buffer, STM2_BANKSIZE/4);
	}

	delete [] buffer;
	buffer = NULL;
}


// Free-store exception handler.
void CONTROLLER::NewHandler(void)
{
	// Free enouph memory for reporting error.
	if (errorMemory)
	{
		delete errorMemory;
		errorMemory = NULL;
		// Report out of memory error.
		UI_WINDOW_OBJECT::errorSystem->Beep();
		UI_WINDOW_OBJECT::errorSystem->ReportError(UI_WINDOW_OBJECT::windowManager,
                WOS_NO_STATUS, "No hay memoria disponible!\r\n""De salida hacia DOS.");
    }
	delete RTEngine;
    UI_DISPLAY *display = UI_WINDOW_OBJECT::display;
    delete UI_WINDOW_OBJECT::windowManager;
    delete display;
    delete Receipts;
    delete PRNReceipts;
    //
    DosSetExceptionHandler(0x0D, OldGPFHandler, NULL);
    exit(1);
}

#endif


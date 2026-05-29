//
// [ ST.CPP ]
//

//	 Date: 13-07-1993 !!!

#include "stdst.h"
#include "conio.h"

#define USE_RAW_KEYS

#include <ui_win.hpp>
#include <control.h>
#include <info.h>

#if defined(__DEMO__)
#include <dongle.h>
#endif

#include <eeprom.h>

#define USE_HELP_CONTEXTS
#include <res.hpp>
#include <help.hpp>

char  *g_SHORT_APP_ENG  = "GCC";
char  *g_LONG_APP_ENG   = "Gonzalo Contento Casta¤o";

extern CFG  *g_cfg;

// general app info
char *g_SHORT_APP_NAME 	= "ST";
char *g_LONG_APP_NAME  	= "SmartTar";

APP_INFO g_appInfo;

#if !defined(__TEST__)
#include <stm2.h>
STM2 *g_STM2  = NULL;
#endif

static void Prolog(void);
static void clean(UI_DISPLAY *display, UI_EVENT_MANAGER *eventManager, UI_WINDOW_MANAGER *windowManager);

static BOOL cancelBadShutDown(void);
static void logSTM2(char c);
//
EVENT_TYPE Exit(UI_DISPLAY *display, UI_EVENT_MANAGER *, UI_WINDOW_MANAGER *windowManager);

main(int , char *argv[])
{
	{
		Log log(Log::OUT|Log::CREATE);
		log.put(Log::NORMALSTART);
	}
#if !defined(__TEST__)
	g_STM2 = new STM2;
	g_STM2->login();
#endif
	//
	// --- To test "Out of memory"
	/*
    USHORT hungrySelector;
	if (DosAllocHuge(4*16, 0, &hungrySelector, 0, 0))
	{
		cerr << "Problems with DosAllocHuge ...\n";
		return (2);
	}
	*/
	// --- end of test
	//
	// Set up the storage and display search path.

	// Begin 2.21.8 Build 6
	UI_STORAGE::searchPath =  new UI_PATH(argv[0], TRUE);
	// End 2.21.8 Build 6

	//
	Prolog();
	UI_DISPLAY *display = new UI_GRAPHICS_DISPLAY;
	UI_EVENT_MANAGER *eventManager = new UI_EVENT_MANAGER(display);
	*eventManager
		+ new UID_KEYBOARD
		+ new UID_MOUSE
		+ new UID_CURSOR;

	UI_WINDOW_MANAGER *windowManager = new UI_WINDOW_MANAGER(display, eventManager, Exit);
	UID_KEYBOARD::breakHandlerSet = L_EXIT_FUNCTION;

	// Attach the error and help systems.
	UI_WINDOW_OBJECT::errorSystem = new UI_ERROR_SYSTEM;

	UI_WINDOW_OBJECT::defaultStorage = new UI_STORAGE("res.dat", UIS_READ);

	if (UI_WINDOW_OBJECT::defaultStorage->storageError)
	{
		// Begin 2.21.8 Build 6
		STR256 strMsg;
		g_GetErrnoStr(strMsg);
		strcat(strMsg, ":\n");
		strcat(strMsg, UI_STORAGE::searchPath->FirstPathName());
		strcat(strMsg, "\\RES.DAT.");
		// End 2.21.8 Build 6

		UI_WINDOW_OBJECT::errorSystem->ReportError(windowManager, WOS_NO_STATUS, strMsg);

		Log log(Log::OUT|Log::CREATE);
		log.put(Log::APPBADTRY);
		// Clean up.
		clean(display, eventManager, windowManager);
		return (1);
	}

	UI_WINDOW_OBJECT::helpSystem = new UI_HELP_SYSTEM("help.dat", windowManager, H_GENERAL);

#if !defined(__DEMO__)
	if (g_STM2->getStatus() == STM2::NONE)
	{
		UI_WINDOW_OBJECT::errorSystem->ReportError(windowManager,
				WOS_NO_STATUS, "\n\n      Acceso Negado\n");
		Log log(Log::OUT|Log::CREATE);
		log.put(Log::STM2BADTRY);
		// Clean up.
		clean(display, eventManager, windowManager);
		return (2);
	}
	// Activate EEPROM.  version 2.19b
	extern SUPER_APP_INFO g_superAppInfo;
	if (!g_superAppInfo.Attr.NoEEPROM)
	{
		EEPROM eeprom;
		if (!eeprom.isValidVersion())
		{
			UI_WINDOW_OBJECT::errorSystem->ReportError(windowManager,
					WOS_NO_STATUS, "\n\n      Versión no válida\n");
			// Clean up.
			Log log(Log::OUT|Log::CREATE);
			log.put(Log::EEPROMBADTRY);
			clean(display, eventManager, windowManager);
			return (2);
		}
	}
#else
#if !defined(__NO_DONGLE__)
DONGLE dongle;
	if (!dongle.isThere())
	{
		UI_WINDOW_OBJECT::errorSystem->ReportError(windowManager,
                WOS_NO_STATUS, "\n\n      Acceso Negado\n"
                                                  );
        Log log(Log::OUT|Log::CREATE);
		log.put(Log::DONGLEBADTRY);
        // Clean up.
        clean(display, eventManager, windowManager);
        return (2);
    }
#endif
#endif // __DEMO__
    // attach the new devices ...
	CONTROLLER *controller = NULL;
	*eventManager
		+ (controller = new CONTROLLER(eventManager, windowManager))
	;
    if (!g_cfg->IsDemoMode() && g_cfg->GetStatus() != CFG::OK)
    {
        UI_WINDOW_OBJECT::errorSystem->ReportError(windowManager, WOS_NO_STATUS,
                "\n\n""      Acceso Negado\n"
                "Configuración no disponible.\n"
                " Deberá instalar el sistema."
                                                  );
		Log log(Log::OUT|Log::CREATE);
        log.put(Log::CFGBADTRY);
        // Clean up.
        clean(display, eventManager, windowManager);
        return (2);
    }
    UI_KEY key;
    key.shiftState = S_ALT;
    key.value = ALT_F10 >> 8;
    eventManager->Put(UI_EVENT(E_KEY, ALT_F10, key)); // maximize
    UI_EVENT event;
	// Process user events.
    EVENT_TYPE ccode;
    do
    {
        if (!eventManager->Get(event, Q_NO_BLOCK|Q_BEGIN|Q_DESTROY|Q_POLL))
            ccode = windowManager->Event(event);
        else
        {
			controller->RefreshView();
            controller->RefreshBoothDisplay();
#if defined(__DEMO__)
#if !defined(__NO_DONGLE__)
            static UI_TIME __ULastTime, __UCurTime;
            static int __CurHour, __CurMin, __CurSec;
            static int __LastHour, __LastMin, __LastSec;
            __UCurTime.Import();
            __UCurTime.Export(&__CurHour, &__CurMin, &__CurSec);
            __ULastTime.Export(&__LastHour, &__LastMin, &__LastSec);
            if (__CurMin != __LastMin)
            {
                __ULastTime.Import();
                if (!dongle.isThere())
                {
                    UI_WINDOW_OBJECT::errorSystem->ReportError(windowManager,
                            WOS_NO_STATUS, "\n\n      Acceso Negado\n");
                    // Clean up.
                    clean(display, eventManager, windowManager);
                    return (2);
				}
            }
#endif
#endif
        }
    }
    while (ccode != L_EXIT && ccode != S_NO_OBJECT);
    // Clean up.
    clean(display, eventManager, windowManager);
	if (TraceInfo::s_bTest)
    {
		cout << "Waiting " << TraceInfo::s_nSemWait << " Times ..." << endl;
		cout << "Available RAM after loadding: " << TraceInfo::s_nAvailableRAM << endl;
		cout << "Pass " << TraceInfo::s_nPass << endl;
    }
    {
        Log log(Log::OUT|Log::CREATE);
        log.put(Log::NORMALSHUTDOWN);
    }
    //
	return (0);
}

void clean(UI_DISPLAY *display, UI_EVENT_MANAGER *eventManager, UI_WINDOW_MANAGER *windowManager)
{
    delete UI_WINDOW_OBJECT::helpSystem;
    delete UI_WINDOW_OBJECT::errorSystem;
    delete UI_WINDOW_OBJECT::defaultStorage;
    delete windowManager;
    delete eventManager;
    delete display;
#if !defined(__TEST__)
	g_STM2->logout();
	delete g_STM2;
#endif
}

// user exit function ...
EVENT_TYPE Exit(UI_DISPLAY *display, UI_EVENT_MANAGER *, UI_WINDOW_MANAGER *windowManager)
{
	int width = 57, height = 8;
    int left = (display->columns / display->cellWidth - width) / 2;
    int top = (display->lines / display->cellHeight - height) / 2;
    UIW_BUTTON *wNoButton;
    // Create the exit Window.
    UIW_WINDOW *window = new UIW_WINDOW(left, top, width, height,
                                        WOF_NO_FLAGS, WOAF_NORMAL_HOT_KEYS|WOAF_MODAL|WOAF_DIALOG_OBJECT|WOAF_NO_SIZE);
    window->helpContext = H_QUIT;
    *window
		+ new UIW_BORDER
		+ new UIW_SYSTEM_BUTTON(SYF_GENERIC)
		+ new UIW_TITLE("SmartTar", WOF_JUSTIFY_CENTER)
	;
	// check to see if it's possible to go out
	BOOL isDemo    = CONTROLLER::RTEngineIsDemo();
	BOOL sysIsBusy = !isDemo && CONTROLLER::RTEngineIsBusy();
	if (sysIsBusy)
	{
		*window
		+ new UIW_BUTTON( 3, 3, 9, "", BTF_NO_3D|BTF_AUTO_SIZE|BTF_STATIC_BITMAPARRAY, WOF_NON_SELECTABLE, NULL, 0, "ST")
		+ new UIW_PROMPT(15, 1, "El sistema está procesando información.")
		+ new UIW_PROMPT(15, 2, "Por favor verifique todas las cabinas.")
		+ new UIW_BUTTON(19, 5, 15, "~Aceptar", BTF_NO_TOGGLE|BTF_AUTO_SIZE|BTF_SEND_MESSAGE|BTF_STATIC_BITMAPARRAY, WOF_JUSTIFY_CENTER, NULL, S_CLOSE)
		;
	}
	else if (isDemo)
	{
		*window
		+ new UIW_BUTTON( 3, 3, 9, "", BTF_NO_3D|BTF_AUTO_SIZE|BTF_STATIC_BITMAPARRAY, WOF_NON_SELECTABLE, NULL, 0, "ST")
		+ new UIW_PROMPT(15, 2, "Detener la simulación y salir ?")
		+ new UIW_BUTTON(14, 5, 11, "~Si", BTF_NO_TOGGLE|BTF_AUTO_SIZE|BTF_SEND_MESSAGE|BTF_STATIC_BITMAPARRAY, WOF_JUSTIFY_CENTER, NULL, L_EXIT)
		+ ( wNoButton =
				new UIW_BUTTON(31, 5, 11, "~No", BTF_NO_TOGGLE|BTF_AUTO_SIZE|BTF_SEND_MESSAGE|BTF_STATIC_BITMAPARRAY, WOF_JUSTIFY_CENTER, NULL, S_CLOSE)
		  )
		;
		wNoButton->woStatus |= WOS_CURRENT;
	}
	else
	{
		*window
		+ new UIW_BUTTON( 3, 3, 9, "", BTF_NO_3D|BTF_AUTO_SIZE|BTF_STATIC_BITMAPARRAY, WOF_NON_SELECTABLE, NULL, 0, "ST")
		+ new UIW_PROMPT(15, 2, "Terminar la sesión de trabajo ?")
		+ new UIW_BUTTON(14, 5, 11, "~Si", BTF_NO_TOGGLE|BTF_AUTO_SIZE|BTF_SEND_MESSAGE|BTF_STATIC_BITMAPARRAY, WOF_JUSTIFY_CENTER, NULL, L_EXIT)
		+ ( wNoButton =
				new UIW_BUTTON(31, 5, 11, "~No", BTF_NO_TOGGLE|BTF_AUTO_SIZE|BTF_SEND_MESSAGE|BTF_STATIC_BITMAPARRAY, WOF_JUSTIFY_CENTER, NULL, S_CLOSE)
		  )
		;
		wNoButton->woStatus |= WOS_CURRENT;
	}

	*windowManager + window;
	windowManager->Center(window);

	return (S_CONTINUE);
}

void Prolog(void)
{
	extern SUPER_APP_INFO g_superAppInfo;
	g_appInfo = g_superAppInfo.Data;
	if (TraceInfo::s_bDevelopment)
		g_superAppInfo.Attr.STPro = TRUE;
	if (g_cfg->IsDemoMode())
		g_superAppInfo.Attr.STPro = TRUE;
	if (g_superAppInfo.Attr.Serialized)
		_Decrypt(&g_appInfo, sizeof(APP_INFO));
	// I love colors. GCC/gcc.
	UI_DISPLAY::backgroundPalette->fillPattern     = PTN_SOLID_FILL;
	UI_DISPLAY::backgroundPalette->colorBackground = GREEN;
	if (!g_cfg->IsDemoMode())
	{
	WORD status = g_STM2->getStatus();
	if (status != STM2::NONE)
	{
		cout
			<< APP_VER_NAME  << endl
			<< APP_COPYRIGHT << endl
			<< "  Serial: " << g_appInfo.Serial << endl
		;
		Log log(Log::OUT|Log::CREATE);
		switch (status)
		{
		case STM2::GARBAGE:
			log.put(Log::STM2GARBAGE);
			logSTM2('*');
			g_STM2->forceOk();
			break;
		case STM2::BAD_SHUTDOWN:
			int date, time;
			g_STM2->get(STM2::DATE, &date);
			g_STM2->get(STM2::TIME, &time);
			log.put(date, time, Log::STM2BADSHUTDOWN);
			if (cancelBadShutDown())
			{
				log.put(Log::STM2IGNORERECOVER);
				g_STM2->forceOk();
			}
			else
			{
				log.put(Log::STM2RECOVER);
			}
			break;
		}
	}
	}
}

BOOL cancelBadShutDown(void)
{
	BOOL ok = TRUE;
	BOOL cancel = FALSE;
	UI_DATE date;
	UI_TIME time;
	int  seconds = 0;
	int  lastss = 0;
	do
	{
		if (kbhit())
		{
			// based on upgrade number assign key
			cancel = (getch() == 'a'+APP_UPGRADE_VER);
			ok = !cancel;
		}
		else
		{
			// count five seconds
			time.Import();
			int hh, mm, ss;
			time.Export(&hh, &mm, &ss);
			if (ss != lastss)
			{
				lastss = ss;
				seconds++;
				cout << '+';
			}
			if (seconds == 5)
				ok = FALSE;
		}
	}
	while (ok);
	cout << endl;
	//
	logSTM2(cancel?'-':'+');
	//
	return cancel;
}

void logSTM2(char c)
{
	UI_DATE date;
	UI_TIME time;
	int packedDate;
	g_STM2->get(STM2::DATE, &packedDate);
	if (date.Import(packedDate) != DTI_OK)
		date.Import();
	char downDateStr[0x20], upDateStr[0x20];
	date.Export(downDateStr, DTF_EUROPEAN_FORMAT);
	date.Import();
	date.Export(upDateStr, DTF_EUROPEAN_FORMAT);
	int packedTime;
	g_STM2->get(STM2::TIME, &packedTime);
	if (time.Import(packedTime) != TMI_OK)
		time.Import();
	char downTimeStr[0x10], upTimeStr[0x10];
	time.Export(downTimeStr, TMF_TWENTY_FOUR_HOUR|TMF_ZERO_FILL);
	time.Import();
	time.Export(upTimeStr, TMF_TWENTY_FOUR_HOUR|TMF_ZERO_FILL);
	//
	FILE_NAME filename;
	strcat(_GetAppPath(filename), "ST.LOG");
	chmod(filename, S_IREAD|S_IWRITE);
	ofstream logFile(filename, ios::out|ios::app);
	logFile
		<< "[" << c << "] "
		<< downDateStr << ' ' << downTimeStr << " -> "
		<< upDateStr << ' ' << upTimeStr
		<< endl;
	;
	logFile.close();
	chmod(filename, S_IREAD);
}


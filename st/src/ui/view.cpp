//
// [ VIEW.CPP ]
//

#include "stdst.h"

#include <rt_eng.h>
#include <view.h>
#include <events.h>
#include <w_table.h>
#include <hb_ids.h>
#include <info.h>
#include <b_button.h>
#include <control.h>  // 2.50 -- CONTROLLER::RTEngineIsDemo() demo gate

#ifndef USE_HELP_CONTEXTS
#define USE_HELP_CONTEXTS
#include <help.hpp>
#endif

#define _MENU_SEPARATOR {""  , MNIF_SEPARATOR}

#define  W_GAP          2
#define  VIEW_WIDTH     91
#define  VIEW_HEIGHT    20
#define  VISIBLE_BOOTHS 16
#define  WOF_DATA       (WOF_BORDER|WOF_MINICELL|WOF_NON_SELECTABLE)

extern APP_INFO       	g_appInfo;
extern SUPER_APP_INFO 	g_superAppInfo;
extern CFG            	*g_cfg;

// ---------------------------------------------------------------------------
//								UIW_VIEW
// ---------------------------------------------------------------------------

UIW_VIEW::UIW_VIEW(int numOfClusters, int numOfGroups)
        : UIW_WINDOW(0, 0, 0, 0),
		NumOfClusters(numOfClusters),
        NumOfGroups(numOfGroups),
        NumOfBooths(numOfClusters*CLUSTER_SIZE)
{
    loadStatBMP();

	m_callInfo = new PH_ENGINE::CallInfo[NumOfClusters][CLUSTER_SIZE];

    STR256 title;
	sprintf(title, "%s - %s %s", g_appInfo.Title, g_cfg->COMPANY, g_cfg->CITY);
    // add the other objects to the Window ...
    *this
		+ new UIW_BORDER
		+ new UIW_MAXIMIZE_BUTTON
		+ new UIW_MINIMIZE_BUTTON
		+ new UIW_SYSTEM_BUTTON(SYF_GENERIC)
		+ new UIW_TITLE(title, WOF_JUSTIFY_CENTER)
		+ new UIW_ICON(10, 10,"ST", "SmartTar", ICF_MINIMIZE_OBJECT)
	;

	REGION *groupRegions = new REGION[NumOfGroups];
	initPos(groupRegions);

	addStatBar();
	addMenu();
	addToolbar();
	addTitleGroups(groupRegions);
	addTable(groupRegions);

	delete [] groupRegions;
}

UIW_VIEW::~UIW_VIEW()
{
	delete [] m_callInfo;
}

void UIW_VIEW::initPos(REGION *groupRegions)
{
	groupRegions[0].W = 60;
	groupRegions[1].W = 100;
	groupRegions[2].W = 60;
	groupRegions[3].W = (sizeof(PHONE)-1)*10;
	groupRegions[4].W = (sizeof(CITY_NAME)-1)*10;
	groupRegions[5].W = 70;
	groupRegions[6].W = 50;
	groupRegions[7].W = 130;
	groupRegions[8].W = 80;
	for (int i=0; i<NumOfGroups; i++)
	{
		groupRegions[i].X = (!i) ? 0: groupRegions[i-1].X + groupRegions[i-1].W - 1;
		groupRegions[i].Y = GBUTTON_HEIGHT+W_GAP;
	}
}

void UIW_VIEW::loadStatBMP(void)
{
	if (defaultStorage && !defaultStorage->storageError)
	{
		defaultStorage->ChDir("~UI_BITMAP");
		short bmpWidth, bmpHeight;
		for (int i=0; i<RT_ENGINE::END; i++)
        {
            UI_STORAGE_OBJECT bmpFile(*defaultStorage, States[i].BitmapName, ID_BITMAP_IMAGE, UIS_READ);
            if (!bmpFile.objectError)
            {
                bmpFile.Load(&bmpWidth);
                bmpFile.Load(&bmpHeight);
                States[i].Bitmap = new UCHAR[bmpWidth*bmpHeight];
                bmpFile.Load(States[i].Bitmap, sizeof(UCHAR), bmpWidth*bmpHeight);
            }
        }
    }
}

void UIW_VIEW::addStatBar(void)
{
    WStatBar = new UIW_STAT_BAR;
    UIW_STAT_BAR::HelpInfo = HelpInfo;
    *this + WStatBar;
}

void UIW_VIEW::addMenu(void)
{
    WFileMenu = new UIW_PULL_DOWN_ITEM("&Archivo");
    *WFileMenu
    + new UIW_POP_UP_ITEM("&Vaciar bases de datos", MNIF_SEND_MESSAGE, BTF_NO_3D, WOF_NO_FLAGS, NULL, UE_FLUSH_ALL)
    + new UIW_POP_UP_ITEM("&Reconstruir bases de datos", MNIF_SEND_MESSAGE, BTF_NO_3D, WOF_NO_FLAGS, NULL, UE_REBUILD_ALL)
    + new UIW_POP_UP_ITEM("", MNIF_SEPARATOR)
    + new UIW_POP_UP_ITEM("&Salir"                , MNIF_SEND_MESSAGE, BTF_NO_3D, WOF_NO_FLAGS, NULL, S_CLOSE)
    ;
    WFileMenu->helpContext = H_FILE_MENU;
    //
    WConfigMenu= new UIW_PULL_DOWN_ITEM("&Configuración");
	*WConfigMenu
    + new UIW_POP_UP_ITEM("&Hora y fecha actual"       ,  MNIF_SEND_MESSAGE, BTF_NO_3D, WOF_NO_FLAGS, NULL, UE_TIME_DATE       )
    + new UIW_POP_UP_ITEM("&Dias festivos"             ,  MNIF_SEND_MESSAGE, BTF_NO_3D, WOF_NO_FLAGS, NULL, UE_SDAYS           )
    + new UIW_POP_UP_ITEM("", MNIF_SEPARATOR)
    + new UIW_POP_UP_ITEM("&Señal de contestación"     ,  MNIF_SEND_MESSAGE, BTF_NO_3D, WOF_NO_FLAGS, NULL, UE_SIGNAL          )
    + new UIW_POP_UP_ITEM("&Tiempos de operación"      ,  MNIF_SEND_MESSAGE, BTF_NO_3D, WOF_NO_FLAGS, NULL, UE_TIMES           )
    + new UIW_POP_UP_ITEM("&Bloqueo de numeración"     ,  MNIF_SEND_MESSAGE, BTF_NO_3D, WOF_NO_FLAGS, NULL, UE_LOCK_NUM        )
	+ new UIW_POP_UP_ITEM("", MNIF_SEPARATOR)
    + new UIW_POP_UP_ITEM("T&arifas nacionales"        , MNIF_SEND_MESSAGE, BTF_NO_3D, WOF_NO_FLAGS, NULL, UE_NAL_TAR          )
    + new UIW_POP_UP_ITEM("Tari&fas internacionales"   , MNIF_SEND_MESSAGE, BTF_NO_3D, WOF_NO_FLAGS, NULL, UE_INTER_TAR        )
    + new UIW_POP_UP_ITEM("&Nuevas localidades"        , MNIF_SEND_MESSAGE, BTF_NO_3D, WOF_NO_FLAGS, NULL, UE_NEW_CITY         )
    + new UIW_POP_UP_ITEM("N&uevos paises"             , MNIF_SEND_MESSAGE, BTF_NO_3D, WOF_NO_FLAGS, NULL, UE_NEW_COUNTRY      )
    + new UIW_POP_UP_ITEM("", MNIF_SEPARATOR)
    + new UIW_POP_UP_ITEM("&Identificadores de cabinas", MNIF_SEND_MESSAGE, BTF_NO_3D, WOF_NO_FLAGS, NULL, UE_ALIAS            )
    + new UIW_POP_UP_ITEM("&Redondeo total factura"    , MNIF_SEND_MESSAGE, BTF_NO_3D, WOF_NO_FLAGS, NULL, UE_ROUND            )
    + new UIW_POP_UP_ITEM("&Puerto de impresora"       , MNIF_SEND_MESSAGE, BTF_NO_3D, WOF_NO_FLAGS, NULL, UE_PPORT            )
    + new UIW_POP_UP_ITEM("Apertura de Ca&ja "         , MNIF_SEND_MESSAGE, BTF_NO_3D, WOF_NO_FLAGS, NULL, UE_CASH             )
    + new UIW_POP_UP_ITEM("Desplie&gue en cabinas"     , MNIF_SEND_MESSAGE, BTF_NO_3D, WOF_NO_FLAGS, NULL, UE_DISPLAY          )
    + new UIW_POP_UP_ITEM("", MNIF_SEPARATOR)
    + ( WChangePasswd = new UIW_POP_UP_ITEM(
							"&Cambiar código de acceso", MNIF_SEND_MESSAGE, BTF_NO_3D, WOF_NO_FLAGS, NULL, UE_CHANGE_PASSWD)
      )
	+ new UIW_POP_UP_ITEM("Si&mulacion del sistema\t(F2)"  , MNIF_SEND_MESSAGE, BTF_NO_3D, WOF_NO_FLAGS, NULL, UE_SIMULA           )
    + new UIW_POP_UP_ITEM("", MNIF_SEPARATOR)
    + new UIW_POP_UP_ITEM("Desacti&var este menú"    , MNIF_SEND_MESSAGE, BTF_NO_3D, WOF_NO_FLAGS, NULL, UE_DEACTIVATE_CONFIG)
    ;
    WConfigMenu->helpContext = H_CONFIG_MENU;
	//
    WInfoMenu = new UIW_PULL_DOWN_ITEM("I&nformación");
    *WInfoMenu
    + new UIW_POP_UP_ITEM("&Configuración del sistema"  , MNIF_SEND_MESSAGE, BTF_NO_3D, WOF_NO_FLAGS, NULL, UE_SYS_INFO       )
    + new UIW_POP_UP_ITEM("&Identificadores de cabinas" , MNIF_SEND_MESSAGE, BTF_NO_3D, WOF_NO_FLAGS, NULL, UE_I_ALIAS        )
    + new UIW_POP_UP_ITEM("I&dentificación del operario", MNIF_SEND_MESSAGE, BTF_NO_3D, WOF_NO_FLAGS, NULL, UE_OP_ID          )
    + new UIW_POP_UP_ITEM("", MNIF_SEPARATOR)
	+ new UIW_POP_UP_ITEM("Con&sulta rápida de tarifas\t(F10)", MNIF_SEND_MESSAGE, BTF_NO_3D, WOF_NO_FLAGS, NULL, UE_PHQUERY  ) // 2.50
	+ new UIW_POP_UP_ITEM("&Tarifas nacionales"         , MNIF_SEND_MESSAGE, BTF_NO_3D, WOF_NO_FLAGS, NULL, UE_I_NAL_TAR      )
    + new UIW_POP_UP_ITEM("Ta&rifas internacionales"    , MNIF_SEND_MESSAGE, BTF_NO_3D, WOF_NO_FLAGS, NULL, UE_I_INTER_TAR    )
    + new UIW_POP_UP_ITEM("", MNIF_SEPARATOR)
    + new UIW_POP_UP_ITEM("A&larmas del sistema"        , MNIF_SEND_MESSAGE, BTF_NO_3D, WOF_NO_FLAGS, NULL, UE_ALARM          )
    + new UIW_POP_UP_ITEM("", MNIF_SEPARATOR)
	+ (WActivateConfig =
           new UIW_POP_UP_ITEM("&Activar menú de configuración", MNIF_SEND_MESSAGE, BTF_NO_3D, WOF_NO_FLAGS, NULL, UE_ACTIVATE_CONFIG)
      )
    + (WActivateExt =
           new UIW_POP_UP_ITEM("Acti&var menú de extensiones", MNIF_SEND_MESSAGE, BTF_NO_3D, WOF_NO_FLAGS, NULL, UE_E_ACTIVATE)
      )
    ;
	if (CONTROLLER::RTEngineIsDemo())          // 2.50 -- demo-only operator control
		*WInfoMenu
		+ new UIW_POP_UP_ITEM("", MNIF_SEPARATOR)
		+ new UIW_POP_UP_ITEM("&Detener/Reanudar simulacion", MNIF_SEND_MESSAGE, BTF_NO_3D, WOF_NO_FLAGS, NULL, UE_DEMO_TOGGLE);
	WInfoMenu->helpContext = H_INFO_MENU;
    //
    WPrintMenu = new UIW_PULL_DOWN_ITEM("&Impresión");
    *WPrintMenu
    + new UIW_POP_UP_ITEM("&Acumulados de turno"     , MNIF_SEND_MESSAGE, BTF_NO_3D, WOF_NO_FLAGS, NULL, UE_ACCUM )
    + new UIW_POP_UP_ITEM("A&cumulados especiales"   , MNIF_SEND_MESSAGE, BTF_NO_3D, WOF_NO_FLAGS, NULL, UE_SACCUM)
    + new UIW_POP_UP_ITEM("", MNIF_SEPARATOR)
	+ new UIW_POP_UP_ITEM("&Recibos de turno\t(F9)", MNIF_SEND_MESSAGE, BTF_NO_3D, WOF_NO_FLAGS, NULL, UE_VIEW_TURN)
    + new UIW_POP_UP_ITEM("R&ecibos de otros turnos" , MNIF_SEND_MESSAGE, BTF_NO_3D, WOF_NO_FLAGS, NULL, UE_VIEW_OTHER_TURN)
    + new UIW_POP_UP_ITEM("", MNIF_SEPARATOR)
    + new UIW_POP_UP_ITEM("&Impresión zonal"         , MNIF_SEND_MESSAGE, BTF_NO_3D, WOF_NO_FLAGS, NULL, UE_ZPRINT)
    + new UIW_POP_UP_ITEM("Im&presión internacional" , MNIF_SEND_MESSAGE, BTF_NO_3D, WOF_NO_FLAGS, NULL, UE_IPRINT)
    + new UIW_POP_UP_ITEM("", MNIF_SEPARATOR)
	+ new UIW_POP_UP_ITEM("Ma&nejo de recibos\t(F6)" , MNIF_SEND_MESSAGE, BTF_NO_3D, WOF_NO_FLAGS, NULL, UE_ADM_REC)
    + new UIW_POP_UP_ITEM("&Mensaje de recibos"      , MNIF_SEND_MESSAGE, BTF_NO_3D, WOF_NO_FLAGS, NULL, UE_FOOTER)
    ;
    WPrintMenu->helpContext = H_PRINT_MENU;
    //
    WExtMenu = new UIW_PULL_DOWN_ITEM("&Extensiones");
    *WExtMenu
	+ new UIW_POP_UP_ITEM("&Estado de Cuenta", MNIF_SEND_MESSAGE, BTF_NO_3D, WOF_NO_FLAGS, NULL, UE_E_ACCOUNT)
    + new UIW_POP_UP_ITEM("&Acumulado", MNIF_SEND_MESSAGE, BTF_NO_3D, WOF_NO_FLAGS, NULL, UE_E_ACCUM)
    + new UIW_POP_UP_ITEM("", MNIF_SEPARATOR)
	+ new UIW_POP_UP_ITEM("&Parámetros", MNIF_SEND_MESSAGE, BTF_NO_3D, WOF_NO_FLAGS, NULL, UE_E_PARAMETERS)
	+ new UIW_POP_UP_ITEM("", MNIF_SEPARATOR)
	+ new UIW_POP_UP_ITEM("&Desactivar este menú", MNIF_SEND_MESSAGE, BTF_NO_3D, WOF_NO_FLAGS, NULL, UE_E_DEACTIVATE)
	;
	// WExtMenu->helpContext = H_EXTENSION_MENU;
	//
	WHelpMenu = new UIW_PULL_DOWN_ITEM("A&yudas");
	*WHelpMenu
	+ new UIW_POP_UP_ITEM("&Manejo del programa", MNIF_SEND_MESSAGE, BTF_NO_3D, WOF_NO_FLAGS, NULL, UE_PRG_MNG)
	+ new UIW_POP_UP_ITEM("", MNIF_SEPARATOR)
	+ new UIW_POP_UP_ITEM("&Acerca de SmartTar\t(Shift+F1)" , MNIF_SEND_MESSAGE, BTF_NO_3D, WOF_NO_FLAGS, NULL, UE_ABOUT  )
	;
	WHelpMenu->helpContext = H_HELP_MENU;
	WMenu = new UIW_PULL_DOWN_MENU(0);
	*WMenu
	+ WFileMenu
	+ WConfigMenu
	+ WInfoMenu
	+ WPrintMenu
	+ WExtMenu
	+ WHelpMenu
	;
	//
	*this + WMenu;
	WConfigMenu->woFlags |= WOF_NON_SELECTABLE;
	WExtMenu->woFlags |= WOF_NON_SELECTABLE;
	if (!g_superAppInfo.Attr.STPro)
		WActivateExt->woFlags |= WOF_NON_SELECTABLE;
	if (TraceInfo::s_bDevelopment)
    {
		WConfigMenu->woFlags  &= ~WOF_NON_SELECTABLE;
        WExtMenu->woFlags     &= ~WOF_NON_SELECTABLE;
        WActivateExt->woFlags &= ~WOF_NON_SELECTABLE;
    }
}

void UIW_VIEW::addToolbar(void)
{
    BTF_FLAGS tBTF_TBB = BTF_NO_TOGGLE|BTF_SEND_MESSAGE;
    WOF_FLAGS tWOF_TBB = WOF_MINICELL|WOF_JUSTIFY_CENTER;
    int toolbarWidth = 64;
    int microBmpWidth = toolbarWidth+20;
	UIW_TBUTTON *tools[0xB];
    WToolBar = new UIW_TOOL_BAR(0, 0, 400, 21, WNF_BITMAP_CHILDREN|WNF_NO_WRAP, WOF_MINICELL|WOF_BORDER|WOF_NON_FIELD_REGION);
    int buttonTop = 1;
    int toolbarLeft[0xC];
    toolbarLeft[0x0] = 10;
    toolbarLeft[0x1] = toolbarLeft[0x0] + 70;
    toolbarLeft[0x2] = toolbarLeft[0x1] + toolbarWidth;
    toolbarLeft[0x3] = toolbarLeft[0x2] + toolbarWidth + 10;
	toolbarLeft[0x4] = toolbarLeft[0x3] + toolbarWidth-1;
    toolbarLeft[0x5] = toolbarLeft[0x4] + toolbarWidth-1;
    toolbarLeft[0x6] = toolbarLeft[0x5] + toolbarWidth + 10;
    toolbarLeft[0x7] = toolbarLeft[0x6] + toolbarWidth-1;
    toolbarLeft[0x8] = toolbarLeft[0x7] + toolbarWidth-1;
    toolbarLeft[0x9] = toolbarLeft[0x8] + toolbarWidth-1;
    toolbarLeft[0xA] = toolbarLeft[0x9] + toolbarWidth + 10;
    toolbarLeft[0xB] = toolbarLeft[0xA] + toolbarWidth + 10;
    //
    BoothNumber = 1;
    char range[0x08], s[0x04];
    strcpy(range, "1..");
	strcat(range, itoa(g_cfg->ACTIVE_CLUSTERS*CLUSTER_SIZE, s, 10));
    *WToolBar
    +               new UIW_PROMPT (toolbarLeft[0x0]  , buttonTop  , 60, "Cabina", WOF_MINICELL)
    + (WBNumber   = new UIW_INTEGER(toolbarLeft[0x0]+6, buttonTop+8, 40, &BoothNumber, range, NMF_NO_FLAGS, WOF_AUTO_CLEAR|WOF_BORDER|WOF_MINICELL, processBooth))
    + (tools[0x0] = new UIW_TBUTTON(toolbarLeft[0x1], buttonTop, toolbarWidth , NULL, tBTF_TBB, tWOF_TBB, NULL, UE_SP_SERV  , "NotePhone"))
    + (tools[0x1] = new UIW_TBUTTON(toolbarLeft[0x2], buttonTop, toolbarWidth , NULL, tBTF_TBB, tWOF_TBB, NULL, UE_CALC     , "Calc"))
    + (tools[0x2] = new UIW_TBUTTON(toolbarLeft[0x3], buttonTop, toolbarWidth , NULL, tBTF_TBB, tWOF_TBB, NULL, UE_SPY      , "Microphone"))
    + (tools[0x3] = new UIW_TBUTTON(toolbarLeft[0x4], buttonTop, toolbarWidth , NULL, tBTF_TBB, tWOF_TBB, NULL, UE_INTER    , "World"))
	+ (tools[0x4] = new UIW_TBUTTON(toolbarLeft[0x5], buttonTop, toolbarWidth , NULL, tBTF_TBB, tWOF_TBB, NULL, UE_LOCK     , "NoSolidYellowPhone"))
    + (tools[0x5] = new UIW_TBUTTON(toolbarLeft[0x6], buttonTop, toolbarWidth , NULL, tBTF_TBB, tWOF_TBB, NULL, UE_RECEIPT  , "LaserPrinter"))
    + (tools[0x6] = new UIW_TBUTTON(toolbarLeft[0x7], buttonTop, toolbarWidth , NULL, tBTF_TBB, tWOF_TBB, NULL, UE_ACCUM    , "PageClock"))
    + (tools[0x7] = new UIW_TBUTTON(toolbarLeft[0x8], buttonTop, toolbarWidth , NULL, tBTF_TBB, tWOF_TBB, NULL, UE_OPERATION, "HandComp"))
    + (tools[0x8] = new UIW_TBUTTON(toolbarLeft[0x9], buttonTop, toolbarWidth , NULL, tBTF_TBB, tWOF_TBB, NULL, UE_FORMS    , "ReceiptStack"))
    + (tools[0x9] = new UIW_TBUTTON(toolbarLeft[0xA], buttonTop, toolbarWidth , NULL, tBTF_TBB, tWOF_TBB, NULL, S_CLOSE     , "openedDoor"))
    + (tools[0xA] = new UIW_TBUTTON(toolbarLeft[0xB], buttonTop, microBmpWidth, NULL, tBTF_TBB, tWOF_TBB, NULL, UE_MD       , "MD"))
    ;
    //
    WBNumber->helpContext = H_MANUAL;
    int integer = 3;
    WBNumber->Information(SET_TEXT_LENGTH, &integer);
	if (!g_cfg->MANUAL)
        WBNumber->woFlags |= WOF_NON_SELECTABLE;
    tools[0x0]->helpContext = H_SP_SERV;
    tools[0x1]->helpContext = H_CALC;
    tools[0x2]->helpContext = H_SPY;
    tools[0x3]->helpContext = H_INTER;
    tools[0x4]->helpContext = H_LOCK;
    tools[0x5]->helpContext = H_RECEIPT;
	tools[0x6]->helpContext = H_ACCUM;
    tools[0x7]->helpContext = H_OPERATION;
    tools[0x8]->helpContext = H_FORMS;
    tools[0x9]->helpContext = H_QUIT;
    tools[0xA]->helpContext = H_MD;
    //
    *this + WToolBar;
}

void UIW_VIEW::addTitleGroups(REGION *groupRegions)
{
    WOF_FLAGS wofTB = WOF_MINICELL|WOF_JUSTIFY_CENTER;
	*this + &
	(
		*new UIW_SGROUP
		(0, 0+W_GAP, 10*VIEW_WIDTH, GBUTTON_HEIGHT+1, WOF_MINICELL | WOF_NON_SELECTABLE)
			+ new UIW_GBUTTON(groupRegions[0].X, 0, groupRegions[0].W, "Cab"      , BTF_NO_TOGGLE, wofTB, NULL)
			+ new UIW_GBUTTON(groupRegions[1].X, 0, groupRegions[1].W, "Estado"	  , BTF_NO_TOGGLE, wofTB, NULL)
			+ new UIW_GBUTTON(groupRegions[2].X, 0, groupRegions[2].W, "Área"	    , BTF_NO_TOGGLE, wofTB, NULL)
			+ new UIW_GBUTTON(groupRegions[3].X, 0, groupRegions[3].W, "Teléfono" , BTF_NO_TOGGLE, wofTB, NULL)
			+ new UIW_GBUTTON(groupRegions[4].X, 0, groupRegions[4].W, "Localidad", BTF_NO_TOGGLE, wofTB, NULL)
			+ new UIW_GBUTTON(groupRegions[5].X, 0, groupRegions[5].W, "Minut"    , BTF_NO_TOGGLE, wofTB, NULL)
			+ new UIW_GBUTTON(groupRegions[6].X, 0, groupRegions[6].W, "Tar"      , BTF_NO_TOGGLE, wofTB, NULL)
			+ new UIW_GBUTTON(groupRegions[7].X, 0, groupRegions[7].W, "Valor"    , BTF_NO_TOGGLE, wofTB, NULL)
			+ new UIW_GBUTTON(groupRegions[8].X, 0, groupRegions[8].W, "LLamad"   , BTF_NO_TOGGLE, wofTB, NULL)
	);
}

void UIW_VIEW::addTable(REGION *groupRegions)
{
	UIW_TABLE *Table = new UIW_TABLE
	(
		groupRegions[0].X, groupRegions[0].Y, (VIEW_WIDTH-1)*10,
		(VISIBLE_BOOTHS)*GBUTTON_HEIGHT+2, WOF_BORDER|WOF_MINICELL
	);
	//
    // a group of tools indicating each Booth number ...
    //
	WBoothNumbers = new UIW_BUTTON*[NumOfClusters][CLUSTER_SIZE];
	int cNum, bNum, boothCount;
    char s[5];
	for (cNum=0; cNum<NumOfClusters; cNum++)
	{
		for (bNum=0; bNum<CLUSTER_SIZE; bNum++)
		{
			boothCount = cNum*CLUSTER_SIZE+bNum;
			itoa(boothCount+1, s, 10);
			WBoothNumbers[cNum][bNum] = new UIW_GBUTTON
			(
				groupRegions[0].X, boothCount*GBUTTON_HEIGHT,
				groupRegions[0].W, s,
				BTF_NO_TOGGLE, WOF_JUSTIFY_CENTER|WOF_MINICELL,
				processBooth
			);
			*Table + WBoothNumbers[cNum][bNum];
		}
	}
	//
	// a group of bitmap indicating each Booth status ...
	//
	WStates  = new UIW_BUTTON*[NumOfClusters][CLUSTER_SIZE];
	ToneFSs  = new int[NumOfClusters][CLUSTER_SIZE];
	PulseFSs = new int[NumOfClusters][CLUSTER_SIZE];
	for (cNum=0; cNum<NumOfClusters;cNum++)
		for (bNum=0; bNum<CLUSTER_SIZE; bNum++)
		{
			boothCount = cNum*CLUSTER_SIZE+bNum;
			ToneFSs[cNum][bNum]	 = RT_ENGINE::ONHOOK;
			PulseFSs[cNum][bNum] = RT_ENGINE::LOCK;
			WStates[cNum][bNum] = new UIW_GBUTTON
			(
				groupRegions[1].X, boothCount*GBUTTON_HEIGHT,
				groupRegions[1].W,
				States[RT_ENGINE::NOPHONE].Text,
				BTF_NO_3D|BTF_NO_TOGGLE, WOF_DATA, NULL, 0,
				States[RT_ENGINE::NOPHONE].BitmapName
			);
			*Table + WStates[cNum][bNum];
		}
	//
	// a group of strings indicating the area code ...
	//
	WAreas= new UIW_STRING*[NumOfClusters][CLUSTER_SIZE];
	for (cNum=0; cNum<NumOfClusters;cNum++)
		for (bNum=0; bNum<CLUSTER_SIZE; bNum++)
		{
			boothCount = cNum*CLUSTER_SIZE+bNum;
			WAreas[cNum][bNum] = new UIW_STRING
			(
				groupRegions[2].X, boothCount*GBUTTON_HEIGHT,
				groupRegions[2].W,
				m_callInfo[cNum][bNum].area,
				groupRegions[3].W, STF_NO_FLAGS,
				WOF_DATA|WOF_JUSTIFY_RIGHT
			);
			*Table + WAreas[cNum][bNum];
        }
    //
    // a group of formatted strings indicating the phone number ...
    //
	WPhones = new UIW_STRING*[NumOfClusters][CLUSTER_SIZE];
	for (cNum=0; cNum<NumOfClusters;cNum++)
	{
		for (bNum=0; bNum<CLUSTER_SIZE; bNum++)
		{
			boothCount = cNum*CLUSTER_SIZE+bNum;
			WPhones[cNum][bNum] = new UIW_STRING
			(
				groupRegions[3].X, boothCount*GBUTTON_HEIGHT,  groupRegions[3].W,
				m_callInfo[cNum][bNum].phone, groupRegions[3].W,
				STF_NO_FLAGS, WOF_DATA
			);
			*Table + WPhones[cNum][bNum];
		}
    }
	//
	// a group of strings indicating the target city...
	//
	WCities= new UIW_STRING*[NumOfClusters][CLUSTER_SIZE];
	for (cNum=0; cNum<NumOfClusters;cNum++)
		for (bNum=0; bNum<CLUSTER_SIZE; bNum++)
		{
			boothCount = cNum*CLUSTER_SIZE+bNum;
			WCities[cNum][bNum] = new UIW_STRING
			(
				groupRegions[4].X, boothCount*GBUTTON_HEIGHT, groupRegions[4].W,
				m_callInfo[cNum][bNum].city, groupRegions[4].W,
				STF_NO_FLAGS, WOF_DATA
			);
			*Table + WCities[cNum][bNum];
        }
    //
    // a group of times ...
    //
	WElapsedTimes= new UIW_REAL*[NumOfClusters][CLUSTER_SIZE];
	for (cNum=0; cNum<NumOfClusters;cNum++)
	{
		for (bNum=0; bNum<CLUSTER_SIZE; bNum++)
		{
			boothCount = cNum*CLUSTER_SIZE+bNum;
			WElapsedTimes[cNum][bNum] = new UIW_REAL
			(
				groupRegions[5].X, boothCount*GBUTTON_HEIGHT,
				groupRegions[5].W,
				&m_callInfo[cNum][bNum].rawMin,
				NULL, NMF_DECIMAL(4), WOF_DATA
			);
			*Table + WElapsedTimes[cNum][bNum];
		}
    }
	//
	// a group of integers indicating the Tariff ...
	//
	WTariffs= new UIW_INTEGER*[NumOfClusters][CLUSTER_SIZE];
	for (cNum=0; cNum<NumOfClusters;cNum++)
	{
		for (bNum=0; bNum<CLUSTER_SIZE; bNum++)
		{
			boothCount = cNum*CLUSTER_SIZE+bNum;
			WTariffs[cNum][bNum] = new UIW_INTEGER
			(
				groupRegions[6].X, boothCount*GBUTTON_HEIGHT,
				groupRegions[6].W,
				&m_callInfo[cNum][bNum].nTariff, NULL, NMF_NO_FLAGS,
				WOF_DATA|WOF_JUSTIFY_RIGHT
			);
			*Table + WTariffs[cNum][bNum];
		}
    }
	//
	// a group of reals (BIGNUMS) indicating the value of the call ...
	//
	WValues= new UIW_BIGNUM*[NumOfClusters][CLUSTER_SIZE];
	for (cNum=0; cNum<NumOfClusters;cNum++)
	{
		for (bNum=0; bNum<CLUSTER_SIZE; bNum++)
		{
			boothCount = cNum*CLUSTER_SIZE+bNum;
			WValues[cNum][bNum] = new UIW_BIGNUM
			(
				groupRegions[7].X, boothCount*GBUTTON_HEIGHT,
				groupRegions[7].W,
				&UI_BIGNUM(m_callInfo[cNum][bNum].value), NULL,
				NMF_CURRENCY|NMF_COMMAS|NMF_DECIMAL(g_cfg->VIEW_DECIMALS),
				WOF_DATA|WOF_JUSTIFY_RIGHT
			);
			*Table + WValues[cNum][bNum];
		}
    }
	//
	// a group of integers indicating how many calls are made...
	//
	WNumOfCalls= new UIW_INTEGER*[NumOfClusters][CLUSTER_SIZE];
	int w = groupRegions[8].W-7;

	if (NumOfClusters > 2)
		w -= 26;

	for (cNum=0; cNum<NumOfClusters; cNum++)
	{
		for (bNum=0; bNum<CLUSTER_SIZE; bNum++)
        {
			boothCount = cNum*CLUSTER_SIZE+bNum;
			WNumOfCalls[cNum][bNum] = new UIW_INTEGER
			(
				groupRegions[8].X, boothCount*GBUTTON_HEIGHT, w,
				&m_callInfo[cNum][bNum].nCalls, NULL,
				NMF_NO_FLAGS, WOF_DATA
			);
			*Table + WNumOfCalls[cNum][bNum];
		}
	}
	
	*this + Table;
}

void UIW_VIEW::resetData(UINT cNum, UINT bNum)
{
	ToneFSs[cNum][bNum]	   	= RT_ENGINE::LOCK;
	PulseFSs[cNum][bNum]	= RT_ENGINE::LOCK;

	m_callInfo[cNum][bNum].phone[0]   	= '\0';
	m_callInfo[cNum][bNum].city[0]    	= '\0';
	m_callInfo[cNum][bNum].area[0]    	= '\0';
	m_callInfo[cNum][bNum].elapsedTime 	= 0;
	m_callInfo[cNum][bNum].nTariff	   	= 0;
}

static UIW_VIEW::STATE states[] = {
    {"NoYellowPhone"   , "Blq"  , NULL},// RT_ENGINE::LOCK
    {"YellowPhone"     , "Lib"  , NULL},// RT_ENGINE::ONHOOK
    //
    {"RingUp"          , "Ring" , NULL},// RT_ENGINE::RINGUP
    {"RingDown"        , "Ring" , NULL},// RT_ENGINE::RINGDOWN
    {"IncomeTalk"      , "Com"  , NULL},// RT_ENGINE::INCOMETALK
    //
    {"unhook"          , "Des"  , NULL},// RT_ENGINE::OFF_HOOK
    {"unhook"          , "Des"  , NULL},// RT_ENGINE::BREAK
    {"unhook"          , "Des"  , NULL},// RT_ENGINE::MAKE
    {"unhook"          , "Des"  , NULL},// RT_ENGINE::INTERDIG
    {"unhook"          , "Des"  , NULL},// RT_ENGINE::DTMF_FLAG
    {"unhook"          , "Des"  , NULL},// RT_ENGINE::ANSWER
    //
    {"MouthPhone"      , "Com"  , NULL},// RT_ENGINE::TALK
    {"Disk"            , "Alm"  , NULL},// RT_ENGINE::STORE
    {"SmallMicrophone" , "Inv"  , NULL},// RT_ENGINE::SPY
    {"Colombia"        , "Nal"  , NULL},// RT_ENGINE::NAL
    {"WorldMap"        , "Inter", NULL},// RT_ENGINE::INTER
    {"NoKeyYellowPhone", "ErM"  , NULL},// RT_ENGINE::DIALERR
    {"ComErr"          , "ErC"  , NULL},// RT_ENGINE::COMERR
	{"NCPhone"         , "NC"   , NULL} // RT_ENGINE::NOPHONE
};
UIW_VIEW::STATE *UIW_VIEW::States = states;

void UIW_VIEW::GetDumpData(void * & ptr, int &size)
{
	ptr  = m_callInfo;
	size = sizeof(PH_ENGINE::CallInfo) * NumOfClusters * CLUSTER_SIZE;
}

void UIW_VIEW::SetDumpData(void * ptr, int size)
{
	memcpy(m_callInfo, ptr, size);
}

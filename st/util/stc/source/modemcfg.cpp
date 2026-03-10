//
// [ MODEMCFG.CPP ]
//

#include <comdef.hpp>
#include <compplib.hpp>
#include <stc.h>

extern CFG *g_cfg;

MODEMCFG::MODEMCFG(void): UIW_WINDOW("MODEMCFG", defaultStorage)
{
    UIW_BUTTON *wBtn;
    char *btnStr;
    // COM
    btnStr = "COM2";
	switch (g_cfg->MODEM_COM)
    {
    case COM1:
        btnStr = "COM1";
        break;
    case COM2:
        btnStr = "COM2";
        break;
    case COM3:
        btnStr = "COM3";
        break;
    case COM4:
        btnStr = "COM4";
        break;
    }
    wBtn = (UIW_BUTTON *)Get(btnStr);
    wBtn->woStatus |= WOS_SELECTED;
    wBtn->Event(S_CURRENT);
    // IRQ
    btnStr = "IRQ3";
	switch (g_cfg->MODEM_IRQ)
    {
    case IRQ1:
        btnStr = "IRQ1";
        break;
    case IRQ2:
        btnStr = "IRQ2";
        break;
    case IRQ3:
        btnStr = "IRQ3";
        break;
    case IRQ4:
        btnStr = "IRQ4";
        break;
    }
    wBtn = (UIW_BUTTON *)Get(btnStr);
    wBtn->woStatus |= WOS_SELECTED;
    wBtn->Event(S_CURRENT);
    // BASE
    btnStr = "2F8";
	switch (g_cfg->MODEM_BASE)
    {
    case 0x2E8:
        btnStr = "2E8";
        break;
    case 0x2F8:
        btnStr = "2F8";
        break;
    case 0x3E8:
        btnStr = "3E8";
        break;
    case 0x3F8:
        btnStr = "3F8";
        break;
    }
    wBtn = (UIW_BUTTON *)Get(btnStr);
    wBtn->woStatus |= WOS_SELECTED;
    wBtn->Event(S_CURRENT);
    // BAUDS
    btnStr = "9600";
	switch (g_cfg->MODEM_BAUDS)
    {
    case  1200L:
        btnStr =  "1200";
        break;
    case  2400L:
        btnStr =  "2400";
        break;
    case  9600L:
        btnStr =  "9600";
        break;
    case  19200:
        btnStr = "19200";
        break;
    }
    wBtn = (UIW_BUTTON *)Get(btnStr);
    wBtn->woStatus |= WOS_SELECTED;
    wBtn->Event(S_CURRENT);
    // DIAL
    btnStr = "PULSE";
	switch (g_cfg->MODEM_DIAL)
    {
    case  GCPP_TONE:
        btnStr = "TONE" ;
        break;
    case GCPP_PULSE:
        btnStr = "PULSE";
        break;
    }
    wBtn = (UIW_BUTTON *)Get(btnStr);
    wBtn->woStatus |= WOS_SELECTED;
    wBtn->Event(S_CURRENT);
    // helpContext = H_CONFIG;
}


MODEMCFG::~MODEMCFG(void)
{}


EVENT_TYPE MODEMCFG::Event(const UI_EVENT &event)
{
    EVENT_TYPE ccode = event.type;
    switch (ccode)
    {
    case UE_CANCEL:
        eventManager->Put(UI_EVENT(S_CLOSE,0));
        break;
    case UE_ACCEPT:
        {
            // COM
            if (FlagSet(((UIW_BUTTON *)Get("COM1"))->woStatus, WOS_SELECTED))
				g_cfg->MODEM_COM = COM1;
            else if (FlagSet(((UIW_BUTTON *)Get("COM2"))->woStatus, WOS_SELECTED))
				g_cfg->MODEM_COM = COM2;
            else if (FlagSet(((UIW_BUTTON *)Get("COM3"))->woStatus, WOS_SELECTED))
				g_cfg->MODEM_COM = COM3;
            else if (FlagSet(((UIW_BUTTON *)Get("COM4"))->woStatus, WOS_SELECTED))
				g_cfg->MODEM_COM = COM4;
            // IRQ
            if (FlagSet(((UIW_BUTTON *)Get("IRQ1"))->woStatus, WOS_SELECTED))
				g_cfg->MODEM_IRQ = IRQ1;
            else if (FlagSet(((UIW_BUTTON *)Get("IRQ2"))->woStatus, WOS_SELECTED))
				g_cfg->MODEM_IRQ = IRQ2;
            else if (FlagSet(((UIW_BUTTON *)Get("IRQ3"))->woStatus, WOS_SELECTED))
				g_cfg->MODEM_IRQ = IRQ3;
            else if (FlagSet(((UIW_BUTTON *)Get("IRQ4"))->woStatus, WOS_SELECTED))
				g_cfg->MODEM_IRQ = IRQ4;
            // BASE
            if (FlagSet(((UIW_BUTTON *)Get("2E8"))->woStatus, WOS_SELECTED))
				g_cfg->MODEM_BASE = 0x2E8;
            else if (FlagSet(((UIW_BUTTON *)Get("2F8"))->woStatus, WOS_SELECTED))
				g_cfg->MODEM_BASE = 0x2F8;
            else if (FlagSet(((UIW_BUTTON *)Get("3E8"))->woStatus, WOS_SELECTED))
				g_cfg->MODEM_BASE = 0x3E8;
            else if (FlagSet(((UIW_BUTTON *)Get("3F8"))->woStatus, WOS_SELECTED))
				g_cfg->MODEM_BASE = 0x3F8;
            // BAUDS
            if (FlagSet(((UIW_BUTTON *)Get("1200"))->woStatus, WOS_SELECTED))
				g_cfg->MODEM_BAUDS = 1200L;
            else if (FlagSet(((UIW_BUTTON *)Get("2400"))->woStatus, WOS_SELECTED))
				g_cfg->MODEM_BAUDS = 2400L;
            else if (FlagSet(((UIW_BUTTON *)Get("9600"))->woStatus, WOS_SELECTED))
				g_cfg->MODEM_BAUDS = 9600L;
            else if (FlagSet(((UIW_BUTTON *)Get("19200"))->woStatus, WOS_SELECTED))
				g_cfg->MODEM_BAUDS = 19200L;
            // DIAL
            if (FlagSet(((UIW_BUTTON *)Get("TONE"))->woStatus, WOS_SELECTED))
				g_cfg->MODEM_DIAL = GCPP_TONE;
            else
				g_cfg->MODEM_DIAL = GCPP_PULSE;
            //
            g_cfg->Save();
            //
            eventManager->Put(UI_EVENT(S_CLOSE,0));
            break;
        }
    default:
        ccode = UIW_WINDOW::Event(event);
        break;
    }
    return ccode;
}


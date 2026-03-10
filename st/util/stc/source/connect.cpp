//
// [ CONNECT.CPP ]
//
#include <comdef.hpp>
#include <compplib.hpp>
#include <stc.h>
#include <sid.h>

extern CFG *g_cfg;
extern int g_where;

CONNECTION::CONNECTION(WORD connectionType): UIW_WINDOW("CONNECTION", defaultStorage)
{
    this->connectionType = connectionType;
    // last phone
	((UIW_STRING *)Get(SID_PHONE))->DataSet(g_cfg->MODEM_PHONE);
    // helpContext = H_CONNECTION;
    // connect to window
    MODEM_DEVICE::setSourceWindow(this);
    STC::modemDevice->setRole(MODEM_DEVICE::NO_ROLE);
    STC::modemDevice->setOperation(MODEM_DEVICE::IDLE_OPERATION);
}

CONNECTION::~CONNECTION(void)
{
    // disconnect from window
    MODEM_DEVICE::resetSourceWindow();
}

EVENT_TYPE CONNECTION::Event(const UI_EVENT &event)
{
    EVENT_TYPE ccode = event.type;
    switch (ccode)
    {
    case UE_CONNECT:
        {
            if (connect())
            {
                // drop window ...
                woAdvancedFlags &= ~WOAF_LOCKED; // disconnect lock !!!
                (void *)&(*UI_WINDOW_OBJECT::windowManager - this);
                delete this;
                //
                if (connectionType == ASCLIENT)
                {
                    UIW_WINDOW *window = new CLIENT(CLIENT::CONNECTIONMODE);
                    windowManager->Center(window);
                    *windowManager + window;
                }
                else
                {
                    UIW_WINDOW *window = new SERVER(SERVER::CONNECTIONMODE);
                    windowManager->Center(window);
                    *windowManager + window;
                }
            }
            else
            {
                STC::modemDevice->setRole(MODEM_DEVICE::NO_ROLE);
                STC::modemDevice->setOperation(MODEM_DEVICE::IDLE_OPERATION);
                //
                UI_WINDOW_OBJECT::errorSystem->ReportError(
                    UI_WINDOW_OBJECT::windowManager,
                    WOS_NO_STATUS,
                    "No se pudo establecer la comunicación."
                );
                // show the final message into local wMessage
                UIW_STRING *wMessage = (UIW_STRING *) Get(SID_MESSAGE);
                wMessage->DataSet(STC::modemDevice->getViewMessage());
            }
            break;
        }
    case UE_VIEW:
        {
            UIW_STRING *wMessage;
            if (messageWindow)
            {
                wMessage = (UIW_STRING *) messageWindow->Get(SID_MESSAGE);
                wMessage->DataSet(STC::modemDevice->getViewMessage());
            }
            else
            {
                wMessage = (UIW_STRING *) Get(SID_MESSAGE);
                wMessage->DataSet(STC::modemDevice->getViewMessage());
            }
            break;
        }
    case UE_CANCEL:
        {
            woAdvancedFlags &= ~WOAF_LOCKED; // disconnect lock !!!
            eventManager->Put(UI_EVENT(S_CLOSE,0));
            break;
        }
    default:
        ccode = UIW_WINDOW::Event(event);
        break;
    }
    return ccode;
}

BOOL CONNECTION::connect(void)
{
    BOOL ok = FALSE;
    //
    eventManager->DeviceState(E_MOUSE, DM_WAIT);
    // create the message window
    messageWindow = new UIW_WINDOW(SID_SPLASH, UI_WINDOW_OBJECT::defaultStorage);
    UI_WINDOW_OBJECT::windowManager->Center(messageWindow);
    (void *)&(*UI_WINDOW_OBJECT::windowManager + messageWindow);
    //
    if (STC::modemDevice->activate())
    {
        PHONE phone;
        strcpy(phone, ((UIW_STRING *)Get(SID_PHONE))->DataGet());
        if (STC::modemDevice->connect(phone))
        {
			strcpy(g_cfg->MODEM_PHONE, phone);
            g_cfg->Save();
            ok = TRUE;
        }
        else
        {
            STC::modemDevice->disconnect();
            STC::modemDevice->deactivate();
        }
    }
    else
    {
        STC::modemDevice->deactivate();
    }
    // eliminate message window
    (void *)&(*UI_WINDOW_OBJECT::windowManager - messageWindow);
    delete messageWindow;
    messageWindow = NULL;
    //
    eventManager->DeviceState(E_MOUSE, DM_VIEW);
    //
    return ok;
}

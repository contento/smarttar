//
// [ CLIENT.CPP ]
//

#include <comdef.hpp>
#include <compplib.hpp>
#include <stc.h>
#include <sid.h>

extern int g_where;

CLIENT::CLIENT(WORD mode): UIW_WINDOW("CLIENT", defaultStorage)
{
    this->mode = mode;
    // enable/disable operations
    UIW_BUTTON *wButton = (UIW_BUTTON *) Get(SID_CLOSE);
    if (mode == ACTIVATIONMODE)
    {
        wButton->DataSet("&Desactivar");
        // enable/disable operations
        wButton = (UIW_BUTTON *) Get(SID_SEND);
        wButton->woFlags |= WOF_NON_SELECTABLE;
        wButton = (UIW_BUTTON *) Get(SID_RECEIVE);
        wButton->woFlags |= WOF_NON_SELECTABLE;
        wButton = (UIW_BUTTON *) Get(SID_CONSOLE);
        wButton->woFlags |= WOF_NON_SELECTABLE;
    }
    // connect to window
    MODEM_DEVICE::setSourceWindow(this);
    //
    STC::modemDevice->setRole(MODEM_DEVICE::CLIENT_ROLE);
    STC::modemDevice->setOperation(MODEM_DEVICE::IDLE_OPERATION);
    // helpContext = H_CLIENT;
}


CLIENT::~CLIENT(void)
{
    // disconnect from window
    MODEM_DEVICE::resetSourceWindow();
}

EVENT_TYPE CLIENT::Event(const UI_EVENT &event)
{
    static UIW_WINDOW *wConsole    = NULL;
    static UIW_WINDOW *wFTransfer  = NULL;
    //
    EVENT_TYPE ccode = event.type;
    switch (ccode)
    {
    case UE_DISCONNECT:
        {
            // first change role and operation
            STC::modemDevice->setRole(MODEM_DEVICE::NO_ROLE);
            STC::modemDevice->setOperation(MODEM_DEVICE::IDLE_OPERATION);
            disconnect();
            //
            woAdvancedFlags &= ~WOAF_LOCKED; // disconnect lock !!!
            eventManager->Put(UI_EVENT(S_CLOSE,0));
            break;
        }
    case UE_SEND:
        {
            if (createList())
            {
                wFTransfer = new FTRANSFER;
                windowManager->Center(wFTransfer);
                *windowManager + wFTransfer;
                messageWindow = wFTransfer;
                //
                STC::modemDevice->setOperation(MODEM_DEVICE::SEND_OPERATION);
            }
            else
            {
                UI_WINDOW_OBJECT::errorSystem->ReportError(
                    UI_WINDOW_OBJECT::windowManager,
                    WOS_NO_STATUS,
                    "No se han seleccionado archivos"
                );
            }
            break;
        }
    case UE_RECEIVE:
        {
            if (createList())
            {
                wFTransfer = new FTRANSFER;
                windowManager->Center(wFTransfer);
                *windowManager + wFTransfer;
                messageWindow = wFTransfer;
                //
                STC::modemDevice->setOperation(MODEM_DEVICE::RECEIVE_OPERATION);
            }
            else
            {
                UI_WINDOW_OBJECT::errorSystem->ReportError(
                    UI_WINDOW_OBJECT::windowManager,
                    WOS_NO_STATUS,
                    "No se han seleccionado archivos"
                );
            }
            break;
        }
    case UE_NEXT:
        { // next file
            if (iterator)
            {
                if (*iterator)
                {
                    FILE_NAME name;
                    FILE_NAME remotePath, localPath;
                    strncpy( localPath, ((UIW_STRING *) Get(SID_LOCAL))->DataGet(), sizeof(localPath)-1);
                    localPath[sizeof(localPath)-1] = '\0';
                    strncpy(remotePath, ((UIW_STRING *) Get(SID_REMOTE))->DataGet(), sizeof(remotePath)-1);
                    remotePath[sizeof(remotePath)-1] = '\0';
                    strncpy(name, iterator->current(), sizeof(name)-1);
                    name[sizeof(name)-1] = '\0';
                    STC::modemDevice->setFile(localPath, remotePath, name);
                    (*iterator)++;
                }
                else
                {
                    // first modify operation
                    STC::modemDevice->setOperation(MODEM_DEVICE::IDLE_OPERATION);
                    //
                    delete filenames;
                    filenames = NULL;
                    delete iterator ;
                    iterator  = NULL;
                    //
                    if (wFTransfer)
                    { // to test close message
                        (void *)&(*UI_WINDOW_OBJECT::windowManager - wFTransfer);
                        delete wFTransfer;
                        wFTransfer = NULL;
                    }
                }
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
            if (STC::modemDevice->isConnected())
            {
                UIW_BUTTON *wButton;
                // enable/disable operations
                wButton = (UIW_BUTTON *) Get(SID_SEND);
                wButton->woFlags &= ~WOF_NON_SELECTABLE;
                wButton->Event(UI_EVENT(S_REDISPLAY, 0));
                wButton = (UIW_BUTTON *) Get(SID_RECEIVE);
                wButton->woFlags &= ~WOF_NON_SELECTABLE;
                wButton->Event(UI_EVENT(S_REDISPLAY, 0));
                wButton = (UIW_BUTTON *) Get(SID_CONSOLE);
                wButton->woFlags &= ~WOF_NON_SELECTABLE;
                wButton->Event(UI_EVENT(S_REDISPLAY, 0));
            }
            break;
        }
    case UE_CANCEL:
        { // canceled by device
            // first change role and operation
            STC::modemDevice->setRole(MODEM_DEVICE::NO_ROLE);
            STC::modemDevice->setOperation(MODEM_DEVICE::IDLE_OPERATION);
            // eliminate popup windows
            if (wConsole)
            {
                (void *)&(*UI_WINDOW_OBJECT::windowManager - wConsole);
                delete wConsole;
                wConsole = NULL;
            }
            if (wFTransfer)
            {
                (void *)&(*UI_WINDOW_OBJECT::windowManager - wFTransfer);
                delete wFTransfer;
                wFTransfer = NULL;
                if (filenames)
                {
                    delete filenames;
                    filenames = NULL;
                }
                if (iterator)
                {
                    delete iterator ;
                    iterator  = NULL;
                }
            }
            // next show the message (the device works independent)
            UI_WINDOW_OBJECT::errorSystem->ReportError(
                UI_WINDOW_OBJECT::windowManager,
                WOS_NO_STATUS,
                "La comunicaci�n fue suspendida."
            );
            // disconnect
            eventManager->Put(UI_EVENT(UE_DISCONNECT, 0));
            break;
        }
    case UE_CONSOLE:
        {
            wConsole = new CONSOLE;
            windowManager->Center(wConsole);
            *windowManager + wConsole;
            messageWindow = wConsole;
            STC::modemDevice->setOperation(MODEM_DEVICE::CONSOLE_OPERATION);
            break;
        }
    default:
        ccode = UIW_WINDOW::Event(event);
        break;
    }
    return ccode;
}

BOOL CLIENT::createList(void)
{
    BOOL ok = TRUE;
    // create list
    filenames = new Filenames;
    FILE_NAME name;
    UIW_GROUP *wGroup;
    UIW_BUTTON *wFile = NULL;
    wGroup = (UIW_GROUP *) Get("FILES");
    for (wFile = (UIW_BUTTON *) wGroup->First(); wFile; wFile=(UIW_BUTTON *)wFile->Next())
    {
        if (FlagSet(wFile->woStatus, WOS_SELECTED))
        {
            strncpy(name, wFile->DataGet(), sizeof(name)-1);
            name[sizeof(name)-1] = '\0';
            filenames->addAtTail(String(name));
        }
    }
    strncpy(name, ((UIW_STRING *)Get("ANOTHER"))->DataGet(), sizeof(name)-1);
    name[sizeof(name)-1] = '\0';
    if (strlen(name))
        filenames->addAtTail(String(name));
    // prepare iterator
    iterator  = new Iterator(*filenames);
    if (!(*iterator))
    {
        delete filenames;
        filenames = NULL;
        delete iterator ;
        iterator  = NULL;
        ok = FALSE;
    }
    return ok;
}

void CLIENT::disconnect(void)
{
    eventManager->DeviceState(E_MOUSE, DM_WAIT);
    // create the message window
    messageWindow = new UIW_WINDOW(SID_SPLASH, UI_WINDOW_OBJECT::defaultStorage);
    UI_WINDOW_OBJECT::windowManager->Center(messageWindow);
    (void *)&(*UI_WINDOW_OBJECT::windowManager + messageWindow);
    //
    STC::modemDevice->disconnect();
    STC::modemDevice->deactivate();
    //
    // eliminate message window
    (void *)&(*UI_WINDOW_OBJECT::windowManager - messageWindow);
    delete messageWindow;
    messageWindow = NULL;
}

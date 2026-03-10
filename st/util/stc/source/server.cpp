//
// [ SERVER.CPP ]
//

#include <stc.h>
#include <sid.h>

extern CFG *g_cfg;

SERVER::SERVER(WORD mode): UIW_WINDOW("SERVER", defaultStorage)
{
    this->mode = mode;
    //
    windowManager->Center(this);
    messageWindow = this;
    //
    UIW_BUTTON *wButton = (UIW_BUTTON *) Get(SID_CLOSE);
    if (mode == ACTIVATIONMODE)
        wButton->DataSet("&Desactivar");
    // connect to window
    MODEM_DEVICE::setSourceWindow(this);
    STC::modemDevice->setRole(MODEM_DEVICE::SERVER_ROLE);
    STC::modemDevice->setOperation(MODEM_DEVICE::IDLE_OPERATION);
    //	helpContext = H_SERVER;
}


SERVER::~SERVER()
{
    // disconnect from window
    MODEM_DEVICE::resetSourceWindow();
}

EVENT_TYPE SERVER::Event(const UI_EVENT &event)
{
    EVENT_TYPE ccode = event.type;
    switch (ccode)
    {
    case UE_CLOSE :  // from device
    case UE_ACCEPT:
        {
            // first change role and operation
            STC::modemDevice->setRole(MODEM_DEVICE::NO_ROLE);
            STC::modemDevice->setOperation(MODEM_DEVICE::IDLE_OPERATION);
            //
            STC::modemDevice->disconnect();
            STC::modemDevice->deactivate();
            //
            eventManager->Put(UI_EVENT(S_CLOSE,0));
            break;
        }
    case UE_VIEW:
        {
            if (messageWindow)
            {
                UIW_STRING *wMessage;
                wMessage = (UIW_STRING *) messageWindow->Get(SID_MESSAGE);
                wMessage->DataSet(STC::modemDevice->getViewMessage());
            }
            break;
        }
    case UE_CANCEL:
        { // canceled by device
            // first change role and operation
            STC::modemDevice->setOperation(MODEM_DEVICE::IDLE_OPERATION);
            // next show the message (the device works independent)
            if (messageWindow)
            {
                UIW_STRING *wMessage;
                wMessage = (UIW_STRING *) messageWindow->Get(SID_MESSAGE);
                wMessage->DataSet("La comunicación fue suspendida.");
            }
            break;
        }
    default:
        ccode = UIW_WINDOW::Event(event);
        break;
    }
    return ccode;
}

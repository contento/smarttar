//
// [ ACTIVATE.CPP ]
//

#include <comdef.hpp>
#include <compplib.hpp>
#include <stc.h>
#include <sid.h>

ACTIVATION::ACTIVATION(WORD activationType): UIW_WINDOW("ACTIVATION", defaultStorage)
{
    this->activationType = activationType;
    // helpContext = H_CONNECTION;
    // connect to window
    messageWindow = this;
    MODEM_DEVICE::setSourceWindow(this);
    STC::modemDevice->setRole(MODEM_DEVICE::NO_ROLE);
    STC::modemDevice->setOperation(MODEM_DEVICE::IDLE_OPERATION);
}

ACTIVATION::~ACTIVATION(void)
{
    // disconnect from window
    MODEM_DEVICE::resetSourceWindow();
}

EVENT_TYPE ACTIVATION::Event(const UI_EVENT &event)
{
    EVENT_TYPE ccode = event.type;
    switch (ccode)
    {
    case S_CREATE:
        {
            eventManager->Put(UI_EVENT(UE_CONTINUE, 0));
            ccode = UIW_WINDOW::Event(event);
            break;
        }
    case UE_CONTINUE:
        {
            if (STC::modemDevice->activate())
            {
                // drop window ...
                (void *)&(*UI_WINDOW_OBJECT::windowManager - this);
                delete this;
                //
                if (activationType == ASCLIENT)
                {
                    UIW_WINDOW *window = new CLIENT(CLIENT::ACTIVATIONMODE);
                    windowManager->Center(window);
                    *windowManager + window;
                }
                else
                {
                    UIW_WINDOW *window = new SERVER(SERVER::ACTIVATIONMODE);
                    windowManager->Center(window);
                    *windowManager + window;
                }
            }
            else
            {
                STC::modemDevice->deactivate();
                // drop window ...
                (void *)&(*UI_WINDOW_OBJECT::windowManager - this);
                delete this;
                //
                UI_WINDOW_OBJECT::errorSystem->ReportError(
                    UI_WINDOW_OBJECT::windowManager,
                    WOS_NO_STATUS,
                    "No se pudo activar el modem."
                );
                STC::modemDevice->setRole(MODEM_DEVICE::NO_ROLE);
                STC::modemDevice->setOperation(MODEM_DEVICE::IDLE_OPERATION);
            }
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
    default:
        ccode = UIW_WINDOW::Event(event);
        break;
    }
    return ccode;
}

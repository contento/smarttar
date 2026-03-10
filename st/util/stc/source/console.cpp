//
// [ CONSOLE.CPP ]
//

#include <comdef.hpp>
#include <compplib.hpp>
#include <stc.h>
#include <sid.h>

extern CFG *g_cfg;

CONSOLE::CONSOLE(void): UIW_WINDOW("CONSOLE", defaultStorage)
{
	// helpContext = H_CONSOLE;
}

CONSOLE::~CONSOLE(void)
{}


EVENT_TYPE CONSOLE::Event(const UI_EVENT &event)
{
    EVENT_TYPE ccode = event.type;
    switch (ccode)
    {
    case UE_ACCEPT:
        {
            eventManager->Put(UI_EVENT(S_CLOSE,0));
            break;
        }
    case UE_SEND:
        {
            ((UIW_STRING *)Get(SID_MESSAGE))->DataSet(""); // clean answer
            STC::modemDevice->sendMessage(
                ((UIW_STRING *)Get(SID_LINE))->DataGet()
            );
            break;
        }
    default:
        ccode = UIW_WINDOW::Event(event);
        break;
    }
    return ccode;
}

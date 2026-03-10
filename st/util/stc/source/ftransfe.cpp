//
// [ FTRANSFER.CPP ]
//

#include <comdef.hpp>
#include <compplib.hpp>
#include <stc.h>
#include <sid.h>

extern CFG *g_cfg;

FTRANSFER::FTRANSFER(void): UIW_WINDOW("FTRANSFER", defaultStorage)
{
	// helpContext = H_ZMODEM;
}


FTRANSFER::~FTRANSFER(void)
{
}

EVENT_TYPE FTRANSFER::Event(const UI_EVENT &event)
{
    EVENT_TYPE ccode = event.type;
    switch (ccode)
    {
    case UE_ACCEPT:
        {
            eventManager->Put(UI_EVENT(S_CLOSE,0));
            break;
        }
    default:
        ccode = UIW_WINDOW::Event(event);
        break;
    }
    return ccode;
}

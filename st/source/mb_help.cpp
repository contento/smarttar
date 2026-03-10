//
// [ MB_HELP.CPP ]
//

#include "stdst.h"

#include <db_eng.h>
#include <menubar.h>
#include <events.h>

#ifndef USE_HELP_CONTEXTS
#define USE_HELP_CONTEXTS
#include <help.hpp>
#endif

// --------------------------------------------------------------------------
//								UIW_ABOUT
// --------------------------------------------------------------------------

const char *MD_AUTHORS =
    APP_COPYRIGHT_DATE "\n"
    APP_ISO_COMPANY    "\n"
    "Correo Electrónico:\n"
	"  microdis@geo.net.co\n"
    "Teléfono:\n"
    "  +57 (4) 341-5600"
    ;

const char *DEV_AUTHORS =
    "GCSoft Ltda.\n"
    "Gonzalo Contento C.\n"
    "  mailto:gonzalo@conten.to\n"
    "Adriana M. Giraldo M.\n"
    "  mailto:adriana@conten.to\n"
    ;

UIW_ABOUT::UIW_ABOUT(void) : UIW_WINDOW("W_ABOUT", defaultStorage)
{
	((UIW_PROMPT *)Get("VERSION"))->DataSet(APP_VER_ID_NAME);

	STR128 str;
	strcpy(str, APP_BUILD);
	if (TraceInfo::s_bTest)
		strcat(str, " Test");
	if (TraceInfo::s_bDevelopment)
		strcat(str, " Dev");
	((UIW_PROMPT *)Get("BUILD"))->DataSet(str); // 2.21.8 build 5

	((UIW_TEXT *)Get("AUTHORS"))->DataSet((char *)MD_AUTHORS);

	windowManager->Center(this);
	helpContext = H_ABOUT;
}

EVENT_TYPE UIW_ABOUT::Event(const UI_EVENT &event)
{
    // Switch on the type of event.
    EVENT_TYPE ccode = event.type;
    switch (ccode)
    {
    case UE_AUTHORS:
        static BOOL md = FALSE;
        ((UIW_TEXT *)this->Get("AUTHORS"))->DataSet(
            (md)?(char *)MD_AUTHORS:(char *)DEV_AUTHORS
        );
        md = !md;
        break;
    case UE_ACCEPT:
        eventManager->Put(UI_EVENT(S_CLOSE,0));
        break;
    default:
        ccode = UIW_WINDOW::Event(event);
        break;
    }
    return ccode;
}

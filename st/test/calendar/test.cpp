//
// [ TEST.CPP ]
//
#if !defined(UI_WIN_HPP)
#include <ui_win.hpp>
#endif


#if !defined(__CALENDAR_H)
#include <calendar.h>
#endif

#if !defined(__CFG_H)
#include <cfg.h>
#endif

TConfig *_config;

int UI_APPLICATION::Main(void)
{
    _config = new TConfig;
    _config->Load();
    // The UI_APPLICATION constructor automatically initializes the
    // display, eventManager, and windowManager variables.
    // This line fixes linkers that don't look for main in the .LIBs.
    UI_APPLICATION::LinkMain();
    // Initialize the help system.
    *windowManager
    + new CALENDAR(1, 1, 71, 14);
    // Process user responses.
    UI_APPLICATION::Control();
    // Clean up.
    delete _config;
    return (0);
}

//
// [ TEST.CPP ]
//

//	 Date: 14-11-1997

#if !defined(__CFG_H)
#include <cfg.h>
#endif

#if !defined(__SPOOLER_H)
#include <spooler.h>
#endif

#if !defined(__EVENTS_H)
#include <events.h>
#endif

CFG        *_cfg;
SPOOLER    *_spooler;

#define USE_RAW_KEYS

#if !defined(UI_WIN_HPP)
#include <ui_win.hpp>
#endif

class EXPORT VIEW : public UIW_WINDOW
{
public:
    VIEW();
    ~VIEW();
    virtual EVENT_TYPE Event(const UI_EVENT &event);
private:
    UIW_STRING *wString;
};

class CONTROLLER : public UI_DEVICE
{
public:
    CONTROLLER(UI_EVENT_MANAGER *eventManager, UI_WINDOW_MANAGER *windowManager);
    ~CONTROLLER();
private:
    UI_EVENT_MANAGER *EventManager;
    VIEW             *View;
    //
    EVENT_TYPE  Event(const UI_EVENT &event);
    void        Poll(void);
};

main(int , char **)
{
    UI_DISPLAY *display = new UI_GRAPHICS_DISPLAY;
    UI_EVENT_MANAGER *eventManager = new UI_EVENT_MANAGER(display);
    *eventManager
    + new UID_KEYBOARD
    + new UID_MOUSE
    + new UID_CURSOR
    ;
    UI_WINDOW_MANAGER *windowManager = new UI_WINDOW_MANAGER(display, eventManager);
    // attach the new devices ...
    *eventManager
    + new CONTROLLER(eventManager, windowManager)
    ;
    // Process user events.
    EVENT_TYPE ccode;
    UI_EVENT event;
    do
    {
        if (!eventManager->Get(event, Q_NO_BLOCK|Q_BEGIN|Q_DESTROY|Q_POLL))
            ccode = windowManager->Event(event);
    }
    while (ccode != L_EXIT && ccode != S_NO_OBJECT);
    // Clean up.
    delete windowManager;
    delete eventManager;
    delete display;
    return (0);
}

CONTROLLER::CONTROLLER(UI_EVENT_MANAGER *eventManager, UI_WINDOW_MANAGER *windowManager)
        : UI_DEVICE(E_CONTROLLER, D_ON),
        EventManager(eventManager)
{
    _cfg = new CFG;
    _cfg->Load();
    //
    _spooler  = new SPOOLER;
    *EventManager
    + _spooler
    ;
    View = new VIEW;
    windowManager->Center(View);
    *windowManager
    + View
    ;
    // This line assigns the exit function to be called before the main
    // Window is closed.  It MUST be after the Window is added to windowManager.
    windowManager->screenID = View->screenID;
}

CONTROLLER::~CONTROLLER()
{
    delete _cfg;
}

void CONTROLLER::Poll(void)
{
    if (state != D_ON)
        return;
}

EVENT_TYPE CONTROLLER::Event(const UI_EVENT &event)
{
    EVENT_TYPE ccode = event.type;
    switch (ccode)
    {
    case E_DEVICE:
    case E_CONTROLLER:
        // Turn the ??? on or off.
        switch (event.rawCode)
        {
        case D_OFF:
        case D_ON:
            {
                state = event.rawCode;
                break;
            }
        }
        ccode = state;
        break;
    }
    // Return the control code.
    return (ccode);
}

VIEW::VIEW()
        : UIW_WINDOW(15, 5, 50, 6)
{
    *this
    + new UIW_BORDER
    + new UIW_MAXIMIZE_BUTTON
    + new UIW_MINIMIZE_BUTTON
    + new UIW_SYSTEM_BUTTON(SYF_GENERIC)
    + new UIW_TITLE("Testing Spooler", WOF_JUSTIFY_CENTER)
    ;
    UIW_BUTTON *wButton = new UIW_BUTTON(34, 2, 10, "Print",
                                         BTF_NO_TOGGLE|BTF_AUTO_SIZE|BTF_SEND_MESSAGE, WOF_JUSTIFY_CENTER,
                                         NULL, UE_PRINT
                                        );
    *this + wButton;
    wString = new UIW_STRING(2, 2, 30, "Testing this stupid PRN", 30);
    *this + wString;
}

VIEW::~VIEW()
{}


EVENT_TYPE VIEW::Event(const UI_EVENT &event)
{
    EVENT_TYPE ccode = event.type;
    switch (ccode)
    {
    case UE_PRINT:
        {
            char str[200];
            strcpy( str,
                    "\x1B\x40"
                    "Esta es la linea de prueba""\n"
                  );
            //
            strcat(str, wString->DataGet());
            strcat(str, "\n");
            strcat(str, wString->DataGet());
            strcat(str, "\n");
            strcat(str, wString->DataGet());
            strcat(str, "\n");
            strcat(str, wString->DataGet());
            strcat(str, "\n");
            //
            strcat(str, "  ---- Fin ----""\n");
            strcat(str,
                   "\n"
                   "\xFF"
                  );
            _spooler->Print(0, str, TRUE);
            break;
        }
    default:
        ccode = UIW_WINDOW::Event(event);
    }
    return ccode;
}

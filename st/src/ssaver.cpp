//
// [ SSAVER.CPP ]
//

#include "stdst.h"

#include <ui_win.hpp>
#include <events.h>
#include <ssaver.h>

extern CFG *g_cfg;

SSAVER::SSAVER(void) : UI_DEVICE(E_SAVER, D_ON),
        Bmp(NULL), Active(FALSE), Time(1),
        AsyncTrigger(FALSE),
        Left(display->columns/2), Top(display->lines/2)
{
    if (UI_WINDOW_OBJECT::defaultStorage && !UI_WINDOW_OBJECT::defaultStorage->storageError)
    {
        UI_WINDOW_OBJECT::defaultStorage->ChDir("~UI_BITMAP");
        char bmpName[0x10];
		sprintf(bmpName, "SS_%02d", g_cfg->SS_ID);
        UI_STORAGE_OBJECT *bmpFile = new UI_STORAGE_OBJECT(*UI_WINDOW_OBJECT::defaultStorage, bmpName, ID_BITMAP_IMAGE, UIS_READ);
        if (bmpFile->objectError)
        {
            delete bmpFile;
            bmpFile = new UI_STORAGE_OBJECT(*UI_WINDOW_OBJECT::defaultStorage, "SS_00", ID_BITMAP_IMAGE, UIS_READ);
        }
        if (!bmpFile->objectError)
        {
            bmpFile->Load(&BmpWidth);
            bmpFile->Load(&BmpHeight);
            long bmpSize = (long)BmpWidth * (long)BmpHeight;
            if (bmpSize > 0)
            {
                Bmp = new UCHAR[bmpSize];
                bmpFile->Load(Bmp, sizeof(UCHAR), bmpSize);
            }
        }
        delete bmpFile;
        //
        if (Bmp)
        {
            MaxCols = display->columns-BmpWidth;
            MaxLines= display->lines-BmpHeight;
        }
    }
}

SSAVER::~SSAVER(void)
{
    if (Bmp)
        delete [] Bmp;
}

EVENT_TYPE SSAVER::Event(const UI_EVENT &event)
{
    EVENT_TYPE ccode = event.type;
    switch (ccode)
    {
    case E_DEVICE:
    case E_SAVER:
        {
            // messages via EventManager->Put()
            switch (event.rawCode)
            {
            case D_ON:
            case D_OFF:
                state = event.rawCode;
                break;
            case DM_VIEW:
                AsyncTrigger = TRUE;
                break;
            }
            ccode = state;
            break;
        }
    case DM_VIEW:
        AsyncTrigger = TRUE;
        break;
    }
    return (ccode);
}

void SSAVER::Poll(void)
{
    if (state != D_ON)
        return;
    //
    static UI_TIME curTime;
    int curHour, curMin, curSec;
    curTime.Import();
    curTime.Export(&curHour, &curMin, &curSec);
    static int lastSec = curSec;
    if (lastSec != curSec)
    {
        lastSec = curSec;
        Time++; // in seconds
    }
    //


    if (!Active || AsyncTrigger)
    {
        if (Time == g_cfg->SS_TIME*60 || AsyncTrigger)
        {
            InitSaver();
            AsyncTrigger = FALSE;
        }
    }
    if (Active)
    {
        Time = 1; // avoid re-entrance
        DoSaver();
    }
    // check to see if other devices inserted msg
    UI_EVENT tmpEvent;
    if (!eventManager->Get(tmpEvent, Q_BEGIN | Q_NO_DESTROY | Q_NO_BLOCK | Q_NO_POLL))
    {
        if (tmpEvent.type == E_KEY || tmpEvent.type == E_MOUSE)
        {
            Time = 1; // while other are processing not to activate
            if (Active)
            {
                Active = FALSE;
                UI_WINDOW_OBJECT::windowManager->Event(UI_EVENT(S_REDISPLAY));
                eventManager->DeviceState(E_MOUSE, D_ON);
                // bye to the lastest event to avoid surprises in screen
                eventManager->Get(tmpEvent, Q_BEGIN|Q_NO_BLOCK|Q_NO_POLL);
            }
        }
    }
}

void SSAVER::InitSaver(void)
{
    static UI_PALETTE blackPalette = {
        ' ', attrib(BLACK, BLACK),
        attrib(MONO_BLACK, MONO_BLACK), PTN_SOLID_FILL, BLACK, BLACK,
        BW_BLACK, BW_BLACK, GS_GRAY, GS_GRAY
    };
    eventManager->DeviceState(E_MOUSE, D_HIDE);
    display->Rectangle(ID_DIRECT, 0, 0, display->columns-1, display->lines-1, &blackPalette, 1, TRUE);
    display->Bitmap(ID_DIRECT, Left, Top, BmpWidth, BmpHeight, Bmp);
    Active = TRUE;
}

void SSAVER::DoSaver(void)
{
    static int downWard = TRUE, rightWard = TRUE;
    //
    if (Active)
    {
        UI_REGION oldRegion;
        oldRegion.left = Left;
        oldRegion.top  = Top;
        oldRegion.right  = Left+BmpWidth;
        oldRegion.bottom = Top +BmpHeight;
        // Calc new position
        if (rightWard)
        {
            Left++;
            if (Left == MaxCols) rightWard = FALSE;
        }
        else
        {
            Left--;
            if (!Left) rightWard = TRUE;
        }
        if (downWard)
        {
            Top++;
            if (Top == MaxLines) downWard = FALSE;
        }
        else
        {
            Top--;
            if (!Top) downWard = TRUE;
        }
        //
        display->RegionMove(oldRegion, Left, Top);
    }
}

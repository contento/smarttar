//
// [ TEST.CPP ]
//
// SmartTar Communication Center. Gonzalo Contento (c) 1997.
// MicroDiseño Ltda. Nov. 1997.
//

#define Uses_TDialog
#define Uses_TApplication
#define Uses_TEditWindow
#define Uses_TDeskTop
#define Uses_TProgram
#define Uses_TScreen
#define Uses_TInputLine
#define Uses_TLabel
#define Uses_TRect
#define Uses_TButton
#define Uses_MsgBox
#define Uses_TMenuBar
#define Uses_TMenu
#define Uses_TSubMenu
#define Uses_TMenuItem
#define Uses_TEditor
#define Uses_TFileEditor
#define Uses_TKeys
#define Uses_TStatusLine
#define Uses_TStatusItem
#define Uses_TStatusDef
#define Uses_THistory
#define Uses_TFilterValidator

#include <tvision\tv.h>

#if !defined(__STRSTREA_H)
#include <strstrea.h>
#endif

#if !defined(__FSTREAM_H)
#include <fstream.h>
#endif

#if !defined(__ST_DEFS_H)
#include <st_defs.h>
#endif

#if !defined(__PH_ENG_H)
#include <ph_eng.h>
#endif

#if !defined(__CFG_H)
#include <cfg.h>
#endif

#if !defined(__NEW_H)
#include <new.h>
#endif

// ~%^%&$#@!&*, _stklen GRRR ...
extern unsigned _stklen = 0x4000;

CFG *_cfg;

#if (_TV_VERSION <= 0x103) // Turbo Vision 1.0
ushort executeDialog(TDialog *d, void *data);
#endif

const cmInf2Dat = 100;
const cmDat2Inf = 101;

class TApp : public TApplication
{
public:
    TApp();
    //
    virtual void handleEvent(TEvent& event);
    static TMenuBar    *initMenuBar(TRect);
    static TStatusLine *initStatusLine(TRect);
    virtual void outOfMemory();
private:
    void inf2Dat(void);
    void dat2Inf(void);
};

TApp::TApp() :
        TProgInit(&TApp::initStatusLine,
                  &TApp::initMenuBar,
                  &TApp::initDeskTop
                 )
{}


TMenuBar *TApp::initMenuBar(TRect r)
{
    // llamar
    TSubMenu& stMenu = *new TSubMenu("~S~martTar", kbAltS) +
                       *new TMenuItem("~I~nf2Dat ...", cmInf2Dat, kbCtrlI) +
                       *new TMenuItem("~D~at2Inf ...", cmDat2Inf, kbCtrlD)
                       ;
    r.b.y = r.a.y+1;
    return new TMenuBar(r,
                        stMenu
                       );
}

TStatusLine *TApp::initStatusLine(TRect r)
{
    r.a.y = r.b.y-1;
    return new TStatusLine(r,
                           *new TStatusDef(0, 0xFFFF) +
                           *new TStatusItem("~Alt-F4~ Salir", kbAltF4, cmQuit) +
                           *new TStatusItem("[MicroDise¤o Ltda]", kbNoKey, cmDefault)
                          );
}

void TApp::handleEvent(TEvent& event)
{
    TApplication::handleEvent(event);
    if(event.what != evCommand)
        return;
    else
        switch(event.message.command)
        {
        case cmInf2Dat :
            inf2Dat();
            break;
        case cmDat2Inf :
            dat2Inf();
            break;
            //
        default:
            return ;
        }
    clearEvent(event);
}

void TApp::outOfMemory()
{
    messageBox("No hay suficiente memoria para esta operaci¢n.", mfError | mfOKButton);
}

void TApp::inf2Dat(void)
{
    PH_ENGINE *phEngine = new PH_ENGINE;
    phEngine->LoadFromInfs(); // from .INF
    phEngine->Save();
    delete phEngine;
}

void TApp::dat2Inf(void)
{
    PH_ENGINE *phEngine = new PH_ENGINE;
    phEngine->Load(); // from .DAT
    phEngine->Save();
    delete phEngine;
}

void mem_warn()
{
    cerr << "\nCan't allocate!";
    exit(1);
}

int main(int , char **)
{
    set_new_handler(mem_warn);

#if (_TV_VERSION >= 0x200) // Turbo Vision 2.0
    TScreen::clearOnSuspend=False;
#endif
    cout << "STC Copyright (c) 1998 Microdise¤o Ltda." << endl;
    _cfg = new CFG;
    WORD cfgStatus = _cfg->Load(); // load CFG
    if (cfgStatus != CFG::OK)
    {
        char buf[100];
        ostrstream os(buf, sizeof(buf));
        os << "  Error: El archivo de configuraci¢n (st.cfg) ";
        switch (cfgStatus)
        {
        case CFG::NO_CFG_FILE :
            os << "no existe." << ends;
            break;
        case CFG::BAD_CFG_FILE:
            os << "est  corrupto." << ends;
            break;
        }
        cerr << buf << endl;
        delete _cfg;
        return 1;
    }
    //
#if (_TV_VERSION >= 0x200) // Turbo Vision 2.0
    TScreen::clearOnSuspend=True;
#endif
    //
    TApp *app = new TApp;
    app->run();
    TObject::destroy(app);
    //
    delete _cfg;
    //
    return 0;
}

#if (_TV_VERSION <= 0x103) // Turbo Vision 1.0
#pragma warn .rvl

ushort executeDialog(TDialog *d, void *data)
{
    TView *p = TProgram::application->validView(d);
    if(p == 0)
        return cmCancel;
    else
    {
        if(data != 0)
            p->setData(data);
        ushort result = TProgram::deskTop->execView(p);
        if(result != cmCancel && data != 0)
            p->getData(data);
        TObject::destroy(p);
        return result;
    }
}
#endif

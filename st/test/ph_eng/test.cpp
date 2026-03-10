//
// [ TEST.CPP ]
//
#include <iostream.h>
#include <ph_eng.h>
#include <cfg.h>

#ifdef DOSX286
#include <phapi.h>
#endif

extern unsigned _stklen = 0x4000; // ~%^%&$#@!&*, _stklen GRRR ...

CFG *_cfg;

char *errorMemory;

void NewHandler(void);

int main(void)
{
    errorMemory = new char[0x200];
    extern void (*_new_handler)(void);
    _new_handler = NewHandler;

    _cfg = new CFG;
    if (_cfg->Load() == CFG::OK)
    {
        PH_ENGINE* phEngine;
        phEngine = new PH_ENGINE;
        //
        // Load from .INF
        //
        cout << "Loading ..." << endl;
        //		phEngine->LoadFromInfs();
        phEngine->Load();
        //
        PH_ENGINE::PLACE_ENTRY entry;
        PH_ENGINE::CALL_PARAMETERS parameters;
        PHONE phone;
        do
        {
            cout << "N£mero (0 para terminar): ";
            cin >> (char *)&phone;
            if (atol(phone) != 0L)
            {
                if (phEngine->Search(phone, entry, parameters))
                {
                    cout
                    << "\tC¢digo de Area: " << parameters.AreaCode << endl
                    << "\t         Lugar: " << (char *)entry.Place << endl
                    << "\t        Tarifa: " << entry.TariffNum    << endl
                    ;
                    if (entry.TariffNum)
                    {
                        if (parameters.Attr & INTERNATIONAL_CALL_MASK)
                        {
                            cout
                            << "\t   Cada minuto: " << phEngine->GetDDITariff(entry.TariffNum).Value      << endl
                            << "\t      Impuesto: " << phEngine->GetDDITariff(entry.TariffNum).TaxPercent << '%' << endl
                            ;
                        }
                        else
                        {
                            cout
                            << "\t   Cada minuto: " << phEngine->GetDDNTariff(entry.TariffNum).Value      << endl
                            << "\t      Impuesto: " << phEngine->GetDDNTariff(entry.TariffNum).TaxPercent << '%' << endl
                            ;
                        }
                    }
                }
                else
                {
                    cout << "No incluida" << endl;
                }
            }
        }
        while (atol(phone) != 0L);
        //
        // save both .DAT and .INFs
        //
        cout << "Saving ..." << endl;
        //		phEngine->Save();
        //		phEngine->SaveToInfs();
    }

    else
    {
        cerr << "Config file has problems !!!";
    }
    delete _cfg;
    return 0;
}

void NewHandler(void)
{
    if (errorMemory)
        delete [] errorMemory;
    cout << "You lost the memory ..." << endl;
    exit(1);
}


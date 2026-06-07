//
// [ TEST.CPP ]
//
#include <iostream.h>
#include <cfg.h>

int main(void)
{
    CFG *cfg;
    cfg = new CFG;

    int status = cfg->Load(); // load cfg
    if (status == CFG::OK)
        cout << "Config Ok !!!" << endl;
    else
        cout << "Default cfg will be created !!!" << endl;
    cfg->Save(); // save .CFG and .INI
    //
    delete cfg;
    return 0;
}

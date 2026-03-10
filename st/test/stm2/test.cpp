//
// [ TEST.CPP ]
//
#include <iostream.h>

#include <cfg.h>
#include <info.h>
#include <stm2.h>

void printSize(const char *label, WORD size, WORD offset)
{
    cout << "  " << label << size << " bytes at " << offset << endl;
}

APP_INFO g_appInfo;
STM2     g_STM2;
CFG      *g_cfg;

int main(void)
{
	CFG *g_cfg;
	g_cfg = new CFG;
	int status = g_cfg->Load(); // load cfg
    if (status != CFG::OK)
    {
        cout << "Default cfg will be created !!!" << endl;
		g_cfg->Save(); // save .CFG and .INI
    }

    g_appInfo.MaxClusters = 2;
    //
    STM2::Data *data;
#define PREPARE(object)	sizeof(object), ((int)&(object) - (int)(data))
    //
    cout << "[ STM2 Data Features ]" << endl;
    printSize("               Total: ", PREPARE(*data));
    printSize("         Exit string: ", PREPARE(data->exitString));
    printSize("              Serial: ", PREPARE(data->sysInfo.serial));
    printSize("                Date: ", PREPARE(data->sysInfo.date));
    printSize("                Time: ", PREPARE(data->sysInfo.time));
    printSize("       Booth cluster: ", PREPARE(data->boothClusters));
    printSize("          Statistics: ", PREPARE(data->statistics));
    printSize("            Receipts: ", PREPARE(data->receipts));
    cout << "    Number of receipts: " << sizeof(data->receipts)/sizeof(Receipt) << endl;
    printSize("Extension statistics: ", PREPARE(data->extensionStatistics));
    //
    delete g_cfg;
    //
    return 0;
}

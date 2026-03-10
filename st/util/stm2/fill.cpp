//
// [ FILL.CPP ]
//

#include "stdst.h"

#include <info.h>
#include <stm2.h>

APP_INFO g_appInfo;
STM2     g_STM2;
CFG      *g_cfg;

int main(int argc, char *argv[])
{
    cout
		<< "FILL 1.0 (" << APP_VER_NAME << ')' << endl
		<< APP_COPYRIGHT << endl
		<< "  uso: fill caracter" << endl
	;
	char c = 0;
	if (argc > 1)
		c = argv[1][0];
	//
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
	char *statusStr;
	switch (g_STM2.getStatus())
	{
	case STM2::NONE        :
		statusStr = "NONE"        ;
		break;
	case STM2::OK          :
		statusStr = "OK"          ;
		break;
	case STM2::BAD_SHUTDOWN:
		statusStr = "BAD_SHUTDOWN";
		break;
	case STM2::GARBAGE     :
		statusStr = "GARBAGE"     ;
		break;
	}
	cout << "STM2 status: " << statusStr << endl;
	//
	WORD banks = g_STM2.fill(c);
	cout
		<< "fill " << banks << " bank(s) "
		<< "with character '" << c << "'"
		<< " (" << int(c) << ")" << endl
	;
	//
	delete g_cfg;
	//
	return 0;
}

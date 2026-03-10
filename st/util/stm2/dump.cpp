//
// [ DUMP.CPP ]
//
#include "stdst.h"

#include <info.h>
#include <stm2.h>

APP_INFO 	g_appInfo;
STM2    	g_STM2;
CFG      	*g_cfg;

int main(int argc, char *argv[])
{
    cout
		<< "DUMP 1.0 (" << APP_VER_NAME << ')' << endl
		<< APP_COPYRIGHT << endl
		<< "  uso: dump [base] [cantidad]" << endl
	;

	WORD offset = 0, size = 256;
	if (argc > 2)
	{
		offset = atoi(argv[1]);
		size   = atoi(argv[2]);
	}

	CFG *g_cfg;
	g_cfg = new CFG;
	WORD status = g_cfg->Load(); // load CFG
	if (status != CFG::OK)
	{
		char *msg = " tiene una falla general.";
		switch (status)
		{
		case CFG::NO_CFG_FILE :
			msg = "no existe."    ;
			break;
		case CFG::BAD_CFG_FILE:
			msg = "est  corrupto.";
			break;
		}
		cerr << "El archivo de configuraci¢n " << msg << endl;

		delete g_cfg;

		return 1;
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
	cout << "  STM2 status: " << statusStr << endl;

	const BUFSIZE = 16;
	char  buffer[BUFSIZE];
	int x;

	for (WORD i = offset; i < (offset + size); i += BUFSIZE)
	{
		g_STM2.dump(i, buffer, BUFSIZE);
		cout.fill(0);
		for (x = 0; x < BUFSIZE; x++)
		{
			cout
			<< setiosflags(ios::hex|ios::uppercase)
			<< setw(2)
			<< toascii(buffer[x]) << ' '
			;
		}
		cout << " ";
		for (x = 0; x < BUFSIZE; x++)
		{
			char c = isprint(buffer[x])?buffer[x]:'.';
			cout << c;
		}
		cout << endl;
	}

	delete g_cfg;

	return 0;
}

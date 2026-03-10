//
// [ FDUMP.CPP ]
//
#include "stdst.h"

#include <info.h>
#include <stm2.h>

APP_INFO 	g_appInfo;
STM2    	g_STM2;
CFG      	*g_cfg;

int main()
{
	cout
		<< "FDUMP 1.0 (" << APP_VER_NAME << ')' << endl
		<< APP_COPYRIGHT << endl
	;

	CFG *g_cfg;
	g_cfg = new CFG;
	int status = g_cfg->Load(); // load cfg
	if (status != CFG::OK)
	{
		cout << "Usando CFG por defecto." << endl;
	}

	g_appInfo.MaxClusters = 2;

	const BUFSIZE = 0x400;
	char  buffer[BUFSIZE];

	FILE_NAME szFilename;

	WORD year, month, day, hour, minutes;
	_GetSysTime(hour, minutes);
	_GetSysDate(year, month, day);

	sprintf(szFilename, "%02d%02d%02d%02d.dmp", month, day, hour, minutes);
	_PrefixAppPath(szFilename);

	ofstream file(szFilename, ios::out|ios::binary);

	for (WORD i = 0; i < STM2_BANKSIZE; i += BUFSIZE)
	{
		g_STM2.dump(i, buffer, BUFSIZE);
		file.write(buffer, BUFSIZE);
	}

	cout << "Archivo: \"" << szFilename << "\" Creado." << endl;
	delete g_cfg;

	return 0;
}

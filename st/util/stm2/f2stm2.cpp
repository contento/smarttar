//
// [ FDUMP.CPP ]
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
		<< "F2STM2 1.0 (" << APP_VER_NAME << ')' << endl
		<< APP_COPYRIGHT << endl
	;

	if (argc != 2)
	{
		cerr << "Usage: f2stm2 filename" << endl;
		return 1;
	}

	FILE_NAME szFilename;
	strcpy(szFilename, argv[1]);

	ifstream file(szFilename, ios::in|ios::binary);
	if (!file)
	{
		cerr << "File not open " << szFilename << endl;
		return 1;
	}

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

	for (WORD i = 0; i < STM2_BANKSIZE; i += BUFSIZE)
	{
		file.read(buffer, BUFSIZE);
		g_STM2.replace(i, buffer, BUFSIZE);
	}

	cout << "STM2 replace with file: \"" << szFilename << "\"." << endl;
	delete g_cfg;

	return 0;
}

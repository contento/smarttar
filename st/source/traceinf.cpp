//
// [ TRACEINF.CPP ]
//

#include <stdst.h>

BOOL 	TraceInfo::s_bTest		 	= FALSE;
BOOL 	TraceInfo::s_bDevelopment	= FALSE;
WORD 	TraceInfo::s_nSemWait		= 0;
DWORD 	TraceInfo::s_nAvailableRAM	= 0L;
WORD	TraceInfo::s_nPass			= 0;

TraceInfo::TraceInfo()
{
	/////////////////////////////////////////////////////////////////
	// test variable
	char *pszSTTest = getenv("STTEST");
	if (pszSTTest && !strcmp(pszSTTest, "1"))
	{
		s_bTest = TRUE;
	}

	/////////////////////////////////////////////////////////////////
	// development
	FILE_NAME filename;
	strcat(_GetAppPath(filename), "ROSINA");
	ifstream file(filename);
	if (file)
	{
		STR256 str;
		file.get(str, sizeof(STR256));
		file.close();
		if (!strcmp(str, "My mother"))
			s_bDevelopment = TRUE;
	}
}

static TraceInfo s_traceInfo;

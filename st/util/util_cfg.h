#ifndef __UTIL_CFG_H
#define __UTIL_CFG_H

//
// util_cfg.h -- shared CFG load + password authentication for utilities
//
// Eliminates the identical boilerplate duplicated across 17+ util programs.
// Include this after cfg.h and st_util.h.
//

#if !defined(__CFG_H)
#include <cfg.h>
#endif

#if !defined(__ST_UTIL_H)
#include <st_util.h>
#endif

#include <iostream.h>
#include <string.h>

// ---------------------------------------------------------------------------
// util_cfgLoad -- load CFG, print localized error on failure
//
// Handles the TraceInfo::s_bTest exception (missing CFG OK in test mode).
// Returns TRUE if CFG loaded successfully, FALSE on error (caller should
// delete g_cfg and return).
// ---------------------------------------------------------------------------
inline BOOL util_cfgLoad(CFG *g_cfg)
{
	WORD status = g_cfg->Load();
	if (status != CFG::OK && !(TraceInfo::s_bTest && status == CFG::NO_CFG_FILE))
	{
		const char *msg = " tiene una falla general.";
		switch (status)
		{
		case CFG::NO_CFG_FILE :
			msg = "no existe."    ;
			break;
		case CFG::BAD_CFG_FILE:
			msg = "est\xE0 corrupto.";
			break;
		}
		cerr << "El archivo de configuraci\xE2n " << msg << endl;
		return FALSE;
	}
	return TRUE;
}

// ---------------------------------------------------------------------------
// util_authenticate -- prompt for UTIL password, validate
//
// Skipped in TraceInfo::s_bTest mode.
// Returns TRUE if authenticated (or test mode), FALSE if user aborted or
// access denied (caller should print nothing more, just return).
// ---------------------------------------------------------------------------
inline BOOL util_authenticate(CFG *g_cfg)
{
	if (!TraceInfo::s_bTest)
	{
		STR32 password;
		cout << "Presione Esc para abortar operaci\xE2n." << endl;
		cout   << "C\xF3digo de acceso: ";
		_ReadPassword(password, sizeof(CFG::PASSWORD)-1);
		if (!strlen(password))
			return FALSE;

		if (!g_cfg->isUtilPassword(password))
		{
			cerr << "Lo siento, acceso negado." << endl;
			return FALSE;
		}
	}
	return TRUE;
}

#endif // __UTIL_CFG_H

//
// [ ENG_FACT.CPP ]
//
// Factory Method for the ENGINE hierarchy.  Reads g_cfg->ENGINE_KIND
// at startup and returns the matching concrete: "demo" yields a
// DEMO_ENGINE, anything else yields RT_ENGINE.  Callers hold ENGINE*
// and never know which concrete is behind the pointer.
//

#include "stdst.h"

#include <eng_fact.h>
#include <engine.h>
#include <rt_eng.h>
#include <demo_eng.h>
#include <cfg.h>

#include <string.h>

extern CFG *g_cfg;

ENGINE *MakeEngine(WORD numOfClusters)
{
	if (strcmp(g_cfg->ENGINE_KIND, "demo") == 0)
		return new DEMO_ENGINE(numOfClusters);
	return new RT_ENGINE(numOfClusters);
}

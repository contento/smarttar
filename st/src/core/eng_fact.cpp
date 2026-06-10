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
#include <demo_eng.h>
#include <cfg.h>

#include <string.h>

// In a demo_dos build the real engine is deactivated and never linked
// (real_dos\rt_eng.obj is excluded from the demo OBJS), so the factory
// must not reference RT_ENGINE here.  See MINI_SMARTTAR_PLAN P1.3/P1.5.
#if !defined(__DEMO__)
#include <rt_eng.h>
#endif

extern CFG *g_cfg;

ENGINE *MakeEngine(WORD numOfClusters)
{
#if defined(__DEMO__)
	return new DEMO_ENGINE(numOfClusters);
#else
	if (strcmp(g_cfg->ENGINE_KIND, "demo") == 0)
		return new DEMO_ENGINE(numOfClusters);
	return new RT_ENGINE(numOfClusters);
#endif
}

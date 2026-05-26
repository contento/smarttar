#ifndef __ENG_FACT_H
#define __ENG_FACT_H

#if !defined(__ST_DEFS_H)
#include <st_defs.h>
#endif

class ENGINE;

//
// Factory Method. Reads g_cfg->ENGINE_KIND ("real" or "demo") and
// returns the matching concrete. Default is "real" (production
// hardware). Callers hold ENGINE* and never know which concrete is
// behind it.
//
ENGINE *MakeEngine(WORD numOfClusters);

#endif // __ENG_FACT_H

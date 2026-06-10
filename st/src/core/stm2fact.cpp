//
// [ STM2FACT.CPP ]
//
// Factory Method for the STM2 hierarchy.  In a demo_dos build only the
// NullStm2 backend is linked, so the factory returns it unconditionally.
// In a real build both backends are present and the choice mirrors the
// engine: g_cfg->IsDemoMode() picks NullStm2, otherwise BankStm2.
// See MINI_SMARTTAR_PLAN P1.3b.
//

#include "stdst.h"

#include <stm2fact.h>
#include <nullstm2.h>

#if !defined(DEMO_DOS)
#include <bankstm2.h>
#include <cfg.h>
extern CFG *g_cfg;
#endif

STM2 *MakeStm2(void)
{
#if defined(DEMO_DOS)
	return new NullStm2;
#else
	if (g_cfg->IsDemoMode())
		return new NullStm2;
	return new BankStm2;
#endif
}

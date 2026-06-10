#ifndef __STM2FACT_H
#define __STM2FACT_H

#if !defined(__STM2_H)
#include <stm2.h>
#endif

//
// Factory for the STM2 backend.  Mirrors MakeEngine(): a demo_dos build
// always gets a NullStm2 (the real backend is not linked); a real build
// picks NullStm2 vs BankStm2 by g_cfg->IsDemoMode().  Callers hold an
// STM2* (g_STM2) and never see the concrete.  Construct via this, never
// `new STM2` (STM2 is abstract).
//
STM2 *MakeStm2(void);

#endif // __STM2FACT_H

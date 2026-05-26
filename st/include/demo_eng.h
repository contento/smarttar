#ifndef __DEMO_ENG_H
#define __DEMO_ENG_H

#if !defined(__ENGINE_H)
#include <engine.h>
#endif

//
// DEMO_ENGINE: fake engine for demo / dev / training use. No hardware
// dependency. Phase 1: stub overrides (booths stay idle). Phase 2 will
// add a Poisson arrival generator driven by util/ini2cfg/demo_engine.ini.
//
// Honors the same concurrency contract as RT_ENGINE: hooks IRQ0 via the
// base class InstallISRs(), writes BoothCluster::_DataPort fields from
// inside OnTimerTick() so downstream consumers can't tell it's synthetic.
//
class DEMO_ENGINE : public ENGINE
{
public:
	DEMO_ENGINE(WORD numOfClusters=1);
	virtual ~DEMO_ENGINE(void);

	virtual void InitHardware(WORD numOfClusters);
	virtual void RecoverState(void);
	virtual void OnTimerTick(WORD cNum, BoothCluster::_DataPort & dataPort);
	virtual void OnTimerEnd(void);
};

#endif // __DEMO_ENG_H

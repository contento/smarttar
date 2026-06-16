#ifndef __RT_ENG_H
#define __RT_ENG_H

#if !defined(__ENGINE_H)
#include <engine.h>
#endif

//
// RT_ENGINE: the real-time engine concrete. Talks to booth-cluster
// hardware via inportb/outportb on APP_PORT_BASE + PO_* offsets, with
// the per-tick polling driven by IRQ0 (NewISR08h on the base). This is
// the production engine.
//
class RT_ENGINE : public ENGINE
{
public:
	RT_ENGINE(WORD numOfClusters=1);
	virtual ~RT_ENGINE(void);

	virtual void InitHardware(WORD numOfClusters);
	virtual BOOL CheckHardware(void);
	virtual void ShutdownHardware(void);
	virtual void RecoverState(void);
	virtual void OnTimerTick(WORD cNum, BoothCluster::_DataPort & dataPort);
	virtual void OnTimerEnd(void);

private:
	// Owned hardware resources. NULL in demo/unit-test builds.
	class STM2 *_stm2;
};

#endif // __RT_ENG_H

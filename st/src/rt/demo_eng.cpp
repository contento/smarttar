//
// [ DEMO_ENG.CPP ]
//
// DEMO_ENGINE: fake engine for demo / dev / training use.
//
// Phase 1 (this file): stub overrides. InitHardware sets the booth
// FSMs to NOPHONE so the rest of the system sees idle booths. The
// per-tick hooks are no-ops; nothing rings, nothing answers. Honors
// the same concurrency contract as RT_ENGINE (hooks IRQ0 via the
// base, writes to BoothCluster::_DataPort), it just writes nothing
// yet -- proving the engine swap end-to-end before Phase 2 wires up
// the Poisson generator.
//

#include "stdst.h"

#include <demo_eng.h>

extern CFG *g_cfg;

DEMO_ENGINE::DEMO_ENGINE(WORD numOfClusters)
	: ENGINE(numOfClusters)
{
	InitHardware(numOfClusters);
	RecoverState();
	InstallISRs();
}

DEMO_ENGINE::~DEMO_ENGINE(void)
{
	// Base dtor handles ISR uninstall and resource cleanup.  Nothing
	// hardware-side to wind down.
}

void DEMO_ENGINE::InitHardware(WORD numOfClusters)
{
	// Trust the configured cluster count -- no hardware to probe.
	// If unset, fall back to the requested numOfClusters.
	if (g_cfg->ACTIVE_CLUSTERS == 0)
		g_cfg->ACTIVE_CLUSTERS = numOfClusters;

	Clusters = new BoothCluster[g_cfg->ACTIVE_CLUSTERS];

	// Mark every booth NOPHONE so EvalToneState short-circuits and
	// the UI shows idle booths.  Phase 2 will replace this with real
	// booth states driven by the Poisson generator.
	for (WORD cNum=0; cNum < g_cfg->ACTIVE_CLUSTERS; cNum++)
	{
		for (WORD bNum=0; bNum < CLUSTER_SIZE; bNum++)
		{
			SetToneFS(cNum, bNum, NOPHONE);
		}
	}
}

void DEMO_ENGINE::RecoverState(void)
{
	// Demo runs are not persistent -- nothing to recover.
}

void DEMO_ENGINE::OnTimerTick(WORD /*cNum*/, BoothCluster::_DataPort & /*dp*/)
{
	// Phase 1 stub.  Phase 2 will synthesize OOD/Answer/DTMF fields
	// here based on the Poisson generator's per-booth schedule.
}

void DEMO_ENGINE::OnTimerEnd(void)
{
	// No general-port hardware to drive.
}

//
// [ RT_ENG.CPP ]
//
// RT_ENGINE: real-hardware concrete. Provides the three Template
// Method hooks (InitHardware / RecoverState / OnTimerTick + OnTimerEnd)
// that drive booth-cluster port I/O. Everything else (FSM dispatch,
// ISR install, accessors) lives on the ENGINE base.
//

#include "stdst.h"

#include <rt_eng.h>

#if !defined(__TEST__)
#include <stm2.h>
extern STM2 *g_STM2;
#endif

extern CFG  *g_cfg;
extern UINT g_pass;

RT_ENGINE::RT_ENGINE(WORD numOfClusters)
	: ENGINE(numOfClusters)
{
	InitHardware(numOfClusters);
	RecoverState();
	InstallISRs();
}

RT_ENGINE::~RT_ENGINE(void)
{
	if (g_cfg->ACTIVATE_RELAY)
	{ // v.219.3
		GeneralPort &= ~GP_CASH; // relay to control locks
		// force for the last time.  We are getting out. v.220
		outportb(APP_PORT_BASE+PO_GENERAL, GeneralPort);
	}
}

void RT_ENGINE::InitHardware(WORD numOfClusters)
{
	if (g_cfg->ACTIVATE_RELAY) // v.219.3
		GeneralPort |= GP_CASH;  // relay to control locks

	//
	// clear the per-cluster general ports.  Just the first cluster is
	// handled into the application. JEAM/gcc
	//
	for (WORD cNum=0; cNum < numOfClusters; cNum++)
		outportb(APP_PORT_BASE+PO_GENERAL+cNum*0x10, 0x00);

	//
	// probe DTMF flag to see if the STB-x is present.  All the bits
	// into the cluster to ONE, else somebody is playing with a MFC
	// phone key !!!
	//
	g_cfg->ACTIVE_CLUSTERS = 0;
	for (cNum=0; cNum < numOfClusters; cNum++)
	{
		if (inportb(APP_PORT_BASE+cNum*PORTS_BY_CLUSTER+PO_DTMF_FLAGS) != 0xFF)
		{
			g_cfg->ACTIVE_CLUSTERS++; // 2.30
		}
	}

	Clusters = new BoothCluster[g_cfg->ACTIVE_CLUSTERS]; // 2.30

	// set active flag
	for (cNum=0; cNum < g_cfg->ACTIVE_CLUSTERS; cNum++)
	{
		for (WORD bNum=0; bNum<CLUSTER_SIZE; bNum++)
		{
			Clusters[cNum].Available = TRUE;
		}
	}
}

//
// Recover booth-cluster state from STM2 after an unclean shutdown.
//
// Warning !!! this is the most critical event, because we are trying
// to recover the system after a problem (Reset, disconnection etc.)
// GCC/gcc.
//
void RT_ENGINE::RecoverState(void)
{
#if !defined(__TEST__)
	// Only called when the factory built an RT_ENGINE (real hardware).
	// DEMO_ENGINE has its own no-op RecoverState override, so no
	// runtime IsDemoMode() guard needed here.
	if (g_STM2->getStatus() != STM2::OK && g_STM2->getStatus() != STM2::GARBAGE)
	{
		// last states of clusters
		g_STM2->get(STM2::BOOTHCLUSTERS, Clusters);
		// force to find it
		for (WORD i=0; i < g_cfg->ACTIVE_CLUSTERS; i++)
			memset(&Clusters[i].Found, FALSE, sizeof(BOOL)*CLUSTER_SIZE);
	}
#endif // !defined(__TEST__)
}

//
// Per-cluster port I/O. Called from ENGINE::NewISR08h once per active
// cluster on every IRQ0 tick. Writes outputs (lock/spy relays) and
// reads inputs (off-hook detection, answer signal, thread C, DTMF) into
// the BoothCluster::_DataPort fields that downstream code consumes.
//
void RT_ENGINE::OnTimerTick(WORD cNum, BoothCluster::_DataPort & dp)
{
	WORD portOfs = cNum * PORTS_BY_CLUSTER;

	// OUT (set states)
	outportb(APP_PORT_BASE+portOfs+PO_LOCK, dp.Lock);
	outportb(APP_PORT_BASE+portOfs+PO_SPY , dp.Spy);

	// IN (poll applications ports)
	dp.OOD             = inportb(APP_PORT_BASE+portOfs+PO_OOD);
	dp.Answer          = inportb(APP_PORT_BASE+portOfs+PO_ANSWER);
	dp.ThreadC         = inportb(APP_PORT_BASE+portOfs+PO_C_THREAD);
	dp.DTMFFlags       = inportb(APP_PORT_BASE+portOfs+PO_DTMF_FLAGS);
	dp.U_DTMFDigits[0] = inport (APP_PORT_BASE+portOfs+PO_DTMF_DIGITS);
	dp.U_DTMFDigits[1] = inport (APP_PORT_BASE+portOfs+PO_DTMF_DIGITS+2);
}

//
// End-of-tick: write the per-system general port (relays etc.).  Run
// once per IRQ0 tick after every cluster has been polled.
//
void RT_ENGINE::OnTimerEnd(void)
{
	outportb(APP_PORT_BASE+PO_GENERAL, GetGeneralPort());
}

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

#if defined(__DEMO__) && !defined(__NO_DONGLE__)
#include <dongle.h>
#endif

#if !defined(__TEST__) && !defined(__DEMO__)
#include <eeprom.h>
#endif

extern CFG  *g_cfg;
extern UINT g_pass;

RT_ENGINE::RT_ENGINE(WORD numOfClusters)
	: ENGINE(numOfClusters), _stm2(NULL)
{
	InitHardware(numOfClusters);
	RecoverState();
	InstallISRs();
}

RT_ENGINE::~RT_ENGINE(void)
{
    ShutdownHardware();
	if (g_cfg->ACTIVATE_RELAY)
	{ // v.219.3
		GeneralPort &= ~GP_CASH; // relay to control locks
		// force for the last time.  We are getting out. v.220
		outportb(APP_PORT_BASE+PO_GENERAL, GeneralPort);
	}
}

BOOL RT_ENGINE::CheckHardware(void)
{
    // STM2 was created in InitHardware; verify its status and
    // check EEPROM version on real hardware.
#if !defined(__TEST__) && !defined(__DEMO__)
    if (_stm2->getStatus() == STM2::NONE)
        return FALSE;

    extern SUPER_APP_INFO g_superAppInfo;
    if (!g_superAppInfo.Attr.NoEEPROM)
    {
        EEPROM eeprom;
        if (!eeprom.isValidVersion())
            return FALSE;
    }
#elif defined(__DEMO__) && !defined(__NO_DONGLE__)
    DONGLE dongle;
    if (!dongle.isThere())
        return FALSE;
#endif
    return TRUE;
}

void RT_ENGINE::ShutdownHardware(void)
{
#if !defined(__TEST__)
    if (_stm2)
    {
        _stm2->logout();
        delete _stm2;
        _stm2 = NULL;
        ::g_STM2 = NULL;
    }
#endif
}

void RT_ENGINE::InitHardware(WORD numOfClusters)
{
#if !defined(__TEST__) && !defined(__DEMO__)
    _stm2 = new STM2;
    _stm2->login();
    ::g_STM2 = _stm2;
#endif

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
void RT_ENGINE::RecoverState(void)
{
#if !defined(__TEST__)
	if (_stm2 && _stm2->getStatus() != STM2::OK && _stm2->getStatus() != STM2::GARBAGE)
	{
		// last states of clusters
		_stm2->get(STM2::BOOTHCLUSTERS, Clusters);
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

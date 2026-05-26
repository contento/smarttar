//
// [ RT_ISR.CPP ]
//
// Static ISR handlers for the ENGINE base.  NewISR08h is the main
// per-tick orchestrator: it dispatches the per-cluster I/O and the
// end-of-tick work to the virtual hooks (OnTimerTick / OnTimerEnd),
// so RT_ENGINE and DEMO_ENGINE see the same call site but produce
// different behavior.
//

#include "stdst.h"

#include "engine.h"

extern UINT _pass;
extern CFG 	*g_cfg;

#ifdef DOSX286
REALPTR   ENGINE::OldRealIV08h = (REALPTR)0;
PIHANDLER ENGINE::OldProtIV08h = (PIHANDLER)0;
#else
void interrupt (far *ENGINE::OldIV08h)(...) = 0;
#endif

#ifdef DOSX286
void interrupt far ENGINE::NewISR08h(REGS_BINT)
#else
void interrupt far ENGINE::NewISR08h(...)
#endif
{
    static BOOL isInside = FALSE;
    static WORD cycles = 0;

	if (isInside)
	{
		// go away ...
		_AL = EOI;
		outportb(PIC_PORT, _AL);
		_ES = 0xB0B0; // JCAR/gcc trigger GP
	}
	else
	{
		isInside = TRUE;
		// --- to the own code
		static WORD cNum, bNum;
		for (cNum=0; cNum<g_cfg->ACTIVE_CLUSTERS; cNum++) // 2.30
		{
			// Per-cluster I/O: RT reads hardware ports into dp; DEMO
			// synthesizes the same fields.  Either way, downstream
			// EvalTone/EvalPulseState read from dp identically.
			BoothCluster::_DataPort & dataPort = pThis->GetDataPort(cNum);
			pThis->OnTimerTick(cNum, dataPort);

			// for each booth increment the counters and evaluate
			for (bNum=0; bNum<CLUSTER_SIZE; bNum++)
			{
				pThis->IncStateCount(cNum, bNum);
				pThis->IncDialCount(cNum, bNum);

				// simula !!!
				if (pThis->GetSimula(cNum, bNum))
				{
					dataPort.OOD |= (1 << bNum);
				}

				// lookout !!!
				// EvalToneState takes a higher priority over EvalPulseState,
				// !!!
				pThis->EvalToneState (cNum, bNum);
				pThis->EvalPulseState(cNum, bNum);
			}
		}

		// End-of-tick: RT writes the general port (relays etc.); DEMO no-ops.
		pThis->OnTimerEnd();

		isInside = FALSE;
		// --- end own code
		//
		// check for 65536 pulses at a rate of 1.19 Mhz,
		// equivalent to 18.2 times/second.
		//
		//
		cycles += BOOTH_PIT_DIVISOR; // keep track of all cycles
		if (_FLAGS & 0x01) // check carry flag
		{
#ifdef DOSX286

			DosChainToRealIntr(OldRealIV08h);
#else
			(*OldIV08h)();    // call old ISR
#endif
		}
		else
		{
			_AL = EOI;
			outportb(PIC_PORT, _AL);
		}
	}
}

//
// --- Other ISR's ----------------------------------------------------------
//
#ifdef DOSX286
REALPTR   ENGINE::OldRealIV09h = (REALPTR)0;
PIHANDLER ENGINE::OldProtIV09h = (PIHANDLER)0;
#else
void interrupt (far *ENGINE::OldIV09h)(...) = 0;
#endif

#ifdef DOSX286
void interrupt far ENGINE::NewISR09h(REGS_BINT)
#else
void interrupt far ENGINE::NewISR09h(...)
#endif
{
	static const UINT KBD_DATA_PORT = 0x60;
	static const BYTE NUMLK_CODE = 0x45;
	static const BYTE CTRL_CODE  = 0x1D;
	static BOOL ctrlKey = FALSE;
	static BYTE keyCode;
	keyCode = inportb(KBD_DATA_PORT);
	if (keyCode == NUMLK_CODE && ctrlKey)
	{
		_AL = EOI;
		outportb(PIC_PORT, _AL);
	}
	else
	{
		ctrlKey = (keyCode == CTRL_CODE);
#ifdef DOSX286
		DosChainToRealIntr(OldRealIV09h);
#else
        (*OldIV09h)();    // call old ISR
#endif
    }
}

#ifdef DOSX286
REALPTR   ENGINE::OldRealIV23h = (REALPTR)0;
PIHANDLER ENGINE::OldProtIV23h = (PIHANDLER)0;
#else
void interrupt (far *ENGINE::OldIV23h)(...) = 0;
#endif

#ifdef DOSX286
REALPTR   ENGINE::OldRealIV1Bh = (REALPTR)0;
PIHANDLER ENGINE::OldProtIV1Bh = (PIHANDLER)0;
#else
void interrupt (far *ENGINE::OldIV1Bh)(...) = 0;
#endif

#ifdef DOSX286
REALPTR   ENGINE::OldRealIV24h = (REALPTR)0;
PIHANDLER ENGINE::OldProtIV24h = (PIHANDLER)0;
#else
void interrupt (far *ENGINE::OldIV24h)(...) = 0;
#endif

#ifdef DOSX286
void interrupt far ENGINE::NewISR23h(REGS_BINT)
#else
void interrupt far ENGINE::NewISR23h(...)
#endif
{
}

#ifdef DOSX286
void interrupt far ENGINE::NewISR1Bh(REGS_BINT)
#else
void interrupt far ENGINE::NewISR1Bh(...)
#endif
{
}

#ifdef DOSX286
void interrupt far ENGINE::NewISR24h(REGS_BINT regs)
{
    regs.ax &= 0xFF00;
#else
void interrupt far ENGINE::NewISR24h(...)
{
    _AX &= 0xFF00;
#endif
}

void ENGINE::SetPITRate(WORD divisor)
{
    // the programming word at port 043h has the following mean:
    //		  SC1, SC0 =  00: select counter 0.
    //  	  RL1, RL2 =  11: read/load LSB first, then MSB counter.
    //		M2, M1, M0 = 011: mode 3, square wave generator.
    //		       BCD =   0: binary counter 16 bits.
    _AL = 0x36; // 00110110b, program PIT
    outportb(0x43, _AL);
    // load counter divisor for counter 0 ...
    _AX = divisor;
    outportb(0x40, _AL);     // LSB
    outportb(0x40, _AH);     // MSB
}

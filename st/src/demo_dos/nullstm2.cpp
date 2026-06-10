//
// [ NULLSTM2.CPP ]
//
// NullStm2: RAM-backed STM2 backend for the demo_dos build.  Implements
// the hardware seam (check / read / write) against an in-RAM buffer the
// size of one NVRAM bank -- enough to hold the whole STM2::Data image
// (the STM2 overflow #error in stm2.h guarantees Data fits in one bank).
// No persistence across runs; the generic record logic in core/stm2.cpp
// runs unchanged on top of this.  See MINI_SMARTTAR_PLAN P1.3b.
//

#include "stdst.h"

#include <nullstm2.h>

#include <string.h>

NullStm2::NullStm2(void)
{
	// check() is virtual -- run it from the derived ctor so this backend's
	// override (a clean, healthy RAM bank) is the one that takes effect.
	check();
}

NullStm2::~NullStm2(void)
{
}

void NullStm2::check(void)
{
	memset(m_ram, 0, sizeof(m_ram));
	banks         = 1;
	status        = OK;
	receiptsFront = 0;
	receiptsRear  = 0;
	receiptsCount = 0;
}

WORD NullStm2::write(WORD offset, const void *buffer, WORD bufSize)
{
	if ((long)offset + (long)bufSize > (long)sizeof(m_ram))
		return 0;
	memcpy(m_ram + offset, buffer, bufSize);
	return bufSize;
}

WORD NullStm2::read(WORD offset, void *buffer, WORD bufSize)
{
	if ((long)offset + (long)bufSize > (long)sizeof(m_ram))
		return 0;
	memcpy(buffer, m_ram + offset, bufSize);
	return bufSize;
}

#ifndef __NULLSTM2_H
#define __NULLSTM2_H

#if !defined(__STM2_H)
#include <stm2.h>
#endif

//
// NullStm2: RAM-backed STM2 backend for the demo_dos build.  No hardware,
// no persistence across runs -- in demo the DB files (RX.*) are the real
// store.  Reports a healthy single bank with status OK so no recovery /
// garbage / bad-shutdown paths fire.
//
class NullStm2 : public STM2
{
public:
	NullStm2 (void);
	virtual ~NullStm2(void);

protected:
	virtual void check(void);
	virtual WORD write(WORD offset, const void *buffer, WORD bufSize);
	virtual WORD read (WORD offset,       void *buffer, WORD bufSize);

private:
	char m_ram[STM2_BANKSIZE];
};

#endif // __NULLSTM2_H

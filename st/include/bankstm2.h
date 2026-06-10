#ifndef __BANKSTM2_H
#define __BANKSTM2_H

#if !defined(__STM2_H)
#include <stm2.h>
#endif

//
// BankStm2: production STM2 backend.  Battery-backed NVRAM accessed as
// 1-2 banks of STM2_BANKSIZE bytes via port I/O (ADDR_PORT / *_BANK_PORT).
// real_dos -- not linked in the demo_dos build.
//
class BankStm2 : public STM2
{
public:
	BankStm2 (void);
	virtual ~BankStm2(void);

protected:
	virtual void check(void);
	virtual WORD write(WORD offset, const void *buffer, WORD bufSize);
	virtual WORD read (WORD offset,       void *buffer, WORD bufSize);

private:
	WORD fill(char c);
	BYTE in (WORD addressPort, WORD offset, WORD dataPort);
	void out(WORD addressPort, WORD offset, WORD dataPort, const unsigned char byte);
};

#endif // __BANKSTM2_H

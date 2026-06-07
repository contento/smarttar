// trace/test.cpp -- Intentional crash test for Phar Lap stack traces
//
// PURPOSE: Deliberately dereferences a NULL pointer to trigger an
// exception. When run under Phar Lap DOS extender, this produces
// a stack trace that helps verify the debugger/trace infrastructure
// is working correctly.
//
// EXPECTED: Segmentation fault / access violation with Phar Lap
// stack trace output. This test is SUPPOSED to crash.
//
// NOTE: Only works with DOSX286 (Phar Lap 286 extender).
//
#include <iostream.h>

#ifndef DOSX286
#error This works only with Phar Lap
#endif

void main()
{
	char * s = NULL;
    s[0] = 'a';  // intentional NULL dereference
	cout << "Msg: " << s << endl;
}


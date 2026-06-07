//
// [ MUTEX.CPP ]
//

#include "stdst.h"
// __emit__(0x0F, 0xBA, 0x34, 0x00); // btr dword ptr[si], 0
// __emit__(0x0F, 0xBA, 0x2C, 0x00); // bts dword ptr[si], cx

//
// INVARIANT (read before reusing this primitive):
//   The ctor busy-waits on a `btr` bit-test-and-reset spin with NO timeout
//   and NO interrupt disable; the dtor releases via `bts`. Consequences:
//     - It is NOT safe to take a lock from an ISR while the main thread may
//       hold it: the ISR would spin forever (IF stays set, so the main thread
//       can still run, but if the ISR pre-empted the holder on a single CPU
//       and the holder cannot make progress, it deadlocks). The RT receipt /
//       booth-cluster ISR paths must respect this -- see ISR_VOLATILE_NOTES.
//     - s_mutex is a single shared word per mutex class (one critical section
//       per type, not per instance); nesting the same class re-enters the spin
//       and self-deadlocks.
//     - There is no fairness or priority; longest-waiter is not guaranteed.
//
#define DEFINE_MUTEX(x) 					\
unsigned x##Mutex::s_mutex = 1;			    	\
x##Mutex::x##Mutex()             			\
{                                       	\
	_SI = (unsigned int)&s_mutex;			\
	do 										\
	{   									\
		__emit__(0x0F, 0xBA, 0x34, 0x00); 	\
	} 										\
	while (!(_FLAGS & 0x01));				\
}                                       	\
											\
x##Mutex::~x##Mutex()         				\
{                                       	\
	_SI = (unsigned int)&s_mutex;			\
	__emit__(0x0F, 0xBA, 0x2C, 0x00); 		\
}

DEFINE_MUTEX(RTReceiptQueue);
DEFINE_MUTEX(RTBoothClusters);

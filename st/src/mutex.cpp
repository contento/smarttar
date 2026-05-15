//
// [ MUTEX.CPP ]
//

#include "stdst.h"
// __emit__(0x0F, 0xBA, 0x34, 0x00); // btr dword ptr[si], 0
// __emit__(0x0F, 0xBA, 0x2C, 0x00); // bts dword ptr[si], cx

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

#include <iostream.h>

void main()
{
	static long s_mutexes = 1;

	_SI = (unsigned int)&s_mutexes;
	do
	{
		__emit__(0x0F, 0xBA, 0x34, 0x00); // btr dword ptr[si], 0
	}
	while (!(_FLAGS & 0x01));

	_SI = (unsigned int)&s_mutexes;
	__emit__(0x0F, 0xBA, 0x2C, 0x00); // bts dword ptr[si], cx

	_SI = (unsigned int)&s_mutexes;
	do
	{
		__emit__(0x0F, 0xBA, 0x34, 0x00); // btr dword ptr[si], 1
	}
	while (!(_FLAGS & 0x01));

}

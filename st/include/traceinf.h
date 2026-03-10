#ifndef __TRACEINF_H
#define __TRACEINF_H

#if !defined(__ST_DEFS_H)
#include <st_defs.h>
#endif

struct TraceInfo
{
	TraceInfo();

	static BOOL    	s_bTest;			// enviroment STTEST=1
	static BOOL	   	s_bDevelopment;	// my own thing == ROSINA
	static WORD    	s_nSemWait; 		// To know how many times the engine wait
	static DWORD   	s_nAvailableRAM;
	static WORD   	s_nPass; 			// general check point
};

#endif // __TRACEINF_H

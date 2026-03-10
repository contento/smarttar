#ifndef __INFO_H
#define __INFO_H

#if !defined(__ST_DEFS_H)
#include <st_defs.h>
#endif

struct APP_INFO
{
	UINT   MaxClusters; 		// Maximum allowed number of clusters
	TITLE  Title;       		// for the VIEW. e.g. "SmartTar ST32"
	SERIAL_NUMBER Serial;      	// Application serial
	SERIAL_NUMBER ShortSerial; 	// Short serial
}
;

struct SUPER_APP_INFO
{
	KEY Key;        // to access EXE buffer
	struct
	{
		WORD Serialized :1; // Is already serialized or is a test
		WORD STPro      :1; // Is SmartTar Pro ?
		WORD EDA        :1; // this EDA, @#$%@!
		WORD Outside    :1; // Local copy ?
		WORD NoEEPROM   :1; // EEPROM activated.
	} Attr;
	APP_INFO Data;
};

#endif // __INFO_H

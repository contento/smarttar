/*	WORD1A.H (WORD) - Dictionary tutorial, C to C++.		*/
/*	COPYRIGHT (C) 1990-1992.  All Rights Reserved.			*/
/*	Zinc Software Incorporated.  Pleasant Grove, Utah  USA	*/

typedef struct
{
	char string[64];
} SYNONYM, ANTONYM;

typedef struct WORD_STRUCT
{
	char string[64];
	char definition[1024];
	int synonymCount;
	SYNONYM synonym[10];
	int antonymCount;
	ANTONYM antonym[10];
} WORD;

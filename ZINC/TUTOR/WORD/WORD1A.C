/*	WORD1A.C (WORD) - Dictionary tutorial, C to C++.		*/
/*	COPYRIGHT (C) 1990-1992.  All Rights Reserved.			*/
/*	Zinc Software Incorporated.  Pleasant Grove, Utah  USA	*/

#include <stdio.h>
#include <string.h>
#include "word1a.h"
#if !defined(TRUE)
	const int TRUE = 1;
	const int FALSE = 0;
#endif

#if (defined(_MSC_VER) && !defined(WIN32)) || defined(__IBMCPP__) 
#	define	stricmp	_strcmpi
#elif defined(hpux)
#	define stricmp strcasecmp
#elif defined(_IBM_RS6000) || defined(sun)
#include <ctype.h>
static int stricmp(const char *string1, const char *string2)
{
	char char1, char2;
	for (;;)
	{
		char1 = *string1; char2 = *string2;
		if (islower(char1))
			char1 = toupper(char1);
		if (islower(char2))
			char2 = toupper(char2);
		if (char1 != char2 || !char1)
			break;
		string1++; string2++;
	}
	return char1 - char2;
}
#endif

static void PrintWord(WORD *word)
{
	int i;

	/* Print out the word information. */
	printf("%s - %s\n", word->string, word->definition);
	printf("\tsynonyms - ");
	for (i = 1; i <= word->synonymCount; i++)
		printf((i < word->synonymCount) ? "%s, " : "%s.\n", word->synonym[i-1].string);
	printf("\tantonyms - ");
	for (i = 1; i <= word->antonymCount; i++)
		printf((i < word->antonymCount) ? "%s, " : "%s.\n", word->antonym[i-1].string);
}

static void ReadWord(FILE *file, WORD *word)
{
	char token[64];

	/* Read the word. */
	fscanf(file, "%s", word->string);
	if (!strcmp("word:", word->string))
		fscanf(file, "%s", word->string);

	/* Read the definition. */
	fscanf(file, "%s", word->definition);	/* The '-' character. */
	fscanf(file, "%s", word->definition);
	for (fscanf(file, "%s", token); strcmp("synonyms:", token);
		fscanf(file, "%s", token))
	{
		strcat(word->definition, " ");
		strcat(word->definition, token);
	}

	/* Read in the synonyms. */
	word->synonymCount = 0;
	for (fscanf(file, "%s", token); strcmp("antonyms:", token);
		fscanf(file, "%s", token))
		{
			strcpy(word->synonym[word->synonymCount].string, token);
			word->synonymCount++;
		}

	/* Read in the antonyms. */
	word->antonymCount = 0;
	for (fscanf(file, "%s", token); !feof(file) && strcmp("word:", token);
		fscanf(file, "%s", token))
		{
			strcpy(word->antonym[word->antonymCount].string, token);
			word->antonymCount++;
		}
}

main(int argc, char *argv[])
{
	int validWord;
	WORD word;
	FILE *file;

	/* Make sure there is a word. */
	if (argc < 2)
	{
		printf("Usage: word1a <word>\n");
		return(0);
	}

	/* Make sure the dictionary file exists. */
	file = fopen("word.dct", "rt");
	if (!file)
	{
		printf("The dictionary file 'WORD.DCT' could not be found.\n");
		return(0);
	}

	/* Search for a word match. */
	validWord = FALSE;
	while (!feof(file))
	{
		ReadWord(file, &word);
		if (!stricmp(word.string, argv[1]))
		{
			PrintWord(&word);
			validWord = TRUE;
			break;
		}
	}
	if (!validWord)
		printf("The word \"%s\" could not be found.\n", argv[1]);

	/* Close the file. */
	fclose(file);
	return(0);
}

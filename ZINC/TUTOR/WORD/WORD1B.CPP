//	WORD1B.CPP (WORD) - Dictionary tutorial, C to C++.
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA
//  May be freely copied, used and distributed.

#include <stdio.h>
#include <string.h>
#include <ui_env.hpp>
extern int ui_stricmp(const char *a, const char *b);
extern char * ui_strdup(const char *a);
#include "list.hpp"
#include "word1b.hpp"

main(int argc, char *argv[])
{
	// Make sure there is a word.
	if (argc < 2)
	{
		printf("Usage: word1b <word>\n");
		return(0);
	}

	// Create the dictionary.
	DICTIONARY dictionary;
	if (!dictionary.found)
	{
		printf("The dictionary file 'WORD.DCT' could not be found.\n");
		return(0);
	}

	// Search for a word match.
	D_WORD *word = dictionary.Get(argv[1]);
	if (word)
		word->Print();
	else
		printf("The word \"%s\" could not be found.\n", argv[1]);

	// Cleanup is automatic.
	return (0);
}

D_WORD::D_WORD(FILE *file) : antonymList(), synonymList()
{
	char token[64];

	// Read the word.
	char a_word[64];
	fscanf(file, "%s", a_word);
	if (!strcmp("word:", a_word))
		fscanf(file, "%s", a_word);
	string = ui_strdup(a_word);

	// Read the definition.
	char a_definition[1024];
	fscanf(file, "%s", a_definition);	// The '-' character.
	fscanf(file, "%s", a_definition);
	for (fscanf(file, "%s", token); strcmp("synonyms:", token);
		fscanf(file, "%s", token))
	{
		strcat(a_definition, " ");
		strcat(a_definition, token);
	}
	definition = ui_strdup(a_definition);

	// Read in the synonyms.
	for (fscanf(file, "%s", token); strcmp("antonyms:", token);
		fscanf(file, "%s", token))
		synonymList.Add(new WORD(token));

	// Read in the antonyms.
	for (fscanf(file, "%s", token); !feof(file) && strcmp("word:", token);
		fscanf(file, "%s", token))
		antonymList.Add(new WORD(token));
}

void D_WORD::Print(void)
{
	// Print out the word information.
	printf("%s - %s\n", string, definition);
	printf("\tsynonyms - ");
	for (WORD *word = synonymList.First(); word; word = word->Next())
		printf(word->Next() ? "%s, " : "%s.\n", word->string);
	printf("\tantonyms - ");
	for (word = antonymList.First(); word; word = word->Next())
		printf(word->Next() ? "%s, " : "%s.\n", word->string);
}

DICTIONARY::DICTIONARY(void) : D_WORD_LIST()
{
	// Open the dictionary file and read in the words.
	FILE *file = fopen("word.dct", "rt");
	if (file)
	{
		found = TRUE;
		while (!feof(file))
			UI_LIST::Add(new D_WORD(file));
	}
	else
		found = FALSE;
	fclose(file);
}

//	Zinc Interface Library - Z_UTILS.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include <stdio.h>
#include <stdarg.h>
#if !(defined(__GNUC__) && defined(__DVX__))
#include <locale.h>
#endif
#include <string.h>
#include <ctype.h>
#include "ui_gen.hpp"
#if defined(_SUNOS4)
#	include <malloc.h>
#endif

#if defined(__GNUC__) && defined(__DVX__)
static struct lconv _lconv = {
	".", ",", "\3", "USD", "$", ".", ",", "\3",
	"", "-", 2, 2, 1, 0, 1, 0, 0, 0
};

struct lconv *localeconv( void )
{
	return &_lconv;
}

char *setlocale( int category, const char *locale )
{
}

#endif


char *ui_strdup(const char *srcString)
{
	// strdup using new instead of malloc.
//	if (!srcString || srcString[0] == '\0')		// Bug.Feature
	if (!srcString)
		return (NULL);
	char *destString = new char[strlen(srcString) + 1];
	strcpy(destString, srcString);
	return (destString);
}

int ui_strlen(const char *string)
{
	// Return the length of a string.
	return (string ? strlen(string) : 0);
}

void ui_strrepc(char *string, int c, int repc)
{
	if (!string)
		return;

	// Replace a character (c) in a string to a new character (repc).
	while (*string)
	{
		if (*string == c)
			*string = repc;
		string++;
	}
}

void ui_strstrip(char *string, int c)
{
	if (!string)
		return;

	// strip all occurances of c out of string
	char *src = string;
	char *dest = string;
	do {
		while (*src == c)
			src++;
	} while ((*dest++ = *src++) != 0);
}

char *ui_itoa(int value, char *string, int radix)
{
	switch(radix)
	{
		case 16:
			::sprintf(string, "%x", value);
			break;
		case 10:
			::sprintf(string, "%d", value);
			break;
	}
	return string;
}


//	ui_WildStrcmp() is a strcmp() that handles the * and ? wildcards
//	in pattern.
int ui_WildStrcmp(char *str, char *pattern)
{
	for ( ;  *str && (*str == *pattern || *pattern == '?' || *pattern == '*');
			str++, pattern++)
		if (*pattern == '*') {
			// Compress multiple astrisks, eg: "**".
			while (*++pattern == '*')
				;	// Update pattern count.
			if (!*pattern)
				return(0);		// pattern ended in *.
			char *nextTest = str;
			do {
				nextTest = strchr(nextTest, *pattern);
				if (!nextTest)
				{
					pattern--;
					break;
				}
				if (!ui_WildStrcmp(nextTest, pattern)) {
					str = nextTest;
					break;
				}
				nextTest++;
			} while (nextTest);
		}
	while (*pattern == '*')
		pattern++;
	return *str - *pattern;
}

int ui_stricmp(const char *a, const char *b)
{
	char la, lb;

	for (;;) {
		la = *a; lb = *b;
		if (islower(la)) la = toupper(la);
		if (islower(lb)) lb = toupper(lb);
	if (la != lb || !la) break;
		a++; b++;
	}
	return la - lb;
}

int ui_strnicmp(const char *a, const char *b, int n)
{
	char la, lb;

	for (;;) {
		la = *a; lb = *b;
		if (islower(la)) la = toupper(la);
		if (islower(lb)) lb = toupper(lb);
	if (la != lb || !la || !n) break;
		a++; b++; n--;
	}
	if (n)
		return la - lb;
	return(0);
}

void ui_strupr(char *a)
{
	while (*a)
	{
		if (islower(*a)) *a = toupper(*a);
		a++;
	}
}

void ui_strlwr(char *a)
{
	while (*a)
	{
		if (isupper(*a)) *a = tolower(*a);
		a++;
	}
}

#if defined(_SUNOS4)
void *memmove(void *s1, const void *s2, size_t n)
{
	char *tmp = malloc(n);
	memcpy(tmp, s2, n);
	memcpy(s1, tmp, n);
	free (tmp);
	return(s1);
}
#endif

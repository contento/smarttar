#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#include "ui_gen.hpp"
#define LENGTHOF(x)	(sizeof(x)/sizeof((x)[0]))

UI_STORAGE *src, *dst;

static void CopyDir(void)
{
	UI_STORAGE_OBJECT *o = new UI_STORAGE_OBJECT(*src, ".", 0, 0);
	if (o->objectError != 0) {
		delete o;
		return;
	}
	UI_STATS_INFO *s = o->Stats();
	int dirlen = (int)s->size;;
	char *objs = new char[dirlen];
	char *ptr = objs;
	delete o;

	for (char *entry = src->FindFirstObject("*"); entry; entry = src->FindNextObject()) {
		if ((strcmp(entry, ".") == 0 || strcmp(entry, "..") == 0)) continue;
		(void) strcpy(ptr, entry);
		ptr += strlen(ptr) + 1;
	}
	*ptr = '\0';
	for (ptr = objs; *ptr; ptr += strlen(ptr)+1) {
		if (src->ChDir(ptr) >= 0) {
			dst->MkDir(ptr);
			dst->ChDir(ptr);
			CopyDir();
			src->ChDir("..");
			dst->ChDir("..");
		} else {
			UI_STORAGE_OBJECT *sobj =
				new UI_STORAGE_OBJECT(*src, ptr, 0, UIS_READ);
			UI_STORAGE_OBJECT *dobj =
				new UI_STORAGE_OBJECT(*dst, ptr, sobj->objectID, UIS_CREATE|UIS_READWRITE);
			char *buff = new char[16384];
			int len;

			while ((len = sobj->Load(buff, 16384, 1)) > 0)
				dobj->Store(buff, len, 1);

#if defined(ZIL_DELETE)
			delete []buff;
#else
			delete [16384]buff;
#endif
			delete sobj;
			delete dobj;
		}
	}
#if defined(ZIL_DELETE)
	delete []objs;
#else
	delete [dirlen]objs;
#endif
}

static void usage(void)
{
#ifdef DOGC
	printf("usage: DOGC <filename>\n");
#else
	printf("usage: cppath <filename> <objectpath> <filename> <objectpath>\n");
#endif
	exit(1);
}

int main(int arDOGC, char **argv)
{
	UI_STORAGE::cacheSize = 240;
	arDOGC--; argv++;
#ifdef DOGC
	if (arDOGC != 1) usage();
#else
	if (arDOGC != 4) usage();
#endif
	src = new UI_STORAGE(*argv, UIS_READ);
	if (src->storageError != 0) {
		errno = src->storageError;
		perror(*argv);
		exit(2);
	}
#ifdef DOGC
	dst = new UI_STORAGE("tmp.$$$", UIS_READWRITE|UIS_CREATE);
#else
	argv++;
	if (src->ChDir(*argv) < 0) {
	}
	argv++;
	dst = new UI_STORAGE(*argv, UIS_READWRITE|UIS_OPENCREATE);
#endif
	if (dst->storageError != 0) {
		errno = src->storageError;
		perror(*argv);
		delete src;
		delete dst;
		exit(2);
	}
#ifndef DOGC
	argv++;
	if (dst->ChDir(*argv) < 0) {
	}
#endif
	CopyDir();
	delete src;
#ifdef DOGC
	dst->SaveAs(*argv, 9);
#else
	dst->Save(1);
#endif
	delete dst;
	return 0;
}

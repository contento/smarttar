#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>

#include "ui_gen.hpp"

static int RmInDir(UI_STORAGE *f)
{
	UI_STORAGE_OBJECT *o = new UI_STORAGE_OBJECT(*f, ".", 0, 0);
	if (o->objectError != 0) {
		delete o;
		return -1;
	}
	UI_STATS_INFO *s = o->Stats();
	char *buff = new char[(int)s->size];
	char *ptr = buff;
	delete o;

	for (char *entry = f->FindFirstObject("*"); entry; entry = f->FindNextObject()) {
		if ((strcmp(entry, ".") == 0 || strcmp(entry, "..") == 0)) continue;
		(void) strcpy(ptr, entry);
		ptr += strlen(ptr) + 1;
	}
	*ptr = '\0';
	for (ptr = buff; *ptr; ptr += strlen(ptr)+1) {
		if (f->ChDir(ptr) >= 0) {
			if (RmInDir(f) < 0 || f->ChDir("..") < 0 ||
			    f->RmDir(ptr) < 0) {
				delete buff;
				return -1;
			}
		} else if (f->DestroyObject(ptr) < 0) {
			delete buff;
			return -1;
		}
	}
	delete buff;
	return 0;
}

static int RRmDir(UI_STORAGE *f, char *name)
{
	if (f->ChDir(name) < 0) return -1;
	if (RmInDir(f) < 0) return -1;
	if (f->ChDir("~") < 0) return -1;
	if (f->RmDir(name) < 0) return -1;
	return 0;
}

static void usage(void)
{
	printf("usage: rrmdir <filename> <pathname>\n");
	exit(1);
}

int main(int argc, char **argv)
{
	UI_STORAGE *f;

	if (argc != 3) usage();
	f = new UI_STORAGE(argv[1], UIS_READWRITE);
	RRmDir(f, argv[2]);
	f->Save(1);
	delete f;
	return 0;
}

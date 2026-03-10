#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#include "ui_gen.hpp"
#define LENGTHOF(x)	(sizeof(x)/sizeof((x)[0]))
#define INDENT		4

static int cflag = 0;
static int eflag = 0;

static void PrintInfo(UI_STATS_INFO *s, char *name, int spaces)
{
	if (eflag) {
		printf("%4d ", s->inum);
		printf("%c ", (s->useCount & 0x8000 ? 'd' : '-'));
		printf("%4d ", s->useCount & ~0x8000);
	}
	char *buff = cflag ? ctime((time_t *)&s->createTime) :
			     ctime((time_t *)&s->modifyTime);
	buff[24] = '\0';
	printf("%s ", &buff[4]);
	printf("%7ld ", s->size);
	printf("%*s", eflag ? spaces : spaces+INDENT, "");
	printf("%-10s", name);
	printf("\n");
}

static void ListDir(UI_STORAGE *f, int spaces)
{
	for (char *entry = f->FindFirstObject("*"); entry; entry = f->FindNextObject())
	{
		if (eflag || (strcmp(entry, ".") != 0 && strcmp(entry, "..") != 0))
		{
			UI_STORAGE_OBJECT *o = new UI_STORAGE_OBJECT(*f, entry, 0, 0);
			UI_STATS_INFO *s = o->Stats();
			PrintInfo(s, entry, spaces);
			delete o;

			if ((s->useCount & 0x8000) && (strcmp(entry, ".") != 0) &&
		    	(strcmp(entry, "..") != 0))
			{
				char hold[64];
				strcpy(hold, entry);
				f->ChDir(hold);
				ListDir(f, spaces+INDENT);
				f->ChDir("..");
				entry = f->FindFirstObject("*");
				while (strcmp(entry, hold) != 0)
					entry = f->FindNextObject();
			}
		}
	}
}

static void usage(void)
{
	printf("usage: browse [-ce] <filename>\n");
	exit(1);
}

int main(int argc, char **argv)
{
	UI_STORAGE *f;

	argc--; argv++;
	if (argc <= 0) usage();
	if (**argv == '-') {
		(*argv)++;
		while (**argv) {
			if (**argv == 'c') cflag = 1;
			else if (**argv == 'e') eflag = 1;
			else usage();
			(*argv)++;
		}
		argc--; argv++;
	}
	if (argc != 1) usage();
	f = new UI_STORAGE(*argv, UIS_READ);
	if (f->storageError != 0) {
		errno = f->storageError;
		perror(*argv);
		exit(2);
	}
	if (!eflag) {
		UI_STORAGE_OBJECT *o = new UI_STORAGE_OBJECT(*f, ".", 0, 0);
		UI_STATS_INFO *s = o->Stats();
		PrintInfo(s, "ROOT", -INDENT);
		delete o;
	}
	ListDir(f, 0);
	delete f;
	return 0;
}

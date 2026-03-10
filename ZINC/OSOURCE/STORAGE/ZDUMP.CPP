#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

#include "ui_gen.hpp"
#define LENGTHOF(x)	(sizeof(x)/sizeof((x)[0]))

static void Dump(UI_STORAGE_OBJECT *o)
{
	int i, j, k;
	UCHAR buff[16];

	j = 0;
	while ((k = o->Load(buff, sizeof(buff[0]), LENGTHOF(buff))) > 0) {
		printf("%04x  ", j);
		for (i=0; i < LENGTHOF(buff); i++) {
			if (i == (LENGTHOF(buff) >> 1)) printf("  ");
			if (i < k)
				printf("%02x ", buff[i]);
			else
				printf("   ");
		}
		printf("   |");
		for (i=0; i < LENGTHOF(buff); i++) {
			if (i == (LENGTHOF(buff) >> 1)) printf("|");
			if (i < k && isprint(buff[i]))
				printf("%c", buff[i]);
			else	
				printf(" ");
		}
		printf("|\n");
		j += LENGTHOF(buff);
	}
}

static void usage(void)
{
	printf("usage: zdump <filename> <objectname>\n");
	exit(0);
}

int main(int argc, char **argv)
{
	UI_STORAGE *f;

#if 0
	if (argc != 4) usage();
	f = new UI_STORAGE(argv[1], UIS_READ);
	f->ChDir(argv[2]);
	UI_STORAGE_OBJECT *o = new UI_STORAGE_OBJECT(*f, argv[3], 0, UIS_READ);
#else
	if (argc != 3) usage();
	f = new UI_STORAGE(argv[1], UIS_READ);
	UI_STORAGE_OBJECT *o = new UI_STORAGE_OBJECT(*f, argv[2], 0, UIS_READ);
#endif
	Dump(o);
	delete o;
	delete f;
	return 0;
}

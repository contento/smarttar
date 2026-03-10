#include <iostream.h>

#ifndef DOSX286
#error This works only with Phar Lap
#endif

void main()
{
	char * s = NULL;
    s[0] = 'a';
	cout << "Msg: " << s << endl;
}


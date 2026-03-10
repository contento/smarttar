//
// [ TESPWD.CPP ]
//
#include <iostream.h>
#include <string.h>
#include <st_util.h>

extern _stklen = 0x4000; // ~%^%&$#@!&*, _stklen GRRR ...

char *errorMemory;

int main(void)
{
	char password[80];
	int  i, n;

	cout << "Password: ";
	cin.getline(password, 80);
	n = strlen(password)+1; // include '\0'
	_Encrypt(password, n, 15);

	cout << endl;
	for (i = 0; i < n; i++)
	{
		cout << "\\x" << hex << int(password[i]);
	}

	return 0;
}


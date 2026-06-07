//
// [ TEST.CPP ]
//
#include <iostream.h>
#include <string.h>
#include <util.h>

int main(void)
{
    char s[0x40];
    cout << "String: ";
    cin.getline(s, sizeof(s));
    int offset;
    cout << "Seed: " ;
    cin >> offset;
    int len = strlen(s)+1;
    _Encrypt(s, len, offset);
    cout << "Encrypted: ";
    cout.setf(ios::hex|ios::uppercase);
    for (int i=0; i < len; i++)
        cout << "\\x" << (int)s[i];
    return 0;
}
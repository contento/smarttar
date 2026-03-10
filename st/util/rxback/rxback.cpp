#include <iostream.h>
#include <fstream.h>
#include <io.h>
#include <string.h>
#include <sys\stat.h>

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		cerr << "Falta el nombre del archivo." << endl;
		return 1;
	}

	char szFilename[0x100];
	strcpy(szFilename, argv[1]);

	if (access(szFilename, 0) != 0)
	{
		cerr << "El archivo " << szFilename << " no existe." << endl;
		return 1;
	}

	if (access(szFilename, 6) != 0)
	{
		chmod(szFilename, S_IREAD|S_IWRITE); // enable read/write
	}

	char szTmpFilename[0x100];

	// rename as .DAB
	strcpy(szTmpFilename, szFilename);
	int pos = 0;
	for (int i=0; szTmpFilename[i]; i++)
		if (szTmpFilename[i] == '.')
			pos = i;
	if (!pos)
	{
		cerr << "El archivo " << szFilename << " no tiene extension." << endl;
		return 1;
	}

	strcpy(&szTmpFilename[pos+1], "DAB");
	if (access(szTmpFilename, 0) == 0)
	{
		cerr << "El archivo " << szTmpFilename << " ya existe." << endl;
		return 1;
	}

	if (rename(szFilename, szTmpFilename) != 0)
	{
		cerr << "El archivo " << szFilename << " no pudo ser renombrado a .DAB." << endl;
		return 1;
	}

	cout << "Convirtiendo !!!" << endl;

	ifstream bakFile(szTmpFilename, ios::in|ios::binary);
	if (!bakFile)
	{
		cerr << "El archivo " << szTmpFilename << " no pudo ser abierto." << endl;
		return 1;
	}

	ofstream file(szFilename, ios::out|ios::binary);
	if (!file)
	{
		cerr << "El archivo " << szFilename << " no pudo ser abierto." << endl;
		return 1;
	}

	// copy .DAT header
	unsigned char headerBuffer[196];
	memset(headerBuffer, 0, 196);

	bakFile.read(headerBuffer, 196);
	int nRead = bakFile.gcount();
	if (nRead != 196)
	{
		cerr << "El archivo " << szTmpFilename << " solo tiene " << nRead << " bytes." << endl;
		return 1;
	}
	file.write(headerBuffer, 196);

	// copy records ...

	unsigned char bakFileBuffer[112];
	unsigned char fileBuffer[111];

	while (bakFile)
	{
		bakFile.read(bakFileBuffer, 112);
		if (bakFile.gcount() == 112)
		{
			// fix
			memcpy(fileBuffer, bakFileBuffer, 10);
			// change the WORD to BYTE
			fileBuffer[10] = (unsigned char)*(unsigned int*)(&bakFileBuffer[10]);
			// add the remaining buffer
			memcpy(&fileBuffer[11], &bakFileBuffer[12], 112-12);

			file.write(fileBuffer, 111);
		}
	}

	bakFile.close();
	file.close();

	// fix IDX

	strcpy(&szFilename[pos+1], "IDX");

	// rename as .IDB
	strcpy(szTmpFilename, szFilename);
	pos = 0;
	for (i=0; szTmpFilename[i]; i++)
		if (szTmpFilename[i] == '.')
			pos = i;
	if (!pos)
	{
		cerr << "El archivo " << szFilename << " no tiene extension." << endl;
		return 1;
	}

	strcpy(&szTmpFilename[pos+1], "IDB");
	if (access(szTmpFilename, 0) == 0)
	{
		cerr << "El archivo " << szTmpFilename << " ya existe." << endl;
		return 1;
	}

	if (rename(szFilename, szTmpFilename) != 0)
	{
		cerr << "El archivo " << szFilename << " no pudo ser renombrado a .IDB." << endl;
		return 1;
	}

	bakFile.open(szTmpFilename, ios::in|ios::binary);
	if (!bakFile)
	{
		cerr << "El archivo " << szTmpFilename << " no pudo ser abierto." << endl;
		return 1;
	}

	file.open(szFilename, ios::out|ios::binary);
	if (!file)
	{
		cerr << "El archivo " << szFilename << " no pudo ser abierto." << endl;
		return 1;
	}

	// copy .IDX header

	bakFile.read(headerBuffer, 158);
	nRead = bakFile.gcount();
	if (nRead != 158)
	{
		cerr << "El archivo " << szTmpFilename << " solo tiene " << nRead << " bytes." << endl;
		return 1;
	}
	file.write(headerBuffer, 158);

	// copy idx records ...
	i = 0;
	while (bakFile)
	{
		bakFile.read(bakFileBuffer, 12);
		if (bakFile.gcount() == 12)
		{
			// fix
			(*(long *)(&bakFileBuffer[8])) -= i;
			++i;
			file.write(bakFileBuffer, 12);
		}
	}
	cout << "Listo." << endl;

	return 0;
}

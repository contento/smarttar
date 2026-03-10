//
// [ ST_UTIL.CPP ]
//

#include "stdst.h"

#include <errno.h>

#ifdef DOSX286
#include <phapi.h>
#endif

#include <conio.h>

double __doubleZero = 0.0F;

char _ISO2ASCII(char byte)
{
    switch (byte)
    {
    case '\xE1':
        byte = ' ';
        break;
    case '\xC1':
        byte = 'A';
        break;
    case '\xE9':
        byte = '‚';
        break;
    case '\xC9':
        byte = 'E';
        break;
    case '\xED':
        byte = '¡';
        break;
    case '\xCD':
        byte = 'I';
        break;
    case '\xF3':
        byte = '¢';
        break;
    case '\xD3':
        byte = 'O';
        break;
    case '\xFA':
        byte = '£';
        break;
    case '\xDA':
        byte = 'U';
        break;
    case '\xF1':
        byte = '¤';
        break;
    case '\xD1':
        byte = '¥';
        break;
    }
    return byte;
}

char _ISO2ASCII2(char byte)
{
    switch (byte)
    {
    case '\xE1':
        byte = 'a';
        break;
    case '\xC1':
        byte = 'A';
        break;
    case '\xE9':
        byte = 'e';
        break;
    case '\xC9':
        byte = 'E';
        break;
    case '\xED':
        byte = 'i';
        break;
    case '\xCD':
        byte = 'I';
        break;
    case '\xF3':
        byte = 'o';
        break;
    case '\xD3':
        byte = 'O';
        break;
    case '\xFA':
        byte = 'u';
        break;
    case '\xDA':
        byte = 'U';
        break;
    case '\xF1':
        byte = 'n';
        break;
    case '\xD1':
        byte = 'N';
        break;
    }
    return byte;
}

char *_ISO2ASCII(char* str)
{
    if (str != NULL)
    {
        for (int i=0; str[i]; i++)
        {
            str[i] = _ISO2ASCII(str[i]);
        }
    }
    return str;
}

char *_ISO2ASCII2(char* str)
{
    if (str != NULL)
    {
        for (int i=0; str[i]; i++)
        {
            str[i] = _ISO2ASCII2(str[i]);
        }
    }
    return str;
}

void _ReadPassword(char *password, WORD size)
{
    const WORD ESC       = 0x1B;
    const WORD ENTER     = 0x0D;
    const WORD BACKSPACE = 0x08;
    char key;
    WORD i = 0;
    do
    {
        key = getch();
        switch (key)
        {
        case ENTER:
            password[i] = '\0';
            putch('\r');
            putch('\n');
            break;
        case ESC:
            password[0] = '\0';
            putch('\r');
            putch('\n');
            return;
        case BACKSPACE:
            if (i)
            {
                putch(key);
                putch(' ');
                putch(key);
                i--;
            }
            break;
        case 0: // F1, F2, UP, etc.
            key = getch();
            break;
        default:
            if (i < size && !iscntrl(key))
            {
                password[i++] = key;
                putch('#');
            }
        }
    }
    while (key != ENTER);
}

BOOL _isDatePassword(const char *password)
{
    STR32 datePwd;
    WORD year, month, day;
    _GetSysDate(year, month, day);
    sprintf(datePwd, "%02d%02d", day, month); // ddmm
    return (strcmp(datePwd, password) == 0);
}

char *_PrefixAppPath(char *filename)
{
    char fn[MAXPATH];
    char drive[MAXDRIVE];
    char dir[MAXDIR];
    char file[MAXFILE];
    char ext[MAXEXT];
    fnsplit(_argv[0], drive, dir, file, ext);
    strcpy(fn, filename); // save it
    strcpy(filename, drive);
    strcat(filename, dir);
    strcat(filename, fn);
    return filename;
}

char *_GetAppPath(char *path)
{
    char drive[MAXDRIVE];
    char dir[MAXDIR];
    char file[MAXFILE];
    char ext[MAXEXT];
    fnsplit(_argv[0], drive, dir, file, ext);
    strcpy(path, drive);
    strcat(path, dir);
    return path;
}

char *_getSysDatePath(char *path, BOOL dayPath)
{
    WORD year, month, day;
    _GetSysDate(year, month, day);
    if (dayPath)
        sprintf(path, "%04d\\%02d\\%02d", year, month, day);
    else
        sprintf(path, "%04d\\%02d", year, month);
    return path;
}

BOOL _mkSysDateDir(const char *basePath, BOOL createDayPath)
{
    BOOL ok = TRUE;
    // base path must exist !
    WORD year, month, day;
    _GetSysDate(year, month, day);
    FILE_NAME datePath, tempPath;
    if (basePath)
        strcpy(tempPath, basePath);
    else
        _GetAppPath(tempPath); // default
    sprintf(datePath, "%s\\%04d", tempPath, year);
    if (access(datePath, 0) != 0)
        ok &= (mkdir(datePath) == 0);
    sprintf(tempPath, "\\%02d", month);
    strcat(datePath, tempPath);
    if (access(datePath, 0) != 0)
        ok &= (mkdir(datePath) == 0);
    if (createDayPath)
    {
        sprintf(tempPath, "\\%02d", day);
        strcat(datePath, tempPath);
        if (access(datePath, 0) != 0)
            ok &= (mkdir(datePath) == 0);
    }
    return ok;
}


BOOL _SaveAsBak(const FILE_NAME& filename)
{
    FILE_NAME bakFilename;
    strcpy((char *)&bakFilename, (char *)&filename);
    WORD pos = 0;
    for (WORD i=0; bakFilename[i]; i++)
        if (bakFilename[i] == '.')
            pos = i;
    if (!pos) return FALSE;
    strcpy(&bakFilename[pos+1], "BAK");
    unlink(bakFilename);
    rename((char *)&filename, (char *)&bakFilename);
    return TRUE;
}

void _Encrypt(void *buffer, WORD bufferSize, WORD seed)
{
    for (WORD i=0; i<bufferSize; i++)
        ((char *)(buffer))[i] ^= (i+seed);
}

void _Decrypt(void *buffer, WORD bufferSize, WORD seed)
{
    for (WORD i=0; i<bufferSize; i++)
        ((char *)(buffer))[i] ^= (i+seed);
}

void _RTrim(char *s)
{
    // delete right spaces
    if (!s) return;
    for (int i=strlen(s)-1; i>=0; i--)
        if (!isspace(s[i]))
            break;
    s[i+1] = '\0';
}

void _DelSpaces(char *s)
{
    for (int i = 0; s[i]; i++)
        if (isspace(s[i]))
            strcpy(&s[i], &s[i+1]);
}

WORD _CopyFile(const char *source, const char *target)
{
    // detect if the filenames are equal
    char sourceFilename[MAXPATH], targetFilename[MAXPATH];
    if (_fullpath(sourceFilename, source, MAXPATH))
    {
        strupr(sourceFilename);
        if (_fullpath(targetFilename, target, MAXPATH))
        {
            strupr(targetFilename);
            if (!strcmp(sourceFilename, targetFilename))
                return CP_COPY_ITSELF;
        }
    }
    // try to open for input
    int sourceFile = 0;
    if ((sourceFile = open(sourceFilename, O_BINARY, S_IREAD)) == -1)
        return CP_SOURCE_NOT_FOUND;
    _fmode = O_BINARY;
    int targetFile;
    if ((targetFile = creat(targetFilename, S_IWRITE))== -1)
        return CP_TARGET_NOT_OPEN;
    // try to allocate 16k Buffer
    const BUF_SIZE = 0x4000;
    char *buffer = new char[BUF_SIZE];
    if (!buffer)
        return CP_GEN_FAIL;
    WORD readBytes = 0;
    while (!eof(sourceFile))
    {
        readBytes = read(sourceFile, buffer, BUF_SIZE);
        write(targetFile, buffer, readBytes);
    }
    close(sourceFile);
    close(targetFile);
    delete [] buffer;
    return CP_OK;
}

static WORD _FindPatch(int file, const char *key, long& seekPos)
{
    const BUF_SIZE = 0x4000;
    char *buffer = new char[BUF_SIZE];
    if (!buffer)
        return PATCH_GEN_FAIL;
    BOOL idOk  = 0;
    int count = 0;
    while (!eof(file) && !idOk)
    {
        seekPos = tell(file); // remind pos
        count = read(file, buffer, BUF_SIZE);
        for (int i=0; i<count; i++)
        {
            if (!memcmp(&buffer[i], key, strlen(key)))
            {
                seekPos += i; // adjust pos
                seekPos += sizeof(KEY); // + key
                idOk = 1;
                break;
            }
        }
    }
    delete [] buffer;
    return (idOk)?PATCH_OK:PATCH_ID_NOT_FOUND;
}

WORD _PatchFile(const char *filename, const char *key, void *patch, WORD size)
{
    // detect file
    if (access(filename, 0) != 0)
        return PATCH_FILE_NOT_FOUND;
    if (access(filename, 6) != 0)
        chmod(filename, S_IREAD|S_IWRITE); // enable read/write
    int file = open(filename, O_RDWR|O_BINARY, S_IWRITE|S_IREAD);
    if (file == -1)
        return PATCH_GEN_FAIL;
    // look for Key
    long seekPos  = 0;
    WORD result = _FindPatch(file, key, seekPos);
    if (result == PATCH_OK)
    {
        // write patch
        lseek(file, seekPos, SEEK_SET);
        write(file, patch, size);
    }
    close(file);
    return result;
}

WORD _GetPatchFromFile(const char *filename, const char *key, void *patch, WORD size)
{
    // detect file
    if (access(filename, 0) != 0)
        return PATCH_FILE_NOT_FOUND;
    int file = open(filename, O_RDWR|O_BINARY, S_IREAD);
    if (file == -1)
        return PATCH_GEN_FAIL;
    // look for Key
    long seekPos = 0;
    WORD result = _FindPatch(file, key, seekPos);
    if (result == PATCH_OK)
    {
        // read patch
        lseek(file, seekPos, SEEK_SET);
		read(file, patch, size);
	}
	close(file);
	return result;
}

static int __LongCompare(const void *itemA, const void *itemB)
{
    return (*(long *)itemA == *(long *)itemB)?0:(*(long *)itemA < *(long *)itemB)?-1:1;
}

void _LongQSort(long *items, size_t numOfItems)
{
    qsort(items, numOfItems, sizeof(long), __LongCompare);
}

long *_LongBSearch(long key, long *items, size_t numOfItems)
{
    return (long *)bsearch(&key, items, numOfItems, sizeof(long), __LongCompare);
}

BOOL g_LongLFind(DWORD key, DWORD *pItems, size_t numOfItems)
{
	return lfind(&key, pItems, &numOfItems, sizeof(DWORD), __LongCompare) != NULL;
}

static WORD daysInMonth[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

static WORD LeapYear(WORD year)
{
    return ((year % 400 == 0) || ((year % 4 == 0) && (year % 100 != 0)));
}

WORD _GetSysDate(void)
{
    struct dosdate_t date;
    _dos_getdate(&date);
    WORD packedDate;
    return _PackDate(packedDate, date.year, date.month, date.day);
}

void _GetSysDate(WORD& year, WORD& month, WORD& day)
{
    struct dosdate_t date;
    _dos_getdate(&date);
    year = date.year;
    month = date.month;
	day = date.day;
}

char *_GetSysDate(char *strDate, BOOL europeanStyle)
{
    WORD year, month, day;
    _GetSysDate(year, month, day);
    char *fmt = "%02d/%02d/%04d";
    if (europeanStyle)
        sprintf(strDate, fmt, day, month, year);
    else
        sprintf(strDate, fmt, month, day, year);
    return strDate;
}

void _SetSysDate(WORD packedDate)
{
    struct dosdate_t date;
    WORD year, month, day;
    _UnpackDate(packedDate, year, month, day);
	date.year = year;
    date.month = month;
    date.day = day;
    _dos_setdate(&date);
}

void _SetSysDate(WORD year, WORD month, WORD day)
{
    struct dosdate_t date;
    date.year = year;
    date.month = month;
    date.day = day;
    _dos_setdate(&date);
}

WORD _GetDayOfWeek(WORD year, WORD month, WORD day)
{
    // Compute the julian date from three integer values using
    // algorithm 199 of CALGO of the ACM.
    // -1 to indicate bad date
	if (year <= 0 || month < 1 || month > 12 || day < 1 ||
            (month == 2 && day == 29 && !LeapYear(year)) ||
            ((month != 2 || day != 29) && day > daysInMonth[month-1]))
        return -1;
	if (month > 2)
        month -= 3;
    else
    {
        month += 9;
        year--;
    }
    long value = ((146097L * (year / 100)) / 4) + ((1461L * (year % 100)) / 4) + (153L * month + 2) / 5 + day + 1721119L;
    return ((WORD)(((value + 1) % 7) + 1));
}

WORD _GetDayOfWeek(WORD packedDate)
{
    WORD year, month, day;
    _UnpackDate(packedDate, year, month, day);
    return _GetDayOfWeek(year, month, day);
}

BOOL _IsSaturday(WORD year, WORD month, WORD day)
{
	return (_GetDayOfWeek(year, month, day) == 7);
}

BOOL _IsSaturday(WORD packedDate)
{
    return (_GetDayOfWeek(packedDate) == 7);
}

BOOL _IsSunday(WORD year, WORD month, WORD day)
{
    return (_GetDayOfWeek(year, month, day) == 1);
}

BOOL _IsSunday(WORD packedDate)
{
    return (_GetDayOfWeek(packedDate) == 1);
}

BOOL _IsWeekend(WORD year, WORD month, WORD day)
{
	return (_IsSaturday(year, month, day) || _IsSunday(year, month, day));
}

BOOL _IsWeekend(WORD packedDate)
{
    return (_IsSaturday(packedDate) || _IsSunday(packedDate));
}

WORD _PackDate(WORD& packedDate, WORD year, WORD month, WORD day)
{
	return packedDate = ((year - 1980) << 9) | (month << 5) | day;
}

void _UnpackDate(WORD packedDate, WORD& year, WORD& month, WORD& day)
{
	year  = 1980 + (((WORD)(packedDate & 0xFE00)) >> 9);
	month = (packedDate & 0x01E0) >> 5;
	day   = packedDate & 0x001F;
}

void _UnpackDate(WORD packedDate, WORD& year, WORD& month, WORD& day, WORD& dayOfWeek)
{
	_UnpackDate(packedDate, year, month, day);
	dayOfWeek = _GetDayOfWeek(year, month, day);
}

///////////////////////////////////////////////////////////
// Time functions
///////////////////////////////////////////////////////////

void g_Milisec2Time(DWORD ms, int& hour, int& minutes, int& seconds)
{
	hour    = (int)(ms/1000)/3600;
	minutes = (int)((ms/1000)%3600)/60;
	seconds = (int)((ms/1000)%3600)%60;
}

double g_Milisec2Time(DWORD ms, int correctionTime)
{
	// Convert to time with a decimal place.

	ms += correctionTime; // ms !

	// convert to minutes
	double decTime = ((double)(ms/1000))/60.0F;

	// lookout with the trick to get tenths
	decTime = floor(decTime*10)/10;

	return decTime;
}

WORD _GetSysTime(void)
{
	struct dostime_t time;
	_dos_gettime(&time);
	WORD packedTime;
	return _PackTime(packedTime, time.hour, time.minute, time.second);
}

void _GetSysTime(WORD& hour, WORD& minute, WORD &second)
{
	struct dostime_t time;
	_dos_gettime(&time);
	hour = time.hour;
	minute = time.minute;
	second = time.second;
}

void _GetSysTime(WORD& hour, WORD& minute)
{
	struct dostime_t time;
	_dos_gettime(&time);
	hour = time.hour;
	minute = time.minute;
}

char *_GetSysTime(char *strTime, BOOL printSeconds)
{
	WORD hour, minute, second;
	_GetSysTime(hour, minute, second);
	if (printSeconds)
		sprintf(strTime, "%02d:%02d:%02d", hour, minute, second);
	else
		sprintf(strTime, "%02d:%02d", hour, minute);
	return strTime;
}

void _SetSysTime(WORD packedTime)
{
    struct dostime_t time;
    WORD hour, minute, second;
    _UnpackTime(packedTime, hour, minute, second);
    time.hour = hour;
    time.minute = minute;
    time.second = second;
    _dos_settime(&time);
}

void _SetSysTime(WORD hour, WORD minute, WORD second)
{
    struct dostime_t time;
    time.hour = hour;
    time.minute = minute;
    time.second = second;
    _dos_settime(&time);
}

WORD _PackTime(WORD& packedTime, WORD hour, WORD minute, WORD second)
{
	// WARNING:
	// Seconds are divided by 2 (integer division) so we mustn't use
	// for time elapsing !!!
	// 2.20.9
	return packedTime = (hour << 11) | (minute << 5) | second / 2;
}

void _UnpackTime(WORD packedTime, WORD& hour, WORD& minute)
{
    hour   = (packedTime & 0xF800) >> 11;
    minute = (packedTime & 0x07E0) >> 5;
}

void _UnpackTime(WORD packedTime, WORD& hour, WORD& minute, WORD& second)
{
    hour   = (packedTime & 0xF800) >> 11;
    minute = (packedTime & 0x07E0) >> 5;
    second = 2 * (packedTime & 0x001F);
}

BOOL _Str2Date(const char *strDate, WORD& year, WORD& month, WORD& day, BOOL europeanStyle)
{
    BOOL ok = FALSE;
    if (strlen(strDate) >= 8)
    {
        char *fmt = "%02d/%02d/%04d";
        int nValues;
        if (europeanStyle)
        {
            nValues = sscanf(strDate, fmt, &day, &month, &year);
        }
        else
        {
            nValues = sscanf(strDate, fmt, &month, &day, &year);
        }
        if (nValues == 3)
            ok = TRUE;
    }
    return ok;
}

char *_Date2Str(char *strDate, WORD year, WORD month, WORD day, BOOL europeanStyle)
{
    char *fmt = "%02d/%02d/%04d";
    if (europeanStyle)
        sprintf(strDate, fmt, day, month, year);
    else
        sprintf(strDate, fmt, month, day, year);
    return strDate;
}

char *_Time2Str(char *strTime, WORD hour, WORD minute, WORD second, BOOL printSeconds)
{
	if (printSeconds)
		sprintf(strTime, "%02d:%02d:%02d", hour, minute, second);
	else
		sprintf(strTime, "%02d:%02d", hour, minute);
	return strTime;
}

double g_Round(double value, double roundVal)
{
	if (!roundVal)
		return 0;

	return floor((value+roundVal/2.0)/roundVal)*roundVal;
}

double g_Ceil(double value, double ceilVal)
{
	if (ceilVal == 0.0)
		return value;

	if (ceilVal == 1.0)
		return ceil(value);

	double remain = value-floor(value); // 2.34: moved up
	if (remain == 0.0)
		return value;

	if (ceilVal == 0.5)
	{
		if (remain <= 0.5)
			return floor(value) + 0.5;

		return ceil(value);
	}

	// 2.34
	// This is for Telecom & Orbitel. June 21, 2003

	if (ceilVal == 0.3) // Telecom
	{
		if (remain <= 0.9)
			return floor(value) + ceil(remain/0.3)*0.3;

		return ceil(value);
	}

	if (ceilVal == 0.33) // Orbitel
	{
		if (remain <= 0.66)
			return floor(value) + ceil(remain/0.33)*0.33;

		return ceil(value);
	}

	return 0.0; // still here, something is wrong
}

char * g_GetErrnoStr(char *pszErr)
{
	switch (errno)
	{
	case EINVFNC: strcpy(pszErr, "Funcion incorrecta"); break;
	case ENOFILE: strcpy(pszErr, "Archivo no encontrado"); break;
	case ENOPATH: strcpy(pszErr, "Ruta no encontrada"); break;
	case EMFILE : strcpy(pszErr, "Demasiados archivos abiertos"); break;
	case EACCES : strcpy(pszErr, "Acceso negado"); break;
	case EFAULT : strcpy(pszErr, "Error desconocido"); break;
	case ECONTR : strcpy(pszErr, "Bloques de memoria destruidos"); break;
	case EINVMEM: strcpy(pszErr, "Direccion de bloque de memoria invalido"); break;
	case EINVENV: strcpy(pszErr, "Entorno (enviroment) invalido"); break;
	case EINVFMT: strcpy(pszErr, "Formato invalido"); break;
	case EINVACC: strcpy(pszErr, "Codigo de acceso invalido"); break;
	case EINVDAT: strcpy(pszErr, "Datos invalidos"); break;
	case EINVDRV: strcpy(pszErr, "Unidad (drive) especificada invalida"); break;
	case ECURDIR: strcpy(pszErr, "Intento de remover directorio actual"); break;
	case ENMFILE: strcpy(pszErr, "No hay mas archivos"); break;
	case EEXIST : strcpy(pszErr, "El archivo ya existe"); break;

	default:
		strcpy(pszErr, "Error no procesado: ");
		strcat(pszErr, sys_errlist[errno]);
	}
	return pszErr;
}

char * g_Number2Str(char *pszNumber, long number, WORD nDigits, BOOL bLeadingZeros)
{
	if (bLeadingZeros)
		sprintf(pszNumber, "%0*ld", nDigits, number);
	else
		sprintf(pszNumber, "%*ld", nDigits, number);

	return pszNumber;
}
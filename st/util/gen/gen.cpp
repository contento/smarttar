//
// [ GEN.CPP ]
//

#include <iostream.h>>
#include <fstream.h>>
#include <strstrea.h>>
#include <string.h>>
#include <io.h>>
#include <conio.h>>
#include <process.h>>
#include <dir.h>>
//
#include <info.h>>
#include <st_util.h>>

static BOOL patchFile(const char *path, const char *filename, SUPER_APP_INFO &superAppInfo);

int main(int argc, char *argv[])
{
    cout
    << "GEN 2.04 (" << APP_VER_NAME << ')' << endl
    << APP_COPYRIGHT << endl << endl
    << "  gen [c¢digo]" << endl
    << "    c¢digo: c¢digo de la localidad." << endl << endl
    << "  (Advertencia: GEN usa PKZIP.EXE y ZIP2EXE.EXE)" << endl
    << endl
    ;
    if (argc < 2)
    {
        return 1;
    }
    if (strlen(argv[1]) != 3)
    {
        cerr << "  C¢digo err¢neo." << endl;
        return 2;
    }
    //
    // search into database
    //
    ifstream file("GEN.LST");
    TEXT_FILE_LINE line;
    SERIAL_NUMBER serial;
    int  clusters;
    STR16 options[2];
    memset(options, 0, sizeof(STR16)*2);
    BOOL found = FALSE;
    while (file)
    {
        file.getline(line, sizeof(TEXT_FILE_LINE));
        strupr(line);
        strstream str;
        str << line;
        str >> serial >> clusters >> options[0] >> options[1];
        if (strlen(serial) == 19)
        {
            if (!strcmp(&serial[16], argv[1]))
            {
                found = TRUE;
                break;
            }
        }
    }
    if (!found)
    {
        cerr << "  C¢digo no encontrado." << endl;
        return 2;
    }
    // valid options are: /2: SmartTar16, /4: SmartTar32
    if (clusters != 2 && clusters != 4)
    {
        cerr << "  N£mero de bloques incorrecto {" << clusters << "} in GEN.LST" << endl;
        return 2;
    }
    BOOL isEDA = FALSE, isPRO = FALSE;
    BOOL noEEPROM = FALSE;
    if (!strcmp(options[0], "EDA") || !strcmp(options[1], "EDA"))
        isEDA = TRUE;
    if (!strcmp(options[0], "PRO") || !strcmp(options[1], "PRO"))
        isPRO = TRUE;
    // v.219
    if (!strcmp(options[0], "NOEEPROM") || !strcmp(options[1], "NOEEPROM"))
        noEEPROM = TRUE;
    //
	extern SUPER_APP_INFO g_superAppInfo;
	g_superAppInfo.Attr.Serialized = TRUE;
	g_superAppInfo.Attr.STPro      = isPRO;
	g_superAppInfo.Attr.EDA        = isEDA;
	g_superAppInfo.Attr.NoEEPROM	  = noEEPROM;
	g_superAppInfo.Attr.Outside    = !strncmp(serial, "78", 2)?FALSE:TRUE;
    //
	g_superAppInfo.Data.MaxClusters = clusters;
    if (isPRO)
    {
		if (g_superAppInfo.Data.MaxClusters == 2)
			strcpy(g_superAppInfo.Data.Title, "SmartTar Pro ST16");
        else
			strcpy(g_superAppInfo.Data.Title, "SmartTar Pro ST32");
    }
    else
    {
		if (g_superAppInfo.Data.MaxClusters == 2)
			strcpy(g_superAppInfo.Data.Title, "SmartTar ST16");
        else
			strcpy(g_superAppInfo.Data.Title, "SmartTar ST32");
    }
	strcpy(g_superAppInfo.Data.Serial, serial);
	strncpy(g_superAppInfo.Data.ShortSerial, &g_superAppInfo.Data.Serial[3], 4);
    //
	_Encrypt(&g_superAppInfo.Data, sizeof(SUPER_APP_INFO));
    //
    // build list file for pkzip
    //
    ofstream lstFile("stx.lst");
    char *nonCommonPath = "common\\";
    char *commonPath    = "common\\";
    if (isEDA)
        nonCommonPath = "eda\\";
	// non-common files
    lstFile
    << nonCommonPath << "st.exe" << endl
    << nonCommonPath << "pr_sr80.dll"  << endl
    << nonCommonPath << "pr_dr80.dll"  << endl
    << nonCommonPath << "pr_lin80.dll" << endl
    << nonCommonPath << "pr_drpre.dll" << endl
    << nonCommonPath << "pr_dr40.dll"  << endl
    << nonCommonPath << "pr_sr40.dll"  << endl
    << nonCommonPath << "pr_dr18.dll"  << endl
    << nonCommonPath << "pr_sr28.dll"  << endl
    << nonCommonPath << "pr_dreme.dll" << endl
    << nonCommonPath << "pr_drhal.dll" << endl
    ;
    // common files
    lstFile
    << commonPath << "st.ico"       << endl
    << commonPath << "leame.bat"    << endl
    << commonPath << "stella"       << endl
    << commonPath << "res.dat"      << endl
    << commonPath << "help.dat"     << endl
    << commonPath << "ph_patch.dat" << endl
    << commonPath << "setup.exe"    << endl
    << commonPath << "ini2cfg.exe"  << endl
    << commonPath << "inf2dat.exe"  << endl
    << commonPath << "dump.exe"     << endl
    << commonPath << "repair.exe"   << endl
    << commonPath << "update.exe"   << endl
    << commonPath << "viewlog.exe"  << endl
    ;
    lstFile.close();
    //
    // Patch
    //
    cout << "  - Parchando ... ";
    BOOL ok;
    ok =
		patchFile(nonCommonPath, "ST.EXE"     , g_superAppInfo) &&
		patchFile(commonPath   , "SETUP.EXE"  , g_superAppInfo) &&
		patchFile(commonPath   , "INF2DAT.EXE", g_superAppInfo) &&
		patchFile(commonPath   , "INI2CFG.EXE", g_superAppInfo) &&
		patchFile(commonPath   , "REPAIR.EXE" , g_superAppInfo) &&
		patchFile(commonPath   , "UPDATE.EXE" , g_superAppInfo) &&
		patchFile(commonPath   , "DUMP.EXE"   , g_superAppInfo) &&
		patchFile(commonPath   , "VIEWLOG.EXE", g_superAppInfo)
        ;
    cout << endl;
    if (!ok)
    {
        cerr << "  Error: No se pudo parchar alguno de los archivos." << endl;
        return 4;
    }
    //
    // delete files
    //
    unlink("stx.zip");
    unlink("stx.exe");
    //
    // removing attribs
    //
    int result;
    result = spawnlp(P_WAIT, "command", "", "/c", "attrib", "-r", "-h", "COMMON\\*.*", ">>", "NUL", NULL);
    result = spawnlp(P_WAIT, "command", "", "/c", "attrib", "-r", "-h", "EDA\\*.*", ">>", "NUL", NULL);
    //
    // compress
    //
    cout << "  - Comprimiendo ... ";
    result = spawnlp(P_WAIT, "command", "", "/c", "pkzip.exe", "-sGCC", "stx", "@stx.lst", ">>", "NUL", NULL);
    result = spawnlp(P_WAIT, "command", "", "/c", "zip2exe.exe", "stx.zip", ">>", "NUL", NULL);
    if (result == -1)
        cerr << "  Advertencia: No se pudo comprimir los archivos.";
    cout << endl;
    //
    // copy to diskette
    //
    cout << endl << "  Inserte un disquete y presione una tecla ([Esc] para cancelar)...";
    if (getch() == '\x1B')
    {
        cerr << endl << endl << "  Advertencia:  Abortado por el usuario." << endl;
        unlink("stx.zip");
        unlink("stx.exe");
        unlink("stx.lst");
        return 3;
    }
    cout << endl << endl;
    cout << "  - Creando disquete ... ";
    result = _CopyFile("STX.EXE"            , "A:STX.EX_");
    result = _CopyFile("COMMON\\INSTALL.BAT", "A:INSTALL.BAT");
    result = _CopyFile("COMMON\\LEAME.TXT"  , "A:LEAME.TXT");
    cerr << endl;
    if (result != CP_OK)
        cerr << "  Advertencia no se pudo hacer la copia completamente." << endl;
    //
    unlink("stx.zip");
    unlink("stx.exe");
    unlink("stx.lst");
    cout << endl << "  --- Listo, todo parece bien ---";
    return 0;
}

BOOL patchFile(const char *path, const char *filename, SUPER_APP_INFO &superAppInfo)
{
    FILE_NAME target;
    strcat(strcpy(target, path), filename);
    int result = _PatchFile(target,
                            superAppInfo.Key,
                            &superAppInfo.Attr,
                            sizeof(WORD)+sizeof(APP_INFO)
                           );
    return (result == PATCH_OK);
}

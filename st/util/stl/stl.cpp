//
// [ STL.CPP ]
//

#include "stdst.h"

#include <stm2.h>
#include <dongle.h>
#include <rxproces.h>

#ifdef DOSX286
#include <phapi.h>
#endif

extern unsigned _stklen = 0x4000; // ~%^%&$#@!&*, _stklen GRRR ...

CFG 	*g_cfg;
char 	*errorMemory;

BOOL parseCommandLine(int argc, char *argv[], CommandOptions& cmdOptions);
void showHelp(void);

void NewHandler(void);

int main(int argc, char *argv[])
{
	errorMemory = new char[0x200];
	extern void (*_new_handler)(void);
	_new_handler = NewHandler;

	cout
        << "STL 1.31 (" << APP_VER_NAME << ')' << endl
		<< APP_COPYRIGHT << endl
	;

	g_cfg = new CFG;

	int nRet = 1;
	if (!g_cfg)
	{
		return nRet;
	}

	WORD status = g_cfg->Load(); // load CFG

	do // non SEH
	{
		if (status != CFG::OK)
		{
			char *msg = " tiene una falla general.";
			switch (status)
			{
			case CFG::NO_CFG_FILE :
				msg = "no existe."    ;
				break;
			case CFG::BAD_CFG_FILE:
				msg = "est� corrupto.";
				break;
			}
			cerr << "El archivo de configuraci�n " << msg << endl;
			break;
		}

		CommandOptions cmdOptions;
		if (!parseCommandLine(argc, argv, cmdOptions))
		{
			cerr << "Opci�n o combinaci�n de opciones inv�lida.  Use STL /h" << endl;
			break;
		}

		if (cmdOptions.showHelp)
		{
			showHelp();
			nRet = 0;
			break;
		}

		if (strlen(cmdOptions.password) == 0)
		{
			cout << "\"stl /h\" muestra ayudas." << endl;
			cout << "Presione Esc para abortar operaci�n." << endl;
			cout << "C�digo de acceso: ";
			_ReadPassword(cmdOptions.password, sizeof(CFG::PASSWORD)-1);
		}

		if (!strlen(cmdOptions.password))
		{
			break;
		}

		if (!g_cfg->isUtilPassword(cmdOptions.password))
		{
			cerr << "Lo siento, acceso negado." << endl;
			break;
		}

		STM2  stm2;
		BOOL ok = FALSE;

		if (stm2.getStatus() != STM2::NONE)
		{
			ok = TRUE;
		}
		else
		{
			DONGLE dongle;
			ok = dongle.isThere();
		}


		if (!ok)
		{
			cerr << "Lo siento, acceso negado, aplicaci�n no autorizada." << endl;
			break;
		}

		RXProcessor rxProcessor(cmdOptions);
		mkdir(cmdOptions.lstPath); // no error processing !!!

		cout << "Procesando recibos ...";
		if (!rxProcessor.processDAT())
		{
			cerr << endl << "Lo siento, error convirtiendo datos a archivos planos." << endl;
			break;
		}

		cout << endl << "Procesando acumulados ...";
		if (!rxProcessor.processSTA())
		{
			cerr << endl << "Lo siento, error convirtiendo acumulados a archivos planos." << endl;
			break;
		}

		cout << endl << "Listo." << endl;

		nRet = 0;
	}
	while (0); // only once

	delete g_cfg;

	return nRet;
}

void showHelp(void)
{
    cout
    << "STL [/a] [/c] [/h] [/iID] [/lDIR] [/n] [/pClave] [/rRX] [/s] [/t] [/v]" << endl
    << "  /a Adicionar recibos al final del archivo .PRN existente." << endl
    << "  /c Ordenado por cabinas." << endl
    << "  /h Ayudas." << endl
    << "  /iID Identificaci�n serial de cuatro digitos." << endl
    << "  /lDIR Directorio para los archivos .LST y .PRN. Por defecto la del archivo." << endl
    << "  /n S�lo recibos no cobrados." << endl
	<< "  /pCLAVE Clave de usuario. Por defecto la solicitar� al ejecutarse." << endl
    << "  /rRX Nombre del archivo RX. Por defecto turno actual." << endl
    << "  /s S�lo recibos de servicios especiales." << endl
    << "  /t S�lo recibos de telefon�a autom�tica." << endl
    << "  /v S�lo recibos de pago revertido." << endl
    << "Ejemplos:" << endl
    << "  STL" << endl
    << "    Genera archivo RXDAT.LST en el directorio actual." << endl
    << "  STL /r1999\\06\\RX08_01 /t /n" << endl
    << "    Recibos no cobrados de telefon�a autom�tica del turno 1 de junio 8 de 1999." << endl
    << "    Genera archivos RX08_01D.LST, RX08_01S.LST, RX08_01D.PRN y RX08_01S.PRN " << endl
    << "    dentro del directorio 1999\\06." << endl
    ;
}

BOOL parseCommandLine(int argc, char *argv[], CommandOptions& cmdOptions)
{
    memset((char *)&cmdOptions, 0, sizeof(cmdOptions));
    strcpy(cmdOptions.rxBasePath, ".");
    strcpy(cmdOptions.rxBaseFilename, "rx");
    strcpy(cmdOptions.lstPath, cmdOptions.rxBasePath);
    cmdOptions.currentTurn = TRUE;
    //
    BOOL ok = TRUE;
    STR32 arg;
    for (int i=1; i<argc; i++)
    {
        strcpy(arg, argv[i]);
        strlwr(arg);
        if (!strcmp(arg, "/a"))
        {
            cmdOptions.appendDatResult = TRUE;
        }
        else if (!strcmp(arg, "/c"))
        {
            cmdOptions.sortByBooth = TRUE;
        }
        else if (!strcmp(arg, "/h"))
        {
            cmdOptions.showHelp = TRUE;
        }
        else if (!strcmp(arg, "/n"))
        {
            cmdOptions.onlyNotPaid = TRUE;
        }
        else if (!strcmp(arg, "/s"))
        {
            cmdOptions.onlySpecialReceipts = TRUE;
        }
        else if (!strcmp(arg, "/t"))
        {
            cmdOptions.onlyAutomaticReceipts = TRUE;
        }
        else if (!strcmp(arg, "/v"))
        {
            cmdOptions.onlyTollFree = TRUE;
        }
        // compound options
        else if (!strncmp(arg, "/l", 2))
        {
            // process LST path
            strcpy(cmdOptions.lstPath, &arg[2]);
        }
        else if (!strncmp(arg, "/r", 2))
        {
            strcpy(cmdOptions.rxBasePath, &arg[2]);
            // extract base filename
            strcpy(cmdOptions.rxBaseFilename, cmdOptions.rxBasePath);
            strrev(cmdOptions.rxBaseFilename);
            char *backslash = strchr(cmdOptions.rxBaseFilename, '\\');
            if (backslash)
            {
                backslash[0] =  '\0';
                strrev(cmdOptions.rxBaseFilename);
                // trim filename from base path
                cmdOptions.rxBasePath[
                    strlen(cmdOptions.rxBasePath)-strlen(cmdOptions.rxBaseFilename)-1
                ] = '\0';
                // extract date and turn: yyyy\mm\rxdd_tt
                if (strlen(cmdOptions.rxBasePath) == 7 && strlen(cmdOptions.rxBaseFilename) == 7)
                {
                    WORD year, month, day;
                    sscanf(cmdOptions.rxBasePath, "%04d\\%02d", &year, &month);
                    sscanf(cmdOptions.rxBaseFilename, "rx%02d_%02d", &day, &cmdOptions.turn);
                    _PackDate(cmdOptions.date, year, month, day);
                    cmdOptions.currentTurn = FALSE;
                }
            }
            else
            {
                strrev(cmdOptions.rxBaseFilename);
                strcpy(cmdOptions.rxBasePath, ".");
            }
            // same lst path by default
            if (!strcmp(cmdOptions.lstPath, ".")) // /l processed ?
                strcpy(cmdOptions.lstPath, cmdOptions.rxBasePath);
        }
        else if (!strncmp(arg, "/p", 2))
        {
            strcpy(cmdOptions.password, &argv[i][2]); // password is case sensitive
        }

        else if (!strncmp(arg, "/i", 2))
        {
            strcpy(cmdOptions.shortSerial, &arg[2]);
        }
        else
        {
            ok = FALSE;
        }
    }
    if (cmdOptions.onlyAutomaticReceipts && cmdOptions.onlySpecialReceipts)
    {
        ok = FALSE; // it's a mistake!
    }

    return ok;
}

void NewHandler(void)
{
    if (errorMemory)
        delete [] errorMemory;
    cout
    << "No hay memoria disponible" << endl
    << "por favor reporte este problema ..."
    << endl
    ;
    exit(1);
}

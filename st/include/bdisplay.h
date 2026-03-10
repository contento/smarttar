#ifndef __BDISPLAY_H
#define __BDISPLAY_H

#if !defined(__ST_DEFS_H)
#include <st_defs.h>
#endif

#if !defined(__STRING_H)
#include <string.h>
#endif

#if !defined(__SERIAL_H)
#include <serial.h>
#endif

class BoothDisplay
{
public:
	struct Info
	{
		Info()
		{
			memset(this, 0, sizeof(Info)); // 2.21.1 Build 5
		}
		WORD      displayNum;
		WORD      boothNum;
		STR64     message;
		CITY_NAME cityName;
		PHONE     phone;
		double    elapsedTime; // current call
		double    cost;        // current call
		WORD      numOfCalls;  // several calls
		double    totalCost;   // several calls
	};
	BoothDisplay();
	~BoothDisplay();
	//
	BOOL install(const char *message, WORD com=SERIAL::COM2, WORD bauds=SERIAL::S9600);
	// commands
	void setDefaultMessage(const char *message);
	//
	void showOnHook        (Info info);
	void showOffHook       (Info info);
	void showDialing       (Info info);
	void showLastRefresh   (Info info);
	void showComm          (Info info);
	void showTotalCash     (Info info);
	void showLocked        (Info info);
	void showSpy           (Info info);
	void showDialErr       (Info info);
	void showCommErr       (Info info);

	BOOL isInstalled(void)
	{
		return installed;
	}

private:
	enum COMMANDS {
		// 00h     : Cabina cero, para usos futuros
		// 01h..20h: Cabinas 1..32
		// 21h     : Todas las cabinas (broadcast)
		ON_HOOK_CMD       = 0xF0,  // mensaje por defecto.
		OFF_HOOK_CMD      = 0xF1,  // descolgado.
		DIALING_CMD       = 0xF2,  // marcando: numero + ciudad
		COMM_CMD          = 0xF3,  // comunicacion: tiempo + valor + nro llamadas + costo total.
		NOTUSED_CMD2      = 0xF4,  // international
		SPY_CMD           = 0xF5,  // intervencion.
		LOCKED_CMD        = 0xF6,  // bloqueado.
		DIAL_ERR_CMD      = 0xF7,  // error de marcacion.
		COMM_ERR_CMD      = 0xF8,  // error de comunicacion.
		TOTAL_CASH_CMD    = 0xF9,  // total: cabina, numero de llamadas y valor total.
		LAST_REFRESH_CMD  = 0xFC,  // total: cabina, numero de llamadas y valor total.
		//
		SET_DEFAULT_MESSAGE_CMD = 0xFD, // colocacion de mensaje por defecto.
		SEPARATOR_CMD           = 0xFE, // separador en comandos.
		END_CMD                 = 0xFF  // fin de comando.
	};
private:
	SERIAL *serial;
	BOOL   installed;

	Info *m_CurrentInfo;
};

#endif // __BDISPLAY_H

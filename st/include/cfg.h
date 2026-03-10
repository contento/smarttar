#ifndef __CFG_H
#define __CFG_H

#if !defined(__ST_DEFS_H)
#include <st_defs.h>
#endif

#if !defined(__STRING_H)
#include <string.h>
#endif

#define  MAX_DEFAULT_TARIFFS 0x0A   // max num of default tariffs

//
// the general configuration for the ST system, useful if the ST.INI file
// is missed or corrupted.
// please if changes will make talk with deparment of engineering. GCC/gcc
// T_ stands for Time, F_ stands for Flag
//
class CFG
{
public:
	CFG(void)
	{
	}

	~CFG()
	{
	}
	//
	enum STATUS_TAG { OK, NO_CFG_FILE, BAD_CFG_FILE };
	//
	WORD Load(const char *path = NULL, BOOL fromIni = FALSE);
	WORD Save(const char *path = NULL, BOOL saveIni = TRUE);
	WORD GetStatus(void); // load status
	//
	void setDefaultPasswords(void);
	BOOL isUtilPassword(const char *password);
	BOOL SetCurrentPassword(const char *password);
    BOOL ChangePassword(const char *oldPassword, const char *newPassword);
    BOOL PasswordIs(const char *password, WORD owner)
    {
		return !strcmp(password, Passwords[owner]);
	}
	BOOL IsHollyday(WORD year, WORD month, WORD day);
	BOOL IsHollyday(WORD packedDate);
	inline BOOL IsExtension(WORD cNum, WORD bNum);
	void nextTurn(void);
	//
	enum FORM_TAG
	{
        DR_80,
        //		TEL_80,
        DR_PRE,
        LINEAL_80,
        SR_80,
        DR_40,
        SR_40,
        DR_18,
		SR_28,
        DR_EME,
        DR_HALF
    };
	//
	//  [ Passwords ] Always leave the passwords as the first group !!!
    //  Backdoor as the first and current as the last for config.
    //  The operator password, which is useful to access extension services,
    //  let the operator see information but not to change it.
    //  Util is for the Utilities.
    //
	enum PASSWORD_TAG
	{
		BACKDOOR, SUPERVISOR, USER1, USER2,
		CURRENT, OPERATOR, UTIL, // v.220 introduced UTIL
		NOPWD1, NOPWD2 // 2 additonal passwords for future !!!
	};
	enum SIGNAL_TAG { S_BIAS, S_TIME, S_THREAD, S_TONE };
	typedef char PASSWORD[8+1];
	PASSWORD Passwords[NOPWD2+1];
	//
	// [ Identificadores de cabinas ]
	//
	enum BOOTH_ATTR
	{
		ACTIVE_EXT = 0x0001 // SmartTar Pro (Booth/Extension)
	};
    struct BOOTH_ENTRY
    {
        char Name[12+1];
        WORD Attr;
    };
    BOOTH_ENTRY BoothInfo[MAX_BOOTH]; // june 13/95.
    //
    // [ Dias festivos ]
    //
    CALENDAR_ENTRY Hollydays[MAX_HOLLY_YEARS];
    //
    // [ Sistema ]
    //
    typedef char GROUP[0x20];
	//
    GROUP SysGroup;
    STR32 COUNTRY;      // pais donde es instalado el sistema
    STR16 CURRENCY;     // simbolo monetario $, US, S\., etc.
    WORD DEALER;             // Distribuidor del producto
    CITY_NAME    CITY;       // nombre de la ciudad
    COMPANY_NAME COMPANY;    // nombre de la compania.  Ejemplo: TELECOM
    COMPANY_NAME OPERATOR_NAME;        // quien proveera el servicio de enlace
    STR32        ID;         // NIT, CC, etc.
	WORD  CLUSTERS;          // numero de clusters 8 cabinas instaladas
	STR32 TAX_NAME;          // nombre del impuesto
	double TAX_PERCENT;      // valor del impuesto
	double DDN_TAX;          // valor del impuesto llamadas nacionales
	double DDI_TAX;          // valor del impuesto llamadas internacionales
	char OP_TITLE[0x0A];     // titulo del operador
    STR32 OP_NAME;           // nombre del operador
    WORD SS_ID;   					 // screen saver id
    WORD SS_TIME; 					 // screen saver time
    //
    // [ Aplicacion ]
	//
    GROUP AppGroup;
    WORD MANUAL;             // to quick access instead P_OPERATION
    WORD MANUAL_ANSWER;      // El circuito de JEAM para contestacion manual
    // T_LOCK es el tiempo de cuelgue en el VIEW !!!
    char P_OPERATION[0x0F];  // tipo de operacion de tarificacion
    STR32 COM;               // configuracion de puerto serial
    WORD  LPT;               // Puerto LPT1 o LPT2
    WORD DOUBLE_PRN;         // trabajar doble impresora
    char P_PORT[0x0F]; 	     // tipo de conexion de la impresora
    WORD FORM;               // to quick acces
    STR32 P_FORM;            // formato de impresion
    char P_FOOTER[0x90];     // pie de pagina de los recibos
    STR64 P_FOOTER1;    // pie de pagina 1 de los recibos
	STR64 P_FOOTER2;    // pie de pagina 2 de los recibos
    char CASH[0x0F];    // apertura de la caja de billetes
	WORD OLD_M_ROUND;   // Redondeo de total de factura
    CITY_NAME USA;      // USA name in spanish
    //
    WORD MIN_NAL;      // numero minimo de minutos, nacional
	WORD MIN_INTER;    // numero minimo de minutos, internacional
    WORD MIN_USA;      // numero minimo de minutos, USA
	WORD MIN_BORDER;   // numero minimo de minutos, frontera
	WORD MIN_CELLULAR; // numero minimo de minutos, Celular
	WORD OLD_CEIL_NAL;       // llevar minuto entero, nacional
	WORD OLD_CEIL_INTER;     // llevar minuto entero, internacional
	WORD OLD_CEIL_USA;       // llevar minuto entero, USA
	WORD OLD_CEIL_BORDER;    // llevar minuto entero, frontera
	WORD OLD_CEIL_CELLULAR;   // llevar minuto entero, celular
	//
	STR64  MCARDS;                    // to read values
	double MCARD[MAX_MAGNETIC_CARDS]; // valores de las targetas magneticas
	double NA_TLX_BASE;
	double NA_INTER_TLX_BASE; // v. 2.10 telex internacional
	LONG N_RECEIPT;    // numero de recibo (0..999.999)
	WORD N_COM_ERR;    // numero de errores de comunicacion
	WORD N_DIAL_ERR;   // numero de errores de marcacion
    WORD MAX_COM_ERR;  // maximo numero de errores de comunicacion
    WORD MAX_DIAL_ERR; // maximo numero de errores de marcacion
    WORD LOCK_OTHER_OPERATORS; // bloquear otros operadores ?
	WORD DEF_TARIFFS[MAX_DEFAULT_TARIFFS];
    char DEFAULT_TARIFFS[0x60];  // extern (.INI)
	WORD APPLY_DDN_SCHEDULE;     // applicar reducidas ...
	WORD APPLY_DDI_SCHEDULE;     // ... ?
	WORD GENERATE_PREPAID_RECEIPT; // generar recibo de prepago
	WORD DOUBLE_PREPAID_RECEIPT;   // generar recibo de prepago doble
	//
	// [ Telefonia ]
	//
	GROUP PhoneGroup;
	WORD T_ON_HOOK;          // tiempo de cuelgue
	WORD T_OFF_HOOK;         // tiempo de descuelgue
	WORD T_BREAK;            // apertura de circuito en decadica
	WORD T_MAKE;             // cierre de circuito en decadica
	WORD T_MAKE_MARGIN;      // para ir despues de MAKE a INTERDIG
	WORD T_INTERDIG;         // tiempo de interdigital en decadicada
	WORD T_DTMF_FLAG;        // tiempo para la bandera de DTMF
	WORD T_DTMF_INTERDIG;    // tiempo de interdigital en DTMF
	WORD T_ANSWER;       	 // tiempo para contestacion -> T_BIAS
	WORD T_TALK;         	 // tiempo de gracia
	WORD T_BIAS;             // tiempo de inversion de polaridad
	WORD T_DIAL;         	 // tiempo para marcacion
	WORD T_COM;              // tiempo de inicio de comunicacion
	WORD T_LOCK;           	 // tiempo de bloqueo real para colgar el telefono
	WORD T_INTER_RING;       // tiempo entre rings maximo, despues se asume como on-hook
	char SIGNAL[0x0F];   	// Se¤al de contestacion extern (.INI)
    WORD ASIGNAL;           // to quick access
    //
    GROUP DialingGroup;
    WORD ACCESS_LEVELS;   // numero de niveles de acceso (2 o 3)
    WORD ACCESS;          // codigo de accesso: 0, 9
    WORD SPECIAL_ACCESS;  // digito de acceso para llamadas de servicios especiales
	WORD OPERATOR_ACCESS; // codigo de acceso del proveedor
	WORD EDA_ACCESS;      // digito de acceso para llamadas a EDA
	WORD CELLULAR_ACCESS; // segundo digito para llamadas a celulares
    WORD CELLULAR_OPERATOR_ACCESS; // codigo de acceso del proveedor de celular
	WORD BORDER_ACCESS;   // segundo digito para llamadas a la frontera
    WORD INTER_ACCESS;    // segundo digito para llamadas a internacionales
    //
    WORD INTER_DIGITS;    // numero de digitos para internacional
	WORD BORDER_DIGITS;   // numero de digitos para frontera
	WORD CELLULAR_DIGITS; // numero de digitos para celular
    WORD NAL_DIGITS;      // numero de digitos para llamada nacional
    WORD LOCAL_DIGITS;    // numero de digitos para llamada local
    WORD SPECIAL_DIGITS;  // numero de digitos para servicio especial
	WORD NAL_DIGITS_MARGIN;   // margen de cifras en contestacion nacional
	WORD INTER_DIGITS_MARGIN; // margen de cifras en contestacion internacional
    //
    // these items are useful for extensions (SmartTar Pro)
    //
    GROUP ExtGroup;
	WORD   E_APPLY_DDN_SCHEDULE; // applicar reducidas ...
	WORD   E_APPLY_DDI_SCHEDULE; // ... ?
	double E_DISCOUNT;          // porcentaje de descuento en llamadas
	double E_INSTALL_COST;      // costo de instalacion
	double E_LINE_COST;         // costo de la linea
	WORD   E_SHOW_PHONE;        // mostrar telefonos en pantalla
    WORD   E_FIRST_EXT;         // primer numero de extension
    LONG   E_N_RECEIPT;         // contador de recibos de extension
    WORD   E_MIN_NAL;     			// numero minimo de minutos, nacional
	WORD   E_MIN_INTER;         // numero minimo de minutos, internacional
    WORD   E_MIN_USA;           // numero minimo de minutos, USA
	WORD   E_MIN_BORDER;        // numero minimo de minutos, frontera
	WORD   E_MIN_CELLULAR;      // numero minimo de minutos, Celular
	WORD   OLD_E_CEIL_NAL;          // llevar minuto entero, nacional
	WORD   OLD_E_CEIL_INTER;        // llevar minuto entero, internacional
	WORD   OLD_E_CEIL_USA;          // llevar minuto entero, USA
	WORD   OLD_E_CEIL_BORDER;       // llevar minuto entero, frontera
	WORD   OLD_E_CEIL_CELLULAR;     // llevar minuto entero, celular
	double E_MIN_AVAILABLE;     // minimo disponible
	WORD   E_APPLY_ROUND;       // aplicar redondeo ?
	GROUP CriticalGroup;
	WORD CHECK_PAUSE_KEY;       // desactivacion de la tecla pausa
	WORD IGNORE_EXTRA_DIGITS;   // ignorar cifras mas alla de las programadas
	WORD EXCLUSIVE_SPY;         // intervencion exclusiva entre clusters
	WORD IGNORE_PRE_ANSWER;     // ignore falsas respuestas
	WORD EXIST_FALSE_ONE;       // algunas centrales producen un uno falso al descolgar
	WORD DETECT_INCOME;          // detectar llamada entrante
	GROUP ModemGroup; // greenleaf types
	int            MODEM_COM;
	unsigned short MODEM_BASE;
    int            MODEM_IRQ;
    long           MODEM_BAUDS;
    int            MODEM_DIAL;    // type of dialing
	int            MODEM_SPEAKER;
	int            MODEM_DELAY;
    int            MODEM_ACKTIME; // acknowdledge time
	long           MODEM_MAXTIME;
	PHONE          MODEM_PHONE;
	GROUP DisplayGroup; // SERIAL.H types
	BOOL  DISPLAY_ENABLE;
	WORD  DISPLAY_COM;
    WORD  DISPLAY_BAUDS;
	STR64 DISPLAY_DEFAULT_MESSAGE;
	//
    // remember for compatibility you have to put the newest members here !!!
	//
	WORD LOCAL_DIGITS_MARGIN;    // margen de cifras en contestacion local
	BOOL MULTIPLE_PREPAID_CALLS; // llamadas multiples en prepago
	BOOL CALL_ACTUAL_COST;       // valor real de la llamada
	BOOL NO_SOUND_WHILE_SPY;     // desactivar sonido de alarma mientras intervencion
	BOOL ACTIVATE_RELAY;         // activar el relevo de proteccion
	BOOL RELAY_NUMBER;           // imprimir log
	WORD TURN_DAY;               // dia del ultimo numero de turno
	WORD TURN_NUMBER;            // numero de turno en un dia
	int  MODEM_RECEIVESENDDELAY; // retardo para envio y recepcion
	// 2.20.7
	double M_ROUND;              // redondeo en punto flotante
	WORD   VIEW_DECIMALS;        // numero de decimales en costo llamada
	// 2.20.8
	double CEIL_NAL;       // llevar minuto entero/medio, nacional
	double CEIL_INTER;     // llevar minuto entero/medio, internacional
	double CEIL_USA;       // llevar minuto entero/medio, USA
	double CEIL_BORDER;    // llevar minuto entero/medio, frontera
	double CEIL_CELLULAR;   // llevar minuto entero/medio, celular
	//
	double   E_CEIL_NAL;          // llevar minuto entero/medio, nacional
	double   E_CEIL_INTER;        // llevar minuto entero/medio, internacional
	double   E_CEIL_USA;          // llevar minuto entero/medio, USA
	double   E_CEIL_BORDER;       // llevar minuto entero/medio, frontera
	double   E_CEIL_CELLULAR;     // llevar minuto entero/medio, celular
	//
	int INTER_DIGITS_NOT_INCLUDED; // numero de digitos para corte de internacional si no incluida
	int NAL_DIGITS_NOT_INCLUDED;   // numero de digitos para corte de nacional si no incluida
	int LOCAL_DIGITS_NOT_INCLUDED; // numero de digitos para corte de local si no incluida
	// 2.20.9
	STR256 HEADER_LINE;         // linea
    STR64 HEADER_LINE1;         // linea 1
    STR64 HEADER_LINE2;         // linea 2
    STR64 HEADER_LINE3;         // linea 3
	STR64 HEADER_LINE4;         // linea 4 -> wich will include %s or %d
	BOOL  HEADER_PRINT_TAXNAME; // imprimir impuesto
	BOOL  HEADER_PRINT_RECNO;   // imprimir numero de registro
	// 2.20.9 build 5
	WORD  CORRECTION_TIME;      // tiempo adicional en milisegundos
	// 2.21.1 Build 5
	WORD  T_STORE;      // tiempo adicional en milisegundos
	// 2.22.1 build 25
	WORD CACHE_SIZE;
	BOOL CHECK_DUPS;
	// 2.30
	double INTERNET_TAX;	// tax de internet
	double INTERNET_ROUND; 	// facturacion en minutos
	double INTERNET_TARIFF; // valor

	BOOL RECNO_LEADING_ZEROS;	// llenar con ceros la izquierda
	WORD RECNO_DIGITS;        	// numero de digitos, depende de leading zeros

	STR64 RECNO_LABEL;         // version larga "Rec."

	WORD ACTIVE_CLUSTERS; // 2.30 useful for RT_ENGINE
	WORD VIEW_REFRESH_TIME; // 2.30 build 13 useful for CTRL_REFRESH
	BOOL VIEW_PHONE; // 2.30 build 13 useful for CTRL_REFRESH

	// 2.33
	double CELLULAR_TAX;	// impuesto de llamadas celulares

	//
	// To adjust some members
	//
	void AdjustFooter(void);
	void AdjustHeader(void);
	void AdjustForm  (void);
private:
	struct ENTRY
	{
		enum TAG
		{
			INTEGER   = 0x0001,
			LONG      = 0x0002,
			FLOAT     = 0x0003,
            DOUBLE    = 0x0004,
            STRING    = 0x0005,
            GROUP     = 0x00FF,
            //
			NORMAL    = 0x0000, // no mask ...
			UNSIGNED  = 0x0100, // ... masks
			UPPER     = 0x0200,
            LOWER     = 0x0400,
            NO_SPACES = 0x0800
        };
        WORD  Type;
        char *Id;      // token identifier
        void *Value;   // token value pointer
	};

	void FillCfgTable(ENTRY *table);
	BOOL Line2Entry(const ENTRY *table, const char *line);
	BOOL Entry2Line(const ENTRY *table, WORD offset, char *line);
	int  SearchId(const ENTRY *table, const char *id);
    void SetDefault(BOOL setAll = FALSE);
    void Adjust(void);
};

#endif // __CFG_H

//
// [ VIEW_HB.CPP ]
//

#include "stdst.h"

#include <view.h>
#include <events.h>
#include <hb_ids.h>

#ifndef USE_HELP_CONTEXTS
#define USE_HELP_CONTEXTS
#include <help.hpp>
#endif

static HELP_INFO __HelpInfo[] = {
    // general purpose
    {HB_ENTER_BOOTH  , "Entre el número de cabina (nn)", BLACK, LIGHTGRAY},
    {HB_ENTER_TEL    , "Entre el número telefónico", BLACK, LIGHTGRAY},
    {HB_ENTER_TAR    , "Entre el número de tarifa (nn)", BLACK, LIGHTGRAY},
    {HB_ENTER_IAMOUNT, "Entre la cantidad (nnn)", BLACK, LIGHTGRAY},
    {HB_ENTER_DAMOUNT, "Entre la cantidad (nnn.d)", BLACK, LIGHTGRAY},
    {HB_ENTER_CITY   , "Entre la localidad/ciudad", BLACK, LIGHTGRAY},
    {HB_ENTER_COUNTRY, "Entre el país", BLACK, LIGHTGRAY},
    {HB_ENTER_TIME   , "Entre la hora (hh:mm)", BLACK, LIGHTGRAY},
    {HB_ENTER_DATE   , "Entre la fecha (dd-mm-aa)", BLACK, LIGHTGRAY},
    {HB_ENTER_ETIME  , "Entre el tiempo del servicio (nnn.d)", BLACK, LIGHTGRAY},
    {HB_ENTER_VALUE  , "Entre el valor (nnn.d)", BLACK, LIGHTGRAY},
	{HB_ENTER_WORDS  , "Entre el numero de minutos (nnn.n)", BLACK, LIGHTGRAY},
    {HB_ENTER_MOTIF  , "Entre el motivo del servicio", BLACK, LIGHTGRAY},
    {HB_ENTER_UV     , "Entre el valor unitario (nnn.d)", BLACK, LIGHTGRAY},
    // Toolbar
    {H_MANUAL   , "Procesamiento manual de recibos", BLACK, LIGHTGRAY},
    {H_SP_SERV  , "Servicios especiales", BLACK, LIGHTGRAY},
    {H_CALC     , "Activar calculadora", BLACK, LIGHTGRAY},
	{H_SPY      , "Intervenir/liberar cabinas (F7)", BLACK, LIGHTGRAY},
	{H_INTER    , "LLamadas internacionales (F8)", BLACK, LIGHTGRAY},
	{H_LOCK     , "Bloquear/desbloquear cabinas", BLACK, LIGHTGRAY},
	{H_RECEIPT  , "Imprimir recibos", BLACK, LIGHTGRAY},
	{H_ACCUM    , "Imprimir acumulados de turno", BLACK, LIGHTGRAY},
	{H_OPERATION, "Operación manual/automática de tarificación", BLACK, LIGHTGRAY},
	{H_FORMS    , "Formatos de recibos para impresora", BLACK, LIGHTGRAY},
	{H_QUIT     , "Salir de SmartTar (F3)", LIGHTRED, LIGHTGRAY},
	{H_MD       , "MicroDiseño Ltda: microdis@geo.net (Ctrl+F1)", LIGHTGREEN, LIGHTGRAY},
	// forms
	{HB_80_FORM, "EPSON FX, LX, LQ, etc.", BLACK, LIGHTGRAY},
	{HB_40_FORM, "EPSON TM-930", BLACK, LIGHTGRAY},
	{HB_18_FORM, "Star SP-312", BLACK, LIGHTGRAY},
	{HB_28_FORM, "EPSON TM-930, Star SP-312", BLACK, LIGHTGRAY},
	//
	{H_FILE_MENU  , "Menú de manejo de archivo", BLACK, LIGHTGRAY},
	{H_CONFIG_MENU, "Menú de configuración del sistema", BLACK, LIGHTGRAY},
	{H_INFO_MENU  , "Menú de información del sistema", BLACK, LIGHTGRAY},
	{H_PRINT_MENU , "Menú de impresión", BLACK, LIGHTGRAY},
	{H_HELP_MENU  , "Menú de ayudas del sistema", BLACK, LIGHTGRAY},
	{0, NULL, 0, 0}
};

HELP_INFO *UIW_VIEW::HelpInfo = __HelpInfo;

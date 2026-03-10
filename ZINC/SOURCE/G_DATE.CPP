//	Zinc Interface Library - G_DATE.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "ui_gen.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif
#if defined(ZIL_LINKBUG)
void z_date_dummy(void) { }		// Bug fix for broken linkers.
#endif

static char *_monthNames[] =
{
#if defined(__ENGLISH__)
	"Jan.",		"January",
	"Feb.",   "February",
	"Mar.",   "March",
	"Apr.",   "April",
	"May",    "May",
	"June",   "June",
	"July",   "July",
	"Aug.",		"August",
	"Sept.",	"September",
	"Oct.",		"October",
	"Nov.",		"November",
	"Dec.",		"December"
#else
	"Ene.", "Enero",
	"Feb.", "Febrero",
	"Mar.", "Marzo",
	"Abr.", "Abril",
	"May.", "Mayo",
	"Jun.", "Junio",
	"Jul.", "Julio",
	"Ago.", "Agosto",
	"Sep.", "Septiembre",
	"Oct.", "Octubre",
	"Nov.", "Noviembre",
	"Dic.", "Diciembre"
#endif
};
char **UI_DATE::monthTable = _monthNames;

static char *_dayNames[] =
{
#if defined(__ENGLISH__)
	"Sun.",		"Sunday",
	"Mon.",		"Monday",
	"Tues.",	"Tuesday",
	"Wed.",		"Wednesday",
	"Thurs.",	"Thursday",
	"Fri.",		"Friday",
	"Sat.",		"Saturday"
#else
	"Dom.",	"Domingo",
	"Lun.",	"Lunes",
	"Mar.",	"Martes",
	"Mie.",	"Miercoles",
	"Jue.",	"Jueves",
	"Vie.",	"Viernes",
	"Sab.",	"Sábado"
#endif
};
char **UI_DATE::dayTable = _dayNames;

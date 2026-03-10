//
// [PR_DRPRE.C]
//

#pragma hdrstop

#include <dll_defs.h>

//
// Config and others
//
BOOL PASCAL_EXPORT getConfigFmt(char far **format)
{
    BOOL ok = TRUE;
    *format =
        "\x1B\x40"                 // init
        "\x1B\x41\x06"             // line spacing
        "\n"
        "\x1B\x40"                 // init
        "\x1B\x43\x10"             // page size
        "\x1B\x21\x00"             // style
        "\x1B\x44\x02\x30\x00\xFF" // tabs
        ;
    return ok;
}

BOOL PASCAL_EXPORT getHeaderFmt(char far **format)
{
    BOOL ok = TRUE;
    *format =
        "\n\n\n"
        "\x1B\x21\x21""\t%s\n" // empresa
        "\x1B\x21\x05""\t%s <%s> %s %s\t%s <%s> %s %s\n" // ciudad, serial, fecha, hora
        ;
    return ok;
}

BOOL PASCAL_EXPORT getShortHeaderFmt(char far **format)
{
    BOOL ok = TRUE;
    *format =
        "\n\n\n"
        "\x1B\x21\x05""\t%s %s %s\t%s %s %s\n";  // ciudad, fecha, hora
    ;
    return ok;
}

BOOL PASCAL_EXPORT getPrePaidFmt(char far **format)
{
    BOOL ok = TRUE;
    *format =
        "\x1B\x21\x01""\tPrepago\tPrepago\n"
        "\tCabina: %d    \tCabina: %d\n"
        "\t Valor: %s%.2f\t Valor: %s%.2f\n"
        ;
    return ok;
}

BOOL PASCAL_EXPORT getRemFmt(char far **format)
{
    BOOL ok = TRUE;
    *format =
        "\x1B\x21\x11\t   Dev.: %s%.2f\n"
        ;
    return ok;
}

BOOL PASCAL_EXPORT getSummaryFmt(char far **format)
{
    BOOL ok = TRUE;
    *format =
        "\x1B\x21\x01""\t    LL.: %d\n"
        "\x1B\x21\x11""\tG.TOTAL: %s%.2f (D.: %s%.2f)\n"
        ;
    return ok;
}

BOOL PASCAL_EXPORT getFooterFmt(char far **format)
{
    BOOL ok = TRUE;
    *format =
        "\x1B\x21\x04""\t%s\n"
        ;
    return ok;
}

BOOL PASCAL_EXPORT getFFFmt(char far **format)
{
    BOOL ok = TRUE;
    *format =
        "\x0C"
        ;
    return ok;
}

BOOL PASCAL_EXPORT getLFFmt(char far **format)
{
    BOOL ok = TRUE;
    *format =
        "\x0C"
        ;
    return ok;
}

BOOL PASCAL_EXPORT getLogTitleFmt(char far **format)
{
    BOOL ok = TRUE;
    *format =
        "\x1B\x21\x01""\nBitácora:\n"
        ;
    return ok;
}

//
// Normal Receipts
//
BOOL PASCAL_EXPORT nrGetFmt(char far **format)
{
    BOOL ok = TRUE;
    *format =
		"\x1B\x21\x01""\t%s: %s Cab.: %s\t%s: %s Cab.: %s\n"
		"\x1B\x21\x05""\tTEL.: %s %s Dur.: %s\tTEL.: %s %s Dur.: %s\n"
		"\x1B\x21\x01""\tM. Cob.: %.1f  V. Mto.: %s%.2f [%d%%]\tM. Cob.: %.1f  V. Mto.: %s%.2f [%d%%]\n"
		"\t  Valor: %s%.2f  %s %.0f%%: %s%.2f\t  Valor: %s%.2f  %s %.0f%%: %s%.2f\n"
		"\x1B\x21\x11""\t  Total: %s%.2f %s\t  Total: %s%.2f %s\n"
		;
	return ok;
}


//
// Special Receipts
//

BOOL PASCAL_EXPORT srGetTelFmt(char far **format)
{
	BOOL ok = TRUE;
	*format =
		"\x1B\x21\x01""\t%s: %s Cab.: %s\t%s: %s Cab.: %s\n"
		"\x1B\x21\x05""\tTEL.: %s %s Dur.: %s\tTEL.: %s %s Dur.: %s\n"
        "\n"
        "\x1B\x21\x01""\tM. Cob.: %.1f  V. Mto.: %s%.2f [%d%%]\tM. Cob.: %.1f  V. Mto.: %s%.2f [%d%%]\n"
        "\t  Valor: %s%.2f  %s %.0f%%: %s%.2f\t  Valor: %s%.2f  %s %.0f%%: %s%.2f\n"
        "\x1B\x21\x11""\t  Total: %s%.2f  \t  Total: %s%.2f\n"
        ;
    return ok;
}

BOOL PASCAL_EXPORT srGetFaxFmt(char far **format)
{
    BOOL ok = TRUE;
    *format =
		"\x1B\x21\x01""\t%s:. %s\t%s: %s\n"
        "\x1B\x21\x05""\tFAX: %s %s\tFAX: %s %s\n"
        "\n"
        "\x1B\x21\x01""\t   Pag.: %d  V. Pag.: %s%.2f\t   Pag.: %d  V. Pag.: %s%.2f\n"
        "\t  Valor: %s%.2f  %s %.0f%%: %s%.2f\t  Valor: %s%.2f  %s %.0f%%: %s%.2f\n"
        "\x1B\x21\x11""\t  Total: %s%.2f  \t  Total: %s%.2f\n"
        ;
    return ok;
}

BOOL PASCAL_EXPORT srGetTelexFmt(char far **format)
{
    BOOL ok = TRUE;
    *format =
		"\x1B\x21\x01""\t%s: %s\t%s: %s\n"
		"\x1B\x21\x05""\tInternet\tInternet\n"
        "\n"
		"\x1B\x21\x01""\t Min.: %1.f Cob.: %.0f Tarifa: %s%.2f\t Min.: %1.f Cob.: %.0f Tarifa: %s%.2f\n"
        "\t  Valor: %s%.2f  %s %.0f%%: %s%.2f\t  Valor: %s%.2f  %s %.0f%%: %s%.2f\n"
        "\x1B\x21\x11""\t  Total: %s%.2f  \t  Total: %s%.2f\n"
        ;
    return ok;
}

BOOL PASCAL_EXPORT srGetMagneticCardFmt(char far **format)
{
    BOOL ok = TRUE;
    *format =
		"\x1B\x21\x01""\t%s: %s\t%s: %s\n"
        "\x1B\x21\x05""\tTARJ. MAGN.\tTARJ. MAGN.\n"
        "\x1B\x21\x05""\t[%d] %.2f, [%d] %.2f, [%d] %.2f, [%d] %.2f"
        "\t[%d] %.2f, [%d] %.2f, [%d] %.2f, [%d] %.2f\n"
        "\x1B\x21\x01""\tCant.: %d\tCant.: %d\n"
        "\tValor: %s%.2f %s %.0f%%: %s%.2f\tValor: %s%.2f %s %.0f%%: %s%.2f\n"
        "\x1B\x21\x11""\tTotal: %s%.2f  \tTotal: %s%.2f\n"
        ;
    return ok;
}

BOOL PASCAL_EXPORT srGetOtherFmt(char far **format)
{
    BOOL ok = TRUE;
    *format =
		"\x1B\x21\x01""\t%s: %s\t%s: %s\n"
        "\x1B\x21\x05""\tOTROS\tOTROS\n"
        "\x1B\x21\x01""\t%s\t%s\n"
        "\x1B\x21\x01""\tCant.: %d  V.U. : %.2f\tCant.: %d  V.U. : %.2f\n"
        "\t  Valor: %s%.2f  %s %.0f%%: %s%.2f\t  Valor: %s%.2f  %s %.0f%%: %s%.2f\n"
        "\x1B\x21\x11""\t  Total: %s%.2f  \t  Total: %s%.2f\n"
        ;
    return ok;
}

//
// Statistics
//
BOOL PASCAL_EXPORT stGetTurnTitleFmt(char far **format)
{
    BOOL ok = TRUE;
    *format =
        "\x1B\x21\x05"
        "\tAcumulado: %s\tAcumulado: %s\n"
        "\n"
        "\tOperador(a): %s\tOperador(a): %s\n"
        "\n"
        "\t De: %s %s A: %s %s\t De: %s %s A: %s %s\n"
		"\t%s: %d entre %ld y %ld\t%s: %d entre %ld y %ld\n"
        "\n"
        ;
    return ok;
}

BOOL PASCAL_EXPORT stGetNotTurnTitleFmt(char far **format)
{
    BOOL ok = TRUE;
    *format =
        "\x1B\x21\x05"
        "\tAcumulado: %s\tAcumulado: %s\n"
        "\n"
        "\t De: %s %s A: %s %s\t De: %s %s A: %s %s\n"
		"\t%s: %d entre %ld y %ld\t%s: %d entre %ld y %ld\n"
        "\n"
        ;
    return ok;
}

BOOL PASCAL_EXPORT stGetNotPaidFmt(char far **format)
{
    BOOL ok = TRUE;
    *format =
		"\t[   Recibos No Cobrados   ]\t[   Recibos No Cobrados   ]\n"
        "\t No cob.: %d %.2f\t No cob: %d %.2f\n"
        "\tPag rev.: %d %.2f\tPag rev: %d %.2f\n"
        "\tTotal: %.2f\tTotal: %.2f\n"
        "\n"
        ;
    return ok;
}

BOOL PASCAL_EXPORT stGetNormalFmt(char far **format)
{
	BOOL ok = TRUE;
	*format =
		"\t[   Recibos Cobrados   ]\t[   Recibos Cobrados   ]\n"
		"\tConvención\tConvención\n"
		"\tLl-Min Habl-Min Cob-Val\tLl-Min Habl-Min Cob-Val\n"
		"\t--- Servicios Automáticos ---\t--- Servicios Automáticos ---\n"
		"\tNacional:\tNacional:\n"
#if !defined(__EDA__)
		"\t %d %.1f %.1f %.2f\t %d %.1f %.1f %.2f\n"
		"\tCelular:\tCelular:\n"
		"\t %d %.1f %.1f %.2f\t %d %.1f %.1f %.2f\n"
#else
		"\t EDA-EDA: %d %.1f %.1f %.2f\t EDA-EDA: %d %.1f %.1f %.2f\n"
        "\t EDA-EPM: %d %.1f %.1f %.2f\t EDA-EPM: %d %.1f %.1f %.2f\n"
        "\t EDA-TEL: %d %.1f %.1f %.2f\t EDA-TEL: %d %.1f %.1f %.2f\n"
        "\t Celular: %d %.1f %.1f %.2f\t Celular: %d %.1f %.1f %.2f\n"
#endif
        "\tInternacional:\tInternacional:\n"
#if !defined(__EDA__)
        "\t %d %.1f %.1f %.2f\t %d %.1f %.1f %.2f\n"
#else
		"\t EDA-TEL: %d %.1f %.1f %.2f\t EDA-TEL: %d %.1f %.1f %.2f\n"
#endif
		"\n"
		"\t Sub.: %.2f\t Sub.: %.2f\n"
		"\t  %3s: %.2f\t  %3s: %.2f\n"
		"\tTotal: %.2f\tTotal: %.2f\n"
		"\n"
	;

	return ok;
}

BOOL PASCAL_EXPORT stGetNalSpecialFmt(char far **format)
{
	BOOL ok = TRUE;
	*format =
		"\t--- Servicios Especiales ---\t--- Servicios Especiales --- \n"
		"\tNacional:\tNacional:\n"
#if !defined(__EDA__)
		"\t Tel: %d %.2f\t Tel: %d %.2f\n"
        "\t Cel: %d %.2f\t Cel: %d %.2f\n"
        "\t Fax: %d %.0f %.2f\t Fax: %d %.0f %.2f\n"
#else
		"\t Tel:\t Tel:\n"
		"\t  EDA-EDA: %d %.2f\t  EDA-EDA: %d %.2f\n"
		"\t  EDA-EPM: %d %.2f\t  EDA-EPM: %d %.2f\n"
		"\t  EDA-TEL: %d %.2f\t  EDA-TEL: %d %.2f\n"
		"\t  Celular: %d %.2f\t  Celular: %d %.2f\n"
		"\t Fax:\t Fax:\n"
		"\t  EDA-EDA: %d %.0f %.2f\t  EDA-EDA: %d %.0f %.2f\n"
		"\t  EDA-EPM: %d %.0f %.2f\t  EDA-EPM: %d %.0f %.2f\n"
		"\t  EDA-TEL: %d %.0f %.2f\t  EDA-TEL: %d %.0f %.2f\n"
#endif
		"\t Internet: %d %.0f %.2f\t Internet: %d %.0f %.2f\n"
	;

	return ok;
}

int PASCAL_EXPORT stGetInterSpecialFmt(char far **format)
{
	BOOL ok = TRUE;
	*format =
		"\tInternacional:\tInternacional:\n"
#if !defined(__EDA__)
		"\t Tel: %d %.2f\t Tel: %d %.2f\n"
		"\t Fax: %d %.2f %.2f\t Fax: %d %.2f %.2f\n"
#else
"\t Tel:\t Tel:\n"
		"\t  EDA-TEL: %d %.2f\t  EDA-TEL: %d %.2f\n"
		"\t Fax:\t Fax:\n"
		"\t  EDA-TEL: %d %.0f %.2f\t  EDA-TEL: %d %.0f %.2f\n"
#endif
		;
	return ok;
}

BOOL PASCAL_EXPORT stGetOtherSpecialFmt(char far **format)
{
	BOOL ok = TRUE;
	*format =
		"\tVarios:\tVarios:\n"
		"\t Tarj. Mag.: %d %.2f\t Tarj. Mag.: %d %.2f\n"
		"\t  [%d] %.2f [%d] %.2f\t  [%d] %.2f [%d] %.2f\n"
		"\t  [%d] %.2f [%d] %.2f\t  [%d] %.2f [%d] %.2f\n"
		"\t Otros: %d %.2f\t Otros: %d %.2f\n"
		;
	return ok;
}

BOOL PASCAL_EXPORT stGetEDATotalFmt(char far **format)
{
	BOOL ok = TRUE;
	*format =
        "\tTOTALES EDA:\tTOTALES EDA:\n"
        "\t EDA-EDA\t EDA-EDA\n"
        "\t  Sub.: %.2f\t  Sub.: %.2f\n"
        "\t   %3s: %.2f\t   %3s: %.2f\n"
        "\t Total: %.2f\t Total: %.2f\n"
        "\t EDA-EPM\t EDA-EPM\n"
        "\t  Sub.: %.2f\t  Sub.: %.2f\n"
        "\t   %3s: %.2f\t   %3s: %.2f\n"
        "\t Total: %.2f\t Total: %.2f\n"
        "\t EDA-TEL\t EDA-TEL\n"
        "\t  Sub.: %.2f\t  Sub.: %.2f\n"
        "\t   %3s: %.2f\t   %3s: %.2f\n"
        "\t Total: %.2f\t Total: %.2f\n"
        //
        "\t Celular\t Celular\n"
        "\t  Sub.: %.2f\t  Sub.: %.2f\n"
        "\t   %3s: %.2f\t   %3s: %.2f\n"
        "\t Total: %.2f\t Total: %.2f\n"
        ;
    return ok;
}

BOOL PASCAL_EXPORT stGetSpecialTotalFmt(char far **format)
{
    BOOL ok = TRUE;
    *format =
        "\n"
        "\t Sub.: %.2f\t Sub.: %.2f\n"
        "\t  %3s: %.2f\t  %3s: %.2f\n"
        "\tTotal: %.2f\tTotal: %.2f\n"
        "\n"
        ;
    return ok;
}


BOOL PASCAL_EXPORT stGetTotalFmt(char far **format)
{
    BOOL ok = TRUE;
    *format =
        "\n"
        "\x1B\x21\x11"
        "\tEFECTIVO\tEFECTIVO\n"
        "\t G.SUB.: %s%.2f\t G.SUB.: %s%.2f\n"
        "\t  G.%3s: %s%.2f\t  G.%3s: %s%.2f\n"
        "\tG.TOTAL: %s%.2f\tG.TOTAL: %s%.2f\n"
        ;
    return ok;
}

BOOL PASCAL_EXPORT stGetDoublePrnFmt(char far **format)
{
    BOOL ok = TRUE;
    *format =
        "\n"
        "\tCaja 1: [%u] %.2f""\tCaja 1: [%u] %.2f""\n"
        "\tCaja 2: [%u] %.2f""\tCaja 2: [%u] %.2f""\n"
        ;
    return ok;
}

/*
BOOL PASCAL_EXPORT get(char far **format)
{
	BOOL ok = TRUE;
	*format =
    ""
  ;
	return ok;
}
*/


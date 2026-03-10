//
// [PR_SR40.C]
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
        "\x1B\x40"
        "\xFF"
        ;
    return ok;
}

BOOL PASCAL_EXPORT getHeaderFmt(char far **format)
{
    BOOL ok = TRUE;
    *format =
        "\x1B\x21\x18""%s\n" // empresa
        "\x1B\x21\x01""%s\n" // id
        "\x1B\x63\x30\x03"
        "\x1B\x7A\x01""%s %s %s\n" // ciudad, fecha, hora
        "%s\n"
        "<%s>  "; //  serial

    return ok;
}

BOOL PASCAL_EXPORT getShortHeaderFmt(char far **format)
{
    BOOL ok = TRUE;
    *format =
        "\x1B\x63\x30\x03\x1B\x7A\x01%s %s %s\n"
        ;
    return ok;
}

BOOL PASCAL_EXPORT getPrePaidFmt(char far **format)
{
    BOOL ok = TRUE;
    *format =
        "\x1B\x21\x01""Prepago\n"
        " Cabina: %d\n"
        "  Valor: %s%.2f\n"
        ;
    return ok;
}

BOOL PASCAL_EXPORT getRemFmt(char far **format)
{
    BOOL ok = TRUE;
    *format =
        "\x1B\x21\x21""  Dev.: %s%.2f"
        "\x1B\x21\x01\n"
        ;
    return ok;
}

BOOL PASCAL_EXPORT getSummaryFmt(char far **format)
{
    BOOL ok = TRUE;
    *format =
        "\n"
        "\x1B\x21\x01""    LL.: %d\n"
        "\x1B\x21\x21""G.TOTAL: %s%.2f\n"
        "\x1B\x21\x01""   Dev.: %s%.2f\n"
        ;
    return ok;
}

BOOL PASCAL_EXPORT getFooterFmt(char far **format)
{
    BOOL ok = TRUE;
    *format =
        "\x1B\x21\x01\n"
        "\x1B\x21\x01%s\n"
        ;
    return ok;
}

BOOL PASCAL_EXPORT getFFFmt(char far **format)
{
    BOOL ok = TRUE;
    *format =
        "\n\n\n\n\n\n\n\n\n\x1B\x69"
        ;
    return ok;
}

BOOL PASCAL_EXPORT getLFFmt(char far **format)
{
    BOOL ok = TRUE;
    *format =
        "\n\n"
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
		"\x1B\x21\x01""%s: %s Cab.: %s\n"
        "Tel.: %s %s\n"
        "Dur.: %s  M.Cob.: %.1f\n"
        "V. Mto: %s%.2f [%d%%]\n"
        " Valor: %s%.2f  %s %.0f%%: %s%.2f\n"
        "\x1B\x21\x21"" Total: %s%.2f%s""\x1B\x21\x01\n"
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
		"%s: %s Cab.: %s\n"
		"\x1B\x21\x01""TEL.: %s %s\n"
		"\n"
		"Dur.: %s\n"
		"\x1B\x21\x01""M.Cob.: %.1f\n"
		"V. Mto: %s%.2f [%d%%]\n"
		"\n"
		"  Valor: %s%.2f\n"
		"%s %.0f%%: %s%.2f\n"
		"\x1B\x21\x21""  Total: %s%.2f\n"
		;
	return ok;
}

BOOL PASCAL_EXPORT srGetFaxFmt(char far **format)
{
	BOOL ok = TRUE;
	*format =
		"%s: %s\n"
		"\x1B\x21\x01""FAX: %s %s\n"
		"\n"
		"   Pag.: %d\n"
		"\x1B\x21\x01""V. Pag.: %s%.2f\n"
		"\n"
		"  Valor: %s%.2f\n"
		"%s %.0f%%: %s%.2f\n"
		"\x1B\x21\x21""  Total: %s%.2f\n"
		;
	return ok;
}

BOOL PASCAL_EXPORT srGetTelexFmt(char far **format)
{
	BOOL ok = TRUE;
	*format =
					  "%s: %s\n"
		"\x1B\x21\x01""Internet\n"
					  "\n"
					  " Minutos: %.1f\n"
					  "Cobrados: %.0f\n"
		"\x1B\x21\x01""  Tarifa: %s%.2f\n"
					  "\n"
					  "   Valor: %s%.2f\n"
					  " %s %.0f%%: %s%.2f\n"
					  "\x1B\x21\x21""Total: %s%.2f\n"
	;

    return ok;
}

BOOL PASCAL_EXPORT srGetMagneticCardFmt(char far **format)
{
    BOOL ok = TRUE;
    *format =
		"%s: %s\n"
        "\x1B\x21\x01TARJ. MAGN.\n"
        "\n"
        "\x1B\x21\x05""\t[%d]  %.2f, [%d] %.2f\n"
        "\x1B\x21\x05""\t[%d] %.2f, [%d] %.2f\n"
        "Cant.: %d\n"
        "\n"
        "  Valor: %s%.2f\n"
        "%s %.0f%%: %s%.2f\n"
        "\x1B\x21\x21""  Total: %s%.2f\n"
        ;
    return ok;
}

BOOL PASCAL_EXPORT srGetOtherFmt(char far **format)
{
    BOOL ok = TRUE;
    *format =
		"%s: %s\n"
        "\x1B\x21\x01""OTROS\n"
        "\n"
        "%s\n"
        "Cant.: %d\n"
        "V.U. : %.2f\n"
        "\n"
        "  Valor: %s%.2f\n"
        "%s %.0f%%: %s%.2f\n"
        "\x1B\x21\x21""  Total: %s%.2f\n"
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
        "Acumulado: %s\n"
        "\n"
        "Operador(a): %s\n"
        "\n"
        " De: %s %s A: %s %s\n"
		"%s: %d entre %ld y %ld\n"
        "\n"
        ;
    return ok;
}

BOOL PASCAL_EXPORT stGetNotTurnTitleFmt(char far **format)
{
    BOOL ok = TRUE;
    *format =
        "\x1B\x21\x05"
        "Acumulado: %s\n"
        "\n"
        " De: %s %s A: %s %s\n"
		"%s: %d entre %ld y %ld\n"
        "\n"
        ;
    return ok;
}

BOOL PASCAL_EXPORT stGetNotPaidFmt(char far **format)
{
    BOOL ok = TRUE;
    *format =
		"[   Recibos No Cobrados  ]\n"
        " No cob.: %d %.2f\n"
        "Pag rev.: %d %.2f\n"
        "Total: %.2f\n\n"
        ;
    return ok;
}

BOOL PASCAL_EXPORT stGetNormalFmt(char far **format)
{
    BOOL ok = TRUE;
    *format =
		"[   Recibos Cobrados  ]\n"
        "Convención\n"
        "Ll-Min Habl-Min Cob-Val\n"
        "---Servicios Automáticos---\n"
        "Nacional:\n"
#if !defined(__EDA__)
        " %d %.1f %.1f %.2f\n"
        "Celular:\n"
        " %d %.1f %.1f %.2f\n"
#else
" EDA-EDA: %d %.1f %.1f %.2f\n"
        " EDA-EPM: %d %.1f %.1f %.2f\n"
        " EDA-TEL: %d %.1f %.1f %.2f\n"
        " Celular: %d %.1f %.1f %.2f\n"
#endif
        "Internacional:\n"
#if !defined(__EDA__)
        " %d %.1f %.1f %.2f\n"
#else
        " EDA TEL: %d %.1f %.1f %.2f\n"
#endif
        "\n"
        " Sub.: %.2f\n"
        "  %3s: %.2f\n"
        "Total: %.2f\n"
        "\n"
        ;
    return ok;
}

BOOL PASCAL_EXPORT stGetNalSpecialFmt(char far **format)
{
	BOOL ok = TRUE;
	*format =
		"---Servicios Especiales---\n"
		"Nacional:\n"
#if !defined(__EDA__)
		" Tel: %d %.2f\n"
		" Cel: %d %.2f\n"
		" Fax: %d %.0f %.2f\n"
#else
		" Tel:\n"
		"  EDA-EDA: %d %.2f\n"
		"  EDA-EPM: %d %.2f\n"
		"  EDA-TEL: %d %.2f\n"
		"  Celular: %d %.2f\n"
		" Fax:\n"
		"  EDA-EDA: %d %.0f %.2f\n"
		"  EDA-EPM: %d %.0f %.2f\n"
		"  EDA-TEL: %d %.0f %.2f\n"
#endif
		" Internet: %d %.0f %.2f\n"
	;

	return ok;
}

int PASCAL_EXPORT stGetInterSpecialFmt(char far **format)
{
    BOOL ok = TRUE;
    *format =
        "Internacional:\n"
#if !defined(__EDA__)
        " Tel: %d %.2f\n"
        " Fax: %d %.0f %.2f\n"
#else
" Tel:\n"
        "  EDA-TEL: %d %.2f\n"
        " Fax:\n"
        "  EDA-TEL: %d %.0f %.2f\n"
#endif
		;
    return ok;
}

BOOL PASCAL_EXPORT stGetOtherSpecialFmt(char far **format)
{
    BOOL ok = TRUE;
    *format =
        "\tVarios:\n"
        " Tarj. Mag.: %d %.2f\n"
        "  [%d] %.2f [%d] %.2f\n"
        "  [%d] %.2f [%d] %.2f\n"
        " Otros: %d %.2f\n"
        ;
    return ok;
}

BOOL PASCAL_EXPORT stGetEDATotalFmt(char far **format)
{
    BOOL ok = TRUE;
    *format =
        "TOTALES EDA:\n"
        " EDA-EDA\n"
        "  Sub.: %.2f\n"
        "   %3s: %.2f\n"
        " Total: %.2f\n"
        " EDA-EPM\n"
        "  Sub.: %.2f\n"
        "   %3s: %.2f\n"
        " Total: %.2f\n"
        " EDA-TEL\n"
        "  Sub.: %.2f\n"
        "   %3s: %.2f\n"
        " Total: %.2f\n"
        " Celular\n"
        "  Sub.: %.2f\n"
        "   %3s: %.2f\n"
        " Total: %.2f\n"
        ;
    return ok;
}

BOOL PASCAL_EXPORT stGetSpecialTotalFmt(char far **format)
{
    BOOL ok = TRUE;
    *format =
        "\n"
        " Sub.: %.2f\n"
        "  %3s: %.2f\n"
        "Total: %.2f\n"
        "\n"
        ;
    return ok;
}


BOOL PASCAL_EXPORT stGetTotalFmt(char far **format)
{
    BOOL ok = TRUE;
    *format =
        "\n"
        "\x1B\x21\x21"
        "EFECTIVO\n"
        " G.SUBT: %s%.2f\n"
        "  G.%3s: %s%.2f\n"
        "G.TOTAL: %s%.2f\n"
        ;
    return ok;
}

BOOL PASCAL_EXPORT stGetDoublePrnFmt(char far **format)
{
    BOOL ok = TRUE;
    *format =
        "\n"
        "Caja 1: [%u] %.2f""\n"
        "Caja 2: [%u] %.2f""\n"
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


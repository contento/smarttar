//
// [PR_SR28.C]
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
        "%s\n"      // empresa
        "%s\n"      // id
        "\n"
        "%s <%s>\n" // ciudad, serial, fecha, hora
        "%s %s\n"
        "%s" // legal
        "\n"
        ;
    return ok;
}

BOOL PASCAL_EXPORT getShortHeaderFmt(char far **format)
{
    BOOL ok = TRUE;
    *format =
        "%s\n"
        "%s %s\n"
        ;
    return ok;
}

BOOL PASCAL_EXPORT getPrePaidFmt(char far **format)
{
    BOOL ok = TRUE;
    *format =
        "\x1B\x4D""Prepago\n"
        "Cabina: %d\n"
        " Valor: %s%.2f\n"
        ;
    return ok;
}

BOOL PASCAL_EXPORT getRemFmt(char far **format)
{
    BOOL ok = TRUE;
    *format =
        "\x1B\x50"" Dev.: %s%.2f\n"
        ;
    return ok;
}

BOOL PASCAL_EXPORT getSummaryFmt(char far **format)
{
    BOOL ok = TRUE;
    *format =
        "\n"
        "    LL.: %d\n"
        "\x1B\x50""G.TOTAL: %s%.2f\n"
        "   Dev.: %s%.2f\n"
        ;
    return ok;
}

BOOL PASCAL_EXPORT getFooterFmt(char far **format)
{
    BOOL ok = TRUE;
    *format =
        "\n"
        "\x1B\x50%s\n"
        ;
    return ok;
}

BOOL PASCAL_EXPORT getFFFmt(char far **format)
{
    BOOL ok = TRUE;
    *format =
        "\n\n\n\n\n\n\n"
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
		"%s: %s Cab.: %s\n"
		"\x1B\x4D""TEL.: %s\n"
		"Loc.: %s\n"
		"\n"
		"Dur.: %s\n"
		"M.C.: %.1f\n"
		"V.M.: %s%.1f [%d%%]\n"
		"\n"
		"  Valor: %s%.2f\n"
		"%s%.0f%%: %s%.2f\n"
		"\x1B\x50""Total:%s%.2f%s\n"
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
		"\x1B\x4D""TEL.: %s\n"
		"Loc.: %s\n"
		"\n"
		"Dur.: %s\n"
		"M.C.: %.1f\n"
		"V.M.: %s%.1f [%d%%]\n"
		"\n"
		"  Valor: %s%.2f\n"
		"%s%.0f%%: %s%.2f\n"
		"\x1B\x50""Total: %s%.2f\n"
		;
	return ok;
}

BOOL PASCAL_EXPORT srGetFaxFmt(char far **format)
{
	BOOL ok = TRUE;
	*format =
		"%s: %s\n"
		"\x1B\x4D"" FAX: %s\n"
		" Loc: %s\n"
		"\n"
		"Pag.: %d\n"
		"V.P.: %s%.1f\n"
		"\n"
		"  Valor: %s%.2f\n"
		"%s%.0f%%: %s%.2f\n"
		"\x1B\x50""  Total: %s%.2f\n"
		;
	return ok;
}

BOOL PASCAL_EXPORT srGetTelexFmt(char far **format)
{
	BOOL ok = TRUE;
	*format =
		"%s: %s\n"
		"\x1B\x4D""Internet\n"
		"\n"
		"  Min.: %.1f\n"
		"  Cob.: %.0f\n"
		"Tarifa: %s%.1f\n"
		"\n"
		"  Valor: %s%.2f\n"
		"%s %.0f%%: %s%.2f\n"
		"\x1B\x50""  Total: %s%.2f\n"
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
		"\x1B\x50""  Total: %s%.2f\n"
		;
	return ok;
}

BOOL PASCAL_EXPORT srGetOtherFmt(char far **format)
{
	BOOL ok = TRUE;
	*format =
		"%s: %s\n"
		"\x1B\x4D""OTROS\n"
		"\n"
		"%s\n"
		"Cant.: %d\n"
		"V.U. : %.2f\n"
		"\n"
		"  Valor: %s%.2f\n"
		"%s%.0f%%: %s%.2f\n"
		"\x1B\x50""  Total: %s%.2f\n"
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
		"Acumulado: %s\n"
		"\n"
		"Operador(a):\n"
		"%s\n"
		"\n"
		" De: %s %s\n"
        "  A: %s %s\n"
		"%s: %d entre %ld y %ld\n\n"
        ;
    return ok;
}

BOOL PASCAL_EXPORT stGetNotTurnTitleFmt(char far **format)
{
    BOOL ok = TRUE;
    *format =
        "Acumulado: %s\n"
        "\n"
        " De: %s %s\n"
        "  A: %s %s\n"
		"%s: %d entre %ld y %ld\n\n"
        ;
    return ok;
}

BOOL PASCAL_EXPORT stGetNotPaidFmt(char far **format)
{
    BOOL ok = TRUE;
    *format =
		"[  Recibos no cobrados  ]\n"
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
		"[  Recibos cobrados  ]\n"
        "Convención\n"
        "Ll-Min Habl-Min Cob-Val\n"
        "---Servicios Automáticos---\n"
        "Nacional:\n"
#if !defined(__EDA__)
        " %d %.1f %.1f %.2f\n"
        "Celular:\n"
        " %d %.1f %.1f %.2f\n"
#else
" EDA-EDA:\n"
        " %d %.1f %.1f %.2f\n"
        " EDA-EPM:\n"
        " %d %.1f %.1f %.2f\n"
        " EDA-TEL:\n"
        " %d %.1f %.1f %.2f\n"
        " Celular:\n"
        " %d %.1f %.1f %.2f\n"
#endif
        "Internacional:\n"
#if !defined(__EDA__)
        " %d %.1f %.1f %.2f\n"
#else
        " EDA-TEL:\n"
        " %d %.1f %.1f %.2f\n"
#endif
        "\n"
        " Sub.: %.2f\n"
        "  %3s: %.2f\n"
        "Total: %.2f\n"
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
        " EDA-EDA:\n"
        " %d %.2f \n"
        " EDA-EPM:\n"
        " %d %.2f\n"
        " EDA-TEL:\n"
        " %d %.2f\n"
        " Celular:\n"
        " %d %.2f\n"
        " Fax:\n"
        " EDA-EDA:\n"
        " %d %.0f %.2f\n"
        " EDA-EPM:\n"
        " %d %.0f %.2f\n"
        " EDA-TEL:\n"
        " %d %.0f %.2f\n"
#endif
		" Int.: %d %.0f %.2f\n"
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
		" Tel:"
		" EDA-TEL:\n"
		" %d %.2f\n"
		" Fax:"
		" EDA-TEL:\n"
		" %d %.0f %.2f\n"
#endif
		;
    return ok;
}

BOOL PASCAL_EXPORT stGetOtherSpecialFmt(char far **format)
{
    BOOL ok = TRUE;
    *format =
        "Varios:\n"
        " Tarj. Mag.: %d %.2f\n"
        "  [%d]%.2f [%d]%.2f\n"
        "  [%d]%.2f [%d]%.2f\n"
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
        "EFECTIVO\n"
        "  G.SUB:%s%.2f\n"
        "  G.%3s:%s%.2f\n"
        "G.TOTAL:%s%.2f\n"
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


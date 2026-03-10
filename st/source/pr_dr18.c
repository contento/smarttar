//
// [PR_DR18.C]
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
        "\x1B\x30"
        "\x1B\x4D\x1B\x44\x14\x00\xFF"
        ;
    return ok;
}

BOOL PASCAL_EXPORT getHeaderFmt(char far **format)
{
    BOOL ok = TRUE;
    *format =
        "\x1B\x50\x0E""%s\n" // empresa
        "\x14\x1B\x4D""%s\n" // id
        "\n"
        "%s\t%s\n" // ciudad, serial, fecha, hora
        "<%s>\t<%s>\n"
        "%s %s\t%s %s\n"
        "%s"
        "\n";
    return ok;
}

BOOL PASCAL_EXPORT getShortHeaderFmt(char far **format)
{
    BOOL ok = TRUE;
    *format =
        "%s\t%s\n"
        "%s %s\t%s %s\n"
        ;
    return ok;
}

BOOL PASCAL_EXPORT getPrePaidFmt(char far **format)
{
    BOOL ok = TRUE;
    *format =
        "Prepago\tPrepago\n"
        "Cabina: %d\tCabina: %d\n"
        " Valor: %s%.2f\t Valor: %s%.2f\n"
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
        "\x1B\x4D%s\n"
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
        "\x14\x1B\x4D""\nBitácora:\n"
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
		"\x1B\x50""%s: %s\t%s: %s\n"
		"Cab.: %s\tCab.: %s\n"
		"\x1B\x4D""TEL.: %s\tTEL.: %s\n"
		"%s\t%s\n"
		"\n"
		"Dur.: %s\tDur.: %s\n"
		"M.C.: %.1f\tM.C.  %.1f\n"
		"V.M.: %s%.1f [%d%%]\tV.M.: %s%.1f [%d%%]\n"
		"\n"
		"  Valor: %s%.2f \tValor: %s%.2f\n"
		"%s%.0f%%: %s%.2f\t%s%.0f%%: %s%.2f\n"
		"\x1B\x50""Tot:%s%.2f%s\tTot:%s%.2f%s\n"
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
		"\x1B\x50""%s: %s\t%s: %s\n"
		"Cab.: %s\tCab.: %s\n"
		"\x1B\x4D""TEL.: %s\tTEL.: %s\n"
		"%s\t%s\n"
		"\n"
		"Dur.: %s\tDur.: %s\n"
		"M.C.: %.1f\tM.C.  %.1f\n"
		"V.M.: %s%.1f [%d%%]\tV.M.: %s%.1f [%d%%]\n"
		"\n"
		"  Valor: %s%.2f\tValor: %s%.2f\n"
		"%s%.0f%%: %s%.2f\t%s%.0f%%: %s%.2f\n"
		"\x1B\x50""Tot: %s%.2f\tTot: %s%.2f\n"
		;
	return ok;
}

BOOL PASCAL_EXPORT srGetFaxFmt(char far **format)
{
	BOOL ok = TRUE;
	*format =
		"\x1B\x50""%s: %s\t%s: %s\n"
		"FAX: %s\tFAX: %s\n"
		"%s\t%s"
		"\n"
		"\x1B\x4D""   Pag.: %d\t   Pag.: %d\n"
		"V. Pag.: %s%.2f\tV. Pag.: %s%.2f\n"
		"\n"
		"  Valor: %s%.2f \tValor: %s%.2f\n"
		"%s%.0f%%: %s%.2f\t%s%.0f%%: %s%.2f\n"
		"\x1B\x50""Tot: %s%.2f\tTot: %s%.2f\n"
		;
	return ok;
}

BOOL PASCAL_EXPORT srGetTelexFmt(char far **format)
{
	BOOL ok = TRUE;
	*format =
		"\x1B\x50""%s: %s\t%s: %s\n"
				  "Internet\tInternet\n"
				  "\n"
		"\x1B\x4D""  Min.: %.1f\t  Min.: %.1f\n"
				  "  Cob.: %.0f\t  Cob.: %.0f\n"
				  "Tarifa: %s%.2f\tTarifa: %s%.2f\n"
				  "\n"
				  "  Valor: %s%.2f \tValor: %s%.2f\n"
				  "%s%.0f%%: %s%.2f\t%s%.0f%%: %s%.2f\n"
		"\x1B\x50""Tot: %s%.2f\tTot: %s%.2f\n"
	;
    return ok;
}

BOOL PASCAL_EXPORT srGetMagneticCardFmt(char far **format)
{
    BOOL ok = TRUE;
    *format =
		"\x1B\x50""%s: %s\t%s: %s\n"
		"T. MAGN.\tT. MAGN.\n"
		"\n"
		"[%d] %.2f\t[%d] %.2f\n"
		"[%d] %.2f\t[%d] %.2f\n"
		"[%d] %.2f\t[%d] %.2f\n"
		"[%d] %.2f\t[%d] %.2f\n"
		"\x1B\x4D""Cant.: %d\tCant.: %d\n"
		"\n"
		" Valor: %s%.2f \tValor: %s%.2f\n"
		"%s%.0f%%: %s%.2f\t%s%.0f%%: %s%.2f\n"
		"\x1B\x50""Tot: %s%.2f\tTot: %s%.2f\n"
		;
	return ok;
}

BOOL PASCAL_EXPORT srGetOtherFmt(char far **format)
{
	BOOL ok = TRUE;
	*format =
		"\x1B\x50""%s: %s\t%s: %s\n"
		"OTROS\tOTROS\n"
		"\n"
		"%s\t%s\n"
		"\x1B\x4D""Cant.: %d\tCant.: %d\n"
		"\x1B\x4D""V.U. : %.2f\tV.U. : %.2f\n"
		"\n"
		" Valor: %s%.2f \tValor: %s%.2f\n"
		"%s%.0f%%: %s%.2f\t%s%.0f%%: %s%.2f\n"
        "\x1B\x50""Tot: %s%.2f\tTot: %s%.2f\n"
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
        "\x1B\x4D"
        "Acumulado:\tAcumulado:\n"
        "  %s\t  %s\n"
        "\n"
        "Operador(a):\tOperador(a)\n"
        " %s\t %s\n"
        "\n"
        "De:\tDe:\n"
        " %s %s\t %s %s\n"
        "A:\tA:\n"
        " %s %s\t %s %s\n"
		"%s: %d\t%s: %d\n"
        " %ld A: %ld\t %ld A: %ld\n"
        "\n"
        ;
    return ok;
}

BOOL PASCAL_EXPORT stGetNotTurnTitleFmt(char far **format)
{
    BOOL ok = TRUE;
    *format =
        "\x1B\x4D"
        "Acumulado:\tAcumulado:\n"
        "  %s\t  %s\n"
        "\n"
        "De:\tDe:\n"
        " %s %s\t %s %s\n"
        "A:\tA:\n"
        " %s %s\t %s %s\n"
		"%s: %d\t%s: %d\n"
        " %ld A: %ld\t %ld A: %ld\n"
        "\n"
        ;
    return ok;
}

BOOL PASCAL_EXPORT stGetNotPaidFmt(char far **format)
{
    BOOL ok = TRUE;
    *format =
        "[   No cobrados  ]\t[   No cobrados  ]\n"
        "No .Cob.:\tNo. Cob.:\n"
        " %d %.2f\t %d %.2f\n"
        "Pag. Rev.:\tPag.Rev.:\n"
        " %d %.2f\t %d %.2f\n"
        "Total: %.2f\tTotal: %.2f\n\n"
        ;
    return ok;
}

BOOL PASCAL_EXPORT stGetNormalFmt(char far **format)
{
    BOOL ok = TRUE;
    *format =
        "[   Cobrados  ]\t[   Cobrados  ]\n"
        "--- S. Automat. ---\t--- S. Automat ---\n"
        "Convención\tConvención\n"
        "Ll-M Habl-M Cb Vl\tLl-M Habl-M Cb Vl\n"
        "Nal.:\tNal.:\n"
#if !defined(__EDA__)
        " %d %.1f %.1f %.2f\t %d %.1f %.1f %.2f\n"
        "Cel.:\tCel.:\n"
        " %d %.1f %.1f %.2f\t %d %.1f %.1f %.2f\n"
#else
" EDA-EDA:\t EDA-EDA:\n"
        " %d %.1f %.1f %.2f\t %d %.1f %.1f %.2f\n"
        " EDA-EPM:\t EDA-EPM:\n"
        " %d %.1f %.1f %.2f\t %d %.1f %.1f %.2f\n"
        " EDA-TEL:\t EDA-TEL:\n"
        " %d %.1f %.1f %.2f\t %d %.1f %.1f %.2f\n"
        " Celular:\t Celular:\n"
        " %d %.1f %.1f %.2f\t %d %.1f %.1f %.2f\n"
#endif
        "Inter.:\tInter.:\n"
#if !defined(__EDA__)
        " %d %.1f %.1f %.2f\t %d %.1f %.1f %.2f\n"
#else
" EDA-TEL:\t EDA-TEL:\n"
        " %d %.1f %.1f %.2f\t %d %.1f %.1f %.2f\n"
#endif
        "\n"
        "Sub.:\tSub.:\n"
        " %.2f\t %.2f\n"
        "%3s:\t%3s:\n"
        " %.2f\t %.2f\n"
        "Total:\tTotal:\n"
        " %.2f\t %.2f\n"
        "\n"
        ;
    return ok;
}

BOOL PASCAL_EXPORT stGetNalSpecialFmt(char far **format)
{
    BOOL ok = TRUE;
    *format =
        "--- S. Espec. ---\t--- S.Espec. ---\n"
        "Nal.:\tNal.:\n"
#if !defined(__EDA__)
        " Tel:\t Tel:\n"
        " %d %.2f\t %d %.2f\n"
        " Cel:\t Cel:\n"
        " %d %.2f\t %d %.2f\n"
        " Fax:\t Fax:\n"
        " %d %.0f %.2f\t %d %.0f %.2f\n"
#else
        " Tel:\t Tel:\n"
        " EDA-EDA:\t EDA-EDA:\n"
        " %d %.2f\t %d %.2f\n"
        " EDA-EPM:\t EDA-EPM:\n"
        " %d %.2f\t %d %.2f\n"
        " EDA-TEL:\t EDA-TEL:\n"
        " %d %.2f\t %d %.2f\n"
        " Celular:\t Celular:\n"
        " %d %.2f\t %d %.2f\n"
        " Fax:\t Fax:\n"
        " EDA-EDA:\t EDA-EDA:\n"
        " %d %.0f %.2f\t %d %.0f %.2f\n"
        " EDA-EPM:\t EDA-EPM:\n"
        " %d %.0f %.2f\t %d %.0f %.2f\n"
        " EDA-TEL:\t EDA-TEL:\n"
        " %d %.0f %.2f\t %d %.0f %.2f\n"
#endif
		" Internet:\t Internet:\n"
		" %d %.0f %.2f\t %d %.0f %.2f\n"
        ;
    return ok;
}

int PASCAL_EXPORT stGetInterSpecialFmt(char far **format)
{
	BOOL ok = TRUE;
	*format =
		"Inter.:\tInter.:\n"
#if !defined(__EDA__)
		" Tel:\t Tel:\n"
		" %d %.2f\t %d %.2f\n"
		" Fax:\t Fax:\n"
		" %d %.0f %.2f\t %d %.0f %.2f\n"
#else
		" Tel:\t Tel:\n"
		" EDA-TEL:\t EDA-TEL:\n"
		" %d %.2f\t %d %.2f\n"
		" Fax:\t Fax:\n"
		" EDA-TEL:\t EDA-TEL:\n"
		" %d %.0f %.2f\t %d %.0f %.2f\n"
#endif
		" %d %.0f %.2f\t %d %.0f %.2f\n"
	;
	return ok;
}

BOOL PASCAL_EXPORT stGetOtherSpecialFmt(char far **format)
{
    BOOL ok = TRUE;
    *format =
        "Varios:\tVarios:\n"
        " Tarj. Mag.:\tTarj. Mag.:\n"
        " %d %.2f\t %d %.2f\n"
        " [%d]%.2f\t [%d]%.2f\n"
        " [%d]%.2f\t [%d]%.2f\n"
        " [%d]%.2f\t [%d]%.2f\n"
        " [%d]%.2f\t [%d]%.2f\n"
        " Otros:\tOtros:\n"
        " %d %.2f\t %d %.2f\n"
        ;
    return ok;
}

BOOL PASCAL_EXPORT stGetEDATotalFmt(char far **format)
{
    BOOL ok = TRUE;
    *format =
        "TOTALES EDA:\tTOTALES EDA:\n"
        "EDA-EDA\tEDA-EDA\n"
        " Sub.: %.2f\t Sub.: %.2f\n"
        "  %3s: %.2f\t  %3s: %.2f\n"
        "Total: %.2f\tTotal: %.2f\n"
        "EDA-EPM\tEDA-EPM\n"
        " Sub.: %.2f\t Sub.: %.2f\n"
        "  %3s: %.2f\t  %3s: %.2f\n"
        "Total: %.2f\tTotal: %.2f\n"
        "EDA-TEL\tEDA-TEL\n"
        " Sub.: %.2f\t Sub.: %.2f\n"
        "  %3s: %.2f\t  %3s: %.2f\n"
        "Total: %.2f\tTotal: %.2f\n"
        "Celular\tCelular\n"
        " Sub.: %.2f\t Sub.: %.2f\n"
        "  %3s: %.2f\t  %3s: %.2f\n"
        "Total: %.2f\tTotal: %.2f\n"
        ;
    return ok;
}

BOOL PASCAL_EXPORT stGetSpecialTotalFmt(char far **format)
{
    BOOL ok = TRUE;
    *format =
        "\n"
        "Sub.:\tSub.:\n"
        " %.2f\t %.2f\n"
        "%3s:\t%3s:\n"
        " %.2f\t %.2f\n"
        "Total:\tTotal:\n"
        " %.2f\t %.2f\n"
        "\n"
        ;
    return ok;
}


BOOL PASCAL_EXPORT stGetTotalFmt(char far **format)
{
    BOOL ok = TRUE;
    *format =
        "\n"
        "\x1B\x50"
        "EFECTIVO\tEFECTIVO"
        "G.SUB.:\tG.SUB.:\n"
        " %s%.2f\t %s%.2f\n"
        "G.%3s:\tG. %3s:\n"
        " %s%.2f\t %s%.2f\n"
        "G.TOTAL:\tG.TOTAL:\n"
        " %s%.2f\t %s%.2f\n"
        ;
    return ok;
}

BOOL PASCAL_EXPORT stGetDoublePrnFmt(char far **format)
{
    BOOL ok = TRUE;
    *format =
        "\n"
        "Caja 1:""\tCaja 1:\n"
        " [%u] %.2f""\t [%u] %.2f""\n"
        "Caja 2:""\tCaja 2:\n"
        " [%u] %.2f""\t [%u] %.2f""\n"
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


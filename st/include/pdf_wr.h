//
// [ PDF_WR.H ]
//
// Minimal PDF 1.4 bytestream writer for SmartTar.
// Generates one-receipt-per-page PDF files from printer-formatted text.
//

#ifndef __PDF_WR_H
#define __PDF_WR_H

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

// Page dimensions in PDF user units (1 unit = 1/72 inch).
// US Letter: 612 x 792 pts.
#define PDF_PAGE_W  612
#define PDF_PAGE_H  792

// Margins in points.
#define PDF_MARGIN_L  50
#define PDF_MARGIN_T  50

// Monospace font: Courier at 8 pts -> 4.8 pts/char horizontal,
// 9.6 pts/line vertical (leading).  Chosen to fit ~96 chars/line
// and ~66 lines/page -- close to 80-col receipt paper.
#define PDF_FONT_SIZE   8
#define PDF_CHAR_W     4.8
#define PDF_LINE_H     9.6

// Max text lines per page (derived from margins and line height).
#define PDF_MAX_LINES  ((PDF_PAGE_H - 2 * PDF_MARGIN_T) / (int)PDF_LINE_H)

// Opaque context -- caller allocates, we fill.
typedef struct PDF_WR PDF_WR;

// Open a new PDF file for writing.  Returns NULL on failure.
PDF_WR *pdf_wr_open(const char *path);

// Append a single text line (NUL-terminated, no trailing CR/LF).
// Handles page-break automatically when the page is full.
void pdf_wr_line(PDF_WR *ctx, const char *line);

// Insert a page break (form-feed).
void pdf_wr_page_break(PDF_WR *ctx);

// Close the file and free the context.  Must be called to produce
// a valid PDF.  Returns 0 on success, -1 on I/O error.
int pdf_wr_close(PDF_WR *ctx);

#ifdef __cplusplus
}
#endif

#endif // __PDF_WR_H

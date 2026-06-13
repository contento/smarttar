//
// [ PDF_WR.C ]
//
// Minimal PDF 1.4 bytestream writer.
// Produces a valid PDF from a sequence of text lines.
// Courier 8pt monospace, US Letter, automatic page breaks.
//
// Object layout (objects may appear in the file in any order as long
// as the xref offsets are correct):
//   obj 1 = Pages catalog   -- number reserved at open, body written
//                              at close (when Kids/Count are known, so
//                              no fixed-size placeholder can overflow).
//   obj 2 = Font (Courier)  -- written at open.
//   obj 3,5,7,... = Page    -- /Parent 1 0 R
//   obj 4,6,8,... = Content stream (/Length back-patched into a
//                                   fixed 10-digit field).
//   last obj      = Document catalog (/Type /Catalog /Pages 1 0 R).
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "pdf_wr.h"

// Capacity limits.  A day's receipts in automatic mode can run to many
// pages; cap generously and bound-check to avoid array overflow.
#define PDF_MAX_OBJS   1024
#define PDF_MAX_PAGES  ((PDF_MAX_OBJS - 4) / 2)

// --- Internal state ---------------------------------------------------------

struct PDF_WR
{
    FILE *fp;
    long  objOffsets[PDF_MAX_OBJS]; // byte offset of object N at index N-1
    int   objCount;                 // highest object number allocated
    int   pageObjs[PDF_MAX_PAGES];  // object number of each Page object
    int   pageCount;                // number of pages so far
    int   lineOnPage;               // 0-based line index within current page
    int   streamOpen;               // is a content stream currently open?
    long  lenFieldOffset;           // file offset of current /Length field
    long  streamStart;              // file offset of current stream's data
};

// --- Helpers ----------------------------------------------------------------

static void writeStr(FILE *fp, const char *s)
{
    fwrite(s, 1, strlen(s), fp);
}

// Allocate the next object number, record its offset, write its header.
static void beginObj(PDF_WR *ctx)
{
    long offset = ftell(ctx->fp);
    char buf[32];
    ctx->objCount++;
    sprintf(buf, "%d 0 obj\r\n", ctx->objCount);
    writeStr(ctx->fp, buf);
    if (ctx->objCount - 1 < PDF_MAX_OBJS)
        ctx->objOffsets[ctx->objCount - 1] = offset;
}

static void endObj(PDF_WR *ctx)
{
    writeStr(ctx->fp, "endobj\r\n");
}

// Close the current content stream and back-patch its /Length.
static void closeStream(PDF_WR *ctx)
{
    long streamLen;
    long save;

    if (!ctx->streamOpen)
        return;

    // Length = bytes between the "stream\r\n" EOL and "endstream".
    streamLen = ftell(ctx->fp) - ctx->streamStart;
    writeStr(ctx->fp, "endstream\r\n");
    endObj(ctx);

    save = ftell(ctx->fp);
    fseek(ctx->fp, ctx->lenFieldOffset, SEEK_SET);
    fprintf(ctx->fp, "%010ld", streamLen);
    fseek(ctx->fp, save, SEEK_SET);

    ctx->streamOpen = 0;
}

// --- New page ---------------------------------------------------------------

static void newPage(PDF_WR *ctx)
{
    char buf[64];

    // Refuse to overflow the object / page tables.
    if (ctx->pageCount >= PDF_MAX_PAGES || ctx->objCount + 2 >= PDF_MAX_OBJS)
        return;

    // Page object.
    beginObj(ctx);
    ctx->pageObjs[ctx->pageCount++] = ctx->objCount;
    writeStr(ctx->fp, "<< /Type /Page /Parent 1 0 R ");
    sprintf(buf, "/MediaBox [0 0 %d %d] ", PDF_PAGE_W, PDF_PAGE_H);
    writeStr(ctx->fp, buf);
    sprintf(buf, "/Contents %d 0 R ", ctx->objCount + 1); // next obj = stream
    writeStr(ctx->fp, buf);
    writeStr(ctx->fp, "/Resources << /Font << /F1 2 0 R >> >> >>\r\n");
    endObj(ctx);

    // Content stream object with a fixed-width /Length field to patch.
    beginObj(ctx);
    writeStr(ctx->fp, "<< /Length ");
    ctx->lenFieldOffset = ftell(ctx->fp);
    writeStr(ctx->fp, "0000000000 >>\r\n");
    writeStr(ctx->fp, "stream\r\n");
    ctx->streamStart = ftell(ctx->fp);
    ctx->streamOpen = 1;
    ctx->lineOnPage = 0;
}

// --- Public API -------------------------------------------------------------

PDF_WR *pdf_wr_open(const char *path)
{
    PDF_WR *ctx;
    ctx = (PDF_WR *)malloc(sizeof(PDF_WR));
    if (!ctx)
        return NULL;
    memset(ctx, 0, sizeof(PDF_WR));

    ctx->fp = fopen(path, "wb");
    if (!ctx->fp)
    {
        free(ctx);
        return NULL;
    }

    // PDF header.
    writeStr(ctx->fp, "%PDF-1.4\r\n");

    // Reserve object 1 for the Pages catalog (body written at close).
    ctx->objCount = 1;

    // Object 2: Font resource (Courier, fixed-pitch).
    beginObj(ctx);
    writeStr(ctx->fp, "<< /Type /Font /Subtype /Type1 /BaseFont /Courier >>\r\n");
    endObj(ctx);

    return ctx;
}

void pdf_wr_line(PDF_WR *ctx, const char *line)
{
    char buf[256];
    int x;
    int y;
    const char *p;
    char c;

    if (!ctx || !ctx->fp)
        return;

    // Start a new page if needed.
    if (!ctx->streamOpen)
    {
        newPage(ctx);
        if (!ctx->streamOpen) // table full -- silently drop
            return;
    }

    // PDF text matrix: Td moves to (x, y) from page origin (bottom-left).
    // We count lines from the top.
    x = PDF_MARGIN_L;
    y = PDF_PAGE_H - PDF_MARGIN_T - (ctx->lineOnPage * (int)PDF_LINE_H);

    sprintf(buf, "BT /F1 %d Tf %d %d Td (", PDF_FONT_SIZE, x, y);
    writeStr(ctx->fp, buf);

    // Write escaped text.
    for (p = line; *p; p++)
    {
        c = *p;
        if (c == '(' || c == ')' || c == '\\')
        {
            fputc('\\', ctx->fp);
            fputc(c, ctx->fp);
        }
        else
        {
            fputc(c, ctx->fp);
        }
    }

    writeStr(ctx->fp, ") Tj ET\r\n");

    ctx->lineOnPage++;

    // Page full?  Close the stream; next line() starts a new page.
    if (ctx->lineOnPage >= PDF_MAX_LINES)
        closeStream(ctx);
}

void pdf_wr_page_break(PDF_WR *ctx)
{
    if (!ctx || !ctx->fp)
        return;
    closeStream(ctx); // next line() starts a fresh page
}

int pdf_wr_close(PDF_WR *ctx)
{
    long xrefOffset;
    int  catalogObj;
    int  ok;
    int  i;
    char buf[64];

    if (!ctx || !ctx->fp)
        return -1;

    // Close the last page's content stream if one is open.
    closeStream(ctx);

    // Object 1: Pages catalog, written now that Kids/Count are known.
    ctx->objOffsets[0] = ftell(ctx->fp);
    writeStr(ctx->fp, "1 0 obj\r\n<< /Type /Pages /Kids [");
    for (i = 0; i < ctx->pageCount; i++)
    {
        sprintf(buf, "%d 0 R ", ctx->pageObjs[i]);
        writeStr(ctx->fp, buf);
    }
    sprintf(buf, "] /Count %d >>\r\n", ctx->pageCount);
    writeStr(ctx->fp, buf);
    endObj(ctx);

    // Document catalog (points to the Pages object).
    beginObj(ctx);
    catalogObj = ctx->objCount;
    writeStr(ctx->fp, "<< /Type /Catalog /Pages 1 0 R >>\r\n");
    endObj(ctx);

    // Cross-reference table.
    xrefOffset = ftell(ctx->fp);
    writeStr(ctx->fp, "xref\r\n");
    sprintf(buf, "0 %d\r\n", ctx->objCount + 1);
    writeStr(ctx->fp, buf);
    writeStr(ctx->fp, "0000000000 65535 f\r\n"); // entry 0: free
    for (i = 0; i < ctx->objCount; i++)
    {
        sprintf(buf, "%010ld 00000 n\r\n", ctx->objOffsets[i]);
        writeStr(ctx->fp, buf);
    }

    // Trailer.
    writeStr(ctx->fp, "trailer\r\n");
    sprintf(buf, "<< /Size %d /Root %d 0 R >>\r\n", ctx->objCount + 1, catalogObj);
    writeStr(ctx->fp, buf);
    writeStr(ctx->fp, "startxref\r\n");
    sprintf(buf, "%ld\r\n", xrefOffset);
    writeStr(ctx->fp, buf);
    writeStr(ctx->fp, "%%EOF\r\n");

    ok = (ferror(ctx->fp) == 0);
    fclose(ctx->fp);
    free(ctx);
    return ok ? 0 : -1;
}

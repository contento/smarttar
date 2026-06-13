//
// [ PDF_WR.C ]
//
// Minimal PDF 1.4 bytestream writer.
// Produces a valid PDF from a sequence of text lines.
// Courier 8pt monospace, US Letter, automatic page breaks.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "pdf_wr.h"

// --- Internal state ---------------------------------------------------------

struct PDF_WR
{
    FILE *fp;
    long  objOffsets[64];   // byte offsets of each indirect object (max 64)
    int   objCount;         // number of objects written so far
    int   pageNum;          // current 1-based page number
    int   lineOnPage;       // 0-based line index within current page
};

// --- Helpers ----------------------------------------------------------------

static void writeStr(FILE *fp, const char *s)
{
    fwrite(s, 1, strlen(s), fp);
}

// Write a single indirect object header: "N 0 obj\r\n"
// Returns the byte offset of this object (for xref).
static long beginObj(PDF_WR *ctx)
{
    long offset;
    char buf[32];
    offset = ftell(ctx->fp);
    sprintf(buf, "%d 0 obj\r\n", ctx->objCount + 1);
    writeStr(ctx->fp, buf);
    ctx->objOffsets[ctx->objCount] = offset;
    ctx->objCount++;
    return offset;
}

static void endObj(PDF_WR *ctx)
{
    writeStr(ctx->fp, "endobj\r\n");
}

// --- New page ---------------------------------------------------------------

static void newPage(PDF_WR *ctx)
{
    char buf[64];

    beginObj(ctx);  // Page object
    writeStr(ctx->fp, "<< /Type /Page /Parent 1 0 R ");
    sprintf(buf, "/MediaBox [0 0 %d %d] ", PDF_PAGE_W, PDF_PAGE_H);
    writeStr(ctx->fp, buf);
    sprintf(buf, "/Contents %d 0 R ", ctx->objCount + 1);  // next obj = content stream
    writeStr(ctx->fp, buf);
    writeStr(ctx->fp, "/Resources << /Font << /F1 2 0 R >> >> >>\r\n");
    endObj(ctx);

    // Begin content stream object (we'll patch the length later)
    beginObj(ctx);
    writeStr(ctx->fp, "<< /Length 0 >>\r\n");  // placeholder
    writeStr(ctx->fp, "stream\r\n");
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

    // PDF header
    writeStr(ctx->fp, "%PDF-1.4\r\n");

    // Object 1: Pages catalog
    beginObj(ctx);
    writeStr(ctx->fp, "<< /Type /Pages /Kids [] /Count 0 >>\r\n");
    endObj(ctx);

    // Object 2: Font resource (Courier, fixed-pitch)
    beginObj(ctx);
    writeStr(ctx->fp, "<< /Type /Font /Subtype /Type1 /BaseFont /Courier >>\r\n");
    endObj(ctx);

    ctx->pageNum = 0;
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

    // Start a new page if needed
    if (ctx->lineOnPage == 0)
    {
        ctx->pageNum++;
        newPage(ctx);
    }

    // Emit text positioning and content.
    // PDF text matrix: Td moves to (x, y) from page origin (bottom-left).
    // We count from top, so y = PDF_PAGE_H - PDF_MARGIN_T - (lineOnPage * LINE_H).
    x = PDF_MARGIN_L;
    y = PDF_PAGE_H - PDF_MARGIN_T - (ctx->lineOnPage * (int)PDF_LINE_H);

    // Escape special PDF characters in the text
    sprintf(buf, "BT /F1 %d Tf %d %d Td (", PDF_FONT_SIZE, x, y);
    writeStr(ctx->fp, buf);

    // Write escaped text
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

    // Page full? Close the stream and start a new page on next line.
    if (ctx->lineOnPage >= PDF_MAX_LINES)
    {
        writeStr(ctx->fp, "endstream\r\n");
        endObj(ctx);
        ctx->lineOnPage = 0;  // triggers newPage on next line()
    }
}

void pdf_wr_page_break(PDF_WR *ctx)
{
    if (!ctx || !ctx->fp)
        return;
    // Close current page's stream if one is open
    if (ctx->lineOnPage > 0 || ctx->pageNum == 0)
    {
        if (ctx->lineOnPage > 0)
        {
            writeStr(ctx->fp, "endstream\r\n");
            endObj(ctx);
        }
        ctx->lineOnPage = 0;
    }
}

int pdf_wr_close(PDF_WR *ctx)
{
    long savedPos;
    long xrefOffset;
    int ok;
    int i;
    char buf[64];

    if (!ctx || !ctx->fp)
        return -1;

    ok = 0;

    // Close the last page's content stream if one is open
    if (ctx->lineOnPage > 0)
    {
        writeStr(ctx->fp, "endstream\r\n");
        endObj(ctx);
    }

    // Attempt to patch the Pages catalog (obj 1) with actual Kids/Count.
    // Seek back to obj 1 and overwrite the /Kids [] /Count 0 with real values.
    savedPos = ftell(ctx->fp);
    fseek(ctx->fp, ctx->objOffsets[0] + strlen("<< /Type /Pages "), SEEK_SET);
    writeStr(ctx->fp, "/Kids [");
    for (i = 0; i < ctx->pageNum; i++)
    {
        // Page object for page i is at obj index 2 + i*2 (0-based)
        // = object number 3 + i*2 (1-based PDF)
        sprintf(buf, "%d 0 R ", 3 + i * 2);
        writeStr(ctx->fp, buf);
    }
    sprintf(buf, "] /Count %d ", ctx->pageNum);
    writeStr(ctx->fp, buf);
    fseek(ctx->fp, savedPos, SEEK_SET);

    // Object: Document catalog (points to our Pages object)
    beginObj(ctx);
    writeStr(ctx->fp, "<< /Type /Catalog /Pages 1 0 R >>\r\n");
    endObj(ctx);

    // Cross-reference table
    xrefOffset = ftell(ctx->fp);
    writeStr(ctx->fp, "xref\r\n");
    sprintf(buf, "0 %d\r\n", ctx->objCount + 1);
    writeStr(ctx->fp, buf);
    // Entry 0: free object
    writeStr(ctx->fp, "0000000000 65535 f \r\n");
    for (i = 0; i < ctx->objCount; i++)
    {
        sprintf(buf, "%010ld 00000 n \r\n", ctx->objOffsets[i]);
        writeStr(ctx->fp, buf);
    }

    // Trailer
    writeStr(ctx->fp, "trailer\r\n");
    sprintf(buf, "<< /Size %d /Root %d 0 R >>\r\n",
            ctx->objCount + 1, ctx->objCount);  // root = last obj (catalog)
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

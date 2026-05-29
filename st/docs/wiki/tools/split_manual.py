#!/usr/bin/env python3
"""Split a pandoc-converted manual (single .md) into one page per H1 chapter,
with frontmatter, plus an index.md MOC that wikilinks the chapters in order.

Usage:
    python3 split_manual.py <manual.md> <out_dir> <lang> <manual_id> <index_title>

Reusable for all four manuals. Pandoc page-break rules (^---+$) are dropped.
Chapter files are numbered (NN-slug.md) so file order == reading order == the
order build-docs.sh reassembles them for docx export.
"""
import os
import re
import sys
import unicodedata


def slugify(text):
    text = unicodedata.normalize("NFKD", text)
    text = "".join(c for c in text if not unicodedata.combining(c))
    text = re.sub(r"[^a-zA-Z0-9]+", "-", text).strip("-").lower()
    return text or "seccion"


def unescape(text):
    # pandoc gfm escapes markdown punctuation with backslashes; undo for titles
    return re.sub(r"\\([\[\]_*#`~()<>.|-])", r"\1", text)


def yaml_quote(s):
    return '"' + s.replace("\\", "\\\\").replace('"', '\\"') + '"'


def main():
    if len(sys.argv) != 6:
        sys.exit("usage: split_manual.py <manual.md> <out_dir> <lang> "
                 "<manual_id> <index_title>")
    src, out_dir, lang, manual_id, index_title = sys.argv[1:6]
    os.makedirs(out_dir, exist_ok=True)

    with open(src, encoding="utf-8") as fh:
        lines = fh.read().split("\n")

    # Split into chapters on top-level (#) headings.
    chapters = []  # (title, [body lines])
    cur = None
    for line in lines:
        m = re.match(r"^# (?!#)(.*)$", line)
        if m:
            if cur:
                chapters.append(cur)
            cur = (unescape(m.group(1)).strip(), [])
        elif cur:
            if re.match(r"^-{3,}\s*$", line):
                continue  # drop pandoc page-break rules
            cur[1].append(line)
    if cur:
        chapters.append(cur)

    index_rows = []
    for i, (title, body) in enumerate(chapters, 1):
        slug = "%02d-%s" % (i, slugify(title))
        # trim leading/trailing blank lines of the body
        while body and body[0].strip() == "":
            body.pop(0)
        while body and body[-1].strip() == "":
            body.pop()
        fm = [
            "---",
            "title: " + yaml_quote(title),
            "lang: " + lang,
            "manual: " + manual_id,
            "order: %d" % i,
            "---",
            "",
            "# " + title,
            "",
        ]
        with open(os.path.join(out_dir, slug + ".md"), "w",
                  encoding="utf-8") as fh:
            fh.write("\n".join(fm) + "\n".join(body) + "\n")
        index_rows.append("%d. [[%s|%s]]" % (i, slug, title))

    idx = [
        "---",
        "title: " + yaml_quote(index_title),
        "lang: " + lang,
        "manual: " + manual_id,
        "order: 0",
        "---",
        "",
        "# " + index_title,
        "",
        "## Contenido",
        "",
    ] + index_rows + [""]
    with open(os.path.join(out_dir, "index.md"), "w", encoding="utf-8") as fh:
        fh.write("\n".join(idx))
    print("%s: %d chapters -> %s" % (manual_id, len(chapters), out_dir))


if __name__ == "__main__":
    main()

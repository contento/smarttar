#!/usr/bin/env python3
"""Generator: per-topic Markdown pages (UTF-8) -> help.txt (Latin-1).

Usage:
    python3 md2help.py <ayuda_dir> <out_help.txt> [--crlf]

Reads every <ayuda_dir>/*.md (frontmatter: topic/title/order), orders by the
`order` field, and emits a Zinc help.txt that genhelp compiles into help.dat.
Output is Latin-1 -- the encoding Zinc expects (see CLAUDE.md). Default line
endings are LF (matching the tracked help.txt); pass --crlf for DOS endings.
"""
import glob
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from helpfmt import HELP_ENCODING, render_record


def parse_frontmatter(text):
    """Return (meta dict, body str). Expects a leading '---' YAML block."""
    lines = text.split("\n")
    if not lines or lines[0].strip() != "---":
        raise ValueError("missing frontmatter")
    meta = {}
    i = 1
    while i < len(lines) and lines[i].strip() != "---":
        line = lines[i]
        if ":" in line:
            key, val = line.split(":", 1)
            val = val.strip()
            if len(val) >= 2 and val[0] == '"' and val[-1] == '"':
                val = val[1:-1].replace('\\"', '"').replace("\\\\", "\\")
            meta[key.strip()] = val
        i += 1
    body = "\n".join(lines[i + 1:])
    return meta, body


def body_to_logical(body):
    """Markdown body -> logical lines: one per non-blank line, '' per blank,
    runs of blanks collapsed, leading/trailing blanks trimmed."""
    out = []
    prev_blank = True  # trims leading blanks
    for raw in body.split("\n"):
        line = raw.rstrip()
        if line == "":
            if not prev_blank:
                out.append("")
            prev_blank = True
        else:
            out.append(line)
            prev_blank = False
    while out and out[-1] == "":
        out.pop()
    return out


def main():
    args = [a for a in sys.argv[1:] if not a.startswith("--")]
    crlf = "--crlf" in sys.argv[1:]
    if len(args) != 2:
        sys.exit("usage: md2help.py <ayuda_dir> <out_help.txt> [--crlf]")
    ayuda_dir, out_path = args

    records = []
    for path in glob.glob(os.path.join(ayuda_dir, "*.md")):
        if os.path.basename(path) == "index.md":
            continue  # MOC page, not a help topic
        with open(path, encoding="utf-8") as fh:
            meta, body = parse_frontmatter(fh.read())
        if "topic" not in meta:
            sys.exit("missing 'topic' in %s" % path)
        records.append((int(meta.get("order", 0)), meta["topic"],
                        meta.get("title", ""), body_to_logical(body)))
    records.sort(key=lambda r: (r[0], r[1]))

    chunks = [render_record(topic, title, body)
              for _, topic, title, body in records]
    text = "\n".join(chunks) + "\n"
    if crlf:
        text = text.replace("\n", "\r\n")
    with open(out_path, "wb") as fh:
        fh.write(text.encode(HELP_ENCODING))
    print("wrote %d topics to %s (%s)"
          % (len(records), out_path, "CRLF" if crlf else "LF"))


if __name__ == "__main__":
    main()

#!/usr/bin/env python3
"""One-time extractor: help.txt (Latin-1) -> per-topic Markdown pages (UTF-8).

Usage:
    python3 help2md.py <help.txt> <out_dir>

Writes <out_dir>/<TOPIC>.md for each help topic, with YAML frontmatter
(topic/title/lang/order) and the body as one Markdown line per logical line
(blank lines = paragraph breaks). Run from the vault tools/ dir.
"""
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from helpfmt import parse_help


def yaml_quote(s):
    return '"' + s.replace("\\", "\\\\").replace('"', '\\"') + '"'


def main():
    if len(sys.argv) != 3:
        sys.exit("usage: help2md.py <help.txt> <out_dir>")
    src, out_dir = sys.argv[1], sys.argv[2]
    os.makedirs(out_dir, exist_ok=True)

    records = parse_help(src)
    for order, rec in enumerate(records):
        fm = [
            "---",
            "topic: " + rec["topic"],
            "title: " + yaml_quote(rec["title"]),
            "lang: es",
            "order: %d" % order,
            "---",
            "",
        ]
        body = "\n".join(rec["body"])
        text = "\n".join(fm) + body + "\n"
        path = os.path.join(out_dir, rec["topic"] + ".md")
        with open(path, "w", encoding="utf-8") as fh:
            fh.write(text)
    print("wrote %d topic pages to %s" % (len(records), out_dir))


if __name__ == "__main__":
    main()

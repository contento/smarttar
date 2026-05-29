"""Shared parsing for the SmartTar Zinc help format (help.txt -> help.dat).

The on-disk help.txt is ISO-8859-1 (Latin-1) -- it is consumed by Zinc, which
draws via its own bitmap font and expects Latin-1 (NOT CP850; see CLAUDE.md).
The Obsidian vault stores everything as UTF-8; transcoding happens only here and
in md2help.py, so the fragile high-bit encoding never lives in the vault.

Help format (one record per topic):

    --- H_TOPIC_NAME
    <title line>
    <body...>            body physical lines soft-wrap freely; a logical line
    ...                  ends at a trailing backslash. A bare "\\" line is a
    blank line marker.\\  paragraph break. The last logical line has no
    final line                backslash.

A "logical line" is the unit Zinc reflows: continuation physical lines are
joined and internal whitespace runs collapse to a single space.
"""
import re

HELP_ENCODING = "iso-8859-1"
TOPIC_RE = re.compile(r"^--- (\S+)\s*$")


def _build_logical(body_lines):
    """Collapse soft-wrapped physical lines into logical lines.

    A physical line ending in '\\' terminates a logical line; otherwise it is a
    soft-wrap continuation. A bare '\\' yields an empty logical line (paragraph
    break). A trailing logical line with no backslash is kept as-is.
    """
    out, cur = [], []
    for pl in body_lines:
        if pl.endswith("\\"):
            cur.append(pl[:-1])
            text = re.sub(r"\s+", " ", " ".join(cur)).strip()
            out.append(text)
            cur = []
        else:
            cur.append(pl)
    if cur:
        text = re.sub(r"\s+", " ", " ".join(cur)).strip()
        if text:
            out.append(text)
    # Trim leading/trailing blank logical lines (cosmetic, not content).
    while out and out[0] == "":
        out.pop(0)
    while out and out[-1] == "":
        out.pop()
    return out


def parse_help(path):
    """Parse a help.txt file into an ordered list of records.

    Each record is a dict: {topic, title, body} where body is a list of logical
    lines ("" entries are paragraph breaks).
    """
    with open(path, "rb") as fh:
        text = fh.read().decode(HELP_ENCODING)
    lines = text.replace("\r\n", "\n").replace("\r", "\n").split("\n")

    records = []
    cur = None
    pending_title = False
    for line in lines:
        m = TOPIC_RE.match(line)
        if m:
            if cur is not None:
                cur["body"] = _build_logical(cur["_raw"])
                del cur["_raw"]
                records.append(cur)
            cur = {"topic": m.group(1), "title": "", "_raw": []}
            pending_title = True
            continue
        if cur is None:
            continue  # preamble before first topic (none expected)
        if pending_title:
            cur["title"] = line.strip()
            pending_title = False
        else:
            cur["_raw"].append(line)
    if cur is not None:
        cur["body"] = _build_logical(cur["_raw"])
        del cur["_raw"]
        records.append(cur)
    return records


def render_record(topic, title, body):
    """Render one topic back into help.txt text (no trailing newline).

    body is a list of logical lines ("" = paragraph break). Every logical line
    gets a trailing backslash except the final one, matching the original.
    """
    lines = ["--- " + topic, title]
    n = len(body)
    for i, log in enumerate(body):
        last = i == n - 1
        lines.append(log if last else log + "\\")
    return "\n".join(lines)

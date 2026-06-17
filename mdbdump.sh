#!/bin/bash
# mdbdump.sh — dump MiniDB .db contents from the st/ directory.
# Usage: ./mdbdump.sh [options]
#
# Defaults to st/bin/RX.db. Passes all options through to the Python tool.
DIR="$(cd "$(dirname "$0")" && pwd)"
DB="$DIR/st/bin/RX.db"
TOOL="$DIR/st/util/lsmdb/mdbdump.py"
[ -f "$TOOL" ] || TOOL="$DIR/st/bin/mdbdump.py"

if [ ! -f "$TOOL" ]; then
    echo "ERROR: mdbdump.py not found (looked in util/lsmdb/ and bin/)"
    exit 1
fi

if [ ! -f "$DB" ]; then
    echo "ERROR: database not found: $DB"
    echo "Run the app first to generate a database."
    exit 1
fi

exec python3 "$TOOL" "$@" "$DB"

#!/usr/bin/env bash
# Regenerate the SmartTar documentation OUTPUTS from the Obsidian vault.
#
# The vault (es/, en/, *.md) is the single source of truth for user-facing
# documentation. This script produces, into _build/:
#   * the four .docx manuals + the STC doc   (pandoc, Markdown -> docx)
#
# NOTE: st/res/help.txt is a source asset for the app (consumed by genhelp),
# NOT a wiki output. It lives in st/res/ and is edited directly.
#
# Outputs land in _build/ rather than overwriting tracked files so you can
# diff before promoting.
#
# Requires: pandoc. Run from anywhere.
set -euo pipefail
shopt -s nullglob

WIKI_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
MANIFEST="$WIKI_DIR/manifest.json"
OUT_DIR="$WIKI_DIR/$(python3 -c 'import json,sys;print(json.load(open(sys.argv[1]))["outputs_dir"])' "$MANIFEST")"
mkdir -p "$OUT_DIR"

command -v pandoc >/dev/null || { echo "ERROR: pandoc not found" >&2; exit 1; }

# Print a markdown file with its leading YAML frontmatter block removed.
strip_fm() {
  awk 'BEGIN{fm=0} NR==1&&$0=="---"{fm=1;next} fm==1&&$0=="---"{fm=0;next} fm==0{print}' "$1"
}

echo "==> Building manuals (Markdown -> docx)"
while IFS=$'\t' read -r id src out title subtitle vendor copyr; do
  SRC="$WIKI_DIR/$src"
  TMP="$(mktemp -t stdoc)"
  {
    printf '# %s\n\n' "$title"
    printf '**%s**\\\n' "$subtitle"
    printf '%s\\\n' "$vendor"
    printf '%s\n\n' "$copyr"
    printf -- '---\n\n'
    chapters=("$SRC"/[0-9]*.md)
    if [ ${#chapters[@]} -gt 0 ]; then
      for f in "${chapters[@]}"; do strip_fm "$f"; printf '\n\n'; done
    else
      strip_fm "$SRC/index.md"      # single-page docs (e.g. STC)
    fi
  } > "$TMP"
  pandoc "$TMP" -f gfm -o "$OUT_DIR/$out"
  rm -f "$TMP"
  echo "    $out"
done < <(python3 - "$MANIFEST" <<'PY'
import json, sys
for x in json.load(open(sys.argv[1]))["manuals"]:
    print("\t".join([x["id"], x["src_dir"], x["output"], x["title"],
                     x["subtitle"], x["vendor"], x["copyright"]]))
PY
)

echo "==> Done. Outputs in $OUT_DIR"

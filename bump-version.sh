#!/usr/bin/env bash
# Bump or set the SmartTar version.
#
# Usage:
#   ./bump-version.sh patch                   # 2.34.1 -> 2.34.2
#   ./bump-version.sh minor                   # 2.34.1 -> 2.35.0
#   ./bump-version.sh major                   # 2.34.1 -> 3.0.0
#   ./bump-version.sh set <X.Y.Z>             # explicit
#   ./bump-version.sh --dry-run <action> ...  # preview, no writes
#
# Updates:
#   - CLAUDE.md      "Current version: **X.Y.Z**"
#   - st/versions.txt prepends a [ X.Y.Z ] block with a TODO description stub
#
# Does NOT commit, tag, or push — see RELEASING.md for the full workflow.

set -euo pipefail
cd "$(dirname "$0")"

usage() {
  sed -n '2,16p' "$0" | sed 's/^# \{0,1\}//' >&2
  exit 2
}

dry_run=0
if [[ "${1:-}" == "--dry-run" ]]; then
  dry_run=1
  shift
fi

[[ $# -ge 1 ]] || usage

action="$1"

# Current version from CLAUDE.md
current=$(grep -oE 'Current version: \*\*[0-9]+\.[0-9]+\.[0-9]+\*\*' CLAUDE.md \
          | grep -oE '[0-9]+\.[0-9]+\.[0-9]+' | head -1)
[[ -n "$current" ]] || { echo "Cannot find 'Current version: **X.Y.Z**' in CLAUDE.md" >&2; exit 1; }

IFS=. read -r major minor patch <<< "$current"

case "$action" in
  patch) patch=$((patch + 1)) ;;
  minor) minor=$((minor + 1)); patch=0 ;;
  major) major=$((major + 1)); minor=0; patch=0 ;;
  set)
    [[ $# -ge 2 ]] || { echo "set requires X.Y.Z" >&2; usage; }
    if [[ ! "$2" =~ ^[0-9]+\.[0-9]+\.[0-9]+$ ]]; then
      echo "Invalid version '$2' (must be X.Y.Z)" >&2; exit 1
    fi
    IFS=. read -r major minor patch <<< "$2"
    ;;
  *) usage ;;
esac

new="$major.$minor.$patch"
echo "Bumping version: $current -> $new"

if [[ "$current" == "$new" ]]; then
  echo "No change (target equals current)." >&2
  exit 0
fi

if (( dry_run )); then
  echo "(dry-run; no files written)"
  exit 0
fi

# Update CLAUDE.md (portable in-place sed: -i.bak then remove backup)
sed -i.bak \
  "s/Current version: \*\*${current//./\\.}\*\*/Current version: **${new}**/" \
  CLAUDE.md
rm -f CLAUDE.md.bak

# Prepend a new entry to st/versions.txt
today=$(date '+%b %d %Y')
tmp=$(mktemp)
{
  printf '[ %s ]\n - %s: TODO add description.\n \n' "$new" "$today"
  cat st/versions.txt
} > "$tmp"
mv "$tmp" st/versions.txt

cat <<EOF
Updated:
  CLAUDE.md       (Current version → $new)
  st/versions.txt (new [ $new ] block at top — please edit the description)

Next steps (per RELEASING.md):
  1. Edit st/versions.txt — replace 'TODO add description.' with the real changelog
  2. git add CLAUDE.md st/versions.txt
  3. git commit -m "Release v$new: <summary>"
  4. git tag -a v$new -m "..."
  5. git push origin main v$new
EOF

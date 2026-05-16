#!/usr/bin/env bash
# Run a SmartTar build inside DOSBox-X non-interactively.
#
# Usage:   ./make-headless.sh [--keep-log-in-st] [demo|debug|eda|auto|prod]
# Default: demo, log at repo root (build.log)
#
# Only one instance can run at a time (DOSBox-X locks its config / display).
#
# Flags:
#   --keep-log-in-st   Write the log inside st/ (st/build.log) instead of the
#                      repo root. Useful when you want the log next to the
#                      object files / binaries it describes.
#
# Always passes HELP=1 so the MAKEFILE's gated help.dat rule fires when
# bin/help.dat is missing or stale. Make's dependency tracking means
# this is a no-op when help.dat is up to date.
#
# Exit: 0 if the build batch printed "Build succeeded.", 1 otherwise.
#
# Note: DOSBox-X still opens a window during the run; there is no fully
# headless mode on macOS. The window closes when the build finishes
# (the final -c "exit" terminates DOSBox-X).

set -euo pipefail
cd "$(dirname "$0")"

keep_in_st=0
variant=""

for arg in "$@"; do
  case "$arg" in
    --keep-log-in-st) keep_in_st=1 ;;
    demo|debug|eda|auto|prod)
      if [[ -n "$variant" ]]; then
        echo "usage: $(basename "$0") [--keep-log-in-st] [demo|debug|eda|auto|prod]" >&2
        exit 2
      fi
      variant="$arg"
      ;;
    -h|--help)
      sed -n '2,22p' "$0" | sed 's/^# \{0,1\}//'
      exit 0
      ;;
    *)
      echo "usage: $(basename "$0") [--keep-log-in-st] [demo|debug|eda|auto|prod]" >&2
      exit 2
      ;;
  esac
done
variant="${variant:-demo}"

: "${DOSBOX_X:=dosbox-x}"
if ! command -v "$DOSBOX_X" >/dev/null 2>&1; then
  echo "dosbox-x not found on PATH (set DOSBOX_X to override)" >&2
  exit 127
fi

if (( keep_in_st )); then
  log="st/build.log"
  dos_log='C:\ST\build.log'
else
  log="build.log"
  dos_log='C:\build.log'
fi
rm -f "$log"

"$DOSBOX_X" -conf dosbox-x.conf -fastlaunch \
  -c "command /c make${variant}.bat HELP=1 > ${dos_log}" \
  -c "exit" >/dev/null 2>&1 || true

if [[ ! -f "$log" ]]; then
  echo "build $variant: NO LOG — DOSBox-X did not write to the mount" >&2
  exit 1
fi

if grep -q "Build succeeded\." "$log"; then
  echo "build $variant: OK (see $log)"
  exit 0
fi

echo "build $variant: FAIL (see $log)"
echo "--- last 30 lines of $log ---"
tail -n 30 "$log"
exit 1

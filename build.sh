#!/usr/bin/env bash
# Run a SmartTar build inside DOSBox-X non-interactively.
#
# Usage:   ./build.sh [--force] [--keep-log-in-st]
#                              [demo|dbg|eda|prod]
# Default: demo, log at repo root (build.log)
#
# Only one instance can run at a time (DOSBox-X locks its config / display).
#
# Flags:
#   --force            Pass -B to Borland MAKE (force rebuild of all targets,
#                      ignoring timestamps). Alias: --rebuild.
#   --keep-log-in-st   Write the log inside st/ (st/build.log) instead of the
#                      repo root. Useful when you want the log next to the
#                      object files / binaries it describes.
#
# help.dat is built unconditionally by the MAKEFILE (no HELP=1 needed).
#
# Exit: 0 if the build batch printed "Build succeeded.", 1 otherwise.
#
# Note: DOSBox-X still opens a window during the run; there is no fully
# headless mode on macOS. The window closes when the build finishes
# (the final -c "exit" terminates DOSBox-X).

set -euo pipefail
cd "$(dirname "$0")"

keep_in_st=0
force=0
variant=""
usage="usage: $(basename "$0") [--force] [--keep-log-in-st] [demo|dbg|eda|prod]"

for arg in "$@"; do
  case "$arg" in
    --keep-log-in-st) keep_in_st=1 ;;
    --force|--rebuild) force=1 ;;
    demo|dbg|eda|prod)
      if [[ -n "$variant" ]]; then
        echo "$usage" >&2
        exit 2
      fi
      variant="$arg"
      ;;
    -h|--help)
      sed -n '2,24p' "$0" | sed 's/^# \{0,1\}//'
      exit 0
      ;;
    *)
      echo "$usage" >&2
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

# --- Check for vendor/ (proprietary toolchain) --------------------------------
if [[ ! -d vendor ]]; then
  echo "" >&2
  echo "build: vendor/ directory not found." >&2
  echo "" >&2
  echo "The vendor/ directory contains proprietary toolchain binaries required" >&2
  echo "to build SmartTar (Borland C++ 3.1, Pharlap 286, Zinc 3.5)." >&2
  echo "" >&2
  echo "To set it up, run:" >&2
  echo "  ./setup-vendor.sh          # clones from private smarttar-vendor repo" >&2
  echo "" >&2
  echo "If you don't have SSH access to the private repo, you can obtain" >&2
  echo "the components manually. See VENDOR_SETUP.md for details:" >&2
  echo "  - Borland C++ 3.1  (vendor/bc/)" >&2
  echo "  - Pharlap 286      (vendor/pharlap/)" >&2
  echo "  - Zinc 3.5         (vendor/zinc/)" >&2
  echo "" >&2
  exit 1
fi

if (( keep_in_st )); then
  log="st/build.log"
  dos_log='C:\ST\build.log'
else
  log="build.log"
  dos_log='C:\build.log'
fi
rm -f "$log"

make_args=""
banner_suffix=""
if (( force )); then
  make_args="-B"
  banner_suffix=" (force rebuild)"
fi

echo "build: building variant '$variant'$banner_suffix (log: $log) ..."
echo "build: streaming compile output below; window stays silent."
echo "----------------------------------------------------------------------"

# Touch the log so tail has something to follow even before DOSBox-X starts
# writing to it. tail -F (capital F) survives the file being truncated by
# the DOS-side redirect.
touch "$log"
TAIL_PID=
tail -F "$log" 2>/dev/null &
TAIL_PID=$!
mkdir -p st/build st/bin st/lib

# Capture DOSBox-X's own stderr (crash traces, protection faults) when
# MAKE_HEADLESS_DEBUG is set. CI sets this for failure diagnostics.
dx_redir=">/dev/null 2>&1"
dx_log="$PWD/dosbox-x-build.log"
if [ -n "${MAKE_HEADLESS_DEBUG:-}" ]; then
  dx_redir=">>'$dx_log' 2>&1"
  echo "build ${variant}: DOSBox-X debug output -> $dx_log"
fi

eval "\"$DOSBOX_X\" -conf dosbox-x.conf -fastlaunch \
  -c \"echo === SmartTar build starting (variant ${variant}) ===\" \
  -c \"echo === log: ${dos_log} (silent until exit) ===\" \
  -c \"command /c make${variant}.bat $make_args >> ${dos_log}\" \
  -c \"echo .\" \
  -c \"echo === Build finished ===\" \
  -c \"exit\" ${dx_redir}" || true

# Stop streaming. Ignore errors — the tail process may already have exited
# or been killed when DOSBox-X closed its last command.
kill ${TAIL_PID:--1} 2>/dev/null || true
wait ${TAIL_PID:--1} 2>/dev/null || true
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

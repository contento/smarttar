#!/usr/bin/env bash
# Run a SmartTar build inside DOSBox-X non-interactively.
#
# Usage:   ./build.sh [--force] [--debug] [--keep-log-in-st]
#                              [demo_dos|real_dos]
# Default: demo_dos, log at repo root (build.log)
#
# Variants (mini-smarttar): demo_dos is the buildable variant; real_dos is
# DEACTIVATED and fails by design (real_dos/ trips a #error unless
# REAL_DOS_ENABLED). See MINI_SMARTTAR_PLAN.
#
# Only one instance can run at a time (DOSBox-X locks its config / display).
#
# Flags:
#   --force            Pass -B to Borland MAKE (force rebuild of all targets,
#                      ignoring timestamps). Alias: --rebuild.
#   --debug            Add -DDEBUG (Borland -v symbols). Modifier on the
#                      variant; there is no separate debug variant.
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
debug=0
variant=""
usage="usage: $(basename "$0") [--force] [--debug] [--keep-log-in-st] [demo_dos|real_dos]"

for arg in "$@"; do
  case "$arg" in
    --keep-log-in-st) keep_in_st=1 ;;
    --force|--rebuild) force=1 ;;
    --debug) debug=1 ;;
    demo_dos|real_dos)
      if [[ -n "$variant" ]]; then
        echo "$usage" >&2
        exit 2
      fi
      variant="$arg"
      ;;
    -h|--help)
      sed -n '2,30p' "$0" | sed 's/^# \{0,1\}//'
      exit 0
      ;;
    *)
      echo "$usage" >&2
      exit 2
      ;;
  esac
done
variant="${variant:-demo_dos}"
# Map variant to 8.3-safe batch filename (LFN disabled in DOSBox-X).
batfile=""
case "$variant" in
  demo_dos) batfile="mkdemos" ;;
  real_dos) batfile="mkrldos" ;;
esac


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

make_args=""
banner_suffix=""
if (( force )); then
  make_args="-B"
  banner_suffix=" (force rebuild)"
fi
if (( debug )); then
  make_args="${make_args:+$make_args }-DDEBUG"
  banner_suffix="$banner_suffix (debug)"
fi

echo "build: building variant '$variant'$banner_suffix (log: $log) ..."
echo "build: streaming compile output below."
echo "----------------------------------------------------------------------"

# Touch the log so tail has something to follow even before DOSBox-X starts
# writing to it. tail -F (capital F) survives the file being truncated by
# the DOS-side redirect.
touch "$log"
tail -F "$log" 2>/dev/null &
TAIL_PID=$!
trap 'kill $TAIL_PID 2>/dev/null; wait $TAIL_PID 2>/dev/null' EXIT

# Build output dirs are gitignored and absent on a fresh checkout. C: is the
# repo mount, so creating them host-side makes them visible inside DOSBox-X.
mkdir -p st/build st/bin st/lib

# Run the .bat directly (NOT through command /c — that creates a new shell
# environment that breaks build paths). The bat writes build status to
# C:\build.log internally. Make output is visible in the DOSBox-X window.
cmd=("$DOSBOX_X" -conf dosbox-x.conf -fastlaunch -exit)
cmd+=(-c "${batfile}.bat $make_args")
"${cmd[@]}" >/dev/null 2>&1 || true
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

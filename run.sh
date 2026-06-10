#!/usr/bin/env bash
# Launch SmartTar inside DOSBox-X. Closes DOSBox-X automatically when
# the user exits the app (the trailing -c "exit" runs as soon as st.exe
# returns control to COMMAND.COM).
#
# Usage:   ./run.sh [--keep-open] [--log [file]] [-- <args>]
#
# Flags:
#   --keep-open   After st.exe exits, drop into the DOS prompt instead
#                 of closing DOSBox-X. Useful when poking at log files
#                 or RX.* state without relaunching the extender.
#   --log [file]  Capture DOSBox-X debug output (LOG: messages, E_Exit, etc.)
#                 to a file. Defaults to run.log in the project root when
#                 --log is given without a path. Pass --log "" to disable.
#
# Anything after `--` is forwarded as command-line args to st.exe (same
# positional pass-through as st/run.bat).
#
# Notes:
#   - The DOSBox-X window is the SmartTar UI; DOSBox-X debug LOG: lines
#     are captured to run.log by default and also streamed to stderr.
#   - Only one DOSBox-X instance at a time (it locks its config / display).
#   - Override the dosbox-x binary path with the DOSBOX_X env var.

set -euo pipefail
cd "$(dirname "$0")"

keep_open=0
log_file="run.log"
st_args=""
usage="usage: $(basename "$0") [--keep-open] [--log [file]] [-- <st.exe args>]"

while (($#)); do
  case "$1" in
    --keep-open) keep_open=1; shift ;;
    --log)
      log_file="run.log"
      if [[ $# -gt 1 && "${2:-}" != "--" && "${2:-}" != -* ]]; then
        log_file="$2"; shift
      fi
      shift ;;
    -h|--help)
      sed -n '2,25p' "$0" | sed 's/^# \{0,1\}//'
      exit 0
      ;;
    --) shift; st_args="$*"; break ;;
    *)
      echo "$usage" >&2
      exit 2
      ;;
  esac
done

: "${DOSBOX_X:=dosbox-x}"
if ! command -v "$DOSBOX_X" >/dev/null 2>&1; then
  echo "dosbox-x not found on PATH (set DOSBOX_X to override)" >&2
  exit 127
fi

if [[ ! -f st/bin/st.exe ]]; then
  echo "st/bin/st.exe not found. Build first: ./build.sh [variant]" >&2
  exit 1
fi

# DOSBox-X autoexec (dosbox-x.conf) handles mount, cd to st, PATH setup.
# Queue: cd into bin -> run st with forwarded args -> then exit (unless
# --keep-open, which leaves the DOS prompt up).
cmd=("$DOSBOX_X" -conf dosbox-x.conf -fastlaunch)
cmd+=(-c "cd bin")
cmd+=(-c "st $st_args")
if (( keep_open )); then
  cmd+=(-c "echo.")
  cmd+=(-c "echo st.exe exited. Type 'exit' to close DOSBox-X.")
else
  cmd+=(-c "exit")
fi

if [[ -n "$log_file" ]]; then
  echo "Logging to: $log_file" >&2
  "${cmd[@]}" 2>&1 | tee "$log_file"
else
  exec "${cmd[@]}"
fi

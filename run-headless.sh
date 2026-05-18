#!/usr/bin/env bash
# Launch SmartTar inside DOSBox-X. Closes DOSBox-X automatically when
# the user exits the app (the trailing -c "exit" runs as soon as st.exe
# returns control to COMMAND.COM).
#
# Usage:   ./run-headless.sh [--keep-open] [--log [file]] [-- <args>]
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
  echo "st/bin/st.exe not found. Build first: ./make-headless.sh [variant]" >&2
  exit 1
fi

# DOSBox-X autoexec (dosbox-x.conf) already mounts C: and cd's into ST.
# Queue: cd into bin -> run st with forwarded args -> exit (skipped if
# --keep-open is set, leaving the DOS prompt in the foreground).
dos_cmds=(
  -c "cd bin"
  -c "st $st_args"
)
if (( ! keep_open )); then
  dos_cmds+=( -c "exit" )
fi

if [[ -n "$log_file" ]]; then
  echo "Logging to: $log_file" >&2
  "$DOSBOX_X" -conf dosbox-x.conf -fastlaunch -log "$log_file" "${dos_cmds[@]}"
else
  exec "$DOSBOX_X" -conf dosbox-x.conf -fastlaunch "${dos_cmds[@]}"
fi

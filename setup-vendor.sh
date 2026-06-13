#!/usr/bin/env bash
# Clone the smarttar-vendor private repository into ./vendor/
#
# The vendor/ directory contains proprietary toolchain binaries required to
# build SmartTar:
#
#   vendor/bc/      Borland C++ 3.1 (compiler, debugger, TASM, TLIB, etc.)
#   vendor/pharlap/ Pharlap 286 DOS extender (BCC286, BIND286, CFIG286, ...)
#   vendor/zinc/    Zinc 3.5 UI framework (headers, libs, GENHELP.EXE, ...)
#   vendor/util/    DOS utilities (Norton Commander, QEdit, PKWARE, etc.)
#
# These are NOT redistributable and live in a separate private repository
# to avoid copyright issues in the main smarttar repo.
#
# Usage:
#   ./setup-vendor.sh            # clone into ./vendor/
#   ./setup-vendor.sh --force    # remove existing vendor/ and re-clone
#
# Prerequisites:
#   - git with SSH access to github.com (or HTTPS + credentials)
#   - The smarttar-vendor repository must be accessible to your account
#
# If the clone fails, see VENDOR_SETUP.md for manual instructions on
# obtaining the required toolchain components (Zinc 3.5, Borland C++ 3.1,
# Pharlap 286).

set -euo pipefail
cd "$(dirname "$0")"

VENDOR_REPO="git@github.com:contento/smarttar-vendor.git"
VENDOR_DIR="vendor"

usage="usage: $(basename "$0") [--force]"

force=0
for arg in "$@"; do
  case "$arg" in
    --force) force=1 ;;
    -h|--help)
      sed -n '2,28p' "$0" | sed 's/^# \{0,1\}//'
      exit 0
      ;;
    *)
      echo "$usage" >&2
      exit 2
      ;;
  esac
done

# --- Check if vendor/ already exists -----------------------------------------
if [[ -d "$VENDOR_DIR" ]]; then
  if (( force )); then
    echo "setup-vendor: removing existing $VENDOR_DIR/ (--force)"
    rm -rf "$VENDOR_DIR"
  else
    echo "setup-vendor: $VENDOR_DIR/ already exists. Use --force to re-clone." >&2
    exit 0
  fi
fi

# --- Check for git -----------------------------------------------------------
if ! command -v git >/dev/null 2>&1; then
  echo "setup-vendor: git not found on PATH" >&2
  exit 127
fi

# --- Clone -------------------------------------------------------------------
echo "setup-vendor: cloning $VENDOR_REPO -> $VENDOR_DIR/ ..."
if git clone "$VENDOR_REPO" "$VENDOR_DIR"; then
  echo "setup-vendor: done. vendor/ is ready."
  echo ""
  echo "Contents:"
  ls -1 "$VENDOR_DIR"/
else
  echo "" >&2
  echo "setup-vendor: clone FAILED. Possible causes:" >&2
  echo "  1. You don't have SSH access to github.com/contento/smarttar-vendor" >&2
  echo "  2. The repository is private and your SSH key isn't configured" >&2
  echo "  3. Network connectivity issues" >&2
  echo "" >&2
  echo "To fix SSH access:" >&2
  echo "  ssh-add -l                     # check if key is loaded" >&2
  echo "  ssh -T git@github.com          # test GitHub connectivity" >&2
  echo "" >&2
  echo "Alternatively, you can obtain the toolchain components manually:" >&2
  echo "  - Borland C++ 3.1  (bc/)" >&2
  echo "  - Pharlap 286      (pharlap/)" >&2
  echo "  - Zinc 3.5         (zinc/)" >&2
  echo "  See VENDOR_SETUP.md for details." >&2
  exit 1
fi

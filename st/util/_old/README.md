# Deprecated Utilities

This directory contains deprecated utility programs that are no longer
part of the active build. They are kept for historical reference only.

## Files

### `sip.cpp`
Secure ZIP utility. Decrypts a hardcoded password and invokes `pkzip.exe`
with it. Superseded by the modern encryption utilities in `st/util/encrypt/`.

### `sar.cpp`
Secure RAR utility. Same pattern as `sip.cpp` but invokes `rar32.exe`.
Superseded by the modern encryption utilities.

### `rxshow.cpp`
Receipt viewer/display tool. Contains duplicated type definitions that
diverged from the canonical types in `include/`. Superseded by the
viewer utilities in `st/util/viewer/`.

### `rw-temp/`
EEPROM 93CS46 read/write utilities for hardware programming via parallel
port. These were development tools for initial EEPROM programming and
are not needed for normal operation.

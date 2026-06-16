# INI2CFG — Configuration Text-to-Binary Compiler

**Status:** DEPRECATED — Moved to active utilities.

**Purpose:** Compiles human-readable configuration file (ST.INI) into binary SmartTar configuration database (ST.CFG).

## Overview

INI2CFG converts `ST.INI` (text-based configuration with booth settings, operator passwords, tariff mode flags, etc.) into the binary ST.CFG format used by the SmartTar runtime.

**Input:** `ST.INI` — Text file with [sections] and key=value pairs:
```
[BOOTH]
BOOTH_ID=001
NAME=Booth North

[TARIFF]
ENGINE=demo
TARIFF_MODE=QUERY

[SECURITY]
PASSWORD=xxxxx
...
```

**Output:** `ST.CFG` — Binary configuration database

**Build process:**
1. Parser reads ST.INI with error checking (duplicate keys, unknown sections)
2. Validates configuration constraints (booth ID range, password length, mode flags)
3. Encodes configuration into ST.CFG binary format
4. Saves to multiple destinations via MAKEFILE CFG_DESTS rule:
   - `st/bin/ST.CFG` (runtime)
   - `st/util/*/ST.CFG` (each utility gets a copy for offline access)

**Invocation:** Automatically run by MAKEFILE when ST.INI changes; manual invocation only for debugging/recovery.

**Current location:** `st/util/ini2cfg/` (active directory, but kept for historical reference in _deprecated).

## Why deprecated here

The _deprecated/ copies record the original design. The authoritative builds are in `st/util/ini2cfg/` (run as part of the MAKEFILE). Configuration management is now fully automated.

**References:**
- `ST.INI` — Canonical source (st/util/ini2cfg/st.ini)
- CFG class — src/cfg.cpp, include/cfg.h (binary format, validation rules)
- MAKEFILE — Build system integration and distribution

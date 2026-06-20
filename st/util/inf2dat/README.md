# INF2DAT — Telephony Exchange/Tariff Data Compiler

**Status:** DEPRECATED — Moved to active utilities.

**Purpose:** Compiles human-readable telephony exchange and tariff configuration files (`.inf` format) into binary telephony database (PH_INFO.DAT).

## Overview

INF2DAT was the canonical compiler for `.inf` source files that define:
- Exchange codes and routing prefixes
- Local area codes and numbering plans
- Tariff rate tables (call types, duration brackets, charges)
- Geographic place names and booth locations

**Input files:**
- `ddi.inf` — Direct inward dial exchange data
- `ddn.inf` — Dial directory numbers
- `local.inf` — Local area and prefix definitions
- Encoding: ISO-8859-1 (Latin-1)

**Output:** `PH_INFO.DAT` — Binary database used by PH_ENGINE at runtime

**Build process:**
1. Parser reads `.inf` files with validation
2. Compiles to intermediate binary format
3. Checksums and validates record counts
4. Outputs PH_INFO.DAT to `util/inf2dat/` then MAKEFILE copies to `bin/`

**Invocation:** Automatically run by MAKEFILE when any `.inf` source changes; no manual invocation needed.

**Current location:** `st/util/inf2dat/` (active directory, but kept for historical reference in _deprecated).

## Why deprecated here

The _deprecated/ copies of inf2dat and ini2cfg are kept for reference only. The authoritative builds are in `st/util/inf2dat/` and `st/util/ini2cfg/` (run as part of the MAKEFILE). This history snapshot records their original design.

**References:**
- `.inf` source files — st/util/inf2dat/*.inf (ddi.inf, ddn.inf, local.inf)
- Parser — src/ph/parser.cpp, include/parser.h
- PH_ENGINE — src/ph/ph_eng.cpp (runtime tariff lookup)

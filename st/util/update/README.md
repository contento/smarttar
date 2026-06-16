# UPDATE — Tariff Data Update Tool

**Purpose:** Creates and applies tariff rate table updates without requiring full system reinstall.

## Overview

UPDATE allows technicians to roll out new tariff data (exchange codes, rate tables, local prefixes) after initial deployment. It supports incremental updates, reducing data transfer overhead on slow modem connections.

**Usage:**
```
update /c          # Create update package
update /a          # Apply update package
```

**Workflow (create):**
1. Compares current tariff data (PH_INFO.DAT) against baseline
2. Generates binary diff/patch file (`.upd` format)
3. Compresses with optional encryption
4. Output ready for modem transmission or diskette distribution

**Workflow (apply):**
1. Loads existing PH_INFO.DAT
2. Reads update package from stdin or file
3. Applies patch to existing tariff data
4. Validates result (checksum, record count)
5. Commits updated PH_INFO.DAT to disk

**Update formats:**
- `.upd` — Binary update package (versioned, checksummed)
- Incremental only — never replaces entire PH_INFO.DAT to minimize transmission

**Status:** Active. Post-deployment tariff maintenance tool. Critical for managing rates across distributed booths without field visits.

**References:**
- PH_ENGINE — tariff storage and lookup (src/ph/ph_eng.cpp)
- File format — include/filehdr.h, PH_INFO structure
- Binary patching — st_util.cpp — binary diff/merge utilities

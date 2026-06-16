# REPAIR — Transaction Archive Repair

**Purpose:** Repairs and rebuilds receipt transaction archives from dated backup segments.

## Overview

REPAIR is a recovery tool for corrupted or incomplete transaction databases. It reconstructs the main receipt database (RX.DAT / RX.IDX) by recovering committed transactions from the archived dated backup files.

**Workflow:**
1. Prompts for the date of the last successful archive
2. Locates archived segments in the yearly/monthly directory structure: `YYYY/MM/RXYY_MM.STA`
3. Reconstructs the transaction database by merging recovered segments
4. Validates the rebuilt database
5. Creates a new RX.DAT / RX.IDX from recovered transactions

**Archive structure:**
- Organized by year and month: `1999/12/rx99_12.sta` (last committed transactions for Dec 1999)
- STA format: Archive of transactions for that period

**Status:** Active. Post-incident recovery tool. Used after database corruption or incomplete shutdown.

**References:**
- DB_ENGINE — database access and recovery (src/db/db_eng.h, src/db/dstorage.cpp)
- File structure — `util/inf2dat/` and dating conventions

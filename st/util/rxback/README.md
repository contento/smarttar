# RXBACK — Transaction Backup Converter

**Purpose:** Converts transaction database files (RX.DAT) to backup format (RX.DAB) for archival and recovery.

## Overview

RXBACK is a simple file conversion utility that renames and archives the active transaction database into a timestamped backup. It prepares transaction data for offline archival, long-term storage, or recovery operations.

**Workflow:**
1. Takes a filename as argument (e.g., RX.DAT)
2. Changes file attributes to read-write (removes read-only / hidden flags)
3. Renames file from `.DAT` to `.DAB` extension
4. Verifies no existing `.DAB` file exists (prevents overwrite)

**File naming:** 
- Input: RX.DAT → Output: RX.DAB
- Example: RX.DAT (2026-06-16) → RX.DAB

**Status:** Active. Lightweight backup utility. Used before destructive operations or as part of archival scripts.

**References:**
- STL.CPP — batch processing may invoke RXBACK as part of scheduled backups
- File header format — `include/filehdr.h`

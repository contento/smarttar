# STM2 — Session and Login Management Utilities

**Purpose:** Session database utilities for SmartTar Multi-Machine (STM2) deployments with shared login servers.

## Overview

This directory contains tools for managing the STM2 session database, which tracks operator logins, sessions, and audit trails across multiple booths sharing a central server.

## Utilities

### DUMP — Session Database Dump
Exports the STM2 session database to a hex dump for inspection and recovery.

**Usage:**
```
dump [base] [count]
  base:  starting offset (default: 0)
  count: bytes to dump (default: 256)
```

**Purpose:** Low-level inspection of session data, verification of session integrity, recovery of corrupted sessions by exporting to a text editor.

### F2STM2 — File to Session Database Converter
Converts flat transaction or session files to STM2 session database format, enabling bulk session import.

**Purpose:** Migration tool for converting legacy session data or importing from external systems into STM2 format.

**Status:** Active. STM2 is the multi-machine deployment model (server-based operator login and audit logging). These utilities support configuration recovery, database inspection, and cross-system data migration.

**References:**
- STM2 class — include/stm2.h, src/stm2.cpp
- Session format — SID, OPERATOR, BOOTH identity mappings
- Multi-machine architecture — CLAUDE.md § (server/client model, network auth)

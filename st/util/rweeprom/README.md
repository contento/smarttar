# RWEEPROM — EEPROM Read/Write Utility

**Purpose:** Reads and writes data to the system EEPROM (93CS46) for hardware configuration and personalization.

## Overview

RWEEPROM provides low-level access to the EEPROM chip on the SmartTar hardware. It allows field technicians to program device-specific data (booth identity, serial numbers, location codes) directly into non-volatile memory.

**Usage:**
```
rweeprom        # Read EEPROM (default)
rweeprom /w     # Write EEPROM
```

**Workflow (read):**
1. Prompts for date-based access code (master unlock)
2. Reads up to 80 bytes from EEPROM
3. Displays the bytes read

**Workflow (write):**
1. Prompts for date-based access code
2. Accepts new data from stdin
3. Writes to EEPROM with ASCIIZ terminator
4. Verifies write by reading back

**EEPROM:** 93CS46 serial EEPROM connected via parallel port or direct hardware interface.

**Status:** Active. Hardware initialization and recovery tool. Critical for booth identification and booth-to-tariff mapping.

**References:**
- EEPROM class — src/eeprom.cpp, include/eeprom.h
- Hardware interface — parallel port or dedicated EEPROM programmer

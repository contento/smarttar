# RW-TEMP — EEPROM 93CS46 Parallel Port Programmer

**Status:** DEPRECATED — Superseded by RWEEPROM utility and integrated EEPROM hardware support.

**Purpose:** Low-level utilities for reading and writing the 93CS46 serial EEPROM chip via parallel port bit-banging.

## Overview

RW-TEMP contains raw EEPROM programming code that directly manipulates parallel port I/O pins to communicate with the 93CS46 EEPROM chip. This was used for initial hardware programming during manufacturing or field initialization before EEPROM hardware support was integrated into the main system.

## Programs

### r.cpp — EEPROM Read
Reads data from 93CS46 EEPROM using parallel port bit-banging.

**I/O mapping:**
- Port 0x28F: Input (READ line)
- Port 0x28D: Output (Control lines: DI, CS, PRE, PE, SK)

**Protocol:**
1. Initialize EEPROM (send init codes)
2. Set read mode (0x80 command)
3. Bit-bang 80 bytes (clock + read)
4. Return ASCIIZ string

### w.cpp — EEPROM Write
Writes data to 93CS46 EEPROM.

**Similar port-banging approach but with write enable and verification.**

### reade2pr.c / wre2pr.c
Alternative implementations (C instead of C++), possibly for debugging or alternate hardware configurations.

**Why deprecated:**
- Modern EEPROM access is abstracted via EEPROM class (src/eeprom.cpp)
- Parallel port support is no longer standard on modern hardware (USB adapters required)
- Integrated RWEEPROM utility provides safer, user-friendly interface with authentication
- Direct I/O port access at privilege level is risky and incompatible with modern OS

**Historical value:** Useful for understanding 93CS46 protocol timing, parallel port signaling, and bit-banging techniques.

**References:**
- EEPROM class — src/eeprom.cpp (modern abstraction)
- RWEEPROM — st/util/rweeprom/ (successor utility)
- 93CS46 datasheet — Serial EEPROM protocol (clock, chip select, data lines)

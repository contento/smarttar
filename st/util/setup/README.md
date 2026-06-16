# SETUP — Installation and Configuration Wizard

**Purpose:** Interactive installation utility for configuring SmartTar on new hardware or recovering lost configuration.

## Overview

SETUP is a full-screen Zinc UI application that guides technicians through initial system setup, configuration entry, tariff data installation, and EEPROM programming. It handles both clean installation and configuration recovery.

**Modes:**
- **INSTALL:** First-time setup of a new SmartTar booth
- **SETUP:** Reconfigure an existing installation

**Workflows:**

**Installation:**
1. Verifies system configuration and software compatibility
2. Prompts for location code, booth identity
3. Guides tariff data entry (exchange code, local prefix, rate tables)
4. Programs EEPROM with booth identity
5. Verifies all data and creates initial ST.CFG
6. Runs peripheral hardware tests

**Reconfiguration:**
1. Loads existing ST.CFG
2. Allows selective updates to settings
3. Supports tariff data refresh without full reinstall

**Key buttons (Zinc UI):**
- INI2CFG: Compile configuration to binary format
- CFG2INI: Export configuration to text form
- INF2DAT: Compile tariff data
- DAT2INF: Export tariff data
- Test: Hardware diagnostics

**Status:** Active. Core deployment tool. Replaces command-line config utilities for technician-facing workflows.

**References:**
- Zinc UI framework — UIW_WINDOW, UIW_PUSHBUTTON, event handling
- CFG class — src/cfg.cpp, include/cfg.h
- PH_ENGINE — tariff and exchange data (src/ph/ph_eng.cpp)
- EEPROM — src/eeprom.cpp

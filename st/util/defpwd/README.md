# DEFPWD — Default Password Reset

**Purpose:** Resets administrator account passwords using a master unlock code.

## Overview

DEFPWD allows recovery from locked administrative accounts by resetting passwords to factory defaults. It requires a valid date-based access code (derived from the system date and embedded in the application) to authorize the reset.

**Workflow:**
1. Loads configuration (ST.CFG)
2. Prompts for a master access code
3. Validates the code against the date-based password check (`_isDatePassword()`)
4. If valid, resets all configured passwords to defaults
5. Saves the updated configuration

**Access code:** Date-derived code embedded at build time. Vendors provide the code to authorized technicians.

**Status:** Active. Emergency account recovery tool. Should remain accessible even when main app is locked.

**References:**
- CFG — configuration and password management (src/cfg.cpp, src/cfg.h)
- st_util.cpp — `_isDatePassword()`, `_ReadPassword()` utilities

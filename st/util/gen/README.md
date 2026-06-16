# GEN — Distribution Package Generator

**Purpose:** Creates customized installation packages for SmartTar deployments.

## Overview

GEN packages SmartTar executables and supporting files into a self-extracting distribution archive (STX.EXE). It embeds location-specific metadata (serial number, version tier, configuration) directly into the binaries before packaging.

**Workflow:**
1. Looks up a 3-digit location code in `GEN.LST`
2. Retrieves the associated serial number, cluster configuration, and options (Pro, EDA, NOEEPROM)
3. Encrypts and patches multiple executables with this metadata
4. Compresses the patched files using PKZIP → ZIP2EXE
5. Copies the resulting self-extracting archive to a distribution diskette

**Patched executables:**
- ST.EXE (main application)
- SETUP.EXE, INF2DAT.EXE, INI2CFG.EXE, REPAIR.EXE, UPDATE.EXE, DUMP.EXE, VIEWLOG.EXE

**Status:** Active. Obsolete distribution mechanism (diskette-based), but generator logic may be referenced for understanding versioning/patching patterns.

**References:** CLAUDE.md § Conventions (patching, SUPER_APP_INFO structure)

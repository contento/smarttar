# CHKRX — Receipt Database Checker

**Purpose:** Validates the integrity and structure of the receipt transaction database.

## Overview

CHKRX is a command-line utility that performs sanity checks on the receipt database (RX.DAT / RX.IDX). It iterates through all receipts, validates their structure, and reports any corruption or anomalies.

**Features:**
- Reads RX.DAT transaction records sequentially
- Validates receipt structure and fields
- Outputs all receipt numbers for audit/verification
- Requires valid configuration (ST.CFG) and admin password
- Memory allocation: ~8 KB stack

**Exit codes:**
- 0: Success
- 1: Initialization failure
- Other: Database corruption detected

**Status:** Active. Used in automated database maintenance and repair workflows.

**References:** 
- DB_ENGINE — database access layer (src/db/db_eng.h)
- CFG — configuration and authentication (src/cfg.cpp)

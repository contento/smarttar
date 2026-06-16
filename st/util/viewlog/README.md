# VIEWLOG — Event Log Viewer

**Purpose:** Command-line utility for browsing system event logs and diagnostic traces.

## Overview

VIEWLOG displays the SmartTar event log (LOG.DAT) in human-readable format. It shows system events, errors, operator actions, and maintenance operations with timestamps for diagnostic and audit purposes.

**Usage:**
```
viewlog               # View default system log (LOG.DAT)
viewlog filename.log  # View custom log file
```

**Log entry structure:**
- Timestamp (hour:minute:second, date)
- Event code / category
- Event description / message
- Operator ID (if logged in)

**Event types:**
- **System:** Startup, shutdown, configuration load/save
- **Error:** Database corruption, EEPROM failures, hardware errors
- **Operator:** Login, logout, configuration changes, special operations
- **Maintenance:** Backup, repair, update application

**Output format:**
- Sequential human-readable list
- Timestamps formatted as HH:MM:SS and DD/MM/YYYY
- One entry per line

**Typical uses:**
- Post-incident diagnosis (crash logs, error sequences)
- Audit trail for regulatory compliance
- Operator activity verification
- System health monitoring

**Status:** Active. Low-level diagnostic tool for technicians and support.

**References:**
- LOG class — include/log.h, src/log.cpp (log file format, entry structure)
- CFG authentication — required to prevent unauthorized access to logs

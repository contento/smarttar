---
title: "System Log Events"
lang: en
manual: reference-manual-en
order: 12
---

# System Log Events
| Event constant | Description |
|----|----|
| NORMALSTART | Application started successfully. |
| NORMALSHUTDOWN | Application exited cleanly. |
| APPBADTRY | Application failed to start (RES.DAT missing or corrupt). |
| CFGBADTRY | Application failed to start (ST.CFG missing or corrupt). |
| STM2BADTRY | Access denied: STM2 hardware authentication failed. |
| DONGLEBADTRY | Access denied: hardware dongle not found (demo build). |
| EEPROMBADTRY | Access denied: EEPROM version mismatch. |
| STM2GARBAGE | STM2 state was corrupted at startup; state was reset. |
| STM2BADSHUTDOWN | Previous session did not terminate cleanly; recovery initiated. |
| STM2RECOVER | System entered recovery mode after bad shutdown. |
| STM2IGNORERECOVER | Operator bypassed recovery after bad shutdown. |

Log entries include date, time, and the relevant event code. The log file is read-only when not being written; the application temporarily sets it read-write before appending.

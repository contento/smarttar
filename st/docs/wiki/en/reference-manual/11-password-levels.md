---
title: "Password Levels"
lang: en
manual: reference-manual-en
order: 11
---

# Password Levels
| Level | Internal name | Access granted |
|----|----|----|
| Backdoor | BACKDOOR | Full system access including configuration changes. Used by service technicians. |
| Supervisor | SUPERVISOR | Full operational access: configuration, reports, end-of-turn, telephony database. |
| User 1 | USER1 | Standard operator access: booth monitoring, printing receipts, manual settlement. |
| User 2 | USER2 | Restricted operator access. |
| Operator | OPERATOR | View-only access to extension services and information screens. Cannot modify configuration. |
| Utility | UTIL | Access for the UTIL diagnostic utility program. |

Passwords are 8 characters maximum and are stored encrypted in ST.CFG. The STM2 EEPROM validates the logged-in state; if a bad shutdown is detected, the system prompts for recovery confirmation before proceeding.

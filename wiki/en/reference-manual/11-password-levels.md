---
title: "Password Levels"
lang: en
manual: reference-manual-en
order: 11
---

# Password Levels
| Level | Internal name | Default code | Access granted |
|----|----|----|----|
| Backdoor | BACKDOOR | (vendor-reserved) | Full system access including configuration changes. Used by service technicians. |
| Supervisor | SUPERVISOR | `Super` | Full operational access: configuration, reports, end-of-turn, telephony database. |
| User 1 | USER1 | `User1` | Standard operator access: booth monitoring, printing receipts, manual settlement. |
| User 2 | USER2 | `User2` | Restricted operator access. |
| Operator | OPERATOR | `Oper` | View-only access to extension services and information screens. Cannot modify configuration. |
| Utility | UTIL | `Util` | Access for the UTIL diagnostic utility program. |

**Default codes.** A fresh installation -- or one reset with the DEFPWD utility -- uses the codes in the table. They are **case-sensitive** and accept up to 8 characters. **Activating the Configuration menu** accepts any valid code *except* `Util`; `Super` grants full access, the others a restricted access. A vendor-reserved master recovery code also exists (not published). Change these default codes on any production installation via *Configuración > Cambiar código de acceso*.

Passwords are 8 characters maximum and are stored encrypted in ST.CFG. The STM2 EEPROM validates the logged-in state; if a bad shutdown is detected, the system prompts for recovery confirmation before proceeding.

---

## Related help topics

The following in-app help topics (Spanish, compiled into `help.dat`) cover related functionality:

- [[es/ayuda/H_PASSWORD|H_PASSWORD]]
- [[es/ayuda/H_CHANGE_PASSWD|H_CHANGE_PASSWD]]

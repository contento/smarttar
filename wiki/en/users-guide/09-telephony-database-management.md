---
title: "Telephony Database Management"
lang: en
manual: users-guide-en
order: 9
---

# Telephony Database Management
The telephony database defines how calls are classified and priced. It consists of the numbering plan (LOCAL.INF, DDN.INF, DDI.INF) compiled into PH_INFO.DAT.

## When to Update the Database

Update the telephony database when:

- The telephone carrier changes tariff rates or schedules.
- New area codes or country codes are added.
- New destinations need to be blocked (added to the locked numbers list).
- Local numbering plan prefixes change.

## Update Procedure

1.  Edit the source files (LOCAL.INF, DDN.INF, DDI.INF) using a plain-text editor, or use the tariff editor in **Configuración \> Tarifa**.
2.  Run the SETUP utility to recompile the source files into PH_INFO.DAT.
3.  Restart SmartTar. The new database is loaded at startup.

**Note:** Do not edit PH_INFO.DAT directly. It is a compiled binary file and will be overwritten the next time SETUP runs.

## Locked Numbers

Locked numbers are telephone numbers or prefixes that are blocked from use. When a customer dials a locked number, the call is intercepted before it connects and no charge is incurred. To add or remove locked numbers, use **Configuración \> Tarifa \> Números Bloqueados** and recompile the database.

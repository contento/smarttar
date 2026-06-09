---
title: "Hardware Interface"
lang: en
manual: reference-manual-en
order: 14
---

# Hardware Interface
## STB-x Cluster Board

Each cluster board handles 8 telephone lines via the ISA I/O port bus. Cluster boards are assigned consecutive 16-byte I/O blocks starting at APP_PORT_BASE. With 4 clusters, total I/O space used is 64 bytes.

The real-time engine polls cluster ports at regular timer ISR intervals. The PO_GENERAL output port drives the cash drawer relay (GP_CASH bit) when ACTIVATE_RELAY is set.

## EEPROM Dongle

The hardware protection dongle connects to the parallel port (LPT1). It contains an EEPROM storing the system serial number, the maximum allowed cluster count, and the version key. At startup, the application reads the EEPROM, validates the version, and records the login event. On normal exit, it records the logout. An abnormal exit leaves the STM2 state as BAD_SHUTDOWN, detected on the next startup.

## Booth Display

An optional customer-facing LED display can be connected to a serial port (DISPLAY_COM). The BoothDisplay class sends the current call cost or DISPLAY_DEFAULT_MESSAGE to the display in real time. Enabled by setting DISPLAY_ENABLE = TRUE in ST.INI.


*SmartTar Reference Manual — Version 2.34* *MicroDiseño Ltda. — Copyright © 1993–2003. All rights reserved.* *Gonzalo Contento Castañ*

---

## Related help topics

The following in-app help topics (Spanish, compiled into `help.dat`) cover related functionality:

- [[es/ayuda/H_INSTALL|H_INSTALL]]
- [[es/ayuda/H_P_PORT|H_P_PORT]]
- [[es/ayuda/H_S_PORT|H_S_PORT]]

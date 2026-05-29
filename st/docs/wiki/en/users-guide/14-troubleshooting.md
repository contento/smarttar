---
title: "Troubleshooting"
lang: en
manual: users-guide-en
order: 14
---

# Troubleshooting
## Application Will Not Start

| Symptom | Action |
|----|----|
| “Acceso Negado” message | Check that the dongle is connected to LPT1. Verify the dongle version matches this release. |
| “RES.DAT” error | Verify that RES.DAT exists in the application directory. Reinstall from the distribution media. |
| “Configuración no disponible” | ST.CFG is missing or corrupt. Run the SETUP utility to reinstall configuration. |
| “Versión no válida” | EEPROM version mismatch. Contact your distributor for a firmware update. |
| DOS error or program abort | Insufficient extended memory. Ensure no memory-resident programs are consuming extended memory. |

## Booths Not Detected

| Symptom | Action |
|----|----|
| ACTIVE_CLUSTERS shows 0 | Check that the STB-x boards are seated correctly in the ISA bus and that power is connected. |
| Fewer clusters than expected | One or more STB-x boards may have failed. DTMF flag port returns 0xFF for missing boards. |
| No calls detected on a booth | Verify the telephone line is connected to the correct port on the STB-x board. Check CLUSTERS in ST.INI. |

## Billing Problems

| Symptom | Action |
|----|----|
| Billing starts too early or too late | Adjust T_TALK (grace period) and the answer signal method in Configuración \> Señal de Contestación. |
| Calls classified incorrectly | Check the numbering plan in LOCAL.INF / DDN.INF. Verify ACCESS, INTER_ACCESS, CELLULAR_ACCESS, and digit count parameters. |
| Wrong tariff rate applied | Verify tariff bands and schedules in Configuración \> Tarifa. Check APPLY_DDN_SCHEDULE / APPLY_DDI_SCHEDULE settings. |
| Receipt total rounds unexpectedly | Check M_ROUND in application settings. Set to 0 to disable rounding. |

## Printer Problems

| Symptom | Action |
|----|----|
| No receipts printed | Verify the printer is online and connected to the configured port. Check P_PORT and FORM in Configuración \> Impresora. |
| Garbled or misaligned output | Verify the FORM selection matches the physical paper width and printer model. |
| Print queue builds up | Check for printer hardware faults (paper jam, offline). The spooler retries automatically when the printer is ready. |

## Bad Shutdown / Recovery Issues

- Always allow recovery to proceed (do not bypass) unless instructed by a service technician.
- After recovery, verify that the receipt counter is correct by checking the last receipt in the database view.
- Frequent bad shutdowns indicate a power supply problem. Install an uninterruptible power supply (UPS).

## Database Issues

- If RX.DAT becomes corrupted, use the SETUP or UTIL utility to run a database recovery. Back up RX.DAT and RX.IDX before running any repair tool.
- Do not delete RX.DAT or RX.IDX while SmartTar is running.
- If the database exceeds its maximum record count, start a new turn to archive the current data.

## Obtaining Support

When reporting a problem, provide the following information:

- SmartTar version and serial number (from **Información \> Acerca de**).
- The contents of the ST.LOG file from the application directory.
- A description of the steps leading to the problem.
- Whether a bad shutdown event was recorded.


*SmartTar User’s Guide — Version 2.34* *MicroDiseño Ltda. — Copyright © 1993–2003. All rights reserved.* *Gonzalo Contento Castañ*

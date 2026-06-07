---
title: "Starting the System"
lang: en
manual: users-guide-en
order: 2
---

# Starting the System
## Prerequisites

- The hardware dongle (EEPROM key) must be connected to the parallel port (LPT1) before starting.
- The STB-x telephone cluster boards must be installed and connected to the telephone lines.
- The printer must be connected, powered on, and loaded with paper.
- All runtime data files (ST.CFG, RES.DAT, PH_INFO.DAT, etc.) must be present in the application directory.

## Starting the Application

From the DOS prompt, change to the application directory and run:

    C:\SMARTTAR\BIN> ST

The application loads the Pharlap DOS extender, displays the version and serial number, then initializes the graphical interface. The main window opens maximized.

## Startup Sequence

During startup, SmartTar performs the following steps:

1.  **Hardware authentication.** The EEPROM dongle is read and validated. If the dongle is missing or the version does not match, the message **“Acceso Negado”** is displayed and the application exits.
2.  **Configuration load.** ST.CFG and ST.INI are read from the application directory.
3.  **Telephony database load.** PH_INFO.DAT is loaded into protected-mode memory.
4.  **Cluster detection.** The real-time engine probes each configured cluster port to count physically installed STB-x boards. The ACTIVE_CLUSTERS count is updated accordingly.
5.  **Bad shutdown recovery.** If the previous session did not terminate cleanly, a recovery prompt is displayed.
6.  **Main view display.** The booth monitoring view is shown, maximized to full screen.

## Bad Shutdown Recovery

If the previous session ended abnormally (power failure, system reset, application crash), the EEPROM dongle records this as a BAD_SHUTDOWN event. On the next startup, SmartTar displays the date and time of the abnormal termination and asks whether to proceed with recovery.

During the 5-second recovery window, press the appropriate key to bypass recovery, or wait for the timeout to let recovery proceed automatically. Recovery restores the receipt counter from the last known good EEPROM state, preventing duplicate or missing receipt numbers.

**Note:** Always allow recovery to complete if you are unsure. Bypassing recovery may result in receipt numbering gaps.

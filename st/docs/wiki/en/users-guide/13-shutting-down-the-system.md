---
title: "Shutting Down the System"
lang: en
manual: users-guide-en
order: 13
---

# Shutting Down the System
Follow this procedure to shut down SmartTar safely:

1.  **Verify all booths are idle.** The booth view should show every booth as *Libre*. If any calls are still active, wait for them to complete.
2.  **Close the current turn** if a shift change is occurring (see Section 10).
3.  **Print the turn receipt** for supervisor verification if required.
4.  Press **Alt+F4** or close the main window. A confirmation dialog asks: *“Terminar la sesión de trabajo?”* (End the work session?).
5.  Click **Sí** (Yes) to confirm. The application flushes all databases, records the logout event in the EEPROM, and exits to DOS.

**Warning:** Do not power off the PC or reboot while SmartTar is running, especially not while calls are active. Doing so will cause a bad shutdown event, which must be recovered on the next startup. Always exit SmartTar cleanly before powering off.

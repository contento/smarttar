---
title: "Error Messages"
lang: en
manual: reference-manual-en
order: 13
---

# Error Messages
## Startup Errors

- **“Acceso Negado” (Access Denied).** STM2 hardware authentication failed, EEPROM version mismatch, or dongle not found. Application exits after this message.
- **“Configuración no disponible” (Configuration not available).** ST.CFG is missing or corrupted. Run the SETUP utility to reinstall.
- **errno + “\RES.DAT”.** Zinc resource file cannot be opened. Verify RES.DAT is present in the application directory.
- **“Versión no válida” (Invalid version).** EEPROM version does not match this release. Contact the distributor for a firmware update.

## Runtime Errors

- **“El sistema está procesando información” (System is processing).** Displayed in the exit dialog when the real-time engine has active calls. Wait for all booths to go idle before exiting.
- **Out of memory.** The new_handler is invoked when heap allocation fails. The system attempts to release the emergency memory reserve (2 KB) and display a message.
- **GPF (General Protection Fault).** Unhandled GPFs in 286 protected mode are caught by a custom exception handler (NewGPFHandler). The handler records the fault and attempts a controlled exit.

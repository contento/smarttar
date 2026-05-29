---
title: "Configuration Menu"
lang: en
manual: users-guide-en
order: 6
---

# Configuration Menu
The Configuration menu (**Configuración**) provides dialogs for adjusting system parameters. Most dialogs require Supervisor-level access.

> **Activating the menu.** The Configuration menu is hidden by default and has
> **no keyboard shortcut**. To reveal it: press [ALT] to enter the menu bar,
> open **Información** and choose **Activar menú de configuración**. Once shown,
> it opens with its **Alt+C** mnemonic. When finished, hide it again via
> **Configuración > Desactivar este menú**.

## Fecha y Hora (Date and Time)

Sets the system date and time. Changes take effect immediately and are reflected in the status bar. This dialog writes the new time to the DOS real-time clock.

**Path:** Configuración \> Fecha y Hora

## Señal de Contestación (Answer Signal)

Selects the method used to detect when a call is answered. Four methods are available:

| Method | Description |
|----|----|
| Por inversión | Answer detected by a reversal of line polarity. Select **Permanente** if sustained, or **Durante 150 ms** if a brief pulse. |
| Por tiempo | Answer assumed after a configurable number of seconds. Enter the timeout in the *Tiempo* field. |
| Por hilo C | Answer detected by a C-wire earth or −48 V signal. |
| Por tono | Answer detected by tone recognition. |

**Path:** Configuración \> Señal de Contestación

**Note:** Select the method that matches your telephone central office. An incorrect setting will cause premature or missed billing starts.

## Tiempos (Timing)

Adjusts the telephony timing parameters. Fields correspond to T_TALK, T_DIAL, T_LOCK, T_ANSWER, and T_COM. All values are entered in seconds and stored internally in milliseconds.

**Path:** Configuración \> Tiempos

**Caution:** Changing timing parameters affects all booths immediately. Use default values unless directed by a service technician.

## Nombres de Cabinas (Booth Names)

Assigns a display name (up to 12 characters) to each booth. These names appear in the main view, on receipts, and in reports. In SmartTar Pro, each booth can also be designated as an extension line.

**Path:** Configuración \> Nombres de Cabinas

## Impresora (Printer)

Configures the printer connection and form type. Select the appropriate form from the drop-down list. Set the port to LPT1, LPT2, or a COM port as appropriate for your hardware.

**Path:** Configuración \> Impresora

## Tarifa (Tariff)

Opens the tariff configuration editor. From here, supervisors can view and edit:

- DDN tariff bands and rates.
- DDI tariff bands and rates.
- Time-of-day schedules (normal, reduced, super-reduced) for DDN and DDI.
- Local tariff levels (A1–A9).
- Locked numbers list.

**Path:** Configuración \> Tarifa

After editing, use **Base de datos telefónica \> Compilar** to recompile PH_INFO.DAT and make the new tariffs active.

## Días Festivos (Holidays)

Defines the holiday calendar for the current and upcoming years. Holiday dates are used by the tariff scheduler to switch to holiday tariff rates. Enter dates as day/month pairs.

**Path:** Configuración \> Días Festivos

## Claves (Passwords)

Allows the supervisor to change access passwords. Enter the current password, then enter and confirm the new password. Passwords are 1–8 characters.

**Path:** Configuración \> Claves

**Important:** Keep a record of the supervisor password in a secure location. If it is lost, a service technician with backdoor access is required to reset it.

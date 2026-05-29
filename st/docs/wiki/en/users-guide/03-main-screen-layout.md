---
title: "Main Screen Layout"
lang: en
manual: users-guide-en
order: 3
---

# Main Screen Layout
The main SmartTar screen is divided into four functional areas.

## Menu Bar

Located at the top of the screen. Available menus depend on the current operator’s access level:

| Menu | Contents |
|----|----|
| **Archivo** (File) | Turn management (start/end turn), archiving, exit. |
| **Imprimir** (Print) | Report printing: sales accumulator, booth history, transaction log. |
| **Configuración** (Configuration) | System configuration dialogs: time/date, signal type, timing, printer, telephony settings. |
| **Información** (Information) | System information, version, serial number. |
| **Simulación** (Simulation) | (Demo/development builds only.) Simulated call testing. |
| **Ayuda** (Help) | In-application help and about dialog. |

## Toolbar

The toolbar below the menu bar provides icon buttons for the most frequent operations, including manual settlement, printing, configuration shortcuts, and view options. Hover over a button to see its function in the status bar.

## Booth View (Main Working Area)

The central area shows a grid of booth cells, one per telephone booth. Each cell displays:

- Booth name or number (configurable in **Configuración \> Nombres de cabinas**).
- Current call state (idle, off-hook, dialing, ringing, connected, locked).
- Elapsed call time (displayed in real time during a connected call).
- Accumulated call cost (updated in real time; decimal places set by VIEW_DECIMALS).
- Call type indicator (local, DDN, DDI, cellular, border).
- Alarm indicators: communication error flag, dial error flag.

## Status Bar

The status bar at the bottom of the screen shows the current date and time, current turn number, number of active calls, last receipt number issued, and operator name and access level.

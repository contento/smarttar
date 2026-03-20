# SmartTar

Real-time telephone tariff management system for public telephone booths

Developed by [MicroDiseño Ltda.](https://micronotes.com) · Copyright © 1993–2003 · Version 2.34

![SmartTar](st/docs/SmartTar.jpg)

---

## Overview

SmartTar is a DOS-based point-of-sale system designed for Colombian telecommunications operators running public telephone booth centers (*cabinas telefónicas*). It monitors active calls in real time, classifies them by destination, applies the correct tariff schedule, prints itemized receipts with tax, and maintains a full transaction database — all from a single protected-mode DOS executable.

### Key capabilities

- **Real-time call metering** — monitors up to 32 telephone booths (4 clusters × 8 booths) simultaneously via serial line
- **Automatic call classification** — resolves dialed numbers against a configurable numbering plan tree to determine call type: local, national long-distance (DDN), international (DDI), border, cellular, or special
- **Tariff engine** — applies time-of-day and day-type schedules (up to 16 DDN / 20 DDI tariff bands, 5–6 schedule slots, 3 day types including holidays)
- **Holiday calendar** — configurable per year, used by the tariff scheduler
- **Receipt printing** — issues numbered, tax-inclusive receipts (IVA) through a plug-in printer driver system supporting 18-, 40-, and 80-column printers plus thermal models
- **Transaction database** — indexed file store (`RX.DAT` / `RX.IDX`) with duplicate detection and archiving
- **Prepaid magnetic card support** — up to 4 card denominations
- **Modem integration** — for remote reporting and synchronization
- **External booth display** — optional serial customer-facing display per booth
- **Statistics module** — daily call and revenue summaries
- **Hardware dongle** — copy-protection via parallel-port dongle (bypassable in demo builds)

---

## System Requirements

| Component | Requirement |
| --- | --- |
| OS | MS-DOS 5.0 |
| CPU | 286 or better |
| Memory | Extended memory via Pharlap 286 DOS Extender |
| Printer | Serial or parallel; 18-, 40-, or 80-column |
| Hardware lock | Parallel-port dongle (production builds) |

---

## Toolchain

| Tool | Version |
| --- | --- |
| Compiler | Borland C++ 3.1 (`BCC` / `BCC286`) |
| Assembler | Turbo Assembler (`TASM`) |
| Linker | `TLINK` + Pharlap `BIND286` |
| UI framework | Zinc Interface Library 3.5 |
| DOS extender | Pharlap 286 v3.0 |
| Build system | Borland `MAKE` |

All toolchain binaries are vendored under [`BC/`](BC/), [`PHARLAP/`](PHARLAP/), and [`ZINC/`](ZINC/).

---

## Repository Layout

```text
BC/              Borland C++ 3.1 — compiler, debugger (TD), TASM, TLIB, TLINK
PHARLAP/         Pharlap 286 DOS Extender — BCC286, BIND286, CFIG286, RUN286, runtime DLLs
ZINC/            Zinc Interface Library 3.5 — headers, pre-built libs, designer tools
st/              Application
  source/        C++ and C source files (.cpp / .c / .asm)
  include/       Header files
  obj/           Object files — intermediate build output (not committed)
  lib/           Static libraries
  bin/           Output binaries and runtime data
  MAKEFILE       Borland MAKE build script
  MV.BAT         DOS mv replacement (copy + erase)
  MKD.BAT        Shortcut: make demo + bind (DEMO + RUN + NODONGLE)
  S.BAT          Run st.exe from the st/ directory
```

---

## Building

All build commands are run from the `st/` directory inside a DOS 5.0 environment (physical machine, DOSBox, or compatible emulator).

### Standard release build

```bat
cd st
make RUN=1
```

`RUN=1` invokes `BIND286` and `CFIG286` after linking to produce a self-contained Pharlap protected-mode executable.

### Build flags

| Flag | Effect |
| --- | --- |
| `RUN=1` | Bind and configure the executable with Pharlap (`BIND286` + `CFIG286`) |
| `DEBUG=1` | Enable debug symbols (`-v`) and linker debug info |
| `DEMO=1` | Define `__DEMO__` — activates demo mode |
| `NODONGLE=1` | Define `__NO_DONGLE__` — skip dongle check (requires `DEMO=1`) |
| `AUTO=1` | Define `__AUTO__` — simulation / auto-pilot mode |
| `EDA=1` | Define `__EDA__` — build variant for EDA operator |
| `HELP=1` | Regenerate `bin/help.dat` from `doc/help.txt` via `genhelp` |

**Demo build (no dongle required):**

```bat
make RUN=1 DEMO=1 NODONGLE=1
```

Or use the shortcut:

```bat
mkd
```

**Debug build:**

```bat
make DEBUG=1 RUN=1
```

### Output

The build produces:

- `bin/st.exe` — main protected-mode application
- `bin/pr_*.dll` — printer driver DLLs (one per supported printer model)

### Printer driver DLLs

Printer drivers are compiled as Pharlap DLLs from `source/pr_*.c`:

| DLL | Description |
| --- | --- |
| `pr_sr80.dll` | Serial 80-column |
| `pr_dr80.dll` | Parallel 80-column |
| `pr_lin80.dll` | Line printer 80-column |
| `pr_drpre.dll` | Pre-printed forms |
| `pr_dr40.dll` | Parallel 40-column |
| `pr_sr40.dll` | Serial 40-column |
| `pr_dr18.dll` | Parallel 18-column |
| `pr_sr28.dll` | Serial 28-column |
| `pr_dreme.dll` | Thermal REME |
| `pr_drhal.dll` | Thermal HAL |

---

## Configuration

The application is configured through two plain-text files in `bin/`:

### `st.ini`

Operator settings. Generated by the `SETUP` utility. Key sections:

| Section | Settings |
| --- | --- |
| `[Sistema]` | Country, currency, company name, tax rate (IVA), printer port, serial port |
| `[Aplicacion]` | Call floor times, rounding, receipt numbering, display options |
| `[Telefonia]` | Timing parameters — hook detection, DTMF, answer timeout, dial timeout |
| `[Marcacion]` | Access codes, digit counts per call type, access levels |
| `[Extensiones]` | Internal extension line settings |
| `[Valores Criticos]` | Edge-case detection flags |
| `[Modem]` | Modem COM port, baud rate, dial prefix, timeouts |
| `[Display]` | Booth display enable, COM port, baud rate, default message |

### `local.inf`

Local hometown numbering information — maps dialed prefixes to destination city names and call categories for local calls. The sample file covers Antioquia, Colombia (434 entries).

### `bin/ddn.inf`

National long-distance telephony information — numbering plan and tariff data for DDN (domestic) calls.

### `bin/ddi.inf`

International telephony information — numbering plan and tariff data for DDI (international) calls.

`local.inf`, `ddn.inf`, and `ddi.inf` are compiled together by `SETUP` into `bin/ph_info.dat`.

---

## Runtime Data Files

| File | Purpose |
| --- | --- |
| `bin/st.exe` | Main application |
| `bin/st.ini` | Operator configuration |
| `bin/ph_info.dat` | Compiled telephony database (from `ddi.inf` + `ddn.inf` + `local.inf`) |
| `bin/res.dat` | Zinc UI resources |
| `bin/help.dat` | In-application help database |
| `bin/RX.DAT` | Transaction records |
| `bin/RX.IDX` | Transaction index |
| `bin/RX.STA` | Statistics snapshot |
| `bin/LOG.DAT` | System event log |
| `bin/ddi.inf` | International telephony information (source) |
| `bin/ddn.inf` | National long-distance telephony information (source) |
| `bin/local.inf` | Local hometown telephony information (source) |

---

## Architecture

SmartTar is a single-process protected-mode application. The main subsystems are:

```text
┌─────────────────────────────────────────────────────┐
│  Zinc UI layer  (menus, dialogs, toolbar, views)    │
├──────────────┬──────────────────────────────────────┤
│  Controller  │  Real-time engine (rt_eng)           │
│  (control)   │  — per-booth call state machine      │
│              │  — ISR-driven timing (rt_isr)        │
├──────────────┼──────────────────────────────────────┤
│  Tariff      │  Database engine (db_eng / dstorage) │
│  engine      │  — RX.DAT / RX.IDX                   │
│  (ph_eng)    │  — receipt lifecycle                 │
├──────────────┴──────────────────────────────────────┤
│  Serial driver · Print spooler · Dongle · Modem     │
│  Config (cfg) · Logger (log) · Screen saver         │
└─────────────────────────────────────────────────────┘
          Pharlap 286 DOS Extender
          MS-DOS 5.0 / PC hardware
```

### Call lifecycle

1. **Off-hook detected** — serial line signals booth activity; `rt_eng` starts the call timer
2. **Digits captured** — DTMF digits accumulated; `parser` resolves prefix against the numbering plan
3. **Answer detected** — billing starts; tariff rate looked up from `ph_eng` based on call type, time of day, and day type
4. **On-hook detected** — elapsed time computed, final cost calculated (with ceiling/rounding), receipt written to `RX.DAT`
5. **Receipt printed** — spooler dispatches to the active printer driver DLL
6. **Statistics updated** — `dstatist` aggregates daily totals

---

## Build Variants

| Variant | Flags | Use |
| --- | --- | --- |
| Production | `RUN=1` | Full build with dongle check |
| Demo | `DEMO=1 NODONGLE=1 RUN=1` | Trade shows, evaluation |
| EDA | `EDA=1 RUN=1` | EDA operator — different call type classification |
| Debug | `DEBUG=1 RUN=1` | Development; use with Pharlap `TDP.EXE` debugger |
| Simulation | `AUTO=1 RUN=1` | Automated test / demo playback |

---

## Screenshots

| Version | Screenshot |
| --- | --- |
| 2.40 | ![SmartTar 2.40](st/docs/SmartTar%202.40.jpg) |
| 2.33 | ![SmartTar 2.33](st/docs/SmartTar%202.33.gif) |
| 2.32.1 | ![SmartTar 2.32.1](st/docs/SmartTar%202.32.1.gif) |

---

## License

Copyright © 1993–2003 MicroDiseño Ltda. All rights reserved.

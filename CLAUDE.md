# SmartTar — CLAUDE.md

## Project Overview

SmartTar is a DOS 5.0 protected-mode application. It is a point-of-sale / tariff management system built with:

- **Compiler**: Borland C++ 3.1 (`BCC`/`BCC286`)
- **UI Framework**: Zinc Interface Library 3.5
- **DOS Extender**: Pharlap 286 v3.0 (runs in 286 protected mode)
- **Assembler**: Turbo Assembler (`TASM`)
- **Linker**: `TLINK` + `BIND286` (Pharlap binder)
- **Build tool**: Borland `MAKE`

## Repository Layout

```text
BC/          Borland C++ 3.1 toolchain (compiler, debugger, TASM, TLIB, etc.)
PHARLAP/     Pharlap 286 DOS extender (BCC286, BIND286, CFIG286, RUN286, DLLs)
ZINC/        Zinc 3.5 UI framework (headers, libs, tools)
UTIL/        DOS utilities for development (NC, MOUSE, PKLITE, PKZIP, SWEEP, etc.)
st/          Application source
  source/    .cpp / .c source files and telephony .inf data files
  include/   Header files
  obj/       Compiled object files (intermediate, not committed)
  lib/       Static libraries
  bin/       Output binaries and runtime data files
  docs/      Screenshots and user/reference documentation (Word, images)
  test/      Development test utilities (chkrx, defpwd, gen, inf2dat, etc.)
  util/      Build and maintenance utilities
  web/       Web assets (default.html, SmartTar.gif, versions.txt)
  MAKEFILE   Main build file (Borland MAKE syntax)
  MV.BAT     DOS mv replacement (copy + erase)
  mkd.bat    Directory creation helper
  s.bat      Run st.exe from the st/ directory
  st.cfg     BCC286 compiler configuration (distinct from bin/st.cfg)
  st.def     Pharlap linker definition file
  st.prj     Borland IDE project file
  versions.txt  Version history
```

## Build System

Build is driven by `st/MAKEFILE` using Borland `MAKE`. All commands must be run from the `st/` directory inside a DOS environment (or DOSBox/emulator).

### Compiler driver

`BCC286` is the Pharlap-wrapped version of BCC that targets 286 protected mode. Configuration lives in `st.cfg`.

### Key MAKE targets / flags

| Flag | Effect |
| ---- | ------ |
| (default) | Release build → `bin/st.exe` |
| `DEBUG=1` | Includes debug symbols (`-v`), sets linker `/v` |
| `DEMO=1` | Defines `__DEMO__` |
| `NODONGLE=1` | Defines `__NO_DONGLE__` (requires `DEMO=1`) |
| `AUTO=1` | Defines `__AUTO__` (simulation mode) |
| `EDA=1` | Defines `__EDA__` |
| `RUN=1` | Runs `BIND286` + `CFIG286` after link (produces runnable protected-mode EXE) |
| `HELP=1` | Regenerates `bin/help.dat` from `source/help.txt` via `genhelp` |

Example debug build:

```sh
make DEBUG=1 RUN=1
```

### Linker libraries

```text
phapi  d16_zil  d16_gfx  bc_16gfx  emu286  emu  mathl  bcl286  ptclassl  pbidsl
```

### Printer DLLs

Printer drivers are built as Pharlap DLLs from `source/pr_*.c`:

```text
pr_sr80  pr_dr80  pr_lin80  pr_drpre  pr_dr40  pr_sr40  pr_dr18  pr_sr28  pr_dreme  pr_drhal
```

## Language & Style

- C++ with Borland extensions (far pointers, `BOOL`, `WORD`, `DWORD` typedefs)
- Header guards use `#ifndef` / `#define` / `#endif`
- Class names are `PascalCase`; file names are `snake_case` (all lowercase)
- Zinc widget classes derive from Zinc base types; event handling uses Zinc's message-passing model
- `NDEBUG` is defined in release builds (controls `assert` and custom trace macros)

## Key Source Modules

| File | Purpose |
| ---- | ------- |
| `source/st.cpp` | Application entry point |
| `source/control.cpp` | Main controller / event loop |
| `source/ph_eng.cpp` | Telephony/hardware engine |
| `source/ph_tar.cpp` | Tariff engine |
| `source/ph_place.cpp` | Telephony place lookup |
| `source/ph_query.cpp` | Telephony query handling |
| `source/ph_util.cpp` | Telephony utilities |
| `source/db_eng.cpp` | Database engine |
| `source/db_view.cpp` | Database view |
| `source/rt_eng.cpp` | Real-time engine |
| `source/rt_do.cpp` | Real-time action dispatcher |
| `source/rt_isr.cpp` | Real-time interrupt service |
| `source/rt_store.cpp` | Real-time data storage |
| `source/rt_util.cpp` | Real-time utilities |
| `source/serial.cpp` | Serial port driver |
| `source/res.cpp` | Resource management |
| `source/cfg.cpp` | Configuration |
| `source/dongle.cpp` | Hardware dongle (copy protection) |
| `source/spooler.cpp` | Print spooler |
| `source/parser.cpp` | Telephony `.inf` file parser |
| `source/log.cpp` | Event logging |
| `source/eeprom.cpp` | EEPROM access |
| `source/view.cpp` | Main view |
| `source/view_ev.cpp` | View event handlers |
| `source/view_hb.cpp` | View heartbeat handlers |
| `source/w_phone.cpp` | Phone widget |
| `source/w_sgroup.cpp` | Service-group widget |
| `source/w_statbr.cpp` | Status-bar widget |
| `source/w_table.cpp` | Table widget |
| `source/mb_*.cpp` | Menu-bar handlers (file, config, help, print…) |
| `source/tb_*.cpp` | Toolbar handlers |
| `source/ct_pr_*.cpp` | Call-type printer handlers |
| `source/pr_*.c` | Printer driver DLLs |

## Runtime Data Files (in `bin/`)

| File | Purpose |
| ---- | ------- |
| `st.cfg` | Application configuration |
| `st.ini` | Runtime settings |
| `res.dat` | Resources (Zinc UI data) |
| `help.dat` | Help database |
| `ph_info.dat` | Compiled telephony database (from `ddi.inf` + `ddn.inf` + `local.inf`) |
| `ddi.inf` | International telephony information (source) |
| `ddn.inf` | National long-distance telephony information (source) |
| `local.inf` | Local hometown telephony information (source) |
| `RX.DAT` / `RX.IDX` | Transaction database |
| `RX.STA` | Transaction statistics |
| `LOG.DAT` | Event log |

## Important Constraints

- **Target OS is DOS 5.0** — no Win32, POSIX, or modern APIs.
- **286 protected mode** via Pharlap — flat memory model is NOT available; use `PHAPI` for extended memory access.
- **Zinc 3.5 API** — do not use later Zinc idioms; the framework version is fixed.
- **Borland C++ 3.1** — C++98 is not fully supported; use Borland-era C++ only (no STL, no exceptions unless Borland-specific, no namespaces).
- File names must stay within **8.3 DOS naming** conventions.
- Object files go to `obj/`, binaries to `bin/` — never mix with sources.

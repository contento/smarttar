# SmartTar — CLAUDE.md

## Project Overview

SmartTar is a DOS 5.0 protected-mode point-of-sale / tariff management system. Current version: **2.34.1** (2003).

### Runtime environment

- **Physical machine**: 386 (or compatible)
- **OS**: DOS 5.0
- **Execution model**: Pharlap 286 extended DOS — 286 protected-mode *instruction set* and segmented memory model, running on the 386 hardware via the Pharlap DOS extender. Not flat 386 mode.
- **Build / dev environment**: DOSBox-X (Mac and Windows)

### Toolchain

- **Compiler**: Borland C++ 3.1 (`BCC`/`BCC286`)
- **UI Framework**: Zinc Interface Library 3.5
- **DOS Extender**: Pharlap 286 v3.0 (`gorun286.exe` stub, `BIND286`, `CFIG286`)
- **Assembler**: Turbo Assembler (`TASM`)
- **Linker**: `TLINK` + `BIND286` (Pharlap binder)
- **Build tool**: Borland `MAKE`

## Project Goal

Maintain the codebase, fix Zinc 3.5 UI bugs, and make the application maximally stable under Extended DOS. No new features — stability only.

## Working Style

Work in small, focused steps with explicit checkpoints. Gonzalo (the original author, "GCC") knows this codebase deeply — confirm approach before making broad changes.

## Repository Layout

```text
bc/          Borland C++ 3.1 toolchain (compiler, debugger, TASM, TLIB, etc.)
pharlap/     Pharlap 286 DOS extender (BCC286, BIND286, CFIG286, RUN286, DLLs)
zinc/        Zinc 3.5 UI framework (headers, libs, tools)
util/        DOS utilities for development (NC, MOUSE, PKLITE, PKZIP, SWEEP, etc.)
st/          Application source
  src/    .cpp / .c source files, organized into subsystem subdirectories:
    ph/      Telephony engine, tariff, place lookup, query, utilities, parser
    rt/      Real-time engine, ISR, dispatcher, storage, utilities, serial
    db/      Database engine, view, storage, statistics, extended storage, file header
    pr/      Printer DLLs (pr_*.c) and plain_pr / prn_fmt helpers
    ui/      Zinc UI layer: view, view_ev, view_hb, widgets (w_phone, w_sgroup, w_statbr, w_table), bdisplay
    mb/      Menu-bar handlers (mb_conf, mb_ext, mb_file, mb_help, mb_inf, mb_print, mb_simul)
    tb/      Toolbar handlers (tb_conf, tb_man, tb_print, tb_sp, tb_tools, tb_util)
    ct/      Call-type printer handlers (ct_pr_nr, ct_pr_sr, ct_pr_st, ct_util)
    ctrl/    Controller (control, ctrl_ev, ctrl_pr, ctrl_rf)
    (root)   st.cpp, cfg.cpp, cstr.cpp, dongle.cpp, eeprom.cpp, info.cpp, log.cpp,
             mutex.cpp, res.cpp, sid.cpp, spooler.cpp, ssaver.cpp, st_util.cpp,
             stm2.cpp, traceinf.cpp, calc.cpp, calendar.cpp
  include/   Header files
  build/       Compiled object files (intermediate, not committed)
  lib/       Static libraries
  bin/       Output binaries and runtime data files
  docs/      Screenshots and user/reference documentation (Word, images)
  test/      Development test utilities (chkrx, defpwd, gen, inf2dat, etc.)
  util/      Build and maintenance utilities
  web/       Web assets (default.html, SmartTar.gif, versions.txt)
  MAKEFILE   Main build file (Borland MAKE syntax); uses .PATH.cpp to list all subdirs
  run.bat    Launch st.exe from bin/ (cd bin → st.exe → cd ..)
  makedemo.bat  Build demo version: make [args] -DDEMO -DRUN -DNODONGLE
  st.cfg     BCC286 compiler configuration (distinct from bin/st.cfg)
  st.def     Pharlap linker definition file
  st.prj     Borland IDE project file
  versions.txt  Version history
```

## Build System

Build is driven by `st/MAKEFILE` using Borland `MAKE`. All commands must be run from the `st/` directory inside a DOS environment (or DOSBox/emulator).

### Compiler driver

`BCC286` is the Pharlap-wrapped version of BCC that targets 286 protected mode. Configuration lives in `st.cfg`.

Key flags in `st.cfg`:

- `-ml` — large memory model (multiple code + data segments; all pointers default to `far`)
- `-k-` — stack checking **disabled** (do not re-enable; it causes instability)
- `-H=st.sym` — precompiled header via `stdst.h` / `#pragma hdrstop`
- `-D__FHEADER=3;__DEBUG=0;__BTN__` — always-on defines

### Linker / runtime binding

`st.def` declares `PROTMODE` / `EXETYPE OS2` (DPMI-compatible Pharlap format) with `STUB gorun286.exe`.  
After linking, `BIND286` embeds the Pharlap run-time stub; `CFIG286` tunes it:

- `-NISTACK 10` — 10 nested interrupt stacks
- `-LDTSIZE 4096` — 4096 LDT descriptor slots
- Stack size: 8 KB (`STACKSIZE 8192` in `st.def`)

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
| `HELP=1` | Regenerates `bin/help.dat` from `src/help.txt` via `genhelp` |

Example debug build:

```sh
make DEBUG=1 RUN=1
```

### Linker libraries

```text
phapi  d16_zil  d16_gfx  bc_16gfx  emu286  emu  mathl  bcl286  ptclassl  pbidsl
```

### Printer DLLs

Printer drivers are built as Pharlap DLLs from `src/pr_*.c`:

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
| `src/st.cpp` | Application entry point |
| `src/ctrl/control.cpp` | Main controller / event loop |
| `src/ctrl/ctrl_ev.cpp` | Controller event handling |
| `src/ctrl/ctrl_pr.cpp` | Controller print handling |
| `src/ctrl/ctrl_rf.cpp` | Controller refresh handling |
| `src/ph/ph_eng.cpp` | Telephony/hardware engine |
| `src/ph/ph_tar.cpp` | Tariff engine |
| `src/ph/ph_place.cpp` | Telephony place lookup |
| `src/ph/ph_query.cpp` | Telephony query handling |
| `src/ph/ph_util.cpp` | Telephony utilities |
| `src/ph/parser.cpp` | Telephony `.inf` file parser |
| `src/db/db_eng.cpp` | Database engine |
| `src/db/db_view.cpp` | Database view |
| `src/db/dstorage.cpp` | Database storage |
| `src/db/dstatist.cpp` | Database statistics |
| `src/db/d_ext_st.cpp` | Extended database storage |
| `src/db/filehdr.cpp` | File header management |
| `src/rt/rt_eng.cpp` | Real-time engine |
| `src/rt/rt_do.cpp` | Real-time action dispatcher |
| `src/rt/rt_isr.cpp` | Real-time interrupt service |
| `src/rt/rt_store.cpp` | Real-time data storage |
| `src/rt/rt_util.cpp` | Real-time utilities |
| `src/rt/serial.cpp` | Serial port driver |
| `src/ui/view.cpp` | Main view |
| `src/ui/view_ev.cpp` | View event handlers |
| `src/ui/view_hb.cpp` | View heartbeat handlers |
| `src/ui/w_phone.cpp` | Phone widget |
| `src/ui/w_sgroup.cpp` | Service-group widget |
| `src/ui/w_statbr.cpp` | Status-bar widget |
| `src/ui/w_table.cpp` | Table widget |
| `src/ui/bdisplay.cpp` | Booth display |
| `src/mb/mb_*.cpp` | Menu-bar handlers (file, config, help, print…) |
| `src/tb/tb_*.cpp` | Toolbar handlers |
| `src/ct/ct_pr_*.cpp` | Call-type printer handlers |
| `src/pr/pr_*.c` | Printer driver DLLs |
| `src/res.cpp` | Resource management (Zinc `res.dat`) |
| `src/cfg.cpp` | Configuration |
| `src/dongle.cpp` | Hardware dongle (copy protection) |
| `src/spooler.cpp` | Print spooler |
| `src/log.cpp` | Event logging |
| `src/eeprom.cpp` | EEPROM access |
| `src/stm2.cpp` | STM2 session/login management |
| `src/mutex.cpp` | Mutex primitive |
| `src/ssaver.cpp` | Screen saver |

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

- **Physical hardware: 386**, running DOS 5.0. The Pharlap extender uses the 286 protection model (segmented, not flat), but the CPU executing it is a 386.
- **286 protected mode via Pharlap** — flat memory model is NOT available; use `PHAPI` for extended memory access. Large model (`-ml`): all pointers are implicitly `far`.
- **Stack checking is OFF** (`-k-` in `st.cfg`) — do not add it back.
- **Zinc 3.5 API** — do not use later Zinc idioms; the framework version is fixed.
- **Borland C++ 3.1** — Borland-era C++ only: no STL, no standard exceptions, no namespaces, no `bool` (use `BOOL`/`TRUE`/`FALSE`).
- File names must stay within **8.3 DOS naming** conventions.
- Object files go to `build/`, binaries to `bin/` — never mix with sources.
- **Build environment**: DOSBox-X (Mac and Windows). All `make` commands run inside DOSBox-X, not on the host.
- Source files live under `src/` subsystem subdirectories (`ph/`, `rt/`, `db/`, `ui/`, etc.) — the MAKEFILE uses `.PATH.cpp` to find them; do not move files without updating `.PATH.cpp`.
